/* $APPASERVER_HOME/library/choose_role.c		*/
/* -----------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* -----------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "boolean.h"
#include "String.h"
#include "appaserver_user.h"
#include "appaserver_library.h"
#include "appaserver_parameter.h"
#include "application.h"
#include "environ.h"
#include "frameset.h"
#include "choose_role.h"

CHOOSE_ROLE *choose_role_calloc( void )
{
	CHOOSE_ROLE *choose_role;

	if ( ! ( choose_role =
			calloc( 1, sizeof( CHOOSE_ROLE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return choose_role;
}

char *choose_role_title_html( char *title_string )
{
	static char html[ 256 ];

	if ( !title_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: title_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"<h1>%s</h1>",
		title_string );

	return html;
}

char *choose_role_title_string( char *login_name )
{
	static char title[ 256 ];
	char buffer[ 128 ];

	if ( !login_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: login_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(title,
		"%s choose role for %s",
		application_title_string(
			environment_application_name() ),
		string_initial_capital(
			buffer,
			login_name ) );

	return title;
}

char *choose_role_post_action_string(
			char *choose_role_post_executable,
			char *application_name,
			char *session_key,
			char *login_name )
{
	char action_string[ 1024 ];

	if ( !choose_role_post_executable
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
		" action=\"%s/%s?%s+%s+%s\"",
			appaserver_library_http_prompt(
				appaserver_parameter_cgi_directory(),
				appaserver_library_server_address(),
				application_ssl_support_yn(
					application_name ),
				application_prepend_http_protocol_yn(
					application_name ) ),
		choose_role_post_executable,
		application_name,
		session_key,
		login_name );

	return strdup( action_string );
}

char *choose_role_target_frame(
			boolean frameset_menu_horizontal )
{
	if ( !frameset_menu_horizontal )
		return FRAMESET_MENU_FRAME;
	else
		return FRAMESET_PROMPT_FRAME;
}

CHOOSE_ROLE *choose_role_new(
			char *application_name,
			char *session_key,
			char *login_name,
			boolean frameset_menu_horizontal )
{
	CHOOSE_ROLE *choose_role = choose_role_calloc();

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

	choose_role->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		choose_role_title_string(
			login_name );

	choose_role->post_action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		choose_role_post_action_string(
			CHOOSE_ROLE_POST_EXECUTABLE,
			application_name,
			session_key,
			login_name );

	choose_role->target_frame =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		choose_role_target_frame(
			frameset_menu_horizontal );

	choose_role->title_html =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		choose_role_title_html(
			choose_role->title_string );

	choose_role->document =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		document_new(
			application_name,
			choose_role_title_string( login_name ),
			choose_role->title_html,
			(char *)0 /* subtitle_html */,
			(char *)0 /* subsubtitle_html */,
			(char *)0 /* javascript_replace */,
			0 /* menu_boolean */,
			(MENU *)0,
			(char *)0 /* menu_setup_string */,
			(char *)0 /* calendar_setup_string */,
			(char *)0 /* javascript_include_string */,
			(char *)0 /* input_onload_string */ );

	choose_role->form_choose_role =
		form_choose_role_new(
			appaserver_user_role_name_list( login_name ),
			choose_role->post_action_string,
			choose_role->target_frame,
			CHOOSE_ROLE_FORM_NAME,
			CHOOSE_ROLE_DROP_DOWN_ELEMENT_NAME );

	if ( !choose_role->form_choose_role )
	{
		return (CHOOSE_ROLE *)0;
	}

	choose_role->document_form_html =
		document_form_html(
			choose_role->document->html,
			choose_role->document->document_head->html,
			document_head_close_html(),
			choose_role->document->document_body->html,
			choose_role->form_choose_role->html,
			document_body_close_html(),
			document_close_html() );

	return choose_role;
}

CHOOSE_ROLE *choose_role_default_new( char *login_name )
{
	char *default_where;
	char *default_system_string;
	char *default_role_name;

	default_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		choose_role_default_where(
			login_name );

	default_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		choose_role_default_system_string(
			default_where );

	if ( ( default_role_name =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			choose_role_default_role_name(
				default_system_string ) ) )
	{
		CHOOSE_ROLE *choose_role = choose_role_calloc();

		choose_role->default_where = default_where;
		choose_role->default_system_string = default_system_string;
		choose_role->default_role_name = default_role_name;

		return choose_role;
	}

	return (CHOOSE_ROLE *)0;
}

char *choose_role_default_where( char *login_name )
{
	static char where[ 128 ];

	sprintf(where,
		"login_name = '%s'",
		login_name );

	return where;
}

char *choose_role_default_system_string( char *where )
{
	static char system_string[ 256 ];

	sprintf(system_string,
		"select.sh role login_default_role \"%s\"",
		where );

	return system_string;
}

char *choose_role_default_role_name( char *system_string )
{
	/* Returns heap memory or null */
	/* --------------------------- */
	return string_pipe_fetch( system_string );
}

char *choose_role_output_system_string(
			char *executable,
			char *session_key,
			char *login_name,
			boolean frameset_menu_horizontal,
			char *output_filename )
{
	char system_string[ 1024 ];

	if ( !executable
	||   !session_key
	||   !login_name
	||   !output_filename )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"%s %s %s %c > %s",
		executable,
		session_key,
		login_name,
		(frameset_menu_horizontal) ? 'y' : 'n',
		output_filename );

	return strdup( system_string );
}

