/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_prompt_process.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "boolean.h"
#include "document.h"
#include "appaserver_error.h"
#include "application.h"
#include "menu.h"
#include "frameset.h"
#include "security.h"
#include "post_prompt_process.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_or_set_name;
	boolean is_drillthru;
	POST_PROMPT_PROCESS *post_prompt_process;
	boolean omit_content_type = 0;

	if ( argc < 7 )
	{
		fprintf(stderr,
"Usage: %s application session login role process_or_set is_drillthru_yn [omit_content_type_yn]\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	process_or_set_name = argv[ 5 ];
	is_drillthru = ( *argv[ 6 ] == 'y' );

	if ( argc == 8 )
	{
		omit_content_type = ( *argv[ 7 ] == 'y' );
	}

	if ( !omit_content_type ) document_content_type_output();

	post_prompt_process =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_prompt_process_new(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			process_or_set_name,
			is_drillthru );

	if ( post_prompt_process->prompt_process_output_system_string )
	{
		security_system(
			post_prompt_process->
				prompt_process_output_system_string );
	}
	else
	if ( post_prompt_process->command_line )
	{

		if ( post_prompt_process->
			post_choose_process_no_parameters )
		{
			printf(	"%s\n",
				post_prompt_process->
					document_form_html );
			fflush( stdout );
		}

		appaserver_error_message_file(
			application_name,
			login_name,
			post_prompt_process->command_line /* message */ );

		security_system(
			post_prompt_process->command_line );
	}

	return 0;
}

