/* $APPASERVER_HOME/library/post_choose_role.c		*/
/* -----------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* -----------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "boolean.h"
#include "String.h"
#include "appaserver_library.h"
#include "appaserver_parameter.h"
#include "application.h"
#include "post_choose_role.h"

POST_CHOOSE_ROLE *post_choose_role_calloc( void )
{
	POST_CHOOSE_ROLE *post_choose_role;

	if ( ! ( post_choose_role =
			calloc( 1, sizeof( POST_CHOOSE_ROLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return post_choose_role;
}

char *post_choose_role_href_string(
			char *post_choose_role_executable,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *frameset_prompt_frame )
{
	char href_string[ 1024 ];

	if ( !post_choose_role_executable
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
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
		"href=\"%s/%s?%s+%s+%s+%s\" target=%s",
		appaserver_library_http_prompt(
			appaserver_parameter_cgi_directory(),
			appaserver_library_server_address(),
			application_ssl_support_yn(
				application_name ),
			application_prepend_http_protocol_yn(
				application_name ) ),
		post_choose_role_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		frameset_prompt_frame );

	return strdup( href_string );
}

POST_CHOOSE_ROLE *post_choose_role_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name )
{
	POST_CHOOSE_ROLE *post_choose_role = post_choose_role_calloc();

	if ( !application_name
	||   !session_key
	||   !login_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return post_choose_role;
}

char *post_choose_role_action_string(
			char *post_choose_role_executable,
			char *application_name,
			char *session_key,
			char *login_name )
{
	char action_string[ 1024 ];

	if ( !post_choose_role_executable
	||   !application_name
	||   !session_key
	||   !login_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(action_string,
		"action=\"%s/%s?%s+%s+%s+\"",
		appaserver_library_http_prompt(
			appaserver_parameter_cgi_directory(),
			appaserver_library_server_address(),
			application_ssl_support_yn(
				application_name ),
			application_prepend_http_protocol_yn(
				application_name ) ),
		post_choose_role_executable,
		application_name,
		session_key,
		login_name );

	return strdup( action_string );
}

