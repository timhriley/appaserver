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
#include "form_choose_role.h"
#include "document.h"

/* Constants */
/* --------- */
#define CHOOSE_ROLE_OUTPUT_EXECUTABLE		"output_choose_role"
#define CHOOSE_ROLE_FORM_NAME			"choose_role_form"
#define CHOOSE_ROLE_DROP_DOWN_ELEMENT_NAME	"choose_role_drop_down"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *default_where;
	char *default_system_string;
	char *default_role_name;
	char *title_string;
	char *post_action_string;
	char *target_frame;
	char *title_html;
	DOCUMENT *document;
	FORM_CHOOSE_ROLE *form_choose_role;
	char *document_form_html;
} CHOOSE_ROLE;

/* CHOOSE_ROLE operations */
/* ---------------------- */
CHOOSE_ROLE *choose_role_calloc(
			void );

CHOOSE_ROLE *choose_role_default_new(
			char *login_name );

/* Returns static memory */
/* --------------------- */
char *choose_role_default_where(
			char *login_name );

/* Returns static memory */
/* --------------------- */
char *choose_role_default_system_string(
			char *choose_role_default_where );

/* Returns heap memory or null */
/* --------------------------- */
char *choose_role_default_role_name(
			char *choose_role_default_system_string );

CHOOSE_ROLE *choose_role_new(
			char *application_name,
			char *login_name,
			char *session_key,
			boolean frameset_menu_horizontal );

/* Returns static memory */
/* --------------------- */
char *choose_role_title_string(
			char *login_name );

/* Returns heap memory */
/* ------------------- */
char *choose_role_post_action_string(
			char *application_name,
			char *login_name,
			char *session_key );

/* Returns program memory */
/* ---------------------- */
char *choose_role_target_frame(
			boolean frameset_menu_horizontal );

/* Returns static memory */
/* --------------------- */
char *choose_role_title_html(
			char *choose_role_title_string );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *choose_role_output_system_string(
			char *choose_role_output_executable,
			char *session_key,
			char *login_name,
			boolean frameset_menu_horizontal,
			char *output_filename );

typedef struct
{
} CHOOSE_ROLE_POST;

/* CHOOSE_ROLE_POST operations */
/* --------------------------- */
CHOOSE_ROLE_POST *choose_role_post_calloc(
			void );

#endif
