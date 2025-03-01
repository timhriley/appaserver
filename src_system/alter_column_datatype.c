/* --------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_system/alter_column_datatype.c			*/
/* --------------------------------------------------------------- 	*/
/* No warranty and freely available software. Visit Appaserver.org	*/
/* --------------------------------------------------------------- 	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "boolean.h"
#include "String.h"
#include "appaserver.h"
#include "document.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "environ.h"
#include "security.h"
#include "shell_script.h"
#include "alter_datatype.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *folder_name;
	char *attribute_name;
	boolean execute;
	APPASERVER_PARAMETER *appaserver_parameter;
	ALTER_DATATYPE *alter_datatype;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf(stderr,
			"Usage: %s process folder attribute execute_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	folder_name = argv[ 2 ];
	attribute_name = argv[ 3 ];
	execute = ( *argv[ 4 ] == 'y' );

	appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name );

	if ( !*attribute_name
	||   strcmp( attribute_name, "attribute" ) == 0 )
	{
		printf( "<h3>Please select a Folder Attribute.</h3>\n" );

		/* ------------------------ */
		/* Both </body> and </html> */
		/* ------------------------ */
		document_close();
		exit( 0 );
	}

	alter_datatype =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		alter_datatype_new(
			application_name,
			/* ---------------------------- */
			/* Returns heap memory or datum */
			/* ---------------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				folder_name ),
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				attribute_name ),
			appaserver_parameter->data_directory );

	printf( "<p>%s\n", alter_datatype->execute_system_string );

	if ( execute )
	{
		char *results;

		fflush( stdout );
		results =
			string_pipe_fetch(
				alter_datatype->execute_system_string );

		if ( results ) printf( "<p>%s\n", results );

		process_execution_count_increment( process_name );
		printf( "<h3>Process complete</h3>\n" );
	}
	else
	{
		FILE *output_file;

		output_file =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			appaserver_output_file(
				alter_datatype->process_filespecification );

		fprintf(output_file,
			"%s\n",
			alter_datatype->shell_script );

		fclose( output_file );

		if ( system(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			shell_script_execute_bit_system_string(
				alter_datatype->
					process_filespecification ) ) ){}

		printf( "<h3>Created: %s</h3>\n",
			alter_datatype->process_filespecification );
	}

	document_close();

	return 0;
}

