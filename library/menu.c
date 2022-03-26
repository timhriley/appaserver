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
#include "appaserver.h"
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

	menu_item->lookup_href_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_choose_folder_href_string(
			POST_CHOOSE_FOLDER_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			APPASERVER_LOOKUP_STATE,
			FRAMESET_PROMPT_FRAME );

	if ( role_folder_insert_exists )
	{
		menu_item->insert_href_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			post_choose_folder_href_string(
				POST_CHOOSE_FOLDER_EXECUTABLE,
				application_name,
				session_key,
				login_name,
				role_name,
				folder_name,
				APPASERVER_INSERT_STATE,
				FRAMESET_PROMPT_FRAME );
	}

	menu_item->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_item_vertical_folder_html(
			folder_menu_count_display,
			folder_name,
			menu_item->lookup_href_string,
			menu_item->insert_href_string );

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

	menu_item->post_choose_folder_href_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_choose_folder_href_string(
			POST_CHOOSE_FOLDER_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			state,
			FRAMESET_PROMPT_FRAME );

	menu_item->html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_item_horizontal_html(
			menu_item->span_tag,
			menu_item->post_choose_folder_href_string );

	return menu_item;
}

char *menu_item_horizontal_html(
			char *span_tag,
			char *href_string )
{
	char html[ 1024 ];

	if ( !span_tag || !href_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(html,
		"\t<li><a %s>%s</a>",
		href_string,
		span_tag );

	return strdup( html );
}

MENU_ITEM *menu_item_vertical_process_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name )
{
	MENU_ITEM *menu_item = menu_item_calloc();

	menu_item->post_choose_process_href_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_choose_process_href_string(
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
			menu_item->post_choose_process_href_string );

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

	menu_item->menu_horizontal_span_tag =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		menu_horizontal_span_tag(
			process_or_set_name );

	menu_item->post_choose_process_href_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_choose_process_href_string(
			POST_CHOOSE_PROCESS_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			process_or_set_name,
			FRAMESET_PROMPT_FRAME );

	menu_item->html =
		menu_item_horizontal_html(
			menu_item->menu_horizontal_span_tag,
			menu_item->post_choose_process_href_string );

	return menu_item;
}

char *menu_horizontal_span_tag( char *heading_name )
{
	char buffer[ 128 ];
	static char span_tag[ 256 ];

	if ( !heading_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: heading_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(span_tag,
		"<span class=menu>%s</span>",
		format_initial_capital(
			buffer,
			heading_name ) );

	return span_tag;
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
			boolean frameset_menu_horizontal,
			LIST *folder_menu_count_list )
{
	MENU *menu = menu_calloc();

	menu->role_folder_lookup_list = role_folder_lookup_list( role_name );
	menu->role_folder_insert_list = role_folder_insert_list( role_name );
	menu->role_process_list = role_process_list( role_name );
	menu->role_process_set_list = role_process_set_list( role_name );

	menu->appaserver_user_role_name_list =
		appaserver_user_role_name_list(
			login_name );

	menu->role_process_group_name_list =
		role_process_group_name_list(
			menu->role_process_list,
			menu->role_process_set_list );

	menu->role_folder_subschema_name_list =
		role_folder_subschema_name_list(
			menu->role_folder_lookup_list );

	menu->role_folder_subschema_missing_folder_name_list =
		role_folder_subschema_missing_folder_name_list(
			menu->role_folder_lookup_list );

	menu->role_process_group_missing_process_name_list =
		role_process_group_missing_process_name_list(
			menu->role_process_list,
			menu->role_process_set_list );

	if ( !frameset_menu_horizontal )
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
			menu->role_process_set_list,
			menu->appaserver_user_role_name_list,
			menu->role_folder_subschema_name_list,
			menu->role_folder_subschema_missing_folder_name_list,
			menu->role_process_group_name_list,
			menu->role_process_group_missing_process_name_list );
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
			menu->role_process_set_list,
			menu->appaserver_user_role_name_list,
			menu->role_folder_subschema_name_list,
			menu->role_folder_subschema_missing_folder_name_list,
			menu->role_process_group_name_list,
			menu->role_process_group_missing_process_name_list );
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

			list_string_in_order(
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

			list_string_in_order(
				group_name_list,
				group_name );

		} while ( list_next( role_process_set_group_name_list ) );
	}

	return group_name_list;
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
		ptr += sprintf( ptr, "%s", tmp );
		free( tmp );
	}

	ptr += sprintf( ptr, "\t\t</ul>\n" );

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
	char output_filename_segment[ 128 ];

	if ( !menu_output_executable
	||   !session_key
	||   !login_name
	||   !role_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( output_filename )
	{
		sprintf(output_filename_segment,
			"> %s",
			output_filename );
	}
	else
	{
		*output_filename_segment = '\0';
	}

	sprintf(system_string,
		"%s %s %s %s %c %s",
		menu_output_executable,
		session_key,
		login_name,
		role_name,
		(frameset_menu_horizontal) ? 'y' : 'n',
		output_filename_segment );

	return strdup( system_string );
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
			LIST *menu_item_role_change_list,
			char *document_body_hide_preload_html )
{
	char html[ STRING_TWO_MEG ];
	char *ptr = html;
	MENU_HORIZONTAL_SUBSCHEMA *lookup_subschema;
	MENU_HORIZONTAL_SUBSCHEMA *insert_subschema;
	MENU_ITEM *menu_item_lookup_folder;
	MENU_ITEM *menu_item_insert_folder;
	MENU_HORIZONTAL_PROCESS_GROUP *menu_horizontal_process_group;
	MENU_ITEM *menu_item_horizontal_process;
	MENU_ITEM *menu_item_role_change;
	char buffer[ 128 ];

	if ( !login_name
	||   !role_name
	||   !document_body_hide_preload_html )
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
		"%s\n",
		document_body_hide_preload_html );

	ptr += sprintf( ptr, "<ul id=menu>\n" );

	if ( list_length( lookup_subschema_list )
	||   list_length( menu_item_lookup_folder_list ) )
	{
		ptr += sprintf( ptr, "<li><ul id=menu>\n" );
		ptr += sprintf( ptr, "<li>Lookup\n" );
	}

	if ( list_rewind( lookup_subschema_list ) )
	{
		do {
			lookup_subschema =
				list_get(
					lookup_subschema_list );

			if ( !lookup_subschema->menu_item_list_horizontal_html )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: menu_item_list_horizontal is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			ptr += sprintf(
				ptr,
				"%s\n",
				lookup_subschema->
					menu_item_list_horizontal_html);

			free( lookup_subschema->
				menu_item_list_horizontal_html );

		} while ( list_next( lookup_subschema_list ) );
	}

	if ( list_rewind( menu_item_lookup_folder_list ) )
	{
		do {
			menu_item_lookup_folder =
				list_get(
					menu_item_lookup_folder_list );

			if ( !menu_item_lookup_folder->html )
			{
				fprintf(stderr,
	"ERROR in %s/%s()/%d: menu_item_lookup_folder->html is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			ptr += sprintf(
				ptr,
				"%s\n",
				menu_item_lookup_folder->html );

			free( menu_item_lookup_folder->html );

		} while ( list_next( menu_item_lookup_folder_list ) );
	}

	if ( list_length( lookup_subschema_list )
	||   list_length( menu_item_lookup_folder_list ) )
	{
		ptr += sprintf( ptr, "</ul\n" );
	}

	if ( list_length( insert_subschema_list )
	||   list_length( menu_item_insert_folder_list ) )
	{
		ptr += sprintf( ptr, "<li><ul id=menu>\n" );
		ptr += sprintf( ptr, "<li>Insert\n" );
	}

	if ( list_rewind( insert_subschema_list ) )
	{
		do {
			insert_subschema =
				list_get(
					insert_subschema_list );

			if ( !insert_subschema->menu_item_list_horizontal_html )
			{
				fprintf(stderr,
	"ERROR in %s/%s()/%d: menu_item_list_horizontal_html is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			ptr += sprintf(
				ptr,
				"%s\n",
				insert_subschema->
					menu_item_list_horizontal_html );

			free( insert_subschema->
				menu_item_list_horizontal_html );

		} while ( list_next( insert_subschema_list ) );
	}

	if ( list_rewind( menu_item_insert_folder_list ) )
	{
		do {
			menu_item_insert_folder =
				list_get(
					menu_item_insert_folder_list );

			if ( !menu_item_insert_folder->html )
			{
				fprintf(stderr,
	"ERROR in %s/%s()/%d: menu_item_insert_folder->html is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			ptr += sprintf(
				ptr,
				"%s\n",
				menu_item_insert_folder->html );

			free( menu_item_insert_folder->html );

		} while ( list_next( menu_item_insert_folder_list ) );
	}

	if ( list_length( insert_subschema_list )
	||   list_length( menu_item_insert_folder_list ) )
	{
		ptr += sprintf( ptr, "</ul\n" );
	}

	if ( list_rewind( menu_horizontal_process_group_list ) )
	{
		ptr += sprintf( ptr, "<li><ul id=menu>\n" );

		do {
			menu_horizontal_process_group =
				list_get(
					menu_horizontal_process_group_list );

			if ( !menu_horizontal_process_group->html )
			{
				fprintf(stderr,
	"ERROR in %s/%s()/%d: menu_horizontal_process_group->html is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			ptr += sprintf(
				ptr,
				"%s\n",
				menu_horizontal_process_group->html );

			free( menu_horizontal_process_group->html );

		} while ( list_next( menu_horizontal_process_group_list ) );

		ptr += sprintf( ptr, "</ul\n" );
	}

	if ( list_rewind( menu_item_horizontal_process_list ) )
	{
		ptr += sprintf( ptr, "<li><ul id=menu>\n" );
		ptr += sprintf( ptr, "<li>Process\n" );

		do {
			menu_item_horizontal_process =
				list_get(
					menu_item_horizontal_process_list );

			if ( !menu_item_horizontal_process->html )
			{
				fprintf(stderr,
	"ERROR in %s/%s()/%d: menu_item_horizontal_process->html is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			ptr += sprintf(
				ptr,
				"%s\n",
				menu_item_horizontal_process->html );

			free( menu_item_horizontal_process->html );

		} while ( list_next( menu_item_horizontal_process_list ) );

		ptr += sprintf( ptr, "</ul\n" );
	}

	if ( list_rewind( menu_item_role_change_list ) )
	{
		ptr += sprintf( ptr, "<li><ul id=menu>\n" );
		ptr += sprintf( ptr, "<li>Role\n" );

		do {
			menu_item_role_change =
				list_get(
					menu_item_role_change_list );

			if ( !menu_item_role_change->html )
			{
				fprintf(stderr,
	"ERROR in %s/%s()/%d: menu_item_role_change->html is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			ptr += sprintf(
				ptr,
				"%s\n",
				menu_item_role_change->html );

			free( menu_item_role_change->html );

		} while ( list_next( menu_item_role_change_list ) );

		ptr += sprintf( ptr, "</ul\n" );
	}

	ptr += sprintf(
		ptr,
		"<li><a><label style=color:black>%s</label></a>",
		login_name );

	ptr += sprintf(
		ptr,
		"<li><a><label style=color:black>%s</label></a>",
		string_initial_capital(
			buffer,
			role_name ) );

	ptr += sprintf( ptr, "</ul>" );

	return strdup( html );
}

char *menu_vertical_html(
			char *login_name,
			char *role_name,
			LIST *vertical_subschema_list,
			LIST *menu_item_folder_list,
			LIST *menu_vertical_process_group_list,
			LIST *menu_item_process_list,
			char *menu_item_role_change_html )
{
	char html[ STRING_TWO_MEG ];
	char *ptr = html;
	MENU_VERTICAL_SUBSCHEMA *vertical_subschema;
	MENU_VERTICAL_PROCESS_GROUP *menu_vertical_process_group;
	MENU_ITEM *menu_item_folder;
	MENU_ITEM *menu_item_process;
	char buffer1[ 128 ];
	char buffer2[ 128 ];

	if ( !login_name || !role_name )
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
		"<h1>%s/%s</h1>\n",
		string_initial_capital(
			buffer1,
			login_name ),
		string_initial_capital(
			buffer2,
			role_name ) );

	ptr += sprintf( ptr, "<table border=1>\n" );

	if ( list_rewind( vertical_subschema_list ) )
	{
		do {
			vertical_subschema =
				list_get(
					vertical_subschema_list );

			if ( !vertical_subschema->menu_item_list_vertical_html )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: menu_item_list_vertical_html is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			ptr += sprintf(
				ptr,
				"%s\n",
				vertical_subschema->
					menu_item_list_vertical_html );

			free( vertical_subschema->
				menu_item_list_vertical_html );

		} while ( list_next( vertical_subschema_list ) );
	}

	if ( list_rewind( menu_item_folder_list ) )
	{
		ptr += sprintf( ptr, "<tr><th colspan=3>Unassigned\n" );

		do {
			menu_item_folder =
				list_get(
					menu_item_folder_list );

			if ( !menu_item_folder->html )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: menu_item_folder->html is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			ptr += sprintf(
				ptr,
				"%s\n",
				menu_item_folder->html );

			free( menu_item_folder->html );

		} while ( list_next( menu_item_folder_list ) );
	}

	if ( list_rewind( menu_vertical_process_group_list ) )
	{
		do {
			menu_vertical_process_group =
				list_get(
					menu_vertical_process_group_list );

			if ( !menu_vertical_process_group->html )
			{
				fprintf(stderr,
	"ERROR in %s/%s()/%d: menu_vertical_process_group->html is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

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
			menu_item_process =
				list_get(
					menu_item_process_list );

			if ( !menu_item_process->html )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: menu_item_process->html is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			ptr += sprintf(
				ptr,
				"%s\n",
				menu_item_process->html );

			free( menu_item_process->html );

		} while ( list_next( menu_item_process_list ) );
	}

	ptr += sprintf( ptr, "</table>" );

	if ( menu_item_role_change_html )
	{
		ptr += sprintf( ptr, "<h3>Role Change</h3>\n" );
		ptr += sprintf(
			ptr,
			"%s\n",
			menu_item_role_change_html );

		free( menu_item_role_change_html );
	}

	return strdup( html );
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

#ifdef NOT_DEFINED
char *menu_subschema_list_horizontal_html(
			LIST *menu_subschema_list )
{
	char html[ STRING_TWO_MEG ];
	char *ptr = html;
	MENU_HORIZONTAL_SUBSCHEMA *menu_horizontal_subschema;
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
		menu_horizontal_subschema =
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
#endif

char *menu_item_vertical_folder_html(
			char *folder_menu_count_display,
			char *folder_name,
			char *lookup_href_string,
			char *insert_href_string )
{
	char html[ 1024 ];
	char *ptr = html;
	char buffer[ 128 ];

	if ( !lookup_href_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: lookup_href_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"<tr><td><a class=vertical_menu %s>%s</a>",
		lookup_href_string,
		string_initial_capital( buffer, folder_name ) );

	if ( folder_menu_count_display )
	{
		ptr += sprintf(
			ptr,
			"<td align=right>%s",
			folder_menu_count_display );
	}

	if ( insert_href_string )
	{
		ptr += sprintf(
			ptr,
			"<td><a class=vertical_menu %s>New</a>",
			insert_href_string );
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

		if ( !menu_item->html )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: html is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( ptr != html ) ptr += sprintf( ptr, "\n" );

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
	MENU_ITEM *menu_item = menu_item_calloc();

	if ( !list_length( menu_item_role_name_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: menu_item_role_name_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	menu_item->post_choose_role_action_string =
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
			menu_item_role_name_list,
			menu_item->post_choose_role_action_string,
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

MENU_VERTICAL_PROCESS_GROUP *menu_vertical_process_group_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name ,
			char *group_name,
			LIST *role_process_list,
			LIST *role_process_set_list )
{
	char *process_or_set_name;

	MENU_VERTICAL_PROCESS_GROUP *menu_vertical_process_group =
		menu_vertical_process_group_calloc();

	menu_vertical_process_group->role_process_or_set_name_list =
		role_process_or_set_name_list(
			group_name,
			role_process_list,
			role_process_set_list );

	if ( !list_rewind(
		menu_vertical_process_group->
			role_process_or_set_name_list ) )
	{
		free( menu_vertical_process_group );
		return (MENU_VERTICAL_PROCESS_GROUP *)0;
	}

	menu_vertical_process_group->menu_item_process_list = list_new();

	do {
		process_or_set_name =
			list_get(
				menu_vertical_process_group->
					role_process_or_set_name_list );

		list_set(
			menu_vertical_process_group->menu_item_process_list,
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
			menu_item_process_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: menu_item_process_list is empty.\n",
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
			group_name /* heading_name */,
			menu_vertical_process_group->
				menu_item_process_list );

	if ( !menu_vertical_process_group->html )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: menu_vertical_process_group->html is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return menu_vertical_process_group;
}

MENU_HORIZONTAL_PROCESS_GROUP *menu_horizontal_process_group_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name ,
			char *group_name,
			LIST *role_process_list,
			LIST *role_process_set_list )
{
	char *process_or_set_name;

	MENU_HORIZONTAL_PROCESS_GROUP *menu_horizontal_process_group =
		menu_horizontal_process_group_calloc();

	menu_horizontal_process_group->role_process_or_set_name_list =
		role_process_or_set_name_list(
			group_name,
			role_process_list,
			role_process_set_list );

	if ( !list_rewind(
		menu_horizontal_process_group->
			role_process_or_set_name_list ) )
	{
		free( menu_horizontal_process_group );
		return (MENU_HORIZONTAL_PROCESS_GROUP *)0;
	}

	menu_horizontal_process_group->menu_item_process_list = list_new();

	do {
		process_or_set_name =
			list_get(
				menu_horizontal_process_group->
					role_process_or_set_name_list );

		list_set(
			menu_horizontal_process_group->menu_item_process_list,
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
			menu_item_process_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: menu_item_process_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	menu_horizontal_process_group->menu_horizontal_span_tag =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		menu_horizontal_span_tag( group_name );

	menu_horizontal_process_group->html =
		menu_item_list_horizontal_html(
			menu_horizontal_process_group->
				menu_horizontal_span_tag,
			menu_horizontal_process_group->
				menu_item_process_list );

	if ( !menu_horizontal_process_group->html )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: menu_process_group->html is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return menu_horizontal_process_group;
}

char *menu_item_list_vertical_html(
			char *heading_name,
			LIST *menu_item_list )
{
	char html[ STRING_64K ];
	char *ptr = html;
	char buffer[ 64 ];
	char *tmp;

	if ( !heading_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: heading_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( menu_item_list ) ) return (char *)0;

	ptr += sprintf(
		ptr,
		"<tr><th colspan=3>%s\n",
		string_initial_capital(
			buffer,
			heading_name ) );

	if ( ( tmp =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		menu_item_list_html(
			menu_item_list ) ) )
	{
		ptr += sprintf( ptr, "%s", tmp );
		free( tmp );
	}

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
		ptr += sprintf( ptr, "%s", tmp );
		free( tmp );
	}

	ptr += sprintf( ptr, "\t\t</ul>\n" );

	return strdup( html );
}

MENU_VERTICAL_PROCESS_GROUP *menu_vertical_process_group_calloc( void )
{
	MENU_VERTICAL_PROCESS_GROUP *menu_vertical_process_group;

	if ( ! ( menu_vertical_process_group =
			calloc( 1, sizeof( MENU_VERTICAL_PROCESS_GROUP ) ) ) )
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
			calloc( 1, sizeof( MENU_HORIZONTAL_PROCESS_GROUP ) ) ) )
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
	MENU_HORIZONTAL_SUBSCHEMA *menu_horizontal_subschema =
		menu_horizontal_subschema_calloc();

	char *folder_name;
	FOLDER_MENU_COUNT *folder_menu_count = {0};

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

	menu_horizontal_subschema->menu_horizontal_span_tag =
		menu_horizontal_span_tag(
			subschema_name );

	menu_horizontal_subschema->menu_item_list_horizontal_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		menu_item_list_horizontal_html(
			menu_horizontal_subschema->
				menu_horizontal_span_tag,
			menu_horizontal_subschema->menu_item_list );

	if ( !menu_horizontal_subschema->menu_item_list_horizontal_html )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: menu_item_list_horizontal_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return menu_horizontal_subschema;
}

MENU_HORIZONTAL_SUBSCHEMA *menu_horizontal_subschema_calloc( void )
{
	MENU_HORIZONTAL_SUBSCHEMA *menu_horizontal_subschema;

	if ( ! ( menu_horizontal_subschema =
			calloc( 1, sizeof( MENU_HORIZONTAL_SUBSCHEMA ) ) ) )
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
	char *folder_name;
	FOLDER_MENU_COUNT *folder_menu_count = {0};

	MENU_VERTICAL_SUBSCHEMA *menu_vertical_subschema =
		menu_vertical_subschema_calloc();

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
				role_folder_insert_exists(
					folder_name,
					role_folder_insert_list ) ) );
	} while( list_next(
		menu_vertical_subschema->
			role_folder_subschema_folder_name_list ) );

	menu_vertical_subschema->menu_item_list_vertical_html =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		menu_item_list_vertical_html(
			subschema_name /* heading_name */,
			menu_vertical_subschema->menu_item_list );

	if ( !menu_vertical_subschema->menu_item_list_vertical_html )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: menu_item_list_vertical_html() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return menu_vertical_subschema;
}

MENU_VERTICAL_SUBSCHEMA *menu_vertical_subschema_calloc( void )
{
	MENU_VERTICAL_SUBSCHEMA *menu_vertical_subschema;

	if ( ! ( menu_vertical_subschema =
			calloc( 1, sizeof( MENU_VERTICAL_SUBSCHEMA ) ) ) )
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

char *menu_html(	MENU_VERTICAL *menu_vertical,
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
	MENU_ITEM *menu_item = menu_item_calloc();

	menu_item->menu_horizontal_span_tag =
		menu_horizontal_span_tag(
			role_name );

	menu_item->post_choose_role_href_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		post_choose_role_href_string(
			POST_CHOOSE_ROLE_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			FRAMESET_PROMPT_FRAME );

	menu_item->html =
		menu_item_horizontal_html(
			menu_item->menu_horizontal_span_tag,
			menu_item->post_choose_role_href_string );

	return menu_item;
}

MENU_VERTICAL *menu_vertical_calloc( void )
{
	MENU_VERTICAL *menu_vertical;

	if ( ! ( menu_vertical = calloc( 1, sizeof( MENU_VERTICAL ) ) ) )
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
			LIST *role_process_set_list,
			LIST *appaserver_user_role_name_list,
			LIST *role_folder_subschema_name_list,
			LIST *role_folder_subschema_missing_folder_name_list,
			LIST *role_process_group_name_list,
			LIST *role_process_group_missing_process_name_list )
{
	MENU_VERTICAL *menu_vertical = menu_vertical_calloc();
	char *subschema_name;
	char *folder_name;
	char *group_name;
	char *process_or_set_name;

	if ( list_rewind( role_folder_subschema_name_list ) )
	{
		menu_vertical->vertical_subschema_list = list_new();

		do {
			subschema_name =
				list_get(
					role_folder_subschema_name_list );

			list_set(
				menu_vertical->vertical_subschema_list,
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

	if ( list_rewind( role_process_group_name_list ) )
	{
		menu_vertical->menu_vertical_process_group_list = list_new();

		do {
			group_name =
				list_get(
					role_process_group_name_list );

			list_set(
				menu_vertical->menu_vertical_process_group_list,
				menu_vertical_process_group_new(
					application_name,
					session_key,
					login_name,
					role_name,
					group_name,
					role_process_list,
					role_process_set_list ) );

		} while ( list_next( role_process_group_name_list ) );
	}

	if ( list_rewind( role_process_group_missing_process_name_list ) )
	{
		menu_vertical->menu_item_vertical_process_list = list_new();

		do {
			process_or_set_name =
			    list_get(
				 role_process_group_missing_process_name_list );

			list_set(
				menu_vertical->
					menu_item_vertical_process_list,
				menu_item_vertical_process_new(
					application_name,
					session_key,
					login_name,
					role_name,
					process_or_set_name ) );

		} while ( list_next(
			      role_process_group_missing_process_name_list ) );
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
			menu_vertical->vertical_subschema_list,
			menu_vertical->menu_item_folder_list,
			menu_vertical->menu_vertical_process_group_list,
			menu_vertical->menu_item_vertical_process_list,
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
			LIST *role_process_set_list,
			LIST *appaserver_user_role_name_list,
			LIST *role_folder_subschema_name_list,
			LIST *role_folder_subschema_missing_folder_name_list,
			LIST *role_process_group_name_list,
			LIST *role_process_group_missing_process_name_list )
{
	MENU_HORIZONTAL *menu_horizontal = menu_horizontal_calloc();
	char *subschema_name;
	char *folder_name;
	char *group_name;
	char *process_or_set_name;

	if ( list_rewind( role_folder_subschema_name_list ) )
	{
		menu_horizontal->lookup_subschema_list = list_new();

		subschema_name =
			list_get(
				role_folder_subschema_name_list );

		do {
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

		subschema_name =
			list_get(
				role_folder_subschema_name_list );

		do {
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
					(folder_menu_count)
						? folder_menu_count->display
						: (char *)0,
					APPASERVER_LOOKUP_STATE ) );
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
					(char *)0
						/* folder_menu_count_display */,
					APPASERVER_INSERT_STATE ) );
		} while (
			list_next(
		            role_folder_subschema_missing_folder_name_list ) );
	}

	if ( list_rewind( role_process_group_name_list ) )
	{
		menu_horizontal->
			menu_horizontal_process_group_list =
				list_new();

		do {
			group_name =
				list_get(
					role_process_group_name_list );

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
					role_process_set_list ) );

		} while ( list_next( role_process_group_name_list ) );
	}

	if ( list_rewind( role_process_group_missing_process_name_list ) )
	{
		menu_horizontal->menu_item_horizontal_process_list = list_new();

		do {
			process_or_set_name =
			    list_get(
				 role_process_group_missing_process_name_list );

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
			      role_process_group_missing_process_name_list ) );
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
		menu_horizontal_html(
			login_name,
			role_name,
			menu_horizontal->lookup_subschema_list,
			menu_horizontal->insert_subschema_list,
			menu_horizontal->menu_item_lookup_folder_list,
			menu_horizontal->menu_item_insert_folder_list,
			menu_horizontal->menu_horizontal_process_group_list,
			menu_horizontal->menu_item_horizontal_process_list,
			menu_horizontal->menu_item_role_change_list,
			document_body_hide_preload_html(
				1 /* menu_boolean */ ) );

	return menu_horizontal;
}

MENU_HORIZONTAL *menu_horizontal_calloc( void )
{
	MENU_HORIZONTAL *menu_horizontal;

	if ( ! ( menu_horizontal = calloc( 1, sizeof( MENU_HORIZONTAL ) ) ) )
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
