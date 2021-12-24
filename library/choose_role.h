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
#include "document.h"

/* Constants */
/* --------- */

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Process */
	/* ------- */
	char *post_action_string;
	char *title_string;
	DOCUMENT_CHOOSE_ROLE *document_choose_role;
	char *document_html;
} CHOOSE_ROLE;

/* Prompt operations */
/* ----------------- */
CHOOSE_ROLE *choose_role_calloc(
			void );

CHOOSE_ROLE *choose_role_prompt_new(
			char *application_name,
			char *session_key,
			char *login_name );

/* Safely returns heap memory */
/* -------------------------- */
char *choose_role_title_string(
			char *login_name );

/* Safely returns heap memory */
/* -------------------------- */
char *choose_role_post_action_string(
			char *application_name,
			char *session_key,
			char *login_name );

char *choose_role_document_html(
			DOCUMENT_CHOOSE_ROLE *document_choose_role );

/* Post operations */
/* --------------- */

#endif
