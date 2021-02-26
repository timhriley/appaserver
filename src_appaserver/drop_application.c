/* --------------------------------------------------- 	*/
/* drop_application.c				       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "document.h"
#include "attribute.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "environ.h"

/* Constants */
/* --------- */
#define SESSION_PROCESS_NAME	"drop_application"

/* Prototypes */
/* ---------- */
char *get_sys_string( char *table_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *table_name = {0};
	char *really_yn;
	char *sys_string;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf(stderr,
	"Usage: %s ignored ignored ignored ignored really_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	really_yn = argv[ 5 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	document = document_new( "", application_name );
	document_set_output_content_type( document );

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

	sys_string = get_sys_string( table_name );

	printf( "<BR><p>%s\n", sys_string );

	if ( toupper( *really_yn ) == 'Y' )
	{
		fflush( stdout );
		system( sys_string );
		printf( "<BR><h2>Process complete.</h2>\n" );
	}
	else
	{
		printf( "<BR><h2>Only a test.</h2>\n" );
	}

	document_close();
	exit( 0 );
} /* main() */


char *get_sys_string( char *table_name )
{
	char buffer[ 1024 ];

	sprintf( buffer,
		 "echo \"drop table %s;\"		|"
		 "sql.e 2>&1 				|"
		 "html_paragraph_wrapper",
		 table_name );

	return strdup( buffer );
} /* get_sys_string() */

