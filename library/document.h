/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/document.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "list.h"
#include "boolean.h"
#include "javascript.h"

typedef struct
{
	char *meta_string;
	char *stylesheet_string;
	char *title_tag;
	char *html;
} DOCUMENT_HEAD;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DOCUMENT_HEAD *document_head_new(
		char *application_name,
		char *application_title_string,
		char *document_head_menu_setup_string,
		char *document_head_calendar_setup_string,
		char *javascript_include_string );

/* Process */
/* ------- */
DOCUMENT_HEAD *document_head_calloc(
		void );

/* Returns program memory */
/* ---------------------- */
char *document_head_meta_string(
		void );

/* Returns program memory */
/* ---------------------- */
char *document_head_stylesheet_string(
		void );

/* Returns heap memory */
/* ------------------- */
char *document_head_title_tag(
		char *application_title_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *document_head_html(
		char *document_head_meta_string,
		char *document_head_stylesheet_string,
		char *document_head_title_tag,
		char *document_head_menu_setup_string,
		char *document_head_calendar_setup_string,
		char *document_head_javascript_include_string );

/* Process */
/* ------- */

/* Returns program memory */
/* ---------------------- */
char *document_head_open_tag(
		void );

/* Usage */
/* ----- */
void document_head_output_html_stream(
		FILE *output_stream );

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
char *document_head_close_tag(
		void );

void document_head_output_stylesheet(
		FILE *output_stream,
		char *application_name,
		char *stylesheet_filename );

/* Returns program memory */
/* ---------------------- */
char *document_head_javascript_string(
		void );

typedef struct
{
	char *menu_onload_string;
	char *onload_open_tag;
	char *hide_preload_html;
	char *notepad_html;
	char *html;
} DOCUMENT_BODY;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DOCUMENT_BODY *document_body_new(
		char *menu_html,
		char *title_string,
		char *sub_title_string,
		char *sub_sub_title_string,
		char *notepad,
		char *onload_javascript_string );

/* Process */
/* ------- */
DOCUMENT_BODY *document_body_calloc(
		void );

/* Returns program memory */
/* ---------------------- */
char *document_body_menu_onload_string(
		void );

/* Returns program memory */
/* ---------------------- */
char *document_body_hide_preload_html(
		void );

/* Returns heap memory */
/* ------------------- */
char *document_body_html(
		char *menu_html,
		char *title_string,
		char *sub_title_string,
		char *sub_sub_title_string,
		char *body_onload_open_tag,
		char *document_body_hide_preload_html,
		char *document_body_notepad_html );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *document_body_notepad_html(
		char *notepad );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *document_body_onload_open_tag(
		const char *javascript_wait_over,
		char *onload_javascript_string,
		char *menu_onload_string );

/* Usage */
/* ----- */

/* Returns javascript_string, append_javascript_string, heap memory or null */
/* ------------------------------------------------------------------------ */
char *document_body_set_onload_javascript_string(
		char *javascript_string,
		char *append_javascript_string );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *document_body_open_tag(
		void );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *document_body_close_tag(
		void );

typedef struct
{
	DOCUMENT_HEAD *document_head;
	DOCUMENT_BODY *document_body;
	char *type_html;
	char *standard_string;
	char *javascript_include_string;
	char *html;
} DOCUMENT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DOCUMENT *document_new(
		char *application_name,
		char *application_title_string,
		char *title_string,
		char *subtitle_string,
		char *sub_sub_title_string,
		char *notepad,
		char *onload_javascript_string,
		char *menu_html,
		char *document_head_menu_setup_string,
		char *document_head_calendar_setup_string,
		char *javascript_include_string );

/* Process */
/* ------- */
DOCUMENT *document_calloc(
		void );

/* Returns program memory */
/* ---------------------- */
char *document_type_html(
		void );

/* Returns program memory */
/* ---------------------- */
char *document_standard_string(
		void );

/* Returns static memory */
/* --------------------- */
char *document_html(
		char *document_type_html,
		char *document_standard_string );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DOCUMENT *document_basic_new(
		char *application_name,
		LIST *javascript_filename_list,
		char *application_title_string );

/* Usage */
/* ----- */
void document_process_output(
		char *application_name,
		LIST *javascript_filename_list,
		char *title_string );

/* Process */
/* ------- */

/* Returns static memory or null */
/* ----------------------------- */
char *document_process_title_string(
		char *title_string );

/* Returns heap memory */
/* ------------------- */
char *document_process_sub_title_string(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *document_form_html(
		char *document_html,
		char *document_head_html,
		char *document_head_close_tag,
		char *document_body_html,
		char *form_html,
		char *document_body_close_tag,
		char *document_close_tag );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *document_content_type_html(
		void );

/* Usage */
/* ----- */
void document_content_type_output(
		void );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *document_script_close_tag(
		void );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *document_close_tag(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *document_both_close_tag(
		void );

/* Usage */
/* ----- */

/* Outputs both </body> and </html> */
/* -------------------------------- */
void document_close(
		void );

/* Usage */
/* ----- */

/* Returns static memory or null */
/* ----------------------------- */
char *document_capacity_message(
		const char *function_name,
		const int max_strlen,
		const double appaserver_capacity_threshold,
		char *label,
		int strlen_html );

/* Process */
/* ------- */
double document_capacity(
		const int max_strlen,
		int strlen_html );

#endif
