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
} QUERY_STATISTIC_ATTRIBUTE_AGGREGATE;

/* QUERY_STATISTIC_ATTRIBUTE_AGGREGATE operations */
/* ---------------------------------------------- */
QUERY_STATISTIC_ATTRIBUTE_AGGREGATE *
	query_statistic_attribute_aggregate_calloc(
			void );

QUERY_STATISTIC_ATTRIBUTE_AGGREGATE *
	query_statistic_attribute_aggregate_new(
			boolean is_date,
			int piece_offset,
			LIST *record_list );

FILE *query_statistic_attribute_aggregate_output_pipe(
			boolean is_date,
			char *output_filename );

void query_statistic_attribute_aggregate_generate(
			FILE *output_pipe,
			int piece_offset,
			LIST *record_list );

QUERY_STATISTIC_ATTRIBUTE_AGGREGATE *
	query_statistic_attribute_aggregate_file_read(
			boolean is_date,
			char *query_statistic_attribute_aggregate_filename );

typedef struct
{
	/* Input */
	/* ----- */
	char *folder_attribute_name;
	boolean is_date;
	int piece_offset;
	LIST *query_statistic_record_list;

	/* Process */
	/* ------- */
	QUERY_STATISTIC_ATTRIBUTE_AGGREGATE *
		query_statistic_attribute_aggregate;

	char *html;
} QUERY_STATISTIC_ATTRIBUTE;

/* QUERY_STATISTIC_ATTRIBUTE operations */
/* ------------------------------------ */
QUERY_STATISTIC_ATTRIBUTE *
	query_statistic_attribute_calloc(
			void );

QUERY_STATISTIC_ATTRIBUTE *
	query_statistic_attribute_new(
			char *folder_attribute_name,
			boolean is_date,
			int piece_offset,
			LIST *query_statistic_record_list );

/* Returns static memory */
/* --------------------- */
char *query_statistic_attribute_html(
			char *folder_attribute_name,
			boolean is_date,
			QUERY_STATISTIC_ATTRIBUTE_AGGREGATE * );

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
	LIST *record_list;
	LIST *query_statistic_attribute_list;
	char *html;
} QUERY_STATISTIC;

/* QUERY_STATISTIC operations */
/* -------------------------- */
QUERY_STATISTIC *query_statistic_calloc(
			void );

QUERY_STATISTIC *query_statistic_new(
			LIST *folder_attribute_number_name_list,
			LIST *folder_attribute_date_name_list,
			LIST *folder_attribute_date_time_name_list,
			char *from_clause,
			char *where_clause );

LIST *query_statistic_record_list(
			LIST *folder_attribute_number_name_list,
			LIST *folder_attribute_date_name_list,
			LIST *folder_attribute_date_time_name_list,
			char *from_clause,
			char *where_clause );

/* Returns heap memory */
/* ------------------- */
char *query_statistic_html(
			LIST *query_statistic_attribute_list );

#endif
