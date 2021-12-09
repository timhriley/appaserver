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
#include "frameset.h"
#include "folder_menu.h"
#include "role.h"
#include "role_folder.h"
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

	menu_item->action_tag =
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
			application_name,
			login_name,
			session_key,
			role_name,
			target_frame );

	if ( !menu_item->action_tag )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: menu_item_folder_action_tag() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	menu_item->span_tag =
		menu_item_folder_span_tag(
			folder_name,
			folder_menu_count_display );

	return menu_item;
}

MENU_ITEM *menu_item_process_new(
			char *process_or_set_name,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	MENU_ITEM *menu_item = menu_item_calloc();

	menu_item->process_or_set_name = process_or_set_name;
	menu_item->application_name = application_name;
	menu_item->login_name = login_name;
	menu_item->session_key = session_key;
	menu_item->role_name = role_name;
	menu_item->target_frame = target_frame;

	menu_item->action_tag =
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
			application_name,
			login_name,
			session_key,
			role_name,
			target_frame );

	if ( !menu_item->action_tag )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: menu_item_process_action_tag() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	menu_item->span_tag =
		menu_item_process_span_tag(
			process_or_set_name );

	return menu_item;
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
				application_name,
				login_name,
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
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	char action_tag[ 1024 ];

	if ( !http_prompt || !*http_prompt )		return (char *)0;
	if ( !folder_name || !*folder_name )		return (char *)0;
	if ( !state || !*state )			return (char *)0;
	if ( !application_name || !*application_name )	return (char *)0;
	if ( !login_name || !*login_name )		return (char *)0;
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
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	char action_tag[ 1024 ];

	if ( !http_prompt || !*http_prompt ) return (char *)0;

	if ( !process_or_set_name || !*process_or_set_name )
		return (char *)0;

	if ( !application_name || !*application_name )	return (char *)0;
	if ( !login_name || !*login_name )		return (char *)0;
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

char *menu_subschema_span_tag( char *subschema_name )
{
	return menu_item_span_tag( subschema_name );
}

MENU *menu_calloc( void )
{
	MENU *menu;

	if ( ! ( menu = calloc( 1, sizeof( MENU ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return menu;
}

MENU *menu_fetch(	char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame,
			LIST *folder_menu_lookup_count_list )
{
	MENU *menu = menu_calloc();
	MENU_VERB *menu_verb;

	menu->menu_verb_list = list_new();

	list_set(
		menu->menu_verb_list,
		menu_verb_lookup_subschema_fetch(
			role_folder_lookup_list(
				role_name ),
			folder_menu_lookup_count_list,
			application_name,
			login_name,
			session_key,
			role_name,
			target_frame ) );

	list_set(
		menu->menu_verb_list,
		menu_verb_insert_subschema_fetch(
			role_folder_insert_list(
				role_name ),
			application_name,
			login_name,
			session_key,
			role_name,
			target_frame ) );

	menu->role_process_list = role_process_list( role_name );
	menu->role_process_set_list = role_process_set_list( role_name );

	menu->menu_process_group_name_list =
		menu_process_group_name_list(
			role_process_group_name_list(
				menu->role_process_list ),
			role_process_set_group_name_list(
				menu->role_process_set_list ) );

	if ( list_rewind( menu->menu_process_group_name_list ) )
	{
		char *group_name;

		do {
			group_name =
				list_get(
					menu->menu_process_group_name_list );

			list_set(
				menu->menu_verb_list,
				menu_verb_process_fetch(
					group_name /* verb */,
					menu->role_process_list,
					menu->role_process_set_list,
					application_name,
					login_name,
					session_key,
					role_name,
					target_frame ) );

		} while ( list_next( menu->menu_process_group_name_list ) );
	}

	if ( ( menu_verb =
		menu_verb_role_change_fetch(
			role_name_list(
				role_list_fetch(
					login_name ) ),
			application_name,
			login_name,
			session_key,
			role_name ) ) )
	{
		list_set(
			menu->menu_verb_list,
			menu_verb );
	}

	list_set(
		menu->menu_verb_list,
		menu_verb_role_display( role_name ) );

	list_set(
		menu->menu_verb_list,
		menu_verb_login_name_display( login_name ) );

	return menu;
}

MENU_VERB *menu_verb_calloc( void )
{
	MENU_VERB *menu_verb;

	if ( ! ( menu_verb = calloc( 1, sizeof( MENU_VERB ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return menu_verb;
}

MENU_VERB *menu_verb_lookup_subschema_fetch(
			LIST *role_folder_lookup_list,
			LIST *folder_menu_lookup_count_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	MENU_VERB *menu_verb = menu_verb_calloc();

	menu_verb->subschema_list =
		menu_lookup_subschema_list(
			role_folder_lookup_list,
			folder_menu_lookup_count_list,
			application_name,
			login_name,
			session_key,
			role_name,
			target_frame );

	menu_verb->item_list =
		menu_lookup_folder_item_list(
			role_folder_subschema_missing_folder_name_list(
				role_folder_lookup_list )
					/* role_folder_lookup_name_list */,
			folder_menu_lookup_count_list,
			application_name,
			login_name,
			session_key,
			role_name,
			target_frame );

	if ( !list_length( menu_verb->subschema_list )
	&&   !list_length( menu_verb->item_list ) )
	{
		return (MENU_VERB *)0;
	}

	menu_verb->menu_verb_tag =
		menu_verb_tag(
			"lookup" /* verb */ );

	return menu_verb;
}

MENU_VERB *menu_verb_insert_subschema_fetch(
			LIST *role_folder_insert_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	MENU_VERB *menu_verb = menu_verb_calloc();

	menu_verb->subschema_list =
		menu_insert_subschema_list(
			role_folder_insert_list,
			application_name,
			login_name,
			session_key,
			role_name,
			target_frame );

	menu_verb->item_list =
		menu_insert_folder_item_list(
			role_folder_subschema_missing_folder_name_list(
				role_folder_insert_list )
					/* role_folder_insert_name_list */,
			application_name,
			login_name,
			session_key,
			role_name,
			target_frame );

	if ( !list_length( menu_verb->subschema_list )
	&&   !list_length( menu_verb->item_list ) )
	{
		return (MENU_VERB *)0;
	}

	menu_verb->menu_verb_tag =
		menu_verb_tag(
			"insert" /* verb */ );

	return menu_verb;
}

LIST *menu_lookup_subschema_list(
			LIST *role_folder_lookup_list,
			LIST *folder_menu_lookup_count_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	LIST *subschema_name_list;
	char *subschema_name;
	LIST *subschema_list;

	subschema_name_list =
		role_folder_subschema_name_list(
			role_folder_lookup_list );

	if ( !list_rewind( subschema_name_list ) ) return (LIST *)0;

	subschema_list = list_new();

	do {
		subschema_name =
			list_get(
				subschema_name_list );

		list_set(
			subschema_list,
			menu_lookup_subschema_fetch(
				subschema_name,
				role_folder_subschema_folder_name_list(
					subschema_name,
					role_folder_lookup_list ),
				folder_menu_lookup_count_list,
				application_name,
				login_name,
				session_key,
				role_name,
				target_frame ) );

	} while ( list_next( subschema_name_list ) );

	return subschema_list;
}

LIST *menu_insert_subschema_list(
			LIST *role_folder_insert_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	LIST *subschema_name_list;
	char *subschema_name;
	LIST *subschema_list;

	subschema_name_list =
		role_folder_subschema_name_list(
			role_folder_insert_list );

	if ( !list_rewind( subschema_name_list ) ) return (LIST *)0;

	subschema_list = list_new();

	do {
		subschema_name =
			list_get(
				subschema_name_list );

		list_set(
			subschema_list,
			menu_insert_subschema_fetch(
				subschema_name,
				role_folder_subschema_folder_name_list(
					subschema_name,
					role_folder_insert_list ),
				application_name,
				login_name,
				session_key,
				role_name,
				target_frame ) );

	} while ( list_next( subschema_name_list ) );

	return subschema_list;
}

MENU_SUBSCHEMA *menu_lookup_subschema_fetch(
			char *subschema_name,
			LIST *role_folder_lookup_name_list,
			LIST *folder_menu_lookup_count_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	MENU_SUBSCHEMA *menu_subschema = menu_subschema_calloc();

	menu_subschema->subschema_name = subschema_name;

	menu_subschema->item_list =
		menu_lookup_folder_item_list(
			role_folder_lookup_name_list,
			folder_menu_lookup_count_list,
			application_name,
			login_name,
			session_key,
			role_name,
			target_frame );

	if ( !list_length( menu_subschema->item_list ) )
		return (MENU_SUBSCHEMA *)0;

	menu_subschema->span_tag =
		menu_subschema_span_tag(
			subschema_name );

	return menu_subschema;
}

MENU_SUBSCHEMA *menu_insert_subschema_fetch(
			char *subschema_name,
			LIST *role_folder_insert_name_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	MENU_SUBSCHEMA *menu_subschema = menu_subschema_calloc();

	menu_subschema->subschema_name = subschema_name;

	menu_subschema->item_list =
		menu_insert_folder_item_list(
			role_folder_insert_name_list,
			application_name,
			login_name,
			session_key,
			role_name,
			target_frame );

	if ( !list_length( menu_subschema->item_list ) )
		return (MENU_SUBSCHEMA *)0;

	menu_subschema->span_tag =
		menu_subschema_span_tag(
			subschema_name );

	return menu_subschema;
}

LIST *menu_lookup_folder_item_list(
			LIST *role_folder_lookup_name_list,
			LIST *folder_menu_lookup_count_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	char *folder_name;
	LIST *menu_item_list;
	FOLDER_MENU_COUNT *folder_menu_count;
	char *folder_menu_count_display;

	if ( !list_rewind( role_folder_lookup_name_list ) ) return (LIST *)0;

	menu_item_list = list_new();

	do {
		folder_name =
			list_get(
				role_folder_lookup_name_list );

		if ( ( folder_menu_count =
				folder_menu_count_seek(
					folder_name,
					folder_menu_lookup_count_list ) ) )
		{
			folder_menu_count_display =
				folder_menu_count->display;
		}
		else
		{
			folder_menu_count_display = (char *)0;
		}

		list_set(
			menu_item_list,
			menu_item_folder_new(
				folder_name,
				folder_menu_count_display,
				"lookup" /* state */,
				application_name,
				login_name,
				session_key,
				role_name,
				target_frame ) );

	} while ( list_next( role_folder_lookup_name_list ) );

	return menu_item_list;
}

LIST *menu_insert_folder_item_list(
			LIST *role_folder_insert_name_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	char *folder_name;
	LIST *menu_item_list;

	if ( !list_rewind( role_folder_insert_name_list ) ) return (LIST *)0;

	menu_item_list = list_new();

	do {
		folder_name =
			list_get(
				role_folder_insert_name_list );

		list_set(
			menu_item_list,
			menu_item_folder_new(
				folder_name,
				(char *)0 /* folder_menu_count_display */,
				"insert" /* state */,
				application_name,
				login_name,
				session_key,
				role_name,
				target_frame ) );

	} while ( list_next( role_folder_insert_name_list ) );

	return menu_item_list;
}

char *menu_item_folder_span_tag(
			char *folder_name,
			char *folder_menu_count_display )
{
	char folder_tag[ 128 ];
	char buffer[ 64 ];

	if ( !folder_name || !*folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	if ( folder_menu_count_display
	&&   *folder_menu_count_display )
	{
		char buffer[ 64 ];

		sprintf(folder_tag,
			"<a><span class=menu>%s %s</span></a>",
			format_initial_capital( buffer, folder_name ),
			folder_menu_count_display );
	}
	else
	{
		sprintf(folder_tag,
			"<a><span class=menu>%s</span></a>",
			format_initial_capital( buffer, folder_name ) );
	}

	return strdup( folder_tag );
}

char *menu_item_process_span_tag(
			char *process_or_set_name )
{
	char process_tag[ 128 ];
	char buffer[ 64 ];

	if ( !process_or_set_name || !*process_or_set_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: process_or_set_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(process_tag,
		"<a><span class=menu>%s</span></a>",
		format_initial_capital( buffer, process_or_set_name ) );

	return strdup( process_tag );
}

LIST *menu_process_group_name_list(
			LIST *role_process_group_name_list,
			LIST *role_process_set_group_name_list )
{
	LIST *group_name_list = list_new();
	char *group_name;

	if ( list_rewind( role_process_group_name_list ) )
	{
		do {
			group_name =
				list_get(
					role_process_group_name_list );

			if ( list_string_exists(
				group_name,
				group_name_list ) )
			{
				continue;
			}

			list_set_string_in_order(
				group_name_list,
				group_name );

		} while ( list_next( role_process_group_name_list ) );
	}

	if ( list_rewind( role_process_set_group_name_list ) )
	{
		do {
			group_name =
				list_get(
					role_process_set_group_name_list );

			if ( list_string_exists(
				group_name,
				group_name_list ) )
			{
				continue;
			}

			list_set_string_in_order(
				group_name_list,
				group_name );

		} while ( list_next( role_process_set_group_name_list ) );
	}

	return group_name_list;
}

MENU_VERB *menu_verb_process_fetch(
			char *verb,
			LIST *role_process_list,
			LIST *role_process_set_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	MENU_VERB *menu_verb = menu_verb_calloc();

	menu_verb->item_list =
		menu_process_item_list(
			role_process_or_set_name_list(
				verb /* process_group_name */,
				role_process_list,
				role_process_set_list ),
			application_name,
			login_name,
			session_key,
			role_name,
			target_frame );

	if ( !list_length( menu_verb->item_list ) )
		return (MENU_VERB *)0;

	menu_verb->menu_verb_tag =
		menu_verb_tag(
			verb );

	return menu_verb;
}

LIST *menu_process_item_list(
			LIST *role_process_or_set_name_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame )
{
	LIST *menu_item_list;
	char *process_or_set_name;

	if ( ! list_rewind( role_process_or_set_name_list ) ) return (LIST *)0;

	menu_item_list = list_new();

	do {
		process_or_set_name =
			list_get(
				role_process_or_set_name_list );

		list_set(
			menu_item_list,
			menu_item_process_new(
				process_or_set_name,
				application_name,
				login_name,
				session_key,
				role_name,
				target_frame ) );

	} while ( list_next( role_process_or_set_name_list ) );

	return menu_item_list;
}

char *menu_verb_tag( char *verb )
{
	char tag[ 128 ];
	char buffer[ 64 ];

	if ( !verb || !*verb )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: verb is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(tag,
		"<a><span class=menu>%s</span></a>",
		format_initial_capital(
			buffer,
			verb ) );

	return strdup( tag );
}

char *menu_item_role_name_span_tag( char *role_name )
{
	char role_name_tag[ 128 ];
	char buffer[ 64 ];

	if ( !role_name || !*role_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: role_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(role_name_tag,
		"<a><span class=menu>%s</span></a>",
		format_initial_capital( buffer, role_name ) );

	return strdup( role_name_tag );
}

char *menu_item_role_change_action_tag(
			char *http_prompt,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name )
{
	char action_tag[ 1024 ];

	if ( !http_prompt || !*http_prompt )		return (char *)0;
	if ( !application_name || !*application_name )	return (char *)0;
	if ( !login_name || !*login_name )		return (char *)0;
	if ( !session_key || !*session_key )		return (char *)0;
	if ( !role_name || !*role_name )		return (char *)0;

	sprintf(action_tag,
"<a href=\"%s/post_choose_role_drop_down?%s+%s+%s+%s\">",
		http_prompt,
		application_name,
		login_name,
		session_key,
		role_name );

	return strdup( action_tag );
}

MENU_ITEM *menu_item_role_change_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *change_role_name )
{
	MENU_ITEM *menu_item = menu_item_calloc();

	menu_item->application_name = application_name;
	menu_item->login_name = login_name;
	menu_item->session_key = session_key;
	menu_item->role_name = change_role_name;

	menu_item->action_tag =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		menu_item_role_change_action_tag(
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
			application_name,
			login_name,
			session_key,
			change_role_name );

	if ( !menu_item->action_tag )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: menu_item_role_change_action_tag() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	menu_item->span_tag =
		menu_item_role_name_span_tag(
			change_role_name );

	return menu_item;
}

MENU_VERB *menu_verb_role_change_fetch(
			LIST *role_name_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name )
{
	MENU_VERB *menu_verb = menu_verb_calloc();

	menu_verb->item_list =
		menu_role_change_item_list(
			role_name_list,
			application_name,
			login_name,
			session_key );

	if ( !list_length( menu_verb->item_list ) )
		return (MENU_VERB *)0;

	menu_verb->menu_verb_tag =
		menu_verb_tag(
			role_name );

	return menu_verb;
}

LIST *menu_role_change_item_list(
			LIST *role_name_list,
			char *application_name,
			char *login_name,
			char *session_key )
{
	LIST *menu_item_list;
	char *change_role_name;

	if ( ! list_rewind( role_name_list ) ) return (LIST *)0;

	menu_item_list = list_new();

	do {
		change_role_name =
			list_get(
				role_name_list );

		list_set(
			menu_item_list,
			menu_item_role_change_new(
				application_name,
				login_name,
				session_key,
				change_role_name ) );

	} while ( list_next( role_name_list ) );

	return menu_item_list;
}

MENU_VERB *menu_verb_role_display( char *role_name )
{
	MENU_VERB *menu_verb = menu_verb_calloc();

	menu_verb->menu_verb_tag =
		menu_verb_tag(
			role_name );

	return menu_verb;
}


MENU_VERB *menu_verb_login_name_display( char *login_name )
{
	MENU_VERB *menu_verb = menu_verb_calloc();

	menu_verb->menu_verb_tag =
		menu_verb_tag(
			login_name );

	return menu_verb;
}

char *menu_horizontal_html(
			char *hide_preload_message,
			LIST *menu_verb_list )
{
	MENU_VERB *menu_verb;
	char html[ STRING_FOUR_MEG ];
	char *ptr = html;
	char buffer[ 128 ];

	if ( !list_rewind( menu_verb_list ) ) return (char *)0;

	if ( hide_preload_message && *hide_preload_message )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			hide_preload_message );
	}

	ptr += sprintf( ptr, "<ul id=menu>\n" );

	do {
		menu_verb = list_get( menu_verb_list );

		ptr += sprintf( ptr, "\t<li>\n" );
		ptr += sprintf( ptr, "\t%s\n", menu_verb->menu_verb_tag );

		if ( list_length( menu_verb->subschema_list ) )
		{
			ptr += sprintf( ptr, "\t<ul>\n" );

			ptr += sprintf(
				ptr,
				"%s\n",
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				menu_subschema_horizontal_html(
					menu_verb->subschema_list ) );

			ptr += sprintf( ptr, "\t</ul>\n" );
		}

		if ( list_length( menu_verb->item_list ) )
		{
			ptr += sprintf( ptr, "\t<ul>\n" );

			ptr += sprintf(
				ptr,
				"%s\n",
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				menu_item_horizontal_html(
					menu_verb->item_list ) );

			ptr += sprintf( ptr, "\t</ul>\n" );
		}

		if ( !list_length( menu_verb->subschema_list )
		&&   !list_length( menu_verb->item_list ) )
		{
			ptr += sprintf(
				ptr,
				"\t<li><label style=color:black>%s</label>\n",
				format_initial_capital(
					buffer,
					menu_verb->verb ) );
		}

	} while ( list_next( menu_verb_list ) );

	ptr += sprintf( ptr, "</ul>" );

	return strdup( html );
}

char *menu_subschema_horizontal_html(
			LIST *subschema_list )
{
	MENU_SUBSCHEMA *menu_subschema;
	static char html[ STRING_TWO_MEG ];
	char *ptr = html;

	if ( !list_rewind( subschema_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: subschema_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		menu_subschema = list_get( subschema_list );

		if ( !list_length( menu_subschema->item_list ) )
			continue;

		ptr += sprintf( ptr, "\t\t%s\n", menu_subschema->span_tag );
		ptr += sprintf( ptr, "\t\t<ul>\n" );

		ptr += sprintf(
			ptr,
			"%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			menu_item_horizontal_html(
				menu_subschema->item_list ) );

		ptr += sprintf( ptr, "\t\t</ul>\n" );

	} while ( list_next( subschema_list ) );

	return html;
}

char *menu_item_horizontal_html(
			LIST *menu_item_list )
{
	MENU_ITEM *menu_item;
	static char html[ STRING_ONE_MEG ];
	char *ptr = html;

	if ( !list_rewind( menu_item_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: menu_item_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		menu_item = list_get( menu_item_list );

		ptr += sprintf(
			ptr,
			"\t\t\t%s\n\t\t\t%s\n",
			menu_item->action_tag,
			menu_item->span_tag );

	} while ( list_next( menu_item_list ) );

	return html;
}

boolean menu_boolean(
			char *target_frame,
			boolean frameset_menu_horizontal )
{
	return ( frameset_menu_horizontal &&
		 strcmp( target_frame, FRAMESET_PROMPT_FRAME ) == 0 );
}

