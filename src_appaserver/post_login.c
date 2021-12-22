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
#include "appaserver_error.h"
#include "appaserver.h"
#include "environ.h"
#include "basename.h"
#include "boolean.h"
#include "dictionary.h"
#include "post2dictionary.h"
#include "appaserver_parameter_file.h"
#include "application.h"
#include "security.h"
#include "post_login.h"
#include "frameset.h"

/* Prototypes */
/* ---------- */
int main( int argc, char **argv )
{
	POST_LOGIN *post_login;

	if ( ! ( post_login =
			/* --------------------------------- */
			/* Executes post_login_session_new() */
			/* --------------------------------- */
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

	if ( post_login->missing_login_name )
	{
		char msg[ 1024 ];

		sprintf(msg,
			"Missing login_name from %s",
			environment_get( "REMOTE_ADDR" ) );

		appaserver_output_error_message(
			post_login->post_login_application_name,
			msg,
			(char *)0 /* login_name */ );

		if ( !appaserver_library_from_php(
			post_login->post_login_dictionary ) )
		{
			printf( 
"Content-type: text/html					\n"
"\n"
"		<HTML>						\n"
"		<HEAD>						\n"
"			<TITLE>%s Please Try Again</TITLE>   	\n"
"		</HEAD>						\n"
"								\n"
"		<p>Please try again.				\n"
"		</HTML>						\n",
			application_name );
		}
		else
		{
			post_login_redraw_index_screen(
				post_login->post_login_application_name,
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
				login_name,
				environment_get( "REMOTE_ADDR" ) );
		}
		else
		{
			sprintf(msg,
				"Password missmatch for %s from %s",
				login_name,
				environment_get( "REMOTE_ADDR" ) );
		}

		appaserver_output_error_message(
			post_login->post_login_application_name,
			msg,
			login_name );

		if ( !appaserver_library_from_php( post_dictionary ) )
		{
			printf( 
"Content-type: text/html					\n"
"\n"
"		<HTML>						\n"
"		<HEAD>						\n"
"			<TITLE>%s Please Try Again</TITLE>   	\n"
"		</HEAD>						\n"
"								\n"
"		<p>Please try again.				\n"
"		</HTML>						\n",
			application_name );
		}
		else
		{
			post_login_redraw_index_screen(
				application_name,
				(char *)0 /* location */,
				"invalid_login_yn=y" );
		}

		sleep( POST_LOGIN_SECONDS_TO_SLEEP );
		exit ( 1 );
	}

	environ_set_environment(
		APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
		post_login->post_login_application_name );

	environ_set_environment(
		"DATABASE",
		post_login->post_login_application_name );

	add_utility_to_path();
	add_src_appaserver_to_path();
	environ_appaserver_home();

	add_relative_source_directory_to_path(
		post_login->post_login_application_name );

	appaserver_error_login_name_append_file(
		argc,
		argv,
		post_login->post_login_application_name,
		post_login->sql_injection_login_name );

	post_login_frameset_output(
		post_login->post_login_application_name,
		post_login->sql_injection_login_name );
		post_login->post_login_session );

	return 0;
}

