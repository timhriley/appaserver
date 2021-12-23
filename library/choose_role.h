/* $APPASERVER_HOME/library/choose_role.h		*/
/* -----------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* -----------------------------------------------	*/

#ifndef CHOOSE_ROLE_H
#define CHOOSE_ROLE_H

/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"
#include "role.h"
#include "security.h"
#include "folder.h"
#include "folder_menu.h"
#include "document.h"

/* Constants */
/* --------- */

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ------*/
	char *application_name;
	char *session_key;
	char *login_name;
	char *folder_name;
	char *one2m_role_folder_name;
	char *role_name;

	/* Process */
	/* ------- */
	FOLDER *folder;
	ROLE *role;
	SECURITY_ENTITY *security_entity;
	LIST *delimited_list;
	QUERY *query;
	char *title;
	char *prompt_message;
	char *action_string;
	FOLDER_MENU *folder_menu;
	DOCUMENT *document;
} CHOOSE_ROLE;

/* Prompt operations */
/* ----------------- */
CHOOSE_ROLE *choose_role_calloc(
			void );

CHOOSE_ROLE *choose_role_prompt_fetch(
			/* ----------------------------------- */
			/* See session_folder_integrity_exit() */
			/* ----------------------------------- */
			char *application_name,
			char *session_key,
			char *login_name,
			char *folder_name,
			char *one2m_role_folder_name,
			char *role_name );

/* Safely returns heap memory */
/* -------------------------- */
char *choose_role_title(
			char *one2m_role_folder_name );

/* Returns static memory */
/* --------------------- */
char *choose_role_prompt_message(
			char *one2m_role_folder_name );

/* Safely returns heap memory */
/* -------------------------- */
char *choose_role_action_string(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *one2m_role_folder_name,
			char *role_name );

/* Post operations */
/* --------------- */
CHOOSE_ROLE *choose_role_post_fetch(
			/* ----------------------------------- */
			/* See session_folder_integrity_exit() */
			/* ----------------------------------- */
			char *application_name,
			char *session_key,
			char *login_name,
			char *folder_name,
			char *one2m_role_folder_name,
			char *role_name );

#endif
