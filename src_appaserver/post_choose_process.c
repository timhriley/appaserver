/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_choose_process.c		*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "list.h"
#include "session.h"
#include "appaserver_error.h"
#include "post_choose_process.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_or_set_name;
	POST_CHOOSE_PROCESS *post_choose_process;

	if ( argc != 6 )
	{
		fprintf(stderr, 
		"Usage: %s application session login role process_or_set\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	process_or_set_name = argv[ 5 ];

	document_content_type_output();

	post_choose_process =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_choose_process_new(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			process_or_set_name );

	if ( !post_choose_process->post_prompt_process_system_string
	&&   !post_choose_process->prompt_process_output_system_string )
	{
		char message[ 128 ];

		sprintf(message,
			"post_choose_process_new(%s) returned incomplete.",
			process_or_set_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( post_choose_process->post_prompt_process_system_string )
	{
		if ( system(
			post_choose_process->
				post_prompt_process_system_string ) ){}
	}
	else
	if ( post_choose_process->prompt_process_output_system_string )
	{
		if ( system(
			post_choose_process->
				prompt_process_output_system_string ) ){}
	}

	return 0;
}
