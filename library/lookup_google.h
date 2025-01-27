/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/lookup_google.h				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef LOOKUP_GOOGLE_H
#define LOOKUP_GOOGLE_H

#include "boolean.h"
#include "list.h"
#include "chart.h"
#include "google_graph.h"

#define LOOKUP_GOOGLE_EXECUTABLE		"output_google_chart"

typedef struct
{
	LIST *google_graph_list;
} LOOKUP_GOOGLE_AGGREGATE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_GOOGLE_AGGREGATE *
	lookup_google_aggregate_new(
		char *application_name,
		char *session_key,
		char *folder_name,
		char *data_directory,
		QUERY_CHART *query_chart,
		LIST *chart_aggregate_attribute_list,
		int chart_number );

/* Process */
/* ------- */
LOOKUP_GOOGLE_AGGREGATE *
	lookup_google_aggregate_calloc(
		void );

typedef struct
{
	GOOGLE_GRAPH *google_graph;
	GOOGLE_PROMPT *google_prompt;
} LOOKUP_GOOGLE_GRAPH;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_GOOGLE_GRAPH *lookup_google_graph_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		char *date_attribute_name,
		char *time_attribute_name,
		LIST *query_fetch_row_list,
		char *number_attribute_name,
		char *chart_title,
		char *chart_filename_key,
		int chart_number );

/* Process */
/* ------- */
LOOKUP_GOOGLE_GRAPH *lookup_google_graph_calloc(
		void );

typedef struct
{
	CHART_INPUT *chart_input;
	LIST *lookup_google_graph_list;
	LOOKUP_GOOGLE_AGGREGATE *lookup_google_aggregate;
	GOOGLE_FILENAME *google_filename;
	GOOGLE_WINDOW *google_window;
} LOOKUP_GOOGLE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_GOOGLE *lookup_google_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string,
		char *data_directory );

/* Process */
/* ------- */
LOOKUP_GOOGLE *lookup_google_calloc(
		void );

/* Usage */
/* ----- */
GOOGLE_FILENAME *lookup_google_filename(
		LIST *lookup_google_graph_list );

#endif
