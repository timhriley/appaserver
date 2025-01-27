/* --------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_system/drop_column.c       			*/
/* --------------------------------------------------------------- 	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------- 	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "boolean.h"
#include "String.h"
#include "appaserver.h"
#include "document.h"
#include "process.h"
#include "appaserver_error.h"
#include "environ.h"
#include "security.h"
#include "drop_column.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_name;
	char *folder_name;
	char *attribute_name;
	boolean content_type_boolean;
	boolean execute;
	DROP_COLUMN *drop_column;

	if ( argc != 10 )
	{
		fprintf(stderr,
"Usage: %s application session login role folder attribute process content_type_yn execute_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	process_name = argv[ 5 ];
	folder_name = argv[ 6 ];
	attribute_name = argv[ 7 ];
	content_type_boolean = ( *argv[ 8 ] == 'y' );
	execute = ( *argv[ 9 ] == 'y' );

	application_name =
		/* ---------------------------- */
		/* Returns heap memory or datum */
		/* ---------------------------- */
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			application_name );

	if ( content_type_boolean )
	{
		session_environment_set( application_name );
		document_content_type_output();
	}

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name );

	if ( !*attribute_name
	||   strcmp( attribute_name, "attribute" ) == 0 )
	{
		printf( "<h3>Please select a folder attribute.</h3>\n" );

		/* ------------------------ */
		/* Both </body> and </html> */
		/* ------------------------ */
		document_close();
		exit( 0 );
	}

	drop_column =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drop_column_new(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			/* ---------------------------- */
			/* Returns heap memory or datum */
			/* ---------------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				folder_name ),
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				attribute_name ) );

	printf( "<p>%s\n", drop_column->execute_system_string );

	if ( execute )
	{
		char *results;

		fflush( stdout );
		results =
			string_pipe_fetch(
				drop_column->execute_system_string );

		if ( results ) printf( "<p>%s\n", results );

		process_increment_count( process_name );
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
				drop_column->process_filename );

		fprintf(output_file,
			"%s\n",
			drop_column->shell_script );

		fclose( output_file );

		if ( system(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			appaserver_execute_bit_system_string(
				drop_column->process_filename ) ) ){}

		printf( "<h3>Created: %s</h3>\n",
			drop_column->process_filename );
	}

	document_close();

	return 0;
}
