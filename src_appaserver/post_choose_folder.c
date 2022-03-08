/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_choose_folder.c			*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "query.h"
#include "relation.h"
#include "appaserver.h"
#include "session.h"
#include "security.h"
#include "appaserver_user.h"
#include "application.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "dictionary.h"
#include "post2dictionary.h"
#include "role.h"
#include "post_choose_folder.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *session_key;
	char *role_name;
	char *folder_name;
	char *state;
	SESSION *session;
	POST_CHOOSE_FOLDER *post_choose_folder;

	if ( argc != 7 )
	{
		fprintf( stderr, 
		"Usage: %s login_name application session folder role state\n",
		argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	application_name = argv[ 2 ];
	session_key = argv[ 3 ];
	folder_name = argv[ 4 ];
	role_name = argv[ 5 ];
	state = argv[ 6 ];

	session =
		/* ---------------------------------------------- */
		/* Sets appaserver environment and outputs usage. */
		/* Each parameter is security inspected.	  */
		/* ---------------------------------------------- */
		session_folder_integrity_exit(
			argc,
			argv,
			application_name,
			login_name,
			session_key,
			folder_name,
			role_name,
			state );

	post_choose_folder =
		post_choose_folder_new(
			/* ----------------------------------- */
			/* See session_folder_integrity_exit() */
			/* ----------------------------------- */
			session->sql_injection_escape_application_name,
			session->login_name,
			session->sql_injection_escape_session_key,
			role_name,
			folder_name,
			session->session_state_integrity );

	if ( !post_choose_folder )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: post_choose_folder_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !post_choose_folder->system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( system( post_choose_folder->system_string ) ){}

	return 0;
}

