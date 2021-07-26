/* $APPASERVER_HOME/library/query_attribute_statistics_list.c		*/
/* -------------------------------------------------------------------- */
/* This ADT supports the statistics push button on the prompt screens.  */
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef QUERY_ATTRIBUTE_STATISTICS_H
#define QUERY_ATTRIBUTE_STATISTICS_H

#include "list.h"

/* Constants */
/* --------- */
#define QUERY_ATTRIBUTE_STATISTICS_DELIMITER	'|'

typedef struct
{
	char *folder_name;
	char *attribute_name;
	boolean primary_key;
	char *attribute_datatype;
	char *attribute_statistics_temp_filename;
	char *units_string;
	int input_piece;
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
} QUERY_ATTRIBUTE_STATISTICS;

typedef struct
{
	char *application_name;
	char *folder_name;
	LIST *list;
} QUERY_ATTRIBUTE_STATISTICS_LIST;

QUERY_ATTRIBUTE_STATISTICS_LIST *query_attribute_statistics_list_new(
			char *application_name,
			char *folder_name );

QUERY_ATTRIBUTE_STATISTICS *query_attribute_statistics_new(
			char *attribute_name,
			char *attribute_datatype,
			int input_piece );

LIST *query_attribute_statistics_list_primary_list(
			LIST *attribute_list );

LIST *query_attribute_statistics_list_list(
			LIST *attribute_list );

char *query_query_attribute_statistics_sys_string(
			char *application_name,
			char *folder_name,
			LIST *query_attribute_statistics_list,
			char *where_clause );

void query_attribute_statistics_list_populate_list(
			LIST *query_attribute_statistics_list );

void query_attribute_statistics_list_output_table(
			char *folder_name,
			char *where_clause,
			LIST *query_attribute_statistics_list,
			char *application_name );

void query_attribute_statistics_list_output_folder_count(
			char *application_name,
			char *folder_name,
			char *where_clause,
			LIST *query_attribute_statistics_list );

char *query_attribute_statistics_list_build_each_temp_file_sys_string(
			char *application_name,
			char *folder_name,
			LIST *query_attribute_statistics_list,
			char *where_clause );

LIST *query_attribute_statistics_list_select_attribute_name_list(
			char *application_name,
			LIST *query_attribute_statistics_list );

char *query_attribute_statistics_temp_filename(
			char *attribute_name );

void query_attribute_statistics_remove_temp_file(
			LIST *query_attribute_statistics_list );

void query_attribute_statistics_list_set_units_string(
			LIST *query_attribute_statistics_list,
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
