/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/frameset.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FRAMESET_H
#define FRAMESET_H

#include "boolean.h"
#include "list.h"
#include "folder_menu.h"
#include "menu.h"
#include "document.h"
#include "appaserver_parameter.h"
#include "appaserver_link.h"

#define FRAMESET_EXECUTABLE		"output_frameset"
#define FRAMESET_MENU_FRAME		"menu_frame"
#define FRAMESET_PROMPT_FRAME		"prompt_frame"
#define FRAMESET_TABLE_FRAME		"table_frame"

typedef struct
{
	APPASERVER_LINK *appaserver_link;
	char *output_filename;
	char *prompt_filename;
	char *tag;
} FRAMESET_FRAME;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FRAMESET_FRAME *frameset_frame_new(
		char *application_name,
		char *data_directory,
		char *session_key,
		char *frame_name,
		char *document_form_html );

/* Process */
/* ------- */
FRAMESET_FRAME *frameset_frame_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *frameset_frame_tag(
		char *frame_name,
		char *prompt_filename );

typedef struct
{
	APPASERVER_PARAMETER *appaserver_parameter;
	DOCUMENT *frameset_document;
	DOCUMENT *frame_menu_document;
	char *role_name;
	FOLDER_MENU *folder_menu;
	MENU *menu;
	char *document_form_html;
	FRAMESET_FRAME *frameset_frame_menu;
	FRAMESET_FRAME *frameset_frame_prompt;
	FRAMESET_FRAME *frameset_frame_table;
	char *filename;
	char *execute_system_string_choose_role;
	char *blank_table_frame_system_string;
	char *blank_prompt_frame_system_string;
	char *html;
} FRAMESET;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FRAMESET *frameset_new(
		char *application_name,
		char *session_key,
		char *login_name,
		boolean application_menu_horizontal_boolean );

/* Process */
/* ------- */
FRAMESET *frameset_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *frameset_html(
		char *document_html,
		char *document_head_html,
		char *document_head_close_html,
		FRAMESET_FRAME *frameset_frame_menu,
		char *frameset_frame_prompt_tag,
		char *frameset_frame_table_tag,
		char *document_close_html );

/* Returns heap memory */
/* ------------------- */
char *frameset_blank_system_string(
		char *appliction_name,
		char *application_background_color,
		char *application_title_string,
		char *output_filename,
		char *appaserver_error_filename );

char *frameset_role_name(
		LIST *role_name_list,
		char *appaserver_user_default_role_name );

/* Returns either of ->output_filename */
/* ----------------------------------- */
char *frameset_filename(
		FRAMESET_FRAME *frameset_frame_menu,
		FRAMESET_FRAME *frameset_frame_prompt );

/* Public */
/* ------ */

/* Returns either frameset_prompt_frame or frameset_table_frame */
/* ------------------------------------------------------------ */
char *frameset_current_frame(
		char *frameset_prompt_frame,
		char *frameset_table_frame,
		char *folder_form );
#endif

