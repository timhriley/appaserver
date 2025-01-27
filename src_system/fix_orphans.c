/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_system/fix_orphans.c       			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "String.h"
#include "document.h"
#include "application.h"
#include "environ.h"
#include "security.h"
#include "appaserver_error.h"
#include "orphan.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *table_name;
	boolean delete_boolean;
	boolean execute_boolean;
	boolean stdout_boolean = 0;
	ORPHAN *orphan;
	ORPHAN_FOLDER *orphan_folder;

	application_name = environ_exit_application_name( argv[ 0 ] );

/*
	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );
*/

	if ( argc < 5 )
	{
		fprintf(stderr,
"Usage: %s process_name table_name delete_yn execute_yn [stdout]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	table_name = argv[ 2 ];
	delete_boolean = ( *argv[ 3 ] == 'y' );
	execute_boolean = ( *argv[ 4 ] == 'y' );

	if ( argc == 6 )
	{
		stdout_boolean = ( strcmp( argv[ 5 ], "stdout" ) == 0 );
	}

	if ( !stdout_boolean )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			process_name );
	}

	orphan =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		orphan_new(
			application_name,
			/* ---------------------------- */
			/* Returns heap memory or datum */
			/* ---------------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				table_name ) /* folder_name */,
			delete_boolean );

	if ( orphan_folder_clean_boolean(
		orphan->orphan_folder_list ) )
	{
		orphan_clean_output(
			stdout_boolean,
			orphan->folder_name );
	}
	else
	if ( list_rewind( orphan->orphan_folder_list ) )
	do {
		orphan_folder = list_get( orphan->orphan_folder_list );
/*
orphan_subquery_string_stdout( orphan_folder->orphan_subquery_list );
continue;
*/
		if ( execute_boolean && !delete_boolean )
		{
			orphan_insert_execute(
				orphan_folder->orphan_insert_list );
		}
		else
		if ( execute_boolean && delete_boolean )
		{
			orphan_delete_execute(
				orphan_folder->orphan_delete_list );
		}
		else
		if ( !delete_boolean )
		{
			orphan_insert_list_display(
				stdout_boolean,
				orphan_folder->folder_name,
				orphan_folder->orphan_insert_list );
		}
		else
		{
			orphan_delete_list_display(
				stdout_boolean,
				orphan_folder->orphan_delete_list );
		}

	} while ( list_next( orphan->orphan_folder_list ) );

	if ( !stdout_boolean )
	{
		if ( execute_boolean ) printf( "<h3>Process complete.</h3>\n" );

		document_close();
	}

	return 0;
}

