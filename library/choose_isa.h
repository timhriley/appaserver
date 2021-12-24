/* $APPASERVER_HOME/library/choose_isa.h		*/
/* -----------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* -----------------------------------------------	*/

#ifndef CHOOSE_ISA_H
#define CHOOSE_ISA_H

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
	char *one2m_isa_folder_name;
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
} CHOOSE_ISA;

/* Prompt operations */
/* ----------------- */
CHOOSE_ISA *choose_isa_calloc(
			void );

CHOOSE_ISA *choose_isa_prompt_fetch(
			/* ----------------------------------- */
			/* See session_folder_integrity_exit() */
			/* ----------------------------------- */
			char *application_name,
			char *session_key,
			char *login_name,
			char *folder_name,
			char *one2m_isa_folder_name,
			char *role_name );

/* Returns heap memory */
/* ------------------- */
char *choose_isa_title_string(
			char *folder_name );

/* Returns heap memory */
/* ------------------- */
char *choose_isa_subtitle_html(
			char *folder_name,
			char *one2m_isa_folder_name );

/* Returns heap memory */
/* ------------------- */
char *choose_isa_prompt_message(
			LIST *primary_key_list );

/* Safely returns heap memory */
/* -------------------------- */
char *choose_isa_action_string(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *one2m_isa_folder_name,
			char *role_name );

/* Post operations */
/* --------------- */
CHOOSE_ISA *choose_isa_post_fetch(
			/* ----------------------------------- */
			/* See session_folder_integrity_exit() */
			/* ----------------------------------- */
			char *application_name,
			char *session_key,
			char *login_name,
			char *folder_name,
			char *one2m_isa_folder_name,
			char *role_name );

#endif
