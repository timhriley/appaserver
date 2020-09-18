/* ---------------------------------------------------	*/
/* src_sparrow/upload_field_sheet.c			*/
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
#define DESTINATION_DIRECTORY	"/var/www/sparrow/field_sheet"
#define RELATIVE_DIRECTORY	"/sparrow/field_sheet"
#define ATTRIBUTE_NAME		"field_sheet"
#define ATTRIBUTE_WIDTH		80

/* Prototypes */
/* ---------- */
void update_site_visit(		char *application_name,
				char *quad_sheet,
				int site_number,
				char *visit_date,
				char *field_sheet_filename );

boolean upload_field_sheet(
				char *application_name,
				LIST *quad_sheet_list,
				LIST *site_number_list,
				LIST *visit_date_list,
				char *source_filename_directory_session );

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	LIST *quad_sheet_list;
	LIST *site_number_list;
	LIST *visit_date_list;
	char title[ 128 ];
	char sub_title[ 128 ];
	FILE *input_file;
	char *source_filename_directory_session;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s ignored process_name quad_sheet site_number visit_date field_sheet_filename\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	quad_sheet_list = list_string2list( argv[ 3 ], ',' );
	site_number_list = list_string2list( argv[ 4 ],',' );
	visit_date_list = list_string2list( argv[ 5 ], ',' );
	source_filename_directory_session = argv[ 6 ];

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
				application_get_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );
	
	document_output_body(
				document->application_name,
				document->onload_control_string );

	get_title_and_sub_title(
			title,
			sub_title,
			process_name );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	if ( system( timlib_system_date_string() ) ){};
	fflush( stdout );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !( input_file = fopen( source_filename_directory_session, "r" ) ) )
	{
		printf( "<h3>ERROR: please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}
	fclose( input_file );

	if ( !list_length( quad_sheet_list )
	||   strcmp(	list_get_first_pointer( quad_sheet_list ),
			"quad_sheet" ) == 0 )
	{
		printf( "<h3>ERROR: please choose a site visit.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( (	list_length( quad_sheet_list ) !=
		list_length( site_number_list ) )
	||   (  list_length( quad_sheet_list ) !=
		list_length( visit_date_list ) ) )
	{
		printf( "<h3ERROR: commas are not allowed.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !upload_field_sheet(
				application_name,
				quad_sheet_list,
				site_number_list,
				visit_date_list,
				source_filename_directory_session ) )
	{
		printf( "<h3>Error: the filename is too long.</h3>\n" );
	}
	else
	{
		printf( "<h3>Process complete.</h3>\n" );
	}

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name )
{
	format_initial_capital( title, process_name );

	*sub_title = '\0';
}

boolean upload_field_sheet(
				char *application_name,
				LIST *quad_sheet_list,
				LIST *site_number_list,
				LIST *visit_date_list,
				char *source_filename_directory_session )
{
	char *full_filename_directory;
	char *field_sheet_filename;
	char tmp_buffer[ 256 ];
	char sys_string[ 1024 ];
	char *quad_sheet;
	int site_number;
	char *visit_date;

	strcpy( tmp_buffer, source_filename_directory_session );

	full_filename_directory = right_trim_session( tmp_buffer );

	field_sheet_filename = basename_get_filename( full_filename_directory );

	if ( strlen( field_sheet_filename )
	+    strlen( RELATIVE_DIRECTORY )
	+    1 >= ATTRIBUTE_WIDTH )
	{
		return 0;
	}

	sprintf( sys_string,
		 "cp %s %s/%s",
		 source_filename_directory_session,
		 DESTINATION_DIRECTORY,
		 field_sheet_filename );

	if ( system( sys_string ) ){};

	sprintf( sys_string,
		 "chmod g+w %s/%s",
		 DESTINATION_DIRECTORY,
		 field_sheet_filename );

	if ( system( sys_string ) ){};

	list_rewind( quad_sheet_list );
	list_rewind( site_number_list );
	list_rewind( visit_date_list );

	do {
		quad_sheet = list_get( quad_sheet_list );
		site_number = atoi( list_get( site_number_list ) );
		visit_date = list_get( visit_date_list );

		update_site_visit(	application_name,
					quad_sheet,
					site_number,
					visit_date,
					field_sheet_filename );

		list_next( site_number_list );
		list_next( visit_date_list );

	} while( list_next( quad_sheet_list ) );

	return 1;
}

void update_site_visit(		char *application_name,
				char *quad_sheet,
				int site_number,
				char *visit_date,
				char *field_sheet_filename )
{
	char sys_string[ 1024 ];
	char *table_name;
	char *key_field_list;

	table_name = get_table_name( application_name, "site_visit" );
	key_field_list = "quad_sheet,site_number,visit_date";

	sprintf( sys_string,
		 "echo \"%s^%d^%s^%s^%s/%s\"		|"
		 "update_statement.e	table_name=%s	 "
		 "			key=%s		 "
		 "			carrot=y	|"
		 "sql.e					 ",
		 quad_sheet,
		 site_number,
		 visit_date,
		 ATTRIBUTE_NAME,
		 RELATIVE_DIRECTORY,
		 field_sheet_filename,
		 table_name,
		 key_field_list );

	if ( system( sys_string ) ){};
}

