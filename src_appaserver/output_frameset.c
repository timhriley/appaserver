/* ----------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/output_frameset.c	*/
/* ----------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "appaserver_parameter.h"
#include "application.h"
#include "appaserver_user.h"
#include "appaserver.h"
#include "appaserver_library.h"
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

	session_environment_set( application_name );

	frameset =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		frameset_new(
			application_name,
			session_key,
			login_name,
			frameset_menu_horizontal(
				application_name,
				login_name ) );

	if ( frameset->menu_output_system_string )
	{
		if ( system( frameset->menu_output_system_string ) ){}
	}

	if ( frameset->choose_role_output_system_string )
	{
		if ( system( frameset->choose_role_output_system_string ) ){}
	}

	if ( frameset->blank_edit_frame_system_string )
	{
		if ( system( frameset->blank_edit_frame_system_string ) ){}
	}

	if ( frameset->blank_prompt_frame_system_string )
	{
		if ( system( frameset->blank_prompt_frame_system_string ) ){}
	}

	printf( "%s\n", frameset->html );

	return 0;
}

