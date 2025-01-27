/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/statistic.c			   		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "html.h"
#include "float.h"
#include "appaserver_error.h"
#include "statistic.h"

STATISTIC_NUMBER_ARRAY *statistic_number_array_calloc( void )
{
	STATISTIC_NUMBER_ARRAY *statistic_number_array;

	if ( ! ( statistic_number_array =
			calloc( 1,
				sizeof ( STATISTIC_NUMBER_ARRAY ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return statistic_number_array;
}


void statistic_number_array_insert(
		STATISTIC_NUMBER_ARRAY *statistic_number_array,
		unsigned long statistic_number_array_max_size,
		double statistic_value,
		/* ------------ */
		/* At least one */
		/* ------------ */
		int statistic_weight )
{
	if ( !statistic_number_array
	||   statistic_weight < 1 )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	while ( statistic_weight-- )
	{
		if (	statistic_number_array->array_count == 
			statistic_number_array_max_size )
		{
			statistic_number_array->max_numbers_exceeded = 1;
		}
		else
		{
			statistic_number_array->number_array[
				statistic_number_array->array_count++ ] =
					statistic_value;
		}
	}
}

void statistic_number_array_sort(
		STATISTIC_NUMBER_ARRAY *statistic_number_array )
{
	if ( !statistic_number_array
	||   statistic_number_array->max_numbers_exceeded )
	{
		return;
	}

	if ( statistic_number_array->array_count > 1 )
	{
		qsort(
			statistic_number_array->number_array,
			statistic_number_array->array_count,
			sizeof( double ),
			statistic_number_array_compare );
	}

	statistic_number_array->sorted_boolean = 1;
}

int statistic_number_array_compare(
		const void *this,
		const void *that )
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

void statistic_number_array_free(
		STATISTIC_NUMBER_ARRAY *statistic_number_array )
{
	if ( statistic_number_array ) free( statistic_number_array );
}

STATISTIC_CALCULATE *statistic_calculate_new(
		STATISTIC_NUMBER_ARRAY *statistic_number_array,
		unsigned long weight_count,
		double sum,
		unsigned long row_count,
		unsigned long value_count,
		unsigned long non_zero_count )
{
	STATISTIC_CALCULATE *statistic_calculate = statistic_calculate_calloc();

	if ( sum )
	{
		statistic_calculate->average =
			statistic_calculate_average(
				weight_count,
				sum );
	}

	if ( row_count )
	{
		statistic_calculate->percent_missing =
			statistic_calculate_percent_missing(
				row_count,
				value_count );

		statistic_calculate->non_zero_percent =
			statistic_calculate_non_zero_percent(
				row_count,
				non_zero_count );
	}

	if ( !statistic_number_array ) return statistic_calculate;

	/*
	statistic_number_array_sort( statistic_number_array );
	*/

	statistic_calculate->sum_difference_squared =
		statistic_calculate_sum_difference_squared(
			statistic_number_array,
			statistic_calculate->average );

	statistic_calculate->variance =
		statistic_calculate_variance(
			statistic_number_array->array_count,
			statistic_number_array->max_numbers_exceeded,
			statistic_calculate->sum_difference_squared );

	statistic_calculate->standard_deviation =
		statistic_calculate_standard_deviation(
			statistic_calculate->variance );

	return statistic_calculate;
}

STATISTIC_CALCULATE *statistic_calculate_calloc( void )
{
	STATISTIC_CALCULATE *statistic_calculate;

	if ( ! ( statistic_calculate =
			calloc( 1,
				sizeof ( STATISTIC_CALCULATE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return statistic_calculate;
}

double statistic_calculate_average(
		unsigned long weight_count,
		double sum )
{
	if ( !weight_count )
		return 0.0;
	else
		return sum / (double)weight_count;
}

double statistic_calculate_percent_missing(
		unsigned long row_count,
		unsigned long value_count )
{
	if ( !row_count ) return 0.0;

	return
	( (double)( row_count - value_count ) / (double)row_count ) * 100.0;
}

double statistic_calculate_non_zero_percent(
		unsigned long row_count,
		unsigned long non_zero_count )
{
	double zero_percent;

	if ( !row_count ) return 0.0;

	zero_percent =
		( (double)( row_count - non_zero_count ) /
		  (double)row_count ) * 100.0;

	return 100.0 - zero_percent;
}

double statistic_quartile_minimum(
		STATISTIC_NUMBER_ARRAY *statistic_number_array )
{
	if ( !statistic_number_array
	||   !statistic_number_array->sorted_boolean )
	{
		return 0.0;
	}

	return statistic_number_array->number_array[ 0 ];
}

double statistic_quartile_median(
		STATISTIC_NUMBER_ARRAY *statistic_number_array,
		boolean statistic_quartile_array_even_boolean,
		int median_position )
{
	double median;

	if ( !statistic_number_array )
	{
		char message[ 128 ];

		sprintf(message, "statistic_number_array is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( statistic_number_array->array_count == 1 )
	{
		median =
			statistic_number_array->number_array[ 0 ];
	}
	else
	if ( !statistic_quartile_array_even_boolean )
	{
		median =
			statistic_number_array->
				number_array[
					median_position ];
	}
	else
	{
		double left_value;
		double right_value;

		left_value =
			statistic_number_array->
				number_array[
					median_position ];

		right_value =
			statistic_number_array->
				number_array[
					median_position + 1 ];

		median = ( left_value + right_value ) / 2.0;
	}

	return median;
}

boolean statistic_quartile_array_even_boolean( unsigned long array_count )
{
	if ( array_count % 2 )
		return 0;
	else
		return 1;
}

double statistic_quartile_maximum(
		STATISTIC_NUMBER_ARRAY *statistic_number_array )
{
	if ( !statistic_number_array )
	{
		char message[ 128 ];

		sprintf(message, "statistic_number_array is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	statistic_number_array->number_array[
		statistic_number_array->array_count - 1 ];
}

double statistic_quartile_range(
		double minimum,
		double maximum )
{
	return maximum - minimum;
}

double statistic_calculate_sum_difference_squared(
		STATISTIC_NUMBER_ARRAY *statistic_number_array,
		double average )
{
	double sum_difference_squared;
	double difference;
	unsigned long n;

	if ( !statistic_number_array
	||   statistic_number_array->array_count < 2
	||   statistic_number_array->max_numbers_exceeded )
	{
		return 0.0;
	}

	sum_difference_squared = 0.0;

	for( n = 0; n < statistic_number_array->array_count; n++ )
	{
		difference =
			statistic_number_array->number_array[ n ] -
			average;

		sum_difference_squared +=
			( difference * difference );
	}

	return sum_difference_squared;
}

double statistic_calculate_variance(
		int array_count,
		boolean max_numbers_exceeded,
		double sum_difference_squared )
{
	if ( array_count < 2
	||   max_numbers_exceeded )
	{
		return 0.0;
	}

	return
	sum_difference_squared /
	(double)( array_count - 1 );
}

double statistic_calculate_standard_deviation( double variance )
{
	return sqrt( variance );
}

void statistic_calculate_free( STATISTIC_CALCULATE *statistic_calculate )
{
	if ( statistic_calculate ) free( statistic_calculate );
}

STATISTIC *statistic_new(
		int value_piece,
		int weight_piece,
		char delimiter )
{
	STATISTIC *statistic = statistic_calloc();

	statistic->value_piece = value_piece;
	statistic->weight_piece = weight_piece;
	statistic->delimiter = delimiter;
	statistic->statistic_number_array = statistic_number_array_calloc();

	return statistic;
}

STATISTIC *statistic_calloc( void )
{
	STATISTIC *statistic;

	if ( ! ( statistic = calloc( 1, sizeof ( STATISTIC ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return statistic;
}

void statistic_accumulate(
		STATISTIC *statistic,
		STATISTIC_NUMBER_ARRAY *statistic_number_array,
		char *input )
{
	if ( !statistic )
	{
		char message[ 128 ];

		sprintf(message, "statistic is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	statistic->row_count++;

	statistic->empty_value =
		statistic_empty_value(
			statistic->value_piece,
			statistic->delimiter,
			input );

	if ( !statistic->empty_value )
	{
		statistic->value =
			statistic_value(
				statistic->value_piece,
				statistic->delimiter,
				input );

		statistic->value_count++;

		/* Returns at least one */
		/* -------------------- */
		statistic->weight =
			statistic_weight(
				statistic->weight_piece,
				statistic->delimiter,
				input );

		statistic->weight_count += statistic->weight;

		statistic->weight_value =
			statistic_weight_value(
				statistic->value,
				statistic->weight );

		statistic->sum += statistic->weight_value;

		if ( statistic_number_array
		&&   !statistic_number_array->max_numbers_exceeded )
		{
			statistic_number_array_insert(
				statistic_number_array /* in/out */,
				STATISTIC_NUMBER_ARRAY_MAX_SIZE,
				statistic->value,
				statistic->weight );
		}

		if ( statistic->weight_value )
		{
			statistic->non_zero_count++;
		}

	} /* if !statistic_empty_value() */
}

boolean statistic_empty_value(
		int value_piece,
		char delimiter,
		char *input )
{
	char value_string[ 128 ];

	if ( !input
	||   !*input
	||   value_piece < 0 )
	{
		return 1;
	}

	piece( value_string, delimiter, input, value_piece );

	if ( *value_string )
		return 0;
	else
		return 1;
}

double statistic_value(
		int value_piece,
		char delimiter,
		char *input )
{
	char value_string[ 128 ];

	if ( !input
	||   !*input
	||   value_piece < 0 )
	{
		return 0.0;
	}

	piece( value_string, delimiter, input, value_piece );

	return atof( value_string );
}

int statistic_weight(
		int weight_piece,
		char delimiter,
		char *input )
{
	char weight_string[ 128 ];

	if ( !input
	||   !*input
	||   weight_piece < 0 )
	{
		return 1;
	}

	piece( weight_string, delimiter, input, weight_piece );

	return atoi( weight_string );
}

double statistic_weight_value(
		double statistic_value,
		int statistic_weight )
{
	return statistic_value * (double)statistic_weight;
}

void statistic_free( STATISTIC *statistic )
{
	if ( !statistic ) return;

	statistic_number_array_free(
		statistic->statistic_number_array );

	if ( statistic->statistic_calculate )
	{
		statistic_calculate_free(
			statistic->statistic_calculate );
	}

	if ( statistic->statistic_quartile )
	{
		statistic_quartile_free(
			statistic->statistic_quartile );
	}

	free( statistic );
}

STATISTIC_DATE *statistic_date_new(
		int value_piece,
		char delimiter )
{
	STATISTIC_DATE *statistic_date = statistic_date_calloc();

	statistic_date->value_piece = value_piece;
	statistic_date->delimiter = delimiter;

	return statistic_date;
}

STATISTIC_DATE *statistic_date_calloc( void )
{
	STATISTIC_DATE *statistic_date;

	if ( ! ( statistic_date = calloc( 1, sizeof ( STATISTIC_DATE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return statistic_date;
}

void statistic_date_accumulate(
		STATISTIC_DATE *statistic_date /* in/out */,
		char *input )
{
	if ( !statistic_date
	||   !input )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	statistic_date->row_count++;

	if ( !statistic_empty_value(
		statistic_date->value_piece,
		statistic_date->delimiter,
		input ) )
	{
		char *value;

		value =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			statistic_date_value(
				statistic_date->value_piece,
				statistic_date->delimiter,
				input );

		statistic_date->value_count++;

		statistic_date_new_minimum_value(
			statistic_date->minimum_value /* in/out */,
			value );

		statistic_date_new_maximum_value(
			statistic_date->maximum_value /* in/out */,
			value );
	}

	statistic_date->range =
		/* ---------------------- */
		/* Ignores trailing time. */
		/* ---------------------- */
		date_days_between(
			statistic_date->minimum_value /* from_date */,
			statistic_date->maximum_value /* to_date */ );

	statistic_date->percent_missing =
		statistic_calculate_percent_missing(
			statistic_date->row_count,
			statistic_date->value_count );
}

char *statistic_date_value(
		int value_piece,
		char delimiter,
		char *input )
{
	static char value[ 32 ];

	if ( !input )
	{
		char message[ 128 ];

		sprintf(message, "input is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	piece( value, delimiter, input, value_piece );

	return value;
}

void statistic_date_new_minimum_value(
		char minimum_value[] /* in/out */,
		char *statistic_date_value )
{
	if ( !statistic_date_value || !*statistic_date_value ) return;

	if ( !*minimum_value )
		strcpy( minimum_value, statistic_date_value );
	else
	if ( strcmp( statistic_date_value, minimum_value ) < 0 )
		strcpy( minimum_value, statistic_date_value );
}

void statistic_date_new_maximum_value(
		char maximum_value[] /* in/out */,
		char *statistic_date_value )
{
	if ( !statistic_date_value || !*statistic_date_value ) return;

	if ( !*maximum_value )
		strcpy( maximum_value, statistic_date_value );
	else
	if ( strcmp( statistic_date_value, maximum_value ) > 0 )
		strcpy( maximum_value, statistic_date_value );
}

char *statistic_quartile_range_string( double range )
{
	return
	strdup(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_money(
			range ) );
}

char *statistic_calculate_percent_missing_string( double percent_missing )
{
	return
	strdup(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_money(
			percent_missing ) );
}

LIST *statistic_table_cell_list(
		STATISTIC *statistic,
		char *label,
		boolean outlier_boolean )
{
	LIST *cell_list;

	if ( !statistic
	||   !statistic->statistic_calculate
	||   !statistic->statistic_quartile
	||   !label )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			label /* data */,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------------- */
				/* Returns static memory or "" */
				/* --------------------------- */
				string_commas_unsigned_long(
					statistic->value_count )  ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_money(
					statistic->sum ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_money(
					statistic->
						statistic_quartile->
						minimum ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_money(
					statistic->
						statistic_quartile->
						first ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_money(
					statistic->
						statistic_calculate->
						average ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_money(
					statistic->
						statistic_quartile->
						median ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_money(
					statistic->
						statistic_quartile->
						third ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_money(
					statistic->
						statistic_quartile->
						maximum ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statistic_quartile_range_string(
				statistic->
					statistic_quartile->
					range ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statistic_quartile_range_string(
				statistic->
					statistic_quartile->
					innerquartile_range ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_money(
					statistic->
						statistic_quartile->
						coefficient_of_dispersion ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	if (	outlier_boolean
	&&	statistic->statistic_quartile->outlier_low_count )
	{
		list_set(
			cell_list,
			html_cell_new(
				strdup(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					string_commas_integer(
						statistic->
							statistic_quartile->
							outlier_low_count ) ),
				0 /* not large_boolean */,
				0 /* not bold_boolean */ ) );
	}

	if (	outlier_boolean
	&&	statistic->statistic_quartile->outlier_high_count )
	{
		list_set(
			cell_list,
			html_cell_new(
				strdup(
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					string_commas_integer(
						statistic->
							statistic_quartile->
							outlier_high_count ) ),
				0 /* not large_boolean */,
				0 /* not bold_boolean */ ) );
	}

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				string_commas_money(
					statistic->
						statistic_calculate->
						standard_deviation ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statistic_calculate_percent_missing_string(
				statistic->
					statistic_calculate->
					percent_missing ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	return cell_list;
}

LIST *statistic_table_column_list(
		char *outlier_low_heading_string,
		char *outlier_high_heading_string )
{
	LIST *list = list_new();

	list_set(
		list,
		html_column_new(
			"Attribute",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"Count",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"Sum",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"Minimum",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"1st Quartile",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"Average",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"Median",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"3rd Quartile",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"Maximum",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"Range",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"IQR:<br>Q3&nbsp;-&nbsp;Q1",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"Quartile Dispersion:<br>IQR/<br>(Q3&nbsp;+&nbsp;Q1)",
			1 /* right_justify_boolean */ ) );

	if ( outlier_low_heading_string )
	{
		list_set(
			list,
			html_column_new(
				outlier_low_heading_string,
				1 /* right_justify_boolean */ ) );
	}

	if ( outlier_high_heading_string )
	{
		list_set(
			list,
			html_column_new(
				outlier_high_heading_string,
				1 /* right_justify_boolean */ ) );
	}

	list_set(
		list,
		html_column_new(
			"Standard Deviation",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"Percent Missing",
			1 /* right_justify_boolean */ ) );

	return list;
}

STATISTIC_QUARTILE *statistic_quartile_new(
		STATISTIC_NUMBER_ARRAY *statistic_number_array )
{
	STATISTIC_QUARTILE *statistic_quartile;

	statistic_quartile = statistic_quartile_calloc();

	if (	!statistic_number_array
	||	!statistic_number_array->array_count
	||	!statistic_number_array->sorted_boolean
	||	statistic_number_array->max_numbers_exceeded )
	{
		return statistic_quartile;
	}

	statistic_quartile->minimum =
		statistic_quartile_minimum(
			statistic_number_array );

	statistic_quartile->maximum =
		statistic_quartile_maximum(
			statistic_number_array );

	statistic_quartile->range =
		statistic_quartile_range(
			statistic_quartile->minimum,
			statistic_quartile->maximum );

	statistic_quartile->array_even_boolean =
		statistic_quartile_array_even_boolean(
			statistic_number_array->
				array_count );

	statistic_quartile->median_position =
		statistic_quartile_median_position(
			statistic_number_array->array_count,
			statistic_quartile->array_even_boolean );

	statistic_quartile->median =
		statistic_quartile_median(
			statistic_number_array,
			statistic_quartile->array_even_boolean,
			statistic_quartile->median_position );

	statistic_quartile->first_position =
		statistic_quartile_first_position(
			statistic_number_array->array_count,
			statistic_quartile->median_position );

	statistic_quartile->first =
		statistic_quartile_first(
			statistic_number_array,
			statistic_quartile->array_even_boolean,
			statistic_quartile->range,
			statistic_quartile->first_position );

	statistic_quartile->third_position =
		statistic_quartile_third_position(
			statistic_number_array->array_count,
			statistic_quartile->median_position,
			statistic_quartile->first_position );

	statistic_quartile->third =
		statistic_quartile_third(
			statistic_number_array,
			statistic_quartile->array_even_boolean,
			statistic_quartile->range,
			statistic_quartile->third_position );

	statistic_quartile->innerquartile_range =
		statistic_quartile_innerquartile_range(
			statistic_quartile->first,
			statistic_quartile->third );

	statistic_quartile->coefficient_of_dispersion =
		statistic_quartile_coefficient_of_dispersion(
				statistic_quartile->first,
				statistic_quartile->third,
				statistic_quartile->innerquartile_range );

	statistic_quartile->outlier_low_limit =
		statistic_quartile_outlier_low_limit(
			statistic_quartile->first,
			statistic_quartile->innerquartile_range );

	statistic_quartile->outlier_high_limit =
		statistic_quartile_outlier_high_limit(
			statistic_quartile->third,
			statistic_quartile->innerquartile_range );

	statistic_quartile->outlier_low_count =
		statistic_quartile_outlier_low_count(
			statistic_number_array,
			statistic_quartile->outlier_low_limit );

	statistic_quartile->outlier_high_count =
		statistic_quartile_outlier_high_count(
			statistic_number_array,
			statistic_quartile->outlier_high_limit );

	if ( statistic_quartile->outlier_low_count )
	{
		statistic_quartile->outlier_low_heading_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statistic_quartile_outlier_low_heading_string(
				statistic_quartile->outlier_low_limit );
	}

	if ( statistic_quartile->outlier_high_count )
	{
		statistic_quartile->outlier_high_heading_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			statistic_quartile_outlier_high_heading_string(
				statistic_quartile->outlier_high_limit );
	}

	return statistic_quartile;
}

STATISTIC_QUARTILE *statistic_quartile_calloc( void )
{
	STATISTIC_QUARTILE *statistic_quartile;

	if ( ! ( statistic_quartile =
			calloc( 1, sizeof ( STATISTIC_QUARTILE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return statistic_quartile;
}

void statistic_quartile_free( STATISTIC_QUARTILE *statistic_quartile )
{
	if ( statistic_quartile ) free( statistic_quartile );
}

int statistic_quartile_third_position(
		unsigned long array_count,
		int statistic_quartile_median_position,
		int statistic_quartile_first_position )
{
	int position;

	if ( array_count <= 2 )
	{
		position = 0;
	}
	else
	{
		position =
			statistic_quartile_median_position +
			statistic_quartile_first_position;
	}

	return position;
}

int statistic_quartile_first_position(
		unsigned long array_count,
		int statistic_quartile_median_position )
{
	int position;

	if ( array_count <= 2 )
	{
		position = 0;
	}
	else
	/* ------------------------- */
	/* If median position is odd */
	/* ------------------------- */
	if ( statistic_quartile_median_position % 2 )
	{
		position =
			( statistic_quartile_median_position - 1 ) / 2;
	}
	else
	{
		position =
			( statistic_quartile_median_position - 2 ) / 2;
	}

	return position;
}

int statistic_quartile_median_position(
		unsigned long array_count,
		boolean statistic_quartile_array_even_boolean )
{
	int median_position;

	if ( array_count <= 2 )
		median_position = 0;
	else
	if ( !statistic_quartile_array_even_boolean )
		median_position = ( array_count - 3 ) / 2;
	else
		median_position = ( array_count - 2 ) / 2;

	return median_position;
}

double statistic_quartile_third(
		STATISTIC_NUMBER_ARRAY *statistic_number_array,
		boolean statistic_quartile_array_even_boolean,
		double statistic_quartile_range,
		int statistic_quartile_third_position )
{
	double third;

	if ( !statistic_number_array )
	{
		char message[ 128 ];

		sprintf(message, "statistic_number_array is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( statistic_number_array->array_count == 1 )
	{
		third =
			statistic_number_array->
				number_array[ 0 ];
	}
	else
	if ( statistic_number_array->array_count == 2 )
	{
		third =
			statistic_number_array->number_array[ 1 ] -
			( statistic_quartile_range * 0.25 );
	}
	else
	if ( !statistic_quartile_array_even_boolean )
	{
		third =
			statistic_number_array->
				number_array[
					statistic_quartile_third_position ];
	}
	else
	{
		double left_value;
		double right_value;

		left_value =
			statistic_number_array->
				number_array[
					statistic_quartile_third_position ];

		right_value =
			statistic_number_array->
				number_array[
					statistic_quartile_third_position + 1 ];

		third =
			( left_value + right_value ) / 2.0;
	}

	return third;
}

double statistic_quartile_first(
		STATISTIC_NUMBER_ARRAY *
			statistic_number_array,
		boolean statistic_quartile_array_even_boolean,
		double statistic_quartile_range,
		int statistic_quartile_first_position )
{
	double first;

	if ( !statistic_number_array )
	{
		char message[ 128 ];

		sprintf(message, "statistic_number_array is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( statistic_number_array->array_count == 1 )
	{
		first =
			statistic_number_array->
				number_array[ 0 ];
	}
	else
	if ( statistic_number_array->array_count == 2 )
	{
		first =
			statistic_number_array->number_array[ 0 ] +
			( statistic_quartile_range * 0.25 );
	}
	else
	if ( !statistic_quartile_array_even_boolean )
	{
		first =
			statistic_number_array->
				number_array[
					statistic_quartile_first_position ];
	}
	else
	{
		double left_value;
		double right_value;

		left_value =
			statistic_number_array->
				number_array[
					statistic_quartile_first_position ];
	
		right_value =
			statistic_number_array->
				number_array[
					statistic_quartile_first_position + 1 ];
	
		first =
			( left_value + right_value ) / 2.0;
	}

	return first;
}

double statistic_quartile_innerquartile_range(
		double first,
		double third )
{
	return third - first;
}

double statistic_quartile_coefficient_of_dispersion(
		double first,
		double third,
		double innerquartile_range )
{
	if ( !innerquartile_range ) return 0.0;

	return
	float_abs( innerquartile_range / ( first + third ) );
}

double statistic_quartile_outlier_low_limit(
		double statistic_quartile_first,
		double statistic_quartile_innerquartile_range )
{
	return
	statistic_quartile_first -
	1.5 * statistic_quartile_innerquartile_range;
}

double statistic_quartile_outlier_high_limit(
		double statistic_quartile_third,
		double statistic_quartile_innerquartile_range )
{
	return
	statistic_quartile_third +
	1.5 * statistic_quartile_innerquartile_range;
}

int statistic_quartile_outlier_low_count(
		STATISTIC_NUMBER_ARRAY *
			statistic_number_array,
		double statistic_quartile_outlier_low_limit )
{
	int low_count = 0;
	unsigned long index;

	if ( !statistic_number_array
	||   !statistic_number_array->array_count )
	{
		return 0;
	}

	for (	index = 0;
		index < statistic_number_array->array_count;
		index++ )
	{
		if (	statistic_number_array->number_array[ index ] <
			statistic_quartile_outlier_low_limit )
		{
			low_count++;
		}
	}

	return low_count;
}

int statistic_quartile_outlier_high_count(
		STATISTIC_NUMBER_ARRAY *
			statistic_number_array,
		double statistic_quartile_outlier_high_limit )
{
	int high_count = 0;
	unsigned long index;

	if ( !statistic_number_array
	||   !statistic_number_array->array_count )
	{
		return 0;
	}

	for (	index = 0;
		index < statistic_number_array->array_count;
		index++ )
	{
		if (	statistic_number_array->number_array[ index ] >
			statistic_quartile_outlier_high_limit )
		{
			high_count++;
		}
	}

	return high_count;
}

char *statistic_quartile_outlier_low_heading_string(
		double statistic_quartile_outlier_low_limit )
{
	char heading_string[ 128 ];

	sprintf(heading_string,
		"Low Outlier Count:<br>< %s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_money(
			statistic_quartile_outlier_low_limit ) );

	return strdup( heading_string );
}

char *statistic_quartile_outlier_high_heading_string(
		double statistic_quartile_outlier_high_limit )
{
	char heading_string[ 128 ];

	sprintf(heading_string,
		"High Outlier Count:<br>> %s",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_commas_money(
			statistic_quartile_outlier_high_limit ) );

	return strdup( heading_string );
}

