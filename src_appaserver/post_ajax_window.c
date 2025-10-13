/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_ajax_window.c			*/
/* --------------------------------------------------------------------	*/
/* This process is triggered on the <submit> button of a		*/
/* forked ajax window.							*/
/*									*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "session.h"
#include "security.h"
#include "document.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "ajax.h"

#define CHECK_SESSION 1

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *form_folder_name;
	char *ajax_folder_name;
	char *value;
	SESSION_FOLDER *session_folder;
	AJAX_SERVER *ajax_server;

	if ( argc != 8 )
	{
		fprintf(stderr, 
"Usage: %s application session login_name role form_folder ajax_folder value\n",
			argv[ 0 ] );

		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	form_folder_name = argv[ 5 ];
	ajax_folder_name = argv[ 6 ];
	value = argv[ 7 ];

	if ( CHECK_SESSION )
	{
		session_folder =
			/* --------------------------------------------- */
			/* Sets appaserver environment and outputs argv. */
			/* Each parameter is security inspected.	 */
			/* Any error will exit(1).			 */
			/* --------------------------------------------- */
			session_folder_integrity_exit(
				argc,
				argv,
				application_name,
				session_key,
				login_name,
				role_name,
				form_folder_name,
				APPASERVER_INSERT_STATE,
				(char *)0 /* state2 */ );

		application_name = session_folder->application_name;
		form_folder_name = session_folder->folder_name;
		role_name = session_folder->role_name;
	}

	document_content_type_output();

	ajax_server =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		ajax_server_new(
			application_name,
			form_folder_name,
			role_name,
			/* ---------------------------- */
			/* Returns heap memory or datum */
			/* ---------------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				ajax_folder_name ),
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				value ) );

	if ( !ajax_server->return_string )
	{
		printf( "\n" );
	}
	else
	{
		printf( "%s\n", ajax_server->return_string );
	}

	return 0;
}

