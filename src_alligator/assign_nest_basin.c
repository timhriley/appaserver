/* ---------------------------------------------------	*/
/* assign_nest_basin.c					*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "html_table.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "alligator.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void output_results(
				FILE *output_pipe,
				char *nest_number,
				char really_yn,
				int cell_number,
				char *input_basin,
				char *basin,
				char *input_in_transect_boundary_yn,
				char in_transect_boundary_yn,
				char *input_transect_number_string,
				char *transect_number_string );

int assign_nest_basin(		char *application_name,
				char really_yn );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char title[ 128 ];
	char really_yn;
	int number_reassigned;

	if ( argc != 4 )
	{
		fprintf( stderr, 
	"Usage: %s application process_name really_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	really_yn = *argv[ 3 ];

	appaserver_parameter_file = new_appaserver_parameter_file();

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

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
				document->application_name,
				document->title,
				document->output_content_type,
				appaserver_parameter_file->
					appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );
	
	document_output_body(
				document->application_name,
				document->onload_control_string );

	strcpy( title, process_name );
	format_initial_capital( title, title );
	printf( "<h2>%s</h2>\n", title );
	fflush( stdout );
	printf( "<h2>\n" );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) );
	fflush( stdout );
	printf( "</h2>\n" );
	fflush( stdout );

	number_reassigned =
		assign_nest_basin(	application_name,
					really_yn );

	printf( "<h3>Found %d mismatches.</h3>\n", number_reassigned );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

int assign_nest_basin(	char *application_name,
			char really_yn )
{
	char *transect_number_string;
	char *basin;
	char in_transect_boundary_yn;
	double distance = 0.0;
	int cell_number = 0;
	char sys_string[ 1024 ];
	char *select;
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char nest_number[ 16 ];
	char utm_easting[ 16 ];
	char utm_northing[ 16 ];
	char input_transect_number_string[ 16 ];
	char input_in_transect_boundary_yn[ 16 ];
	char input_basin[ 128 ];
	char *where;
	int count = 0;
	FILE *output_pipe = {0};

	if ( really_yn != 'y' )
	{
		char *heading;

		heading =
"nest_number,cell_number,existing_basin,new_basin,existing_transect,new_transect,existing_boundary_flag,new_boundary_flag";

		sprintf( sys_string,
			 "html_table.e	''				"
			 "		'%s'				"
			 "		'%c'				",
			 heading,
			 FOLDER_DATA_DELIMITER );
	}
	else
	{
		char *table_name;
		char *key;

		table_name = get_table_name( application_name, "nest" );
		key = "nest_number";

		sprintf( sys_string,
			 "update_statement.e table=%s key=%s carrot=y	|"
			 "sql.e						 ",
			 table_name,
			 key );
	}

	output_pipe = popen( sys_string, "w" );

	select =
"nest_number,utm_easting,utm_northing,transect_number,in_transect_boundary_yn,basin";

/*
	where = "basin is null";
*/
	where = "1 = 1";

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=%s		 "
		 "			folder=nest		 "
		 "			where=\"%s\"		|"
		 "cat						 ",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	if ( really_yn != 'y' ) printf( "<table border=1>\n" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	nest_number,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	utm_easting,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	utm_northing,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	input_transect_number_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		piece(	input_in_transect_boundary_yn,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );

		piece(	input_basin,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			5 );

		basin = alligator_get_basin_should_be(
						&cell_number,
						application_name,
						atof( utm_northing ),
						atof( utm_easting ) );

		transect_number_string =
			alligator_get_transect_number_string(
					application_name,
					atoi( utm_northing ) );

		in_transect_boundary_yn =
			alligator_get_in_transect_boundary_should_be_yn(
					&distance,
					application_name,
					(transect_number_string)
					? atoi( transect_number_string )
					: 0,
					atoi( utm_northing ),
					atoi( utm_easting ) );
	
		if ( ( basin && strcmp( basin, input_basin ) != 0 )
		||   ( in_transect_boundary_yn
			&& in_transect_boundary_yn != 
			   *input_in_transect_boundary_yn )
		||   ( transect_number_string
			&& atof( transect_number_string ) != 
			   atof( input_transect_number_string ) ) )
		{
			output_results(
				output_pipe,
				nest_number,
				really_yn,
				cell_number,
				input_basin,
				basin,
				input_in_transect_boundary_yn,
				in_transect_boundary_yn,
				input_transect_number_string,
				transect_number_string );

			count++;
		}
	
	} /* while( get_line() */

	pclose( input_pipe );

	if ( really_yn != 'y' ) printf( "</table>\n" );

	if ( output_pipe ) pclose( output_pipe );

	return count;

} /* assign_nest_basin() */

void output_results(
				FILE *output_pipe,
				char *nest_number,
				char really_yn,
				int cell_number,
				char *input_basin,
				char *basin,
				char *input_in_transect_boundary_yn,
				char in_transect_boundary_yn,
				char *input_transect_number_string,
				char *transect_number_string )
{
	if ( really_yn != 'y' )
	{
		fprintf( output_pipe,
			 "%s%c%d%c%s%c%s%c%s%c%.1lf%c%s%c%c\n",
			 nest_number,
			 FOLDER_DATA_DELIMITER,
			 cell_number,
			 FOLDER_DATA_DELIMITER,
			 input_basin,
			 FOLDER_DATA_DELIMITER,
			 basin,
			 FOLDER_DATA_DELIMITER,
			 input_transect_number_string,
			 FOLDER_DATA_DELIMITER,
			 atof( transect_number_string ),
			 FOLDER_DATA_DELIMITER,
			 input_in_transect_boundary_yn,
			 FOLDER_DATA_DELIMITER,
			 in_transect_boundary_yn );
	}
	else
	{
		fprintf(	output_pipe,
				"%s^basin^%s\n",
				nest_number,
				basin );

		fprintf(	output_pipe,
				"%s^transect_number^%.1lf\n",
				nest_number,
				atof( transect_number_string ) );

		fprintf(	output_pipe,
				"%s^in_transect_boundary_yn^%c\n",
				nest_number,
				in_transect_boundary_yn );

	}
} /* output_results() */

