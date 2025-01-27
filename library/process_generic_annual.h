/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/process_generic_annual.h 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef PROCESS_GENERIC_ANNUAL_H
#define PROCESS_GENERIC_ANNUAL_H

#include "list.h"
#include "boolean.h"
#include "hash_table.h"
#include "folder.h"
#include "date.h"
#include "appaserver_link.h"
#include "aggregate_statistic.h"
#include "statistic.h"
#include "html.h"
#include "output_medium.h"
#include "process_generic.h"

#define PROCESS_GENERIC_ANNUAL_DATE_BEGIN_LABEL	"begin_month_day"
#define PROCESS_GENERIC_ANNUAL_DATE_END_LABEL	"end_month_day"
#define PROCESS_GENERIC_ANNUAL_DATE_BEGIN_YEAR	"begin_year"
#define PROCESS_GENERIC_ANNUAL_DATE_END_YEAR	"end_year"
#define PROCESS_GENERIC_ANNUAL_DATE_LEAP_YEAR 	2000

typedef struct
{
	int begin_month_integer;
	int begin_day_integer;
	int end_month_integer;
	int end_day_integer;
	int begin_year_integer;
	int end_year_integer;
	LIST *year_integer_list;
	int day_range;
	int year_range;
	DATE *begin_date;
	DATE *end_date;
} PROCESS_GENERIC_ANNUAL_DATE;

/* Usage */
/* ----- */
PROCESS_GENERIC_ANNUAL_DATE *process_generic_annual_date_new(
			DICTIONARY *post_dictionary );

/* Process */
/* ------- */
PROCESS_GENERIC_ANNUAL_DATE *process_generic_annual_date_calloc(
			void );

int process_generic_annual_date_day_range(
			int begin_month_integer,
			int begin_day_integer,
			int end_month_integer,
			int end_day_integer,
			int process_generic_annual_date_leap_year );

int process_generic_annual_date_year_range(
			int begin_year_integer,
			int end_year_integer );

/* Usage */
/* ----- */
int process_generic_annual_date_month_integer(
			char *month_day_string );

/* Process */
/* ------- */

/* Usage */
/* ----- */
int process_generic_annual_date_day_integer(
			char *month_day_string );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *process_generic_annual_date_year_integer_list(
			int begin_year_integer,
			int end_year_integer );

typedef struct
{
	char ***value_matrix;
} PROCESS_GENERIC_ANNUAL_MATRIX;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_ANNUAL_MATRIX *process_generic_annual_matrix_fetch(
			char *application_name,
			PROCESS_GENERIC *process_generic,
			PROCESS_GENERIC_ANNUAL_DATE *
				process_generic_annual_date );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *process_generic_annual_matrix_date_string(
			int month_integer,
			int day_integer,
			int year );

/* Usage */
/* ----- */
void process_generic_annual_matrix_input_set(
			char ***value_matrix /* in/out */,
			PROCESS_GENERIC *process_generic,
			PROCESS_GENERIC_ANNUAL_DATE *
				process_generic_annual_date,
			char *input );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
PROCESS_GENERIC_ANNUAL_MATRIX *process_generic_annual_matrix_new(
			int year_range,
			int day_range );

/* Process */
/* ------- */
PROCESS_GENERIC_ANNUAL_MATRIX *process_generic_annual_matrix_calloc(
			void );

/* Usage */
/* ----- */
int process_generic_annual_matrix_day_offset(
			int begin_month_integer,
			int begin_day_integer,
			char *date_string );

/* Process */
/* ------- */

/* Usage */
/* ----- */
int process_generic_annual_matrix_year_offset(
			int begin_year_integer,
			char *date_string );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void process_generic_annual_matrix_value_set(
			char ***value_matrix /* in/out */,
			char *value_string,
			int process_generic_annual_matrix_day_offset,
			int process_generic_annual_matrix_year_offset );

/* Process */
/* ------- */

/* Driver */
/* ------ */
void process_generic_annual_matrix_output(
			int day_range,
			int year_range,
			int begin_year_integer,
			DATE *begin_date /* in/out */,
			char ***value_matrix,
			FILE *stream_file );

/* Process */
/* ------- */

typedef struct
{
	char *process_generic_process_set_name;
	char *process_generic_process_name;
	PROCESS_GENERIC *process_generic;
	PROCESS_GENERIC_ANNUAL_DATE *process_generic_annual_date;
	PROCESS_GENERIC_ANNUAL_MATRIX *process_generic_annual_matrix;
	char *sub_title;
} PROCESS_GENERIC_ANNUAL;

/* Usage */
/* ----- */
PROCESS_GENERIC_ANNUAL *process_generic_annual_new(
			char *application_name,
			char *login_name,
			char *process_name,
			char *output_medium_string,
			char *document_root,
			DICTIONARY *post_dictionary );

/* Process */
/* ------- */
PROCESS_GENERIC_ANNUAL *process_generic_annual_calloc(
			void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *process_generic_annual_sub_title(
			char *value_folder_name,
			char *appaserver_user_date_format_string,
			DATE *begin_date,
			DATE *end_date,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic );

/* Process */
/* ------- */

#endif
