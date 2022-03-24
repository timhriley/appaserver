/* $APPASERVER_HOME/src_appaserver/output_menu.c			*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "environ.h"
#include "frameset.h"
#include "document.h"
#include "folder_menu.h"
#include "menu.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	boolean frameset_menu_horizontal;
	FOLDER_MENU *folder_menu;
	MENU *menu;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf(stderr,
		"Usage: %s session login_name role horizontal_menu_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	frameset_menu_horizontal = (*argv[ 4 ] == 'y');

	if ( ! ( folder_menu =
			folder_menu_new(
				application_name,
				session_key,
				role_name,
				appaserver_parameter_data_directory() ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_menu_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( menu =
			menu_new(
				application_name,
				session_key,
				login_name,
				role_name,
				frameset_menu_horizontal,
				folder_menu->count_list ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: menu_new(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			login_name );
		exit( 1 );
	}

	if ( !menu->html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: menu->html is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	printf( "%s\n", menu->html );

	return 0;
}
