/* ---------------------------------------------------	*/
/* src_appaserver/view_source.c				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "application.h"
#include "folder.h"
#include "appaserver.h"
#include "form.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "environ.h"

/* Constants */
/* --------- */
#define SYS_STRING	"ls -1 makefile *.[hc] *.sh *.js *.awk *.sql"
#define PROCESS_NAME	"view_source"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char current_directory[ 256 ];
	DOCUMENT *document;
	LIST *file_list;
	char *file_to_process;
	char buffer[ 512 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *login_name;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf( stderr,
		"Usage: %s ignored ignored login_name ignored\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 3 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	add_pwd_to_path();

	sprintf( current_directory, 
		 "%s/%s", 
		 appaserver_parameter_file->appaserver_mount_point,
		 application_first_relative_source_directory(
			application_name ) );

	if ( chdir( current_directory ) != 0 )
	{
		fprintf( stderr, 
			 "Cannot change directory to %s\n",
			 current_directory );
		exit( 1 );
	}

	file_list = pipe2list( SYS_STRING );

	document = document_new( "", application_name );

	if ( !appaserver_frameset_menu_horizontal(
					application_name,
					login_name ) )
	{
		document_set_output_content_type( document );
	}

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

	if ( !list_rewind( file_list ) )
	{
		printf( "<p>ERROR: no documents found\n" );
		document_close();
		exit( 1 );
	}

	printf( "<h1>%s Source</h1>\n",
		format_initial_capital( buffer, application_name ) );

	printf( "<ul>\n" );

	do {
		file_to_process = list_get_string( file_list );

		printf(
"<li><a class=external_link target=new_frame href=\"%s:/%s/%s/%s\">%s</a>\n",
			application_http_prefix( application_name ),
			"appaserver",
		 	application_first_relative_source_directory(
				application_name ),
		 	file_to_process,
			file_to_process );

	} while( list_next( file_list ) );

	printf( "</ul>\n" );

	form_output_back_to_top_button();

	document_close();

	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );

	exit( 0 );

} /* main() */

