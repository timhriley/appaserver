/* $APPASERVER_HOME/library/moving_average.h		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef MOVING_AVERAGE_H
#define MOVING_AVERAGE_H

#include "list.h"
#include "aggregate_statistic.h"
#include "statistics_weighted.h"

#define MOVING_AVERAGE_DEFAULT	30

typedef struct
{
	double value;
	char *date_label;
	boolean is_null;
	char *input;
} MOVING_AVERAGE_VALUE;

typedef struct
{
	LIST *value_list;
	int number_of_days;
	char delimiter;
	enum aggregate_statistic aggregate_statistic;
	char *begin_date_string;
} MOVING_AVERAGE;

MOVING_AVERAGE *moving_average_new(
			int number_of_days,
			char delimiter,
			enum aggregate_statistic,
			char *begin_date_string );

MOVING_AVERAGE_VALUE *moving_average_new_value(
			double value,
			char *date_label,
			boolean is_null );

void moving_average_display(
			LIST *value_list,
			int number_of_days,
			char delimiter,
			enum aggregate_statistic,
			char *begin_date_string );

void moving_average_free(
			MOVING_AVERAGE *moving_average );

void moving_average_set(LIST *value_list,
			double value,
			char *date_label,
			boolean is_null );

void moving_average_display_average(
			LIST *value_list,
			int number_of_days,
			char delimiter );

void moving_average_display_maximum(
			LIST *value_list,
			int number_of_days,
			char delimiter );

void moving_average_display_minimum(
			LIST *value_list,
			int number_of_days,
			char delimiter );

void moving_average_display_sum(
			LIST *value_list,
			int number_of_days,
			char delimiter );

void moving_average_display_statistics_weighted(
			LIST *value_list,
			int number_of_days,
			char delimiter,
			char *statistics_weighted_label );

void moving_average_display_non_zero_percent(
			LIST *value_list,
			int number_of_days,
			char delimiter,
			char *statistics_weighted_label );

void moving_average_piece_set(
			LIST *value_list,
			double value,
			char *date_label,
			char *input );

void moving_average_piece_display(
			FILE *output_pipe,
			LIST *value_list,
			int number_of_days,
			char delimiter,
			int value_piece );

void moving_average_piece_display_average(
			FILE *output_pipe,
			LIST *value_list,
			int number_of_days,
			char delimiter,
			int value_piece );

#endif
