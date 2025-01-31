/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_login.c	       			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "String.h"
#include "date.h"
#include "application.h"
#include "appaserver_error.h"
#include "application_create.h"
#include "document.h"
#include "environ.h"
#include "session.h"
#include "post_login.h"

int main( int argc, char **argv )
{
	POST_LOGIN *post_login;

	document_content_type_output();

	post_login =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_login_new(
			argc,
			argv );

	if ( post_login->post_login_document )
	{
		printf(	"%s\n",
			post_login->
				post_login_document->
				html );
	}

	if ( post_login->post_login_input->bot_generated->yes_boolean )
	{
		appaserver_error_message_file(
			APPLICATION_TEMPLATE_NAME,
			(char *)0 /* login_name */,
			post_login->post_login_input->bot_generated->message );

		sleep( BOT_GENERATED_SLEEP_SECONDS );
	}
	else
	if ( post_login->post_login_success )
	{
		environment_set(
			"DATABASE",
			post_login->post_login_input->application_name );

		session_insert(
			SESSION_TABLE,
			SESSION_INSERT_COLUMNS,
			post_login->post_login_success->session_key,
			post_login->post_login_input->login_name,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			session_login_date(),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			session_login_time(),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			session_http_user_agent(
				SESSION_USER_AGENT_WIDTH,
				environment_http_user_agent() ),
			post_login->
				post_login_input->
				bot_generated->
				remote_ip_address );

		if ( post_login->
			post_login_success->
			execute_system_string_frameset )
		{
			if ( system(
				post_login->
				post_login_success->
					execute_system_string_frameset ) ){}
		}

		if ( post_login->
			post_login_success->
			email_system_string )
		{
			appaserver_error_message_file(
				post_login->
					post_login_input->
					application_name,
				post_login->
					post_login_input->
					login_name,
				post_login->
					post_login_success->
					email_link_url );

			appaserver_error_message_file(
				post_login->
					post_login_input->
					application_name,
				post_login->
					post_login_input->
					login_name,
				post_login->
					post_login_success->
					email_system_string );

			if ( system(
				post_login->
					post_login_success->
					email_system_string ) ){}
		}
	}

	return 0;
}
