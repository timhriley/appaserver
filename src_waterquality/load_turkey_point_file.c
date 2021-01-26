/* ----------------------------------------------------------	*/
/* $APPASERVER_HOME/src_waterquality/load_turkey_point_file.c	*/
/* ----------------------------------------------------------	*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ----------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
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
#include "basename.h"
#include "application_constants.h"
#include "load_turkey_point_file.h"
#include "water_quality.h"

#define QUEUE_TOP_BOTTOM_LINES	65536

void remove_error_file(		char *error_filename );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *project_name;
	boolean execute;
	char *input_filename;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	int load_count = 0;
	char buffer[ 128 ];
	WATER_QUALITY *water_quality;
	char heading_error_message[ 65536 ];
	APPLICATION_CONSTANTS *application_constants;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s ignored process_name project_name filename execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	/* application_name = argv[ 1 ]; */
	process_name = argv[ 2 ];
	project_name = argv[ 3 ];
	input_filename = argv[ 4 ];
	execute = (*argv[ 5 ] == 'y');

	appaserver_parameter_file = appaserver_parameter_file_new();

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	printf( "<h2>%s\n", format_initial_capital( buffer, process_name ) );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !*input_filename || strcmp( input_filename, "filename" ) == 0 )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	*heading_error_message = '\0';

	application_constants = application_constants_new();
	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	water_quality =
		water_quality_new(
			application_name,
			project_name );

	water_quality->parameter_unit_alias_list =
		water_get_parameter_unit_alias_list(
			water_quality->input.parameter_name_list,
			water_quality->input.unit_name_list,
			water_quality->input.parameter_alias_list,
			water_quality->input.unit_alias_list );

	/* Doesn't set RESULTS */
	/* ------------------- */
	water_quality->load_column_list =
		water_fetch_turkey_point_column_list(
				heading_error_message,
				input_filename,
				water_quality->parameter_unit_alias_list,
				application_constants->dictionary );

/*
fprintf( stderr,
	 "Load column_list:\n%s\n",
	 water_load_column_list_display(
		water_quality->load_column_list ) );
*/

	if ( execute )
	{
		delete_waterquality(	application_name,
					input_filename,
					water_quality );
	}

	load_count =
		load_concentration_file(
			application_name,
			input_filename,
			water_quality,
			execute,
			heading_error_message );

	if ( execute )
	{
		printf( "<p>Process complete with %d concentrations.\n",
			load_count );

		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}
	else
	{
		printf( "<p>Process did not load %d concentrations.\n",
			load_count );
	}

	document_close();

	return 0;

} /* main() */

int load_concentration_file(
			char *application_name,
			char *input_filename,
			WATER_QUALITY *water_quality,
			boolean execute,
			char *heading_error_message )
{
	char sys_string[ 1024 ];
	FILE *input_file;
	FILE *error_file;
	char input_string[ 4096 ];
	char *collection_table_name;
	char *results_table_name;
	char *results_exception_table_name;
	char station[ 128 ];
	char collection_date_international[ 128 ];
	char depth_meters[ 128 ];
	char longitude[ 128 ];
	char latitude[ 128 ];
	char collection_time_without_colon[ 128 ];
	char error_filename[ 128 ];
	FILE *table_output_pipe = {0};
	FILE *results_insert_pipe = {0};
	FILE *results_exception_insert_pipe = {0};
	FILE *collection_insert_pipe = {0};
	int load_count = 0;
	int line_number = 0;
	RESULTS *results;
	char *error_message;
	char basename_filename[ 128 ];
	EXCEPTION *exception;

	if ( !list_length( water_quality->load_column_list ) ) return 0;

	sprintf(error_filename,
		"/tmp/load_concentration_file_error_%d.txt", getpid() );

	if ( ! ( error_file = fopen( error_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		return 0;
	}

	if ( heading_error_message && *heading_error_message )
	{
		fprintf( error_file,
			 "Unrecognized heading: %s\n",
			 heading_error_message );
	}

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		printf( "<h2>ERROR: cannot open %s for read</h2>\n",
			input_filename );

		fclose( error_file );
		remove_error_file( error_filename );
		return 0;
	}

	trim_index(	basename_filename,
			basename_get_filename( input_filename ) );

	results_table_name =
		get_table_name(	application_name,
				"results" );

	results_exception_table_name =
		get_table_name(	application_name,
				"results_exception" );

	collection_table_name =
		get_table_name(	application_name,
				"collection" );

	if ( execute )
	{
		sprintf(
		 sys_string,
		 "count.e %d 'WQ Load count'				  |"
		 "insert_statement table=%s field=%s del='|' compress=n   |"
		 "sql.e 2>&1						  |"
		 "grep -vi duplicate					  |"
		 "html_paragraph_wrapper.e				   ",
		 STDERR_COUNT,
		 results_table_name,
		 INSERT_RESULTS );

		results_insert_pipe = popen( sys_string, "w" );

		sprintf(
		 sys_string,
		 "insert_statement table=%s field=%s del='|' compress=n   |"
		 "sql.e 2>&1						  |"
		 "grep -vi duplicate					  |"
		 "html_paragraph_wrapper.e				   ",
		 results_exception_table_name,
		 INSERT_RESULTS_EXCEPTION );

		results_exception_insert_pipe = popen( sys_string, "w" );

		sprintf(
		 sys_string,
		 "sort -u						  |"
		 "insert_statement table=%s field=%s del='|' compress=n   |"
		 "sql.e 2>&1						  |"
		 "grep -vi duplicate					  |"
		 "html_paragraph_wrapper.e			 	   ",
		 collection_table_name,
		 INSERT_COLLECTION );

		collection_insert_pipe = popen( sys_string, "w" );
	}
	else
	{
		sprintf( sys_string,
		"queue_top_bottom_lines.e %d				|"
		"html_table.e 'Insert into Water Quality Results' %s '|' ",
			 QUEUE_TOP_BOTTOM_LINES,
			 DISPLAY_RESULTS );

		table_output_pipe = popen( sys_string, "w" );
	}

	/* Skip the header */
	/* --------------- */
	if ( !get_line( input_string, input_file ) )
	{
		close_pipes(
			results_insert_pipe,
			results_exception_insert_pipe,
			collection_insert_pipe,
			table_output_pipe );
		return 0;
	}

	line_number++;

	while( timlib_get_line( input_string, input_file, 4096 ) )
	{
		line_number++;
		trim( input_string );
		if ( !*input_string ) continue;

		if ( !extract_static_attributes(
			&error_message,
			station,
			collection_date_international,
			collection_time_without_colon,
			depth_meters,
			longitude,
			latitude,
			input_string,
			water_quality->load_column_list,
			application_name ) )
		{
			if ( *error_message )
			{
				fprintf(error_file,
			"Warning in line %d: ignored = (%s) because %s.\n",
					line_number,
					input_string,
					error_message );
			}
			else
			{
				fprintf(error_file,
			"Warning in line %d: ignored = (%s)\n",
					line_number,
					input_string );
			}
			continue;
		}

		if ( !list_rewind( water_quality->load_column_list ) )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: emtpy load_column_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		do {

			if ( ! ( results = extract_results(
					input_string,
					water_quality->load_column_list,
					water_quality->
						input.
						exception_list ) ) )
			{
				continue;
			}

			if ( !results->parameter_unit )
			{
				fprintf( stderr,
				"ERROR in %s/%s()/%d: empty parameter_unit.\n",
				 	__FILE__,
				 	__FUNCTION__,
				 	__LINE__ );
				exit( 1 );
			}

			if ( timlib_strncmp(	results->concentration,
						"-9999" ) == 0 )
			{
				continue;
			}

			load_count++;

			if ( table_output_pipe )
			{
				fprintf(
					table_output_pipe,
				 	"%s|%s|%s|%s|%s|%s|%s\n",
				 	station,
				 	collection_date_international,
				 	collection_time_without_colon,
				 	results->parameter_unit->parameter_name,
				 	results->parameter_unit->units,
				 	results->concentration,
					water_exception_display(
						results->exception_list ) );
				continue;

			} /* if table */

			/* if execute */
			/* ---------- */
			fprintf(
				results_insert_pipe,
			 	"%s|%s|%s|%s|%s|%s\n",
			 	station,
			 	collection_date_international,
			 	collection_time_without_colon,
			 	results->parameter_unit->parameter_name,
			 	results->parameter_unit->units,
			 	results->concentration );

			fprintf(
				collection_insert_pipe,
			 	"%s|%s|%s|%s|%s\n",
			 	station,
			 	collection_date_international,
			 	collection_time_without_colon,
			 	depth_meters,
			 	basename_filename );

			if ( list_rewind( results->exception_list ) )
			do {
			     exception =
				list_get_pointer(
					results->
						exception_list );

			     fprintf(
				results_exception_insert_pipe,
			 	"%s|%s|%s|%s|%s|%s\n",
			 	station,
			 	collection_date_international,
			 	collection_time_without_colon,
			 	results->parameter_unit->parameter_name,
			 	results->parameter_unit->units,
			 	exception->exception );

			} while( list_next( results->exception_list ) );

		} while ( list_next( water_quality->load_column_list ) );

	} /* for each line */

	fclose( input_file );
	fclose( error_file );

	close_pipes(
		results_insert_pipe,
		results_exception_insert_pipe,
		collection_insert_pipe,
		table_output_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 300 | html_table.e 'Water Quality Errors' '' '|'",
			 error_filename );
		if ( system( sys_string ) ) {};
	}

	remove_error_file( error_filename );
	return load_count;

} /* load_concentration_file() */

void remove_error_file( char *error_filename )
{
	char sys_string[ 1024 ];

	sprintf( sys_string, "rm %s", error_filename );
	if ( system( sys_string ) ) {};
}

#define DELETE_FIELD_LIST	"station,collection_date,collection_time"

void delete_waterquality(	char *application_name,
				char *input_filename,
				WATER_QUALITY *water_quality )
{
	FILE *input_file;
	FILE *collection_delete_pipe;
	FILE *results_delete_pipe;
	FILE *results_exception_delete_pipe;
	char sys_string[ 1024 ];
	char input_string[ 4096 ];
	char *table_name;
	char station[ 128 ];
	char collection_date_international[ 128 ];
	char depth_meters[ 128 ];
	char longitude[ 128 ];
	char latitude[ 128 ];
	char collection_time_without_colon[ 128 ];

	/* Collection */
	/* ---------- */
	table_name =
		get_table_name(
			application_name, "collection" );

	sprintf( sys_string,
		 "sort -u						|"
		 "delete_statement.e t=%s f=%s d='|'			|"
		 "sql.e 2>&1						|"
		 "html_paragraph_wrapper.e				 ",
		 table_name,
		 DELETE_FIELD_LIST );

	collection_delete_pipe = popen( sys_string, "w" );

	/* Results */
	/* ------- */
	table_name =
		get_table_name(
			application_name, "results" );

	sprintf( sys_string,
		 "delete_statement.e t=%s f=%s d='|'			|"
		 "count.e %d 'WQ delete results count'			|"
		 "sql.e 2>&1						|"
		 "html_paragraph_wrapper.e				 ",
		 table_name,
		 DELETE_FIELD_LIST,
		 STDERR_COUNT );

	results_delete_pipe = popen( sys_string, "w" );

	/* Results Exception */
	/* ----------------- */
	table_name =
		get_table_name(
			application_name, "results_exception" );

	sprintf( sys_string,
		 "delete_statement.e t=%s f=%s d='|'			|"
		 "count.e %d 'WQ delete results exception count'	|"
		 "sql.e 2>&1						|"
		 "html_paragraph_wrapper.e				 ",
		 table_name,
		 DELETE_FIELD_LIST,
		 STDERR_COUNT );

	results_exception_delete_pipe = popen( sys_string, "w" );

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		pclose( collection_delete_pipe );
		pclose( results_delete_pipe );
		pclose( results_exception_delete_pipe );
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	/* Skip the header */
	/* --------------- */
	if ( !get_line( input_string, input_file ) )
	{
		pclose( collection_delete_pipe );
		pclose( results_delete_pipe );
		pclose( results_exception_delete_pipe );
		return;
	}

	while( get_line( input_string, input_file ) )
	{
		trim( input_string );
		if ( !*input_string ) continue;

		if ( !extract_static_attributes(
			(char **)0,
			station,
			collection_date_international,
			collection_time_without_colon,
			depth_meters,
			longitude,
			latitude,
			input_string,
			water_quality->load_column_list,
			application_name ) )
		{
			continue;
		}

		fprintf( collection_delete_pipe,
			 "%s|%s|%s\n",
			 station,
			 collection_date_international,
			 collection_time_without_colon );

		fprintf( results_delete_pipe,
			 "%s|%s|%s\n",
			 station,
			 collection_date_international,
			 collection_time_without_colon );

		fprintf( results_exception_delete_pipe,
			 "%s|%s|%s\n",
			 station,
			 collection_date_international,
			 collection_time_without_colon );
	}

	fclose( input_file );
	pclose( collection_delete_pipe );
	pclose( results_delete_pipe );
	pclose( results_exception_delete_pipe );

} /* delete_waterquality() */

void close_pipes(
		FILE *results_insert_pipe,
		FILE *results_exception_insert_pipe,
		FILE *collection_insert_pipe,
		FILE *table_output_pipe )
{
	if ( table_output_pipe )
	{
		pclose( table_output_pipe );
	}
	else
	{
		pclose( results_insert_pipe );
		pclose( results_exception_insert_pipe );
		pclose( collection_insert_pipe );
	}

} /* close_pipes() */

RESULTS *extract_results(
			char *input_string,
			LIST *load_column_list,
			LIST *exception_list )
{
	char concentration[ 128 ];
	char exception_string[ 128 ];
	WATER_LOAD_COLUMN *load_column;
	static RESULTS results;

	if ( list_past_end( load_column_list ) ) return (RESULTS *)0;

	if ( results.concentration ) free( results.concentration );

	memset( &results, 0, sizeof ( RESULTS ) );

	load_column = list_get( load_column_list );

	if ( load_column->parameter_unit )
	{
		/* Get the concentration */
		/* --------------------- */
		if ( !piece_quote_comma(
			concentration,
			input_string,
			load_column->column_piece ) )
		{
			return (RESULTS *)0;
		}

		if ( !*concentration ) return (RESULTS *)0;

		results.parameter_unit = load_column->parameter_unit;
		results.concentration = strdup( concentration );

		/* Get the exceptions */
		/* ------------------ */
		if ( !piece_quote_comma(
			exception_string,
			input_string,
			load_column->column_piece + 1 ) )
		{
			return (RESULTS *)0;
		}

		if ( *exception_string )
		{
			results.exception_list =
				water_get_results_exception_list(
					exception_string,
					exception_list );
		}

		return &results;
	}

	return (RESULTS *)0;

} /* extract_results() */

boolean extract_static_attributes(
			char **error_message,
			char *station_name,
			char *collection_date_international,
			char *collection_time_without_colon,
			char *depth_meters,
			char *longitude,
			char *latitude,
			char *input_string,
			LIST *load_column_list,
			char *application_name )
{
	char collection_date[ 128 ];
	char collection_time[ 128 ];
	int column_piece;
	int collection_time_piece;
	static int station_attribute_width = 0;
	static LIST *station_list = {0};
	STATION *station;

	if ( !station_list ) station_list = list_new();

	if ( !station_attribute_width )
	{
		if ( ! ( station_attribute_width =
				attribute_fetch_width(
					application_name,
					"station" /* attribute_name */ ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get station_attribute_width.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}
	}

	if ( error_message ) *error_message = "";

	/* Get station */
	/* ----------- */
	column_piece =
		water_get_station_collection_attribute_piece(
			"station",
			load_column_list );

	if ( column_piece == -1 )
	{
		if ( error_message )
			*error_message =
				"cannot identify station";
		return 0;
	}

	if ( !piece_quote_comma(
		station_name,
		input_string,
		column_piece )
	||   !*station_name )
	{
		if ( error_message )
			*error_message =
				"cannot piece station";
		return 0;
	}

	if ( ! ( station =
		/* ------------------------------------ */
		/* Also checks station->alias_name_list */
		/* ------------------------------------ */
		water_station_get_or_set(
			station_list,
			application_name,
			station_name ) ) )
	{
		if ( error_message )
			*error_message =
				"cannot identify station. STATION_ALIAS?";
		return 0;
	}

	if ( strlen( station->station_name ) > station_attribute_width )
	{
		if ( error_message )
		{
			static char buffer[ 256 ];

			sprintf( buffer,
		"station name is too long. Width is %d, max size is %d",
				 (int)strlen( station->station_name ),
			 	 station_attribute_width );

			*error_message = buffer;
		}
		return 0;
	}

	strcpy( station_name, station->station_name );

	/* ------------------------------------------------------------	*/
	/* Get collection_time_without_colon.				*/
	/* Note: execute this before getting the collection date	*/
	/* because the collection date may have the time appended.	*/
	/* ------------------------------------------------------------	*/
	collection_time_piece =
		water_get_station_collection_attribute_piece(
			WATER_QUALITY_COLLECTION_TIME_LABEL,
			load_column_list );

	if ( collection_time_piece == -1 )
	{
		strcpy( collection_time_without_colon, "null" );
	}
	else
	if ( !piece_quote_comma(
		collection_time,
		input_string,
		column_piece )
	||   !*collection_time )
	{
		strcpy( collection_time_without_colon, "null" );
	}
	else
	{
		timlib_strcpy(
			collection_time_without_colon,
			date_remove_colon_from_time(
				collection_time ),
			0 /* buffer_size */ );
	}

	/* Get collection_date */
	/* ------------------- */
	column_piece =
		water_get_station_collection_attribute_piece(
			WATER_QUALITY_COLLECTION_DATE_LABEL,
			load_column_list );

	if ( column_piece == -1 )
	{
		if ( error_message )
			*error_message =
				"cannot identify collection date";
		return 0;
	}

	if ( ! piece_quote_comma(
		collection_date,
		input_string,
		column_piece )
	||   !*collection_date )
	{
		if ( error_message )
			*error_message =
				"cannot identify collection date";
		return 0;
	}

	/* The collection date may have the time appended. */
	/* ----------------------------------------------- */
	if ( collection_time_piece == -1
	&&   timlib_character_exists( collection_date, ' ' ) )
	{
		column( collection_time, 1, collection_date );

		/* Assume midnight means unknown time. */
		/* ----------------------------------- */
		if ( strcmp( collection_time, "00:00" ) != 0 )
		{
			timlib_strcpy(
				collection_time_without_colon,
				date_remove_colon_from_time(
					collection_time ),
				0 /* buffer_size */ );
		}
	}

	*collection_date_international = '\0';

	if ( !date_convert_source_unknown(
			collection_date_international,
			international,
			collection_date ) )
	{
		if ( error_message )
		{
			char buffer[ 256 ];

			sprintf( buffer,
				 "cannot translate %s to international",
				 collection_date );

			*error_message = strdup( buffer );
		}
		return 0;
	}

	if ( !*collection_time_without_colon )
	{
		if ( error_message )
			*error_message =
				"cannot identify collection time";
		return 0;
	}

	/* Get depth_meters */
	/* ---------------- */
	column_piece =
		water_get_station_collection_attribute_piece(
			WATER_QUALITY_DEPTH_LABEL,
			load_column_list );

	if ( column_piece == -1 )
	{
		*depth_meters = '\0';
	}
	else
	{
		piece_quote_comma(
			depth_meters,
			input_string,
			column_piece );
	}

	/* Get longitude */
	/* ------------- */
	column_piece =
		water_get_station_collection_attribute_piece(
			WATER_QUALITY_LONGITUDE_LABEL,
			load_column_list );

	if ( column_piece == -1 )
	{
		*longitude = '\0';
	}
	else
	{
		piece_quote_comma(
			longitude,
			input_string,
			column_piece );
	}

	/* Get latitude */
	/* ------------ */
	column_piece =
		water_get_station_collection_attribute_piece(
			WATER_QUALITY_LATITUDE_LABEL,
			load_column_list );

	if ( column_piece == -1 )
	{
		*latitude = '\0';
	}
	else
	{
		piece_quote_comma(
			latitude,
			input_string,
			column_piece );
	}

	return 1;

} /* extract_static_attributes() */

