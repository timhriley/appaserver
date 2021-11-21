/* $APPASERVER_HOME/utility/statistics_weighted.c		*/
/* ------------------------------------------------------------ */
/*								*/
/* Input: delimited stream with each row containing a number	*/
/* followed by a weight.					*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "piece.h"
#include "statistics_weighted.h"
#include "timlib.h"
#include "date.h"

void for_each_row(	long *count, 
			long *weighted_input_count,
			long *weighted_count,
			double *sum, 
			double *weighted_sum, 
			double *average,
			double *standard_deviation,
			double *coefficient_of_variation,
			double *standard_error_of_mean,
			double *minimum,
			double *median,
			double *maximum,
			double *range,
			double *percent_missing,
			int *number_in_array,
			int *exceeded_max_numbers,
			char delimiter,
			int value_piece,
			int weight_piece );

void output_statistics(	long count, 
			long weighted_input_count,
			double sum, 
			double average, 
			double standard_deviation, 
			double coefficient_of_variation, 
			double standard_error_of_mean, 
			double minimum,
			double median,
			double maximum,
			double range,
			double percent_missing,
			int number_in_array,
			int exceeded_max_numbers );

int main( int argc, char **argv )
{
	double sum = 0.0;
	double weighted_sum = 0.0;
	double average = 0.0;
	double standard_deviation = 0.0;
	double coefficient_of_variation = 0.0;
	double standard_error_of_mean = 0.0;
	double minimum = 0.0;
	double median = 0.0;
	double maximum = 0.0;
	double range = 0.0;
	long weighted_input_count = 0;
	long weighted_count = 0;
	long count = 0;
	int number_in_array = 0;
	int exceeded_max_numbers = 0;
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
		
	for_each_row(	&count, 
			&weighted_input_count,
			&weighted_count,
			&sum, 
			&weighted_sum, 
			&average,
			&standard_deviation,
			&coefficient_of_variation,
			&standard_error_of_mean,
			&minimum,
			&median,
			&maximum,
			&range,
			&percent_missing,
			&number_in_array,
			&exceeded_max_numbers,
			delimiter,
			value_piece,
			weight_piece );

	output_statistics(
			count, 
			weighted_input_count,
			sum, 
			average, 
			standard_deviation, 
			coefficient_of_variation, 
			standard_error_of_mean, 
			minimum,
			median,
			maximum,
			range,
			percent_missing,
			number_in_array,
			exceeded_max_numbers );
	return 0;

}

void for_each_row(	long *count, 
			long *weighted_input_count,
			long *weighted_count,
			double *sum, 
			double *weighted_sum, 
			double *average,
			double *standard_deviation,
			double *coefficient_of_variation,
			double *standard_error_of_mean,
			double *minimum,
			double *median,
			double *maximum,
			double *range,
			double *percent_missing,
			int *number_in_array,
			int *exceeded_max_numbers,
			char delimiter,
			int value_piece,
			int weight_piece )
{
	char buffer[ 1024 ];
	STATISTICS_WEIGHTED *statistics_weighted;

	statistics_weighted = statistics_weighted_new_statistics_weighted();

	while( get_line( buffer, stdin ) )
	{
		statistics_weighted->buffer = buffer;

		statistics_weighted_accumulate(
				&statistics_weighted->number_array,
				&statistics_weighted->weighted_input_count,
				&statistics_weighted->exceeded_max_numbers,
				&statistics_weighted->count,
				&statistics_weighted->count_non_zero,
				&statistics_weighted->weighted_count,
				&statistics_weighted->sum,
				&statistics_weighted->weighted_sum,
				statistics_weighted->buffer,
				value_piece,
				weight_piece,
				delimiter );
	}

	statistics_weighted->average =
		statistics_weighted_compute_average(
			statistics_weighted->weighted_count,
			statistics_weighted->weighted_sum );

	statistics_weighted->percent_missing =
		statistics_weighted_compute_percent_missing(
			statistics_weighted->
				number_array.number_in_array,
			statistics_weighted->weighted_input_count,
			statistics_weighted->weighted_count );

	if ( !statistics_weighted->exceeded_max_numbers )
	{
		statistics_weighted_sort_number_array(
			&statistics_weighted->number_array );

		statistics_weighted->standard_deviation =
			statistics_weighted_compute_standard_deviation(
				&statistics_weighted->coefficient_of_variation,
				&statistics_weighted->standard_error_of_mean,
				statistics_weighted->average,
				&statistics_weighted->number_array );

		statistics_weighted_compute_minimum_median_maxium_range(
					&statistics_weighted->minimum,
					&statistics_weighted->median,
					&statistics_weighted->maximum,
					&statistics_weighted->range,
					&statistics_weighted->number_array );
	}

	*count = statistics_weighted->count;
	*weighted_input_count = statistics_weighted->weighted_input_count;
	*weighted_count = statistics_weighted->weighted_count;
	*sum = statistics_weighted->sum;
	*weighted_sum = statistics_weighted->weighted_sum;
	*average = statistics_weighted->average;
	*standard_deviation = statistics_weighted->standard_deviation;
	*coefficient_of_variation =
		statistics_weighted->coefficient_of_variation;
	*standard_error_of_mean =
		statistics_weighted->standard_error_of_mean;
	*minimum = statistics_weighted->minimum;
	*median = statistics_weighted->median;
	*maximum = statistics_weighted->maximum;
	*range = statistics_weighted->range;
	*percent_missing = statistics_weighted->percent_missing;
	*number_in_array = statistics_weighted->number_array.number_in_array;
	*exceeded_max_numbers = statistics_weighted->exceeded_max_numbers;

}

void output_statistics(	long count, 
			long weighted_input_count,
			double sum, 
			double average, 
			double standard_deviation, 
			double coefficient_of_variation, 
			double standard_error_of_mean, 
			double minimum,
			double median,
			double maximum,
			double range,
			double percent_missing,
			int number_in_array,
			int exceeded_max_numbers )
{

	if ( number_in_array > 0 )
	{
		printf( "%s: %.6lf\n",
			STATISTICS_WEIGHTED_AVERAGE, average );
		printf( "%s: %.6lf\n",
			STATISTICS_WEIGHTED_SUM, sum );
		printf( "%s: %.6lf\n",
			STATISTICS_WEIGHTED_STANDARD_DEVIATION,
			standard_deviation );
		printf( "%s: %.4lf\n",
			STATISTICS_WEIGHTED_COEFFICIENT_OF_VARIATION,
			coefficient_of_variation );
		printf( "%s: %.4lf\n",
			STATISTICS_WEIGHTED_STANDARD_ERROR_OF_MEAN,
			standard_error_of_mean );
		printf( "%s: %.6lf\n",
			STATISTICS_WEIGHTED_MINIMUM, minimum );
		printf( "%s: %.6lf\n",
			STATISTICS_WEIGHTED_MEDIAN, median );
		printf( "%s: %.6lf\n",
			STATISTICS_WEIGHTED_MAXIMUM, maximum );
		printf( "%s: %.6lf\n",
			STATISTICS_WEIGHTED_RANGE, range );
		printf( "%s: %ld\n",
			STATISTICS_WEIGHTED_COUNT, count );
		printf( "%s: %.3lf\n",
			STATISTICS_WEIGHTED_PERCENT_MISSING, percent_missing );
	}
	else
	if ( exceeded_max_numbers )
	{
		printf( "%s: %.6lf\n",
			STATISTICS_WEIGHTED_AVERAGE, average );
		printf( "%s: %.6lf\n",
			STATISTICS_WEIGHTED_SUM, sum );
		printf( "%s: null\n",
			STATISTICS_WEIGHTED_STANDARD_DEVIATION );
		printf( "%s: null\n",
			STATISTICS_WEIGHTED_COEFFICIENT_OF_VARIATION );
		printf( "%s: null\n",
			STATISTICS_WEIGHTED_MINIMUM );
		printf( "%s: null\n",
			STATISTICS_WEIGHTED_MEDIAN );
		printf( "%s: null\n",
			STATISTICS_WEIGHTED_MAXIMUM );
		printf( "%s: null\n",
			STATISTICS_WEIGHTED_RANGE );
		printf( "%s: %ld\n",
			STATISTICS_WEIGHTED_COUNT, count );

		if ( weighted_input_count )
		{
			printf( "%s: %.3lf\n",
				STATISTICS_WEIGHTED_PERCENT_MISSING,
				percent_missing );
		}
		else
		{
			printf( "%s: null\n",
				STATISTICS_WEIGHTED_PERCENT_MISSING );
		}
	
	}
	else
	{
		printf( "%s: null\n",
			STATISTICS_WEIGHTED_AVERAGE );
		printf( "%s: null\n",
			STATISTICS_WEIGHTED_SUM );
		printf( "%s: null\n",
			STATISTICS_WEIGHTED_STANDARD_DEVIATION );
		printf( "%s: null\n",
			STATISTICS_WEIGHTED_COEFFICIENT_OF_VARIATION );
		printf( "%s: null\n",
			STATISTICS_WEIGHTED_MINIMUM );
		printf( "%s: null\n",
			STATISTICS_WEIGHTED_MEDIAN );
		printf( "%s: null\n",
			STATISTICS_WEIGHTED_MAXIMUM );
		printf( "%s: null\n",
			STATISTICS_WEIGHTED_RANGE );
		printf( "%s: null\n",
			STATISTICS_WEIGHTED_COUNT );

		if ( weighted_input_count )
			printf( "%s: 100.000\n",
				STATISTICS_WEIGHTED_PERCENT_MISSING );
		else
			printf( "%s: null\n",
				STATISTICS_WEIGHTED_PERCENT_MISSING );
	}
	fflush( stdout );

}

