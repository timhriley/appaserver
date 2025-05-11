/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/chart.h					*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef CHART_H
#define CHART_H

#include "boolean.h"
#include "list.h"
#include "role.h"
#include "folder.h"
#include "security.h"
#include "post_dictionary.h"
#include "folder_row_level_restriction.h"
#include "dictionary_separate.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "query.h"

typedef struct
{
	LIST *query_row_list;
} CHART_AGGREGATE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CHART_AGGREGATE *chart_aggregate_new(
		char *date_attribute_name,
		char *number_attribute_name,
		LIST *query_fetch_row_list,
		char *chart_aggregate_list_end_date,
		enum aggregate_statistic aggregate_statistic,
		enum aggregate_level aggregate_level );

/* Process */
/* ------- */
CHART_AGGREGATE *chart_aggregate_calloc(
		void );

/* Usage */
/* ----- */
LIST *chart_aggregate_query_row_list(
		char *date_attribute_name,
		char *number_attribute_name,
		LIST *chart_aggregate_fetch_list );

/* Usage */
/* ----- */
LIST *chart_aggregate_query_cell_list(
		char *date_attribute_name,
		char *number_attribute_name,
		char *date_string,
		char *value_string );

typedef struct
{
	char *number_attribute_name;
	LIST *annual_average_row_list;
	LIST *annual_sum_row_list;
	LIST *month_average_row_list;
	LIST *month_sum_row_list;
	LIST *week_average_row_list;
	LIST *week_sum_row_list;
} CHART_AGGREGATE_ATTRIBUTE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CHART_AGGREGATE_ATTRIBUTE *
	chart_aggregate_attribute_new(
		char *date_attribute_name,
		char *number_attribute_name,
		LIST *query_fetch_row_list,
		int query_row_list_days_range,
		char *chart_aggregate_list_end_date );

/* Process */
/* ------- */
CHART_AGGREGATE_ATTRIBUTE *
	chart_aggregate_attribute_calloc(
		void );

typedef struct
{
	char *end_date;
	LIST *chart_aggregate_attribute_list;
} CHART_AGGREGATE_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CHART_AGGREGATE_LIST *chart_aggregate_list_new(
		char *date_attribute_name,
		char *time_attribute_name,
		LIST *query_select_name_list,
		LIST *query_fetch_row_list,
		int query_row_list_days_range );

/* Process */
/* ------- */
CHART_AGGREGATE_LIST *chart_aggregate_list_calloc(
		void );

/* Returns a component of the parameter */
/* ------------------------------------ */
char *chart_aggregate_list_end_date(
	LIST *query_fetch_row_list );

typedef struct
{
	char *date_string;
	char *value_string;
} CHART_AGGREGATE_FETCH;

/* Usage */
/* ----- */
LIST *chart_aggregate_fetch_list(
		LIST *query_fetch_row_list,
		char *attribute_name,
		char *chart_aggregate_list_end_date,
		enum aggregate_statistic aggregate_statistic,
		enum aggregate_level aggregate_level );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CHART_AGGREGATE_FETCH *
	chart_aggregate_fetch_parse(
		char *input_string );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CHART_AGGREGATE_FETCH *
	chart_aggregate_fetch_new(
		char *date_string,
		char *value_string );

/* Process */
/* ------- */
CHART_AGGREGATE_FETCH *
	chart_aggregate_fetch_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *chart_aggregate_fetch_system_string(
		char *chart_aggregate_list_end_date,
		enum aggregate_statistic aggregate_statistic,
		enum aggregate_level aggregate_level );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *chart_aggregate_fetch_output_string(
		char *attribute_name,
		LIST *cell_list );

typedef struct
{
	ROLE *role;
	LIST *role_attribute_exclude_lookup_name_list;
	FOLDER *folder;
	LIST *relation_mto1_list;
	LIST *relation_mto1_isa_list;
	LIST *folder_attribute_append_isa_list;
	LIST *folder_attribute_date_name_list;
	LIST *folder_attribute_name_list;
	FOLDER_ROW_LEVEL_RESTRICTION *
		folder_row_level_restriction;
	SECURITY_ENTITY *security_entity;
	POST_DICTIONARY *post_dictionary;
	DICTIONARY_SEPARATE_POST_PROMPT_LOOKUP *
		dictionary_separate;
	QUERY_CHART *query_chart;
	int row_list_length;
	int query_row_list_days_range;
	CHART_AGGREGATE_LIST *chart_aggregate_list;
} CHART_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CHART_INPUT *chart_input_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string );

/* Process */
/* ------- */
CHART_INPUT *chart_input_calloc(
			void );

int chart_input_row_list_length(
		LIST *query_fetch_row_list );

typedef struct
{
	/* Stub */
	/* ---- */
} CHART;

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *chart_title(
		char *folder_name,
		char *number_attribute_name,
		enum aggregate_statistic,
		enum aggregate_level,
		char *where_string );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *chart_filename_key(
		char *filename_stem,
		char *number_attribute_name,
		enum aggregate_statistic,
		enum aggregate_level );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *chart_window_sub_title_display(
		char *sub_title );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *chart_window_html(
		char *document_body_onload_open_tag,
		char *screen_title,
		char *sub_title_display );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *chart_window_onload_string(
		char *prompt_filename,
		char *target );

#endif
