/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_system/drop_table.c				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "boolean.h"
#include "String.h"
#include "document.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "security.h"
#include "environ.h"
#include "process.h"
#include "security.h"
#include "shell_script.h"
#include "drop_table.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_name;
	char *table_name;
	boolean content_type_boolean;
	boolean execute_boolean;
	DROP_TABLE *drop_table;

	if ( argc != 9 )
	{
		fprintf(stderr,
"Usage: %s application session login role process table_name content_type_yn execute_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	process_name = argv[ 5 ];
	table_name = argv[ 6 ];
	content_type_boolean = ( *argv[ 7 ] == 'y' );
	execute_boolean = ( *argv[ 8 ] == 'y' );

	application_name =
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

	if ( !*table_name
	||   strcmp( table_name, "table_name" ) == 0 )
	{
		printf( "<h3>Please enter in a table name.</h3>\n" );

		/* ------------------------ */
		/* Both </body> and </html> */
		/* ------------------------ */
		document_close();
		exit( 0 );
	}

	drop_table =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		drop_table_new(
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
				table_name ) );

	printf( "<p>%s\n", drop_table->execute_system_string );

	if ( execute_boolean )
	{
		char *results;

		fflush( stdout );
		results =
			string_pipe_fetch(
				drop_table->execute_system_string );

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
				drop_table->process_filespecification );

		fprintf(output_file,
			"%s\n",
			drop_table->shell_script );

		fclose( output_file );

		if ( system(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			shell_script_execute_bit_system_string(
				drop_table->process_filespecification ) ) ){}

		printf( "<h3>Created %s</h3>\n",
			drop_table->process_filespecification );
	}

	document_close();

	return 0;
}
