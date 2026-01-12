/* $APPASERVER_HOME/library/lookup_statistic.c				*/
/* -------------------------------------------------------------------- */
/* This ADT supports the statistics push button on the prompt screens.  */
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef LOOKUP_STATISTIC_H
#define LOOKUP_STATISTIC_H

#include "list.h"
#include "boolean.h"
#include "role.h"
#include "folder.h"
#include "security.h"
#include "post_dictionary.h"
#include "dictionary_separate.h"
#include "folder_row_level_restriction.h"
#include "html.h"
#include "statistic.h"
#include "drillthru.h"
#include "query_statistic.h"

#define LOOKUP_STATISTIC_EXECUTABLE	"lookup_statistic"

typedef struct
{
	HTML_TABLE *html_table;
} LOOKUP_STATISTIC_TABLE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_STATISTIC_TABLE *lookup_statistic_table_new(
		int attribute_number_count,
		LIST *query_statistic_fetch_attribute_list,
		char *outlier_low_heading_string,
		char *outlier_high_heading_string,
		int lookup_statistic_after_minimum_blank_count,
		int lookup_statistic_after_range_blank_count );

/* Process */
/* ------- */
LOOKUP_STATISTIC_TABLE *lookup_statistic_table_calloc(
		void );

/* Usage */
/* ----- */
LIST *lookup_statistic_table_row_list(
		int attribute_number_count,
		LIST *query_statistic_fetch_attribute_list,
		int lookup_statistic_after_minimum_blank_count,
		int lookup_statistic_after_range_blank_count );

/* Usage */
/* ----- */
HTML_ROW *lookup_statistic_table_row_new(
		int attribute_number_count,
		QUERY_STATISTIC_FETCH_ATTRIBUTE *
			query_statistic_fetch_attribute,
		int lookup_statistic_after_minimum_blank_count,
		int lookup_statistic_after_range_blank_count );

/* Usage */
/* ----- */
LIST *lookup_statistic_table_cell_list(
		int attribute_number_count,
		QUERY_STATISTIC_FETCH_ATTRIBUTE *
			query_statistic_fetch_attribute,
		int lookup_statistic_after_minimum_blank_count,
		int lookup_statistic_after_range_blank_count );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *lookup_statistic_table_primary_label(
		char *attribute_name );

/* Usage */
/* ----- */
LIST *lookup_statistic_table_number_cell_list(
		int attribute_number_count,
		STATISTIC *statistic,
		char *label );

/* Process */
/* ------- */
boolean lookup_statistic_table_outlier_boolean(
		int attribute_number_count );

/* Usage */
/* ----- */
LIST *lookup_statistic_table_date_cell_list(
		STATISTIC_DATE *statistic_date,
		char *label,
		int lookup_statistic_after_minimum_blank_count,
		int lookup_statistic_after_range_blank_count );

/* Usage */
/* ----- */
LIST *lookup_statistic_table_primary_cell_list(
		unsigned long row_count,
		char *lookup_statistic_table_primary_label,
		int lookup_statistic_after_minimum_blank_count,
		int lookup_statistic_after_range_blank_count );

/* Process */
/* ------- */
int lookup_statistic_table_blank_count(
		int lookup_statistic_after_minimum_blank_count,
		int lookup_statistic_after_range_blank_count );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *lookup_statistic_table_label(
		char *attribute_name );

typedef struct
{
	ROLE *role;
	LIST *role_attribute_exclude_lookup_name_list;
	FOLDER *folder;
	LIST *relation_mto1_list;
	LIST *relation_mto1_isa_list;
	LIST *folder_attribute_append_isa_list;
	char *primary_key_first;
	FOLDER_ROW_LEVEL_RESTRICTION *folder_row_level_restriction;
	SECURITY_ENTITY *security_entity;
	POST_DICTIONARY *post_dictionary;
	LIST *folder_attribute_number_name_list;
	LIST *folder_attribute_date_name_list;
	DICTIONARY_SEPARATE_POST_PROMPT_LOOKUP *dictionary_separate;
	DRILLTHRU_STATUS *drillthru_status;
	LIST *attribute_number_name_list;
	int attribute_number_count;
	LIST *attribute_date_name_list;
} LOOKUP_STATISTIC_INPUT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_STATISTIC_INPUT *lookup_statistic_input_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string );

/* Process */
/* ------- */
LOOKUP_STATISTIC_INPUT *lookup_statistic_input_calloc(
		void );

/* Returns primary_key_first */
/* ------------------------- */
char *lookup_statistic_input_primary_key_first(
		char *primary_key_first );

LIST *lookup_statistic_input_attribute_number_name_list(
		LIST *folder_attribute_number_name_list,
		LIST *no_display_name_list );

int lookup_statistic_input_attribute_number_count(
	LIST *lookup_statistic_input_attribute_number_name_list );

LIST *lookup_statistic_input_attribute_date_name_list(
		LIST *folder_attribute_date_name_list,
		LIST *no_display_name_list );

typedef struct
{
	LOOKUP_STATISTIC_INPUT *lookup_statistic_input;
	QUERY_STATISTIC *query_statistic;
	int after_minimum_blank_count;
	int after_range_blank_count;
	LOOKUP_STATISTIC_TABLE *lookup_statistic_table;
	char *sub_title_string;
} LOOKUP_STATISTIC;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LOOKUP_STATISTIC *lookup_statistic_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name,
		char *dictionary_string );

/* Process */
/* ------- */
LOOKUP_STATISTIC *lookup_statistic_calloc(
		void );

int lookup_statistic_after_minimum_blank_count(
		void );

int lookup_statistic_after_range_blank_count(
		char *outlier_low_heading_string,
		char *outlier_high_heading_string );

/* Returns query_table_edit_where or program memory */
/* ------------------------------------------------ */
char *lookup_statistic_sub_title_string(
		char *query_table_edit_where_string );

/* Driver */
/* ------ */
void lookup_statistic_output(
		char *application_name,
		LOOKUP_STATISTIC *lookup_statistic );

#endif
