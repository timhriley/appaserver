/* ---------------------------------------------------	*/
/* src_wadingbird/wadingbird_google_map_operation.c	*/
/* ---------------------------------------------------	*/
/* This is the operation process for the google map     */
/* button.						*/
/*							*/
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
#include "folder.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "date.h"
#include "document.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "google_map.h"
#include "application_constants.h"
#include "appaserver_link.h"

/* Constants */
/* --------- */
#define SEMAPHORE_TEMPLATE	"%s/%s_google_map_semaphore_%s.dat"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *initial_takeoff_date;
	char *utm_easting_string;
	char *utm_northing_string;
	char *common_name;
	char *left_right;
	char *bird_count;
	char label[ 128 ];
	char *url_filename;
	char *prompt_filename;
	char semaphore_filename[ 256 ];
	char window_name[ 128 ];
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	FILE *output_file;
	char *parent_process_id_string;
	char *operation_row_count_string;
	char sys_string[ 1024 ];
	char *results;
	char increment_string[ 16];
	char *session;
	boolean group_first_time;
	boolean group_last_time;
	GOOGLE_MAP *google_map;
	char *mode;
	APPASERVER_LINK *google_map_appaserver_link;

	if ( argc != 12 )
	{
		fprintf( stderr,
"Usage: %s application process initial_takeoff_date utm_easting utm_northing common_name left_right bird_count process_id operation_row_count session\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	initial_takeoff_date = argv[ 3 ];
	utm_easting_string = argv[ 4 ];
	utm_northing_string = argv[ 5 ];
	common_name = argv[ 6 ];
	left_right = argv[ 7 ];
	bird_count = argv[ 8 ];
	parent_process_id_string = argv[ 9 ];
	operation_row_count_string = argv[ 10 ];
	session = argv[ 11 ];

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

	sprintf(	semaphore_filename,
			SEMAPHORE_TEMPLATE,
			appaserver_parameter_file->
				appaserver_data_directory,
			application_name,
			parent_process_id_string );

	if ( !timlib_file_exists( semaphore_filename ) )
	{
		char sys_string[ 1024 ];
		sprintf( sys_string, "echo 0 > %s", semaphore_filename );
		fflush( stdout );
		system( sys_string );
		fflush( stdout );
		group_first_time = 1;
	}
	else
	{
		group_first_time = 0;
	}

	sprintf( sys_string, "cat %s", semaphore_filename );
	results = pipe2string( sys_string );
	sprintf( increment_string, "%d", atoi( results ) + 1 );
	sprintf( sys_string,
		 "echo %s > %s",
		 increment_string,
		 semaphore_filename );
	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	group_last_time =
		( strcmp(	operation_row_count_string,
				increment_string ) == 0 );

	document = document_new( "", application_name );
	
	if ( group_first_time )
	{
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
	}
	
	if ( !atoi( utm_easting_string ) || !atoi( utm_northing_string ) )
	{
		printf(
	"<p>Warning: for species = %s, missing coordinates.</p>\n",
			common_name );
		if ( group_last_time ) document_close();
		exit( 0 );
	}

	google_map_appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		appaserver_parameter_file->document_root,
			GOOGLE_MAP_FILENAME_STEM,
			application_name,
			0 /* process_id */,
			session,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"html" );

	url_filename = appaserver_link->output->filename;

	prompt_filename = appaserver_link->prompt->filename;

	if ( group_first_time )
	{
		mode = "w";
	}
	else
	{
		mode = "a";
	}

	if ( ! ( output_file = fopen( url_filename, mode ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot open %s for write.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			url_filename );
		exit( 1 );
	}

	if ( group_first_time )
	{
		char *google_map_key_data;

		google_map_key_data =
			google_map_get_map_key(
				application_name );

		google_map_output_heading(
				output_file,
				(char *)0 /* title */,
				google_map_key_data,
				(char *)0 /* balloon_click_parameter */,
				(char *)0 /* balloon_click_function */,
				application_name,
				0 /* absolute_position_top */,
				0 /* absolute_position_left */,
				GOOGLE_MAP_CENTER_LATITUDE,
				GOOGLE_MAP_CENTER_LONGITUDE,
				GOOGLE_MAP_STARTING_ZOOM,
				GOOGLE_MAP_WIDTH,
				GOOGLE_MAP_HEIGHT );

	}

	sprintf(	label,
"SRF: %s<br>UTM Easting/Northing: %s / %s<br>Species: %s<br>Seat: %s<br>Count: %s",
			initial_takeoff_date,
			utm_easting_string,
			utm_northing_string,
			common_name,
			left_right,
			bird_count );

	google_map = google_map_new();

	google_map_set_point(
			google_map->point_list,
			strdup( label ),
			0.0 /* latitude_double */,
			0.0 /* longitude_double */,
			atoi( utm_easting_string ),
			atoi( utm_northing_string ) );

	google_map_convert_to_latitude_longitude(
					google_map->point_list );

	google_map_output_point_list( output_file, google_map->point_list );

	if ( group_last_time )
	{
		google_map_output_heading_close( output_file );

		google_map_output_body(
			output_file,
			0 /* not with_table */,
			(char *)0 /* additional_javascript */ );

		fprintf( output_file, "</body>\n" );
		fprintf( output_file, "</html>\n" );
	}

	fclose( output_file );

	if ( group_last_time )
	{
		printf( "<body bgcolor=\"%s\" onload=\"",
			application_background_color( application_name ) );

		sprintf( window_name, "%s", process_name );
		printf( "window.open('%s','%s');",
			prompt_filename,
			window_name );

		printf( "\">\n" );
		printf( "<h1>Google Map Viewer " );
		fflush( stdout );
		system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
		fflush( stdout );
		printf( "</h1>\n" );

		printf( "<a href=\"%s\" target=%s>Link to map.</a>\n",
			prompt_filename,
			window_name );

		document_close();
	}

	process_increment_execution_count(
			application_name,
			process_name,
			appaserver_parameter_file_get_dbms() );

	exit( 0 );

} /* main() */

