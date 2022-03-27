/* $APPASERVER_HOME/src_appaserver/output_choose_isa.c			*/
/* --------------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "environ.h"
#include "appaserver.h"
#include "session.h"
#include "frameset.h"
#include "menu.h"
#include "choose_isa.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	char *one_isa_folder_name;
	SESSION_FOLDER *session_folder;
	CHOOSE_ISA *choose_isa;

	if ( argc != 6 )
	{
		fprintf( stderr, 
	"Usage: %s session login_name role folder one_isa_folder_name\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name =
		environment_exit_application_name(
			argv[ 0 ] );

	login_name = argv[ 1 ];
	session_key = argv[ 2 ];
	folder_name = argv[ 3 ];

	if ( ! ( one_isa_folder_name =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			security_sql_injection_escape( argv[ 4 ] ) ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: security_sql_injection_escape(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			argv[ 4 ] );
		exit( 1 );
	}

	role_name = argv[ 5 ];

	session_folder =
		/* --------------------------------------------- */
		/* Sets appaserver environment and outputs argv. */
		/* Each parameter is security inspected.	 */
		/* Any error will exit(1).			 */
		/* --------------------------------------------- */
		session_folder_integrity_exit(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			APPASERVER_INSERT_STATE );

	choose_isa =
		choose_isa_new(
			session_folder->session->application_name,
			session_folder->session->login_name,
			session_folder->session->session_key,
			folder_name,
			one_isa_folder_name,
			session_folder->role_name,
			frameset_menu_horizontal(
				session_folder->
					session->
					application_name,
				session_folder->
					session->
					login_name )
				/* menu_horizontal_boolean */ );

	if ( !choose_isa || !choose_isa->document_form_html )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: choose_isa_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	printf( "%s\n", choose_isa->document_form_html );

	return 0;
}

