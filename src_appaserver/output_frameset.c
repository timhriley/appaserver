/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/output_frameset.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "appaserver_parameter.h"
#include "application.h"
#include "appaserver_user.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "environ.h"
#include "session.h"
#include "frameset.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	FRAMESET *frameset;

	application_name =
		environment_exit_application_name(
			argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s session login_name\n", 
			argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];

	frameset =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		frameset_new(
			application_name,
			session_key,
			login_name,
			application_menu_horizontal_boolean(
				application_name ) );

	if ( system( frameset->blank_table_frame_system_string ) ){}

	if ( frameset->execute_system_string_choose_role )
	{
		if ( system( frameset->execute_system_string_choose_role ) ){}
	}

	if ( frameset->blank_prompt_frame_system_string )
	{
		if ( system( frameset->blank_prompt_frame_system_string ) ){}
	}

	printf( "%s\n", frameset->html );

	return 0;
}

