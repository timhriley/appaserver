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

#define MENU_OUTPUT_EXECUTABLE	"output_menu"

typedef struct
{
	char *subschema_name;
	LIST *role_folder_subschema_folder_name_list;
	LIST *menu_item_list;
	char *span_tag;
} MENU_SUBSCHEMA;

/* MENU_SUBSCHEMA operations */
/* ------------------------- */
MENU_SUBSCHEMA *menu_subschema_new(
			char *subschema_name,
			char *state,
			LIST *role_folder_subschema_folder_name_list,
			LIST *folder_menu_lookup_count_list,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *target_frame );

/* Private */
/* ------- */
MENU_SUBSCHEMA *menu_subschema_calloc(
			void );

/* Public */
/* ------ */
char *menu_subschema_horizontal_tag(
			char *subschema_name );

/* Returns static memory */
/* --------------------- */
char *menu_subschema_horizontal_html(
			LIST *subschema_list );

typedef struct
{
	char *menu_item_display;
	char *span_tag;
	char *action_tag;
} MENU_ITEM;

/* MENU_ITEM operations */
/* -------------------- */

/* Usage */
/* ----- */
MENU_ITEM *menu_item_folder_new(
			LIST *folder_menu_count_list,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state,
			char *target_frame );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *menu_item_folder_span_tag(
			char *folder_name,
			char *folder_menu_count_display );

/* Returns heap memory or null */
/* --------------------------- */
char *menu_item_folder_action_tag(
			char *appaserver_library_http_prompt,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *folder_name,
			char *state,
			char *target_frame );

/* Usage */
/* ----- */
MENU_ITEM *menu_item_process_new(
			char *process_or_set_name,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *target_frame );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *menu_item_process_span_tag(
			char *process_or_set_name );

/* Returns heap memory or null */
/* --------------------------- */
char *menu_item_process_action_tag(
			char *http_prompt,
			char *process_or_set_name,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *target_frame );

/* Usage */
/* ----- */
MENU_ITEM *menu_item_role_change_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *change_role_name );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *menu_item_role_name_span_tag(
			char *role_name );

/* Returns heap memory or null */
/* --------------------------- */
char *menu_item_role_change_action_tag(
			char *http_prompt,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name );

/* Private */
/* ------- */
MENU_ITEM *menu_item_calloc(
			void );

char *menu_item_horizontal_html(
			MENU_ITEM *menu_item );

char *menu_item_vertical_html(
			MENU_ITEM *menu_item );

/* Returns heap memory */
/* ------------------- */
char *menu_item_display(
			char *folder_process_role_name );

/* Public */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *menu_item_horizontal_html(
			LIST *menu_item_list );

typedef struct
{
	LIST *menu_subschema_list;
	LIST *role_folder_subschema_name_list;
	LIST *menu_item_folder_list;
	LIST *role_folder_subschema_missing_folder_name_list;
	LIST *menu_item_process_list;
	LIST *role_process_or_set_name_list;
	LIST *menu_item_role_change_list;
	LIST *menu_verb_role_name_list;
	char *role_display;
	char *login_name_display;
	char *tag;
} MENU_VERB;

/* MENU_VERB operations */
/* -------------------- */

/* Usage */
/* ----- */
MENU_VERB *menu_verb_folder_new(
			char *verb,
			LIST *role_folder_list,
			LIST *folder_menu_lookup_count_list,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *target_frame );

/* Usage */
/* ----- */
MENU_VERB *menu_verb_process_new(
			char *process_group_name,
			LIST *role_process_list,
			LIST *role_process_set_list,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *target_frame );

/* Usage */
/* ----- */
MENU_VERB *menu_verb_role_change_new(
			char *role_name,
			LIST *role_name_list,
			char *application_name,
			char *session_key,
			char *login_name );

/* Usage */
/* ----- */
MENU_VERB *menu_verb_role_display_new(
			char *role_name );

/* Usage */
/* ----- */
MENU_VERB *menu_verb_login_display_new(
			char *login_name );

/* Private */
/* ------- */
MENU_VERB *menu_verb_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *menu_verb_tag(	char *verb );

typedef struct
{
	LIST *role_folder_lookup_list;
	LIST *role_folder_insert_list;
	LIST *role_process_list;
	LIST *role_process_set_list;
	LIST *menu_process_group_name_list,
	LIST *role_name_list;
	LIST *menu_verb_list;
} MENU;

/* MENU operations */
/* --------------- */

/* Usage */
/* ----- */
MENU *menu_new(		char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *target_frame,
			LIST *folder_menu_lookup_count_list );

/* Process */
/* ------- */
LIST *menu_process_group_name_list(
			LIST *role_process_group_name_list,
			LIST *role_process_set_group_name_list );

/* Private */
/* ------- */
MENU *menu_calloc(	void );

/* Public */
/* ------ */
boolean menu_boolean(
			char *current_frame,
			boolean frameset_menu_horizontal );

/* Returns heap memory or null */
/* --------------------------- */
char *menu_horizontal_html(
			char *hide_preload_html,
			LIST *menu_verb_list );

/* Returns heap memory or null */
/* --------------------------- */
char *menu_vertical_html(
			LIST *menu_verb_list );

/* Returns heap memory */
/* ------------------- */
char *menu_output_system_string(
			char *menu_output_executable,
			char *session_key,
			char *login_name,
			char *role_name,
			boolean frameset_menu_horizontal,
			char *output_filename );

#endif
