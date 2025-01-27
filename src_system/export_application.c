/* --------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_system/export_application.c	       		*/
/* --------------------------------------------------------------- 	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------- 	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "String.h"
#include "appaserver_error.h"
#include "document.h"
#include "appaserver_parameter.h"
#include "environ.h"
#include "export_application.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	boolean system_folders_boolean;
	char *output_medium;
	APPASERVER_PARAMETER *appaserver_parameter;
	EXPORT_APPLICATION *export_application;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf(stderr,
			"Usage: %s process system_folders_yn output_medium\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	system_folders_boolean = (*argv[ 2 ] == 'y');
	output_medium = argv[ 3 ];

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name );

	appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	export_application =
		export_application_new(
			application_name,
			system_folders_boolean,
			appaserver_parameter->data_directory );

	if ( !export_application
	||   !list_length( export_application->export_table_list ) )
	{
		printf( "<h3>Nothing to process.</h3>\n" );
	}

	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		export_application_spreadsheet_output(
			application_name,
			export_application->export_table_list,
			appaserver_parameter->data_directory );
	}
	else
	if ( strcmp( output_medium, "shell_script" ) == 0 )
	{
		export_application_shell_output(
			application_name,
			export_application->export_table_list,
			appaserver_parameter->data_directory );
	}
	else
	{
		export_application_html_output(
			export_application->
				export_table_list );
	}

	return 0;
}
