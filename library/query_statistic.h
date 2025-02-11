/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/query_statistic.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef QUERY_STATISTIC_H
#define QUERY_STATISTIC_H

#include "list.h"
#include "boolean.h"
#include "query.h"
#include "statistic.h"

typedef struct
{
	char *attribute_name;
	boolean attribute_is_primary_first;
	boolean attribute_is_number;
	boolean attribute_is_date;
	unsigned long row_count;
	STATISTIC *statistic;
	STATISTIC_DATE *statistic_date;
} QUERY_STATISTIC_FETCH_ATTRIBUTE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_STATISTIC_FETCH_ATTRIBUTE *
	query_statistic_fetch_attribute_new(
		char sql_delimiter,
		char *attribute_name,
		int value_piece,
		boolean attribute_is_primary_first,
		boolean attribute_is_number,
		boolean attribute_is_date );

/* Process */
/* ------- */
QUERY_STATISTIC_FETCH_ATTRIBUTE *query_statistic_fetch_attribute_calloc(
		void );

/* Usage */
/* ----- */
void query_statistic_fetch_attribute_accumulate(
		QUERY_STATISTIC_FETCH_ATTRIBUTE *
			query_statistic_fetch_attribute,
		char *input );

/* Usage */
/* ----- */
void query_statistic_fetch_attribute_calculate_set(
		LIST *attribute_list /* in/out */ );

typedef struct
{
	LIST *attribute_list;
	int numeric_attribute_count;
	char *outlier_low_heading_string;
	char *outlier_high_heading_string;
} QUERY_STATISTIC_FETCH;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_STATISTIC_FETCH *query_statistic_fetch_new(
		char sql_delimiter,
		char *primary_key_first,
		LIST *attribute_number_name_list,
		LIST *attribute_date_name_list,
		char *query_system_string );

/* Process */
/* ------- */
QUERY_STATISTIC_FETCH *query_statistic_fetch_calloc(
		void );

FILE *query_statistic_fetch_input_pipe(
		char *query_system_string );

int query_statistic_fetch_numeric_attribute_count(
		int folder_attribute_number_name_list_length );

/* Returns component of attribute_list or null */
/* ------------------------------------------- */
char *query_statistic_fetch_outlier_low_heading_string(
		LIST *attribute_list );

/* Returns component of attribute_list or null */
/* ------------------------------------------- */
char *query_statistic_fetch_outlier_high_heading_string(
		LIST *attribute_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LIST *query_statistic_fetch_attribute_list(
		char sql_delimiter,
		char *primary_key_first,
		LIST *attribute_number_name_list,
		LIST *attribute_date_name_list );

/* Process */
/* ------- */
boolean query_statistic_fetch_first_number_boolean(
		char *primary_key_first,
		LIST *attribute_number_name_list );

boolean query_statistic_fetch_first_date_boolean(
		char *primary_key_first,
		LIST *attribute_date_name_list );

typedef struct
{
	LIST *select_list;
	char *query_select_list_string;
	LIST *query_select_name_list;
	char *query_from_string;
	QUERY_TABLE_EDIT_WHERE *query_table_edit_where;
	char *query_system_string;
	QUERY_STATISTIC_FETCH *query_statistic_fetch;
} QUERY_STATISTIC;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
QUERY_STATISTIC *query_statistic_new(
		char *application_name,
		char *folder_name,
		/* --------------------- */
		/* Static memory or null */
		/* --------------------- */
		char *security_entity_where,
		DICTIONARY *query_dictionary,
		LIST *folder_attribute_append_isa_list,
		LIST *relation_mto1_list,
		LIST *relation_mto1_isa_list,
		char *primary_key_first,
		LIST *attribute_number_name_list,
		LIST *attribute_date_name_list );

/* Process */
/* ------- */
QUERY_STATISTIC *query_statistic_calloc(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LIST *query_statistic_select_list(
		char *application_name,
		char *folder_name,
		LIST *folder_attribute_append_isa_list,
		int relation_mto1_isa_list_length,
		char *primary_key_first,
		LIST *attribute_number_name_list,
		LIST *attribute_date_name_list );

/* Usage */
/* ----- */

/* Returns folder_name or a component of folder_attribute_append_isa_list */
/* ---------------------------------------------------------------------- */
char *query_statistic_select_folder_name(
		char *folder_name,
		LIST *folder_attribute_append_isa_list,
		int relation_mto1_isa_list_length,
		char *attribute_name );

#endif
