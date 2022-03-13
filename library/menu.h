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
	/* Attribute */
	/* --------- */
	char *subschema_name;

	/* Process */
	/* ------- */
	LIST *menu_item_folder_lookup_list;
	LIST *menu_item_insert_folder_list;
	char *span_tag;
} MENU_SUBSCHEMA;

/* MENU_SUBSCHEMA operations */
/* ------------------------- */
LIST *menu_subschema_lookup_list(
			LIST *role_folder_lookup_list,
			LIST *folder_menu_lookup_count_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

MENU_SUBSCHEMA *menu_subschema_lookup_fetch(
			char *subschema_name,
			LIST *role_folder_subschema_folder_name_list,
			LIST *folder_menu_lookup_count_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

LIST *menu_subschema_insert_list(
			LIST *role_folder_insert_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

MENU_SUBSCHEMA *menu_subschema_insert_fetch(
			char *subschema_name,
			LIST *role_folder_subschema_folder_name_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

/* Private */
/* ------- */
MENU_SUBSCHEMA *menu_subschema_calloc(
			void );

char *menu_subschema_span_tag(
			char *subschema_name );

/* Returns static memory */
/* --------------------- */
char *menu_subschema_horizontal_html(
			LIST *subschema_list );

typedef struct
{
	/* Input */
	/* ----- */
	char *folder_name;
	char *state;
	char *process_or_set_name;
	char *login_name;
	char *application_name;
	char *session_key;
	char *role_name;
	char *target_frame;

	/* Process */
	/* ------- */
	char *action_tag;
	char *span_tag;
} MENU_ITEM;

/* MENU_ITEM operations */
/* -------------------- */
MENU_ITEM *menu_item_calloc(
			void );

LIST *menu_item_folder_lookup_list(
			LIST *role_folder_lookup_name_list,
			LIST *folder_menu_lookup_count_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

LIST *menu_item_insert_folder_list(
			LIST *role_folder_insert_name_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

LIST *menu_item_process_list(
			LIST *role_process_or_set_name_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

LIST *menu_item_role_change_list(
			LIST *role_name_list,
			char *application_name,
			char *login_name,
			char *session_key );

MENU_ITEM *menu_item_folder_new(
			char *folder_name,
			char *folder_menu_count_display,
			char *state,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

MENU_ITEM *menu_item_process_new(
			char *process_or_set_name,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

MENU_ITEM *menu_item_role_change_new(
			char *application_name,
			char *login_name,
			char *session_key,
			char *change_role_name );

/* Returns heap memory */
/* ------------------- */
char *menu_item_folder_span_tag(
			char *folder_name,
			char *folder_menu_count_display );

/* Returns heap memory or null */
/* --------------------------- */
char *menu_item_folder_action_tag(
			char *http_prompt,
			char *folder_name,
			char *state,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

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
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

/* Returns heap memory */
/* ------------------- */
char *menu_item_role_name_span_tag(
			char *role_name );

/* Returns heap memory or null */
/* --------------------------- */
char *menu_item_role_change_action_tag(
			char *http_prompt,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name );

/* Returns static memory */
/* --------------------- */
char *menu_item_horizontal_html(
			LIST *menu_item_list );

typedef struct
{
	/* Input */
	/* ----- */
	char *verb;

	/* Process */
	/* ------- */
	LIST *menu_subschema_lookup_list;
	LIST *menu_item_folder_lookup_list;
	LIST *menu_subschema_insert_list;
	LIST *menu_item_insert_folder_list;
	LIST *menu_item_process_list;
	LIST *menu_item_role_change_list;
	char *tag;
} MENU_VERB;

/* MENU_VERB operations */
/* -------------------- */
MENU_VERB *menu_verb_calloc(
			void );

MENU_VERB *menu_verb_lookup_subschema_fetch(
			LIST *role_folder_lookup_list,
			LIST *folder_menu_lookup_count_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

MENU_VERB *menu_verb_insert_subschema_fetch(
			LIST *role_folder_insert_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

MENU_VERB *menu_verb_process_fetch(
			char *verb,
			LIST *role_process_list,
			LIST *role_process_set_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

MENU_VERB *menu_verb_role_change_fetch(
			LIST *role_name_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name );

MENU_VERB *menu_verb_role_display(
			char *role_name );

MENU_VERB *menu_verb_login_name_display(
			char *login_name );

/* Returns heap memory */
/* ------------------- */
char *menu_verb_tag(	char *verb );

typedef struct
{
	/* Input */
	/* ----- */
	char *application_name;
	char *login_name;
	char *session_key;
	char *role_name;
	char *target_frame;
	LIST *folder_menu_lookup_count_list;

	/* Process */
	/* ------- */
	LIST *menu_verb_list;
	LIST *role_process_list;
	LIST *role_process_set_list;
	LIST *menu_process_group_name_list;
} MENU;

/* MENU operations */
/* --------------- */
MENU *menu_fetch(	char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame,
			LIST *folder_menu_lookup_count_list );

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
