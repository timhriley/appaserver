/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/rchart.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef RCHART_H
#define RCHART_H

#include "boolean.h"
#include "list.h"
#include "appaserver_link.h"
#include "query.h"

#define RCHART_FILENAME_STEM	"rchart"

typedef struct
{
	pid_t process_id;
	APPASERVER_LINK *appaserver_link;
	char *R_output_filename;
	char *R_prompt_filename;
	char *pdf_output_filename;
	char *pdf_prompt_filename;
} RCHART_FILENAME;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RCHART_FILENAME *rchart_filename_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		char *chart_filename_key );

/* Process */
/* ------- */
RCHART_FILENAME *rchart_filename_calloc(
		void );

pid_t rchart_filename_process_id(
		char *session_key );

/* Returns heap memory */
/* ------------------- */
char *rchart_filename_R_output_filename(
		APPASERVER_LINK *appaserver_link );

/* Returns heap memory */
/* ------------------- */
char *rchart_filename_R_prompt_filename(
		APPASERVER_LINK *appaserver_link );

/* Returns heap memory */
/* ------------------- */
char *rchart_filename_pdf_output_filename(
		APPASERVER_LINK *appaserver_link );

/* Returns heap memory */
/* ------------------- */
char *rchart_filename_pdf_prompt_filename(
		APPASERVER_LINK *appaserver_link );

typedef struct
{
	char *pdf_target;
	char *pdf_anchor_html;
	char *R_target;
	char *R_anchor_html;
	char *html;
} RCHART_PROMPT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RCHART_PROMPT *rchart_prompt_new(
		RCHART_FILENAME *rchart_filename,
		char *filename_key,
		pid_t process_id );

/* Process */
/* ------- */
RCHART_PROMPT *rchart_prompt_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *rchart_prompt_pdf_anchor_html(
		char *pdf_prompt_filename,
		char *filename_key,
		char *rchart_prompt_pdf_target );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *rchart_prompt_R_anchor_html(
		char *R_prompt_filename,
		char *filename_key,
		char *rchart_prompt_R_target );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *rchart_prompt_pdf_target(
		char *filename_key,
		pid_t process_id );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *rchart_prompt_R_target(
		char *filename_key,
		pid_t process_id );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *rchart_prompt_html(
		char *rchart_prompt_pdf_anchor_html,
		char *rchart_prompt_R_anchor_html );

typedef struct
{
	char *onload_string;
	char *date_time_now;
	char *screen_title;
	char *html;
} RCHART_WINDOW;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RCHART_WINDOW *rchart_window_new(
		RCHART_FILENAME *rchart_filename,
		char *sub_title,
		char *filename_key,
		pid_t process_id );

/* Process */
/* ------- */
RCHART_WINDOW *rchart_window_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *rchart_window_screen_title(
		char *date_time_now );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *rchart_window_html(
		char *document_body_onload_open_tag,
		char *screen_title,
		char *sub_title_display );

typedef struct
{
	char *date_time_string;
	double value;
} RCHART_POINT;

/* Usage */
/* ----- */
LIST *rchart_point_list(
		char *date_attribute_name,
		char *time_attribute_name,
		char *number_attribute_name,
		LIST *query_fetch_row_list );

/* Process */
/* ------- */
boolean rchart_point_null_boolean(
		char *number_query_cell_select_datum );

double rchart_point_value(
		QUERY_CELL *number_query_cell );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *rchart_point_date_time_string(
		QUERY_CELL *date_query_cell,
		QUERY_CELL *time_query_cell );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *rchart_point_hhmmss(
		QUERY_CELL *time_query_cell );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RCHART_POINT *rchart_point_new(
		char *rchart_point_date_time_string,
		double rchart_point_value );

/* Process */
/* ------- */
RCHART_POINT *rchart_point_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *rchart_point_combine_string(
		char *attribute_name,
		LIST *rchart_point_list,
		boolean date_time_boolean );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *rchart_point_string(
		char *rchart_point_date_time_string,
		double rchart_point_value,
		boolean date_time_boolean );

/* Usage */
/* ----- */
void rchart_point_list_free(
		LIST *rchart_point_list );

typedef struct
{
	RCHART_FILENAME *rchart_filename;
	char *file_header_string;
	LIST *rchart_point_list;
	char *rchart_point_date_combine_string;
	char *rchart_point_value_combine_string;
	char *file_trailer_string;
	char *script_system_string;
} RCHART;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RCHART *rchart_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		char *date_attribute_name,
		char *time_attribute_name,
		char *number_attribute_name,
		LIST *query_fetch_row_list,
		char *sub_title,
		char *chart_title,
		char *chart_filename_key );

/* Process */
/* ------- */
RCHART *rchart_calloc(
		void );

/* Returns program memory */
/* ---------------------- */
char *rchart_file_header_string(
		void );

/* Returns heap memory */
/* ------------------- */
char *rchart_file_trailer_string(
		char *date_attribute_name,
		char *number_attribute_name,
		char *sub_title,
		char *chart_title,
		char *pdf_output_filename );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *rchart_script_system_string(
		char *R_output_filename );

/* Usage */
/* ----- */
void rchart_free(
		RCHART *rchart );

/* Usage */
/* ----- */
void rchart_write(
		char *R_output_filename,
		char *rchart_file_header_string,
		char *rchart_point_date_combine_string,
		char *rchart_point_value_combine_string,
		char *rchart_file_trailer_string );

#endif
