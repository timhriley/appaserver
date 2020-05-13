/* ---------------------------------------------------	*/
/* src_hydrology/view_maps_figs.c			*/
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
#include "folder.h"
#include "appaserver.h"
#include "form.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "process.h"
#include "environ.h"
#include "session.h"
#include "application.h"

/* Constants */
/* --------- */
#define PROCESS_NAME		"view_maps"
#define RELATIVE_DIRECTORY	"hydrology"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char current_directory[ 256 ];
	char sys_string[ 512 ];
	DOCUMENT *document;
	LIST *file_list;
	char *file_to_process;
	char buffer[ 512 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *session;
	char *login_name;
	char *role_name;
	char *database_string = {0};
				
	output_starting_argv_stderr( argc, argv );

	if ( argc != 5 )
	{
		fprintf( stderr,
"Usage: %s application session login_name role\n", argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_parameter_file = new_appaserver_parameter_file();

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
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	sprintf( current_directory, 
		 "%s/%s/maps_and_figs", 
		 appaserver_parameter_file->document_root,
		 RELATIVE_DIRECTORY );

	if ( chdir( current_directory ) == -1 )
	{
		fprintf( stderr, 
			 "Cannot change directory to %s\n",
			 current_directory );
		exit( 1 );
	}

	sprintf( sys_string, 
	"echo * | tr ' ' '\012' | grep -v '^\\*' | grep -v appaserver\\.err" );

	file_list = pipe2list( sys_string );

	if ( !list_rewind( file_list ) )
	{
		printf( "<p>ERROR: no documents found\n" );
		document_close();
		exit( 1 );
	}

	printf( "<h1>Maps and Figures</h1>\n" );
	printf( "<ul>\n" );

	do {
		file_to_process = list_get_string( file_list );

		printf(
"<li><a class=external_link target=new_frame href=\"/%s/maps_and_figs/%s\">%s</a>\n",
			RELATIVE_DIRECTORY,
		 	file_to_process,
			format_initial_capital( buffer, file_to_process ) );

	} while( list_next( file_list ) );

	printf( "</ul>\n" );
	document_close();
	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

