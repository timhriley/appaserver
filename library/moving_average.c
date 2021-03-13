/* $APPASERVER_HOME/library/moving_average.c		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <float.h>
#include "timlib.h"
#include "piece.h"
#include "moving_average.h"

MOVING_AVERAGE *moving_average_new(
			int number_of_days,
			char delimiter,
			enum aggregate_statistic aggregate_statistic,
			char *begin_date_string )
{
	MOVING_AVERAGE *moving_average;

	moving_average =
		(MOVING_AVERAGE *)
			calloc( 1, sizeof( MOVING_AVERAGE ) );

	moving_average->value_list = list_new_list();
	moving_average->number_of_days = number_of_days;
	moving_average->delimiter = delimiter;
	moving_average->aggregate_statistic = aggregate_statistic;
	moving_average->begin_date_string = begin_date_string;

	if ( !moving_average->number_of_days )
	{
		moving_average->number_of_days =
			MOVING_AVERAGE_DEFAULT;
	}

	return moving_average;
}

void moving_average_display(
			LIST *value_list,
			int number_of_days,
			char delimiter,
			enum aggregate_statistic aggregate_statistic,
			char *begin_date_string )
{
	MOVING_AVERAGE_VALUE *value;

	if ( !list_go_tail( value_list ) ) return;

	do {
		value = list_get_pointer( value_list );

		if ( strcmp( value->date_label, begin_date_string ) < 0 )
			return;

		if ( aggregate_statistic == average )
		{
			moving_average_display_average(
					value_list,
					number_of_days,
					delimiter );
		}
		else
		if ( aggregate_statistic == maximum )
		{
			moving_average_display_maximum(
					value_list,
					number_of_days,
					delimiter );
		}
		else
		if ( aggregate_statistic == minimum )
		{
			moving_average_display_minimum(
					value_list,
					number_of_days,
					delimiter );
		}
		else
		if ( aggregate_statistic == sum )
		{
			moving_average_display_sum(
					value_list,
					number_of_days,
					delimiter );
		}
		else
		if ( aggregate_statistic == median )
		{
			moving_average_display_statistics_weighted(
					value_list,
					number_of_days,
					delimiter,
					STATISTICS_WEIGHTED_MEDIAN );
		}
		else
		if ( aggregate_statistic == range )
		{
			moving_average_display_statistics_weighted(
					value_list,
					number_of_days,
					delimiter,
					STATISTICS_WEIGHTED_RANGE );
		}
		else
		if ( aggregate_statistic == standard_deviation )
		{
			moving_average_display_statistics_weighted(
				value_list,
				number_of_days,
				delimiter,
				STATISTICS_WEIGHTED_STANDARD_DEVIATION );
		}
		else
		if ( aggregate_statistic == non_zero_percent )
		{
			moving_average_display_non_zero_percent(
				value_list,
				number_of_days,
				delimiter,
				STATISTICS_WEIGHTED_PERCENT_MISSING );
		}
		else
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: Invalid aggregate_statistic = %d\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				aggregate_statistic );
			exit( 1 );
		}
	} while( list_previous( value_list ) );

}

void moving_average_set(LIST *value_list,
			double value,
			char *date_label,
			boolean is_null )
{
	MOVING_AVERAGE_VALUE *moving_average_value;

	moving_average_value = 
		moving_average_new_value(
			value,
			date_label,
			is_null );

	list_set(
		value_list,
		moving_average_value );

}

void moving_average_free( MOVING_AVERAGE *moving_average )
{
	list_free( moving_average->value_list );
	free( moving_average );
}

MOVING_AVERAGE_VALUE *moving_average_new_value(
			double value,
			char *date_label,
			boolean is_null )
{
	MOVING_AVERAGE_VALUE *moving_average_value;

	moving_average_value =
		(MOVING_AVERAGE_VALUE *)
			calloc( 1, sizeof( MOVING_AVERAGE_VALUE ) );

	if ( !moving_average_value )
	{
		fprintf(	stderr,
				"ERROR in %s/%s(): memory allocation error\n",
				__FILE__,
				__FUNCTION__ );
		exit( 1 );
	}

	moving_average_value->value = value;
	moving_average_value->date_label = date_label;
	moving_average_value->is_null = is_null;

	return moving_average_value;
}

void moving_average_display_average(
			LIST *value_list,
			int number_of_days,
			char delimiter )
{
	MOVING_AVERAGE_VALUE *value;
	double total = 0.0;
	int count = 0;

	list_push( value_list );

	do {
		if ( !number_of_days-- ) break;

		value = list_get_pointer( value_list );

		if ( !value->is_null )
		{
			count++;
			total += value->value;
		}
	} while( list_previous( value_list ) );

	list_pop( value_list );

	value = list_get_pointer( value_list );
/*
	if ( count == number_of_days )
*/
	if ( count )
	{
		printf( "%s%c%.3lf\n",
			value->date_label,
			delimiter,
			total / (double)count );
	}
	else
	{
		printf( "%s%cnull\n",
			value->date_label,
			delimiter );
	}
}

void moving_average_display_sum(
			LIST *value_list,
			int number_of_days,
			char delimiter )
{
	MOVING_AVERAGE_VALUE *value;
	double sum = 0.0;
	int count = 0;

	list_push( value_list );

	do {
		if ( !number_of_days-- ) break;

		value = list_get_pointer( value_list );

		if ( !value->is_null )
		{
			sum += value->value;
			count++;
		}
	} while( list_previous( value_list ) );

	list_pop( value_list );

	value = list_get_pointer( value_list );

	if ( count )
	{
		printf( "%s%c%.3lf\n",
			value->date_label,
			delimiter,
			sum );
	}
	else
	{
		printf( "%s%cnull\n",
			value->date_label,
			delimiter );
	}

}

void moving_average_display_statistics_weighted(
			LIST *value_list,
			int number_of_days,
			char delimiter,
			char *statistics_weighted_label )
{
	MOVING_AVERAGE_VALUE *value;
	char temporary_filename[ 128 ];
	char sys_string[ 1024 ];
	FILE *output_pipe;
	int count = 0;

	sprintf( temporary_filename, "/tmp/moving_average_%d.dat", getpid() );

	sprintf( sys_string,
		 "statistics_weighted.e '|' 0 > %s",
		 temporary_filename );
	
	output_pipe = popen( sys_string, "w" );

	list_push( value_list );

	do {
		if ( !number_of_days-- ) break;

		value = list_get_pointer( value_list );

		if ( !value->is_null )
		{
			fprintf( output_pipe, "%.3lf\n", value->value );
			count++;
		}
	} while( list_previous( value_list ) );

	pclose( output_pipe );

	list_pop( value_list );

	value = list_get_pointer( value_list );

	if ( count )
	{
		char *results_string;

		sprintf( sys_string,
			 "grep '%s' %s | piece.e ':' 1",
			 statistics_weighted_label,
			 temporary_filename );

		results_string = pipe2string( sys_string );

		if ( results_string )
		{
			printf( "%s%c%s\n",
				value->date_label,
				delimiter,
				results_string );
		}
		else
		{
			printf( "%s%cnull\n",
				value->date_label,
				delimiter );
		}
	}
	else
	{
		printf( "%s%cnull\n",
			value->date_label,
			delimiter );
	}

}

void moving_average_display_non_zero_percent(
			LIST *value_list,
			int number_of_days,
			char delimiter,
			char *statistics_weighted_label )
{
	MOVING_AVERAGE_VALUE *value;
	char temporary_filename[ 128 ];
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *results_string;

	sprintf( temporary_filename, "/tmp/moving_average_%d.dat", getpid() );

	sprintf( sys_string,
		 "statistics_weighted.e '|' 0 > %s",
		 temporary_filename );
	
	output_pipe = popen( sys_string, "w" );

	list_push( value_list );

	do {
		if ( !number_of_days-- ) break;

		value = list_get_pointer( value_list );

		if ( value->is_null )
		{
			fprintf( output_pipe, "\n" );
		}
		else
		{
			fprintf( output_pipe, "0.0\n" );
		}

	} while( list_previous( value_list ) );

	pclose( output_pipe );

	list_pop( value_list );

	value = list_get_pointer( value_list );


	sprintf( sys_string,
		 "grep '%s' %s | piece.e ':' 1",
		 statistics_weighted_label,
		 temporary_filename );

	results_string = pipe2string( sys_string );

	if ( results_string )
	{
		double inverse;

		inverse = 1.0 - atof( results_string );

		printf( "%s%c%.1lf\n",
			value->date_label,
			delimiter,
			inverse * 100.0 );
	}
	else
	{
		printf( "%s%cnull\n",
			value->date_label,
			delimiter );
	}

}

void moving_average_display_minimum(
			LIST *value_list,
			int number_of_days,
			char delimiter )
{
	MOVING_AVERAGE_VALUE *value;
	double minimum = DOUBLE_MAXIMUM;
	int count = 0;

	list_push( value_list );

	do {
		if ( !number_of_days-- ) break;

		value = list_get_pointer( value_list );

		if ( !value->is_null )
		{
			if ( value->value < minimum )
				minimum = value->value;
			count++;
		}
	} while( list_previous( value_list ) );

	list_pop( value_list );

	value = list_get_pointer( value_list );

	if ( count )
	{
		printf( "%s%c%.3lf\n",
			value->date_label,
			delimiter,
			minimum );
	}
	else
	{
		printf( "%s%cnull\n",
			value->date_label,
			delimiter );
	}

}

void moving_average_display_maximum(
			LIST *value_list,
			int number_of_days,
			char delimiter )
{
	MOVING_AVERAGE_VALUE *value;
	double maximum = DOUBLE_MINIMUM;
	int count = 0;

	list_push( value_list );

	do {
		if ( !number_of_days-- ) break;

		value = list_get_pointer( value_list );

		if ( !value->is_null )
		{
			if ( value->value > maximum )
				maximum = value->value;
			count++;
		}
	} while( list_previous( value_list ) );

	list_pop( value_list );

	value = list_get_pointer( value_list );

	if ( count )
	{
		printf( "%s%c%.3lf\n",
			value->date_label,
			delimiter,
			maximum );
	}
	else
	{
		printf( "%s%cnull\n",
			value->date_label,
			delimiter );
	}

}

void moving_average_piece_set(
			LIST *value_list,
			double value,
			char *date_label,
			char *input )
{
	MOVING_AVERAGE_VALUE *moving_average_value;

	moving_average_value = 
		moving_average_new_value(
			value,
			date_label,
			0 /* not is_null */ );

	moving_average_value->input = input;

	list_set(
		value_list,
		moving_average_value );

}

void moving_average_piece_display(
			FILE *output_pipe,
			LIST *value_list,
			int number_of_days,
			char delimiter,
			int value_piece )
{
	if ( !list_go_tail( value_list ) ) return;

	do {
		moving_average_piece_display_average(
				output_pipe,
				value_list,
				number_of_days,
				delimiter,
				value_piece );

	} while( list_previous( value_list ) );
}

void moving_average_piece_display_average(
			FILE *output_pipe,
			LIST *value_list,
			int number_of_days,
			char delimiter,
			int value_piece )
{
	MOVING_AVERAGE_VALUE *value;
	char value_string[ 128 ];
	double total = 0.0;
	int count = 0;

	list_push( value_list );

	do {
		if ( !number_of_days-- ) break;

		value = list_get( value_list );

		if ( !value->is_null )
		{
			count++;
			total += value->value;
		}
	} while( list_previous( value_list ) );

	list_pop( value_list );

	if ( count )
	{
		sprintf( value_string, "%.4lf", total / (double )count );
	}
	else
	{
		strcpy( value_string, "null" );
	}

	value = list_get( value_list );

	piece_replace(
		value->input /* source_destination */, 
		delimiter, 
		value_string /* new_data */, 
		value_piece );

	fprintf( output_pipe, "%s\n", value->input );
}

