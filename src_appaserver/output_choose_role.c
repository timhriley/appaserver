/* $APPASERVER_HOME/src_appaserver/output_choose_role.c			*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

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
	boolean frameset_menu_horizontal;
	CHOOSE_ROLE *choose_role;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf(stderr,
			"Usage: %s session login_name horizontal_menu_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	frameset_menu_horizontal = (*argv[ 3 ] == 'y');

	choose_role =
		choose_role_new(
			application_name,
			login_name,
			session_key,
			frameset_menu_horizontal );

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

	printf( "%s\n", choose_role->document_form_html );

	return 0;
}
