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
#include "menu.h"
#include "form_choose_isa.h"
#include "document.h"

/* Constants */
/* --------- */
#define CHOOSE_ISA_OUTPUT_EXECUTABLE	"output_choose_isa"
#define CHOOSE_ISA_POST_EXECUTABLE	"post_choose_isa"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Process */
	/* ------- */
	FOLDER *folder;
	ROLE *role;
	SECURITY_ENTITY *security_entity;
	LIST *delimited_list;
	QUERY_ISA_WIDGET *query_isa_widget;
	char *title_string;
	char *title_html;
	char *subtitle_html;
	char *prompt_message;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	char *action_string;
	DOCUMENT *document;
	FORM_CHOOSE_ISA *form_choose_isa;
	char *document_form_html;
} CHOOSE_ISA;

/* CHOOSE_ISA operations */
/* --------------------- */
CHOOSE_ISA *choose_isa_calloc(
			void );

CHOOSE_ISA *choose_isa_new(
			/* ----------------------------------- */
			/* See session_folder_integrity_exit() */
			/* ----------------------------------- */
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *one2m_isa_folder_name,
			char *role_name,
			boolean menu_boolean );

/* Returns static memory */
/* --------------------- */
char *choose_isa_title_string(
			char *folder_name );

/* Returns static memory */
/* --------------------- */
char *choose_isa_title_html(
			char *choose_isa_title_string );

/* Returns heap memory */
/* ------------------- */
char *choose_isa_subtitle_html(
			char *folder_name,
			char *one2m_isa_folder_name );

/* Returns heap memory */
/* ------------------- */
char *choose_isa_prompt_message(
			LIST *primary_key_list );

/* Returns heap memory */
/* ------------------- */
char *choose_isa_action_string(
			char *choose_isa_post_executable,
			char *application_name,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *one2m_isa_folder_name,
			char *role_name );

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *choose_isa_output_system_string(
			char *choose_isa_output_executable,
			char *login_name,
			char *session_key,
			char *folder_name,
			char *one_isa_folder_name,
			char *role_name,
			char *appaserver_error_filename );

typedef struct
{
} CHOOSE_ISA_POST;

/* CHOOSE_ISA_POST operations */
/* -------------------------- */
CHOOSE_ISA_POST *choose_isa_post_new(
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
