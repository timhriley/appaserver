/* ---------------------------------------------------	*/
/* documentation_list.c					*/
/* ---------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "dictionary.h"
#include "appaserver.h"
#include "folder.h"
#include "form.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "application.h"
#include "document.h"
#include "session.h"
#include "environ.h"
#include "basename.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char sys_string[ 512 ];
	DOCUMENT *document;
	LIST *file_list;
	char *file_to_process;
	char *process_name;
	char *login_name;
	char buffer[ 512 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *absolute_directory;
	char *apache_relative_directory;
	char base_name[ 128 ];
	char title[ 128 ];

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s ignored process login_name absolute_directory apache_relative_directory\n", argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	login_name = argv[ 3 ];
	absolute_directory = argv[ 4 ];
	apache_relative_directory = argv[ 5 ];
 
	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( chdir( absolute_directory ) == -1 )
	{
		fprintf( stderr, 
			 "Cannot change directory to %s\n",
			 absolute_directory );
		exit( 1 );
	}

	sprintf( sys_string,
"echo * | tr ' ' '\012' | grep -v '^\\*' | sort" );
	file_list = pipe2list( sys_string );

	document = document_new( "", application_name );

	if ( !*login_name
	||   !appaserver_frameset_menu_horizontal(
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
			"diagram_list.css",
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

	format_initial_capital( title, process_name );
	printf( "<h1>%s</h1>\n", title );
	printf( "<ul>\n" );

	do {
		file_to_process = list_get_string( file_list );

		strcpy( base_name,
			basename_get_base_name(
				file_to_process, 1 /* strip_extension */ ) );
		printf(
	"<li><a class=diagram target=\"%s\" href=\"%s:/%s/%s\">%s</a>\n",
			base_name,
			application_http_prefix( application_name ),
			apache_relative_directory,
		 	file_to_process,
			format_initial_capital( buffer, file_to_process ) );

	} while( list_next( file_list ) );

	printf( "</ul>\n" );

	form_output_back_to_top_button();

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

