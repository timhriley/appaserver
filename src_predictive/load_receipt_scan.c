/* ----------------------------------------------------	*/
/* $APPASERVER_HOME/src_benthic/load_receipt_scan.c	*/
/* ----------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "basename.h"
#include "piece.h"
#include "boolean.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "date.h"
#include "application.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define DESTINATION_DIRECTORY	"%s/%s/receipt_scans"
#define RELATIVE_DIRECTORY	"/%s/receipt_scans"

/* Prototypes */
/* ---------- */
boolean load_receipt_scan(		char *application_name,
				char *fund_name,
				char *source_filename_directory_session,
				char *document_root_directory );

void insert_receipt_upload(	char *application_name,
				char *fund_name,
				char *scan_filename );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *fund_name;
	char title[ 128 ];
	FILE *input_file;
	char *source_filename_directory_session;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 5 )
	{
		fprintf( stderr, 
		"Usage: %s ignored process_name fund scan_filename\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	fund_name = argv[ 3 ];
	source_filename_directory_session = argv[ 4 ];

/*
	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );
*/

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
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
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

	if ( !load_receipt_scan(application_name,
				fund_name,
				source_filename_directory_session,
				appaserver_parameter_file->document_root ) )
	{
		printf( "<h3>Error: this scan file already exists.</h3>\n" );
	}
	else
	{
		printf( "<h3>Process complete.</h3>\n" );
	}

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				(char *)0 /* retired */ );

	return 0;

} /* main() */

boolean load_receipt_scan(	char *application_name,
				char *fund_name,
				char *source_filename_directory_session,
				char *document_root_directory )
{
	char *full_filename_directory;
	char *scan_filename;
	char sys_string[ 1024 ];
	char destination_directory[ 128 ];
	char destination_full_filespecification[ 256 ];
	char buffer[ 256 ];

	timlib_strcpy( buffer, source_filename_directory_session, 256 );

	full_filename_directory =
		right_trim_session(
			buffer );

	scan_filename = basename_get_filename( full_filename_directory );

	sprintf( destination_directory,
		 DESTINATION_DIRECTORY,
		 document_root_directory,
		 application_name );

	sprintf( destination_full_filespecification,
		 "%s/%s",
		 destination_directory,
		 scan_filename );

	if ( timlib_file_exists( destination_full_filespecification ) )
	{
		return 0;
	}

	sprintf( sys_string,
		 "cp %s %s",
		 source_filename_directory_session,
		 destination_full_filespecification );

	system( sys_string );

	sprintf( sys_string,
		 "chmod g+w %s",
		 destination_full_filespecification );

	system( sys_string );

	insert_receipt_upload(	application_name,
				fund_name,
				scan_filename );

	return 1;

} /* load_receipt_scan() */

void insert_receipt_upload(	char *application_name,
				char *fund_name,
				char *scan_filename )
{
	char sys_string[ 1024 ];
	char *table_name;
	char *field_list;
	char *upload_date;
	char relative_directory[ 128 ];

	sprintf( relative_directory,
		 RELATIVE_DIRECTORY,
		 application_name );

	upload_date = date_get_now_yyyy_mm_dd( date_get_utc_offset() );
	table_name = get_table_name( application_name, "receipt_upload" );

	if ( fund_name
	&&   *fund_name
	&&   strcmp( fund_name, "fund" ) != 0 )
	{
		field_list = "receipt_upload_filename,upload_date,fund";

		sprintf( sys_string,
		 "echo \"%s/%s^%s^%s\"			|"
		 "insert_statement.e	table_name=%s	 "
		 "			field=%s	 "
		 "			delimiter='^'	|"
		 "sql.e					 ",
			relative_directory,
		 	scan_filename,
		 	upload_date,
			fund_name,
		 	table_name,
		 	field_list );
	}
	else
	{
		field_list = "receipt_upload_filename,upload_date";

		sprintf( sys_string,
		 "echo \"%s/%s^%s\"			|"
		 "insert_statement.e	table_name=%s	 "
		 "			field=%s	 "
		 "			delimiter='^'	|"
		 "sql.e					 ",
			relative_directory,
		 	scan_filename,
		 	upload_date,
		 	table_name,
		 	field_list );
	}

	system( sys_string );

} /* insert_receipt_upload() */

