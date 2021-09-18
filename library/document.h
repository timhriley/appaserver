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
#include "form.h"

/* Constants */
/* --------- */

#define DOCUMENT_DYNARCH_MENU_ONLOAD_CONTROL_STRING \
"DynarchMenu.setup( 'menu', {electric: 250, blink: false,lazy: true, scrolling: true})"

/* Objects */
/* ------- */
typedef struct
{
	char *stylesheet_string;
	char *menu_setup_string;
	char *calendar_setup_string;
	char *meta_string;
	char *javascript_string;
} DOCUMENT_HEAD;

typedef struct
{
	FORM_PROMPT *form_prompt;
	FORM_TABLE *form_table;
	FORM_DETAIL *form_detail;
	MENU *menu;
} DOCUMENT_BODY;

typedef struct
{
	char *type_string;
	char *html_type_string;
	DOCUMENT_HEAD *document_head;
	char *title;
	char *type_string;
	char *error_function;
	char *application_name;
	char *onload_control_string;
	char *appaserver_mount_point;
	char *stylesheet_filename;
	LIST *javascript_list;
} DOCUMENT;

/* DOCUMENT operations */
/* ------------------- */
DOCUMENT *document_calloc(
			void );

DOCUMENT *document_new( char *title );

/* Returns program memory */
/* ---------------------- */
char *document_type_string(
			void );

/* Returns program memory */
/* ---------------------- */
char *document_html_standard_url(
			void );

void document_output_content_type(
			void );

void document_output_type_string(
			FILE *output_stream,
			char *type_string );

void document_output_html_tag(
			FILE *output_stream,
			char *html_standard_url );

/* Later execute document_close() */
/* ------------------------------ */
void document_quick_output(
			char *application_name );

void document_close(	FILE *output_stream );

/* DOCUMENT_HEAD operations */
/* ------------------------ */
DOCUMENT_HEAD *document_head_calloc(
			void );

DOCUMENT_HEAD *document_head_new(
			char *title,
			char *stylesheet_string,
			char *menu_setup_string,
			char *calendar_setup_string );

void document_head_quick_output(
			char *application_name );

/* Returns program memory */
/* ---------------------- */
char *document_head_meta_string(
			void );

char *document_head_javascript_string(
			void );

/* Safely returns heap memory */
/* -------------------------- */
char *document_head_stylesheet_string(
			char *application_name );

char *document_head_menu_setup_string(
			void );

void document_head_output(
			FILE *output_stream,
			char *title,
			char *sytlesheet_string,
			char *menu_setup_string,
			char *calendar_setup_string,
			char *meta_string,
			char *javascript_string );

/* DOCUMENT_BODY operations */
/* ------------------------ */
DOCUMENT_BODY *document_body_calloc(
			void );

DOCUMENT_BODY *document_body_new(
			char *title );

void document_body_quick_output(
			void );

void document_output_heading(		char *application_name,
					char *title,
					boolean output_content_type,
					char *appaserver_mount_point,
					LIST *javascript_module_list,
					char *stylesheet_filename,
					char *relative_source_directory,
					boolean with_dynarch_menu );

void document_output_head_stream(	FILE *output_stream,
					char *application_name,
					char *title,
					boolean output_content_type,
					char *appaserver_mount_point,
					LIST *javascript_module_list,
					char *stylesheet_filename,
					char *relative_source_directory,
					boolean with_dynarch_menu,
					boolean with_close_head );

void document_output_head(		char *application_name,
					char *title,
					boolean output_content_type,
					char *appaserver_mount_point,
					LIST *javascript_module_list,
					char *stylesheet_filename,
					char *relative_source_directory,
					boolean with_dynarch_menu );

void document_output_body_stream(	FILE *output_stream,
					char *application_name,
					char *onload_control_string );

void document_output_body( 		char *application_name,
					char *onload_control_string );

void document_close_body_stream(	FILE *output_stream );

void document_close_html_stream( 	FILE *output_stream );

void document_close_body( 		void );

void document_close_html( 		void );

void document_close_stream(		FILE *output_stream );

void document_set_javascript_module( 	DOCUMENT *d, 
					char *javascript_filename );

void document_output_each_javascript_source( 
					char *application_name,
					LIST *javascript_module_list,
					char *appaserver_mount_point,
					char *relative_source_directory );

void document_set_on_error_function( 	DOCUMENT *document,
					char *error_function );

void document_output_on_error_function( char *error_function );

void document_output_javascript_source(
				char *application_name,
				char *javascript_filename,
				char *appaserver_mount_point,
				char *relative_source_directory );

char *document_set_onload_control_string(
					char *document_onload_control_string,
					char *s );

void document_output_content_type(	void );

void document_output_closing( 		void );

void document_quick_output_head( 	char *application_name,
					char *appaserver_mount_point );

void document_quick_output_body( 	char *application_name,
					char *appaserver_mount_point );

void document_output_quick_body( 	char *application_name,
					char *appaserver_mount_point );

void document_set_folder_javascript_files(
					DOCUMENT *document,
					char *application_name,
					char *folder_name );

void document_set_process_javascript_files(
					DOCUMENT *document,
					char *application_name,
					char *process_name );

DOCUMENT_JAVASCRIPT_MODULE *document_javascript_module_new(
					LIST *javascript_module_list,
					char *javascript_filename );

void document_output_dynarch_non_frame_html_head_body(
				char *application_name,
				boolean content_type,
				char *dynarch_menu_onload_control_string,
				char *additional_control_string );

void document_output_dynarch_html_body(
				char *dynarch_menu_onload_control_string,
				char *additional_control_string );

void document_output_dynarch_hide_ul_contents(
					void );

void document_output_close_head(	FILE *output_stream );

DOCUMENT *document_output_menu_new(
					char *application_name,
					char *login_name,
					char *session,
					char *role_name,
					char *appaserver_mount_point,
					char *onload_control_string,
					boolean exists_date_attribute );

void document_output_dynarch_heading(
					FILE *output_stream );

void document_output_html_stream(
					FILE *output_stream );

void document_output_stylesheet(
			FILE *output_stream,
			char *application_name );

#endif
