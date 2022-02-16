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
	/* Process */
	/* ------- */
	char *meta_string;
	char *stylesheet_string;
	char *title_tag;
	char *html;
} DOCUMENT_HEAD;

/* DOCUMENT_HEAD operations */
/* ------------------------ */
DOCUMENT_HEAD *document_head_calloc(
			void );

/* Always succeeds */
/* --------------- */
DOCUMENT_HEAD *document_head_new(
			char *application_name,
			char *application_title_string,
			char *document_head_menu_setup_string,
			char *document_head_calendar_setup_string,
			char *document_head_javascript_include_string );

/* Returns program memory */
/* ---------------------- */
char *document_head_meta_string(
			void );

/* Returns heap memory */
/* ------------------- */
char *document_head_stylesheet_string(
			char *application_name );

/* Returns heap memory */
/* ------------------- */
char *document_head_title_tag(
			char *application_title_string );

/* Returns heap memory */
/* ------------------- */
char *document_head_html(
			char *document_head_meta_string,
			char *document_head_stylesheet_string,
			char *document_head_title_tag,
			/* ------------------------------ */
			/* Returns program memory or null */
			/* ------------------------------ */
			char *document_head_menu_setup_string,
			/* ------------------------------ */
			/* Returns program memory or null */
			/* ------------------------------ */
			char *document_head_calendar_setup_string,
			char *document_head_javascript_include_string );

/* Public */
/* ------ */

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

/* Returns program memory */
/* ---------------------- */
char *document_head_close_html(
			void );

typedef struct
{
	/* Process */
	/* ------- */
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
			boolean menu_boolean,
			MENU *menu,
			char *application_title_string,
			char *subtitle_html,
			char *subsubtitle_html,
			char *javascript_replace,
			char *input_onload_string );

/* Returns program memory or null */
/* ------------------------------ */
char *document_body_menu_onload_string(
			boolean menu_boolean );

/* Returns heap memory or null */
/* --------------------------- */
char *document_body_tag(
			char *input_onload_string,
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
			boolean menu_boolean,
			MENU *menu );

/* Returns static memory or null */
/* ----------------------------- */
char *document_body_title_html(
			char *application_title_string );

/* Returns heap memory */
/* ------------------- */
char *document_body_html(
			char *document_body_tag,
			char *document_body_horizontal_menu_html,
			char *document_body_title_html,
			char *subtitle_html,
			char *subsubtitle_html );

/* Public */
/* ------ */

/* Returns heap memory or null */
/* --------------------------- */
char *document_body_set_onload_control_string(
			char *onload_control_string,
			char *append_onload_control_string );

/* Returns program memory */
/* ---------------------- */
char *document_body_close_html(
			void );

typedef struct
{
	/* Process */
	/* ------- */
	DOCUMENT_HEAD *document_head;
	DOCUMENT_BODY *document_body;
	char *type_string;
	char *standard_string;
	char *html;
} DOCUMENT;

/* DOCUMENT operations */
/* ------------------- */
DOCUMENT *document_calloc(
			void );

/* Always succeeds */
/* --------------- */
DOCUMENT *document_new(
			char *application_name,
			char *application_title_string,
			char *subtitle_html,
			char *subsubtitle_html,
			char *javascript_replace,
			boolean menu_boolean,
			MENU *menu,
			char *document_head_menu_setup_string,
			char *document_head_calendar_setup_string,
			char *document_head_javascript_include_string,
			char *input_onload_string );

DOCUMENT *document_quick_new(
			char *application_name,
			char *application_title_string );

/* Process */
/* ------- */

/* Returns program memory */
/* ---------------------- */
char *document_type_string(
			void );

/* Returns program memory */
/* ---------------------- */
char *document_standard_string(
			void );

/* Returns heap memory */
/* ------------------- */
char *document_html(
			char *document_type_string,
			char *document_standard_string );

/* Public */
/* ------ */
void document_output_html_stream(
			FILE *output_stream );

void document_output_content_type(
			void );

/* Need to output document_close_html() */
/* ------------------------------------ */
void document_quick_output(
			char *application_name );

/* Returns program memory */
/* ---------------------- */
char *document_close_html(
			void );

void document_close(	void );

/* Returns heap memory */
/* ------------------- */
char *document_form_html(
			char *document_html,
			char *document_head_html,
			char *document_head_close_html,
			char *document_body_html,
			char *form_html,
			char *document_body_close_html,
			char *document_close_html );

#endif
