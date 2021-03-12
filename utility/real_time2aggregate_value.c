/* real_time2aggregate_value.c				*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "julian.h"
#include "piece.h"
#include "timlib.h"
#include "statistics_weighted.h"
#include "aggregate_level.h"

/* Constants */
/* --------- */
#define DEFAULT_DELIMITER	'^'

/* Prototypes */
/* ---------- */
void output(	char *input_buffer,
		JULIAN *date_julian,
		enum aggregate_level,
		char *aggregate_statistic, 
		int date_piece_offset,
		int time_piece_offset,
		int value_piece_offset,
		char delimiter,
		STATISTICS_WEIGHTED *statistics_weighted,
		boolean append_low_high,
		JULIAN *end_date_julian,
		long weighted_count );

int main( int argc, char **argv )
{
	char input_buffer[ 1024 ];
	char last_input_buffer[ 1024 ];
	char *aggregate_statistic;
	char date_string[ 128 ];
	char time_string[ 128 ];
	char value_string[ 128 ];
	JULIAN *new_date_julian;
	JULIAN *old_date_julian;
	int date_piece_offset;
	int time_piece_offset;
	int value_piece_offset;
	int first_time = 1;
	char delimiter;
	char *aggregate_level_string;
	enum aggregate_level aggregate_level;
	boolean append_low_high;
	int new_aggregate = 0;
	STATISTICS_WEIGHTED *statistics_weighted;
	char *end_date_string = {0};
	JULIAN *end_date_julian = {0};

	output_starting_argv_stderr( argc, argv );

	if ( argc < 8 )
	{
		fprintf(stderr,
"Usage: %s aggregate_statistic date_piece_offset time_piece_offset value_piece_offset delimiter aggregate_level append_low_high_yn [end_date]\n",
			argv[ 0 ] );
		exit( 1 );
	}

	aggregate_statistic = argv[ 1 ];
	date_piece_offset = atoi( argv[ 2 ] );
	time_piece_offset = atoi( argv[ 3 ] );
	value_piece_offset = atoi( argv[ 4 ] );
	delimiter = *argv[ 5 ];
	aggregate_level_string = argv[ 6 ];
	append_low_high = ( *argv[ 7 ] == 'y' );

	if ( argc == 9 ) end_date_string = argv[ 8 ];

	if ( !delimiter || delimiter == 'd' )
		delimiter = DEFAULT_DELIMITER;

	new_date_julian = julian_new_julian( 0 );
	old_date_julian = julian_new_julian( 0 );

	if ( end_date_string
	&&   *end_date_string
	&&   strcmp( end_date_string, "end_date" ) != 0 )
	{
		end_date_julian = julian_new_julian( 0 );
		julian_set_yyyy_mm_dd( end_date_julian, end_date_string );
	}

	aggregate_level =
		aggregate_level_get_aggregate_level(
			aggregate_level_string );

	statistics_weighted = statistics_weighted_new_statistics_weighted();

	while( get_line( input_buffer, stdin ) )
	{
		if ( !*input_buffer || *input_buffer == '#' ) continue;

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

		julian_set_yyyy_mm_dd( new_date_julian, date_string );

		if ( first_time )
		{
			julian_copy( old_date_julian, new_date_julian );
		}

		if ( !piece(	value_string, 
				delimiter, 
				input_buffer, 
				value_piece_offset ) )
		{
			fprintf(stderr,
		"%s Warning: cannot piece(%c) = %d value in (%s)\n",
				__FILE__,
				delimiter,
				value_piece_offset,
				input_buffer );
			continue;
		}

		if ( time_piece_offset != -1
		&&   aggregate_level == half_hour )
		{
			if ( !piece(	time_string, 
					delimiter, 
					input_buffer, 
					time_piece_offset ) )
			{
				fprintf(stderr,
			"%s Warning: cannot piece = %d time in (%s)\n",
					__FILE__,
					time_piece_offset,
					input_buffer );
				continue;
			}

			julian_set_time_hhmm( new_date_julian, time_string );

			if ((julian_get_half_hour_number(
						old_date_julian->
							current ) !=
		     	     julian_get_half_hour_number(
				     		new_date_julian->
							current ))
			|| ( julian_get_hour_number( old_date_julian->
							current ) !=
		     	     julian_get_hour_number( new_date_julian->
							current ))
			|| ( strcmp( date_string,
				     julian_get_yyyy_mm_dd_string(
					old_date_julian->current ) ) != 0 ))
			{
				new_aggregate = 1;
			}
		}
		else
		if ( time_piece_offset != -1
		&&   aggregate_level == hourly )
		{
			if ( !piece(	time_string, 
					delimiter, 
					input_buffer, 
					time_piece_offset ) )
			{
				fprintf(stderr,
			"%s Warning: cannot piece = %d time in (%s)\n",
					__FILE__,
					time_piece_offset,
					input_buffer );
				continue;
			}

			julian_set_time_hhmm( new_date_julian, time_string );

			if ((julian_get_hour_number( old_date_julian->
							current ) !=
		     	     julian_get_hour_number( new_date_julian->
							current ))
			|| ( strcmp( date_string,
				     julian_get_yyyy_mm_dd_string(
					old_date_julian->current ) ) != 0 ) )
			{
				new_aggregate = 1;
			}
		}
		else
		if ( aggregate_level == daily )
		{
			if ( strcmp( date_string,
				     julian_get_yyyy_mm_dd_string(
						old_date_julian->
							current ) ) != 0 )
			{
				new_aggregate = 1;
			}
		}
		else
		if ( aggregate_level == weekly )
		{
			if ((julian_get_week_number( old_date_julian->
							current ) !=
		     	     julian_get_week_number( new_date_julian->
							current ))
			||
		    	   (julian_get_year_number( old_date_julian->
							current ) != 
		     	    julian_get_year_number( new_date_julian->
							current )))
			{
				new_aggregate = 1;
			}
		}
		else
		if ( aggregate_level == monthly )
		{
			if ((julian_get_month_number( old_date_julian->
							current ) !=
		     	     julian_get_month_number( new_date_julian->
							current ))
			||  (julian_get_year_number( old_date_julian->
							current ) != 
		     	     julian_get_year_number( new_date_julian->
							current )))
			{
				new_aggregate = 1;
			}
		}
		else
		if ( aggregate_level == annually )
		{
			if ( julian_get_year_number( old_date_julian->
							current ) != 
		     	     julian_get_year_number( new_date_julian->
							current ) )
			{
				new_aggregate = 1;
			}
		}

		if ( new_aggregate )
		{
			statistics_weighted->average =
				statistics_weighted_compute_average(
					statistics_weighted->weighted_count,
					statistics_weighted->weighted_sum );

			statistics_weighted_sort_number_array(
				&statistics_weighted->number_array );

			statistics_weighted->standard_deviation =
				statistics_weighted_compute_standard_deviation(
					&statistics_weighted->
						coefficient_of_variation,
					&statistics_weighted->
						standard_error_of_mean,
					statistics_weighted->average,
					&statistics_weighted->number_array );

			statistics_weighted->percent_missing =
				statistics_weighted_compute_percent_missing(
					statistics_weighted->
						number_array.number_in_array,
					statistics_weighted->
						weighted_input_count,
					statistics_weighted->count );

			statistics_weighted_compute_minimum_median_maxium_range(
					&statistics_weighted->minimum,
					&statistics_weighted->median,
					&statistics_weighted->maximum,
					&statistics_weighted->range,
					&statistics_weighted->number_array );

			if ( !first_time )
			{
				output(	input_buffer,
					old_date_julian,
					aggregate_level,
					aggregate_statistic,
					date_piece_offset,
					time_piece_offset,
					value_piece_offset,
					delimiter,
					statistics_weighted,
					append_low_high,
					end_date_julian,
					statistics_weighted->weighted_count );
			}

			statistics_weighted_free( statistics_weighted );

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
				statistics_weighted->value_piece,
				statistics_weighted->weight_piece,
				delimiter );

			julian_copy( old_date_julian, new_date_julian );

			new_aggregate = 0;
		}
		else
		{
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
				statistics_weighted->value_piece,
				statistics_weighted->weight_piece,
				delimiter );
		}
		strcpy( last_input_buffer, input_buffer );
		first_time = 0;
	}

	if ( !first_time )
	{
		statistics_weighted->average =
			statistics_weighted_compute_average(
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
	
		output(	last_input_buffer,
			old_date_julian,
			aggregate_level,
			aggregate_statistic,
			date_piece_offset,
			time_piece_offset,
			value_piece_offset,
			delimiter,
			statistics_weighted,
			append_low_high,
			end_date_julian,
			statistics_weighted->weighted_count );
	}

	return 0;
}


void output(	char *input_buffer,
		JULIAN *date_julian,
		enum aggregate_level aggregate_level,
		char *aggregate_statistic, 
		int date_piece_offset,
		int time_piece_offset,
		int value_piece_offset,
		char delimiter,
		STATISTICS_WEIGHTED *statistics_weighted,
		boolean append_low_high,
		JULIAN *end_date_julian,
		long weighted_count )
{
	char results_string[ 128 ];
	char time_string[ 16 ];

	if ( weighted_count == 0 )
	{
		strcpy( results_string, "null" );
	}
	else
	if ( strcmp( aggregate_statistic, "sum" ) == 0 )
		sprintf(results_string,
			"%12.4lf",
			statistics_weighted->sum );
	else
	if ( strcmp( aggregate_statistic, "total" ) == 0 )
		sprintf(results_string,
			"%12.4lf",
			statistics_weighted->sum );
	else
	if ( strcmp( aggregate_statistic, "average" ) == 0 )
		sprintf(results_string,
			"%12.4lf",
			statistics_weighted->average );
	else
	if ( strcmp( aggregate_statistic, "minimum" ) == 0 )
		sprintf(results_string,
			"%12.4lf",
			statistics_weighted->minimum );
	else
	if ( strcmp( aggregate_statistic, "median" ) == 0 )
		sprintf(results_string,
			"%12.4lf",
			statistics_weighted->median );
	else
	if ( strcmp( aggregate_statistic, "maximum" ) == 0 )
		sprintf(results_string,
			"%12.4lf",
			statistics_weighted->maximum );
	else
	if ( strcmp( aggregate_statistic, "range" ) == 0 )
		sprintf(results_string,
			"%12.4lf",
			statistics_weighted->range );
	else
	if ( strcmp( aggregate_statistic, "standard_deviation" ) == 0 )
		sprintf(results_string,
			"%12.4lf",
			statistics_weighted->standard_deviation );
	else
	if ( strcmp( aggregate_statistic, "non_zero_percent" ) == 0 )
	{
		if ( statistics_weighted->count )
		{
			sprintf(results_string,
				"%12.4lf",
				(double)statistics_weighted->count_non_zero /
				(double)statistics_weighted->count * 100.0 );
		}
		else
		{
			strcpy( results_string, "0.0000" );
		}
	}
	else
	{
		sprintf(results_string,
			"%12.4lf",
			statistics_weighted->average );
	}

	if ( end_date_julian )
	{
		date_julian->current =
			julian_get_consistent_date_aggregation(
				date_julian->current,
				aggregate_level,
				end_date_julian );
	}

	replace_piece(	input_buffer,
			delimiter, 
			julian_get_yyyy_mm_dd_string(
				date_julian->current ),
			date_piece_offset );

	if ( time_piece_offset != -1 )
	{
		if ( aggregate_level == half_hour )
		{
			date_julian->current =
				julian_round_to_half_hour(
					date_julian->current );

			strcpy(	time_string,
				julian_get_hhmm_string(
					date_julian->current ) );
	
			replace_piece(	input_buffer,
					delimiter,
					time_string,
					time_piece_offset );
		}
		else
		if ( aggregate_level == hourly )
		{
			strcpy(	time_string,
				julian_get_hhmm_string(
					date_julian->current ) );
	
			/* --------------------------------------------- */
			/* Have 10 and 15 minute measurements truncate   */
			/* the minutes.					 */
			/* --------------------------------------------- */
			*(time_string + 2) = '0';
			*(time_string + 3) = '0';

			replace_piece(	input_buffer,
					delimiter,
					time_string,
					time_piece_offset );
		}
		else
		{
			replace_piece(	input_buffer,
					delimiter, 
					"null",
					time_piece_offset );
		}
	}

	replace_piece(	input_buffer,
			delimiter, 
			results_string,
			value_piece_offset );

	printf( "%s%c%ld",
		input_buffer,
		delimiter,
		statistics_weighted->count );

	if ( append_low_high )
	{
		printf( "%c%12.4lf",
			delimiter,
			statistics_weighted->minimum );
		printf( "%c%12.4lf",
			delimiter,
			statistics_weighted->maximum );
	}

	printf( "\n" );
}

