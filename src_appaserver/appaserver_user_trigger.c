/* ---------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/appaserver_user_trigger.c	*/
/* ---------------------------------------------------------	*/
/* 								*/
/* Freely available software: see Appaserver.org		*/
/* ---------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_error.h"
#include "appaserver_user.h"

/* Prototypes */
/* ---------- */
void appaserver_user_trigger_insert_update(
			APPASERVER_USER *appaserver_user );

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *state;
	APPASERVER_USER *appaserver_user;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s login_name state\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	state = argv[ 2 ];

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) == 0 )
	{
		if ( ! ( appaserver_user =
				appaserver_user_fetch(
					login_name ) ) )
		{
			printf(
		"<h3>Warning: appaserver_user_fetch() returned empty.</h3>\n" );

			exit( 0 );
		}

		appaserver_user_trigger_insert_update( appaserver_user );
	}

	return 0;
}

void appaserver_user_trigger_insert_update(
			APPASERVER_USER *appaserver_user )
{
	if ( !appaserver_user_password_encrypted(
		appaserver_user->database_password ) )
	{
		FILE *update_pipe;

		appaserver_user->database_password =
			/* -------------------- */
			/* Returns heap memory. */
			/* -------------------- */
			appaserver_user_encrypted_password_generate(
				application_name,
				appaserver_user->database_password,
				appaserver_user_mysql_version_password_function(
					appaserver_user_mysql_version() ) );

		update_pipe = appaserver_user_update_open();

		appaserver_user_update(
			update_pipe,
			appaserver_user->database_password,
			appaserver_user->login_name );

		pclose( update_pipe );
	}
}

