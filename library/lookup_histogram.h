/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/lookup_histogram.h				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef LOOKUP_HISTOGRAM_H
#define LOOKUP_HISTOGRAM_H

#include "boolean.h"
#include "list.h"
#include "chart.h"
#include "appaserver_link.h"
#include "histogram.h"

#define LOOKUP_HISTOGRAM_EXECUTABLE		"output_histogram"
#define LOOKUP_HISTOGRAM_FILENAME_STEM		"histogram"

typedef struct
{
	APPASERVER_LINK *appaserver_link;
} LOOKUP_HISTOGRAM_FILENAME;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_HISTOGRAM_FILENAME *lookup_histogram_filename_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		char *number_attribute_name );

/* Process */
/* ------- */
LOOKUP_HISTOGRAM_FILENAME *lookup_histogram_filename_calloc(
		void );

/* Usage */
/* ----- */
LOOKUP_HISTOGRAM_FILENAME *lookup_histogram_filename(
		LIST *lookup_histogram_graph_list );

typedef struct
{
	char *html;
} LOOKUP_HISTOGRAM_WINDOW;

/* Usage */
/* ----- */
LOOKUP_HISTOGRAM_WINDOW *lookup_histogram_window_new(
		char *folder_name,
		LOOKUP_HISTOGRAM_FILENAME *
			lookup_histogram_filename,
		char *where_string,
		pid_t process_id );

/* Process */
/* ------- */
LOOKUP_HISTOGRAM_WINDOW *lookup_histogram_window_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *lookup_histogram_window_onload_string(
		char *http_prompt_filename,
		char *query_prompt_target_frame );

/* Returns static memory */
/* --------------------- */
char *lookup_histogram_window_title_html(
		char *folder_name,
		char *date_time_now );

/* Returns heap memory */
/* ------------------- */
char *lookup_histogram_window_html(
		char *document_body_onload_open_tag,
		char *lookup_histogram_window_title_html,
		char *query_where_display );

typedef struct
{
	char *html;
} LOOKUP_HISTOGRAM_PROMPT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_HISTOGRAM_PROMPT *lookup_histogram_prompt_new(
		char *http_prompt_filename,
		char *number_attribute_name,
		pid_t process_id );

/* Process */
/* ------- */
LOOKUP_HISTOGRAM_PROMPT *lookup_histogram_prompt_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *lookup_histogram_prompt_html(
		char *http_prompt_filename,
		char *number_attribute_name,
		char *query_prompt_target_frame );

typedef struct
{
	char *title;
	LOOKUP_HISTOGRAM_FILENAME *lookup_histogram_filename;
	HISTOGRAM *histogram;
	LOOKUP_HISTOGRAM_PROMPT *lookup_histogram_prompt;
} LOOKUP_HISTOGRAM_GRAPH;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_HISTOGRAM_GRAPH *lookup_histogram_graph_new(
		char *application_name,
		char *session_key,
		char *folder_name,
		char *data_directory,
		char *where_string,
		LIST *query_fetch_row_list,
		char *number_attribute_name );

/* Process */
/* ------- */
LOOKUP_HISTOGRAM_GRAPH *lookup_histogram_graph_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *lookup_histogram_graph_title(
		char *folder_name,
		char *number_attribute_name );

typedef struct
{
	CHART_INPUT *chart_input;
	LIST *lookup_histogram_graph_list;
	LOOKUP_HISTOGRAM_WINDOW *lookup_histogram_window;
	LOOKUP_HISTOGRAM_FILENAME *lookup_histogram_filename;
} LOOKUP_HISTOGRAM;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_HISTOGRAM *lookup_histogram_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string,
		char *data_directory );

/* Process */
/* ------- */
LOOKUP_HISTOGRAM *lookup_histogram_calloc(
		void );

#endif
