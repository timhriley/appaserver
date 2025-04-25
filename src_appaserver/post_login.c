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

	appaserver_error_argv_file(
		argc,
		argv,
		post_login->
			post_login_input->
			application_name,
		post_login->
			post_login_input->
			login_name );

	if ( !post_login->post_login_input->remote_ip_address )
	{
		appaserver_error_message_file(
			post_login->
				post_login_input->
				application_name,
			(char *)0 /* login_name */,
			"remote_ip_address not set" );

		exit( 1 );
	}

	if ( post_login->post_login_document )
	{
		printf(	"%s\n",
			post_login->
				post_login_document->
				html );
	}

	if ( post_login->password_fail_boolean )
	{
		sleep( POST_LOGIN_SLEEP_SECONDS );
	}

	if ( post_login->post_login_success )
	{
		environment_database_set(
			post_login->
				post_login_input->
				application_name );

		session_insert(
			post_login->
				post_login_success->
				session->
				insert_string,
			post_login->
				post_login_success->
				session->
				insert_system_string );

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
