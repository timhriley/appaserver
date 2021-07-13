/* --------------------------------------------------- 	*/
/* src_hydrology/output_measurements_rectangle.c     	*/
/* --------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "timlib.h"
#include "environ.h"
#include "element.h"
#include "document.h"
#include "application.h"
#include "google_map.h"
#include "process.h"
#include "date.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
/*
#define OUTPUT_FILE_TEMPLATE	"%s/%s/station_rectangle_lookup_%s.html"
#define HTTP_FILE_TEMPLATE	"/%s/station_rectangle_lookup_%s.html"
*/
/*
#define MAP_POSITION_TOP	25
#define MAP_POSITION_LEFT	50
*/
#define MAP_POSITION_TOP	0
#define MAP_POSITION_LEFT	0
#define SOUTHWEST_LATITUDE	"25.65"
#define SOUTHWEST_LONGITUDE	"-80.85"
#define NORTHEAST_LATITUDE	"25.75"
#define NORTHEAST_LONGITUDE	"-80.74"

/* Prototypes */
/* ---------- */
void output_map(		FILE *output_file,
				char *application_name,
				char *database_string,
				char *login_name,
				char *session,
				char *process_name,
				char *role_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *database_string = {0};
	char *login_name;
	char *session;
	char *process_name;
	char *role_name;
	char buffer[ 256 ];
	char *output_filename;
	char *http_filename;
	FILE *output_file;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DOCUMENT *document;
	APPASERVER_LINK_FILE *appaserver_link_file;

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s application login_name session process role\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];

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

	login_name = argv[ 2 ];
	session = argv[ 3 ];
	process_name = argv[ 4 ];
	role_name = argv[ 5 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	appaserver_link_file =
		appaserver_link_file_new(
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
			"html" );

	output_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	http_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		fprintf( stderr,
		 "ERROR in %s/%s()/%d: cannot open %s for write.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 output_filename );
		exit( 1 );
	}

	document = document_new(
			(char *)0 /* insert_update_key */,
			application_name );

	document_output_head(
			document->application_name,
			format_initial_capital(
				buffer,
				process_name ) /* title */,
			0 /* not output_content_type */,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			(char *)0 /* additional_onload_control_string */ );


	printf( "<h1>%s</h1>\n",
		format_initial_capital(
			buffer,
			process_name ) );

	printf( "<h2>\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

	output_map(	output_file,
			application_name,
			database_string,
			login_name,
			session,
			process_name,
			role_name );

	fclose( output_file );

	printf(
"<p><a href=%s target=\"%s\">Click here</a> to view the map.\n",
		http_filename,
		process_name );

	process_increment_execution_count(
			application_name,
			process_name,
			appaserver_parameter_file_get_dbms() );

	document_close();

	return 0;

} /* main() */

void output_map(		FILE *output_file,
				char *application_name,
				char *database_string,
				char *login_name,
				char *session,
				char *process_name,
				char *role_name )
{
	char *google_map_key_data;
	char buffer[ 128 ];

	google_map_key_data =
		google_map_get_map_key(
			application_name );

	google_map_output_heading(
				output_file,
				format_initial_capital(
					buffer,
					process_name ) /* title */,
				google_map_key_data,
				(char *)0 /* balloon_click_parameter */,
				(char *)0 /* balloon_click_function */,
				application_name,
				MAP_POSITION_TOP,
				MAP_POSITION_LEFT,
				GOOGLE_MAP_CENTER_LATITUDE,
				GOOGLE_MAP_CENTER_LONGITUDE,
				10 /* google_map_starting_zoom */,
				800 /* google_map_width */,
				GOOGLE_MAP_HEIGHT );

	google_map_output_rectangle(
				output_file,
				SOUTHWEST_LATITUDE,
				SOUTHWEST_LONGITUDE,
				NORTHEAST_LATITUDE,
				NORTHEAST_LONGITUDE );

	google_map_output_heading_close( output_file );

	google_map_output_body(
		output_file,
		1 /* with_table */,
		"rectangle_bounds_changed()" /* additional_javascript */ );

	google_map_output_rectangle_bounds_changed(
				output_file );

	fprintf( output_file,
"<script language=JavaScript1.2 src=/javascript/timlib.js></script>\n" );

	google_map_output_rectangle_copy_to_form(
				output_file );

	fprintf( output_file,
"<form enctype=\"multipart/form-data\" method=post name=\"prompt_process\" action=\"/cgi-bin/post_hydrology_measurements_rectangle?%s+%s+%s+%s\" target=\"%s\">\n",
		 application_name,
		 login_name,
		 session,
		 role_name,
		 PROMPT_FRAME );

	fprintf( output_file,
"<input type=hidden name=rectangle_southwest_latitude>\n"
"<input type=hidden name=rectangle_southwest_longitude>\n"
"<input type=hidden name=rectangle_northeast_latitude>\n"
"<input type=hidden name=rectangle_northeast_longitude>\n" );

	fprintf( output_file,
"<td valign=middle>\n" );

	fprintf( output_file,
"<input type=button value=Submit onClick=\"rectangle_copy_to_form(); document.forms[0].submit(); window.close();\">\n" );

	fprintf( output_file,
"</form>\n</table>\n" );

	fprintf( output_file,
"</body>\n</html>\n" );

} /* output_map() */

