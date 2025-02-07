/* --------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_system/view_csv_backup.c			*/
/* --------------------------------------------------------------- 	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------- 	*/

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
#define SYS_STRING_TEMPLATE	"cd %s/%s && find . -maxdepth 2 -print"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *login_name;
	char *document_root_subdirectory;
	char sys_string[ 256 ];
	DOCUMENT *document;
	LIST *file_list;
	char *file_to_process;
	char buffer[ 512 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf( stderr,
	"Usage: %s ignored process login_name document_root_subdirectory\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	login_name = argv[ 3 ];
	document_root_subdirectory = argv[ 4 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	sprintf( sys_string,
		 SYS_STRING_TEMPLATE,
		 appaserver_parameter_file->document_root,
		 document_root_subdirectory );

	file_list = pipe2list( sys_string );

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

	printf( "<h1>%s</h1>\n",
		format_initial_capital( buffer, process_name ) );

	printf( "<ul>\n" );

	do {
		file_to_process = list_get_string( file_list );

		if ( !timlib_ends_string( file_to_process, ".zip" ) )
			continue;

		printf(
"<li><a class=external_link target=new_frame href=\"%s:/%s/%s\">%s</a>\n",
			application_http_prefix( application_name ),
			document_root_subdirectory,
		 	file_to_process,
			file_to_process );

	} while( list_next( file_list ) );

	printf( "</ul>\n" );
	form_output_back_to_top_button();
	document_close();
	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
}

