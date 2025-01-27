/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/choose_role.h				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef CHOOSE_ROLE_H
#define CHOOSE_ROLE_H

#include "boolean.h"
#include "list.h"
#include "role.h"
#include "form_choose_role.h"
#include "document.h"

typedef struct
{
	char *menu_html;
	DOCUMENT *document;
	char *document_form_html;
} CHOOSE_ROLE_HORIZONTAL;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CHOOSE_ROLE_HORIZONTAL *choose_role_horizontal_new(
		char *application_name,
		char *login_name,
		char *application_title_string,
		char *choose_role_title_string,
		LIST *appaserver_user_role_name_list,
		char *post_choose_role_action_string,
		char *target_frame /* FRAMESET_PROMPT_FRAME */ );

/* Process */
/* ------- */
CHOOSE_ROLE_HORIZONTAL *choose_role_horizontal_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *choose_role_horizontal_menu_html(
		char *login_name,
		LIST *appaserver_user_role_name_list,
		char *target_frame,
		char *post_choose_role_action_string );

typedef struct
{
	DOCUMENT *document;
	FORM_CHOOSE_ROLE *form_choose_role;
	char *document_form_html;
} CHOOSE_ROLE_VERTICAL;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CHOOSE_ROLE_VERTICAL *choose_role_vertical_new(
		char *application_name,
		char *application_title_string,
		char *choose_role_title_string,
		LIST *appaserver_user_role_name_list,
		char *post_choose_role_action_string,
		char *target_frame
			/* FRAMESET_MENU_FRAME */ );

/* Process */
/* ------- */
CHOOSE_ROLE_VERTICAL *choose_role_vertical_calloc(
		void );

#define CHOOSE_ROLE_EXECUTABLE			"output_choose_role"
#define CHOOSE_ROLE_DROP_DOWN_NAME		"choose_role_drop_down"

typedef struct
{
	char *application_title_string;
	char *title_string;
	LIST *appaserver_user_role_name_list;
	char *post_choose_role_action_string;
	CHOOSE_ROLE_HORIZONTAL *choose_role_horizontal;
	CHOOSE_ROLE_VERTICAL *choose_role_vertical;
} CHOOSE_ROLE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CHOOSE_ROLE *choose_role_new(
		char *application_name,
		char *session_key,
		char *login_name,
		boolean application_menu_horizontal_boolean );

/* Process */
/* ------- */
CHOOSE_ROLE *choose_role_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *choose_role_title_string(
		char *application_title_string,
		char *login_name,
		boolean application_menu_horizontal_boolean );

#endif
