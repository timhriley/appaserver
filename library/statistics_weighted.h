/* library/statistics_weighted.h			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef STATISTICS_WEIGHTED_H
#define STATISTICS_WEIGHTED_H

#include <stdio.h>
#include "boolean.h"

#define STATISTICS_WEIGHTED_MAX_NUMBERS			786432
#define STATISTICS_WEIGHTED_COUNT			"Count"
#define STATISTICS_WEIGHTED_AVERAGE			"Average"
#define STATISTICS_WEIGHTED_SUM				"Sum"
#define STATISTICS_WEIGHTED_STANDARD_DEVIATION		"Standard Deviation"
#define STATISTICS_WEIGHTED_COEFFICIENT_OF_VARIATION "Coefficient of Variation"
#define STATISTICS_WEIGHTED_STANDARD_ERROR_OF_MEAN "Standard Error of Mean"
#define STATISTICS_WEIGHTED_MINIMUM			"Minimum"
#define STATISTICS_WEIGHTED_MEDIAN			"Median"
#define STATISTICS_WEIGHTED_MAXIMUM			"Maximum"
#define STATISTICS_WEIGHTED_RANGE			"Range"
#define STATISTICS_WEIGHTED_PERCENT_MISSING		"Percent Missing"

typedef struct
{
	double number[ STATISTICS_WEIGHTED_MAX_NUMBERS ];
	int number_in_array;
} NUMBER_ARRAY;

typedef struct
{
	char *buffer;
	unsigned long count;
	long count_non_zero;
	long weighted_count;
	long weighted_input_count;
	boolean exceeded_max_numbers;
	double percent_missing;
	double average;
	double sum;
	double weighted_sum;
	double minimum;
	double median;
	double maximum;
	double range;
	double standard_deviation;
	double coefficient_of_variation;
	double standard_error_of_mean;
	NUMBER_ARRAY number_array;
	int value_piece;
	int weight_piece;
} STATISTICS_WEIGHTED;

/* Prototypes */
/* ---------- */
STATISTICS_WEIGHTED *statistics_weighted_new_statistics_weighted(
				void );

STATISTICS_WEIGHTED *statistics_weighted_new(
				void );

void statistics_weighted_free(	STATISTICS_WEIGHTED *statistics_weighted );

double statistics_weighted_compute_standard_deviation(
				double *coefficient_of_variation,
				double *standard_error_of_mean,
				double average,
				NUMBER_ARRAY *number_array );

int statistics_weighted_double_compare(
				const void *this,
				const void *that );

void statistics_weighted_sort_number_array(
				NUMBER_ARRAY *number_array );

void statistics_weighted_free(	STATISTICS_WEIGHTED *statistics_weighted );

void statistics_weighted_compute_minimum_median_maxium_range(
				double *minimum,
				double *median,
				double *maximum,
				double *range,
				NUMBER_ARRAY *number_array );

void statistics_weighted_accumulate(
				NUMBER_ARRAY *number_array,
				long *weighted_input_count,
				boolean *exceeded_max_numbers,
				unsigned long *count,
				long *count_non_zero,
				long *weighted_count,
				double *sum,
				double *weighted_sum,
				char *buffer,
				int value_piece,
				int weight_piece,
				char delimiter );

double statistics_weighted_compute_average(
				long weighted_count,
				double weighted_sum );

double statistics_weighted_compute_percent_missing(
				int number_in_array,
				long weighted_input_count,
				long count );

void statistics_weighted_display(
				NUMBER_ARRAY *number_array );

boolean statistics_weighted_fetch_statistics(
				double *mean,
				double *standard_deviation,
				double *standard_error_of_mean,
				unsigned long *sample_size,
				double *sum,
				double *coefficient_of_variation,
				double *minimum,
				double *median,
				double *maximum,
				double *range,
				double *percent_missing,
				char *statistics_filename );

#endif
