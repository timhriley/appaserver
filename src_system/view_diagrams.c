/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_system/view_diagrams.c				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "boolean.h"
#include "String.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "environ.h"
#include "view_source.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	APPASERVER_PARAMETER *appaserver_parameter;
	VIEW_SOURCE *view_source;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s process\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];

	appaserver_parameter = appaserver_parameter_new();

	view_source =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		view_source_new(
			application_name,
			process_name,
			appaserver_parameter->mount_point,
			view_source_diagrams /* view_source_enum */ );

	printf( "%s\n",
		view_source->document_form_html );

	return 0;
}

