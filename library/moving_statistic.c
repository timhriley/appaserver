/* ----------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/moving_statistic.c				   */
/* ----------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	   */
/* ----------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <float.h>
#include "String.h"
#include "piece.h"
#include "appaserver_error.h"
#include "moving_statistic.h"

MOVING_STATISTIC *moving_statistic_new(
			int days_count,
			int value_piece,
			char delimiter,
			enum aggregate_statistic aggregate_statistic,
			char *begin_date_string )
{
	MOVING_STATISTIC *moving_statistic;

	if ( !begin_date_string )
	{
		char message[ 128 ];

		sprintf(message, "begin_date_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	moving_statistic = moving_statistic_calloc();

	if ( !days_count ) days_count = MOVING_STATISTIC_DEFAULT_DAYS;

	moving_statistic->days_count = days_count;
	moving_statistic->value_piece = value_piece;
	moving_statistic->delimiter = delimiter;
	moving_statistic->aggregate_statistic = aggregate_statistic;
	moving_statistic->begin_date_string = begin_date_string;
	moving_statistic->moving_statistic_value_list = list_new();

	return moving_statistic;
}

MOVING_STATISTIC *moving_statistic_calloc( void )
{
	MOVING_STATISTIC *moving_statistic;

	if ( ! ( moving_statistic = calloc( 1, sizeof ( MOVING_STATISTIC ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return moving_statistic;
}

void moving_statistic_input(
			LIST *moving_statistic_value_list,
			char delimiter,
			char *input )
{
	char date_string[ 128 ];

	if ( !moving_statistic_value_list
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

	piece( date_string, delimiter, input, 0 );

	if ( *date_string )
	{
		list_set(
			moving_statistic_value_list,
			moving_statistic_value_new(
				input,
				strdup( date_string ) ) );
	}
}

void moving_statistic_display(
			int days_count,
			int value_piece,
			char delimiter,
			enum aggregate_statistic aggregate_statistic,
			char *begin_date_string,
			LIST *moving_statistic_value_list )
{
	FILE *sort_pipe;
	MOVING_STATISTIC_VALUE *moving_statistic_value;
	STATISTIC *statistic;

	if ( !days_count
	||   !begin_date_string
	||   !moving_statistic_value_list )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_tail( moving_statistic_value_list ) ) return;

	sort_pipe = popen( "sort", "w" );

	do {
		moving_statistic_value =
			list_get(
				moving_statistic_value_list );

		statistic =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			statistic_new(
				value_piece,
				-1 /* weight_piece */,
				delimiter );

		list_push( moving_statistic_value_list );

		moving_statistic_accumulate(
			statistic,
			days_count,
			moving_statistic_value_list );

		list_pop( moving_statistic_value_list );

		statistic->statistic_calculate =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
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
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			statistic_quartile_new(
				statistic->statistic_number_array );

		moving_statistic_output(
			value_piece,
			delimiter,
			aggregate_statistic,
			sort_pipe,
			moving_statistic_value->input,
			statistic->sum,
			statistic->statistic_calculate,
			statistic->statistic_quartile );

		statistic_free( statistic );

		if ( strcmp(
			moving_statistic_value->date_string,
			begin_date_string ) <= 0 )
		{
			break;
		}

	} while ( list_previous( moving_statistic_value_list ) );

	pclose( sort_pipe );
}

void moving_statistic_accumulate(
			STATISTIC *statistic /* in/out */,
			int days_count,
			LIST *moving_statistic_value_list )
{
	MOVING_STATISTIC_VALUE *moving_statistic_value;

	if ( !statistic
	||   !days_count
	||   !moving_statistic_value_list )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	while ( 1 )
	{
		moving_statistic_value =
			list_get(
				moving_statistic_value_list );

		statistic_accumulate(
			statistic /* in/out */,
			statistic->statistic_number_array /* in/out */,
			moving_statistic_value->input );

		list_previous( moving_statistic_value_list );

		if ( --days_count == 0 ) break;
	}
}

void moving_statistic_output(
		int value_piece,
		char delimiter,
		enum aggregate_statistic aggregate_statistic,
		FILE *sort_pipe,
		char *input,
		double sum,
		STATISTIC_CALCULATE *statistic_calculate,
		STATISTIC_QUARTILE *statistic_quartile )
{
	char *replace;

	if ( !sort_pipe
	||   !input
	||   !statistic_calculate
	||   !statistic_quartile )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	replace =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		moving_statistic_replace(
			value_piece,
			delimiter,
			input,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			moving_statistic_value_string(
				aggregate_statistic,
				sum,
				statistic_calculate,
				statistic_quartile ) );

	fprintf(sort_pipe,
		"%s\n",
		replace );

	free( replace );
}

char *moving_statistic_value_string(
		enum aggregate_statistic aggregate_statistic,
		double sum,
		STATISTIC_CALCULATE *statistic_calculate,
		STATISTIC_QUARTILE *statistic_quartile )
{
	if ( !statistic_calculate
	||   !statistic_quartile )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( aggregate_statistic == average )
	{
		return
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_ftoa(
			statistic_calculate->average,
			4 /* decimal_places */ );
	}
	else
	if ( aggregate_statistic == sum )
	{
		return
		string_ftoa( sum, 4 );
	}
	else
	if ( aggregate_statistic == minimum )
	{
		return
		string_ftoa( statistic_quartile->minimum, 4 );
	}
	else
	if ( aggregate_statistic == median )
	{
		return
		string_ftoa( statistic_quartile->median, 4 );
	}
	else
	if ( aggregate_statistic == maximum )
	{
		return
		string_ftoa( statistic_quartile->maximum, 4 );
	}
	else
	if ( aggregate_statistic == range )
	{
		return
		string_ftoa( statistic_quartile->range, 4 );
	}
	else
	if ( aggregate_statistic == standard_deviation )
	{
		return
		string_ftoa( statistic_calculate->standard_deviation, 4 );
	}
	else
	if ( aggregate_statistic == non_zero_percent )
	{
		return
		string_ftoa( statistic_calculate->non_zero_percent, 1 );
	}
	else
	{
		char message[ 128 ];

		sprintf(message,
			"invalid aggregate_statistic = %d.",
			aggregate_statistic );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* Stub */
	/* ---- */
	exit( 1 );
}

char *moving_statistic_replace(
		int value_piece,
		char delimiter,
		char *input,
		char *value_string )
{
	char replace[ 256 ];

	if ( !input
	||   !value_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	strcpy( replace, input );

	piece_replace(
		replace /* source_destination */, 
		delimiter, 
		value_string /* new_data */, 
		value_piece );

	return strdup( replace );
}

MOVING_STATISTIC_VALUE *moving_statistic_value_new(
		char *input,
		char *date_string )
{
	MOVING_STATISTIC_VALUE *moving_statistic_value;

	if ( !input
	||   !date_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	moving_statistic_value = moving_statistic_value_calloc();
	moving_statistic_value->input = input;
	moving_statistic_value->date_string = date_string;

	return moving_statistic_value;
}

MOVING_STATISTIC_VALUE *moving_statistic_value_calloc( void )
{
	MOVING_STATISTIC_VALUE *moving_statistic_value;

	if ( ! ( moving_statistic_value =
			calloc( 1,
				sizeof ( MOVING_STATISTIC_VALUE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return moving_statistic_value;
}
