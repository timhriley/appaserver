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
	boolean delete_boolean;
	boolean execute_boolean;
	boolean stdout_boolean = 0;
	ORPHAN *orphan;
	ORPHAN_FOLDER *orphan_folder;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 4 )
	{
		fprintf(stderr,
"Usage: %s process_name delete_yn execute_yn [stdout]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	process_name = argv[ 1 ];
	delete_boolean = ( *argv[ 2 ] == 'y' );
	execute_boolean = ( *argv[ 3 ] == 'y' );

	if ( argc == 5 )
	{
		stdout_boolean = ( strcmp( argv[ 4 ], "stdout" ) == 0 );
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
			delete_boolean );

	if ( orphan_folder_clean_boolean(
		orphan->orphan_folder_list ) )
	{
		orphan_clean_output(
			stdout_boolean );
	}
	else
	if ( list_rewind( orphan->orphan_folder_list ) )
	do {
		orphan_folder = list_get( orphan->orphan_folder_list );

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

