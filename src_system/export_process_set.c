/* --------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_system/export_process_set.c			*/
/* --------------------------------------------------------------- 	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------- 	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "boolean.h"
#include "String.h"
#include "document.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "environ.h"
#include "process.h"
#include "export_process_set.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *export_process_set_name;
	boolean exclude_roles_boolean;
	APPASERVER_PARAMETER *appaserver_parameter;
	EXPORT_PROCESS_SET *export_process_set;

	application_name = environment_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf(stderr,
		"Usage: %s process_name export_process_set exclude_roles_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	export_process_set_name = argv[ 2 ];
	exclude_roles_boolean = ( *argv[ 3 ] == 'y' );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name /* process_name */ );

	appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	export_process_set =
		export_process_set_new(
			application_name,
			export_process_set_name,
			exclude_roles_boolean,
			appaserver_parameter->data_directory );

	if ( !export_process_set )
	{
		printf( "<h3>Please select a process set.</h3>\n" );

		/* ------------------------ */
		/* Both </body> and </html> */
		/* ------------------------ */
		document_close();
		exit( 0 );
	}

	if ( system( export_process_set->execute_bit_system_string ) ){}

	process_execution_count_increment( process_name );

	printf( "<h3>Created: %s</h3>\n",
		export_process_set->filename );

	document_close();

	return 0;
}

