/* ----------------------------------------------------	*/
/* $APPASERVER_HOME/src_benthic/load_field_sheet_scan.c	*/
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
#define DESTINATION_DIRECTORY	"/var/www/benthic/field_sheet_scans"
#define RELATIVE_DIRECTORY	"/benthic/field_sheet_scans"
#define ATTRIBUTE_NAME		"field_sheet_scan_filename"

/* Prototypes */
/* ---------- */
void update_sampling_point(	char *application_name,
				char *project,
				char *collection_name,
				char *location,
				int site_number,
				char *scan_filename );

void load_field_sheet_scan(
				char *application_name,
				char *project,
				char *collection_name,
				char *location,
				int site_number,
				char *source_filename_directory_session );

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name,
				char *project,
				char *collection_name,
				char *location,
				int site_number );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *project;
	char *collection_name;
	char *location;
	int site_number;
	char title[ 128 ];
	char sub_title[ 128 ];
	FILE *input_file;
	char *source_filename_directory_session;

	if ( argc != 9 )
	{
		fprintf( stderr, 
"Usage: %s application ignored process_name location project site_number collection scan_filename\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	/* session = argv[ 2 ]; */
	process_name = argv[ 3 ];
	location = argv[ 4 ];
	project = argv[ 5 ];
	site_number = atoi( argv[ 6 ] );
	collection_name = argv[ 7 ];
	source_filename_directory_session = argv[ 8 ];

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

	get_title_and_sub_title(
			title,
			sub_title,
			process_name,
			project,
			collection_name,
			location,
			site_number );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
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

	if ( !*collection_name
	||   strcmp( collection_name, "collection_name" ) == 0 )
	{
		printf( "<h3>ERROR: please choose a collection.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !*location || strcmp( location, "location" ) == 0 )
	{
		printf( "<h3>ERROR: please choose a site.</h3>\n" );
		document_close();
		exit( 0 );
	}

	load_field_sheet_scan(
				application_name,
				project,
				collection_name,
				location,
				site_number,
				source_filename_directory_session );

	printf( "<h3>Process complete.</h3>\n" );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name,
				char *project,
				char *collection_name,
				char *location,
				int site_number )
{
	format_initial_capital( title, process_name );

	sprintf(	sub_title,
			"%s/%s %s/%d",
			project,
			collection_name,
			location,
			site_number );

	format_initial_capital( sub_title, sub_title );

} /* get_title_and_sub_title() */

void load_field_sheet_scan(
				char *application_name,
				char *project,
				char *collection_name,
				char *location,
				int site_number,
				char *source_filename_directory_session )
{
	char *full_filename_directory;
	char *scan_filename;
	char tmp_buffer[ 256 ];
	char sys_string[ 1024 ];

	strcpy( tmp_buffer, source_filename_directory_session );

	full_filename_directory =
		right_trim_session(
			tmp_buffer );

	scan_filename = basename_get_filename( full_filename_directory );

	sprintf( sys_string,
		 "cp %s %s/%s",
		 source_filename_directory_session,
		 DESTINATION_DIRECTORY,
		 scan_filename );

	system( sys_string );

	sprintf( sys_string,
		 "chmod g+w %s/%s",
		 DESTINATION_DIRECTORY,
		 scan_filename );

	system( sys_string );

	update_sampling_point(	application_name,
				project,
				collection_name,
				location,
				site_number,
				scan_filename );

} /* load_field_sheet_scan() */

void update_sampling_point(	char *application_name,
				char *project,
				char *collection_name,
				char *location,
				int site_number,
				char *scan_filename )
{
	char sys_string[ 1024 ];
	char *table_name;
	char *key_field_list;

	table_name = get_table_name( application_name, "sampling_point" );
	key_field_list = "project,collection_name,location,site_number";

	sprintf( sys_string,
		 "echo \"%s^%s^%s^%d^%s^%s/%s\"		|"
		 "update_statement.e	table_name=%s	 "
		 "			key=%s		 "
		 "			carrot=y	|"
		 "sql.e					 ",
		 project,
		 collection_name,
		 location,
		 site_number,
		 ATTRIBUTE_NAME,
		 RELATIVE_DIRECTORY,
		 scan_filename,
		 table_name,
		 key_field_list );

	system( sys_string );

} /* update_sampling_point() */

