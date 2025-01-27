/* ------------------------------------------------------------ 	*/
/* $APPASERVER_HOME/utility/statistics.c				*/
/* ------------------------------------------------------------ 	*/
/* Input:	delimited rows and column to apply			*/
/* Process:	does arithmatic on the column				*/
/* Output:	same rows of input plus the results at the end		*/
/* Note:	columns are assumed to be pipe delimited		*/
/* --------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "piece.h"
#include "timlib.h"

#define MAX_FREQUENCY_CELLS	1024
#define MAX_NUMBERS		262144
#define OUTPUT_IDENTIFIER	".results"

typedef struct
{
	double number[ MAX_NUMBERS ];
	int number_in_array;
} NUMBER_ARRAY;

typedef struct
{
	double less_than_this[ MAX_FREQUENCY_CELLS ];
	int count[ MAX_FREQUENCY_CELLS ];
} RANGE;

int load_range( double less_than_this[], int interval );
double compute_std_dev( double average, NUMBER_ARRAY *number_array );
double compute_std_error( double std_dev, int count );
int double_compare( const void *this, const void *that );
void sort_number_array( NUMBER_ARRAY *number_array );

void for_each_row(	int *count, 
			int *input_count,
			double *sum, 
			RANGE *range, 
			NUMBER_ARRAY *number_array,
			int piece_to_apply,
			int echo_input,
			int interval );

void output_number_1(	int count, 
			double average, 
			double std_dev, 
			RANGE *range, 
			NUMBER_ARRAY *number_array,
			char *column_name,
			int interval );

void output_number_2(	int count, 
			int input_count,
			double sum, 
			double average, 
			double std_dev, 
			double std_error, 
			NUMBER_ARRAY *number_array );

void output_number_3(	int count, 
			int input_count,
			double sum, 
			double average, 
			double std_dev, 
			double std_error, 
			NUMBER_ARRAY *number_array );

int get_cell_offset( 	double less_than_this[],
			double value );

void output_histogram_1( 	
		int interval,
		double less_than_this[],
		int count[ MAX_FREQUENCY_CELLS ],
		int total_count );

double extract_piece_value( char *source, int *ignore, int piece_to_apply );
void increment_range( RANGE *range, double value );

int main( int argc, char **argv )
{
	char *column_name;
	int piece_to_apply;
	double sum = 0.0;
	double average = 0.0;
	double std_dev = 0.0;
	double std_error = 0.0;
	int input_count = 0;
	int count = 0;
	RANGE range;
	NUMBER_ARRAY number_array;
	int interval;
	int output_number;
	int echo_input;

	if ( argc < 4 )
	{
		fprintf( stderr,
"Usage: %s piece_to_apply range_interval column_name [output_number]\n",
			 argv[ 0 ] );
		fprintf( stderr, "Note: piece_to_apply is zero based.\n" );
		exit( 1 );
	}

	piece_to_apply = atoi( argv[ 1 ] );
	interval = atoi( argv[ 2 ] );
	column_name = argv[ 3 ];

	if ( argc == 5 )
	{
		output_number = atoi( argv[ 4 ] );
		echo_input = 0;
	}
	else
	{
		echo_input = 1;
		output_number = 1;
	}

	if ( interval )
	{
		if ( ! load_range( range.less_than_this, interval ) )
		{
			fprintf( stderr,
				 "ERROR: %s cannot load range\n", argv[ 0 ] );
			exit( 1 );
		}
	}

	for_each_row(	&count, 
			&input_count,
			&sum, 
			&range, 
			&number_array,
			piece_to_apply,
			echo_input,
			interval );

	average = (count == 0) ? 0.0 : sum / (double)count;

	if ( number_array.number_in_array != -1 )
	{
		sort_number_array( &number_array );
		std_dev = compute_std_dev(average, &number_array );
		std_error = compute_std_error( std_dev, count );
	}

	if ( output_number == 1 )
	{
		output_number_1(	count, 
					average, 
					std_dev, 
					&range, 
					&number_array,
					column_name,
					interval );
	}
	else
	if ( output_number == 2 )
	{
		output_number_2(	count, 
					input_count,
					sum, 
					average, 
					std_dev, 
					std_error, 
					&number_array );
	}
	else
	if ( output_number == 3 )
	{
		output_number_3(	count, 
					input_count,
					sum, 
					average, 
					std_dev, 
					std_error, 
					&number_array );
	}

	return 0;
}

double compute_std_error( double std_dev, int count )
{
	return std_dev / sqrt( count );
}

double compute_std_dev( double average, NUMBER_ARRAY *number_array )
{
	int n;
	double difference;
	double difference_squared;
	double sum_difference_squared = 0.0;

	if ( number_array->number_in_array < 2 ) return 0.0;

	for( n = 0; n < number_array->number_in_array; n++ )
	{
		difference = number_array->number[ n ] - average;
		difference_squared = difference * difference;
		sum_difference_squared += difference_squared;
	}

	return sqrt( ( 1.0 / (double)( n - 1 ) ) * sum_difference_squared );
}

double extract_piece_value( char *source, int *ignore, int piece_to_apply )
{
	char buffer[ 1024 ];

	if ( !piece(	buffer, 
			'|',
			source,
			piece_to_apply ) )
	{
		*ignore = 1;
		return 0.0;
	}
	else
	if ( !*buffer )
	{
		*ignore = 1;
		return 0.0;
	}
	else
	{
		*ignore = 0;
		return atof( buffer );
	}
}

int load_range( double less_than_this[], int interval )
{
	int i;
	int so_far = interval;

	for( i = 0; i < MAX_FREQUENCY_CELLS; i++ )
	{
		less_than_this[ i ] = so_far;
		so_far += interval;
	}

	return i;
}

void for_each_row(	int *count, 
			int *input_count,
			double *sum, 
			RANGE *range, 
			NUMBER_ARRAY *number_array,
			int piece_to_apply,
			int echo_input,
			int interval )
{
	char buffer[ 1024 ];
	double value;
	int i = 0;
	int ignore;
	int str_len = strlen( OUTPUT_IDENTIFIER );
	int exceeded_max_numbers = 0;

	while( get_line( buffer, stdin ) )
	{
		/* Ignore output from another process of this program. */
		/* --------------------------------------------------- */
		if ( strncmp( buffer, OUTPUT_IDENTIFIER, str_len ) == 0 )
			continue;

		if ( echo_input ) puts( buffer );

		(*input_count)++;

		if ( !exceeded_max_numbers && i == MAX_NUMBERS )
		{
			fprintf(stderr,
			"Warning: statistics max numbers of %d exceeded\n",
				MAX_NUMBERS );
			exceeded_max_numbers = 1;
		}

		value = extract_piece_value( buffer, 
					     &ignore, 
					     piece_to_apply );

		if ( ignore ) continue;

		(*count)++;
		(*sum) += value;

		if ( !exceeded_max_numbers )
			number_array->number[ i++ ] = value;

		if ( interval ) increment_range( range, value );
	}

	if ( exceeded_max_numbers )
		number_array->number_in_array = -1;
	else
		number_array->number_in_array = i;
}

void increment_range( RANGE *range, double value )
{
	int cell_offset;

	cell_offset = get_cell_offset( 	range->less_than_this,
					value );

	if ( cell_offset == -1 )
	{
		fprintf( stderr, 
		"statistics: Ignoring value out of range: %7.2lf\n",
			 value );
	}
	else
		range->count[ cell_offset ]++;
}

void output_histogram_1( 	
		int interval,
		double less_than_this[],
		int count[ MAX_FREQUENCY_CELLS ],
		int total_count )
{
	int i;

	for( i = 0; i < MAX_FREQUENCY_CELLS; i++ )
	{
		if ( count[ i ] )
		{
			printf( "%s %7.2lf =< %7.2lf\t%5d which is %5.2lf%c\n", 
				 OUTPUT_IDENTIFIER,
				 less_than_this[ i ] - interval, 
				 less_than_this[ i ], 
				 count[ i ],
				 (total_count == 0) ? 0.0 
						    : (double)count[ i ] / 
						      (double)total_count *
						      100.0,
				 '%' );
			fflush( stdout );
		}
	}
}

int get_cell_offset(
		double less_than_this[],
		double value )
{
	int i;

	for( i = 0; i < MAX_FREQUENCY_CELLS; i++ )
	{
		if ( value < less_than_this[ i ] )
		{
			return i;
		}
	}

	return -1;
}

void sort_number_array( NUMBER_ARRAY *number_array )
{
	qsort(	number_array->number, 
		number_array->number_in_array, 
		sizeof( double ), 
		double_compare );
}

int double_compare( const void *this, const void *that )
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
}

void output_number_1(	int count, 
			double average, 
			double std_dev, 
			RANGE *range, 
			NUMBER_ARRAY *number_array,
			char *column_name,
			int interval )
{
	printf( "\n" );
	printf( "%s For column:         %20s\n", 
		 OUTPUT_IDENTIFIER, 
		 column_name );

	printf( "%s Count:              %20d\n", 
		 OUTPUT_IDENTIFIER, 
		 count );

	printf( "%s Average, Std Dev:   %20.2lf, %-20.2lf\n", 
		 OUTPUT_IDENTIFIER, 
		 average,
		 std_dev );

	printf( "%s Average +- Std Dev: %20.2lf - %-20.2lf\n",
		OUTPUT_IDENTIFIER,
		average - std_dev, 
		average + std_dev );

	if ( number_array->number_in_array > 0 )
	{
		printf( "%s Min Median Max:     %20.2lf  %-.2lf %-.2lf\n",
		OUTPUT_IDENTIFIER,
		number_array->number[ 0 ],
		number_array->number[ 
			( number_array->number_in_array - 1 ) / 2 ],
		number_array->number[ number_array->number_in_array - 1 ] );
	}

	printf( "\n" );

	if ( interval )
	{
		printf ("%s Histogram\n", OUTPUT_IDENTIFIER );
		printf ("%s ---------\n", OUTPUT_IDENTIFIER );
		output_histogram_1(	interval,
			  		range->less_than_this,
			  		range->count,
			  		count );
	}
	fflush( stdout );
}

void output_number_2(	int count, 
			int input_count,
			double sum, 
			double average, 
			double std_dev, 
			double std_error, 
			NUMBER_ARRAY *number_array )
{
	if ( number_array->number_in_array > 0 )
	{
		printf( "Average: %.6lf\n", average );
		printf( "Sum: %.6lf\n", sum );
		printf( "Standard_Deviation: %.6lf\n", std_dev );
		printf( "Standard_Error: %.6lf\n", std_error );
		printf( "Minimum: %.6lf\n", number_array->number[ 0 ] );
		printf( "Median: %.6lf\n", 
			number_array->number[ 
				( number_array->number_in_array - 1 ) / 2 ] );
		printf( "Maximum: %.6lf\n", 
			number_array->number[ 
				number_array->number_in_array - 1 ] );
		printf( "Count: %d\n", count );
		printf( "Percent_Missing: %.4lf\n", 
			(double)(input_count - count) / (double)input_count );
	}
	else
	if ( number_array->number_in_array == -1 )
	{
		printf( "Average: %.6lf\n", average );
		printf( "Sum: %.6lf\n", sum );
		printf( "Standard_Deviation: null\n" );
		printf( "Standard_Error: null\n" );
		printf( "Minimum: null\n" );
		printf( "Median: null\n" );
		printf( "Maximum: null\n" );
		printf( "Count: %d\n", count );
		if ( input_count )
		{
			printf( "Percent_Missing: %.4lf\n", 
			(double)(input_count - count) / (double)input_count );
		}
		else
		{
			printf( "Percent_Missing: null\n" );
		}
	
	}
	else
	{
		printf( "Average: null\n" );
		printf( "Sum: null\n" );
		printf( "Standard_Deviation: null\n" );
		printf( "Standard_Error: null\n" );
		printf( "Minimum: null\n" );
		printf( "Median: null\n" );
		printf( "Maximum: null\n" );
		printf( "Count: null\n" );

		if ( input_count )
			printf( "Percent_Missing: 1.00\n" );
		else
			printf( "Percent_Missing: null\n" );
	}
	fflush( stdout );
}

void output_number_3(	int count, 
			int input_count,
			double sum, 
			double average, 
			double std_dev, 
			double std_error, 
			NUMBER_ARRAY *number_array )
{
	printf( "<table>\n" );

	if ( number_array->number_in_array )
	{
		if ( input_count != count )
		{
			printf( "<tr><td><p>Processed<td><p>%d\n", 
				input_count );
		}
		printf( "<tr><td><p>Count<td><p>%d\n", count );
		printf( "<tr><td><p>Average<td><p>%.6lf\n", average );
		printf( "<tr><td><p>Sum<td><p>%.6lf\n", sum );
		printf( "<tr><td><p>Standard_Deviation<td><p>%.6lf\n", 
			std_dev );
		printf( "<tr><td><p>Standard_Error<td><p>%.6lf\n", std_error );
		printf( "<tr><td><p>Minimum<td><p>%.6lf\n", 
			number_array->number[ 0 ] );
		printf( "<tr><td><p>Median<td><p>%.6lf\n", 
			number_array->number[ 
				( number_array->number_in_array - 1 ) / 2 ] );
		printf( "<tr><td><p>Maximum<td><p>%.6lf\n", 
			number_array->number[ 
				number_array->number_in_array - 1 ] );
		printf( "<tr><td><p>Range<td><p>%.6lf\n", 
			number_array->number[ 
				number_array->number_in_array - 1 ] -
			number_array->number[ 0 ] );
	}
	else
	{
		printf( "<tr><td><p>Processed<td><p>%d\n", input_count );
		printf( "<tr><td><p>Count<td><p>0\n" );
		printf( "<tr><td><p>Average<td><p>null\n" );
		printf( "<tr><td><p>Sum<td><p>null\n" );
		printf( "<tr><td><p>Standard_Deviation<td><p>null\n" );
		printf( "<tr><td><p>Standard_Error<td><p>null\n" );
		printf( "<tr><td><p>Minimum<td><p>null\n" );
		printf( "<tr><td><p>Median<td><p>null\n" );
		printf( "<tr><td><p>Maximum<td><p>null\n" );
		printf( "<tr><td><p>Range<td><p>null\n" );
	}

	printf( "</table>\n" );
}

