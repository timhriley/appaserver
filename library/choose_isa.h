/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/choose_isa.h				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef CHOOSE_ISA_H
#define CHOOSE_ISA_H

#include "boolean.h"
#include "list.h"
#include "role.h"
#include "security.h"
#include "folder.h"
#include "folder_row_level_restriction.h"
#include "folder_menu.h"
#include "menu.h"
#include "query.h"
#include "process.h"
#include "form_choose_isa.h"
#include "document.h"

#define CHOOSE_ISA_EXECUTABLE			"output_choose_isa"
#define CHOOSE_ISA_LOOKUP_CHECKBOX_LABEL	"choose_isa_lookup_checkbox"

typedef struct
{
	FOLDER *one_folder;
	PROCESS *populate_drop_down_process;
	ROLE *role;
	FOLDER_ROW_LEVEL_RESTRICTION *folder_row_level_restriction;
	SECURITY_ENTITY *security_entity;
	FOLDER_MENU *folder_menu;
	MENU *menu;
} CHOOSE_ISA_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CHOOSE_ISA_INPUT *choose_isa_input_new(
		char *application_name,
		char *login_name,
		char *session_key,
		char *role_name,
		char *one_isa_folder_name,
		boolean application_menu_horizontal_boolean );

/* Process */
/* ------- */
CHOOSE_ISA_INPUT *choose_isa_input_calloc(
		void );

typedef struct
{
	CHOOSE_ISA_INPUT *choose_isa_input;
	QUERY_CHOOSE_ISA *query_choose_isa;
	LIST *delimited_list;
	char *title_string;
	char *sub_sub_title_string;
	char *prompt_message;
	char *action_string;
	DOCUMENT *document;
	FORM_CHOOSE_ISA *form_choose_isa;
	char *document_form_html;
} CHOOSE_ISA;

/* Usage */
/* ----- */

/* Sefely returns */
/* -------------- */
CHOOSE_ISA *choose_isa_new(
		/* ----------------------------------- */
		/* See session_folder_integrity_exit() */
		/* ----------------------------------- */
		char *application_name,
		char *login_name,
		char *session_key,
		char *role_name,
		char *insert_folder_name,
		char *one_folder_name,
		boolean application_menu_horizontal_boolean );

/* Process */
/* ------- */
CHOOSE_ISA *choose_isa_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *choose_isa_command_line(
		char *process_command_line,
		char *session_key,
		char *login_name,
		char *role_name,
		char *insert_folder_name,
		char *security_entity_where,
		char *appaserver_error_filename );

/* Returns static memory */
/* --------------------- */
char *choose_isa_title_string(
		char *insert_folder_name );

/* Returns static memory */
/* --------------------- */
char *choose_isa_sub_sub_title_string(
		char *insert_folder_name,
		char *one_isa_folder_name );

/* Returns heap memory */
/* ------------------- */
char *choose_isa_prompt_message(
		char *widget_drop_down_dash_delimiter,
		LIST *primary_key_list );

/* Returns heap memory */
/* ------------------- */
char *choose_isa_action_string(
		const char *post_choose_isa_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *insert_folder_name,
		char *one_folder_name );

#endif
