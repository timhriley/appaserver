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
#include "appaserver_parameter.h"
#include "appaserver_library.h"
#include "appaserver_user.h"
#include "application.h"
#include "frameset.h"
#include "folder_menu.h"
#include "role.h"
#include "role_folder.h"
#include "post_choose_folder.h"
#include "post_choose_process.h"
#include "post_choose_role.h"
#include "choose_role.h"
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

char *menu_item_display( char *folder_process_role_name )
{
	char display[ 128 ];

	if ( !folder_process_role_name )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_process_role_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	return
	strdup(
		string_initial_capital(
			display,
			folder_process_role_name ) );
}

MENU_ITEM *menu_item_vertical_folder_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *folder_menu_count_display,
			boolean role_folder_insert_exists )
{
	MENU_ITEM *menu_item = menu_item_calloc();

	menu_item->lookup_action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_choose_folder_action_string(
			POST_CHOOSE_FOLDER_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			APPASERVER_LOOKUP_STATE,
			FRAMSET_PROMPT_FRAME );

	if ( role_folder_insert_exists )
	{
		menu_item->insert_action_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			post_choose_folder_action_string(
				POST_CHOOSE_FOLDER_EXECUTABLE,
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				APPASERVER_INSERT_STATE,
				FRAMSET_PROMPT_FRAME );
	}

	menu_item->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_item_vertical_folder_html(
			folder_menu_count_display,
			folder_name,
			menu_item->lookup_action_string,
			menu_item->insert_action_string );

	return menu_item;
}

MENU_ITEM *menu_item_horizontal_folder_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state,
			char *folder_menu_count_display )
{
	MENU_ITEM *menu_item = menu_item_calloc();

	menu_item->span_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_item_horizontal_folder_span_tag(
			folder_name,
			folder_menu_count_display );

	menu_item->action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_choose_folder_action_string(
			POST_CHOOSE_FOLDER_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			state,
			FRAMSET_PROMPT_FRAME );

	menu_item->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_item_horizontal_html(
			menu_item->span_tag,
			menu_item->action_string );

	return menu_item;
}

char *menu_item_horizontal_html(
			char *span_tag,
			char *action_tag )
{
	char html[ 1024 ];

	if ( !span_tag || !action_tag )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"<li><a %s>%s</a>",
		action_string,
		span_tag );

	return strdup( html );
}

MENU_ITEM *menu_item_vertical_process_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *role_process_or_set_name )
{
	MENU_ITEM *menu_item = menu_item_calloc();

	menu_item->action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_choose_process_action_string(
			POST_CHOOSE_PROCESS_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			process_or_set_name,
			FRAMESET_PROMPT_FRAME );

	menu_item->html =
		menu_item_vertical_process_html(
			process_or_set_name,
			menu_item->action_string );

	return menu_item;
}

MENU_ITEM *menu_item_horizontal_process_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name )
{
	MENU_ITEM *menu_item = menu_item_calloc();

	menu_item->span_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_item_horizontal_process_span_tag(
			process_or_set_name );

	menu_item->action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_choose_process_action_string(
			POST_CHOOSE_PROCESS_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			process_or_set_name,
			FRAMESET_PROMPT_FRAME );

	menu_item->html =
		menu_item_horizontal_html(
			menu_item->span_tag,
			menu_item->action_string );

	return menu_item;
}

char *menu_item_folder_action_tag(
			char *http_prompt,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state,
			char *target_frame )
{
	char action_tag[ 1024 ];

	if ( !http_prompt
	||   !
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state,
			char *target_frame )

	sprintf(action_tag,
"<a href=\"%s/post_choose_folder?%s+%s+%s+%s+%s+%s\" target=\"%s\">",
		http_prompt,
		application_name,
		session_key,
		login_name,
		role_name,
		folder_name,
		state,
		target_frame );

	return strdup( action_tag );
}

char *menu_item_process_action_tag(
			char *http_prompt,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *role_process_or_set_name,
			char *frameset_prompt_frame )
{
	char action_tag[ 1024 ];

	if ( !http_prompt
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !role_process_or_set_name
	||   !frameset_prompt_frame )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(action_tag,
"href=\"%s/post_choose_process?%s+%s+%s+%s+%s\" target=\"%s\">",
		http_prompt,
		application_name,
		session_key,
		login_name,
		role_name,
		role_process_or_set_name,
		frameset_prompt_frame );

	return strdup( action_tag );
}

char *menu_horizontal_span_tag( char *item_name )
{
	char buffer[ 128 ];
	static char span_tag[ 256 ];

	if ( !item_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: item_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(span_tag,
		"<span class=menu>%s</span>",
		format_initial_capital(
			buffer,
			item_name ) );

	return span_tag;
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

MENU *menu_new(		char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *target_frame,
			LIST *folder_menu_count_list )
{
	MENU *menu = menu_calloc();

	menu->menu_verb_list = list_new();

	menu->role_folder_lookup_list = role_folder_lookup_list( role_name );

	list_set(
		menu->menu_verb_list,
		menu_verb_folder_new(
			"lookup" /* verb */,
			menu->role_folder_lookup_list
				/* role_folder_list */,
			folder_menu_count_list,
			application_name,
			session_key,
			login_name,
			role_name,
			target_frame ) );

	menu->role_folder_insert_list = role_folder_insert_list( role_name );

	list_set(
		menu->menu_verb_list,
		menu_verb_folder_new(
			"insert" /* verb */,
			menu->role_folder_insert_list
				/* role_folder_list */,
			(LIST *)0 /* folder_menu_count_list */,
			application_name,
			session_key,
			login_name,
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
		char *process_group_name;

		do {
			process_group_name =
				list_get(
					menu->menu_process_group_name_list );

			list_set(
				menu->menu_verb_list,
				menu_verb_process_new(
					process_group_name,
					menu->role_process_list,
					menu->role_process_set_list,
					application_name,
					session_key,
					login_name,
					role_name,
					target_frame ) );

		} while ( list_next( menu->menu_process_group_name_list ) );
	}

	menu->role_name_list =
		role_name_list(
			role_list_fetch(
				login_name ) );

	if ( list_length( menu->role_name_list ) > 1 )
	{
		list_set(
			menu->menu_heading_list,
			menu_heading_role_change_new(
				menu->role_name_list,
				application_name,
				session_key,
				login_name,
				role_name,
				target_frame ) );
	}

	list_set(
		menu->menu_verb_list,
		menu_verb_role_display_new( role_name ) );

	list_set(
		menu->menu_verb_list,
		menu_verb_login_display_new( login_name ) );

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

MENU_VERB *menu_verb_folder_new(
			char *verb,
			LIST *role_folder_list,
			LIST *folder_menu_count_list,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *target_frame )
{
	MENU_VERB *menu_verb = menu_verb_calloc();

	menu_verb->role_folder_subschema_name_list =
		role_folder_subschema_name_list(
			role_folder_list );

	if ( list_rewind( menu_verb->role_folder_subschema_name_list ) )
	{
		char *subschema_name;

		menu_verb->menu_subschema_list = list_new();

		do {
			subschema_name =
				list_get(
					menu_verb->
					     role_folder_subschema_name_list );

			list_set(
				menu_verb->menu_subschema_list,
				menu_subschema_new(
					subschema_name,
					verb /* state */,
					role_folder_list,
					folder_menu_count_list,
					application_name,
					session_key,
					login_name,
					role_name,
					target_frame ) );

		} while ( list_next( 
				menu_verb->
					role_folder_subschema_name_list ) );
	}

	menu_verb->role_folder_subschema_missing_folder_name_list =
		role_folder_subschema_missing_folder_name_list(
			role_folder_list );

	if ( list_rewind(
		menu_verb->
			role_folder_subschema_missing_folder_name_list ) )
	{
		char *folder_name;

		menu_verb->menu_item_folder_list = list_new();

		do {
			folder_name =
			   list_get(
			     menu_verb->
			       role_folder_subschema_missing_folder_name_list );

			list_set(
				menu_verb->menu_item_folder_list,
				menu_item_folder_new(
					folder_menu_count_list,
					application_name,
					session_key,
					login_name,
					role_name,
					folder_name,
					verb /* state */,
					target_frame ) );

		} while (
		   list_next( 
		     menu_verb->
			role_folder_subschema_missing_folder_name_list ) );
	}

	if ( !list_length( menu_verb->menu_subschema_list )
	&&   !list_length( menu_verb->menu_item_folder_list ) )
	{
		return (MENU_VERB *)0;
	}

	/* Returns heap memory */
	/* ------------------- */
	menu_verb->tag = menu_verb_tag( verb );

	return menu_verb;
}

MENU_SUBSCHEMA *menu_subschema_new(
			char *subschema_name,
			char *state,
			LIST *role_folder_list,
			LIST *folder_menu_count_list,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *target_frame )
{
	char *folder_name;
	MENU_SUBSCHEMA *menu_subschema = menu_subschema_calloc();

	menu_subschema->subschema_name = subschema_name;

	menu_subschema->role_folder_subschema_folder_name_list =
		role_folder_subschema_folder_name_list(
			subschema_name,
			role_folder_list );

	if ( !list_rewind(
		menu_subschema->
			role_folder_subschema_folder_name_list ) )
	{
		free( menu_subschema );
		return (MENU_SUBSCHEMA *)0;
	}

	menu_subschema->menu_item_list = list_new();

	do {
		folder_name =
			list_get(
				menu_subschema->
				       role_folder_subschema_folder_name_list );

		list_set(
			menu_subschema->menu_item_list,
			menu_item_folder_new(
				folder_menu_count_list,
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				state,
				target_frame ) );
	} while ( list_next(
			menu_subschema->
			       role_folder_subschema_folder_name_list ) );

	if ( !list_length( menu_subschema->menu_item_list ) )
		return (MENU_SUBSCHEMA *)0;

	menu_subschema->horizontal_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_subschema_horizontal_tag(
			subschema_name );

	return menu_subschema;
}

char *menu_item_horizontal_folder_span_tag(
			char *folder_name,
			char *folder_menu_count_display )
{
	char span_tag[ 128 ];
	char *ptr = span_tag;
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


	ptr += sprintf(
		ptr,
		"<span class=menu>%s",
		format_initial_capital( buffer, folder_name ) );

	if ( folder_menu_count_display && *folder_menu_count_display )
	{
		ptr += sprintf( ptr, " %s", folder_menu_count_display );
	}

	ptr += sprintf( ptr, "</span>" );

	return strdup( span_tag );
}

char *menu_item_horizontal_process_span_tag(
			char *process_or_set_name )
{
	char span_tag[ 128 ];
	char buffer[ 64 ];

	if ( !process_or_set_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: process_or_set_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(span_tag,
		"<span class=menu>%s</span>",
		format_initial_capital(
			buffer,
			process_or_set_name ) );

	return strdup( span_tag );
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

MENU_VERB *menu_verb_process_new(
			char *process_group_name,
			LIST *role_process_list,
			LIST *role_process_set_list,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *target_frame )
{
	MENU_VERB *menu_verb = menu_verb_calloc();

	menu_verb->menu_item_process_list = list_new();

	menu_verb->role_process_or_set_name_list =
		role_process_or_set_name_list(
			process_group_name,
			role_process_list,
			role_process_set_list );

	if ( !list_rewind( menu_verb->role_process_or_set_name_list ) )
	{
		free( menu_verb );
		return (MENU_VERB *)0;
	}

	do {
		list_set(
			menu_verb->menu_item_process_list,
			menu_item_process_new(
				application_name,
				login_name,
				session_key,
				role_name,
				list_get(
					menu_verb->
						role_process_or_set_name_list ),
				target_frame ) );

	} while ( list_next(
			menu_verb->role_process_or_set_name_list ) );

	/* Returns heap memory */
	/* ------------------- */
	menu_verb->tag = menu_verb_tag( process_group_name );

	return menu_verb;
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

LIST *menu_verb_role_name_list(
			char *role_name,
			LIST *role_name_list )
{
	LIST *name_list = list_new();
	char *name;

	if ( !list_rewind( role_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: role_name_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		name = list_get( role_name_list );

		if ( strcmp( name, role_name ) == 0 ) continue;

		list_set( name_list, name );

	} while ( list_next( role_name_list ) );

	return name_list;
}

MENU_HEADING *menu_heading_role_change_new(
			LIST *role_name_list,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name )
{
	MENU_HEADING *menu_heading = menu_heading_calloc();

	menu_heading->role_name_list =
		menu_heading_role_name_list(
			role_name,
			role_name_list );

	if ( !list_rewind( menu_heading->role_name_list ) )
	{
		free( menu_heading );
		return (MENU_HEADING *)0;
	}

	menu_heading->menu_item_horizontal_role_change_list = list_new();

	do {
		list_set(
			menu_heading->menu_item_horizontal_role_change_list,
			menu_item_role_horizontal_change_new(
				application_name,
				session_key,
				login_name,
				list_get(
					menu_heading->
						role_name_list ) ) );

	} while ( list_next( menu_heading->role_name_list ) );

	/* Returns heap memory */
	/* ------------------- */
	menu_heading->tag = menu_verb_tag( "role" );

	return menu_heading;
}

char *menu_item_horizontal_role_change_span_tag( char *role_name )
{
	char span_tag[ 128 ];
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

	sprintf(span_tag,
		"<span class=menu>%s</span>",
		format_initial_capital(
			buffer,
			role_name ) );

	return strdup( span_tag );
}

char *menu_item_horizontal_role_change_action_string(
			char *http_prompt,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *frameset_prompt_frame )
{
	char action_string[ 1024 ];

	if ( !http_prompt
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

	sprintf(action_tag,
"<a href=\"%s/post_role_change?%s+%s+%s+%s\" target=%s>",
		http_prompt,
		application_name,
		login_name,
		session_key,
		role_name,
		target_frame );

	return strdup( action_tag );
}

MENU_ITEM *menu_item_role_change_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *target_frame )
{
	MENU_ITEM *menu_item = menu_item_calloc();

	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	menu_item->display = menu_item_display( role_name );

	menu_item->span_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_item_role_name_span_tag(
			role_name );

	menu_item->action_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_item_role_change_action_tag(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			appaserver_library_http_prompt(
				appaserver_parameter_cgi_directory(),
				appaserver_library_server_address(),
				application_ssl_support_yn(
					application_name ),
				application_prepend_http_protocol_yn(
					application_name ) ),
			application_name,
			login_name,
			session_key,
			role_name,
			target_frame );

	return menu_item;
}

char *menu_subschema_horizontal_html(
			char *tag,
			LIST *menu_item_list )
{
	char html[ STRING_ONE_MEG ];
	char *ptr = html;
	char *tmp;

	if ( !tag )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: tag is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( menu_item_list ) ) return (char *)0;

	ptr += sprintf( ptr, "\t\t%s\n", tag );

	ptr += sprintf( ptr, "\t\t<ul>\n" );

	if ( ( tmp =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		menu_item_list_html(
			menu_item_list ) ) )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			tmp );

		free( tmp );
	}

	ptr += sprintf( ptr, "\t\t</ul>\n" );

	return strdup( html );
}

char *menu_item_list_horizontal_html( LIST *menu_item_list )
{
	MENU_ITEM *menu_item;
	char html[ STRING_128K ];
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

	return strdup( html );
}

boolean menu_boolean(	char *current_frame,
			boolean frameset_menu_horizontal )
{
	return ( frameset_menu_horizontal &&
		 string_strcmp(
			current_frame,
			FRAMESET_PROMPT_FRAME ) == 0 );
}

char *menu_output_system_string(
			char *menu_output_executable,
			char *session_key,
			char *login_name,
			char *role_name,
			boolean frameset_menu_horizontal,
			char *output_filename )
{
	char system_string[ 1024 ];

	if ( !menu_output_executable
	||   !session_key
	||   !login_name
	||   !role_name
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
		"%s %s %s %s %c > %s",
		menu_output_executable,
		session_key,
		login_name,
		role_name,
		(frameset_menu_horizontal) ? 'y' : 'n',
		output_filename );

	return strdup( system_string );
}

char *menu_horizontal_html(
			char *hide_preload_html,
			LIST *menu_verb_list )
{
	MENU_VERB *menu_verb;
	char html[ STRING_TWO_MEG ];
	char *ptr = html;
	char *tmp;

	if ( !list_rewind( menu_verb_list ) ) return (char *)0;

	if ( !hide_preload_html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: hide_preload_html is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"%s\n",
		hide_preload_html );

	ptr += sprintf( ptr, "<ul id=menu>\n" );

	do {
		menu_verb = list_get( menu_verb_list );

		ptr += sprintf( ptr, "\t<li>\n" );
		ptr += sprintf( ptr, "\t%s\n", menu_verb->tag );

		if ( list_length( menu_verb->menu_subschema_list ) )
		{
			ptr += sprintf( ptr, "\t<ul>\n" );

			tmp =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				menu_subschema_list_horizontal_html(
					menu_verb->menu_subschema_list );

			ptr += sprintf(
				ptr,
				"%s\n",
				tmp );

			free( tmp );

			ptr += sprintf( ptr, "\t</ul>\n" );
		}
		else
		if ( list_length( menu_verb->menu_item_folder_list ) )
		{
			ptr += sprintf( ptr, "\t<ul>\n" );

			tmp =
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
				menu_item_list_html(
					menu_heading->menu_item_folder_list );

			ptr += sprintf(
				ptr,
				"%s\n",
				tmp );

			free( tmp );

			ptr += sprintf( ptr, "\t</ul>\n" );
		}
		else
		if ( list_length( menu_heading->menu_item_process_list ) )
		{
			ptr += sprintf( ptr, "\t<ul>\n" );

			tmp =
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
				menu_item_list_html(
					menu_heading->menu_item_process_list );

			ptr += sprintf(
				ptr,
				"%s\n",
				tmp );

			free( tmp );

			ptr += sprintf( ptr, "\t</ul>\n" );
		}
		else
		if ( list_length( menu_heading->menu_item_role_change_list ) )
		{
			ptr += sprintf( ptr, "\t<ul>\n" );

			tmp =
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
				menu_item_list_html(
					menu_heading->
						menu_item_role_change_list );

			ptr += sprintf(
				ptr,
				"%s\n",
				tmp );

			free( tmp );

			ptr += sprintf( ptr, "\t</ul>\n" );
		}
		else
		if ( menu_verb->tag )
		{
			ptr += sprintf( ptr, "%s\n", menu_verb->tag );
		}

	} while ( list_next( menu_verb_list ) );

	ptr += sprintf( ptr, "</ul>" );

	return strdup( html );
}

char *menu_vertical_html( LIST *menu_verb_list )
{
	MENU_VERB *menu_verb;
	char html[ STRING_ONE_MEG ];
	char *ptr = html;

	if ( !list_rewind( menu_verb_list ) ) return (char *)0;

	ptr += sprintf( ptr,
"\t<table border>\n"
"\t<th colspan=3><h2 align=center>Data</h2></th>\n" );

	do {
		menu_verb = list_get( menu_verb_list );

		ptr += sprintf ( ptr, "\t<tr>\n" );

/*
		ptr += sprintf( ptr,
"<td><A class=%s HREF=\"%s/post_choose_folder?"
				"%s+%s+%s+%s+%s+%s\""
				" target=%s>%s</A></td>\n",
				VERTICAL_MENU_CLASS,
				appaserver_library_http_prompt(
					apache_cgi_directory,
					server_address,
					application_ssl_support_yn(
						application_name ),
				       application_prepend_http_protocol_yn(
						application_name ) ),
				 login_name,
				 application_name,
				 session,
				 folder->folder_name, 
				 role_name,
				 state,
				 target_frame,
				 format_initial_capital(
						buffer, 
				 		folder->folder_name ) );
*/

	} while ( list_next( menu_verb_list ) );

	return strdup( html );
}

MENU_VERB *menu_verb_role_display_new( char *role_name )
{
	MENU_VERB *menu_verb = menu_verb_calloc();

	/* Returns heap memory */
	/* ------------------- */
	menu_verb->tag = menu_verb_item_tag( role_name );

	return menu_verb;
}

MENU_VERB *menu_verb_login_display_new( char *login_name )
{
	MENU_VERB *menu_verb = menu_verb_calloc();

	/* Returns heap memory */
	/* ------------------- */
	menu_verb->tag = menu_verb_item_tag( login_name );

	return menu_verb;
}

char *menu_verb_item_tag( char *verb )
{
	char tag[ 128 ];
	char buffer[ 64 ];

	if ( !verb )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: verb is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(tag,
		"<li><a><label style=color:black>%s</label></a>",
		string_initial_capital(
			buffer,
			verb ) );

	return strdup( tag );
}

char *menu_subschema_horizontal_tag(
			char *subschema_name )
{
	char tag[ 128 ];
	char buffer[ 64 ];

	if ( !subschema_name || !*subschema_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: subschema_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(tag,
		"<a><span class=menu>%s</span></a>",
		format_initial_capital(
			buffer,
			subschema_name ) );

	return strdup( tag );
}

char *menu_subschema_list_horizontal_html(
			LIST *menu_subschema_list )
{
	char html[ STRING_TWO_MEG ];
	char *ptr = html;
	MENU_SUBSCHEMA *menu_subschema;
	char *tmp;

	if ( !list_rewind( menu_subschema_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: menu_subschema_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf( ptr, "\t<ul>\n" );

	do {
		menu_subschema =
			list_get(
				menu_subschema_list );

		if ( list_length( menu_subschema->menu_item_list ) )
		{
			if ( ( tmp =
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
				menu_subschema_horizontal_html(
					menu_subschema->horizontal_tag,
					menu_subschema->menu_item_list ) ) )
			{
				ptr += sprintf(
					ptr,
					"\t\t%s\n",
					tmp );

				free( tmp );
			}
		}

	} while ( list_next( menu_subschema_list ) );

	ptr += sprintf( ptr, "\t</ul>\n" );

	return strdup( html );
}

char *menu_item_vertical_folder_html(
			char *folder_menu_count_display,
			char *folder_name,
			char *lookup_action_string,
			char *insert_action_string )
{
	char html[ 1024 ];
	char *ptr = html;
	char buffer[ 128 ];

	if ( !menu_item_display || !lookup_action_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	ptr += sprintf(
		ptr,
		"<tr><td><a class=vertical_menu %s>%s</a>",
		lookup_action_string,
		string_initial_capital( buffer, folder_name ) );

	if ( folder_menu_count_display )
	{
		ptr += sprintf(
			ptr,
			"<td align=right>%s",
			folder_menu_count_display );
	}

	if ( insert_action_string )
	{
		ptr += sprintf(
			ptr,
			"<td><a class=vertical_menu %s>New</a>",
			insert_action_string );
	}

	return strdup( html );
}

char *menu_item_list_html( LIST *menu_item_list )
{
	char html[ STRING_64K ];
	char *ptr = html;
	MENU_ITEM *menu_item;

	if ( !list_rewind( menu_item_list ) ) return (char *)0;

	do {
		menu_item = list_get( menu_item_list );

		if ( ptr != html ) ptr += sprintf( ptr, "\n" );

		if ( !menu_item->html )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: html is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		ptr += sprintf( ptr, "%s", menu_item->html );

	} while ( list_next( menu_item_list ) );

	return strdup( html );
}

char *menu_item_vertical_process_html(
			char *process_or_set_name,
			char *action_string )
{
	char html[ 1024 ];
	char buffer[ 128 ];

	if ( !process_or_set_name || !action_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"<tr><td><a class=vertical_menu %s>%s</a>",
		action_string,
		string_initial_capital( buffer, process_or_set_name );

	return strdup( html );
}

MENU_ITEM *menu_item_vertical_role_change_new(
			char *application_name,
			char *session_key,
			char *login_name,
			LIST *role_name_list )
{
	MENU_ITEM *menu_item = menu_item_calloc();

	if ( !list_length( role_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: role_name_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	menu_item->action_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_choose_role_action_string(
			POST_CHOOSE_ROLE_EXECUTABLE,
			application_name,
			session_key,
			login_name );

	menu_item->form_choose_role =
		form_choose_role_new(
			role_name_list,
			menu_item->action_string,
			FRAMESET_MENU_FRAME /* target_frame */,
			CHOOSE_ROLE_FORM_NAME,
			CHOOSE_ROLE_DROP_DOWN_ELEMENT_NAME );

	if ( !menu_item->form_choose_role )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: form_choose_role_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !menu_item->form_choose_role->html )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: form_choose_role->html is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	menu_item->html = menu_item->form_choose_role->html;

	return menu_item;
}

MENU_PROCESS_GROUP *menu_process_group_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name ,
			char *group_name,
			LIST *role_process_list,
			LIST *role_process_set_list,
			boolean frameset_menu_horizontal )
{
	char *process_or_set_name;

	MENU_PROCESS_GROUP *menu_process_group =
		menu_process_group_calloc();

	menu_process_group->role_process_or_set_name_list =
		role_process_or_set_name_list(
			group_name,
			role_process_list,
			role_process_set_list );

	if ( !list_rewind(
		menu_process_group->role_process_or_set_name_list ) )
	{
		free( menu_process_group );
		return (MENU_PROCESS_GROUP *)0;
	}

	menu_process_group->menu_item_process_list = list_new();

	do {
		process_or_set_name =
			list_get(
				menu_process_group->
					role_process_or_set_name_list );

		list_set(
			menu_process_group->menu_item_process_list,
			menu_item_process_new(
				application_name,
				session_key,
				login_name,
				role_name,
				process_or_set_name,
				frameset_menu_horizontal ) );

	} while ( list_next( 
			menu_process_group->
				role_process_or_set_name_list ) );

	if ( !list_length( menu_process_group->menu_item_process_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: menu_item_process_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( frameset_menu_horizontal )
	{
		menu_process_group->menu_horizontal_span_tag =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			menu_horizontal_span_tag( group_name );

		menu_process_group->html =
			menu_item_list_horizontal_html(
				menu_process_group->menu_horizontal_span_tag,
				menu_process_group->menu_item_process_list );
	}
	else
	{
		menu_process_group->html =
			menu_item_list_vertical_html(
				group_name,
				menu_process_group->menu_item_process_list );
	}

	if ( !menu_process_group->html )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: menu_process_group->html is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return menu_process_group;
}

char *menu_item_list_vertical_html(
			char *item_name,
			LIST *menu_item_list )
{
	char html[ STRING_64K ];
	char *ptr = html;
	char buffer[ 64 ];
	char *tmp;

	if ( !item_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: item_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( menu_item_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		"<table border=0><th align=center>%s\n",
		string_initial_capital(
			buffer,
			item_name ) );

	if ( ( tmp =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		menu_item_list_html(
			menu_item_list ) ) )
	{
		ptr += sprintf( "%s", tmp );
		free( tmp );
	}

	ptr += sprintf( ptr, "</table" );

	return strdup( html );
}

char *menu_item_list_horizontal_html(
			char *menu_horizontal_span_tag,
			LIST *menu_item_list )
{
	char html[ STRING_64K ];
	char *ptr = html;
	char *tmp;

	if ( !list_length( menu_item_list ) ) return (char *)0;

	if ( !menu_horizontal_span_tag )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: menu_horizontal_span_tag is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf( ptr, "\t\t%s\n", menu_horizontal_span_tag );

	ptr += sprintf( ptr, "\t\t<ul>\n" );

	if ( ( tmp =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		menu_item_list_html(
			menu_item_list ) ) )
	{
		ptr += sprintf(
			ptr,
			"%s\n",
			tmp );

		free( tmp );
	}

	ptr += sprintf( ptr, "\t\t</ul>\n" );

	return strdup( html );
}

MENU_PROCESS_GROUP *menu_process_group_calloc( void )
{
	MENU_PROCESS_GROUP *menu_process_group;

	if ( ! ( menu_process_group =
			calloc( 1, sizeof( MENU_PROCESS_GROUP ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return menu_process_group;
}

MENU_HORIZONTAL_SUBSCHEMA *menu_horizontal_subschema_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			LIST *folder_menu_count_list,
			LIST *role_folder_list,
			char *subschema_name,
			char *state )
{
}

MENU_HORIZONTAL_SUBSCHEMA *menu_horizontal_subschema_calloc( void )
{
}

MENU_VERTICAL_SUBSCHEMA *menu_vertical_subschema_new(
			char *application_name,
			char *session_key,
			char *login_namespan_,
			char *role_name,
			LIST *folder_menu_count_list,
			LIST *role_folder_lookup_list,
			LIST *role_folder_insert_list,
			char *subschema_name )
{
}

MENU_VERTICAL_SUBSCHEMA *menu_vertical_subschema_calloc( void )
{
}
