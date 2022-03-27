/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/menu.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef MENU_H
#define MENU_H

#include "boolean.h"
#include "list.h"
#include "form_choose_role.h"

#define MENU_OUTPUT_EXECUTABLE	"output_menu"

typedef struct
{
	char *lookup_href_string;
	char *insert_href_string;
	char *post_choose_folder_href_string;
	char *post_choose_role_action_string;
	char *post_choose_role_href_string;
	char *span_tag;
	char *menu_horizontal_span_tag;
	char *post_choose_process_href_string;
	FORM_CHOOSE_ROLE *form_choose_role;
	char *html;
} MENU_ITEM;

/* MENU_ITEM operations */
/* -------------------- */

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

/* Returns heap memory */
/* ------------------- */
char *menu_item_vertical_folder_html(
			char *folder_menu_count_display,
			char *folder_name,
			char *lookup_href_string,
			char *insert_href_string );

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

/* Returns static memory */
/* --------------------- */
char *menu_item_horizontal_folder_span_tag(
			char *folder_name,
			char *folder_menu_count_display );

/* Usage */
/* ----- */
MENU_ITEM *menu_item_vertical_process_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *menu_item_vertical_process_html(
			char *process_or_set_name,
			char *post_choose_process_href_string );

/* Usage */
/* ----- */
MENU_ITEM *menu_item_horizontal_process_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_or_set_name );

/* Process */
/* ------- */

/* Usage */
/* ----- */
MENU_ITEM *menu_item_vertical_role_change_new(
			char *application_name,
			char *session_key,
			char *login_name,
			LIST *menu_item_role_name_list );

/* Usage */
/* ----- */
MENU_ITEM *menu_item_horizontal_role_change_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name );

/* Process */
/* ------- */

/* Private */
/* ------- */
MENU_ITEM *menu_item_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *menu_item_horizontal_html(
			char *span_tag,
			char *post_choose_role_href_string );

/* Public */
/* ------ */
LIST *menu_item_role_name_list(
			char *role_name,
			LIST *appaserver_user_role_name_list );

/* Returns heap memory or null */
/* --------------------------- */
char *menu_item_list_vertical_html(
			char *heading_name,
			LIST *menu_item_list );

/* Returns heap memory or null */
/* --------------------------- */
char *menu_item_list_horizontal_html(
			char *menu_horizontal_span_tag,
			LIST *menu_item_list );

/* Returns heap memory or null */
/* --------------------------- */
char *menu_item_list_html(
			LIST *menu_item_list );

typedef struct
{
	LIST *role_folder_subschema_folder_name_list;
	LIST *menu_item_list;
	char *menu_item_list_vertical_html;
} MENU_VERTICAL_SUBSCHEMA;

/* MENU_VERTICAL_SUBSCHEMA operations */
/* ---------------------------------- */

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

/* Private */
/* ------- */
MENU_VERTICAL_SUBSCHEMA *menu_vertical_subschema_calloc(
			void );

typedef struct
{
	LIST *role_folder_subschema_folder_name_list;
	LIST *menu_item_list;
	char *menu_horizontal_span_tag;
	char *menu_item_list_horizontal_html;
} MENU_HORIZONTAL_SUBSCHEMA;

/* MENU_HORIZONTAL_SUBSCHEMA operations */
/* ------------------------------------ */

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

/* Private */
/* ------- */
MENU_HORIZONTAL_SUBSCHEMA *menu_horizontal_subschema_calloc(
			void );

typedef struct
{
	LIST *role_process_or_set_name_list;
	LIST *menu_item_process_list;
	char *html;
} MENU_VERTICAL_PROCESS_GROUP;

/* MENU_VERTICAL_PROCESS_GROUP operations */
/* -------------------------------------- */
MENU_VERTICAL_PROCESS_GROUP *menu_vertical_process_group_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name ,
			char *group_name,
			LIST *role_process_list,
			LIST *role_process_set_list );

/* Private */
/* ------- */
MENU_VERTICAL_PROCESS_GROUP *menu_vertical_process_group_calloc(
			void );

typedef struct
{
	LIST *role_process_or_set_name_list;
	LIST *menu_item_process_list;
	char *menu_horizontal_span_tag;
	char *html;
} MENU_HORIZONTAL_PROCESS_GROUP;

/* MENU_HORIZONTAL_PROCESS_GROUP operations */
/* ---------------------------------------- */
MENU_HORIZONTAL_PROCESS_GROUP *menu_horizontal_process_group_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name ,
			char *group_name,
			LIST *role_process_list,
			LIST *role_process_set_list );

/* Private */
/* ------- */
MENU_HORIZONTAL_PROCESS_GROUP *menu_horizontal_process_group_calloc(
			void );

typedef struct
{
	LIST *vertical_subschema_list;
	LIST *menu_item_folder_list;
	LIST *menu_vertical_process_group_list;
	LIST *menu_item_vertical_process_list;
	MENU_ITEM *menu_item_role_change;
	char *html;
} MENU_VERTICAL;

/* MENU_VERTICAL operations */
/* ------------------------ */

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
			LIST *role_process_set_list,
			LIST *appaserver_user_role_name_list,
			LIST *role_folder_subschema_name_list,
			LIST *role_folder_subschema_missing_folder_name_list,
			LIST *role_process_group_name_list,
			LIST *role_process_group_missing_process_name_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *menu_vertical_html(
			char *login_name,
			char *role_name,
			LIST *vertical_subschema_list,
			LIST *menu_item_folder_list,
			LIST *menu_vertical_process_group_list,
			LIST *menu_item_process_list,
			char *menu_item_role_change_html );

/* Private */
/* ------- */
MENU_VERTICAL *menu_vertical_calloc(
			void );

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

/* MENU_HORIZONTAL operations */
/* -------------------------- */

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
			LIST *role_process_set_list,
			LIST *appaserver_user_role_name_list,
			LIST *role_folder_subschema_name_list,
			LIST *role_folder_subschema_missing_folder_name_list,
			LIST *role_process_group_name_list,
			LIST *role_process_group_missing_process_name_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
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
			char *document_body_hide_preload_html );

/* Private */
/* ------- */
MENU_HORIZONTAL *menu_horizontal_calloc(
			void );

typedef struct
{
	LIST *role_folder_lookup_list;
	LIST *role_folder_insert_list;
	LIST *role_process_list;
	LIST *role_process_set_list;
	LIST *appaserver_user_role_name_list;
	LIST *role_folder_subschema_name_list;
	LIST *role_folder_subschema_missing_folder_name_list;
	LIST *role_process_group_name_list;
	LIST *role_process_group_missing_process_name_list;
	MENU_VERTICAL *menu_vertical;
	MENU_HORIZONTAL *menu_horizontal;
	char *html;
} MENU;

/* MENU operations */
/* --------------- */

/* Usage */
/* ----- */
MENU *menu_new(		char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			boolean frameset_menu_horizontal,
			LIST *folder_menu_count_list );

/* Process */
/* ------- */
LIST *menu_process_group_name_list(
			LIST *role_process_group_name_list,
			LIST *role_process_set_group_name_list );

LIST *menu_group_missing_process_name_list(
			LIST *role_process_list,
			LIST *role_process_set_list );

char *menu_html(	MENU_VERTICAL *menu_vertical,
			MENU_HORIZONTAL *menu_horizontal );

/* Private */
/* ------- */
MENU *menu_calloc(	void );

/* Public */
/* ------ */
boolean menu_horizontal_boolean(
			char *current_frame,
			char *frameset_prompt_frame,
			boolean frameset_menu_horizontal );

/* Returns static memory */
/* --------------------- */
char *menu_horizontal_span_tag(
			char *heading_name );

/* Returns heap memory */
/* ------------------- */
char *menu_output_system_string(
			char *menu_output_executable,
			char *session_key,
			char *login_name,
			char *role_name,
			boolean frameset_menu_horizontal,
			char *frameset_output_filename );

#endif
