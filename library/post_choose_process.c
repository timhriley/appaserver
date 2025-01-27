/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/post_choose_process.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "String.h"
#include "application.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "appaserver.h"
#include "security.h"
#include "environ.h"
#include "dictionary_separate.h"
#include "process_parameter.h"
#include "post_prompt_process.h"
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

	if ( !argc
	||   !argv
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_or_set_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	post_choose_process = post_choose_process_calloc();

	post_choose_process->session_process =
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

	if ( post_choose_process->session_process->process_name )
	{
		post_choose_process->no_parameters =
			post_choose_process_no_parameters(
				post_choose_process->
					session_process->
					process_name );
	}

	if ( post_choose_process->no_parameters )
	{
		post_choose_process->post_prompt_process_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			post_prompt_process_system_string(
				POST_PROMPT_PROCESS_EXECUTABLE,
				application_name,
				session_key,
				login_name,
				role_name,
				post_choose_process->
					session_process->
					process_name,
				appaserver_error_filename(
					application_name ) );
	}
	else
	{
		post_choose_process->process_parameter_drillthru_boolean =
			process_parameter_drillthru_boolean(
				post_choose_process->
					session_process->
					process_name,
				post_choose_process->
					session_process->
					process_set_name );

		post_choose_process->prompt_process_output_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			prompt_process_output_system_string(
				PROMPT_PROCESS_OUTPUT_EXECUTABLE,
				session_key,
				login_name,
				role_name,
				process_or_set_name,
				(char *)0 /* dictionary_..._send_string() */,
				post_choose_process->
					process_parameter_drillthru_boolean,
				post_choose_process->
					process_parameter_drillthru_boolean
						/* is_drillthru */,
				appaserver_error_filename( application_name ) );
	}

	return post_choose_process;
}

boolean post_choose_process_no_parameters( char *process_name )
{
	char system_string[ 1024 ];
	int results;

	sprintf(system_string,
		"select.sh \"count(1)\" %s \"%s\"",
		PROCESS_PARAMETER_TABLE,
		process_parameter_where(
			process_name,
			0 /* not is_drillthru */ ) );

	results = atoi( string_pipe_fetch( system_string ) );

	if ( results )
		return 0;
	else
		return 1;
}

char *post_choose_process_action_string(
			char *post_choose_process_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name )
{
	static char action_string[ 256 ];

	if ( !post_choose_process_executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( *action_string ) return action_string;

	snprintf(
		action_string,
		sizeof ( action_string ),
		"%s/%s?%s+%s+%s+%s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		application_http_prompt(
			/* ------------------------------------------------ */
			/* Returns component of global_appaserver_parameter */
			/* ------------------------------------------------ */
			appaserver_parameter_cgi_directory(),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
			application_ssl_support_boolean( application_name ) ),
		post_choose_process_executable,
		application_name,
		session_key,
		login_name,
		role_name );

	return action_string;
}

char *post_choose_process_href_string(
			char *process_or_set_name,
			char *post_choose_process_action_string )
{
	static char href_string[ 256 ];

	if ( !process_or_set_name
	||   !post_choose_process_action_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(href_string,
		"href=\"%s+%s\"",
		post_choose_process_action_string,
		process_or_set_name );

	return href_string;
}

char *post_choose_process_anchor_tag(
			char *target_frame,
			char *post_choose_process_href_string )
{
	static char anchor_tag[ 256 ];

	if ( !target_frame
	||   !post_choose_process_href_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(anchor_tag,
		"<a %s target=%s>",
		post_choose_process_href_string,
		target_frame );

	return anchor_tag;
}

