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

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *subschema_name;
	char *application_name;
	char *login_name;
	char *session_key;
	char *role_name;
	char *target_frame;

	/* Process */
	/* ------- */
	LIST *item_list;
	char *span_tag;
} MENU_SUBSCHEMA;

typedef struct
{
	/* Attributes */
	/* ---------- */
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

typedef struct
{
	/* Input */
	/* ----- */
	char *verb;

	/* Process */
	/* ------- */
	LIST *subschema_list;
	LIST *item_list;
	char *menu_verb_tag;
} MENU_VERB;

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
MENU *menu_calloc(	void );

MENU *menu_fetch(	char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame,
			LIST *folder_menu_lookup_count_list );

LIST *menu_process_group_name_list(
			LIST *role_process_group_name_list,
			LIST *role_process_set_group_name_list );

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

/* Safely returns heap memory */
/* -------------------------- */
char *menu_verb_tag(	char *verb );

/* Returns heap memory or null */
/* --------------------------- */
char *menu_horizontal_html(
			char *hide_preload_html,
			LIST *menu_verb_list );

/* MENU_SUBSCHEMA operations */
/* ------------------------- */
LIST *menu_lookup_subschema_list(
			LIST *role_folder_lookup_list,
			LIST *folder_menu_lookup_count_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

LIST *menu_insert_subschema_list(
			LIST *role_folder_insert_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

MENU_SUBSCHEMA *menu_subschema_calloc(
			void );

MENU_SUBSCHEMA *menu_lookup_subschema_fetch(
			char *subschema_name,
			LIST *role_folder_lookup_name_list,
			LIST *folder_menu_lookup_count_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

MENU_SUBSCHEMA *menu_insert_subschema_fetch(
			char *subschema_name,
			LIST *role_folder_insert_name_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

char *menu_subschema_span_tag(
			char *subschema_name );

/* Returns static memory */
/* --------------------- */
char *menu_subschema_horizontal_html(
			LIST *subschema_list );

/* MENU_ITEM operations */
/* -------------------- */
MENU_ITEM *menu_item_calloc(
			void );

LIST *menu_lookup_folder_item_list(
			LIST *role_folder_lookup_name_list,
			LIST *folder_menu_lookup_count_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

LIST *menu_insert_folder_item_list(
			LIST *role_folder_insert_name_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

LIST *menu_process_item_list(
			LIST *role_process_or_set_name_list,
			char *application_name,
			char *login_name,
			char *session_key,
			char *role_name,
			char *target_frame );

LIST *menu_role_change_item_list(
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

/* Safely returns heap memory */
/* -------------------------- */
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

/* Safely returns heap memory */
/* -------------------------- */
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

/* Safely returns heap memory */
/* -------------------------- */
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

boolean menu_boolean(
			char *current_frame,
			boolean frameset_menu_horizontal );

#endif
