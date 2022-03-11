/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_appaserver/post_login.c	       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* This program is attached to the submit button on the */
/* password form. 				       	*/
/* 							*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "folder.h"
#include "appaserver_library.h"
#include "appaserver_user.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "environ.h"
#include "basename.h"
#include "boolean.h"
#include "dictionary.h"
#include "document.h"
#include "application.h"
#include "security.h"
#include "session.h"
#include "post_login.h"

/* Prototypes */
/* ---------- */
int main( int argc, char **argv )
{
	POST_LOGIN *post_login;

	if ( ! ( post_login =
			post_login_new(
				argc,
				argv ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: post_login_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( post_login->missing_name )
	{
		char msg[ 1024 ];

		sprintf(msg,
			"Missing login_name from %s",
			environment_remote_ip_address() );

		appaserver_output_error_message(
			post_login->sql_injection_escape_application_name,
			msg,
			(char *)0 /* login_name */ );

		if ( !appaserver_library_from_php(
			post_login->dictionary ) )
		{
			document_output_content_type();

			document_quick_output(
				post_login->
					sql_injection_escape_application_name );

			printf( "<p>Please try again.\n%s\n",
				document_close_html() );
		}
		else
		{
			post_login_redraw_index_screen(
				post_login->
					sql_injection_escape_application_name,
				(char *)0 /* location */,
				"invalid_login_yn=y" );
		}

		sleep( POST_LOGIN_SECONDS_TO_SLEEP );
		exit ( 1 );
	}

	if ( post_login->missing_database_password
	||   post_login->password_match_return == password_fail )
	{
		char msg[ 1024 ];

		if ( post_login->missing_database_password )
		{
			sprintf(msg,
				"appaserver user %s is forbidden from %s",
				post_login->sql_injection_escape_login_name,
				environment_remote_ip_address() );
		}
		else
		{
			sprintf(msg,
				"Password missmatch for %s from %s",
				post_login->sql_injection_escape_login_name,
				environment_remote_ip_address() );
		}

		appaserver_output_error_message(
			post_login->sql_injection_escape_application_name,
			msg,
			post_login->sql_injection_escape_login_name );

		if ( !appaserver_library_from_php(
			post_login->dictionary ) )
		{
			document_output_content_type();

			document_quick_output(
				post_login->
					sql_injection_escape_application_name );

			printf( "<p>Please try again.\n%s\n",
				document_close_html() );
		}
		else
		{
			post_login_redraw_index_screen(
				post_login->
					sql_injection_escape_application_name,
				(char *)0 /* location */,
				"invalid_login_yn=y" );
		}

		sleep( POST_LOGIN_SECONDS_TO_SLEEP );
		exit ( 1 );
	}

	session_environment_set(
		post_login->sql_injection_escape_application_name );

	appaserver_error_login_name_append_file(
		argc,
		argv,
		post_login->sql_injection_escape_application_name,
		post_login->sql_injection_escape_login_name );

	if ( !post_login->session_key )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: session_key is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	post_login_frameset_output(
		post_login->sql_injection_escape_application_name,
		post_login->sql_injection_escape_login_name,
		post_login->session_key,
		post_login->password_match_return,
		appaserver_user_default_role_name(
			post_login->sql_injection_escape_login_name ),
		appaserver_user_role_name_list(
			post_login->sql_injection_escape_login_name ) );

	return 0;
}

