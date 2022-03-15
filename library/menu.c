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

MENU_ITEM *menu_item_folder_new(
			LIST *folder_menu_count_list,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state,
			char *target_frame )
{
	MENU_ITEM *menu_item = menu_item_calloc();
	char *folder_menu_count_display = {0};
	FOLDER_MENU_COUNT *folder_menu_count;

	if ( list_length( folder_menu_count_list ) )
	{
		if ( ! ( folder_menu_count =
				folder_menu_count_seek(
					folder_name,
					folder_menu_count_list ) ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: folder_menu_count_seek(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				folder_name );
			exit( 1 );
		}

		folder_menu_count_display = folder_menu_count->display;
	}

	menu_item->display =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_item_display(
			folder_name );

	menu_item->span_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_item_folder_span_tag(
			folder_name,
			folder_menu_count_display );

	menu_item->action_tag =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		menu_item_folder_action_tag(
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
			session_key,
			login_name,
			role_name,
			folder_name,
			state,
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

	return menu_item;
}

MENU_ITEM *menu_item_process_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *role_process_or_set_name,
			char *target_frame )
{
	MENU_ITEM *menu_item = menu_item_calloc();

	menu_item->display =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_item_display(
			role_process_or_set_name );

	menu_item->span_tag =
		menu_item_process_span_tag(
			role_process_or_set_name );

	menu_item->action_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_item_process_action_tag(
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
			role_process_or_set_name,
			target_frame );

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

	if ( !http_prompt || !*http_prompt )		return (char *)0;
	if ( !application_name || !*application_name )	return (char *)0;
	if ( !session_key || !*session_key )		return (char *)0;
	if ( !login_name || !*login_name )		return (char *)0;
	if ( !role_name || !*role_name )		return (char *)0;
	if ( !folder_name || !*folder_name )		return (char *)0;
	if ( !state || !*state )			return (char *)0;
	if ( !target_frame || !*target_frame )		return (char *)0;

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
			char *target_frame )
{
	char action_tag[ 1024 ];

	if ( !http_prompt
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !role_process_or_set_name
	||   !target_frame )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	sprintf(action_tag,
"<a href=\"%s/post_choose_process?%s+%s+%s+%s+%s\" target=\"%s\">",
		http_prompt,
		application_name,
		session_key,
		login_name,
		role_name,
		role_process_or_set_name,
		target_frame );

	return strdup( action_tag );
}

char *menu_item_span_tag( char *item_name )
{
	char buffer[ 128 ];
	char span_tag[ 256 ];

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
		"<span class=menu>%s</span></a>",
		format_initial_capital(
			buffer,
			item_name ) );

	return strdup( span_tag );
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
			menu->menu_verb_list,
			menu_verb_role_change_new(
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

MENU_VERB *menu_verb_role_change_new(
			LIST *role_name_list,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *target_frame )
{
	MENU_VERB *menu_verb = menu_verb_calloc();

	menu_verb->role_name_list =
		menu_verb_role_name_list(
			role_name,
			role_name_list );

	if ( !list_rewind( menu_verb->role_name_list ) )
	{
		free( menu_verb );
		return (MENU_VERB *)0;
	}

	menu_verb->menu_item_role_change_list = list_new();

	do {
		list_set(
			menu_verb->menu_item_role_change_list,
			menu_item_role_change_new(
				application_name,
				session_key,
				login_name,
				list_get(
					menu_verb->
						role_name_list ),
				target_frame ) );

	} while ( list_next( menu_verb->role_name_list ) );

	/* Returns heap memory */
	/* ------------------- */
	menu_verb->tag = menu_verb_tag( "role" );

	return menu_verb;
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
			char *session_key,
			char *login_name,
			char *role_name,
			char *target_frame )
{
	char action_tag[ 1024 ];

	if ( !http_prompt
	||   !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !target_frame )
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

	if ( !list_length( menu_item_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: menu_item_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf( ptr, "\t\t%s\n", tag );

	ptr += sprintf( ptr, "\t\t<ul>\n" );

	tmp =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_item_list_horizontal_html(
			menu_item_list );

	ptr += sprintf(
		ptr,
		"%s\n",
		tmp );

	free( tmp );

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
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				menu_item_list_horizontal_html(
					menu_verb->menu_item_folder_list );

			ptr += sprintf(
				ptr,
				"%s\n",
				tmp );

			free( tmp );

			ptr += sprintf( ptr, "\t</ul>\n" );
		}
		else
		if ( list_length( menu_verb->menu_item_process_list ) )
		{
			ptr += sprintf( ptr, "\t<ul>\n" );

			tmp =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				menu_item_list_horizontal_html(
					menu_verb->menu_item_process_list );

			ptr += sprintf(
				ptr,
				"%s\n",
				tmp );

			free( tmp );

			ptr += sprintf( ptr, "\t</ul>\n" );
		}
		else
		if ( list_length( menu_verb->menu_item_role_change_list ) )
		{
			ptr += sprintf( ptr, "\t<ul>\n" );

			tmp =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				menu_item_list_horizontal_html(
					menu_verb->
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
			tmp =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				menu_subschema_horizontal_html(
					menu_subschema->horizontal_tag,
					menu_subschema->menu_item_list );

			ptr += sprintf(
				ptr,
				"\t\t%s\n",
				tmp );

			free( tmp );
		}

	} while ( list_next( menu_subschema_list ) );

	ptr += sprintf( ptr, "\t</ul>\n" );

	return strdup( html );
}
