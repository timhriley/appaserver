/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_appaserver/generic_load.c 	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "environ.h"
#include "generic_load.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_name;
	GENERIC_LOAD_CHOOSE *generic_load_choose;

	application_name = environ_exit_application_name( argv[ 0 ] );

	if ( argc != 5 )
	{
		fprintf(stderr,
			"Usage: %s session login role process\n",
			argv[ 0 ] );
		exit( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	process_name = argv[ 4 ];

	generic_load_choose =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		generic_load_choose_new(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			process_name );

	printf(	"%s\n",
		generic_load_choose->
			document_form_html );

	return 0;
}

