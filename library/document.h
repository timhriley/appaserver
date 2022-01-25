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
			char *application_name,
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
char *document_head_open_html(
			char *meta_string,
			char *stylesheet_string,
			char *title_tag,
			char *menu_setup_string,
			char *calendar_setup_string,
			char *javascript_include_string );

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
	char *html;
} DOCUMENT_BODY;

/* DOCUMENT_BODY operations */
/* ------------------------ */
DOCUMENT_BODY *document_body_calloc(
			void );

/* Always succeeds */
/* --------------- */
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

/* Returns static memory or null */
/* ----------------------------- */
char *document_body_title_html(
			char *document_title );

/* Returns heap memory */
/* ------------------- */
char *document_body_html(
			char *tag,
			char *horizontal_menu_html,
			char *title_html );

/* Returns program memory */
/* ---------------------- */
char *document_body_close_html(
			void );

typedef struct
{
	/* Process */
	/* ------- */
	char *type_string;
	char *standard_string;
	DOCUMENT_HEAD *document_head;
	char *html;

	/* External */
	/* -------- */
	DOCUMENT_BODY *document_body;
} DOCUMENT;

/* DOCUMENT operations */
/* ------------------- */
DOCUMENT *document_calloc(
			void );

/* Always succeeds */
/* --------------- */
DOCUMENT *document_new(	char *application_name,
			char *title,
			boolean menu_boolean,
			int folder_attribute_date_name_list_length );

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

/* Returns heap memory */
/* ------------------- */
char *document_html(
			char *type_string,
			char *standard_string );

/* Returns program memory */
/* ---------------------- */
char *document_close_html(
			void );

void document_close(	void );

typedef struct
{
	/* Process */
	/* ------- */
	DOCUMENT_BODY *document_body;
	FORM_CHOOSE_ISA *form_choose_isa;
	char *html;
} DOCUMENT_BODY_CHOOSE_ISA;

/* DOCUMENT_BODY_CHOOSE_ISA operations */
/* ----------------------------------- */
DOCUMENT_BODY_CHOOSE_ISA *document_body_choose_isa_calloc(
			void );

DOCUMENT_BODY_CHOOSE_ISA *document_body_choose_isa_new(
			char *choose_isa_title_string,
			char *choose_isa_subtitle_html,
			char *choose_isa_prompt_message,
			MENU *menu,
			boolean menu_boolean,
			LIST *primary_key_list,
			LIST *delimited_list,
			boolean no_initial_capital,
			char *choose_isa_post_action_string );

/* Returns heap memory */
/* ------------------- */
char *document_body_choose_isa_html(
			char *document_body_html,
			char *form_choose_isa_html,
			char *document_body_close_html );

typedef struct
{
	/* Process */
	/* ------- */
	DOCUMENT *document;
	DOCUMENT_BODY_CHOOSE_ISA *document_body_choose_isa;
	char *html;
} DOCUMENT_CHOOSE_ISA;

/* DOCUMENT_CHOOSE_ISA operations */
/* ------------------------------ */
DOCUMENT_CHOOSE_ISA *document_choose_isa_calloc(
			void );

DOCUMENT_CHOOSE_ISA *document_choose_isa_new(
			char *application_name,
			char *choose_isa_title_string,
			char *choose_isa_subtitle_html,
			char *choose_isa_prompt_message,
			MENU *menu,
			boolean menu_boolean,
			LIST *primary_key_list,
			LIST *delimited_list,
			boolean no_initial_capital,
			char *choose_isa_post_action_string );

/* Returns heap memory */
/* ------------------- */
char *document_choose_isa_html(
			char *document_html,
			char *document_body_choose_isa_html,
			char *document_close_html );

typedef struct
{
	/* Process */
	/* ------- */
	DOCUMENT_BODY *document_body;
	FORM_CHOOSE_ROLE *form_choose_role;
	char *html;
} DOCUMENT_BODY_CHOOSE_ROLE;

/* DOCUMENT_BODY_CHOOSE_ROLE operations */
/* ------------------------------------ */
DOCUMENT_BODY_CHOOSE_ROLE *document_body_choose_role_calloc(
			void );

DOCUMENT_BODY_CHOOSE_ROLE *document_body_choose_role_new(
			char *choose_role_title_string,
			LIST *role_name_list,
			char *choose_role_post_action_string,
			char *choose_role_target_frame,
			char *choose_role_form_name,
			char *choose_role_drop_down_element_name );

/* Returns heap memory */
/* ------------------- */
char *document_body_choose_role_html(
			char *document_body_html,
			char *form_choose_role_html,
			char *document_body_close_html );

typedef struct
{
	/* Process */
	/* ------- */
	DOCUMENT *document;
	DOCUMENT_BODY_CHOOSE_ROLE *document_body_choose_role;
	char *html;
} DOCUMENT_CHOOSE_ROLE;

/* DOCUMENT_CHOOSE_ROLE operations */
/* ------------------------------- */
DOCUMENT_CHOOSE_ROLE *document_choose_role_calloc(
			void );

DOCUMENT_CHOOSE_ROLE *document_choose_role_new(
			char *application_name,
			char *choose_role_title_string,
			LIST *role_name_list,
			char *choose_role_post_action_string,
			char *choose_role_target_frame,
			char *choose_role_form_name,
			char *choose_role_drop_down_element_name );

/* Returns heap memory */
/* ------------------- */
char *document_choose_role_html(
			char *document_html,
			char *document_body_choose_role_html,
			char *document_close_html );

typedef struct
{
	FORM_EDIT_TABLE *form_edit_table;
	char *html;
	char *trailer_html;
} DOCUMENT_BODY_EDIT_TABLE;

/* DOCUMENT_BODY_EDIT_TABLE operations */
/* ----------------------------------- */
DOCUMENT_BODY_EDIT_TABLE *document_body_edit_table_calloc(
			void );

DOCUMENT_BODY_EDIT_TABLE *document_body_edit_table_new(
			char *folder_name,
			MENU *menu,
			boolean menu_boolean,
			int dictionary_list_length,
			char *edit_table_submit_action_string,
			LIST *operation_list,
			LIST *edit_table_heading_list,
			char *edit_table_title,
			char *javascript_replace,
			char *target_frame,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			DICTIONARY *drillthru_dictionary,
			DICTIONARY *ignore_dictionary );

char *document_body_edit_table_html(
			char *document_body_html,
			char *form_edit_table_html );

char *document_body_edit_table_trailer_html(
			char *form_edit_table_trailer_html );

typedef struct
{
	DOCUMENT *document;
	DOCUMENT_BODY_EDIT_TABLE *document_body_edit_table;
} DOCUMENT_EDIT_TABLE;

/* DOCUMENT_EDIT_TABLE operations */
/* ------------------------------ */
DOCUMENT_EDIT_TABLE *document_edit_table_calloc(
			void );

DOCUMENT_EDIT_TABLE *document_edit_table_new(
			char *application_name,
			char *folder_name,
			char *edit_table_title,
			MENU *menu,
			boolean menu_boolean,
			LIST *folder_attribute_append_isa_list,
			int dictionary_list_length,
			char *edit_table_submit_action_string,
			LIST *operation_list,
			LIST *edit_table_heading_list,
			char *javascript_replace,
			char *target_frame,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			DICTIONARY *drillthru_dictionary,
			DICTIONARY *ignore_dictionary );

/* To be retired */
/* ------------- */
void document_output_html_stream(
			FILE *output_stream );

#endif
