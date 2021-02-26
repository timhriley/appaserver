/* ---------------------------------------------------	*/
/* merge_purge_parameter.c				*/
/* ---------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "boolean.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "list.h"
#include "environ.h"
#include "date.h"
#include "process.h"
#include "date_convert.h"
#include "application.h"
#include "load_sfwmd_file.h"
#include "water_quality.h"

/* Constants */
/* --------- */
#define PARAMETER_UNITS_DELIMITER		'^'
#define TABLE_COLUMNS  "station,collection_date,collection_time,concentration,MDL"

/* Structures */
/* ---------- */
typedef struct
{
	boolean success;
	char *message;
	int success_count;
} MERGE_PURGE_RESULTS;

/* Prototypes */
/* ---------- */
void do_purge(		char *application_name,
			char *purge_parameter,
			char *purge_units );

char *get_where_clause(	char *where,
			char *purge_parameter,
			char *purge_units );

void parse_keep_units_parameters(
			char *keep_parameter,
			char *keep_units,
			char *purge_parameter,
			char *purge_units,
			char *keep_parameter_units,
			char *purge_parameter_units );

MERGE_PURGE_RESULTS merge_purge_parameter(
			char *application_name,
			char *keep_parameter_units,
			char *purge_parameter_units,
			char really_yn );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char really_yn;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *keep_parameter_units;
	char *purge_parameter_units;
	char buffer[ 256 ];
	MERGE_PURGE_RESULTS merge_purge_results;

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application process_name keep_parameter_units purge_parameter_units really_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	keep_parameter_units = argv[ 3 ];
	purge_parameter_units = argv[ 4 ];
	really_yn = *argv[ 5 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	printf( "<h1>%s\n",
		format_initial_capital(	buffer,
					process_name ) );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) );
	printf( "</h1>\n" );
	fflush( stdout );

	document_close();

	merge_purge_results =
		merge_purge_parameter(
			application_name,
			keep_parameter_units,
			purge_parameter_units,
			really_yn );

	if ( really_yn == 'y' && merge_purge_results.success )
	{
		printf( "<p>Process complete with %d results.<p>%s\n",
			merge_purge_results.success_count,
			(merge_purge_results.message) ?
				merge_purge_results.message :
				"" );
	}
	else
	{
		printf( "<p>Process NOT complete.<p>%s\n",
			(merge_purge_results.message) ?
				merge_purge_results.message :
				"" );
	}

	if ( really_yn == 'y' )
	{
		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}

	exit( 0 );
} /* main() */

MERGE_PURGE_RESULTS merge_purge_parameter(
			char *application_name,
			char *keep_parameter_units,
			char *purge_parameter_units,
			char really_yn )
{
	char sys_string[ 1024 ];
	char keep_parameter[ 128 ];
	char keep_parameter_escaped[ 128 ];
	char keep_units[ 128 ];
	char keep_units_escaped[ 128 ];
	char purge_parameter[ 128 ];
	char purge_units[ 128 ];
	static MERGE_PURGE_RESULTS merge_purge_results = {0};
	char *select;
	char where[ 1024 ];
	FILE *input_pipe;
	FILE *output_pipe = {0};
	char input_buffer[ 1024 ];
	char station[ 128 ];
	char collection_date[ 128 ];
	char collection_time[ 128 ];
	char concentration[ 128 ];
	char minimum_detection_limit[ 128 ];
	FILE *results_update_pipe = {0};
	FILE *results_exception_update_pipe = {0};
	char *results_table_name;
	char *results_exception_table_name;

	results_table_name =
		get_table_name( application_name, "results" );

	results_exception_table_name =
		get_table_name( application_name, "results_exception" );

	merge_purge_results.success = 1;

	if ( !character_exists(	keep_parameter_units,
				PARAMETER_UNITS_DELIMITER ) )
	{
		static char message[ 128 ];

		merge_purge_results.success = 0;
		sprintf( message,
"The keep parameter of %s doesn't have the proper delimiter of (%c).",
			 keep_parameter_units,
			 PARAMETER_UNITS_DELIMITER );
		merge_purge_results.message = message;
	}

	if ( !character_exists(	purge_parameter_units,
				PARAMETER_UNITS_DELIMITER ) )
	{
		static char message[ 128 ];

		merge_purge_results.success = 0;
		sprintf( message,
"The purge parameter of %s doesn't have the proper delimiter of (%c).",
			 purge_parameter_units,
			 PARAMETER_UNITS_DELIMITER );
		merge_purge_results.message = message;
	}

	parse_keep_units_parameters(
			keep_parameter,
			keep_units,
			purge_parameter,
			purge_units,
			keep_parameter_units,
			purge_parameter_units );

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
			 "sql.e 2>&1 | html_paragraph_wrapper.e" );
		results_update_pipe = popen( sys_string, "w" );
		results_exception_update_pipe = popen( sys_string, "w" );
	}
	else
	{
		sprintf( sys_string,
"queue_top_bottom_lines.e 50 | html_table.e \"%s(%s) to %s(%s)\" %s '^'",
			 purge_parameter,
			 purge_units,
			 keep_parameter,
			 keep_units,
			 TABLE_COLUMNS );
		output_pipe = popen( sys_string, "w" );
	}


	select =
"station,collection_date,collection_time,concentration,minimum_detection_limit";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=results			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 get_where_clause(
				where,
				purge_parameter,
				purge_units ) );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	station,
			PARAMETER_UNITS_DELIMITER,
			input_buffer,
			0 );

		piece(	collection_date,
			PARAMETER_UNITS_DELIMITER,
			input_buffer,
			1 );

		piece(	collection_time,
			PARAMETER_UNITS_DELIMITER,
			input_buffer,
			2 );

		piece(	concentration,
			PARAMETER_UNITS_DELIMITER,
			input_buffer,
			3 );

		piece(	minimum_detection_limit,
			PARAMETER_UNITS_DELIMITER,
			input_buffer,
			4 );

		if ( output_pipe )
		{
			fprintf( output_pipe,
				 "%s^%s^%s^%s^%s\n",
				 station,
				 collection_date,
				 collection_time,
				 concentration,
				 minimum_detection_limit );
		}

		if ( results_update_pipe )
		{
			fprintf( results_update_pipe,
"update %s set parameter = '%s', units = '%s' where station = '%s' and collection_date = '%s' and collection_time = '%s' and %s;\n",
				 results_table_name,
				 escape_character(
					keep_parameter_escaped,
					keep_parameter,
					'\'' ),
				 escape_character(
					keep_units_escaped,
					keep_units,
					'\'' ),
				 station,
				 collection_date,
				 collection_time,
				 get_where_clause(
						where,
			  			purge_parameter,
			  			purge_units ) );

			merge_purge_results.success_count++;
		}

		if ( results_exception_update_pipe )
		{
			fprintf( results_exception_update_pipe,
"update %s set parameter = '%s', units = '%s' where station = '%s' and collection_date = '%s' and collection_time = '%s' and %s;\n",
				 results_exception_table_name,
				 escape_character(
					keep_parameter_escaped,
					keep_parameter,
					'\'' ),
				 escape_character(
					keep_units_escaped,
					keep_units,
					'\'' ),
				 station,
				 collection_date,
				 collection_time,
				 get_where_clause(
						where,
			  			purge_parameter,
			  			purge_units ) );
		}
	}

	pclose( input_pipe );
	if ( output_pipe ) pclose( output_pipe );
	if ( results_update_pipe ) pclose( results_update_pipe );

	if ( really_yn == 'y' )
	{
		static char message[ 128 ];

		do_purge(	application_name,
				purge_parameter,
				purge_units );

		sprintf( message,
"Note: repaint the top frame for %s(%s) to be removed from the drop-down.\n",
			purge_parameter,
			purge_units );
		merge_purge_results.message = message;
	}

	return merge_purge_results;

} /* merge_purge_parameter() */

void parse_keep_units_parameters(
			char *keep_parameter,
			char *keep_units,
			char *purge_parameter,
			char *purge_units,
			char *keep_parameter_units,
			char *purge_parameter_units )
{
	piece(	keep_parameter,
		PARAMETER_UNITS_DELIMITER,
		keep_parameter_units,
		0 );

	piece(	keep_units,
		PARAMETER_UNITS_DELIMITER,
		keep_parameter_units,
		1 );

	piece(	purge_parameter,
		PARAMETER_UNITS_DELIMITER,
		purge_parameter_units,
		0 );

	piece(	purge_units,
		PARAMETER_UNITS_DELIMITER,
		purge_parameter_units,
		1 );

} /* parse_keep_units_parameters() */

char *get_where_clause(	char *where,
			char *purge_parameter,
			char *purge_units )
{
	char purge_parameter_escaped[ 128 ];
	char purge_units_escaped[ 128 ];

	sprintf( where,
		 "parameter = '%s' and units = '%s'",
		 escape_character(
			purge_parameter_escaped,
			purge_parameter,
			'\'' ),
		 escape_character(
			purge_units_escaped,
			purge_units,
			'\'' ) );

	return where;
} /* get_where_clause() */

void do_purge(	char *application_name,
		char *purge_parameter,
		char *purge_units )
{
	char sys_string[ 1024 ];
	char *table_name;
	char where[ 1024 ];

	get_where_clause(	where,
				purge_parameter,
				purge_units );

	table_name = get_table_name( application_name, "results" );
	sprintf( sys_string,
		 "echo \"delete from %s where %s;\" | sql.e",
		 table_name,
		 where );
	system( sys_string );

	table_name = get_table_name( application_name, "results_exception" );
	sprintf( sys_string,
		 "echo \"delete from %s where %s;\" | sql.e",
		 table_name,
		 where );
	system( sys_string );

	table_name = get_table_name( application_name, "station_parameter" );
	sprintf( sys_string,
		 "echo \"delete from %s where %s;\" | sql.e",
		 table_name,
		 where );
	system( sys_string );

	table_name = get_table_name( application_name, "parameter_unit" );
	sprintf( sys_string,
		 "echo \"delete from %s where %s;\" | sql.e",
		 table_name,
		 where );
	system( sys_string );

} /* do_purge() */

