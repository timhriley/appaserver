/* ----------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_appaserver/generic_load_folder.c 	*/
/* ----------------------------------------------------- 	*/
/* 						       		*/
/* Freely available software: see Appaserver.org		*/
/* ----------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "frameset.h"
#include "menu.h"
#include "generic_load.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *session_key;
	char *process_name;
	char *role_name;
	char *folder_name;
	boolean menu_horizontal;
	GENERIC_LOAD_FOLDER *generic_load_folder;

	application_name = environ_exit_application_name( argv[ 0 ] );

	if ( argc != 6 )
	{
		fprintf(stderr,
			"Usage: %s login session process role folder\n",
			argv[ 0 ] );
		exit( 1 );
	}

	login_name = argv[ 1 ];
	session_key = argv[ 2 ];
	process_name = argv[ 3 ];
	role_name = argv[ 4 ];
	folder_name = argv[ 5 ];

	generic_load_folder =
		generic_load_folder_new(
			application_name,
			login_name,
			session_key,
			process_name,
			role_name,
			folder_name,
			( menu_horizontal =
				frameset_menu_horizontal(
					application_name,
					login_name ) )
					/* menu_horizontal_boolean */ );

	if ( !generic_load_folder )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: generic_load_folder_new(%s/%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			login_name,
			folder_name );
		exit( 1 );
	}

	if ( !menu_horizontal )
	{
		document_output_content_type();
	}

	printf( "%s\n", generic_load_folder->html );

	return 0;
}

