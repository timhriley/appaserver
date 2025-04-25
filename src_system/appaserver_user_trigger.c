/* -------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_system/appaserver_user_trigger.c		*/
/* -------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "environ.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "preupdate_change.h"
#include "appaserver_user.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *state;
	char *preupdate_password;
	char *password;
	enum preupdate_change_state preupdate_change_state;

	application_name = environment_exit_application_name( argv[ 0 ] );

	if ( argc != 5 )
	{
		fprintf(stderr,
		"Usage: %s login_name state preupdate_password password\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	state = argv[ 2 ];
	preupdate_password = argv[ 3 ];
	password = argv[ 4 ];

	argv[ 4 ] = "secret";

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	preupdate_change_state =
		preupdate_change_state_evaluate(
			APPASERVER_INSERT_STATE,
			APPASERVER_PREDELETE_STATE,
			state,
			preupdate_password /* preupdate_datum */,
			password /* postupdate_datum */,
			"preupdate_password" /* preupdate_placeholder_name */ );

	if ( preupdate_change_state == no_change_null
	||   preupdate_change_state == no_change_something
	||   preupdate_change_state == from_something_to_null )
	{
		exit( 0 );
	}

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) == 0 )
	{
		appaserver_user_update(
			APPASERVER_USER_TABLE,
			password,
			login_name );
	}

	return 0;
}

