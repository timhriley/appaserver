/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/choose_role.c				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "boolean.h"
#include "String.h"
#include "appaserver_user.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "application.h"
#include "environ.h"
#include "frameset.h"
#include "menu.h"
#include "post_choose_role.h"
#include "choose_role.h"

CHOOSE_ROLE *choose_role_calloc( void )
{
	CHOOSE_ROLE *choose_role;

	if ( ! ( choose_role = calloc( 1, sizeof ( CHOOSE_ROLE ) ) ) )
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

char *choose_role_title_string(
		char *application_title_string,
		char *login_name,
		boolean application_menu_horizontal_boolean )
{
	static char title[ 256 ];
	char *ptr = title;
	char buffer[ 128 ];

	if ( !application_title_string
	||   !login_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( application_menu_horizontal_boolean )
	{
		ptr += sprintf(
			ptr,
			"%s<br>",
			application_title_string );
	}

	ptr += sprintf(
		ptr,
		"Choose role for %s",
		string_initial_capital(
			buffer,
			login_name ) );

	return title;
}

CHOOSE_ROLE *choose_role_new(
		char *application_name,
		char *session_key,
		char *login_name,
		boolean application_menu_horizontal_boolean )
{
	CHOOSE_ROLE *choose_role;

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

	choose_role = choose_role_calloc();

	choose_role->application_title_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		application_title_string(
			application_name ),

	choose_role->title_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		choose_role_title_string(
			choose_role->application_title_string,
			login_name,
			application_menu_horizontal_boolean );

	choose_role->appaserver_user_role_name_list =
		appaserver_user_role_name_list(
			login_name );

	if ( !list_length( choose_role->appaserver_user_role_name_list ) )
	{
		char message[ 256 ];

		sprintf(message,
			"appaserver_user_role_name_list(%s) returned empty.",
			login_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	choose_role->post_choose_role_action_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_choose_role_action_string(
			POST_CHOOSE_ROLE_EXECUTABLE,
			application_name,
			session_key,
			login_name );

	if ( application_menu_horizontal_boolean )
	{
		choose_role->choose_role_horizontal =
			choose_role_horizontal_new(
				application_name,
				login_name,
				choose_role->application_title_string,
				choose_role->title_string,
				choose_role->appaserver_user_role_name_list,
				choose_role->post_choose_role_action_string,
				FRAMESET_PROMPT_FRAME
					/* target_frame */ );
	}
	else
	{
		choose_role->choose_role_vertical =
			choose_role_vertical_new(
				application_name,
				choose_role->application_title_string,
				choose_role->title_string,
				choose_role->appaserver_user_role_name_list,
				choose_role->post_choose_role_action_string,
				FRAMESET_MENU_FRAME
					/* target_frame */ );
	}

	return choose_role;
}

CHOOSE_ROLE_VERTICAL *choose_role_vertical_new(
		char *application_name,
		char *application_title_string,
		char *choose_role_title_string,
		LIST *appaserver_user_role_name_list,
		char *post_choose_role_action_string,
		char *target_frame
			/* FRAMESET_MENU_FRAME */ )
{
	CHOOSE_ROLE_VERTICAL *choose_role_vertical;

	if ( !application_name
	||   !application_title_string
	||   !choose_role_title_string
	||   !list_length( appaserver_user_role_name_list )
	||   !target_frame )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	choose_role_vertical = choose_role_vertical_calloc();

	choose_role_vertical->document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_new(
			application_name,
			application_title_string,
			choose_role_title_string,
			(char *)0 /* sub_title_string */,
			(char *)0 /* sub_sub_title_string */,
			(char *)0 /* notepad */,
			(char *)0 /* onload_javascript_string */,
			(char *)0 /* menu_html */,
			(char *)0 /* menu_setup_string */,
			(char *)0 /* calendar_setup_string */,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			javascript_include_string(
				application_name,
				(LIST *)0 /* javascript_filename_list */ ) );

	choose_role_vertical->form_choose_role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_choose_role_new(
			appaserver_user_role_name_list,
			post_choose_role_action_string,
			target_frame,
			FORM_CHOOSE_ROLE_NAME,
			CHOOSE_ROLE_DROP_DOWN_NAME );

	choose_role_vertical->document_form_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_form_html(
			choose_role_vertical->document->html,
			choose_role_vertical->document->document_head->html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_head_close_tag(),
			choose_role_vertical->document->document_body->html,
			choose_role_vertical->form_choose_role->html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_body_close_tag(),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_close_tag() );

	return choose_role_vertical;
}

CHOOSE_ROLE_VERTICAL *choose_role_vertical_calloc( void )
{
	CHOOSE_ROLE_VERTICAL *choose_role_vertical;

	if ( ! ( choose_role_vertical =
			calloc( 1, sizeof ( CHOOSE_ROLE_VERTICAL ) ) ) )
	{
		char message[ 256 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return choose_role_vertical;
}

CHOOSE_ROLE_HORIZONTAL *choose_role_horizontal_new(
		char *application_name,
		char *login_name,
		char *application_title_string,
		char *choose_role_title_string,
		LIST *appaserver_user_role_name_list,
		char *post_choose_role_action_string,
		char *target_frame )
{
	CHOOSE_ROLE_HORIZONTAL *choose_role_horizontal;

	if ( !application_name
	||   !login_name
	||   !application_title_string
	||   !choose_role_title_string
	||   !list_length( appaserver_user_role_name_list )
	||   !post_choose_role_action_string
	||   !target_frame )
	{
		char message[ 256 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	choose_role_horizontal = choose_role_horizontal_calloc();

	choose_role_horizontal->menu_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		choose_role_horizontal_menu_html(
			login_name,
			appaserver_user_role_name_list,
			target_frame,
			post_choose_role_action_string );

	choose_role_horizontal->document =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		document_new(
			application_name,
			application_title_string,
			choose_role_title_string,
			(char *)0 /* sub_title_string */,
			(char *)0 /* sub_sub_title_string */,
			(char *)0 /* notepad */,
			(char *)0 /* onload_javascript_string */,
			choose_role_horizontal->menu_html,
			document_head_menu_setup_string(
				1 /* menu_boolean */ ),
			(char *)0 /* calendar_setup_string */,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			javascript_include_string(
				application_name,
				(LIST *)0 /* javascript_filename_list */ ) );

	choose_role_horizontal->document_form_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		document_form_html(
			choose_role_horizontal->document->html,
			choose_role_horizontal->document->document_head->html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_head_close_tag(),
			choose_role_horizontal->document->document_body->html,
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			(char *)0 /* form_html */,
			document_body_close_tag(),
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			document_close_tag() );

	return choose_role_horizontal;
}

CHOOSE_ROLE_HORIZONTAL *choose_role_horizontal_calloc( void )
{
	CHOOSE_ROLE_HORIZONTAL *choose_role_horizontal;

	if ( ! ( choose_role_horizontal =
			calloc( 1,
				sizeof ( CHOOSE_ROLE_HORIZONTAL ) ) ) )
	{
		char message[ 256 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return choose_role_horizontal;
}

char *choose_role_horizontal_menu_html(
		char *login_name,
		LIST *appaserver_user_role_name_list,
		char *target_frame,
		char *post_choose_role_action_string )
{
	char html[ STRING_64K ];
	char *ptr = html;
	char *role_name;
	char *span_tag;
	char *href_string;
	char *anchor_tag;
	char *horizontal_html;
	char buffer[ 128 ];

	ptr += sprintf( ptr, "%s\n", MENU_UNORDERED_LIST_TAG );

	span_tag =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		menu_item_heading_span_tag(
			MENU_ITEM_HORIZONTAL_CLASS_NAME,
			"Role" /* heading_string */ );

	ptr += sprintf( ptr, "%s\n", span_tag );

	ptr += sprintf( ptr, "\t<ul>\n" );

	list_rewind( appaserver_user_role_name_list );

	do {
		role_name = list_get( appaserver_user_role_name_list );

		span_tag =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			menu_item_span_tag(
				MENU_ITEM_HORIZONTAL_CLASS_NAME,
				role_name /* item */ );

		href_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			menu_item_href_string(
				role_name /* item */,
				post_choose_role_action_string );

		anchor_tag =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			menu_item_anchor_tag(
				target_frame,
				href_string );

		horizontal_html =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			menu_item_horizontal_html(
				span_tag,
				anchor_tag );

		ptr += sprintf( ptr, "\t%s\n", horizontal_html );

		free( horizontal_html );

	} while ( list_next( appaserver_user_role_name_list ) );

	ptr += sprintf( ptr, "\t</ul>\n" );

	ptr += sprintf( ptr,
		"<li><a><label style=\"color:black\">" );

	ptr += sprintf( ptr,
		"%s</label></a>\n",
		string_initial_capital(
			buffer,
			login_name ) );

	sprintf( ptr, "</ul>" );

	return strdup( html );
}

