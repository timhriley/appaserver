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

	appaserver_error_login_name_append_file(
		argc,
		argv,
		post_login->sql_injection_escape_application_name,
		post_login->sql_injection_escape_login_name );

	if ( post_login->frameset_output_system_string )
	{
		if ( !post_login->session_key )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: session_key is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

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

