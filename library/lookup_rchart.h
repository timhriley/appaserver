/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/lookup_rchart.h				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef LOOKUP_RCHART_H
#define LOOKUP_RCHART_H

#include "boolean.h"
#include "list.h"
#include "chart.h"
#include "aggregate_statistic.h"
#include "aggregate_level.h"
#include "rchart.h"

#define LOOKUP_RCHART_EXECUTABLE		"output_rchart"

typedef struct
{
	LIST *rchart_chart_list;
} LOOKUP_RCHART_AGGREGATE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_RCHART_AGGREGATE *
	lookup_rchart_aggregate_new(
		char *application_name,
		char *session_key,
		char *folder_name,
		char *data_directory,
		QUERY_CHART *query_chart,
		LIST *chart_aggregate_attribute_list );

/* Process */
/* ------- */
LOOKUP_RCHART_AGGREGATE *
	lookup_rchart_aggregate_calloc(
		void );

typedef struct
{
	char *query_where_display;
	RCHART *rchart;
	RCHART_PROMPT *rchart_prompt;
} LOOKUP_RCHART_CHART;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_RCHART_CHART *lookup_rchart_chart_new(
		char *application_name,
		char *session_key,
		char *data_directory,
		char *sub_title,
		char *date_attribute_name,
		char *time_attribute_name,
		char *number_attribute_name,
		LIST *query_fetch_row_list,
		char *chart_title,
		char *chart_filename_key );

/* Process */
/* ------- */
LOOKUP_RCHART_CHART *lookup_rchart_chart_calloc(
		void );

typedef struct
{
	CHART_INPUT *chart_input;
	LIST *lookup_rchart_chart_list;
	LOOKUP_RCHART_AGGREGATE *lookup_rchart_aggregate;
	RCHART_FILENAME *rchart_filename;
	RCHART_WINDOW *rchart_window;
} LOOKUP_RCHART;

/* Usage */
/* ----- */
LOOKUP_RCHART *lookup_rchart_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string,
		char *data_directory );

/* Process */
/* ------- */
LOOKUP_RCHART *lookup_rchart_calloc(
		void );

/* Usage */
/* ----- */
RCHART_FILENAME *lookup_rchart_filename(
		LIST *lookup_rchart_chart_list );

#endif
