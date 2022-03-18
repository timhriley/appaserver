/* $APPASERVER_HOME/library/post_choose_process.c	*/
/* ----------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "boolean.h"
#include "String.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "appaserver.h"
#include "security.h"
#include "environ.h"
#include "dictionary_separate.h"
#include "post_choose_process.h"

POST_CHOOSE_PROCESS *post_choose_process_calloc( void )
{
	POST_CHOOSE_PROCESS *post_choose_process;

	if ( ! ( post_choose_process =
			calloc( 1, sizeof( POST_CHOOSE_PROCESS ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return post_choose_process;
}

POST_CHOOSE_PROCESS *post_choose_process_new(
			/* ------------------------------------ */
			/* See session_process_integrity_exit() */
			/* ------------------------------------ */
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name )
{
	POST_CHOOSE_PROCESS *post_choose_process =
		post_choose_process_calloc();

	post_choose_process->system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_choose_process_system_string(
			application_name,
			session_key,
			login_name,
			role_name,
			process_or_set_name );

	return post_choose_process;
}

char *post_choose_process_system_string(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name )
{
	return (char *)0;
}

char *post_choose_process_action_string(
			char *post_choose_process_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name,
			char *frameset_prompt_frame )
{
	char action_string[ 1024 ];

	if ( !post_choose_process_executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_or_set_name
	||   !frameset_prompt_frame )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(action_string,
		"href=\"%s/%s?%s+%s+%s+%s+%s\" target=%s",
			appaserver_library_http_prompt(
				appaserver_parameter_cgi_directory(),
				appaserver_library_server_address(),
				application_ssl_support_yn(
					application_name ),
				application_prepend_http_protocol_yn(
					application_name ) ),
		post_choose_process_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		process_or_set_name,
		frameset_prompt_frame );

	return strdup( action_string );
}

