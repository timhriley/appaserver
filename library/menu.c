/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/menu.c					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "appaserver_parameter_file.h"
#include "appaserver_library.h"
#include "application.h"
#include "menu.h"

MENU_ITEM *menu_item_calloc( void )
{
	MENU_ITEM *menu_item;

	if ( ! ( menu_item = calloc( 1, sizeof( MENU_ITEM ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return menu_item;
}

MENU_ITEM *menu_item_folder_new(
			char *folder_name,
			char *folder_menu_count_display,
			char *state,
			char *login_name,
			char *application_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	MENU_ITEM *menu_item = menu_item_calloc();

	menu_item->folder_name = folder_name;
	menu_item->state = state;
	menu_item->login_name = login_name;
	menu_item->application_name = application_name;
	menu_item->session_key = session_key;
	menu_item->role_name = role_name;
	menu_item->target_frame = target_frame;

	menu_item->folder_action_tag =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		menu_item_folder_action_tag(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_library_http_prompt(
				appaserver_parameter_file_cgi_directory(),
				appaserver_library_server_address(),
				application_ssl_support_yn(
					application_name ),
				application_prepend_http_protocol_yn(
					application_name ) ),
			folder_name,
			state,
			login_name,
			application_name,
			session_key,
			role_name,
			target_frame );

	if ( !menu_item->folder_action_tag )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: menu_item_folder_action_tag() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	menu_item->span_tag =
		menu_item_span_tag(
			folder_name );

	return menu_item;
}

MENU_ITEM *menu_item_process_new(
			char *process_or_set_name,
			char *login_name,
			char *application_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	MENU_ITEM *menu_item = menu_item_calloc();

	menu_item->process_or_set_name = process_or_set_name;
	menu_item->login_name = login_name;
	menu_item->application_name = application_name;
	menu_item->session_key = session_key;
	menu_item->role_name = role_name;
	menu_item->target_frame = target_frame;

	menu_item->process_action_tag =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		menu_item_process_action_tag(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_library_http_prompt(
				appaserver_parameter_file_cgi_directory(),
				appaserver_library_server_address(),
				application_ssl_support_yn(
					application_name ),
				application_prepend_http_protocol_yn(
					application_name ) ),
			process_or_set_name,
			login_name,
			application_name,
			session_key,
			role_name,
			target_frame );

	if ( !menu_item->process_action_tag )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: menu_item_process_action_tag() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	menu_item->span_tag =
		menu_item_span_tag(
			process_or_set_name );

	return menu_item;
}

LIST *menu_item_folder_list(
			LIST *role_folder_name_list,
			char *state,
			char *login_name,
			char *application_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	LIST *menu_item_list;
	char *folder_name;

	if ( !list_rewind( role_folder_name_list ) ) return (LIST *)0;

	menu_item_list = list_new();

	do {
		folder_name = list_get( role_folder_name_list );

		list_set(
			menu_item_list,
			menu_item_folder_new(
				folder_name,
				state,
				login_name,
				application_name,
				session_key,
				role_name,
				target_frame ) );

	} while ( list_next( role_folder_name_list ) );

	return menu_item_list;
}

LIST *menu_item_process_list(
			LIST *process_or_set_name_list,
			char *login_name,
			char *application_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	LIST *menu_item_list;
	char *process_or_set_name;

	if ( !list_rewind( process_or_set_name_list ) ) return (LIST *)0;

	menu_item_list = list_new();

	do {
		process_or_set_name =
			list_get(
				process_or_set_name_list );

		list_set(
			menu_item_list,
			menu_item_process_new(
				process_or_set_name,
				login_name,
				application_name,
				session_key,
				role_name,
				target_frame ) );

	} while ( list_next( process_or_set_name_list ) );

	return menu_item_list;
}

char *menu_item_folder_action_tag(
			char *http_prompt,
			char *folder_name,
			char *state,
			char *login_name,
			char *application_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	char action_tag[ 1024 ];

	if ( !http_prompt || !*http_prompt )		return (char *)0;
	if ( !folder_name || !*folder_name )		return (char *)0;
	if ( !state || !*state )			return (char *)0;
	if ( !login_name || !*login_name )		return (char *)0;
	if ( !application_name || !*application_name )	return (char *)0;
	if ( !session_key || !*session_key )		return (char *)0;
	if ( !role_name || !*role_name )		return (char *)0;
	if ( !target_frame || !*target_frame )		return (char *)0;

	sprintf(action_tag,
"<a href=\"%s/post_choose_folder?%s+%s+%s+%s+%s+%s\" target=\"%s\">",
		http_prompt,
		login_name,
		application_name,
		session_key,
		folder_name,
		role_name,
		state,
		target_frame );

	return strdup( action_tag );
}

char *menu_item_process_action_tag(
			char *http_prompt,
			char *process_or_set_name,
			char *login_name,
			char *application_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	char action_tag[ 1024 ];

	if ( !http_prompt || !*http_prompt ) return (char *)0;

	if ( !process_or_set_name || !*process_or_set_name )
		return (char *)0;

	if ( !login_name || !*login_name )		return (char *)0;
	if ( !application_name || !*application_name )	return (char *)0;
	if ( !session_key || !*session_key )		return (char *)0;
	if ( !role_name || !*role_name )		return (char *)0;
	if ( !target_frame || !*target_frame )		return (char *)0;

	sprintf(action_tag,
"<a href=\"%s/post_choose_process?%s+%s+%s+%s+%s\" target=\"%s\">",
		http_prompt,
		login_name,
		application_name,
		session_key,
		process_or_set_name,
		role_name,
		target_frame );

	return strdup( action_tag );
}

char *menu_item_span_tag(
			char *item_name )
{
	char buffer[ 128 ];
	char span_tag[ 256 ];

	sprintf(span_tag,
		"<span class=menu>%s</span></a>",
		format_initial_capital(
			buffer,
			item_name ) );

	return strdup( span_tag );
}

LIST *menu_item_process_or_set_name_list(
			LIST *role_process_name_list,
			LIST *role_process_set_name_list )
{
	LIST *process_or_set_name_list = list_new();

	if ( list_rewind( role_process_name_list ) )
	{
		do {
			list_set_string_in_order(
				process_or_set_name_list,
				list_get( role_process_name_list ) );

		} while ( list_next( role_process_name_list ) );
	}

	if ( list_rewind( role_process_set_name_list ) )
	{
		do {
			list_set_string_in_order(
				process_or_set_name_list,
				list_get( role_process_set_name_list ) );

		} while ( list_next( role_process_set_name_list ) );
	}

	return process_or_set_name_list;
}

MENU_SUBSCHEMA *menu_subschema_calloc( void )
{
	MENU_SUBSCHEMA *menu_subschema;

	if ( ! ( menu_subschema = calloc( 1, sizeof( MENU_SUBSCHEMA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return menu_subschema;
}

MENU_SUBSCHEMA *menu_subschema_fetch(
			char *subschema_name,
			LIST *role_folder_name_list,
			char *verb,
			char *login_name,
			char *application_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	MENU_SUBSCHEMA *menu_subschema = menu_subschema_calloc();

	menu_subschema->subschema_name = subschema_name;
	menu_subschema->role_folder_name_list = role_folder_name_list;

	menu_subschema->menu_item_folder_list =
		menu_item_folder_list(
			role_folder_name_list,
			verb /* state */,
			login_name,
			application_name,
			session_key,
			role_name,
			target_frame );

	menu_subschema->span_tag =
		menu_subschema_span_tag(
			subschema_name );

	return menu_subschema;
}

char *menu_subschema_span_tag( char *subschema_name )
{
	return menu_item_span_tag( subschema_name );
}

