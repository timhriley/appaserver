/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/lookup_grace.h				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef LOOKUP_GRACE_H
#define LOOKUP_GRACE_H

#include "boolean.h"
#include "list.h"
#include "chart.h"
#include "aggregate_statistic.h"
#include "aggregate_level.h"
#include "grace.h"

#define LOOKUP_GRACE_EXECUTABLE		"output_grace_chart"

typedef struct
{
	LIST *grace_graph_list;
} LOOKUP_GRACE_AGGREGATE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_GRACE_AGGREGATE *
	lookup_grace_aggregate_new(
		char *application_name,
		char *session_key,
		char *folder_name,
		char *data_directory,
		QUERY_CHART *query_chart,
		int query_row_list_days_range,
		LIST *chart_aggregate_attribute_list );

/* Process */
/* ------- */
LOOKUP_GRACE_AGGREGATE *
	lookup_grace_aggregate_calloc(
		void );

typedef struct
{
	GRACE_UNITLESS *grace_unitless;
	GRACE_PROMPT *grace_prompt;
} LOOKUP_GRACE_GRAPH;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_GRACE_GRAPH *lookup_grace_graph_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		char *sub_title,
		char *date_attribute_name,
		char *time_attribute_name,
		LIST *query_fetch_row_list,
		int horizontal_range_integer,
		char *lookup_grace_title,
		char *grace_datatype_name,
		char *lookup_grace_filename_key );

/* Process */
/* ------- */
LOOKUP_GRACE_GRAPH *lookup_grace_graph_calloc(
		void );

typedef struct
{
	CHART_INPUT *chart_input;
	LIST *lookup_grace_graph_list;
	LOOKUP_GRACE_AGGREGATE *lookup_grace_aggregate;
	GRACE_FILENAME *grace_filename;
	GRACE_WINDOW *grace_window;
} LOOKUP_GRACE;

/* Usage */
/* ----- */
LOOKUP_GRACE *lookup_grace_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string,
		char *data_directory );

/* Process */
/* ------- */
LOOKUP_GRACE *lookup_grace_calloc(
		void );

/* Usage */
/* ----- */
GRACE_FILENAME *lookup_grace_filename(
		LIST *lookup_grace_graph_list );

#endif
