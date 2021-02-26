/* ---------------------------------------------------	*/
/* view_help.c						*/
/* ---------------------------------------------------	*/
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
#include "appaserver.h"
#include "form.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "application.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application;
	char current_directory[ 256 ];
	char sys_string[ 512 ];
	DOCUMENT *document;
	LIST *file_list;
	char *file_to_process;
	char buffer[ 512 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
				
	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s application\n", argv[ 0 ] );
		exit ( 1 );
	}

	application = argv[ 1 ];

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application );

	appaserver_parameter_file = new_appaserver_parameter_file();

	sprintf( current_directory, 
		 "%s/%s/help", 
		 appaserver_parameter_file->appaserver_mount_point,
		 application );

	if ( chdir( current_directory ) == -1 )
	{
		fprintf( stderr, 
			 "Cannot change directory to %s\n",
			 current_directory );
		exit( 1 );
	}

	sprintf( sys_string, 
		 "echo * | tr ' ' '\012' | grep -v '^\\*'" );
	file_list = pipe2list( sys_string );

	document = document_new( "", application );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application ),
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

	printf( "<h1>Help File List</h1>\n" );
	printf( "<ul>\n" );

	do {
		file_to_process = list_get_string( file_list );

		printf(
"<li><a class=external_link target=view_help href=\"%s:/%s/%s\">%s</a>\n",
			application_http_prefix( application ),
			application,
		 	file_to_process,
			format_initial_capital( buffer, file_to_process ) );

	} while( list_next( file_list ) );

	printf( "</ul>\n" );

	form_output_back_to_top_button();

	document_close();

	exit( 0 );
} /* main() */

