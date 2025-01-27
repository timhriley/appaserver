/* ------------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/utility/statistics_weighted.c			*/
/* ------------------------------------------------------------------- 	*/
/*									*/
/* Input: delimited stream with each row containing a number		*/
/* followed by a weight.						*/
/*									*/
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "piece.h"
#include "boolean.h"
#include "statistic.h"
#include "timlib.h"
#include "date.h"

void for_each_row(
		unsigned long *row_count,
		unsigned long *weight_count,
		double *sum, 
		double *average,
		double *standard_deviation,
		double *minimum,
		double *median,
		double *maximum,
		double *range,
		double *percent_missing,
		boolean *max_numbers_exceeded,
		char delimiter,
		int value_piece,
		int weight_piece );

void output_statistics(
		unsigned long row_count,
		unsigned long weight_count,
		double sum,
		double average,
		double standard_deviation,
		double minimum,
		double median,
		double maximum,
		double range,
		double percent_missing,
		boolean max_numbers_exceeded );

int main( int argc, char **argv )
{
	double sum = 0.0;
	double average = 0.0;
	double standard_deviation = 0.0;
	double minimum = 0.0;
	double median = 0.0;
	double maximum = 0.0;
	double range = 0.0;
	unsigned long row_count = 0;
	unsigned long weight_count = 0;
	boolean max_numbers_exceeded = 0;
	double percent_missing = 0.0;
	char delimiter = {0};
	int value_piece = 0;
	int weight_piece = -1;

	/* "Usage: %s [delimiter] [value_piece] [weight_piece] */
	/* --------------------------------------------------- */

	if ( strcmp( argv[ 0 ], "descriptive_statistics" ) == 0 )
	{
		delimiter = '|';
		value_piece = 0;
		weight_piece = -1;
	}
	else
	if ( argc > 1 )
	{
		delimiter = *argv[ 1 ];
		if ( argc >= 3 ) value_piece = atoi( argv[ 2 ] );
		if ( argc == 4 ) weight_piece = atoi( argv[ 3 ] );
	}
		
	for_each_row(	&row_count,
			&weight_count,
			&sum,
			&average,
			&standard_deviation,
			&minimum,
			&median,
			&maximum,
			&range,
			&percent_missing,
			&max_numbers_exceeded,
			delimiter,
			value_piece,
			weight_piece );

	if ( row_count )
	{
		output_statistics(
			row_count,
			weight_count,
			sum,
			average,
			standard_deviation,
			minimum,
			median,
			maximum,
			range,
			percent_missing,
			max_numbers_exceeded );
	}
	return 0;
}

void for_each_row(
		unsigned long *row_count,
		unsigned long *weight_count,
		double *sum,
		double *average,
		double *standard_deviation,
		double *minimum,
		double *median,
		double *maximum,
		double *range,
		double *percent_missing,
		boolean *max_numbers_exceeded,
		char delimiter,
		int value_piece,
		int weight_piece )
{
	char buffer[ 1024 ];
	STATISTIC *statistic;

	/* Safely returns */
	/* -------------- */
	statistic = statistic_new( value_piece, weight_piece, delimiter );

	while( get_line( buffer, stdin ) )
	{
		statistic_accumulate(
			statistic /* in/out */,
			statistic->statistic_number_array /* in/out */,
			buffer /* input */ );
	}

	statistic->statistic_calculate =
		statistic_calculate_new(
			statistic->statistic_number_array,
			statistic->weight_count,
			statistic->sum,
			statistic->row_count,
			statistic->value_count,
			statistic->non_zero_count );

	statistic_number_array_sort(
		statistic->statistic_number_array /* in/out */ );

	statistic->statistic_quartile =
		statistic_quartile_new(
			statistic->statistic_number_array );

	*row_count = statistic->row_count;
	*weight_count = statistic->weight_count;
	*sum = statistic->sum;
	*average = statistic->statistic_calculate->average;

	*standard_deviation =
		statistic->
			statistic_calculate->
			standard_deviation;

	*minimum = statistic->statistic_quartile->minimum;
	*median = statistic->statistic_quartile->median;
	*maximum = statistic->statistic_quartile->maximum;
	*range = statistic->statistic_quartile->range;
	*percent_missing = statistic->statistic_calculate->percent_missing;

	*max_numbers_exceeded =
		statistic->
			statistic_number_array->
			max_numbers_exceeded;
}

void output_statistics(
		unsigned long row_count, 
		unsigned long weight_count,
		double sum, 
		double average, 
		double standard_deviation, 
		double minimum,
		double median,
		double maximum,
		double range,
		double percent_missing,
		boolean max_numbers_exceeded )
{
	printf( "%s: %ld\n",
		STATISTIC_CALCULATE_ROW_COUNT, row_count );
	printf( "%s: %ld\n",
		STATISTIC_CALCULATE_WEIGHT_COUNT, weight_count );
	printf( "%s: %.4lf\n",
		STATISTIC_CALCULATE_AVERAGE, average );
	printf( "%s: %.4lf\n",
		STATISTIC_CALCULATE_SUM, sum );
	printf( "%s: %.1lf\n",
		STATISTIC_CALCULATE_PERCENT_MISSING, percent_missing );

	if ( !max_numbers_exceeded )
	{
		printf( "%s: %.4lf\n",
			STATISTIC_CALCULATE_STANDARD_DEVIATION,
			standard_deviation );
		printf( "%s: %.4lf\n",
			STATISTIC_QUARTILE_MINIMUM, minimum );
		printf( "%s: %.4lf\n",
			STATISTIC_QUARTILE_MEDIAN, median );
		printf( "%s: %.4lf\n",
			STATISTIC_QUARTILE_MAXIMUM, maximum );
		printf( "%s: %.4lf\n",
			STATISTIC_QUARTILE_RANGE, range );
	}
	else
	{
		printf( "%s: null\n",
			STATISTIC_CALCULATE_STANDARD_DEVIATION );
		printf( "%s: null\n",
			STATISTIC_QUARTILE_MINIMUM );
		printf( "%s: null\n",
			STATISTIC_QUARTILE_MEDIAN );
		printf( "%s: null\n",
			STATISTIC_QUARTILE_MAXIMUM );
		printf( "%s: null\n",
			STATISTIC_QUARTILE_RANGE );
	}

	fflush( stdout );
}

