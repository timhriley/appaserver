/* $APPASERVER_HOME/library/query_statistic.c				*/
/* -------------------------------------------------------------------- */
/* This ADT supports the statistics push button on the prompt screens.  */
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef QUERY_STATISTIC_H
#define QUERY_STATISTIC_H

#include "list.h"

/* Constants */
/* --------- */
#define QUERY_STATISTIC_DELIMITER	'|'

typedef struct
{
	/* Input */
	/* ----- */
	char *attribute_name;
	boolean primary_key;
	char *attribute_datatype;
	char *units_string;

	/* Attributes */
	/* ---------- */
	unsigned long count;
	double sum;
	double average;
	double minimum;
	double median;
	double maximum;
	double range;
	double standard_deviation;
	double coefficient_of_variation;
	double standard_error;
	double percent_missing;
	char *begin_date;
	char *end_date;

	char *temp_filename;
	int input_piece;
} QUERY_STATISTIC_ATTRIBUTE;

/* QUERY_STATISTIC_ATTRIBUTE operations */
/* ------------------------------------ */

typedef struct
{
	/* Input */
	/* ----- */
	LIST *folder_attribute_number_name_list;
	LIST *folder_attribute_date_name_list;
	LIST *folder_attribute_date_time_name_list;
	char *from_clause;
	char *where_clause;

	/* Process */
	/* ------- */
	LIST *query_statistic_attribute_list;
} QUERY_STATISTIC;

/* QUERY_STATISTIC operations */
/* -------------------------- */
QUERY_STATISTIC *query_statistic_new(
			LIST *folder_attribute_number_name_list,
			LIST *folder_attribute_date_name_list,
			LIST *folder_attribute_date_time_name_list,
			char *from_clause,
			char *where_clause );

QUERY_STATISTIC_ATTRIBUTE *query_statistic_attribute_new(
			char *folder_attribute_name,
			int input_piece );

LIST *query_statistics_primary_list(
			LIST *attribute_list );

LIST *query_statistics_list(
			LIST *attribute_list );

char *query_query_attribute_statistics_sys_string(
			char *application_name,
			char *folder_name,
			LIST *query_statistics,
			char *where_clause );

void query_statistics_populate_list(
			LIST *query_statistics );

void query_statistics_output_table(
			char *folder_name,
			char *where_clause,
			LIST *query_statistics,
			char *application_name );

void query_statistics_output_folder_count(
			char *application_name,
			char *folder_name,
			char *where_clause,
			LIST *query_statistics );

char *query_statistics_build_each_temp_file_sys_string(
			char *application_name,
			char *folder_name,
			LIST *query_statistics,
			char *where_clause );

LIST *query_statistics_select_attribute_name_list(
			char *application_name,
			LIST *query_statistics );

char *query_attribute_statistics_temp_filename(
			char *attribute_name );

void query_attribute_statistics_remove_temp_file(
			LIST *query_statistics );

void query_statistics_set_units_string(
			LIST *query_statistics,
			char *attribute_name,
			char *units_string );

void query_attribute_display_where_clause(
			char *display_where_clause,
			char *where_clause,
			char *application_name,
			char *folder_name );

boolean query_attribute_is_valid_non_date_datatype(
			char *attribute_datatype );

boolean query_attribute_is_valid_date_datatype(
			char *attribute_datatype );

boolean query_attribute_is_valid_non_primary_datatype(
			char *attribute_datatype );

#endif
