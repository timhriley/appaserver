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
#define CHOOSE_ROLE_FORM_NAME			"choose_role_form"
#define CHOOSE_ROLE_DROP_DOWN_ELEMENT_NAME	"choose_role_drop_down"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Process */
	/* ------- */
	char *title_string;
	char *post_action_string;
	DOCUMENT_CHOOSE_ROLE *document_choose_role;
	char *html;
} CHOOSE_ROLE;

/* CHOOSE_ROLE prompt operations */
/* ----------------------------- */
CHOOSE_ROLE *choose_role_calloc(
			void );

CHOOSE_ROLE *choose_role_prompt_new(
			char *application_name,
			char *session_key,
			char *login_name,
			boolean frameset_menu_horizontal );

/* Returns static memory */
/* --------------------- */
char *choose_role_title_string(
			char *login_name );

/* Returns heap memory */
/* ------------------- */
char *choose_role_post_action_string(
			char *application_name,
			char *session_key,
			char *login_name );

/* Returns program memory */
/* ---------------------- */
char *choose_role_target_frame(
			boolean frameset_menu_horizontal );

/* Returns document_choose_role_html */
/* --------------------------------- */
char *choose_role_html(
			char *document_choose_role_html );

/* CHOOSE_ROLE post operations */
/* --------------------------- */

#endif
