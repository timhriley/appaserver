/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_system/grant_select_to_user.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_error.h"
#include "document.h"
#include "environ.h"
#include "application.h"
#include "process.h"
#include "security.h"
#include "grant_select.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *login_name;
	char *connect_from_host;
	boolean revoke_only_boolean;
	boolean execute_boolean;
	GRANT_SELECT *grant_select;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s process login_name connect_from_host revoke_only_yn execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	login_name = argv[ 2 ];
	connect_from_host = argv[ 3 ];
	revoke_only_boolean = (*argv[ 4 ] == 'y');
	execute_boolean = (*argv[ 5 ] == 'y');

	grant_select =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		grant_select_new(
			application_name,
			/* ---------------------------- */
			/* Returns heap memory or datum */
			/* ---------------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				login_name ),
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				connect_from_host ),
			revoke_only_boolean );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name );

	if ( !execute_boolean )
	{
		if ( revoke_only_boolean )
		{
			grant_select_revoke_display(
				grant_select->revoke_sql_statement,
				grant_select->drop_user_sql_statement,
				grant_select->flush_privileges_sql_statement );
		}
		else
		{
			grant_select_display(
				grant_select->create_user_sql_statement,
				grant_select->on_table_sql_statement_list,
				grant_select->update_process_sql_statement,
				grant_select->flush_privileges_sql_statement );
		}
	}
	else
	{
		if ( revoke_only_boolean )
		{
			grant_select_revoke_execute(
				grant_select->revoke_sql_statement,
				grant_select->drop_user_sql_statement,
				grant_select->flush_privileges_sql_statement );
		}
		else
		{
			grant_select_execute(
				grant_select->create_user_sql_statement,
				grant_select->on_table_sql_statement_list,
				grant_select->update_process_sql_statement,
				grant_select->flush_privileges_sql_statement );
		}
	}

	if ( execute_boolean )
	{
		process_execution_count_increment( process_name );
	}

	document_close();

	return 0;
}

