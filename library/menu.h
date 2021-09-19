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
	char *folder_action;
	char *process_action;
} MENU_ITEM;


/* MENU_ITEM operations */
/* -------------------- */
MENU_ITEM *menu_item_calloc(
			void );

MENU_ITEM *menu_item_folder_new(
			char *folder_name,
			char *state,
			char *login_name,
			char *application_name,
			char *session_key,
			char *role_name,
			char *target_frame );

MENU_ITEM *menu_item_process_new(
			char *process_or_set_name,
			char *login_name,
			char *application_name,
			char *session_key,
			char *role_name,
			char *target_frame );

LIST *menu_item_folder_list(
			LIST *role_folder_name_list,
			char *state,
			char *login_name,
			char *application_name,
			char *session_key,
			char *role_name,
			char *target_frame );

LIST *menu_item_process_list(
			LIST *role_process_name_list,
			LIST *role_process_set_name_list,
			char *login_name,
			char *application_name,
			char *session_key,
			char *role_name,
			char *target_frame );

/* Returns heap memory or null */
/* --------------------------- */
char *menu_item_folder_action_tag(
			char *http_prompt,
			char *folder_name,
			char *state,
			char *login_name,
			char *application_name,
			char *session_key,
			char *role_name,
			char *target_frame );

/* Safely returns heap memory */
/* -------------------------- */
char *menu_item_folder_span_tag(
			char *folder_name );

/* Returns heap memory or null */
/* --------------------------- */
char *menu_item_process_action_tag(
			char *http_prompt,
			char *process_or_set_name,
			char *login_name,
			char *application_name,
			char *session_key,
			char *role_name,
			char *target_frame );

/* Safely returns heap memory */
/* -------------------------- */
char *menu_item_process_span_tag(
			char *process_or_set_name );

#endif
