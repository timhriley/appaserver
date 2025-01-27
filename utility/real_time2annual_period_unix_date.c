/* real_time2annual_period.c				*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "date.h"
#include "piece.h"
#include "timlib.h"
#include "statistics_weighted.h"

/* Constants */
/* --------- */
#define BEGINNING_SPARROW_SEASON	"03-01"
#define ENDING_SPARROW_SEASON		"07-15"
#define BEGINNING_ENP_WET_SEASON	"06-01"
#define ENDING_ENP_WET_SEASON		"10-31"
#define BEGINNING_ENP_DRY_SEASON	"11-01"
#define ENDING_ENP_DRY_SEASON		"05-31"
#define BEGINNING_SUMMER_SEASON		"06-21"
#define ENDING_SUMMER_SEASON		"09-22"
#define BEGINNING_FALL_SEASON		"09-23"
#define ENDING_FALL_SEASON		"12-21"
#define BEGINNING_WINTER_SEASON		"12-22"
#define ENDING_WINTER_SEASON		"03-20"
#define BEGINNING_SPRING_SEASON		"03-21"
#define ENDING_SPRING_SEASON		"06-20"

enum season_match_results {	different_season,
				same_season_different_year,
				same_season_same_year};

/* Prototypes */
/* ---------- */
boolean is_same_year(		DATE *new_date,
				DATE *old_date );

enum season_match_results get_season_match_resuls(
				DATE *new_date,
				DATE *old_date,
				char *season_string );

void compute_statistics_and_output(
				DATE *date,
				char *aggregate_statistic,
				char delimiter,
				STATISTICS_WEIGHTED *statistics_weighted );

void output(
				DATE *date,
				char *aggregate_statistic, 
				char delimiter,
				STATISTICS_WEIGHTED *statistics_weighted );

int get_month_integer(		char *month_string );

int get_week_integer(		char *week_containing_date_string );

int real_time2annual_annually_period(
				char *input_buffer,
				char *aggregate_statistic,
				DATE *old_date,
				DATE *new_date,
				char *value_string,
				char delimiter,
				boolean output_only );

int real_time2annual_seasonally_period(
				char *input_buffer,
				char *aggregate_statistic,
				DATE *old_date,
				DATE *new_date,
				char *value_string,
				char *period,
				char delimiter,
				boolean output_only );

int real_time2annual_weekly_period(
				char *input_buffer,
				char *aggregate_statistic,
				DATE *old_date,
				DATE *new_date,
				char *value_string,
				int period_week,
				char delimiter,
				boolean output_only );

int real_time2annual_monthly_period(
				char *input_buffer,
				char *aggregate_statistic,
				DATE *old_date,
				DATE *new_date,
				char *value_string,
				char *period,
				char delimiter,
				boolean output_only );

int main( int argc, char **argv )
{
	char *aggregate_statistic;
	int date_piece_offset;
	int value_piece_offset;
	char delimiter;
	char *aggregate_period;
	char *period;
	char input_buffer[ 1024 ];
	char date_string[ 128 ];
	char value_string[ 128 ];
	DATE *new_date;
	DATE *old_date;
	int first_time = 1;
	int period_week = 0;

	output_starting_argv_stderr( argc, argv );

	if ( argc != 7 )
	{
		fprintf(stderr,
"Usage: %s aggregate_statistic date_piece_offset value_piece_offset delimiter aggregate_period period\n",
			argv[ 0 ] );
		exit( 1 );
	}

	aggregate_statistic = argv[ 1 ];
	date_piece_offset = atoi( argv[ 2 ] );
	value_piece_offset = atoi( argv[ 3 ] );
	delimiter = *argv[ 4 ];
	aggregate_period = argv[ 5 ];
	period = argv[ 6 ];

	new_date = date_calloc();
	old_date = date_calloc();

	if ( strcmp( aggregate_period, "weekly" ) == 0 )
	{
		if ( ( period_week = get_week_integer( period ) ) == -1 )
		{
			fprintf(stderr,
			"ERROR in %s/%s(): cannot get week integer for %s\n",
				__FILE__,
				__FUNCTION__,
				period );
			return 0;
		}
	}

	while( get_line( input_buffer, stdin ) )
	{

		if ( !piece(	date_string, 
				delimiter, 
				input_buffer, 
				date_piece_offset ) )
		{
			fprintf(stderr,
"%s Warning: cannot extract date using delimiter = (%c) in (%s)\n",
				__FILE__,
				delimiter,
				input_buffer );
			continue;
		}

		if ( !piece(	value_string, 
				delimiter, 
				input_buffer, 
				value_piece_offset ) )
		{
			fprintf(stderr,
"%s Warning: cannot extract value using delimiter = (%c) in (%s)\n",
				__FILE__,
				delimiter,
				input_buffer );
			continue;
		}

		if ( !*value_string ) continue;

		date_set_yyyy_mm_dd( new_date, date_string );

		if ( first_time )
		{
			date_copy( old_date, new_date );
			first_time = 0;
		}

		if ( strcmp( aggregate_period, "annually" ) == 0 )
		{
			if ( !real_time2annual_annually_period(
					input_buffer,
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					delimiter,
					0 /* not output_only */ ) )
			{
				exit( 1 );
			}
		} /* if annually */
		else
		if ( strcmp( aggregate_period, "seasonally" ) == 0 )
		{
			if ( !real_time2annual_seasonally_period(
					input_buffer,
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					period,
					delimiter,
					0 /* not output_only */ ) )
			{
				exit( 1 );
			}
		} /* if seasonally */
		else
		if ( strcmp( aggregate_period, "monthly" ) == 0 )
		{
			if ( !real_time2annual_monthly_period(
					input_buffer,
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					period,
					delimiter,
					0 /* not output_only */ ) )
			{
				exit( 1 );
			}
		} /* if monthly */
		else
		if ( strcmp( aggregate_period, "weekly" ) == 0 )
		{
			if ( !real_time2annual_weekly_period(
					input_buffer,
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					period_week,
					delimiter,
					0 /* not output_only */ ) )
			{
				exit( 1 );
			}
		} /* if monthly */
		date_copy( old_date, new_date );
	} /* while() */

	if ( strcmp( aggregate_period, "annually" ) == 0 )
	{
		real_time2annual_annually_period(
					input_buffer,
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					delimiter,
					1 /* output_only */ );
	}
	else
	if ( strcmp( aggregate_period, "seasonally" ) == 0 )
	{
		real_time2annual_seasonally_period(
					input_buffer,
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					period,
					delimiter,
					1 /* output_only */ );
	}
	else
	if ( strcmp( aggregate_period, "monthly" ) == 0 )
	{
		real_time2annual_monthly_period(
					input_buffer,
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					period,
					delimiter,
					1 /* output_only */ );
	}
	else
	if ( strcmp( aggregate_period, "weekly" ) == 0 )
	{
		real_time2annual_weekly_period(
					input_buffer,
					aggregate_statistic,
					old_date,
					new_date,
					value_string,
					period_week,
					delimiter,
					1 /* output_only */ );
	}

	return 0;
}

int get_month_integer( char *month_string )
{
	char *month_string_array[] =	{	"january",
						"february",
						"march",
						"april",
						"may",
						"june",
						"july",
						"august",
						"september",
						"october",
						"november",
						"december",
						(char *)0 };
	char **compare_month_pointer = month_string_array;
	int month_integer = 1;

	while( **compare_month_pointer )
	{
		if ( strcasecmp( month_string, *compare_month_pointer ) == 0 )
			return month_integer;
		compare_month_pointer++;
		month_integer++;
	}
	return 0;
} /* get_month_integer() */

int get_week_integer( char *week_containing_date_string )
{
	DATE *date;
	int week_integer;

	date = date_new_yyyy_mm_dd_date( week_containing_date_string );

	if ( !date ) return -1;

	week_integer = date_get_week_number( date );
	date_free( date );
	return week_integer;
} /* get_week_integer() */

void output(		DATE *date,
			char *aggregate_statistic, 
			char delimiter,
			STATISTICS_WEIGHTED *statistics_weighted )
{
	char results_string[ 128 ];

	if ( get_year( date ) == 1900 ) return;

	if ( statistics_weighted->count == 0 )
	{
		strcpy( results_string, "null" );
	}
	else
	if ( strcmp( aggregate_statistic, "sum" ) == 0 )
		sprintf(results_string,
			"%10.2lf",
			statistics_weighted->sum );
	else
	if ( strcmp( aggregate_statistic, "total" ) == 0 )
		sprintf(results_string,
			"%10.2lf",
			statistics_weighted->sum );
	else
	if ( strcmp( aggregate_statistic, "average" ) == 0 )
		sprintf(results_string,
			"%10.2lf",
			statistics_weighted->average );
	else
	if ( strcmp( aggregate_statistic, "minimum" ) == 0 )
		sprintf(results_string,
			"%10.2lf",
			statistics_weighted->minimum );
	else
	if ( strcmp( aggregate_statistic, "median" ) == 0 )
		sprintf(results_string,
			"%10.2lf",
			statistics_weighted->median );
	else
	if ( strcmp( aggregate_statistic, "maximum" ) == 0 )
		sprintf(results_string,
			"%10.2lf",
			statistics_weighted->maximum );
	else
	if ( strcmp( aggregate_statistic, "range" ) == 0 )
		sprintf(results_string,
			"%10.2lf",
			statistics_weighted->range );
	else
	if ( strcmp( aggregate_statistic, "standard_deviation" ) == 0 )
		sprintf(results_string,
			"%10.2lf",
			statistics_weighted->standard_deviation );
	else
		strcpy( results_string, "AGGREGATE?" );

	printf( "%s%c%s%c%d\n",
		date_get_yyyy_mm_dd_string( date ),
		delimiter,
		results_string,
		delimiter,
		statistics_weighted->count );

} /* output() */

int real_time2annual_weekly_period(
				char *input_buffer,
				char *aggregate_statistic,
				DATE *old_date,
				DATE *new_date,
				char *value_string,
				int period_week,
				char delimiter,
				boolean output_only )
{
	static STATISTICS_WEIGHTED *statistics_weighted = {0};
	int new_year;
	int new_week;
	int old_year;

	if ( output_only )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
		}

		return 1;
	}

	old_year = date_get_year_number( old_date );
	new_year = date_get_year_number( new_date );
	new_week = date_get_week_number( new_date );

	if ( period_week != new_week )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
			statistics_weighted_free( statistics_weighted );
			statistics_weighted = (STATISTICS_WEIGHTED *)0;
		}
		return 1;
	}
	else
	if ( old_year != new_year )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
			statistics_weighted_free( statistics_weighted );
			statistics_weighted = (STATISTICS_WEIGHTED *)0;
		}

		statistics_weighted =
			statistics_weighted_new_statistics_weighted();

		statistics_weighted->buffer = value_string;

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
			-1 /* statistics_weighted->value_piece */,
			-1 /* statistics_weighted->weight_piece */,
			(char)0 /* delimiter */ );
		return 1;
	}

	if ( !statistics_weighted )
	{
		statistics_weighted =
			statistics_weighted_new_statistics_weighted();
	}

	statistics_weighted->buffer = value_string;

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
		-1 /* statistics_weighted->value_piece */,
		-1 /* statistics_weighted->weight_piece */,
		(char)0 /* delimiter */ );
	return 1;
} /* real_time2annual_weekly_period() */

int real_time2annual_monthly_period(
				char *input_buffer,
				char *aggregate_statistic,
				DATE *old_date,
				DATE *new_date,
				char *value_string,
				char *period,
				char delimiter,
				boolean output_only )
{
	int period_month;
	static STATISTICS_WEIGHTED *statistics_weighted = {0};
	int new_year;
	int new_month;
	int old_year;

	if ( output_only )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
		}

		return 1;
	}

	old_year = date_get_year_number( old_date );
	new_year = date_get_year_number( new_date );
	new_month = date_get_month_number( new_date );

	if ( ! ( period_month = get_month_integer( period ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s(): cannot get month integer for %s\n",
			__FILE__,
			__FUNCTION__,
			period );
		return 0;
	}

	if ( period_month != new_month )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
			statistics_weighted_free( statistics_weighted );
			statistics_weighted = (STATISTICS_WEIGHTED *)0;
		}
		return 1;
	}
	else
	if ( old_year != new_year )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
			statistics_weighted_free( statistics_weighted );
			statistics_weighted = (STATISTICS_WEIGHTED *)0;
		}

		statistics_weighted =
			statistics_weighted_new_statistics_weighted();

		statistics_weighted->buffer = value_string;

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
			-1 /* statistics_weighted->value_piece */,
			-1 /* statistics_weighted->weight_piece */,
			(char)0 /* delimiter */ );
		return 1;
	}

	if ( !statistics_weighted )
	{
		statistics_weighted =
			statistics_weighted_new_statistics_weighted();
	}

	statistics_weighted->buffer = value_string;

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
		-1 /* statistics_weighted->value_piece */,
		-1 /* statistics_weighted->weight_piece */,
		(char)0 /* delimiter */ );
	return 1;
} /* real_time2annual_monthly_period() */

void compute_statistics_and_output(
				DATE *date,
				char *aggregate_statistic,
				char delimiter,
				STATISTICS_WEIGHTED *statistics_weighted )
{
	statistics_weighted_compute_average(
			&statistics_weighted->average,
			statistics_weighted->weighted_count,
			statistics_weighted->weighted_sum );

	statistics_weighted_sort_number_array(
			&statistics_weighted->number_array );

	statistics_weighted->standard_deviation =
			statistics_weighted_compute_standard_deviation(
				&statistics_weighted->coefficient_of_variation,
				&statistics_weighted->standard_error_of_mean,
				statistics_weighted->average,
				&statistics_weighted->number_array );

	statistics_weighted->percent_missing =
			statistics_weighted_compute_percent_missing(
				statistics_weighted->
					number_array.number_in_array,
				statistics_weighted->weighted_input_count,
				statistics_weighted->count );

	statistics_weighted_compute_minimum_median_maxium_range(
				&statistics_weighted->minimum,
				&statistics_weighted->median,
				&statistics_weighted->maximum,
				&statistics_weighted->range,
				&statistics_weighted->number_array );

	output(		date, 
			aggregate_statistic,
			delimiter,
			statistics_weighted );

} /* compute_statistics_and_output() */

int real_time2annual_annually_period(
				char *input_buffer,
				char *aggregate_statistic,
				DATE *old_date,
				DATE *new_date,
				char *value_string,
				char delimiter,
				boolean output_only )
{
	static STATISTICS_WEIGHTED *statistics_weighted = {0};

	if ( output_only )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
		}

		return 1;
	}

	if ( !is_same_year( new_date, old_date ) )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
			statistics_weighted_free( statistics_weighted );
			statistics_weighted = (STATISTICS_WEIGHTED *)0;
		}

		statistics_weighted =
			statistics_weighted_new_statistics_weighted();

		statistics_weighted->buffer = value_string;

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
			-1 /* statistics_weighted->value_piece */,
			-1 /* statistics_weighted->weight_piece */,
			(char)0 /* delimiter */ );
	}
	else
	/* ----------------- */
	/* Must be same year */
	/* ----------------- */
	{
		if ( !statistics_weighted )
		{
			statistics_weighted =
				statistics_weighted_new_statistics_weighted();
		}
	
		statistics_weighted->buffer = value_string;
	
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
			-1 /* statistics_weighted->value_piece */,
			-1 /* statistics_weighted->weight_piece */,
			(char)0 /* delimiter */ );
	}
	return 1;
} /* real_time2annual_annually_period() */

int real_time2annual_seasonally_period(
				char *input_buffer,
				char *aggregate_statistic,
				DATE *old_date,
				DATE *new_date,
				char *value_string,
				char *period,
				char delimiter,
				boolean output_only )
{
	static STATISTICS_WEIGHTED *statistics_weighted = {0};
	enum season_match_results season_match_results;

	if ( output_only )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
		}

		return 1;
	}

	season_match_results = get_season_match_resuls(
				new_date,
				old_date,
				period );

	if ( season_match_results == different_season )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
			statistics_weighted_free( statistics_weighted );
			statistics_weighted = (STATISTICS_WEIGHTED *)0;
		}
		return 1;
	}

	if ( season_match_results == same_season_different_year )
	{
		if ( statistics_weighted )
		{
			compute_statistics_and_output(
					old_date, 
					aggregate_statistic,
					delimiter,
					statistics_weighted );
			statistics_weighted_free( statistics_weighted );
			statistics_weighted = (STATISTICS_WEIGHTED *)0;
		}

		statistics_weighted =
			statistics_weighted_new_statistics_weighted();

		statistics_weighted->buffer = value_string;

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
			-1 /* statistics_weighted->value_piece */,
			-1 /* statistics_weighted->weight_piece */,
			(char)0 /* delimiter */ );
		return 1;
	}

	/* Must be same_season_same_year */
	/* ----------------------------- */
	if ( !statistics_weighted )
	{
		statistics_weighted =
			statistics_weighted_new_statistics_weighted();
	}

	statistics_weighted->buffer = value_string;

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
		-1 /* statistics_weighted->value_piece */,
		-1 /* statistics_weighted->weight_piece */,
		(char)0 /* delimiter */ );
	return 1;
} /* real_time2annual_seasonally_period() */

enum season_match_results get_season_match_resuls(
				DATE *new_date,
				DATE *old_date,
				char *season_string )
{
	char *date_string;
	char *trimmed_date_string;
	int old_year = date_get_year_number( old_date );
	int new_year = date_get_year_number( new_date );

	date_string = date_get_yyyy_mm_dd_string( new_date );
	trimmed_date_string = date_string + 5;

	if ( strcmp( season_string, "ENP_wet" ) == 0 )
	{
		if ( strcmp(	trimmed_date_string,
				BEGINNING_ENP_WET_SEASON ) >= 0
		&&   strcmp(	trimmed_date_string,
				ENDING_ENP_WET_SEASON ) <= 0 )
		{
			return same_season_same_year;
		}
		else
		{
			return different_season;
		}
	}
	if ( strcmp( season_string, "ENP_dry" ) == 0 )
	{
		if ( strcmp(	trimmed_date_string,
				BEGINNING_ENP_DRY_SEASON ) >= 0
		&&     strcmp( trimmed_date_string, "12-31" ) <= 0 )
		{
			if ( old_year == new_year )
			{
				return same_season_same_year;
			}
			else
			{
				return same_season_different_year;
			}
			
		}

		if ( strcmp( trimmed_date_string, ENDING_ENP_DRY_SEASON ) <= 0
		&&   strcmp( trimmed_date_string, "01-01" ) >= 0 )
		{
			if ( old_year == new_year
			||   old_year == new_year - 1 )
			{
				return same_season_same_year;
			}
			else
			{
				return same_season_different_year;
			}
		}
		return different_season;
	}
	if ( strcmp( season_string, "ENP_sparrow" ) == 0 )
	{
		if ( strcmp(	trimmed_date_string,
				BEGINNING_SPARROW_SEASON ) >= 0
		&&   strcmp(	trimmed_date_string,
				ENDING_SPARROW_SEASON ) <= 0 )
		{
			return same_season_same_year;
		}
		else
		{
			return different_season;
		}
	}
	if ( strcmp( season_string, "summer" ) == 0 )
	{
		if ( strcmp(	trimmed_date_string,
				BEGINNING_SUMMER_SEASON ) >= 0
		&&   strcmp(	trimmed_date_string,
				ENDING_SUMMER_SEASON ) <= 0 )
		{
			return same_season_same_year;
		}
		else
		{
			return different_season;
		}
	}
	if ( strcmp( season_string, "fall" ) == 0 )
	{
		if ( strcmp(	trimmed_date_string,
				BEGINNING_FALL_SEASON ) >= 0
		&&   strcmp(	trimmed_date_string,
				ENDING_FALL_SEASON ) <= 0 )
		{
			return same_season_same_year;
		}
		else
		{
			return different_season;
		}
	}
	if ( strcmp( season_string, "spring" ) == 0 )
	{
		if ( strcmp(	trimmed_date_string,
				BEGINNING_SPRING_SEASON ) >= 0
		&&   strcmp(	trimmed_date_string,
				ENDING_SPRING_SEASON ) <= 0 )
		{
			return same_season_same_year;
		}
		else
		{
			return different_season;
		}
	}
	if ( strcmp( season_string, "winter" ) == 0 )
	{
		if ( strcmp(	trimmed_date_string,
				BEGINNING_WINTER_SEASON ) >= 0
		&&     strcmp( trimmed_date_string, "12-31" ) <= 0 )
		{
			if ( old_year == new_year )
			{
				return same_season_same_year;
			}
			else
			{
				return same_season_different_year;
			}
			
		}

		if ( strcmp( trimmed_date_string, ENDING_WINTER_SEASON ) <= 0
		&&   strcmp( trimmed_date_string, "01-01" ) >= 0 )
		{
			if ( old_year == new_year
			||   old_year == new_year - 1 )
			{
				return same_season_same_year;
			}
			else
			{
				return same_season_different_year;
			}
		}
		return different_season;
	}

	return different_season;

} /* get_season_match_resuls() */

boolean is_same_year( DATE *new_date, DATE *old_date )
{
	int old_year = date_get_year_number( old_date );
	int new_year = date_get_year_number( new_date );

	return ( old_year == new_year );
} /* is_same_year() */

