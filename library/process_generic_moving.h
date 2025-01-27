/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process_generic_moving.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef PROCESS_GENERIC_MOVING_H
#define PROCESS_GENERIC_MOVING_H

#include "list.h"
#include "boolean.h"
#include "dictionary.h"
#include "grace.h"
#include "google_chart.h"
#include "spreadsheet.h"
#include "html_table.h"
#include "process_generic.h"

typedef struct
{
	LIST *heading_list;
	LIST *row_list;
	HTML_TABLE *html_table;
} PROCESS_GENERIC_MOVING_TABLE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_MOVING_TABLE *process_generic_moving_table_new(
		char *process_generic_sub_title,
		LIST *process_generic_point_list );

/* Process */
/* ------- */
PROCESS_GENERIC_MOVING_TABLE *process_generic_moving_table_calloc(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LIST *process_generic_moving_table_heading_list(
		void );

/* Usage */
/* ----- */
LIST *process_generic_moving_table_row_list(
			LIST *process_generic_point_list );

typedef struct
{
	LIST *point_list;
	int process_generic_grace_horizontal_range;
	GRACE_GENERIC *grace_generic;
	GRACE_WINDOW *grace_window;
	GRACE_PROMPT *grace_prompt;
} PROCESS_GENERIC_MOVING_GRACE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_MOVING_GRACE *process_generic_moving_grace_new(
		char *application_name,
		char *data_directory,
		char *value_attribute_name,
		char *process_generic_title,
		char *process_generic_sub_title,
		LIST *process_generic_point_list );

/* Process */
/* ------- */
PROCESS_GENERIC_MOVING_GRACE *process_generic_moving_grace_calloc(
		void );

/* Usage */
/* ----- */
LIST *process_generic_moving_grace_point_list(
		LIST *process_generic_point_list );

typedef struct
{
	LIST *point_list;
	GOOGLE_GENERIC *google_generic;
	GOOGLE_WINDOW *google_window;
} PROCESS_GENERIC_MOVING_GOOGLE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_MOVING_GOOGLE *process_generic_moving_google_new(
		char *application_name,
		char *data_directory,
		char *value_attribute_name,
		char *process_generic_sub_title,
		LIST *process_generic_point_list );

/* Process */
/* ------- */
PROCESS_GENERIC_MOVING_GOOGLE *process_generic_moving_google_calloc(
		void );

/* Usage */
/* ----- */
LIST *process_generic_moving_google_point_list(
		LIST *process_generic_point_list );

typedef struct
{
	LIST *heading_list;
	LIST *row_list;
	SPREADSHEET *spreadsheet;
} PROCESS_GENERIC_MOVING_SPREADSHEET;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_MOVING_SPREADSHEET *process_generic_moving_spreadsheet_new(
		char *application_name,
		char *data_directory,
		char *process_generic_sub_title,
		LIST *process_generic_point_list );

/* Process */
/* ------- */
PROCESS_GENERIC_MOVING_SPREADSHEET *process_generic_moving_spreadsheet_calloc(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LIST *process_generic_moving_spreadsheet_heading_list(
		void );

/* Usage */
/* ----- */
LIST *process_generic_moving_spreadsheet_row_list(
		LIST *process_generic_point_list );

typedef struct
{
	int statistic_days;
	char *process_generic_process_set_name;
	char *process_generic_process_name;
	PROCESS_GENERIC *process_generic;
	char *system_string;
	LIST *process_generic_point_list;
	PROCESS_GENERIC_MOVING_TABLE *
		process_generic_moving_table;
	PROCESS_GENERIC_MOVING_GRACE *
		process_generic_moving_grace;
	PROCESS_GENERIC_MOVING_SPREADSHEET *
		process_generic_moving_spreadsheet;
	PROCESS_GENERIC_MOVING_GOOGLE *
		process_generic_moving_google;
} PROCESS_GENERIC_MOVING;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_MOVING *process_generic_moving_new(
		DICTIONARY *post_dictionary /* in/out */,
		char *application_name,
		char *login_name,
		char *process_name,
		char *process_set_name,
		int moving_statistic_days,
		char *output_medium_string,
		char *data_directory );

/* Process */
/* ------- */
PROCESS_GENERIC_MOVING *process_generic_moving_calloc(
		void );

int process_generic_moving_statistic_days(
		int moving_statistic_default_days,
		int moving_statistic_days );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *process_generic_moving_system_string(
		char delimiter,
		int process_generic_moving_statistic_days,
		char *process_generic_system_string,
		int process_generic_input_date_piece,
		int process_generic_input_value_piece,
		char *process_generic_input_begin_date_string,
		char *process_generic_input_end_date_string,
		enum aggregate_statistic aggregate_statistic );

#endif
