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
	LIST *menu_lookup_folder_item_list;
	LIST *menu_insert_folder_item_list;
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
	char *folder_action_tag;
	char *folder_tag;
	char *process_action_tag;
	char *process_tag;
} MENU_ITEM;

typedef struct
{
	/* Input */
	/* ----- */
	char *verb;

	/* Process */
	/* ------- */
	LIST *menu_lookup_subschema_list;
	LIST *menu_lookup_folder_item_list;
	LIST *menu_insert_subschema_list;
	LIST *menu_insert_folder_item_list;
	LIST *menu_process_item_list;
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
	char *menu_script_tag;
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

/* Safely returns heap memory */
/* -------------------------- */
char *menu_verb_tag(	char *verb );

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

/* MENU_ITEM operations */
/* -------------------- */
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

MENU_ITEM *menu_item_calloc(
			void );

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
char *menu_item_folder_tag(
			char *folder_name,
			char *folder_menu_count_display );

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
char *menu_item_process_tag(
			char *process_or_set_name );

#endif
