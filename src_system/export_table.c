/* --------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_system/export_table.c    			*/
/* --------------------------------------------------------------- 	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------- 	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "boolean.h"
#include "document.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "environ.h"
#include "process.h"
#include "export_table.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *folder_name;
	char *export_output;
	APPASERVER_PARAMETER *appaserver_parameter;
	EXPORT_TABLE *export_table;

	application_name = environment_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf(stderr,
		"Usage: %s process_name folder_name export_output\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	folder_name = argv[ 2 ];
	export_output = argv[ 3 ];

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name /* title_string */ );

	if ( !*folder_name
	||   strcmp( folder_name, "folder" ) == 0 )
	{
		printf( "<h3>Please select a folder.</h3>\n" );

		/* ------------------------ */
		/* Both </body> and </html> */
		/* ------------------------ */
		document_close();
		exit( 0 );
	}

	appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	export_table =
		export_table_new(
			application_name,
			folder_name,
			appaserver_parameter->data_directory,
			(char *)0 /* where */ );

	if ( !export_table )
	{
		char message[ 128 ];

		sprintf(message,
			"export_table_new(%s) returned empty.",
			folder_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strcmp( export_output, "shell_script" ) == 0 )
	{
		export_table_shell_output(
			application_name,
			export_table->application_table_name,
			export_table->appaserver_select,
			export_table->appaserver_system_string,
			export_table->shell_filespecification,
			export_table->execute_bit_system_string,
			1 /* output_prompt_boolean */ );
	}
	else
	if ( strcmp( export_output, "spreadsheet" ) == 0 )
	{
		export_table_spreadsheet_output(
			export_table->appaserver_system_string,
			export_table->spreadsheet_filespecification,
			export_table->
				appaserver_spreadsheet_heading_string,
			export_table->
				appaserver_spreadsheet_output_system_string,
			export_table->appaserver_link_anchor_html );
	}
	else
	{
		export_table_html_output(
			export_table->appaserver_system_string,
			export_table->html_table_system_string );
	}

	process_execution_count_increment( process_name );
	document_close();

	return 0;
}

