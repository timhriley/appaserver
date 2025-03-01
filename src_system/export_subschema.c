/* --------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_system/export_subschema.c  			*/
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
#include "post_dictionary.h"
#include "shell_script.h"
#include "export_subschema.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *folder_name;
	boolean exclude_role_boolean;
	APPASERVER_PARAMETER *appaserver_parameter;
	EXPORT_SUBSCHEMA *export_subschema;

	application_name = environment_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf(stderr,
		"Usage: %s process folder_name exclude_role_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	folder_name = argv[ 2 ];
	exclude_role_boolean = ( *argv[ 3 ] == 'y' );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name );

	appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	export_subschema =
		export_subschema_new(
			application_name,
			folder_name,
			exclude_role_boolean,
			appaserver_parameter->data_directory );

	if ( !export_subschema )
	{
		printf( "<h3>Please select a folder.</h3>\n" );

		/* ------------------------ */
		/* Both </body> and </html> */
		/* ------------------------ */
		document_close();
		exit( 0 );
	}

	if ( system(
		export_subschema->
			shell_script_execute_bit_system_string ) ){}

	process_increment_count( process_name );

	printf( "<h3>Created: %s</h3>\n", export_subschema->filespecification );

	document_close();

	return 0;
}

