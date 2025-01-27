/* -------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/period_wo_date.h				*/
/* -------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------	*/

#ifndef PERIOD_WO_DATE_H
#define PERIOD_WO_DATE_H

#include <stdio.h>
#include "list.h"
#include "aggregate_statistic.h"
#include "boolean.h"

/* Note: delta is not implemented. delta_values.c needs work. */
/* ---------------------------------------------------------- */
enum period_wo_date_delta {
			delta_none,
			delta_previous,
			delta_first };

typedef struct
{
	int year;
	double sum;
	int null_count;
} PERIOD_WO_DATE_YEAR_SUM;

typedef struct
{
	int year;
	double value;
	boolean is_null;
} PERIOD_WO_DATE_YEAR_VALUE;

typedef struct
{
	char *period_label;
	char *representative_week_date_string;
	LIST *year_value_list;
	double minimum;
	double average;
	double maximum;
} PERIOD_WO_DATE_PERIOD;

typedef struct
{
	PERIOD_WO_DATE_PERIOD **period_array;
	enum aggregate_statistic aggregate_statistic;
	boolean is_weekly;
	LIST *year_sum_list;
	double sum_minimum;
	double sum_average;
	double sum_maximum;
	/* ----------------------------- */
	/* Delta is not yet implemented. */
	/* ----------------------------- */
	enum period_wo_date_delta period_wo_date_delta;
} PERIOD_WO_DATE;

/* Prototypes */
/* ---------- */
void period_wo_date_get_period_minimum_average_maximum(
			double *minimum,
			double *average,
			double *maximum,
			LIST *year_list );

void period_wo_date_populate_period_minimum_average_maximum(
			PERIOD_WO_DATE_PERIOD **period_array );

PERIOD_WO_DATE_PERIOD **period_wo_date_get_monthly_period_array(
			char *begin_date_string,
			char *end_date_string );

PERIOD_WO_DATE_YEAR_VALUE *period_wo_date_seek_period_wo_date_year_value(
			LIST *year_value_list,
			int year_int );

PERIOD_WO_DATE_YEAR_SUM *period_wo_date_seek_period_wo_date_year_sum(
			LIST *year_sum_list,
			int year_int );

int period_wo_date_get_month_int(
			char *date_string );

int period_wo_date_get_week_int(
			char *date_string );

int period_wo_date_get_year_int(
			char *date_string );

LIST *period_wo_date_get_year_sum_list(
			int begin_year_int,
			int end_year_int );

LIST *period_wo_date_get_year_value_list(
			int begin_year_int,
			int end_year_int );

PERIOD_WO_DATE_PERIOD **period_wo_date_get_weekly_period_array(
			char *begin_date_string,
			char *end_date_string );

PERIOD_WO_DATE *period_wo_date_new(
			boolean is_weekly,
			enum aggregate_statistic,
			char *delta_string,
			int begin_year_int,
			int end_year_int );

PERIOD_WO_DATE_YEAR_SUM *period_wo_date_year_sum_new(
			int year );

PERIOD_WO_DATE_YEAR_VALUE *period_wo_date_year_value_new(
			int year );

PERIOD_WO_DATE_PERIOD *period_wo_date_period_new(
			char *period_label );

char *period_wo_date_get_delta_string(
			enum period_wo_date_delta period_wo_date_delta );

void period_wo_date_get_minimum_average_maximum(
				double *minimum,
				double *average,
				double *maximum,
				LIST *year_value_list );

void period_wo_date_populate_minimum_average_maximum(
			double *sum_minimum,
			double *sum_average,
			double *sum_maximum,
			PERIOD_WO_DATE_PERIOD **period_array );

char *period_wo_date_get_aggregation_average_value(
				double scalar_value,
				boolean is_weekly );

char *period_wo_date_get_aggregation_value(
			double scalar_value,
			enum aggregate_statistic,
			boolean is_weekly,
			int null_count );

void period_wo_date_populate_year_sum_list_null_count(
		LIST *year_sum_list,
		PERIOD_WO_DATE_PERIOD **period_array );

#endif
