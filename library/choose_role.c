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
#include "choose_role.h"

CHOOSE_ROLE *choose_role_calloc( void )
{
	CHOOSE_ROLE *choose_role;

	if ( ! ( choose_isa =
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
			char *application_name,
			char *session_key,
			char *login_name )
{
	char action_string[ 1024 ];

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


	sprintf(action_string,
		" action=\"%s/%s?%s+%s+%s\"",
			appaserver_library_http_prompt(
				appaserver_parameter_file_cgi_directory(),
				appaserver_library_server_address(),
				application_ssl_support_yn(
					application_name ),
				application_prepend_http_protocol_yn(
					application_name ) ),
		"post_choose_role",
		application_name,
		login_name,
		session_key );

	return strdup( action_string );
}

char *choose_role_document_html(
			DOCUMENT_CHOOSE_ROLE *document_choose_role )
{
}

char *choose_role_target_frame(
			boolean frameset_menu_horizontal )
{
	if ( !frameset_menu_horizontal )
		return FRAMESET_MENU_FRAME
	else
		return FRAMESET_PROMPT_FRAME
}

CHOOSE_ROLE *choose_role_prompt_new(
			char *application_name,
			char *session_key,
			char *login_name,
			boolean frameset_menu_horizontal )
{
	CHOOSE_ROLE *choose_role = choose_role_calloc();

	choose_role->title_string =
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		choose_role_title_string(
			login_name );

	choose_role->post_action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		choose_role_post_action_string(
			application_name,
			session_key,
			login_name );

	choose_role->target_frame =
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		choose_role_target_frame(
			frameset_menu_horizontal );

	choose_role->document_choose_role =
		document_choose_role_new(
			application_name,
			choose_role->title_string,
			appaserver_user_role_name_list( login_name ),
			choose_role->post_action_string,
			choose_role->target_frame );

	if ( !choose_role->document_choose_role )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: document_choose_role_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	choose_role->html =
		choose_role_html(
			choose_role->
				document_choose_role->
				html );

	return choose_role;
}

char *choose_role_html( char *document_choose_role_html )
{
	return document_choose_role_html;
}
