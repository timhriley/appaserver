/* generic_annual_comparison.h				   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef ANNUAL_COMPARISON_H
#define ANNUAL_COMPARISON_H

#include "date.h"

#define ANNUAL_COMPARISON_DEFAULT_YEAR		2000 /* must be a leap year */
#define ANNUAL_COMPARISON_ROWS_BETWEEN_HEADING 20
#define GRACE_TICKLABEL_ANGLE			90
#define GRACE_DATATYPE_ENTITY_PIECE		0
#define GRACE_DATATYPE_PIECE			1
#define GRACE_DATE_PIECE			2
#define GRACE_TIME_PIECE			-1
#define GRACE_VALUE_PIECE			3

typedef struct
{
	char *begin_month_day;
	char *end_month_day;
	int end_year;
	int day_range;
	int year_range;
	char ***value_array;
	int day_array_offset;
	int year_array_offset;
	DATE *begin_date;
	int begin_year;
} ANNUAL_COMPARISON;

ANNUAL_COMPARISON *annual_comparison_new(
				char *begin_day_month,
				char *end_day_month,
				int begin_year,
				int end_year );

int annual_comparison_get_day_range(
				DATE **begin_date,
				char *begin_month_day,
				char *end_month_day );

char ***annual_comparison_value_array_new(
				int year_range,
				int day_range );

boolean annual_comparison_set_value(
				char ***value_array,
				int *day_array_offset,
				int *year_array_offset,
				DATE *begin_date,
				int begin_year,
				int day_range,
				int year_range,
				char *date_yyyymmdd,
				char *value_string );

int annual_comparison_get_array_offset(
				DATE *begin_date,
				char *date_yyyymmdd );

int annual_comparison_get_day_array_offset(
				DATE *begin_date,
				char *date_yyyymmdd );

void annual_comparison_output(
				char ***value_array,
				int day_range,
				int year_range,
				DATE *begin_date,
				int begin_year );

#endif
