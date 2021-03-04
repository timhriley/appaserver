/* output_open_html_document.c						*/
/* ----------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "document.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "timlib.h"

int main( int argc, char **argv )
{
	char *application_name, *title;
	char *content_type_yn;
	DOCUMENT *document;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf( stderr, 
	"Usage: %s ignored ignored title content_type_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	title = argv[ 3 ];
	content_type_yn = argv[ 4 ];

	document = document_new( title, application_name );

	document_output_heading(
			document->application_name,
			document->title,
			(*content_type_yn == 'y' ),
			document->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(	document->application_name,
				document->onload_control_string );

	return 0;
} /* main() */

