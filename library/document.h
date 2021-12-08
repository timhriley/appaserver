/* $APPASERVER_HOME/library/document.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "list.h"
#include "boolean.h"
#include "javascript.h"
#include "menu.h"
#include "form.h"

/* Constants */
/* --------- */

/* Objects */
/* ------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *title_string;
	char *menu_setup_string;
	char *calendar_setup_string;
	char *javascript_include_string;

	/* Process */
	/* ------- */
	char *meta_string;
	char *stylesheet_string;
	char *title_tag;
} DOCUMENT_HEAD;

/* DOCUMENT_HEAD operations */
/* ------------------------ */
DOCUMENT_HEAD *document_head_calloc(
			void );

/* Always succeeds */
/* --------------- */
DOCUMENT_HEAD *document_head_new(
			char *title_string,
			char *menu_setup_string,
			char *calendar_setup_string,
			char *javascript_include_string );

/* Returns program memory */
/* ---------------------- */
char *document_head_meta_string(
			void );

/* Safely returns heap memory */
/* -------------------------- */
char *document_head_stylesheet_string(
			char *application_name );

/* Safely returns heap memory */
/* -------------------------- */
char *document_head_title_tag(
			char *title_string );

/* Returns program memory or null */
/* ------------------------------ */
char *document_head_menu_setup_string(
			boolean menu_boolean );

/* Returns program memory or null */
/* ------------------------------ */
char *document_head_calendar_setup_string(
			int folder_attribute_date_name_list_length );

/* Returns program memory */
/* ---------------------- */
char *document_head_javascript_include_string(
			void );

/* Safely returns heap memory */
/* -------------------------- */
char *document_head_begin_html(
			DOCUMENT_HEAD *document_head );

/* Returns program memory */
/* ---------------------- */
char *document_head_close_html(
			void );

typedef struct
{
	char *menu_onload_string;
	char *tag;
	char *hide_preload_html;
	char *horizontal_menu_html;
	char *title_html;
	char *open_html;
} DOCUMENT_BODY;

/* DOCUMENT_BODY operations */
/* ------------------------ */
DOCUMENT_BODY *document_body_calloc(
			void );

DOCUMENT_BODY *document_body_new(
			MENU *menu,
			boolean menu_boolean,
			char *document_title,
			char *javascript_replace );

/* Returns program memory or null */
/* ------------------------------ */
char *document_body_menu_onload_string(
			boolean menu_boolean );

/* Safely returns heap memory */
/* -------------------------- */
char *document_body_tag(
			char *menu_onload_string,
			char *javascript_replace );

/* Returns program memory or null */
/* ------------------------------ */
char *document_body_hide_preload_html(
			boolean menu_boolean );

/* Returns heap memory or null */
/* --------------------------- */
char *document_body_horizontal_menu_html(
			char *hide_preload_html,
			MENU *menu,
			boolean menu_boolean );

char *document_body_title_html(
			char *document_title );

char *document_body_open_html(
			char *tag,
			char *horizontal_menu_html,
			char *title_html );

char *document_body_close_html(
			void );

typedef struct
{
	char *type_string;
	char *standard_string;
	DOCUMENT_HEAD *document_head;
	DOCUMENT_BODY *document_body;
} DOCUMENT;

/* DOCUMENT operations */
/* ------------------- */
DOCUMENT *document_calloc(
			void );

/* Always succeeds */
/* --------------- */
DOCUMENT *document_new(	char *application_name,
			MENU *menu );

/* Returns program memory */
/* ---------------------- */
char *document_type_string(
			void );

/* Returns program memory */
/* ---------------------- */
char *document_standard_string(
			void );

void document_output_content_type(
			void );

/* Need to execute document_tag_close( stdout ) */
/* -------------------------------------------- */
void document_quick_output(
			char *application_name );

/* Need to output document_tag_close_html() */
/* ---------------------------------------- */
void document_begin(	FILE *output_stream,
			DOCUMENT *document );

/* Safely returns heap memory */
/* -------------------------- */
char *document_begin_html(
			char *type_string,
			char *standard_string );

/* Returns program memory */
/* ---------------------- */
char *document_close_html(
			void );

void document_close(	void );

/* DOCUMENT choose_isa operations */
/* ------------------------------ */
DOCUMENT *document_choose_isa_new(
			char *title,
			char *prompt_message,
			char *one2m_isa_folder_name,
			MENU *menu,
			LIST *primary_key_list,
			LIST *delimited_list,
			boolean no_initial_capital,
			char *action_string );

DOCUMENT_BODY *document_body_choose_isa_new(
			char *title,
			char *prompt_message,
			char *one2m_isa_folder_name,
			MENU *menu,
			LIST *primary_key_list,
			LIST *delimited_list,
			boolean no_initial_capital,
			char *action_string );

typedef struct
{
	FORM_EDIT_TABLE *form_edit_table;
} DOCUMENT_BODY_EDIT_TABLE;

/* DOCUMENT_BODY_EDIT_TABLE operations */
/* ----------------------------------- */
DOCUMENT_BODY_EDIT_TABLE *document_body_edit_table_calloc(
			void );

DOCUMENT_BODY_EDIT_TABLE *document_body_edit_table_new(
			char * );

typedef struct
{
	DOCUMENT_HEAD *document_head;
	DOCUMENT_BODY_EDIT_TABLE *document_body_edit_table;
} DOCUMENT_EDIT_TABLE;

/* DOCUMENT_EDIT_TABLE operations */
/* ------------------------------ */
DOCUMENT_EDIT_TABLE *document_edit_table_calloc(
			void );

DOCUMENT_EDIT_TABLE *document_edit_table_new(
			char *edit_table_title,
			char *edit_table_message,
			char *folder_name,
			MENU *menu,
			LIST *folder_attribute_append_isa_list,
			LIST *dictionary_list,
			char *edit_table_submit_action_string,
			LIST *edit_table_heading_list,
			DICTIONARY *ignore_dictionary,
			DICTIONARY *non_prefixed_dictionary,
			DICTIONARY *query_dictionary,
			DICTIONARY *drillthru_dictionary );


#endif
