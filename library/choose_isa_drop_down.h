/* $APPASERVER_HOME/library/choose_isa_drop_down.h	*/
/* -----------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* -----------------------------------------------	*/

#ifndef CHOOSE_ISA_DROP_DOWN_H
#define CHOOSE_ISA_DROP_DOWN_H

/* Includes */
/* -------- */
#include "boolean.h"
#include "list.h"
#include "role.h"
#include "security.h"
#include "folder.h"
#include "form.h"

/* Constants */
/* --------- */
#define CHOOSE_ISA_DROP_DOWN_PROMPT_PREFIX	"choose_"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ------*/
	char *login_name;
	char *session_key;
	char *folder_name;
	char *one2m_isa_folder_name;
	char *role_name;

	/* Process */
	/* ------- */
	ROLE *role;
	FOLDER *folder;
	SECURITY_ENTITY *security_entity;
	LIST *delimited_list;
	LIST *element_list;
} CHOOSE_ISA_DROP_DOWN;

/* Prompt operations */
/* ----------------- */
CHOOSE_ISA_DROP_DOWN *choose_isa_drop_down_calloc(
			void );

CHOOSE_ISA_DROP_DOWN *choose_isa_drop_down_prompt_fetch(
			/* ----------------------------------- */
			/* See session_folder_integrity_exit() */
			/* ----------------------------------- */
			char *application_name,
			char *session_key,
			char *login_name,
			char *folder_name,
			char *one2m_isa_folder_name,
			char *role_name );

LIST *choose_isa_drop_down_element_list(
			char *one2m_isa_folder_name,
			LIST *primary_key_list,
			LIST *delimited_list );

/* Safely returns heap memory */
/* -------------------------- */
char *choose_isa_drop_down_action_string(
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *one2m_isa_folder_name,
			char *role_name );

/* Returns static memory */
/* --------------------- */
char *choose_isa_drop_down_prompt_message(
			char *one2m_isa_folder_name );

FORM *choose_isa_drop_down_prompt_form(
			char *prompt_message,
			LIST *element_list,
			char *action_string );

/* Post operations */
/* --------------- */
CHOOSE_ISA_DROP_DOWN *choose_isa_drop_down_post_fetch(
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
