/* $APPASERVER_HOME/library/post_choose_process.c	*/
/* ----------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------	*/

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
			int argc,
			char **argv,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name )
{
	POST_CHOOSE_PROCESS *post_choose_process;
	SESSION_PROCESS *session_process;

	session_process =
		/* --------------------------------------------- */
		/* Sets appaserver environment and outputs argv. */
		/* Each parameter is security inspected.	 */
		/* Any error will exit( 1 ).			 */
		/* --------------------------------------------- */
		session_process_integrity_exit(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			process_or_set_name );

	post_choose_process = post_choose_process_calloc();

	post_choose_process->prompt_process_has_preprompt =
		prompt_process_has_preprompt(
			process_or_set_name );

	post_choose_process->prompt_process_output_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		prompt_process_output_system_string(
			PROMPT_PROCESS_OUTPUT_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			process_or_set_name,
			(char *)0 /* dictionary_separate_send_string() */,
			post_choose_process->prompt_process_has_preprompt,
			post_choose_process->prompt_process_has_preprompt
				/* is_preprompt */ );

	return post_choose_process;
}

char *post_choose_process_href_string(
			char *post_choose_process_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name,
			char *frameset_prompt_frame )
{
	char href_string[ 1024 ];

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

	sprintf(href_string,
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

	return strdup( href_string );
}

