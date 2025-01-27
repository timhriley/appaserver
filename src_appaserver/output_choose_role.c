/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/output_choose_role.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "appaserver_error.h"
#include "environ.h"
#include "frameset.h"
#include "choose_role.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	boolean application_menu_horizontal_boolean;
	CHOOSE_ROLE *choose_role;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf(stderr,
		"Usage: %s session login_name application_menu_horizontal_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	application_menu_horizontal_boolean = (*argv[ 3 ] == 'y');

	choose_role =
		choose_role_new(
			application_name,
			session_key,
			login_name,
			application_menu_horizontal_boolean );

	if ( !choose_role )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: choose_role_new(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			login_name );
		exit( 1 );
	}

	if ( choose_role->choose_role_horizontal )
	{
		printf( "%s\n",
			choose_role->
				choose_role_horizontal->
				document_form_html );
	}
	else
	if ( choose_role->choose_role_vertical )
	{
		printf( "%s\n",
			choose_role->
				choose_role_vertical->
				document_form_html );
	}

	return 0;
}
