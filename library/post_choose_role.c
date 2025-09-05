/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/post_choose_role.c				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "boolean.h"
#include "String.h"
#include "session.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "application.h"
#include "menu.h"
#include "frameset.h"
#include "choose_role.h"
#include "execute_system_string.h"
#include "post_choose_role.h"

POST_CHOOSE_ROLE *post_choose_role_calloc( void )
{
	POST_CHOOSE_ROLE *post_choose_role;

	if ( ! ( post_choose_role =
			calloc( 1, sizeof ( POST_CHOOSE_ROLE ) ) ) )
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

char *post_choose_role_action_string(
		char *post_choose_role_executable,
		char *application_name,
		char *session_key,
		char *login_name )
{
	static char action_string[ 256 ];

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

	if ( *action_string ) return action_string;

	sprintf(action_string,
		"%s/%s?%s+%s+%s",
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
		post_choose_role_executable,
		application_name,
		session_key,
		login_name );

	return action_string;
}

char *post_choose_role_name(
		int argc,
		char *argv_4,
		char *choose_role_drop_down_widget_name )
{
	POST_DICTIONARY *post_dictionary;

	if ( argc == 5 )
	{
		return
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			argv_4 );
	}

	post_dictionary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		post_dictionary_stdin_new(
			/* ---------------------------------- */
			/* Used when expecting a spooled file */
			/* ---------------------------------- */
			(char *)0 /* application_name */,
			(char *)0 /* upload_directory */,
			(LIST *)0 /* upload_filename_list */ );

	return
	/* ---------------------------- */
	/* Returns heap memory or datum */
	/* ---------------------------- */
	security_sql_injection_escape(
		SECURITY_ESCAPE_CHARACTER_STRING,
		/* --------------------------------------- */
		/* Returns component of dictionary or null */
		/* --------------------------------------- */
		dictionary_get(
			choose_role_drop_down_widget_name,
			post_dictionary->original_post_dictionary )
				/* datum */ );
}

POST_CHOOSE_ROLE *post_choose_role_new(
		int argc,
		char **argv,
		char *choose_role_drop_down_element_name )
{
	POST_CHOOSE_ROLE *post_choose_role;

{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: \n",
	__FILE__,
	__FUNCTION__,
	__LINE__ );
msg( "brighton", message );
}
	if ( argc < 3 )
	{
		fprintf(stderr,
			"Usage: %s application session login [role]\n",
			argv[ 0 ] );

		return (POST_CHOOSE_ROLE *)0;
	}

	post_choose_role = post_choose_role_calloc();

{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: \n",
	__FILE__,
	__FUNCTION__,
	__LINE__ );
msg( "brighton", message );
}
	post_choose_role->sql_injection_escape_application_name =
		/* ---------------------------- */
		/* Returns heap memory or datum */
		/* ---------------------------- */
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			argv[ 1 ] /* datum */ );

{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: \n",
	__FILE__,
	__FUNCTION__,
	__LINE__ );
msg( "brighton", message );
}
	post_choose_role->sql_injection_escape_session_key =
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			argv[ 2 ] );

	post_choose_role->sql_injection_escape_login_name =
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			argv[ 3 ] );

	post_choose_role->name =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		post_choose_role_name(
			argc,
			argv[ 4 ],
			choose_role_drop_down_element_name );

{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: application_name=[%s]\n",
	__FILE__,
	__FUNCTION__,
	__LINE__,
	post_choose_role->sql_injection_escape_application_name );
msg( "brighton", message );
}
	if ( !post_choose_role->name )
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

{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: \n",
	__FILE__,
	__FUNCTION__,
	__LINE__ );
msg( "brighton", message );
}
	post_choose_role->execute_system_string_menu =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		execute_system_string_menu(
			MENU_EXECUTABLE,
			post_choose_role->sql_injection_escape_application_name,
			post_choose_role->sql_injection_escape_session_key,
			post_choose_role->sql_injection_escape_login_name,
			post_choose_role->name,
			application_menu_horizontal_boolean(
				post_choose_role->
				     sql_injection_escape_application_name ) );

{
char message[ 65536 ];
snprintf(
	message,
	sizeof ( message ),
	"%s/%s()/%d: \n",
	__FILE__,
	__FUNCTION__,
	__LINE__ );
msg( "brighton", message );
}
	return post_choose_role;
}

char *post_choose_role_href_string(
		char *role_name,
		char *post_choose_role_action_string )
{
	static char href_string[ 256 ];

	if ( !role_name
	||   !post_choose_role_action_string )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(href_string,
		"%s+%s",
		post_choose_role_action_string,
		role_name );

	return href_string;
}

char *post_choose_role_anchor_tag(
		char *target_frame,
		char *post_choose_role_href_string )
{
	static char anchor_tag[ 256 ];

	if ( !target_frame
	||   !post_choose_role_href_string )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(anchor_tag,
		"<a href=\"%s\" target=%s>",
		post_choose_role_href_string,
		target_frame );

	return anchor_tag;
}

