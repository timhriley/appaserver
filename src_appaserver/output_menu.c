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
			folder_menu_fetch(
				application_name,
				session_key,
				appaserver_parameter_data_directory(),
				role_name ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_menu_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( menu =
			menu_fetch(
				application_name,
				login_name,
				session_key,
				role_name,
				FRAMESET_PROMPT_FRAME /* target_frame */,
				folder_menu->lookup_count_list ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: menu_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			login_name );
		exit( 1 );
	}

	if ( frameset_menu_horizontal )
	{
		printf( "%s\n",
			menu_horizontal_html(
				document_body_hide_preload_html(
					1 /* menu_boolean */ ),
				menu->menu_verb_list ) );
	}
	else
	{
		printf( "%s\n",
			menu_vertical_html(
				menu->menu_verb_list ) );
	}

	return 0;
}
