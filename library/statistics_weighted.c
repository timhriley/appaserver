/* library/statistics_weighted.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "timlib.h"
#include "piece.h"
#include "statistics_weighted.h"

STATISTICS_WEIGHTED *statistics_weighted_new( void )
{
	return statistics_weighted_new_statistics_weighted();
}

STATISTICS_WEIGHTED *statistics_weighted_new_statistics_weighted( void )
{
	STATISTICS_WEIGHTED *statistics_weighted;

	statistics_weighted =
		(STATISTICS_WEIGHTED *)
			calloc( 1, sizeof( STATISTICS_WEIGHTED ) );

	statistics_weighted->weight_piece = -1;
	return statistics_weighted;
} /* statistics_weighted_new_statistics_weighted() */

void statistics_weighted_free( STATISTICS_WEIGHTED *statistics_weighted )
{
	free( statistics_weighted );
}

double statistics_weighted_compute_standard_deviation(
				double *coefficient_of_variation,
				double *standard_error_of_mean,
				double average,
				NUMBER_ARRAY *number_array )
{
	int n;
	double difference;
	double difference_squared;
	double standard_deviation;
	double sum_difference_squared = 0.0;

	if ( number_array->number_in_array < 2 ) return 0.0;

	for( n = 0; n < number_array->number_in_array; n++ )
	{
		difference = number_array->number[ n ] - average;
		difference_squared = difference * difference;
		sum_difference_squared += difference_squared;
	}

	standard_deviation =
		sqrt( ( 1.0 / (double)( n - 1 ) ) * sum_difference_squared );

	*standard_error_of_mean =
		standard_deviation / sqrt( number_array->number_in_array );

	if ( average )
	{
		*coefficient_of_variation =
			fabs( standard_deviation / average );
	}
	else
	{
		*coefficient_of_variation = 0;
	}
	return standard_deviation;

} /* statistics_weighted_compute_standard_deviation() */

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
					char delimiter )
{
	double value;
	int weight = 0;
	char value_buffer[ 128 ];
	char weight_buffer[ 128 ];

	if ( weight_piece != -1 )
	{
		if ( piece( weight_buffer, delimiter, buffer, weight_piece ) )
		{
			weight = atoi( weight_buffer );
		}
		else
		{
			weight = 1;
		}
	}

	if ( !weight ) weight = 1;

	(*weighted_input_count) += weight;

	if ( value_piece > 0 )
	{
		if ( piece( value_buffer, delimiter, buffer, value_piece ) )
		{
			if ( !*value_buffer ) return;
			value = atof( value_buffer );
		}
		else
		{
			fprintf(stderr,
			"ERROR in %s/%s(): cannot extract value in (%s)\n",
				__FILE__,
				__FUNCTION__,
				buffer );
			exit( 1 );
		}
	}
	else
	{
		if ( !*buffer ) return;
		value = atof( buffer );
	}

	if ( !*exceeded_max_numbers
	&&   number_array->number_in_array == STATISTICS_WEIGHTED_MAX_NUMBERS )
	{
		fprintf(stderr,
		"Warning: %s/%s() max numbers of %d exceeded\n",
			__FILE__,
			__FUNCTION__,
			STATISTICS_WEIGHTED_MAX_NUMBERS );
		*exceeded_max_numbers = 1;
	}

	(*count)++;
	if ( value ) (*count_non_zero)++;
	(*sum) += (value);
	(*weighted_count) += weight;
	(*weighted_sum) += (value * weight);

	if ( !*exceeded_max_numbers )
		number_array->number[ number_array->number_in_array ] = value;

	number_array->number_in_array++;

} /* statistics_weighted_accumulate() */

void statistics_weighted_sort_number_array( NUMBER_ARRAY *number_array )
{
	if ( number_array->number_in_array > 1 )
	{
		qsort(	number_array->number, 
			number_array->number_in_array, 
			sizeof( double ), 
			statistics_weighted_double_compare );
	}
} /* statistics_weighted_sort_number_array() */


int statistics_weighted_double_compare( const void *this, const void *that )
{
	double *a = (double *)this;
	double *b = (double *)that;

	if ( *a < *b )
		return -1;
	else
	if ( *a == *b )
		return 0;
	else
		return 1;

} /* statistics_weighted_double_compare() */

double statistics_weighted_compute_average(
				long weighted_count,
				double weighted_sum )
{
	return (!weighted_count)
		? 0.0 
		: weighted_sum / (double)weighted_count;
}

void statistics_weighted_compute_minimum_median_maxium_range(
				double *minimum,
				double *median,
				double *maximum,
				double *range,
				NUMBER_ARRAY *number_array )
{
	if ( number_array->number_in_array == -1 ) return;

	*minimum = number_array->number[ 0 ];
	*median = number_array->number[ 
			( number_array->number_in_array - 1 ) / 2 ];
	*maximum =
		number_array->number[ 
			number_array->number_in_array - 1 ];

	*range =
		number_array->number[ 
			number_array->number_in_array - 1 ] -
		number_array->number[ 0 ];
} /* statistics_weighted_compute_minimum_median_maxium_range() */

void statistics_weighted_display( NUMBER_ARRAY *number_array )
{
	int i;

	for( i = 0; i < number_array->number_in_array; i++ )
		fprintf( stderr, "%.3lf\n", number_array->number[ i ] );
} /* statistics_weighted_display() */

double statistics_weighted_compute_percent_missing(
				int number_in_array,
				long weighted_input_count,
				long count )
{
	if ( number_in_array > 0 )
	{
		return
			(double)(weighted_input_count - count) /
			(double)weighted_input_count *
			100.0;
	}
	else
	if ( number_in_array == -1 )
	{
		if ( weighted_input_count )
		{
			return
			(double)(weighted_input_count - count) /
			(double)weighted_input_count *
			100.0;
		}
		else
		{
			return -1.0;
		}
	
	}
	else
	{
		if ( weighted_input_count )
			return 100.00;
		else
			return -1.0;
	}
} /* statistics_weighted_compute_percent_missing() */

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
				char *statistics_filename )
{
	FILE *input_file;
	char input_buffer[ 256 ];
	char label[ 128 ];
	char results[ 128 ];

	if ( ! ( input_file = fopen( statistics_filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 statistics_filename );
		return 0;
	}

	if ( mean ) *mean = 0.0;
	if ( standard_deviation ) *standard_deviation = 0.0;
	if ( standard_error_of_mean ) *standard_error_of_mean = 0.0;
	if ( sum ) *sum = 0.0;
	if ( sample_size ) *sample_size = 0;
	if ( coefficient_of_variation ) *coefficient_of_variation = 0;
	if ( minimum ) *minimum = 0.0;
	if ( median ) *median = 0.0;
	if ( maximum ) *maximum = 0.0;
	if ( range ) *range = 0.0;
	if ( percent_missing ) *percent_missing = 0.0;

	while( get_line( input_buffer, input_file ) )
	{
		piece( label, ':', input_buffer, 0 );
		if ( strcmp( label, STATISTICS_WEIGHTED_AVERAGE ) == 0 )
		{
			if ( mean )
			{
				piece( results, ':', input_buffer, 1 );
				*mean = atof( results );
			}
		}
		else
		if ( strcmp(	label,
				STATISTICS_WEIGHTED_STANDARD_DEVIATION ) == 0 )
		{
			if ( standard_deviation )
			{
				piece( results, ':', input_buffer, 1 );
				*standard_deviation = atof( results );
			}
		}
		else
		if ( strcmp(	label,
				STATISTICS_WEIGHTED_COUNT ) == 0 )
		{
			if ( sample_size )
			{
				piece( results, ':', input_buffer, 1 );
				*sample_size =
					strtoul( results,
						 (char **)0,
						 10 /* base 10 */ );
			}
		}
		else
		if ( strcmp(	label,
				STATISTICS_WEIGHTED_SUM ) == 0 )
		{
			if ( sum )
			{
				piece( results, ':', input_buffer, 1 );
				*sum = atof( results );
			}
		}
		else
		if ( strcmp(
			label,
			STATISTICS_WEIGHTED_STANDARD_ERROR_OF_MEAN ) == 0 )
		{
			if ( standard_error_of_mean )
			{
				piece( results, ':', input_buffer, 1 );
				*standard_error_of_mean = atof( results );
			}
		}
		else
		if ( strcmp(
			label,
			STATISTICS_WEIGHTED_COEFFICIENT_OF_VARIATION ) == 0 )
		{
			if ( coefficient_of_variation )
			{
				piece( results, ':', input_buffer, 1 );
				*coefficient_of_variation = atof( results );
			}
		}
		else
		if ( strcmp(
			label,
			STATISTICS_WEIGHTED_MINIMUM ) == 0 )
		{
			if ( minimum )
			{
				piece( results, ':', input_buffer, 1 );
				*minimum = atof( results );
			}
		}
		else
		if ( strcmp(
			label,
			STATISTICS_WEIGHTED_MEDIAN ) == 0 )
		{
			if ( median )
			{
				piece( results, ':', input_buffer, 1 );
				*median = atof( results );
			}
		}
		else
		if ( strcmp(
			label,
			STATISTICS_WEIGHTED_MAXIMUM ) == 0 )
		{
			if ( maximum )
			{
				piece( results, ':', input_buffer, 1 );
				*maximum = atof( results );
			}
		}
		else
		if ( strcmp(
			label,
			STATISTICS_WEIGHTED_RANGE ) == 0 )
		{
			if ( range )
			{
				piece( results, ':', input_buffer, 1 );
				*range = atof( results );
			}
		}
		else
		if ( strcmp(
			label,
			STATISTICS_WEIGHTED_PERCENT_MISSING ) == 0 )
		{
			if ( percent_missing )
			{
				piece( results, ':', input_buffer, 1 );
				*percent_missing = atof( results );
			}
		}
	}
	fclose( input_file );
	return 1;
} /* statistics_weighted_fetch_statistics() */

