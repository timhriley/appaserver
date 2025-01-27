/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/statistic.h			   		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#ifndef STATISTIC_H
#define STATISTIC_H

#include <stdio.h>
#include "boolean.h"

#define STATISTIC_NUMBER_ARRAY_MAX_SIZE		1048576

typedef struct
{
	double number_array[ STATISTIC_NUMBER_ARRAY_MAX_SIZE ];
	unsigned long array_count;
	boolean max_numbers_exceeded;
	boolean sorted_boolean;
} STATISTIC_NUMBER_ARRAY;

/* Usage */
/* ----- */
STATISTIC_NUMBER_ARRAY *statistic_number_array_calloc(
		void );

/* Usage */
/* ----- */
void statistic_number_array_insert(
		STATISTIC_NUMBER_ARRAY *
			statistic_number_array /* in/out */,
		unsigned long statistic_number_array_max_size,
		double statistic_value,
		int statistic_weight );

/* Usage */
/* ----- */
void statistic_number_array_sort(
			STATISTIC_NUMBER_ARRAY *
				statistic_number_array /* in/out */ );

/* Usage */
/* ----- */
int statistic_number_array_compare(
		const void *this,
		const void *that );

/* Public */
/* ------ */
void statistic_number_array_free(
		STATISTIC_NUMBER_ARRAY *statistic_number_array );

#define STATISTIC_QUARTILE_MINIMUM			"Minimum"
#define STATISTIC_QUARTILE_MEDIAN			"Median"
#define STATISTIC_QUARTILE_MAXIMUM			"Maximum"
#define STATISTIC_QUARTILE_RANGE			"Range"

typedef struct
{
	double minimum;
	double maximum;
	double range;
	boolean array_even_boolean;
	int median_position;
	double median;
	int first_position;
	double first;
	int third_position;
	double third;
	double innerquartile_range;
	double coefficient_of_dispersion;
	double outlier_low_limit;
	double outlier_high_limit;
	int outlier_low_count;
	int outlier_high_count;
	char *outlier_low_heading_string;
	char *outlier_high_heading_string;
} STATISTIC_QUARTILE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
STATISTIC_QUARTILE *statistic_quartile_new(
		STATISTIC_NUMBER_ARRAY *statistic_number_array );

/* Process */
/* ------- */
STATISTIC_QUARTILE *statistic_quartile_calloc(
		void );

double statistic_quartile_innerquartile_range(
		double statistic_quartile_first,
		double statistic_quartile_third );

double statistic_quartile_coefficient_of_dispersion(
		double statistic_quartile_first,
		double statistic_quartile_third,
		double statistic_quartile_innerquartile_range );

int statistic_quartile_outlier_low_count(
		STATISTIC_NUMBER_ARRAY *
			statistic_number_array,
		double statistic_quartile_outlier_low_limit );

int statistic_quartile_outlier_high_count(
		STATISTIC_NUMBER_ARRAY *
			statistic_number_array,
		double statistic_quartile_outlier_high_limit );

/* Usage */
/* ----- */
double statistic_quartile_minimum(
		STATISTIC_NUMBER_ARRAY *statistic_number_array );

/* Usage */
/* ----- */
double statistic_quartile_median(
		STATISTIC_NUMBER_ARRAY *statistic_number_array,
		boolean statistic_quartile_array_even_boolean,
		int statistic_quartile_median_position );

/* Usage */
/* ----- */
double statistic_quartile_maximum(
		STATISTIC_NUMBER_ARRAY *statistic_number_array );

/* Usage */
/* ----- */
double statistic_quartile_range(
		double statistic_calculate_minimum,
		double statistic_calculate_maximum );

/* Usage */
/* ----- */
double statistic_quartile_first(
		STATISTIC_NUMBER_ARRAY *
			statistic_number_array,
		boolean statistic_quartile_array_even_boolean,
		double statistic_quartile_range,
		int statistic_quartile_first_position );

/* Usage */
/* ----- */
double statistic_quartile_third(
		STATISTIC_NUMBER_ARRAY *
			statistic_number_array,
		boolean statistic_quartile_array_even_boolean,
		double statistic_quartile_range,
		int statistic_quartile_third_position );

/* Usage */
/* ----- */
double statistic_quartile_outlier_low_limit(
		double statistic_quartile_first,
		double statistic_quartile_innerquartile_range );

/* Usage */
/* ----- */
double statistic_quartile_outlier_high_limit(
		double statistic_quartile_third,
		double statistic_quartile_innerquartile_range );

/* Usage */
/* ----- */
int statistic_quartile_median_position(
		unsigned long array_count,
		boolean statistic_quartile_array_even_boolean );

/* Usage */
/* ----- */
int statistic_quartile_first_position(
		unsigned long array_count,
		int statistic_quartile_median_position );

/* Usage */
/* ----- */
int statistic_quartile_third_position(
		unsigned long array_count,
		int statistic_quartile_median_position,
		int statistic_quartile_first_position );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *statistic_quartile_outlier_low_heading_string(
		double statistic_quartile_outlier_low_limit );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *statistic_quartile_outlier_high_heading_string(
		double statistic_quartile_outlier_high_limit );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *statistic_quartile_range_string(
		double range );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *statistic_quartile_percent_missing_string(
		double percent_missing );

/* Public */
/* ------ */
void statistic_quartile_free(
		STATISTIC_QUARTILE *statistic_quartile );

#define STATISTIC_CALCULATE_ROW_COUNT			"Row Count"
#define STATISTIC_CALCULATE_WEIGHT_COUNT		"Weight Count"
#define STATISTIC_CALCULATE_AVERAGE			"Average"
#define STATISTIC_CALCULATE_SUM				"Sum"
#define STATISTIC_CALCULATE_STANDARD_DEVIATION		"Standard Deviation"
#define STATISTIC_CALCULATE_PERCENT_MISSING		"Percent Missing"

typedef struct
{
	double average;
	double percent_missing;
	double non_zero_percent;
	double sum_difference_squared;
	double variance;
	double standard_deviation;
} STATISTIC_CALCULATE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
STATISTIC_CALCULATE *statistic_calculate_new(
		STATISTIC_NUMBER_ARRAY *statistic_number_array,
		unsigned long weight_count,
		double sum,
		unsigned long row_count,
		unsigned long value_count,
		unsigned long non_zero_count );

/* Process */
/* ------- */
STATISTIC_CALCULATE *statistic_calculate_calloc(
		void );

double statistic_calculate_average(
		unsigned long weight_count,
		double sum );

double statistic_calculate_percent_missing(
		unsigned long row_count,
		unsigned long value_count );

double statistic_calculate_non_zero_percent(
		unsigned long row_count,
		unsigned long non_zero_count );

/* Usage */
/* ----- */
double statistic_calculate_sum_difference_squared(
		STATISTIC_NUMBER_ARRAY *statistic_number_array,
		double statistic_calculate_average );

/* Usage */
/* ----- */
double statistic_calculate_variance(
		int array_count,
		boolean max_numbers_exceeded,
		double statistic_calculate_sum_difference_squared );

/* Usage */
/* ----- */
double statistic_calculate_standard_deviation(
		double statistic_calculate_variance );

/* Public */
/* ------ */
void statistic_calculate_free(
		STATISTIC_CALCULATE *statistic_calculate );

/* Returns heap memory */
/* ------------------- */
char *statistic_calculate_percent_missing_string(
		double percent_missing );

typedef struct
{
	int value_piece;
	int weight_piece;
	char delimiter;
	unsigned long row_count;
	boolean empty_value;
	double value;
	unsigned long value_count;
	int weight;
	unsigned long weight_count;
	double weight_value;
	double sum;
	STATISTIC_NUMBER_ARRAY *statistic_number_array;
	unsigned long non_zero_count;
	STATISTIC_CALCULATE *statistic_calculate;
	STATISTIC_QUARTILE *statistic_quartile;
} STATISTIC;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
STATISTIC *statistic_new(
		int value_piece,
		int weight_piece,
		char delimiter );

/* Process */
/* ------- */
STATISTIC *statistic_calloc(
		void );

/* Usage */
/* ----- */
void statistic_accumulate(
		STATISTIC *statistic
			/* in/out */,
		STATISTIC_NUMBER_ARRAY *statistic_number_array
			/* in/out */,
		char *input );

/* Process */
/* ------- */
boolean statistic_empty_value(
		int value_piece,
		char delimiter,
		char *input );

double statistic_value(
		int value_piece,
		char delimiter,
		char *input );

/* Returns at least one */
/* -------------------- */
int statistic_weight(
		int weight_piece,
		char delimiter,
		char *input );

double statistic_weight_value(
		double statistic_value,
		int statistic_weight );

/* Public */
/* ------ */
void statistic_free(
		STATISTIC *statistic );

typedef struct
{
	int value_piece;
	char delimiter;
	unsigned long row_count;
	unsigned long value_count;
	char minimum_value[ 32 ];
	char maximum_value[ 32 ];
	int range;
	double percent_missing;
} STATISTIC_DATE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
STATISTIC_DATE *statistic_date_new(
		int value_piece,
		char delimiter );

/* Process */
/* ------- */
STATISTIC_DATE *statistic_date_calloc(
		void );

/* Usage */
/* ----- */
void statistic_date_accumulate(
		STATISTIC_DATE *statistic_date /* in/out */,
		char *input );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *statistic_date_value(
		int value_piece,
		char delimiter,
		char *input );

void statistic_date_new_minimum_value(
		char minimum_value[] /* in/out */,
		char *statistic_date_value );

void statistic_date_new_maximum_value(
		char maximum_value[] /* in/out */,
		char *statistic_date_value );

/* Usage */
/* ----- */
LIST *statistic_table_cell_list(
		STATISTIC *statistic,
		char *label,
		boolean outlier_boolean );

/* Usage */
/* ----- */
LIST *statistic_table_column_list(
		char *outlier_low_heading_string,
		char *outlier_high_heading_string );

#endif
