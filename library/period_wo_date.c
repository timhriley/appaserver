/* -------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/period_wo_date.c				*/
/* -------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "period_wo_date.h"
#include "timlib.h"
#include "piece.h"
#include "column.h"

PERIOD_WO_DATE *period_wo_date_new(
			boolean is_weekly,
			enum aggregate_statistic aggregate_statistic,
			char *delta_string,
			int begin_year_int,
			int end_year_int )
{
	PERIOD_WO_DATE *period_wo_date;

	/* Stub */
	/* ---- */
	if ( delta_string ){}

	if ( ! ( period_wo_date =
		(PERIOD_WO_DATE *)
			calloc( 1, sizeof( PERIOD_WO_DATE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	period_wo_date->aggregate_statistic = aggregate_statistic;
	period_wo_date->is_weekly = is_weekly;
	period_wo_date->period_wo_date_delta = delta_none;

	period_wo_date->year_sum_list =
		period_wo_date_get_year_sum_list(
			begin_year_int,
			end_year_int );

	/* Note: delta is not implemented. delta_values.c needs work. */
	/* ---------------------------------------------------------- */
#ifdef NOT_DEFINED
	if ( delta_string && *delta_string )
	{
		if ( strcmp( delta_string, "first" ) == 0 )
			period_wo_date->period_wo_date_delta = delta_first;
		else
		if ( strcmp( delta_string, "previous" ) == 0 )
			period_wo_date->period_wo_date_delta = delta_previous;
		else
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: invalid delta_string = (%s)\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				delta_string );
			exit( 1 );
		}
	}
	else
	{
		period_wo_date->period_wo_date_delta = delta_none;
	}
#endif

	return period_wo_date;
}

PERIOD_WO_DATE_PERIOD *period_wo_date_period_new(
			char *period_label )
{
	PERIOD_WO_DATE_PERIOD *period_wo_date_period;

	if ( ! ( period_wo_date_period =
		(PERIOD_WO_DATE_PERIOD *)
			calloc( 1, sizeof( PERIOD_WO_DATE_PERIOD ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	period_wo_date_period->period_label = period_label;
	return period_wo_date_period;
}

PERIOD_WO_DATE_PERIOD **period_wo_date_get_monthly_period_array(
			char *begin_date_string,
			char *end_date_string )
{
	static PERIOD_WO_DATE_PERIOD *period_array[ 13 ] = {0};
	int begin_year_int;
	int end_year_int;
	char begin_year_string[ 8 ];
	char end_year_string[ 8 ];
	PERIOD_WO_DATE_PERIOD *period_wo_date_period;
	int array_offset = 0;
	int month_int;
	char month_string[ 4 ];

	begin_year_int =
		atoi( piece(begin_year_string, '-', begin_date_string, 0 ) );

	end_year_int =
		atoi( piece( end_year_string, '-', end_date_string, 0 ) );

	for( month_int = 1; month_int <= 12; month_int++ )
	{
		sprintf(	month_string,
				"%s",
				timlib_get_three_character_month_string(
					month_int - 1 ) );

		period_wo_date_period =
			period_wo_date_period_new( strdup( month_string ) );

		period_wo_date_period->year_value_list =
			period_wo_date_get_year_value_list(
				begin_year_int,
				end_year_int );

		period_array[ array_offset++ ] = period_wo_date_period;
	}

	return period_array;
}

PERIOD_WO_DATE_PERIOD **period_wo_date_get_weekly_period_array(
			char *begin_date_string,
			char *end_date_string )
{
	static PERIOD_WO_DATE_PERIOD *period_array[ 53 ] = {0};
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 128 ];
	char date_string[ 16 ];
	char week_string[ 8 ];
	char begin_year_string[ 8 ];
	char end_year_string[ 8 ];
	int begin_year_int;
	int end_year_int;
	char representative_week_date_string[ 256 ];
	PERIOD_WO_DATE_PERIOD *period_wo_date_period;
	int array_offset = 0;

	begin_year_int =
		atoi( piece(begin_year_string, '-', begin_date_string, 0 ) );

	end_year_int =
		atoi( piece( end_year_string, '-', end_date_string, 0 ) );

	strcpy(	sys_string,
		"each_week.e 2000 1 | sed 's/ /-/'" );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		/* Expect like 12-2 49 */
		/* ------------------- */
		column( date_string, 0, input_buffer );
		column( week_string, 1, input_buffer );

		sprintf(	representative_week_date_string,
				"%d-%s",
				begin_year_int,
				date_string );

		period_wo_date_period =
			period_wo_date_period_new(
				strdup( week_string ) );

		period_wo_date_period->representative_week_date_string =
			strdup( representative_week_date_string );

		period_wo_date_period->year_value_list =
			period_wo_date_get_year_value_list(
				begin_year_int,
				end_year_int );

		period_array[ array_offset++ ] = period_wo_date_period;
	}

	pclose( input_pipe );
	return period_array;
}

LIST *period_wo_date_get_year_value_list(
				int begin_year_int,
				int end_year_int )
{
	PERIOD_WO_DATE_YEAR_VALUE *period_wo_date_year_value;
	int year_int;
	LIST *year_value_list;

	year_value_list = list_new();
	for( year_int = begin_year_int; year_int <= end_year_int; year_int++ )
	{
		period_wo_date_year_value =
			period_wo_date_year_value_new(
				year_int );

		list_append_pointer(
			year_value_list,
			period_wo_date_year_value );
	}
	return year_value_list;
}

LIST *period_wo_date_get_year_sum_list(
				int begin_year_int,
				int end_year_int )
{
	PERIOD_WO_DATE_YEAR_SUM *period_wo_date_year_sum;
	int year_int;
	LIST *year_sum_list;

	year_sum_list = list_new();
	for( year_int = begin_year_int; year_int <= end_year_int; year_int++ )
	{
		period_wo_date_year_sum =
			period_wo_date_year_sum_new(
				year_int );

		list_append_pointer(
			year_sum_list,
			period_wo_date_year_sum );
	}
	return year_sum_list;
}

void period_wo_date_get_minimum_average_maximum(
				double *minimum,
				double *average,
				double *maximum,
				LIST *year_value_list )
{
	PERIOD_WO_DATE_YEAR_VALUE *period_wo_date_year_value;
	double local_minimum = DOUBLE_MAXIMUM;
	double local_maximum = DOUBLE_MINIMUM;
	int count = 0;
	double sum = 0.0;

	if ( !list_rewind( year_value_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty year_value_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		period_wo_date_year_value = list_get_pointer( year_value_list );

		if ( !period_wo_date_year_value->is_null )
		{
			sum += period_wo_date_year_value->value;
	
			if ( period_wo_date_year_value->value < local_minimum )
			{
				local_minimum =
					period_wo_date_year_value->value;
			}
	
			if ( period_wo_date_year_value->value > local_maximum )
			{
				local_maximum =
					period_wo_date_year_value->value;
			}
			count++;
		}

	} while( list_next( year_value_list ) );

	if ( count )
	{
		*minimum = local_minimum;
		*average = sum / (double)count;
		*maximum = local_maximum;
	}
	else
	{
		*minimum = 0.0;
		*average = 0.0;
		*maximum = 0.0;
	}
}
 
void period_wo_date_populate_minimum_average_maximum(
		double *sum_minimum,
		double *sum_average,
		double *sum_maximum,
		PERIOD_WO_DATE_PERIOD **period_array )
{
	PERIOD_WO_DATE_PERIOD *period_wo_date_period;

	while( ( period_wo_date_period = *period_array++ ) )
	{
		period_wo_date_get_minimum_average_maximum(
				&period_wo_date_period->minimum,
				&period_wo_date_period->average,
				&period_wo_date_period->maximum,
				period_wo_date_period->year_value_list );

		*sum_minimum += period_wo_date_period->minimum;
		*sum_average += period_wo_date_period->average;
		*sum_maximum += period_wo_date_period->maximum;
	}
}

char *period_wo_date_get_delta_string(
		enum period_wo_date_delta period_wo_date_delta )
{
	if ( period_wo_date_delta == delta_none )
		return "";
	else
	if ( period_wo_date_delta == delta_previous )
		return "previous";
	else
	if ( period_wo_date_delta == delta_first )
		return "first";
	else
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: invalid period_wo_date_delta = (%d)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			period_wo_date_delta );
		exit( 1 );
	}
}

PERIOD_WO_DATE_YEAR_SUM *period_wo_date_year_sum_new(
				int year )
{
	PERIOD_WO_DATE_YEAR_SUM *year_sum;

	year_sum = calloc( 1, sizeof( PERIOD_WO_DATE_YEAR_SUM ) );

	if ( !year_sum )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	year_sum->year = year;

	return year_sum;
}

PERIOD_WO_DATE_YEAR_VALUE *period_wo_date_year_value_new(
				int year )
{
	PERIOD_WO_DATE_YEAR_VALUE *year_value;

	year_value = calloc( 1, sizeof( PERIOD_WO_DATE_YEAR_VALUE ) );

	if ( !year_value )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: memory allocation error.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	year_value->year = year;
	year_value->is_null = 1;

	return year_value;
}

PERIOD_WO_DATE_YEAR_VALUE *period_wo_date_seek_period_wo_date_year_value(
				LIST *year_value_list,
				int year_int )
{
	PERIOD_WO_DATE_YEAR_VALUE *period_wo_date_year_value;

	if ( !list_rewind( year_value_list ) )
		return (PERIOD_WO_DATE_YEAR_VALUE *)0;

	do {
		period_wo_date_year_value =
			list_get_pointer(
				year_value_list );

		if ( period_wo_date_year_value->year == year_int )
			return period_wo_date_year_value;

	} while( list_next( year_value_list ) );

	return (PERIOD_WO_DATE_YEAR_VALUE *)0;
}

PERIOD_WO_DATE_YEAR_SUM *period_wo_date_seek_period_wo_date_year_sum(
				LIST *year_sum_list,
				int year_int )
{
	PERIOD_WO_DATE_YEAR_SUM *period_wo_date_year_sum;


	if ( !list_rewind( year_sum_list ) )
		return (PERIOD_WO_DATE_YEAR_SUM *)0;

	do {
		period_wo_date_year_sum =
			list_get_pointer(
				year_sum_list );

		if ( period_wo_date_year_sum->year == year_int )
			return period_wo_date_year_sum;

	} while( list_next( year_sum_list ) );

	return (PERIOD_WO_DATE_YEAR_SUM *)0;
}

char *period_wo_date_get_aggregation_average_value(
				double scalar_value,
				boolean is_weekly )
{
	char return_value[ 16 ];
	int denominator_value;

	if ( is_weekly )
		denominator_value = 52;
	else
		denominator_value = 12;

	if ( !scalar_value )
	{
		return strdup( "null" );
	}
	else
	{
		sprintf(	return_value,
				"%.2lf",
				scalar_value / denominator_value );
		return strdup( return_value );
	}
}

char *period_wo_date_get_aggregation_value(
				double scalar_value,
				enum aggregate_statistic aggregate_statistic,
				boolean is_weekly,
				int null_count )
{
	char return_value[ 16 ];

	if ( aggregate_statistic == sum )
	{
		sprintf( return_value, "%.2lf", scalar_value );
		return strdup( return_value );
	}
	else
	{
		int denominator_value;

		if ( is_weekly )
		{
			denominator_value = 52 - null_count;
		}
		else
		{
			denominator_value = 12 - null_count;
		}

		if ( !denominator_value )
		{
			return strdup( "null" );
		}
		else
		{
			sprintf(	return_value,
					"%.2lf",
					scalar_value / denominator_value );
			return strdup( return_value );
		}
	}
}

void period_wo_date_populate_year_sum_list_null_count(
		LIST *year_sum_list,
		PERIOD_WO_DATE_PERIOD **period_array )
{
	PERIOD_WO_DATE_PERIOD *period_wo_date_period;
	PERIOD_WO_DATE_YEAR_VALUE *period_wo_date_year_value;
	PERIOD_WO_DATE_YEAR_SUM *period_wo_date_year_sum;

	while( ( period_wo_date_period = *period_array++ ) )
	{
		if ( !list_rewind( period_wo_date_period->year_value_list ) )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty year_value_list\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		do {
			period_wo_date_year_value =
				list_get_pointer(
					period_wo_date_period->
						year_value_list );

			if ( period_wo_date_year_value->is_null )
			{

				if ( ! ( period_wo_date_year_sum =
				period_wo_date_seek_period_wo_date_year_sum(
					year_sum_list,
					period_wo_date_year_value->year ) ) )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot get period_wo_date_year_sum.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

				period_wo_date_year_sum->null_count++;
			}

		} while( list_next( period_wo_date_period->
						year_value_list ) );
	}
}

