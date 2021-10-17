/* ---------------------------------------------------	*/
/* src_appaserver/google_map_operation.c		*/
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
#include "appaserver_user.h"
#include "session.h"
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
boolean get_omit_output_content_type(
			DICTIONARY *dictionary );

char *add_dot_maybe(	char *coordinate );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *latitude;
	char *longitude;
	char *utm_easting;
	char *utm_northing;
	char *label;
	char *url_filename;
	char semaphore_filename[ 256 ];
	char window_name[ 128 ];
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	FILE *output_file;
	char *parent_process_id_string;
	char *operation_row_count_string;
	char sys_string[ 1024 ];
	char *results_string;
	char increment_string[ 16];
	char *session;
	boolean group_first_time;
	boolean group_last_time;
	GOOGLE_MAP *google_map;
	char *mode;
	char *dictionary_string;
	char *login_name;
	char *role_name;
	char *folder_name;
	DICTIONARY *dictionary;
	char *balloon_attribute_name_list_string = {0};
	APPASERVER_LINK *appaserver_link;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 14 )
	{
		fprintf( stderr,
"Usage: %s ignored process login_name role folder latitude longitude utm_easting utm_northing process_id operation_row_count session dictionary [balloon_attribute_list]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	if ( ( login_name = argv [ 3 ] ) ) {};
	role_name = argv[ 4 ];
	folder_name = argv[ 5 ];

	latitude = argv[ 6 ];
	latitude = add_dot_maybe( latitude );

	longitude = argv[ 7 ];
	longitude = add_dot_maybe( longitude );

	utm_easting = argv[ 8 ];
	utm_northing = argv[ 9 ];
	parent_process_id_string = argv[ 10 ];
	operation_row_count_string = argv[ 11 ];
	session = argv[ 12 ];
	dictionary_string = argv[ 13 ];

	if ( argc == 15 )
	{
		balloon_attribute_name_list_string = argv[ 14 ];
	}

	dictionary = dictionary_string2dictionary( dictionary_string );

	appaserver_parameter_file = appaserver_parameter_file_new();

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
	results_string = pipe2string( sys_string );

	sprintf( increment_string, "%d", atoi( results_string ) + 1 );

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
		if ( !get_omit_output_content_type(
				dictionary ) )
		{
			document_set_output_content_type( document );
		}

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

	if ( ( !atoi( latitude ) || !atoi( longitude ) )
	&&   ( !atoi( utm_easting ) || !atoi( utm_northing ) ) )
	{
		printf(
		"<p>Warning: missing coordinates.</p>\n" );
		if ( group_last_time ) document_close();
		exit( 0 );
	}

	appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		appaserver_parameter_file->
				document_root,
			process_name /* filename_stem */,
			application_name,
			0 /* process_id */,
			session,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"html" );

	url_filename = appaserver_link->output->filename;

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

	label = google_map_get_balloon(
				application_name,
				folder_name,
				role_name,
				dictionary,
				balloon_attribute_name_list_string );

	google_map = google_map_new();

	if ( atoi( latitude ) )
	{
		google_map_set_point(
				google_map->point_list,
				strdup( label ),
				google_map_convert_coordinate( latitude ),
				google_map_convert_coordinate( longitude ),
				0 /* utm_easting */,
				0 /* utm_northing */ );
	}
	else
	{
		google_map_set_point(
				google_map->point_list,
				strdup( label ),
				0.0 /* latitude */,
				0.0 /* longitude */,
				atoi( utm_easting ),
				atoi( utm_northing ) );
		google_map_convert_to_latitude_longitude(
			google_map->point_list );
	}

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
		char *prompt_filename;

		prompt_filename = appaserver_link->prompt->filename;

		printf( "<body bgcolor=\"%s\" onload=\"",
			application_background_color( application_name ) );

		sprintf(	window_name,
				"%s_%s",
				process_name,
				parent_process_id_string );

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
}

boolean get_omit_output_content_type(
			DICTIONARY *dictionary )
{
	char *results;

	results = dictionary_get_pointer(
			dictionary,
			CONTENT_TYPE_YN );

	return ( ( !results ) ? 0 : ( *results != 'y') );

}

char *add_dot_maybe( char *coordinate )
{
	char return_coordinate[ 128 ];
	char first_part[ 8 ];
	char second_part[ 16 ];

	if ( timlib_character_exists( coordinate, '.' ) ) return coordinate;

	if ( *coordinate == '-' )
	{
		timlib_strncpy( first_part, coordinate, 3 );
		timlib_strcpy( second_part, coordinate + 3, 16 );
	}
	else
	{
		timlib_strncpy( first_part, coordinate, 2 );
		timlib_strcpy( second_part, coordinate + 2, 16 );
	}

	sprintf( return_coordinate, "%s.%s", first_part, second_part );

	return strdup( return_coordinate );

}

