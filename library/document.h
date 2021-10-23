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

typedef struct
{
	/* Input */
	/* ----- */
	char *onload_string;
	MENU *menu;

	/* Attributes */
	/* ---------- */
	FORM_PROMPT_ISA *form_prompt_isa;
	FORM_PROMPT *form_prompt;
	FORM_TABLE *form_table;
	FORM_DETAIL *form_detail;
} DOCUMENT_BODY;

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

/* Returns program memory */
/* ---------------------- */
char *document_head_menu_setup_string(
			void );

/* Returns program memory */
/* ---------------------- */
char *document_head_calendar_setup_string(
			void );

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

/* DOCUMENT_BODY operations */
/* ------------------------ */
DOCUMENT_BODY *document_body_calloc(
			void );

DOCUMENT_BODY *document_body_new(
			char *onload_string,
			MENU *menu );

/* Returns program memory */
/* ---------------------- */
char *document_body_menu_onload_string(
			void );

/* Safely returns heap memory */
/* -------------------------- */
char *document_body_onload_string(
			char *menu_onload_string,
			char *onload_string );

/* Safely returns heap memory */
/* -------------------------- */
char *document_body_begin_html(
			char *onload_string );

/* Returns program memory */
/* ---------------------- */
char *document_body_hide_preload_message(
			void );

void document_body_horizontal_menu_output(
			FILE *output_stream,
			char *hide_preload_message,
			MENU *menu );

void document_body_vertical_menu_output(
			FILE *output_stream,
			MENU *menu );

void document_body_begin(
			FILE *output_stream,
			DOCUMENT_BODY *document_body );

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

#endif
