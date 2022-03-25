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
#include "session.h"
#include "appaserver_parameter.h"
#include "application.h"
#include "menu.h"
#include "frameset.h"
#include "choose_role.h"
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
		"%s/%s?%s+%s+%s",
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

char *post_choose_role_name(
			int argc,
			char *argv_4,
			char *choose_role_drop_down_element_name )
{
	POST_DICTIONARY *post_dictionary;

	if ( argc == 5 )
	{
		return security_sql_injection_escape( argv_4 );
	}

	post_dictionary =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		post_dictionary_stdin_new(
			/* ---------------------------------- */
			/* Used when expecting a spooled file */
			/* ---------------------------------- */
			(char *)0 /* upload_directory */,
			(char *)0 /* session_key */ );

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	security_sql_injection_escape(
		dictionary_get(
			choose_role_drop_down_element_name,
			post_dictionary->original_post_dictionary ) );
}

POST_CHOOSE_ROLE *post_choose_role_new(
			int argc,
			char **argv,
			char *choose_role_drop_down_element_name )
{
	POST_CHOOSE_ROLE *post_choose_role;

	if ( argc < 3 )
	{
		fprintf(stderr,
			"Usage: %s application session login [role]\n",
			argv[ 0 ] );
		return (POST_CHOOSE_ROLE *)0;
	}

	post_choose_role = post_choose_role_calloc();

	post_choose_role->sql_injection_escape_application_name =
		security_sql_injection_escape(
			argv[ 1 ] );

	post_choose_role->sql_injection_escape_session_key =
		security_sql_injection_escape(
			argv[ 2 ] );

	post_choose_role->sql_injection_escape_login_name =
		security_sql_injection_escape(
			argv[ 3 ] );

	post_choose_role->sql_injection_escape_role_name =
		post_choose_role_name(
			argc,
			argv[ 4 ],
			choose_role_drop_down_element_name );

	if ( !post_choose_role->sql_injection_escape_role_name
	||   !*post_choose_role->sql_injection_escape_role_name )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: cannot get role_name.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (POST_CHOOSE_ROLE *)0;
	}

	session_environment_set(
		post_choose_role->
			sql_injection_escape_application_name );

	post_choose_role->menu_output_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_output_system_string(
			MENU_OUTPUT_EXECUTABLE,
			post_choose_role->sql_injection_escape_session_key,
			post_choose_role->sql_injection_escape_login_name,
			post_choose_role->sql_injection_escape_role_name,
			frameset_menu_horizontal(
				post_choose_role->
					sql_injection_escape_application_name,
				post_choose_role->
					sql_injection_escape_login_name ),
			(char *)0 /* frameset_output_filename */ );

	return post_choose_role;
}
