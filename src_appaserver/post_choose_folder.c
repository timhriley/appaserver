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
#include "session.h"
#include "post_choose_folder.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	char *state;
	SESSION_FOLDER *session_folder;
	POST_CHOOSE_FOLDER *post_choose_folder;

	if ( argc != 7 )
	{
		fprintf( stderr, 
		"Usage: %s application session login_name role folder state\n",
		argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	folder_name = argv[ 5 ];
	state = argv[ 6 ];

	session_folder =
		/* ---------------------------------------------- */
		/* Sets appaserver environment and outputs usage. */
		/* Each parameter is security inspected.	  */
		/* ---------------------------------------------- */
		session_folder_integrity_exit(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			state );

	post_choose_folder =
		post_choose_folder_new(
			/* ----------------------------------- */
			/* See session_folder_integrity_exit() */
			/* ----------------------------------- */
			session_folder->session->application_name,
			session_folder->session->session_key,
			session_folder->session->login_name,
			role_name,
			folder_name,
			state );

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

	return ( system( post_choose_folder->system_string ) );
}

