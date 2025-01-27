/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/google_graph.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef GOOGLE_GRAPH_H
#define GOOGLE_GRAPH_H

#include "boolean.h"
#include "list.h"
#include "appaserver_link.h"
#include "query.h"
#include "google.h"

#define GOOGLE_GRAPH_CONVERT_MONTH_EXPRESSION	atoi( month ) - 1

typedef struct
{
	boolean null_boolean;
	char *display;
} GOOGLE_GRAPH_VALUE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GOOGLE_GRAPH_VALUE *google_graph_value_new(
		char *value_string );

/* Process */
/* ------- */
GOOGLE_GRAPH_VALUE *google_graph_value_calloc(
		void );

boolean google_graph_value_null_boolean(
		char *value_string );

/* Usage */
/* ----- */

/* Returns value_string or program memory */
/* -------------------------------------- */
char *google_graph_value_display(
		char *value_string,
		boolean google_graph_value_null_boolean );

typedef struct
{
	char *javascript;
} GOOGLE_GRAPH_POINT;

/* Usage */
/* ----- */
LIST *google_graph_point_list(
		char *date_attribute_name,
		char *time_attribute_name,
		char *number_attribute_name,
		LIST *query_fetch_row_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *google_graph_point_date_javascript(
		char *yyyy_mm_dd,
		char *hhmm );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *google_graph_point_javascript(
		QUERY_CELL *date_query_cell,
		QUERY_CELL *time_query_cell,
		QUERY_CELL *number_query_cell );

/* Process */
/* ------- */

/* Returns component of time_query_cell or null */
/* -------------------------------------------- */
char *google_graph_point_hhmm(
		QUERY_CELL *time_query_cell );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GOOGLE_GRAPH_POINT *google_graph_point_new(
		char *google_graph_point_javascript );

/* Process */
/* ------- */
GOOGLE_GRAPH_POINT *google_graph_point_calloc(
		void );

typedef struct
{
	GOOGLE_FILENAME *google_filename;
	LIST *google_graph_point_list;
	char *document_open_html;
	char *document_close_html;
} GOOGLE_GRAPH;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
GOOGLE_GRAPH *google_graph_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		char *date_attribute_name,
		char *time_attribute_name,
		char *number_attribute_name,
		LIST *query_fetch_row_list,
		char *chart_title,
		char *chart_filename_key,
		int chart_number,
		char *google_graph_function
			/* either LineChart or ColumnChart */ );

/* Process */
/* ------- */
GOOGLE_GRAPH *google_graph_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *google_graph_number_attribute_label(
		char *number_attribute_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *google_graph_visualization_function_name(
		int chart_number );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *google_graph_id_string(
		int chart_number );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *google_graph_loader_script_html(
		void );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *google_graph_corechart_script_html(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *google_graph_function_script_open(
		char *google_graph_visualization_function_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *google_graph_datatable_statement(
		char *google_graph_number_attribute_label );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *google_graph_options_JSON(
		char *chart_title );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *google_graph_instantiation_statement(
		char *google_graph_id_string,
		char *google_graph_function
			/* either LineChart or ColumnChart */ );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *google_graph_draw_statement(
		void );

/* Usage */
/* ----- */

/* Returns program memory */
/* ---------------------- */
char *google_graph_function_script_close(
	void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *google_graph_set_callback_script_html(
		char *google_graph_visualization_function_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *google_graph_division_tag(
		char *google_graph_id_string );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *google_graph_document_open_html(
		char *google_graph_loader_script_html,
		char *google_graph_set_callback_script_html,
		char *google_graph_corechart_script_html,
		char *google_graph_function_script_open,
		char *google_graph_datatable_statement );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *google_graph_document_close_html(
		char *google_graph_options_JSON,
		char *google_graph_instantiation_statement,
		char *google_graph_draw_statement,
		char *google_graph_function_script_close,
		char *google_graph_set_callback_script_html,
		char *google_graph_division_tag );

/* Driver */
/* ------ */
void google_graph_output(
		GOOGLE_GRAPH *google_graph,
		FILE *output_file );

#endif
