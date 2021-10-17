/* ---------------------------------------------------	*/
/* src_appaserver/upload_source_file.c			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "basename.h"
#include "piece.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "application.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define DESTINATION_DIRECTORY_TEMPLATE	"%s/%s"

/* Prototypes */
/* ---------- */

void upload_source_file(	char *application_name,
				char *source_filename_directory_session,
				char *appaserver_mount_point );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char title[ 128 ];
	FILE *input_file;
	char *source_filename_directory_session;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf( stderr, 
			 "Usage: %s ignored process_name filename\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	source_filename_directory_session = argv[ 3 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

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
	
	document_output_body(	document->application_name,
				document->onload_control_string );

	format_initial_capital( title, process_name );

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>\n" );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ){};
	fflush( stdout );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !source_filename_directory_session
	||   !( input_file = fopen( source_filename_directory_session, "r" ) ) )
	{
		printf( "<h3>ERROR: please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}
	fclose( input_file );

	upload_source_file(	document->application_name,
				source_filename_directory_session,
				appaserver_parameter_file->
					appaserver_mount_point );

	printf( "<h3>Process complete.</h3>\n" );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );

	return 0;

} /* main() */

void upload_source_file(	char *application_name,
				char *source_filename_directory_session,
				char *appaserver_mount_point )
{
	char *full_filename_directory;
	char *filename;
	char tmp_buffer[ 256 ];
	char destination_directory[ 256 ];
	char sys_string[ 1024 ];

	strcpy( tmp_buffer, source_filename_directory_session );

	full_filename_directory = right_trim_session( tmp_buffer );

	filename = basename_get_filename( full_filename_directory );

	sprintf( destination_directory,
		 DESTINATION_DIRECTORY_TEMPLATE,
		 appaserver_mount_point,
		 application_first_relative_source_directory(
			application_name ) );

	sprintf( sys_string,
		 "cp %s %s/%s",
		 source_filename_directory_session,
		 destination_directory,
		 filename );

	if ( system( sys_string ) ){};

	sprintf( sys_string,
		 "chmod g+w %s/%s",
		 destination_directory,
		 filename );

	sprintf( sys_string,
		 "chmod -x %s/%s",
		 destination_directory,
		 filename );

	if ( system( sys_string ) ){};

} /* upload_source_file() */

