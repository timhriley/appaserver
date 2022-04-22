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
#include <unistd.h>
#include "String.h"
#include "date.h"
#include "timlib.h"
#include "appaserver_error.h"
#include "document.h"
#include "environ.h"
#include "session.h"
#include "post_login.h"

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

	appaserver_error_login_name_append_file(
		argc,
		argv,
		post_login->sql_injection_escape_application_name,
		post_login->sql_injection_escape_login_name );

	if ( post_login->password_match_return == signup )
	{
		char destination[ 128 ];

		environment_set(
			"DATABASE",
			POST_LOGIN_TEMPLATE_APPLICATION );

		session_insert(
			post_login->session_key,
			"nobody" /* login_name */,
			date_now_yyyy_mm_dd_string( date_utc_offset() ),
			date_now_hhmm_string( date_utc_offset() ),
			left_string(
				destination,
				environment_http_user_agent(),
				80 ),
			environment_remote_ip_address() );

		if ( !post_login->empty_application_system_string )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: empty_application_system_string is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( system(
			post_login->
				empty_application_system_string ) ){}

		session_delete( post_login->session_key );
	}

	if ( post_login->password_match_return == password_match
	||   post_login->password_match_return == public_login
	||   post_login->password_match_return == email_login )
	{
		char destination[ 128 ];

		environment_set(
			"DATABASE",
			post_login->sql_injection_escape_application_name );

		session_insert(
			post_login->session_key,
			post_login->sql_injection_escape_login_name,
			date_now_yyyy_mm_dd_string( date_utc_offset() ),
			date_now_hhmm_string( date_utc_offset() ),
			left_string(
				destination,
				environment_http_user_agent(),
				80 ),
			environment_remote_ip_address() );
	}

	if ( post_login->frameset_output_system_string )
	{
		document_output_content_type();

		if ( system( post_login->frameset_output_system_string ) ){}
	}

	if ( post_login->email_link_system_string )
	{
		if ( system( post_login->email_link_system_string ) ){}
	}

	if ( post_login->redraw_index_screen_string )
	{
		printf( "%s\n", post_login->redraw_index_screen_string );
	}

	if ( post_login->password_match_return == password_fail )
	{
		sleep( POST_LOGIN_SECONDS_TO_SLEEP );
	}

	return 0;
}

