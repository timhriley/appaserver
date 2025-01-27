/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/menu.c					*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "appaserver.h"
#include "appaserver_parameter.h"
#include "appaserver_user.h"
#include "appaserver_error.h"
#include "application.h"
#include "folder_menu.h"
#include "role.h"
#include "role_folder.h"
#include "post_choose_folder.h"
#include "post_choose_process.h"
#include "post_choose_role.h"
#include "choose_role.h"
#include "javascript.h"
#include "menu.h"

MENU_ITEM *menu_item_calloc( void )
{
	MENU_ITEM *menu_item;

	if ( ! ( menu_item = calloc( 1, sizeof ( MENU_ITEM ) ) ) )
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

MENU_ITEM *menu_item_vertical_folder_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *folder_menu_count_display,
		boolean role_folder_insert_exists )
{
	MENU_ITEM *menu_item;
	char *lookup_anchor_tag;
	char *insert_anchor_tag = {0};

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	menu_item = menu_item_calloc();

	menu_item->action_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_choose_folder_action_string(
			POST_CHOOSE_FOLDER_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			APPASERVER_LOOKUP_STATE,
			folder_name );

	lookup_anchor_tag =
		strdup(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			menu_item_anchor_tag(
				FRAMESET_MENU_FRAME /* target_frame */,
				menu_item->action_string ) );

	if ( role_folder_insert_exists )
	{
		menu_item->action_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			post_choose_folder_action_string(
				POST_CHOOSE_FOLDER_EXECUTABLE,
				application_name,
				session_key,
				login_name,
				role_name,
				APPASERVER_INSERT_STATE,
				folder_name );

		insert_anchor_tag =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			menu_item_anchor_tag(
				FRAMESET_MENU_FRAME /* target_frame */,
				menu_item->action_string );
	}

	menu_item->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_item_vertical_folder_html(
			folder_name,
			folder_menu_count_display,
			lookup_anchor_tag,
			insert_anchor_tag );

	free( lookup_anchor_tag );

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
	MENU_ITEM *menu_item;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !state )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	menu_item = menu_item_calloc();

	menu_item->span_tag =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		menu_item_horizontal_folder_span_tag(
			MENU_ITEM_HORIZONTAL_CLASS_NAME,
			folder_name,
			folder_menu_count_display );

	menu_item->action_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_choose_folder_action_string(
			POST_CHOOSE_FOLDER_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			state,
			folder_name );

	menu_item->anchor_tag =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		menu_item_anchor_tag(
			FRAMESET_PROMPT_FRAME /* target_frame */,
			menu_item->action_string );

	menu_item->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_item_horizontal_html(
			menu_item->span_tag,
			menu_item->anchor_tag );

	return menu_item;
}

MENU_ITEM *menu_item_vertical_process_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_or_set_name )
{
	MENU_ITEM *menu_item;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_or_set_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	menu_item = menu_item_calloc();

	menu_item->span_tag =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		menu_item_span_tag(
			MENU_ITEM_VERTICAL_CLASS_NAME,
			process_or_set_name /* item */ );

	menu_item->action_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_choose_process_action_string(
			POST_CHOOSE_PROCESS_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name );

	menu_item->href_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		menu_item_href_string(
			process_or_set_name /* item */,
			menu_item->action_string );

	menu_item->anchor_tag =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		menu_item_anchor_tag(
			FRAMESET_MENU_FRAME /* target_frame */,
			menu_item->href_string );

	menu_item->html =
		strdup(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			menu_item_vertical_html(
				menu_item->span_tag,
				menu_item->anchor_tag,
				1 /* table_row_boolean */ ) );

	return menu_item;
}

MENU_ITEM *menu_item_horizontal_process_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_or_set_name )
{
	MENU_ITEM *menu_item;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_or_set_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	menu_item = menu_item_calloc();

	menu_item->span_tag =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		menu_item_span_tag(
			MENU_ITEM_HORIZONTAL_CLASS_NAME,
			process_or_set_name /* item */ );

	menu_item->action_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_choose_process_action_string(
			POST_CHOOSE_PROCESS_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name );

	menu_item->href_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		menu_item_href_string(
			process_or_set_name /* item */,
			menu_item->action_string );

	menu_item->anchor_tag =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		menu_item_anchor_tag(
			FRAMESET_PROMPT_FRAME /* target_frame */,
			menu_item->href_string );

	menu_item->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_item_horizontal_html(
			menu_item->span_tag,
			menu_item->anchor_tag );

	return menu_item;
}

char *menu_item_span_tag(
		char *class_name,
		char *item )
{
	static char span_tag[ 256 ];
	char buffer[ 256 ];

	if ( !class_name
	||   !item )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(span_tag,
		"<span class=%s>%s</span>",
		class_name,
		string_initial_capital(
			buffer,
			item ) );

	return span_tag;
}

MENU *menu_calloc( void )
{
	MENU *menu;

	if ( ! ( menu = calloc( 1, sizeof ( MENU ) ) ) )
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

MENU *menu_new( char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		boolean application_menu_horizontal_boolean,
		LIST *folder_menu_count_list )
{
	MENU *menu;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	menu = menu_calloc();

	menu->role_folder_lookup_list = role_folder_lookup_list( role_name );
	menu->role_folder_insert_list = role_folder_insert_list( role_name );

	menu->role_process_list =
		role_process_list(
			role_name,
			1 /* fetch_process */ );

	menu->role_process_set_member_list =
		role_process_set_member_list(
			role_name,
			1 /* fetch_process_set */ );

	menu->appaserver_user_role_name_list =
		appaserver_user_role_name_list(
			login_name );

	menu->role_process_or_set_group_name_list =
		role_process_or_set_group_name_list(
			menu->role_process_list,
			menu->role_process_set_member_list );

	menu->role_folder_subschema_name_list =
		role_folder_subschema_name_list(
			menu->role_folder_lookup_list );

	menu->role_folder_subschema_missing_folder_name_list =
		role_folder_subschema_missing_folder_name_list(
			menu->role_folder_lookup_list );

	menu->role_process_or_set_missing_group_name_list =
		role_process_or_set_missing_group_name_list(
			menu->role_process_list,
			menu->role_process_set_member_list );

	if ( !application_menu_horizontal_boolean )
	{
		menu->menu_vertical =
		     menu_vertical_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_menu_count_list,
			menu->role_folder_lookup_list,
			menu->role_folder_insert_list,
			menu->role_process_list,
			menu->role_process_set_member_list,
			menu->appaserver_user_role_name_list,
			menu->role_folder_subschema_name_list,
			menu->role_folder_subschema_missing_folder_name_list,
			menu->role_process_or_set_group_name_list,
			menu->role_process_or_set_missing_group_name_list );
	}
	else
	{
		menu->menu_horizontal =
		     menu_horizontal_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_menu_count_list,
			menu->role_folder_lookup_list,
			menu->role_folder_insert_list,
			menu->role_process_list,
			menu->role_process_set_member_list,
			menu->appaserver_user_role_name_list,
			menu->role_folder_subschema_name_list,
			menu->role_folder_subschema_missing_folder_name_list,
			menu->role_process_or_set_group_name_list,
			menu->role_process_or_set_missing_group_name_list );
	}

	menu->html =
		menu_html(
			menu->menu_vertical,
			menu->menu_horizontal );

	if ( !menu->html )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: menu_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return menu;
}

char *menu_item_horizontal_folder_span_tag(
		char *menu_item_horizontal_class_name,
		char *folder_name,
		char *folder_menu_count_display )
{
	static char span_tag[ 256 ];
	char *ptr = span_tag;
	char buffer[ 64 ];

	if ( !menu_item_horizontal_class_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf( ptr,
		"<span class=%s>%s",
		menu_item_horizontal_class_name,
		string_initial_capital( buffer, folder_name ) );

	if ( folder_menu_count_display )
	{
		ptr += sprintf( ptr, " %s", folder_menu_count_display );
	}

	sprintf( ptr, "</span>" );

	return span_tag;
}

char *menu_item_list_horizontal_html(
		char *heading_span_tag,
		LIST *menu_item_list )
{
	char html[ STRING_64K ];
	char *ptr = html;
	MENU_ITEM *menu_item;

	if ( !heading_span_tag )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"heading_span_tag is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( menu_item_list ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"menu_item_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf( ptr, "\t%s\n\t<ul>\n", heading_span_tag );

	do {
		menu_item = list_get( menu_item_list );

		if ( !menu_item->html )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: menu_item->html is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if (	strlen( html ) +
			strlen( menu_item->html ) + 9 >= STRING_64K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_64K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf( ptr, "\t\t%s\n", menu_item->html );

		free( menu_item->html );

	} while ( list_next( menu_item_list ) );

	ptr += sprintf( ptr, "\t</ul>" );

	return strdup( html );
}

boolean menu_horizontal_boolean(
		char *current_frame,
		char *frameset_prompt_frame,
		boolean application_menu_horizontal_boolean )
{
	return ( application_menu_horizontal_boolean &&
		 string_strcmp(
			current_frame,
			frameset_prompt_frame ) == 0 );
}

char *menu_horizontal_html(
		char *login_name,
		char *role_name,
		LIST *lookup_subschema_list,
		LIST *insert_subschema_list,
		LIST *menu_item_lookup_folder_list,
		LIST *menu_item_insert_folder_list,
		LIST *menu_horizontal_process_group_list,
		LIST *menu_item_horizontal_process_list,
		LIST *menu_item_role_change_list )
{
	char html[ STRING_128K ];
	char *ptr = html;
	MENU_HORIZONTAL_SUBSCHEMA *subschema;
	MENU_ITEM *menu_item;
	MENU_HORIZONTAL_PROCESS_GROUP *process_group;
	char buffer[ 256 ];

	if ( !login_name
	||   !role_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf( ptr, "%s\n", MENU_UNORDERED_LIST_TAG );

	if ( list_length( lookup_subschema_list )
	||   list_length( menu_item_lookup_folder_list ) )
	{
		ptr += sprintf(
			ptr,
			"%s\n<ul>\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			menu_horizontal_heading_span_tag(
				MENU_ITEM_HORIZONTAL_CLASS_NAME,
				"Lookup" /* heading_string */ ) );
	}

	if ( list_rewind( lookup_subschema_list ) )
	do {
		subschema =
			list_get(
				lookup_subschema_list );

		if ( !subschema->html ) continue;

		if ( strlen( html ) +
		     strlen( subschema->html ) +
		     1 >= STRING_128K )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				STRING_OVERFLOW_TEMPLATE,
				STRING_128K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"%s\n",
			subschema->html );

		free( subschema->html );

	} while ( list_next( lookup_subschema_list ) );

	if ( list_rewind( menu_item_lookup_folder_list ) )
	do {
		menu_item =
			list_get(
				menu_item_lookup_folder_list );

		if ( !menu_item->html ) continue;

		if ( strlen( html ) +
		     strlen( menu_item->html ) +
		     2 >= STRING_128K )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				STRING_OVERFLOW_TEMPLATE,
				STRING_128K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"\t%s\n",
			menu_item->html );

		free( menu_item->html );

	} while ( list_next( menu_item_lookup_folder_list ) );

	if ( list_length( lookup_subschema_list )
	||   list_length( menu_item_lookup_folder_list ) )
	{
		ptr += sprintf( ptr, "</ul><!--Lookup-->\n" );
	}

	if ( list_length( insert_subschema_list )
	||   list_length( menu_item_insert_folder_list ) )
	{
		ptr += sprintf(
			ptr,
			"%s\n<ul>\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			menu_horizontal_heading_span_tag(
				MENU_ITEM_HORIZONTAL_CLASS_NAME,
				"Insert" /* heading_string */ ) );
	}

	if ( list_rewind( insert_subschema_list ) )
	do {
		subschema =
			list_get(
				insert_subschema_list );

		if ( !subschema->html ) continue;

		if ( strlen( html ) +
		     strlen( subschema->html ) +
		     1 >= STRING_128K )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				STRING_OVERFLOW_TEMPLATE,
				STRING_128K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"%s\n",
			subschema->html );

		free( subschema->html );

	} while ( list_next( insert_subschema_list ) );

	if ( list_rewind( menu_item_insert_folder_list ) )
	do {
		menu_item =
			list_get(
				menu_item_insert_folder_list );

		if ( !menu_item->html ) continue;

		if ( strlen( html ) +
		     strlen( menu_item->html ) +
		     2 >= STRING_128K )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				STRING_OVERFLOW_TEMPLATE,
				STRING_128K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"\t%s\n",
			menu_item->html );

		free( menu_item->html );

	} while ( list_next( menu_item_insert_folder_list ) );

	if ( list_length( insert_subschema_list )
	||   list_length( menu_item_insert_folder_list ) )
	{
		ptr += sprintf( ptr, "</ul><!--Insert-->\n" );
	}

	if ( list_rewind( menu_horizontal_process_group_list ) )
	do {
		process_group =
			list_get(
				menu_horizontal_process_group_list );

		if ( !process_group->html ) continue;

		if ( strlen( html ) +
		     strlen( process_group->html ) +
		     1 >= STRING_128K )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				STRING_OVERFLOW_TEMPLATE,
				STRING_128K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		ptr += sprintf(
			ptr,
			"%s\n",
			process_group->html );

		free( process_group->html );

	} while ( list_next( menu_horizontal_process_group_list ) );

	if ( list_rewind( menu_item_horizontal_process_list ) )
	{
		ptr += sprintf(
			ptr,
			"%s\n<ul>\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			menu_horizontal_heading_span_tag(
				MENU_ITEM_HORIZONTAL_CLASS_NAME,
				"Process" /* heading_string */ ) );

		do {
			menu_item =
				list_get(
					menu_item_horizontal_process_list );

			if ( !menu_item->html ) continue;

			if ( strlen( html ) +
			     strlen( menu_item->html ) +
			     1 >= STRING_128K )
			{
				char message[ 128 ];

				snprintf(
					message,
					sizeof ( message ),
					STRING_OVERFLOW_TEMPLATE,
					STRING_128K );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			ptr += sprintf(
				ptr,
				"%s\n",
				menu_item->html );

			free( menu_item->html );

		} while ( list_next( menu_item_horizontal_process_list ) );

		ptr += sprintf( ptr, "</ul><!--Ungrouped process-->\n" );
	}

	if ( list_rewind( menu_item_role_change_list ) )
	{
		ptr += sprintf(
			ptr,
			"%s\n<ul>\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			menu_horizontal_heading_span_tag(
				MENU_ITEM_HORIZONTAL_CLASS_NAME,
				"Role" /* heading_string */ ) );

		do {
			menu_item =
				list_get(
					menu_item_role_change_list );

			if ( !menu_item->html ) continue;

			if ( strlen( html ) +
			     strlen( menu_item->html ) +
			     1 >= STRING_128K )
			{
				char message[ 128 ];

				snprintf(
					message,
					sizeof ( message ),
					STRING_OVERFLOW_TEMPLATE,
					STRING_128K );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			ptr += sprintf(
				ptr,
				"%s\n",
				menu_item->html );

			free( menu_item->html );

		} while ( list_next( menu_item_role_change_list ) );

		ptr += sprintf( ptr, "</ul><!--Role change-->\n" );
	}

	ptr += sprintf(
		ptr,
		"<li><a><label style=color:black>%s</label></a>\n",
		string_initial_capital(
			buffer,
			role_name ) );

	ptr += sprintf(
		ptr,
		"<li><a><label style=color:black>%s</label></a>\n",
		string_initial_capital(
			buffer,
			login_name ) );

	sprintf( ptr, "</ul>" );

	return strdup( html );
}

char *menu_vertical_html(
		char *login_name,
		char *role_name,
		LIST *menu_vertical_subschema_list,
		LIST *menu_item_folder_list,
		LIST *menu_vertical_process_group_list,
		LIST *menu_item_process_list,
		char *menu_item_role_change_html )
{
	char html[ STRING_128K ];
	char *ptr = html;
	MENU_VERTICAL_SUBSCHEMA *menu_vertical_subschema;
	MENU_VERTICAL_PROCESS_GROUP *menu_vertical_process_group;
	MENU_ITEM *menu_item;
	char *title;

	if ( !login_name || !role_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	title =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		menu_vertical_html_title(
			login_name,
			role_name );

	ptr += sprintf( ptr, "%s", title );

	ptr += sprintf( ptr, "<table border=1>\n" );

	if ( list_rewind( menu_vertical_subschema_list ) )
	{
		do {
			menu_vertical_subschema =
				list_get(
					menu_vertical_subschema_list );

			if ( !menu_vertical_subschema->html ) continue;

			ptr += sprintf(
				ptr,
				"%s\n",
				menu_vertical_subschema->html );

			free( menu_vertical_subschema->html );

		} while ( list_next( menu_vertical_subschema_list ) );
	}

	if ( list_rewind( menu_item_folder_list ) )
	{
		ptr += sprintf( ptr, "<tr><th colspan=3>Unassigned\n" );

		do {
			menu_item =
				list_get(
					menu_item_folder_list );

			if ( !menu_item->html ) continue;

			ptr += sprintf(
				ptr,
				"%s\n",
				menu_item->html );

			free( menu_item->html );

		} while ( list_next( menu_item_folder_list ) );
	}

	if ( list_rewind( menu_vertical_process_group_list ) )
	{
		do {
			menu_vertical_process_group =
				list_get(
					menu_vertical_process_group_list );

			if ( !menu_vertical_process_group->html )
				continue;

			ptr += sprintf(
				ptr,
				"%s\n",
				menu_vertical_process_group->html );

			free( menu_vertical_process_group->html );

		} while ( list_next( menu_vertical_process_group_list ) );
	}

	if ( list_rewind( menu_item_process_list ) )
	{
		do {
			menu_item =
				list_get(
					menu_item_process_list );

			if ( !menu_item->html ) continue;

			ptr += sprintf(
				ptr,
				"%s\n",
				menu_item->html );

			free( menu_item->html );

		} while ( list_next( menu_item_process_list ) );
	}

	ptr += sprintf( ptr, "</table>" );

	if ( menu_item_role_change_html )
	{
		ptr += sprintf( ptr, "<h3>Role Change</h3>\n" );

		sprintf(
			ptr,
			"%s\n",
			menu_item_role_change_html );

		free( menu_item_role_change_html );
	}

	return strdup( html );
}

char *menu_horizontal_heading_span_tag(
		char *menu_item_horizontal_class_name,
		char *heading_string )
{
	static char tag[ 128 ];
	char buffer[ 64 ];

	if ( !menu_item_horizontal_class_name
	||   !heading_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(tag,
		"<li><a><span class=%s>%s</span></a>",
		menu_item_horizontal_class_name,
		string_initial_capital(
			buffer,
			heading_string ) );

	return tag;
}

char *menu_item_vertical_folder_html(
		char *folder_name,
		char *folder_menu_count_display,
		char *lookup_anchor_tag,
		char *insert_anchor_tag )
{
	char html[ 2048 ];
	char *ptr = html;
	char *span_tag;
	char *vertical_html;

	if ( !lookup_anchor_tag )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: lookup_anchor_tag is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	span_tag =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		menu_item_span_tag(
			MENU_ITEM_VERTICAL_CLASS_NAME,
			folder_name /* item */ );

	vertical_html =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		menu_item_vertical_html(
			span_tag,
			lookup_anchor_tag,
			1 /* table_row_boolean */ );

	ptr += sprintf(
		ptr,
		"%s",
		vertical_html );

	if ( folder_menu_count_display )
	{
		ptr += sprintf(
			ptr,
			"<td align=right>%s",
			folder_menu_count_display );
	}

	if ( insert_anchor_tag )
	{
		span_tag =
			menu_item_span_tag(
				MENU_ITEM_VERTICAL_CLASS_NAME,
				"new" /* item */ );

		vertical_html =
			menu_item_vertical_html(
				span_tag,
				insert_anchor_tag,
				0 /* not table_row_boolean */ );

		ptr += sprintf(
			ptr,
			"%s",
			vertical_html );
	}

	sprintf( ptr, "\n" );

	return strdup( html );
}

char *menu_item_vertical_process_html(
		char *process_or_set_name,
		char *action_string )
{
	char html[ 1024 ];
	char buffer[ 256 ];

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
		"<tr><td colspan=3><a class=vertical_menu %s>%s</a>",
		action_string,
		string_initial_capital( buffer, process_or_set_name ) );

	return strdup( html );
}

MENU_ITEM *menu_item_vertical_role_change_new(
		char *application_name,
		char *session_key,
		char *login_name,
		LIST *menu_item_role_name_list )
{
	MENU_ITEM *menu_item;
	FORM_CHOOSE_ROLE *form_choose_role;

	if ( !application_name
	||   !session_key
	||   !login_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( menu_item_role_name_list ) ) return (MENU_ITEM *)0;

	menu_item = menu_item_calloc();

	menu_item->action_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_choose_role_action_string(
			POST_CHOOSE_ROLE_EXECUTABLE,
			application_name,
			session_key,
			login_name );

	form_choose_role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		form_choose_role_new(
			menu_item_role_name_list,
			menu_item->action_string,
			FRAMESET_MENU_FRAME /* target_frame */,
			FORM_CHOOSE_ROLE_NAME,
			CHOOSE_ROLE_DROP_DOWN_NAME );

	if ( !form_choose_role->html )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: form_choose_role->html is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	menu_item->html = form_choose_role->html;

	return menu_item;
}

MENU_VERTICAL_PROCESS_GROUP *menu_vertical_process_group_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *group_name,
		LIST *role_process_list,
		LIST *role_process_set_member_list )
{
	MENU_VERTICAL_PROCESS_GROUP *menu_vertical_process_group;
	char *process_or_set_name;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !group_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	menu_vertical_process_group = menu_vertical_process_group_calloc();

	menu_vertical_process_group->role_process_or_set_name_list =
		role_process_or_set_name_list(
			group_name,
			role_process_list,
			role_process_set_member_list );

	if ( !list_rewind(
		menu_vertical_process_group->
			role_process_or_set_name_list ) )
	{
		free( menu_vertical_process_group );
		return (MENU_VERTICAL_PROCESS_GROUP *)0;
	}

	menu_vertical_process_group->menu_item_list = list_new();

	do {
		process_or_set_name =
			list_get(
				menu_vertical_process_group->
					role_process_or_set_name_list );

		list_set(
			menu_vertical_process_group->menu_item_list,
			menu_item_vertical_process_new(
				application_name,
				session_key,
				login_name,
				role_name,
				process_or_set_name ) );

	} while ( list_next( 
			menu_vertical_process_group->
				role_process_or_set_name_list ) );

	if ( !list_length(
		menu_vertical_process_group->
			menu_item_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: menu_item_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	menu_vertical_process_group->html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		menu_item_list_vertical_html(
			group_name /* heading_string */,
			menu_vertical_process_group->
				menu_item_list );

	if ( !menu_vertical_process_group->html )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: menu_item_list_vertical_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list_free( menu_vertical_process_group->menu_item_list );

	return menu_vertical_process_group;
}

MENU_HORIZONTAL_PROCESS_GROUP *menu_horizontal_process_group_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *group_name,
		LIST *role_process_list,
		LIST *role_process_set_member_list )
{

	MENU_HORIZONTAL_PROCESS_GROUP *menu_horizontal_process_group;
	char *process_or_set_name;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !group_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	menu_horizontal_process_group =
		menu_horizontal_process_group_calloc();

	menu_horizontal_process_group->role_process_or_set_name_list =
		role_process_or_set_name_list(
			group_name,
			role_process_list,
			role_process_set_member_list );

	if ( !list_rewind(
		menu_horizontal_process_group->
			role_process_or_set_name_list ) )
	{
		free( menu_horizontal_process_group );
		return (MENU_HORIZONTAL_PROCESS_GROUP *)0;
	}

	menu_horizontal_process_group->menu_item_list = list_new();

	do {
		process_or_set_name =
			list_get(
				menu_horizontal_process_group->
					role_process_or_set_name_list );

		list_set(
			menu_horizontal_process_group->menu_item_list,
			menu_item_horizontal_process_new(
				application_name,
				session_key,
				login_name,
				role_name,
				process_or_set_name ) );

	} while ( list_next(
			menu_horizontal_process_group->
				role_process_or_set_name_list ) );

	if ( !list_length(
		menu_horizontal_process_group->
			menu_item_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: menu_item_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	menu_horizontal_process_group->menu_item_heading_span_tag =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		menu_item_heading_span_tag(
			MENU_ITEM_HORIZONTAL_CLASS_NAME,
			group_name /* heading_string */ );

	menu_horizontal_process_group->html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* Note: frees each ->html     */
		/* --------------------------- */
		menu_item_list_horizontal_html(
			menu_horizontal_process_group->
				menu_item_heading_span_tag,
			menu_horizontal_process_group->
				menu_item_list );

	if ( !menu_horizontal_process_group->html )
	{
		char message[ 128 ];

		sprintf(message,
			"menu_item_list_horizontal_html() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list_free( menu_horizontal_process_group->menu_item_list );
	return menu_horizontal_process_group;
}

char *menu_item_list_vertical_html(
		char *heading_string,
		LIST *menu_item_list )
{
	char html[ STRING_64K ];
	char *ptr = html;
	char buffer[ 64 ];
	MENU_ITEM *menu_item;

	if ( !heading_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: heading_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( menu_item_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		"<tr><th colspan=3>%s\n",
		string_initial_capital(
			buffer,
			heading_string ) );

	do {
		menu_item = list_get( menu_item_list );

		if ( !menu_item->html )
		{
			char message[ 128 ];

			sprintf(message, "menu_item->html is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if (	strlen( html ) +
			strlen( menu_item->html ) + 1 >= STRING_64K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_64K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
			message );
		}

		ptr += sprintf( ptr, "%s\n", menu_item->html );

	} while ( list_next( menu_item_list ) );

	return strdup( html );
}

MENU_VERTICAL_PROCESS_GROUP *menu_vertical_process_group_calloc( void )
{
	MENU_VERTICAL_PROCESS_GROUP *menu_vertical_process_group;

	if ( ! ( menu_vertical_process_group =
			calloc( 1, sizeof ( MENU_VERTICAL_PROCESS_GROUP ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return menu_vertical_process_group;
}

MENU_HORIZONTAL_PROCESS_GROUP *menu_horizontal_process_group_calloc( void )
{
	MENU_HORIZONTAL_PROCESS_GROUP *menu_horizontal_process_group;

	if ( ! ( menu_horizontal_process_group =
			calloc( 1,
				sizeof ( MENU_HORIZONTAL_PROCESS_GROUP ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return menu_horizontal_process_group;
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
	MENU_HORIZONTAL_SUBSCHEMA *menu_horizontal_subschema;
	char *folder_name;
	FOLDER_MENU_COUNT *folder_menu_count = {0};

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !subschema_name
	||   !state )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	menu_horizontal_subschema = menu_horizontal_subschema_calloc();
	menu_horizontal_subschema->subschema_name = subschema_name;

	menu_horizontal_subschema->role_folder_subschema_folder_name_list =
		role_folder_subschema_folder_name_list(
			subschema_name,
			role_folder_list );

	if ( !list_rewind(
		menu_horizontal_subschema->
			role_folder_subschema_folder_name_list ) )
	{
		free( menu_horizontal_subschema );
		return (MENU_HORIZONTAL_SUBSCHEMA *)0;
	}

	menu_horizontal_subschema->menu_item_list = list_new();

	do {
		folder_name =
			list_get(
				menu_horizontal_subschema->
				       role_folder_subschema_folder_name_list );

		if ( folder_menu_count_list )
		{
			folder_menu_count =
				folder_menu_count_seek(
					folder_name,
					folder_menu_count_list );
		}

		list_set(
			menu_horizontal_subschema->menu_item_list,
			menu_item_horizontal_folder_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				state,
				(folder_menu_count)
					? folder_menu_count->display
					: (char *)0 ) );

	} while( list_next(
		menu_horizontal_subschema->
			role_folder_subschema_folder_name_list ) );

	menu_horizontal_subschema->menu_item_heading_span_tag =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		menu_item_heading_span_tag(
			MENU_ITEM_HORIZONTAL_CLASS_NAME,
			subschema_name /* heading_string */ );

	menu_horizontal_subschema->html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* Note: frees each ->html     */
		/* --------------------------- */
		menu_item_list_horizontal_html(
			menu_horizontal_subschema->menu_item_heading_span_tag,
			menu_horizontal_subschema->menu_item_list );

	if ( !menu_horizontal_subschema->html )
	{
		char message[ 128 ];

		sprintf(message,
			"menu_item_list_horizontal_html() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list_free( menu_horizontal_subschema->menu_item_list );

	return menu_horizontal_subschema;
}

MENU_HORIZONTAL_SUBSCHEMA *menu_horizontal_subschema_calloc( void )
{
	MENU_HORIZONTAL_SUBSCHEMA *menu_horizontal_subschema;

	if ( ! ( menu_horizontal_subschema =
			calloc( 1, sizeof ( MENU_HORIZONTAL_SUBSCHEMA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return menu_horizontal_subschema;
}

MENU_VERTICAL_SUBSCHEMA *menu_vertical_subschema_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		LIST *folder_menu_count_list,
		LIST *role_folder_lookup_list,
		LIST *role_folder_insert_list,
		char *subschema_name )
{
	MENU_VERTICAL_SUBSCHEMA *menu_vertical_subschema;
	char *folder_name;
	FOLDER_MENU_COUNT *folder_menu_count = {0};
	boolean insert_exists;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !subschema_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	menu_vertical_subschema = menu_vertical_subschema_calloc();

	menu_vertical_subschema->role_folder_subschema_folder_name_list =
		role_folder_subschema_folder_name_list(
			subschema_name,
			role_folder_lookup_list
				/* role_folder_list */ );

	if ( !list_rewind(
		menu_vertical_subschema->
			role_folder_subschema_folder_name_list ) )
	{
		free( menu_vertical_subschema );
		return (MENU_VERTICAL_SUBSCHEMA *)0;
	}

	menu_vertical_subschema->menu_item_list = list_new();

	do {
		folder_name =
			list_get(
				menu_vertical_subschema->
				       role_folder_subschema_folder_name_list );

		insert_exists =
			role_folder_insert_exists(
				folder_name,
				role_folder_insert_list );

		if ( folder_menu_count_list )
		{
			folder_menu_count =
				folder_menu_count_seek(
					folder_name,
					folder_menu_count_list );
		}

		list_set(
			menu_vertical_subschema->menu_item_list,
			menu_item_vertical_folder_new(
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				(folder_menu_count)
					? folder_menu_count->display
					: (char *)0,
				insert_exists ) );

	} while( list_next(
		menu_vertical_subschema->
			role_folder_subschema_folder_name_list ) );

	menu_vertical_subschema->html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		menu_item_list_vertical_html(
			subschema_name /* heading_string */,
			menu_vertical_subschema->menu_item_list );

	if ( !menu_vertical_subschema->html )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: menu_item_list_vertical_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list_free( menu_vertical_subschema->menu_item_list );

	return menu_vertical_subschema;
}

MENU_VERTICAL_SUBSCHEMA *menu_vertical_subschema_calloc( void )
{
	MENU_VERTICAL_SUBSCHEMA *menu_vertical_subschema;

	if ( ! ( menu_vertical_subschema =
			calloc( 1, sizeof ( MENU_VERTICAL_SUBSCHEMA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return menu_vertical_subschema;
}

char *menu_html(
		MENU_VERTICAL *menu_vertical,
		MENU_HORIZONTAL *menu_horizontal )
{
	if ( menu_vertical )
		return menu_vertical->html;
	else
	if ( menu_horizontal )
		return menu_horizontal->html;
	else
		return (char *)0;
}

MENU_ITEM *menu_item_horizontal_role_change_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name )
{
	MENU_ITEM *menu_item;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	menu_item = menu_item_calloc();

	menu_item->span_tag =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		menu_item_span_tag(
			MENU_ITEM_HORIZONTAL_CLASS_NAME,
			role_name /* item */ );

	menu_item->action_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		post_choose_role_action_string(
			POST_CHOOSE_ROLE_EXECUTABLE,
			application_name,
			session_key,
			login_name );

	menu_item->href_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		menu_item_href_string(
			role_name /* item */,
			menu_item->action_string );

	menu_item->anchor_tag =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		menu_item_anchor_tag(
			FRAMESET_PROMPT_FRAME /* target_frame */,
			menu_item->href_string );

	menu_item->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_item_horizontal_html(
			menu_item->span_tag,
			menu_item->anchor_tag );

	return menu_item;
}

MENU_VERTICAL *menu_vertical_calloc( void )
{
	MENU_VERTICAL *menu_vertical;

	if ( ! ( menu_vertical = calloc( 1, sizeof ( MENU_VERTICAL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return menu_vertical;
}

MENU_VERTICAL *menu_vertical_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		LIST *folder_menu_count_list,
		LIST *role_folder_lookup_list,
		LIST *role_folder_insert_list,
		LIST *role_process_list,
		LIST *role_process_set_member_list,
		LIST *appaserver_user_role_name_list,
		LIST *role_folder_subschema_name_list,
		LIST *role_folder_subschema_missing_folder_name_list,
		LIST *role_process_or_set_group_name_list,
		LIST *role_process_or_set_missing_group_name_list )
{
	MENU_VERTICAL *menu_vertical = menu_vertical_calloc();
	char *subschema_name;
	char *folder_name;
	char *group_name;
	char *process_or_set_name;

	if ( list_rewind( role_folder_subschema_name_list ) )
	{
		menu_vertical->menu_vertical_subschema_list = list_new();

		do {
			subschema_name =
				list_get(
					role_folder_subschema_name_list );

			list_set(
				menu_vertical->menu_vertical_subschema_list,
				menu_vertical_subschema_new(
					application_name,
					session_key,
					login_name,
					role_name,
					folder_menu_count_list,
					role_folder_lookup_list,
					role_folder_insert_list,
					subschema_name ) );

		} while ( list_next( role_folder_subschema_name_list ) );
	}

	if ( list_rewind( role_folder_subschema_missing_folder_name_list ) )
	{
		FOLDER_MENU_COUNT *folder_menu_count = {0};

		menu_vertical->menu_item_folder_list = list_new();

		do {
			folder_name =
			    list_get(
			       role_folder_subschema_missing_folder_name_list );

			if ( folder_menu_count_list )
			{
				folder_menu_count =
					folder_menu_count_seek(
						folder_name,
						folder_menu_count_list );
			}

			list_set(
				menu_vertical->menu_item_folder_list,
				menu_item_vertical_folder_new(
					application_name,
					session_key,
					login_name,
					role_name,
					folder_name,
					(folder_menu_count)
						? folder_menu_count->display
						: (char *)0,
					role_folder_insert_exists(
						folder_name,
						role_folder_insert_list ) ) );

		} while (
			list_next(
		            role_folder_subschema_missing_folder_name_list ) );

	}

	if ( list_rewind( role_process_or_set_group_name_list ) )
	{
		menu_vertical->menu_vertical_process_group_list = list_new();

		do {
			group_name =
				list_get(
					role_process_or_set_group_name_list );

			list_set(
				menu_vertical->menu_vertical_process_group_list,
				menu_vertical_process_group_new(
					application_name,
					session_key,
					login_name,
					role_name,
					group_name,
					role_process_list,
					role_process_set_member_list ) );

		} while ( list_next( role_process_or_set_group_name_list ) );
	}

	if ( list_rewind( role_process_or_set_missing_group_name_list ) )
	{
		menu_vertical->menu_item_process_list = list_new();

		do {
			process_or_set_name =
			    list_get(
				 role_process_or_set_missing_group_name_list );

			list_set(
				menu_vertical->
					menu_item_process_list,
				menu_item_vertical_process_new(
					application_name,
					session_key,
					login_name,
					role_name,
					process_or_set_name ) );

		} while ( list_next(
			      role_process_or_set_missing_group_name_list ) );
	}

	if ( list_length( appaserver_user_role_name_list ) > 1 )
	{
		menu_vertical->menu_item_role_change =
			menu_item_vertical_role_change_new(
				application_name,
				session_key,
				login_name,
				menu_item_role_name_list(
					role_name,
					appaserver_user_role_name_list ) );
	}

	menu_vertical->html =
		menu_vertical_html(
			login_name,
			role_name,
			menu_vertical->menu_vertical_subschema_list,
			menu_vertical->menu_item_folder_list,
			menu_vertical->menu_vertical_process_group_list,
			menu_vertical->menu_item_process_list,
			(menu_vertical->menu_item_role_change)
				? menu_vertical->menu_item_role_change->html
				: (char *)0 );

	return menu_vertical;
}

MENU_HORIZONTAL *menu_horizontal_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		LIST *folder_menu_count_list,
		LIST *role_folder_lookup_list,
		LIST *role_folder_insert_list,
		LIST *role_process_list,
		LIST *role_process_set_member_list,
		LIST *appaserver_user_role_name_list,
		LIST *role_folder_subschema_name_list,
		LIST *role_folder_subschema_missing_folder_name_list,
		LIST *role_process_or_set_group_name_list,
		LIST *role_process_or_set_missing_group_name_list )
{
	MENU_HORIZONTAL *menu_horizontal = menu_horizontal_calloc();
	char *subschema_name;
	char *folder_name;
	char *group_name;
	char *process_or_set_name;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	menu_horizontal = menu_horizontal_calloc();

	if ( list_rewind( role_folder_subschema_name_list ) )
	{
		menu_horizontal->lookup_subschema_list = list_new();

		do {
			subschema_name =
				list_get(
					role_folder_subschema_name_list );

			list_set(
				menu_horizontal->lookup_subschema_list,
				menu_horizontal_subschema_new(
					application_name,
					session_key,
					login_name,
					role_name,
					folder_menu_count_list,
					role_folder_lookup_list
						/* role_folder_list */,
					subschema_name,
					APPASERVER_LOOKUP_STATE ) );

		} while ( list_next( role_folder_subschema_name_list ) );
	}

	if ( list_rewind( role_folder_subschema_name_list ) )
	{
		menu_horizontal->insert_subschema_list = list_new();

		do {
			subschema_name =
				list_get(
					role_folder_subschema_name_list );

			list_set(
				menu_horizontal->insert_subschema_list,
				menu_horizontal_subschema_new(
					application_name,
					session_key,
					login_name,
					role_name,
					(LIST *)0 /* folder_menu_count_list */,
					role_folder_insert_list
						/* role_folder_list */,
					subschema_name,
					APPASERVER_INSERT_STATE ) );

		} while ( list_next( role_folder_subschema_name_list ) );
	}

	if ( list_rewind( role_folder_subschema_missing_folder_name_list ) )
	{
		FOLDER_MENU_COUNT *folder_menu_count = {0};

		menu_horizontal->menu_item_lookup_folder_list = list_new();

		do {
			folder_name =
			    list_get(
			       role_folder_subschema_missing_folder_name_list );

			if ( folder_menu_count_list )
			{
				folder_menu_count =
					folder_menu_count_seek(
						folder_name,
						folder_menu_count_list );
			}

			list_set(
				menu_horizontal->menu_item_lookup_folder_list,
				menu_item_horizontal_folder_new(
					application_name,
					session_key,
					login_name,
					role_name,
					folder_name,
					APPASERVER_LOOKUP_STATE,
					(folder_menu_count)
						? folder_menu_count->display
						: (char *)0 ) );
		} while (
			list_next(
		            role_folder_subschema_missing_folder_name_list ) );
	}

	if ( list_rewind( role_folder_subschema_missing_folder_name_list ) )
	{
		menu_horizontal->menu_item_insert_folder_list = list_new();

		do {
			folder_name =
			    list_get(
			       role_folder_subschema_missing_folder_name_list );

			list_set(
				menu_horizontal->menu_item_insert_folder_list,
				menu_item_horizontal_folder_new(
					application_name,
					session_key,
					login_name,
					role_name,
					folder_name,
					APPASERVER_INSERT_STATE,
					(char *)0
					/* folder_menu_count_display */ ) );
		} while (
			list_next(
		            role_folder_subschema_missing_folder_name_list ) );
	}

	if ( list_rewind( role_process_or_set_group_name_list ) )
	{
		menu_horizontal->
			menu_horizontal_process_group_list =
				list_new();

		do {
			group_name =
				list_get(
					role_process_or_set_group_name_list );

			list_set(
				menu_horizontal->
					menu_horizontal_process_group_list,
				menu_horizontal_process_group_new(
					application_name,
					session_key,
					login_name,
					role_name,
					group_name,
					role_process_list,
					role_process_set_member_list ) );

		} while ( list_next( role_process_or_set_group_name_list ) );
	}

	if ( list_rewind( role_process_or_set_missing_group_name_list ) )
	{
		menu_horizontal->menu_item_horizontal_process_list = list_new();

		do {
			process_or_set_name =
			    list_get(
				 role_process_or_set_missing_group_name_list );

			list_set(
				menu_horizontal->
					menu_item_horizontal_process_list,
				menu_item_horizontal_process_new(
					application_name,
					session_key,
					login_name,
					role_name,
					process_or_set_name ) );

		} while ( list_next(
			      role_process_or_set_missing_group_name_list ) );
	}

	if ( list_length( appaserver_user_role_name_list ) > 1 )
	{
		char *local_role_name;

		menu_horizontal->menu_item_role_change_list = list_new();

		menu_horizontal->menu_item_role_name_list =
			menu_item_role_name_list(
				role_name,
				appaserver_user_role_name_list );

		list_rewind( menu_horizontal->menu_item_role_name_list );

		do {
			local_role_name =
				list_get(
					menu_horizontal->
						menu_item_role_name_list );

			list_set(
				menu_horizontal->menu_item_role_change_list,
				menu_item_horizontal_role_change_new(
					application_name,
					session_key,
					login_name,
					local_role_name ) );
		} while ( list_next(
				menu_horizontal->
					menu_item_role_name_list ) );
	}

	menu_horizontal->html =
		/* ---------------------------- */
		/* Returns heap memory		*/
		/* Note: frees each ->html	*/
		/* ---------------------------- */
		menu_horizontal_html(
			login_name,
			role_name,
			menu_horizontal->lookup_subschema_list,
			menu_horizontal->insert_subschema_list,
			menu_horizontal->menu_item_lookup_folder_list,
			menu_horizontal->menu_item_insert_folder_list,
			menu_horizontal->menu_horizontal_process_group_list,
			menu_horizontal->menu_item_horizontal_process_list,
			menu_horizontal->menu_item_role_change_list );

	return menu_horizontal;
}

MENU_HORIZONTAL *menu_horizontal_calloc( void )
{
	MENU_HORIZONTAL *menu_horizontal;

	if ( ! ( menu_horizontal = calloc( 1, sizeof ( MENU_HORIZONTAL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return menu_horizontal;
}

LIST *menu_item_role_name_list(
		char *role_name,
		LIST *appaserver_user_role_name_list )
{
	LIST *role_name_list;
	char *list_role_name;

	if ( !role_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: role_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( appaserver_user_role_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: list_rewind() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	role_name_list = list_new();

	do {
		list_role_name = list_get( appaserver_user_role_name_list );

		if ( strcmp( role_name, list_role_name) != 0 )
		{
			list_set(
				role_name_list,
				list_role_name );
		}

	} while ( list_next( appaserver_user_role_name_list ) );

	return role_name_list;
}

char *menu_item_anchor_tag(
		char *target_frame,
		char *href_string )
{
	static char anchor_tag[ 512 ];

	if ( !target_frame
	||   !href_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		anchor_tag,
		sizeof ( anchor_tag ),
		"<a target=%s href=\"%s\">",
		target_frame,
		href_string );

#ifdef NOT_DEFINED
	snprintf(
		anchor_tag,
		sizeof ( anchor_tag ),
		"<a target=%s href=\"#\" "
		"onclick=\"%s && "
		"location.href='%s';\">",
		target_frame,
		JAVASCRIPT_WAIT_START,
		href_string );
#endif

	return anchor_tag;
}

char *menu_item_horizontal_html(
		char *menu_item_span_tag,
		char *menu_item_anchor_tag )
{
	char html[ 1024 ];

	if ( !menu_item_span_tag
	||   !menu_item_anchor_tag )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(html,
		"<li>%s%s</a>",
		menu_item_anchor_tag,
		menu_item_span_tag );

	return strdup( html );
}

char *menu_item_heading_span_tag(
		char *class_name,
		char *heading_string )
{
	static char span_tag[ 300 ];

	if ( !class_name
	||   !heading_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(span_tag,
		"<li><a>%s</a>",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		menu_item_span_tag(
			class_name,
			heading_string /* item */ ) );

	return span_tag;
}

char *menu_item_href_string(
		char *item,
		char *action_string )
{
	static char href_string[ 256 ];

	if ( !item
	||   !action_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(href_string,
		"%s+%s",
		action_string,
		item );

	return href_string;
}

char *menu_item_vertical_html(
		char *menu_item_span_tag,
		char *anchor_tag,
		boolean table_row_boolean )
{
	static char html[ 1024 ];
	char *ptr = html;

	if ( !menu_item_span_tag
	||   !anchor_tag )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( table_row_boolean ) ptr += sprintf( ptr, "<tr>" );

	sprintf(
		ptr,
		"<td>%s%s</a>",
		anchor_tag,
		menu_item_span_tag );

	return html;
}

char *menu_vertical_html_title(
		char *login_name,
		char *role_name )
{
	static char title[ 256 ];
	char buffer1[ 64 ];
	char buffer2[ 64 ];

	sprintf(title,
		"<h1>%s/%s</h1>\n",
		string_initial_capital(
			buffer1,
			login_name ),
		string_initial_capital(
			buffer2,
			role_name ) );

	return title;
}

