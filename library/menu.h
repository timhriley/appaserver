/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/menu.h					*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef MENU_H
#define MENU_H

#include "boolean.h"
#include "list.h"

#define MENU_ITEM_HORIZONTAL_CLASS_NAME	"menu"
#define MENU_ITEM_VERTICAL_CLASS_NAME	"vertical_menu"
#define MENU_EXECUTABLE			"output_menu"

typedef struct
{
	char *span_tag;
	char *action_string;
	char *href_string;
	char *anchor_tag;
	char *html;
} MENU_ITEM;

/* Usage */
/* ----- */
MENU_ITEM *menu_item_vertical_folder_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *folder_menu_count_display,
		boolean role_folder_insert_exists );

/* Process */
/* ------- */
MENU_ITEM *menu_item_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *menu_item_vertical_folder_html(
		char *folder_name,
		char *folder_menu_count_display,
		char *lookup_anchor_tag,
		char *insert_anchor_tag );

/* Usage */
/* ----- */
MENU_ITEM *menu_item_vertical_process_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_or_set_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *menu_item_vertical_html(
		char *menu_item_span_tag,
		char *anchor_tag,
		boolean table_row_boolean );

/* Usage */
/* ----- */
MENU_ITEM *menu_item_vertical_role_change_new(
		char *application_name,
		char *session_key,
		char *login_name,
		LIST *menu_item_role_name_list );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *menu_item_list_vertical_html(
		char *heading_string,
		LIST *menu_item_list );

/* Usage */
/* ----- */
MENU_ITEM *menu_item_horizontal_folder_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *state,
		char *folder_menu_count_display );

/* Usage */
/* ----- */
MENU_ITEM *menu_item_horizontal_process_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_or_set_name );

/* Usage */
/* ----- */
MENU_ITEM *menu_item_horizontal_role_change_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *menu_item_horizontal_folder_span_tag(
		char *menu_item_horizontal_class_name,
		char *folder_name,
		char *folder_menu_count_display );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *menu_item_horizontal_html(
		char *menu_item_span_tag,
		char *menu_item_anchor_tag );

/* Usage */
/* ----- */

/* --------------------------- */
/* Returns heap memory or null */
/* Note: frees each ->html     */
/* --------------------------- */
char *menu_item_list_horizontal_html(
		char *menu_item_heading_span_tag,
		LIST *menu_item_list );

/* Usage */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *menu_item_heading_span_tag(
		char *class_name,
		char *heading_string );

/* Usage */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *menu_item_span_tag(
		char *class_name,
		char *item );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *menu_item_href_string(
		char *item,
		char *action_string );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *menu_item_anchor_tag(
		char *target_frame,
		char *menu_item_href_string );

/* Usage */
/* ----- */
LIST *menu_item_role_name_list(
		char *role_name,
		LIST *appaserver_user_role_name_list );

typedef struct
{
	LIST *role_folder_subschema_folder_name_list;
	LIST *menu_item_list;
	char *html;
} MENU_VERTICAL_SUBSCHEMA;

/* Usage */
/* ----- */
MENU_VERTICAL_SUBSCHEMA *menu_vertical_subschema_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		LIST *folder_menu_count_list,
		LIST *role_folder_lookup_list,
		LIST *role_folder_insert_list,
		char *subschema_name );

/* Process */
/* ------- */
MENU_VERTICAL_SUBSCHEMA *menu_vertical_subschema_calloc(
		void );

typedef struct
{
	char *subschema_name;
	LIST *role_folder_subschema_folder_name_list;
	LIST *menu_item_list;
	char *menu_item_heading_span_tag;
	char *html;
} MENU_HORIZONTAL_SUBSCHEMA;

/* Usage */
/* ----- */
MENU_HORIZONTAL_SUBSCHEMA *menu_horizontal_subschema_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		LIST *folder_menu_count_list,
		LIST *role_folder_list,
		char *subschema_name,
		char *state );

/* Process */
/* ------- */
MENU_HORIZONTAL_SUBSCHEMA *menu_horizontal_subschema_calloc(
		void );

typedef struct
{
	LIST *role_process_or_set_name_list;
	LIST *menu_item_list;
	char *html;
} MENU_VERTICAL_PROCESS_GROUP;

/* Usage */
/* ----- */
MENU_VERTICAL_PROCESS_GROUP *menu_vertical_process_group_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *group_name,
		LIST *role_process_list,
		LIST *role_process_set_member_list );

/* Process */
/* ------- */
MENU_VERTICAL_PROCESS_GROUP *menu_vertical_process_group_calloc(
		void );

typedef struct
{
	LIST *role_process_or_set_name_list;
	LIST *menu_item_list;
	char *menu_item_heading_span_tag;
	char *html;
} MENU_HORIZONTAL_PROCESS_GROUP;

/* Usage */
/* ----- */
MENU_HORIZONTAL_PROCESS_GROUP *menu_horizontal_process_group_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *group_name,
		LIST *role_process_list,
		LIST *role_process_set_member_list );

/* Process */
/* ------- */
MENU_HORIZONTAL_PROCESS_GROUP *menu_horizontal_process_group_calloc(
		void );

typedef struct
{
	LIST *menu_vertical_subschema_list;
	LIST *menu_item_folder_list;
	LIST *menu_vertical_process_group_list;
	LIST *menu_item_process_list;
	MENU_ITEM *menu_item_role_change;
	char *html;
} MENU_VERTICAL;

/* Usage */
/* ----- */
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
		LIST *role_process_or_set_missing_group_name_list );

/* Process */
/* ------- */
MENU_VERTICAL *menu_vertical_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *menu_vertical_html(
		char *login_name,
		char *role_name,
		LIST *menu_vertical_subschema_list,
		LIST *menu_item_folder_list,
		LIST *menu_vertical_process_group_list,
		LIST *menu_item_process_list,
		char *menu_item_role_change_html );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *menu_vertical_html_title(
		char *login_name,
		char *role_name );

typedef struct
{
	LIST *lookup_subschema_list;
	LIST *insert_subschema_list;
	LIST *menu_item_lookup_folder_list;
	LIST *menu_item_insert_folder_list;
	LIST *menu_horizontal_process_group_list;
	LIST *menu_item_horizontal_process_list;
	LIST *menu_item_role_name_list;
	LIST *menu_item_role_change_list;
	char *html;
} MENU_HORIZONTAL;

/* Usage */
/* ----- */
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
		LIST *role_process_or_set_missing_group_name_list );

/* Process */
/* ------- */
MENU_HORIZONTAL *menu_horizontal_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *menu_horizontal_heading_span_tag(
		char *menu_item_horizontal_class_name,
		char *heading_string );

/* Usage */
/* ----- */

/* ---------------------------- */
/* Returns heap memory		*/
/* Note: frees each ->html	*/
/* ---------------------------- */
char *menu_horizontal_html(
		char *login_name,
		char *role_name,
		LIST *lookup_subschema_list,
		LIST *insert_subschema_list,
		LIST *menu_item_lookup_folder_list,
		LIST *menu_item_insert_folder_list,
		LIST *menu_horizontal_process_group_list,
		LIST *menu_item_horizontal_process_list,
		LIST *menu_item_role_change_list );

/* Usage */
/* ----- */

#define MENU_EXECUTABLE			"output_menu"
#define MENU_UNORDERED_LIST_TAG		"<ul id=menu>"

typedef struct
{
	LIST *role_folder_lookup_list;
	LIST *role_folder_insert_list;
	LIST *role_process_list;
	LIST *role_process_set_member_list;
	LIST *appaserver_user_role_name_list;
	LIST *role_folder_subschema_name_list;
	LIST *role_folder_subschema_missing_folder_name_list;
	LIST *role_process_or_set_group_name_list;
	LIST *role_process_or_set_missing_group_name_list;
	MENU_VERTICAL *menu_vertical;
	MENU_HORIZONTAL *menu_horizontal;
	char *html;
} MENU;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
MENU *menu_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		boolean application_menu_horizontal_boolean,
		LIST *folder_menu_count_list );

/* Process */
/* ------- */
MENU *menu_calloc(
		void );

char *menu_html(MENU_VERTICAL *menu_vertical,
		MENU_HORIZONTAL *menu_horizontal );

/* Usage */
/* ----- */
boolean menu_horizontal_boolean(
		char *current_frame,
		char *frameset_prompt_frame,
		boolean application_menu_horizontal_boolean );

#endif
