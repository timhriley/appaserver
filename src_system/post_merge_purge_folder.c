/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_system/post_merge_purge_folder.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "String.h"
#include "appaserver_error.h"
#include "document.h"
#include "session.h"
#include "security.h"
#include "merge_purge.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_name;
	char *folder_name;
	MERGE_PURGE_PROCESS *merge_purge_process;

	if ( argc != 7 )
	{
		fprintf(stderr, 
		"Usage: %s application session login role process folder\n",
		argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	process_name = argv[ 5 ];
	folder_name = argv[ 6 ];

	document_content_type_output();

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	session_environment_set(
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			application_name ) );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name );

	merge_purge_process =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		merge_purge_process_new(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			process_name,
			folder_name );

	if ( merge_purge_process->same_boolean )
	{
		printf( MERGE_PURGE_PROCESS_SAME_MESSAGE );
	}
	else
	if ( !merge_purge_process->merge_purge_delete )
	{
		printf( MERGE_PURGE_NO_DELETE_PERMISSION_TEMPLATE,
			folder_name );
	}
	else
	if ( !merge_purge_process->row_exists_boolean )
	{
		printf( MERGE_PURGE_ALREADY_OCCURRED_TEMPLATE,
			folder_name );
	}
	else
	if ( merge_purge_process->execute_boolean )
	{
		merge_purge_process_execute(
			application_name,
			merge_purge_process->merge_purge_update,
			merge_purge_process->merge_purge_delete );

		process_increment_count( process_name );
		printf( "<h3>Process complete</h3>\n" );
	}
	else
	{
		merge_purge_html_display(
			merge_purge_process->
				delete_delimited_string,
			merge_purge_process->
				merge_purge_delete->
				delete );
	}

	document_close();

	return 0;
}
