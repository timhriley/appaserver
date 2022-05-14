/* $APPASERVER_HOME/library/frameset.h			*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef FRAMESET_H
#define FRAMESET_H

#include "boolean.h"
#include "list.h"
#include "appaserver_library.h"
#include "document.h"
#include "appaserver_parameter.h"
#include "appaserver_link.h"

#define FRAMESET_OUTPUT_EXECUTABLE	"output_frameset"
#define FRAMESET_MENU_FRAME		"menu_frame"
#define FRAMESET_PROMPT_FRAME		"prompt_frame"
#define FRAMESET_TABLE_FRAME		"table_frame"

typedef struct
{
	APPASERVER_LINK *appaserver_link;
	char *output_filename;
	char *prompt_filename;
	char *html;
} FRAMESET_FRAME;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
FRAMESET_FRAME *frameset_frame_new(
			char *application_name,
			char *document_root_directory,
			char *session_key,
			char *frame_name );

/* Process */
/* ------- */
FRAMESET_FRAME *frameset_frame_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *frameset_frame_html(
			char *frame_name,
			char *prompt_filename );

typedef struct
{
	APPASERVER_PARAMETER *appaserver_parameter;
	DOCUMENT *document;
	FRAMESET_FRAME *frameset_frame_menu;
	FRAMESET_FRAME *frameset_frame_prompt;
	FRAMESET_FRAME *frameset_frame_table;
	char *html;
	char *blank_table_frame_system_string;
	char *blank_prompt_frame_system_string;
	char *role_name;
	char *output_filename;
	char *menu_output_system_string;
	char *choose_role_output_system_string;
} FRAMESET;

/* Usage */
/* ----- */
FRAMESET *frameset_new(
			char *application_name,
			char *session_key,
			char *login_name,
			boolean frameset_menu_horizontal );

/* Process */
/* ------- */
FRAMESET *frameset_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *frameset_html(	char *document_html,
			char *document_head_html,
			char *document_head_close_html,
			char *frameset_frame_menu_html,
			char *frameset_frame_prompt_html,
			char *frameset_frame_edit_html,
			boolean frameset_menu_horizontal,
			char *document_close_html );

/* Returns heap memory */
/* ------------------- */
char *frameset_blank_system_string(
			char *application_background_color,
			char *application_title_string,
			char *output_filename,
			char *appaserver_error_filename );

char *frameset_role_name(
			LIST *role_name_list,
			char *appaserver_user_default_role_name );

/* Returns either of ->output_filename */
/* ----------------------------------- */
char *frameset_output_filename(
			FRAMESET_FRAME *frameset_frame_menu,
			FRAMESET_FRAME *frameset_frame_prompt );

/* Public */
/* ------ */
boolean frameset_menu_horizontal(
			char *application_name,
			char *login_name );

/* Returns heap memory */
/* ------------------- */
char *frameset_output_system_string(
			char *frameset_output_executable,
			char *session_key,
			char *login_name,
			char *post_login_output_pipe_string,
			char *appaserver_error_filename );

#endif

