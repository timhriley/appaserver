/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/output_choose_isa.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "session.h"
#include "environ.h"
#include "frameset.h"
#include "appaserver.h"
#include "application.h"
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

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	folder_name = argv[ 4 ];

	one_isa_folder_name =
		/* ---------------------------- */
		/* Returns heap memory or datum */
		/* ---------------------------- */
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			argv[ 5 ] );

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
			APPASERVER_INSERT_STATE,
			(char *)0 /* state2 */ );

	choose_isa =
		choose_isa_new(
			session_folder->application_name,
			session_folder->session->session_key,
			session_folder->login_name,
			session_folder->role_name,
			session_folder->folder_name
				/* insert_folder_name */,
			one_isa_folder_name,
			application_menu_horizontal_boolean(
				session_folder->application_name ) );

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

