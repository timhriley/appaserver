/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_system/change_password.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "list.h"
#include "boolean.h"
#include "environ.h"
#include "change_password.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *session_key;
	char *role_name;
	char *process_name;
	CHANGE_PASSWORD *change_password;

	application_name = environ_exit_application_name( argv[ 0 ] );

	if ( argc != 5 )
	{
		fprintf(stderr,
			"Usage: %s session login_name role process\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	process_name = argv[ 4 ];

	change_password =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		change_password_new(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			process_name );

	if ( system( change_password->execute_system_string_table_edit ) ){}

	return 0;
}

