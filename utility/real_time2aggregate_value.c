/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/utility/real_time2aggregate_value.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "julian.h"
#include "piece.h"
#include "column.h"
#include "boolean.h"
#include "timlib.h"
#include "String.h"
#include "statistic.h"
#include "appaserver_error.h"
#include "aggregate_level.h"

#define DEFAULT_DELIMITER	'^'

void output(	char *input_buffer,
		JULIAN *date_julian,
		enum aggregate_level,
		char *aggregate_statistic, 
		int date_piece_offset,
		int time_piece_offset,
		int value_piece_offset,
		char delimiter,
		STATISTIC *statistic,
		boolean append_low_high,
		JULIAN *end_date_julian,
		unsigned long weight_count );

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
	char delimiter;
	char *aggregate_level_string;
	enum aggregate_level aggregate_level;
	boolean append_low_high;
	boolean first_time = 1;
	boolean new_aggregate = 0;
	STATISTIC *statistic;
	char *end_date_string = {0};
	JULIAN *end_date_julian = {0};
	char buffer[ 128 ];

	/* appaserver_error_stderr( argc, argv ); */

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

		julian_set_yyyy_mm_dd(
			end_date_julian,
			column( buffer, 0, end_date_string ) );
	}

	aggregate_level =
		aggregate_level_get(
			aggregate_level_string );

	statistic =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		statistic_new(
			value_piece_offset,
			-1 /* weight_piece */,
			delimiter );

	while( string_input( input_buffer, stdin, 1024 ) )
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

		julian_set_yyyy_mm_dd(
			new_date_julian,
			column( buffer, 0, date_string ) );

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

			if ( ( julian_half_hour_number(
						old_date_julian->
							current ) !=
		     	       julian_half_hour_number(
				     		new_date_julian->
							current ))
			|| ( julian_hour_number( old_date_julian->
							current ) !=
		     	     julian_hour_number( new_date_julian->
							current ))
			|| ( strcmp( date_string,
				     julian_yyyy_mm_dd_string(
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

			if ( ( julian_hour_number( old_date_julian->
							current ) !=
		     	       julian_hour_number( new_date_julian->
							current ))
			|| ( strcmp( date_string,
				     julian_yyyy_mm_dd_string(
					old_date_julian->current ) ) != 0 ) )
			{
				new_aggregate = 1;
			}
		}
		else
		if ( aggregate_level == daily )
		{
			if ( strcmp( date_string,
				     julian_yyyy_mm_dd_string(
						old_date_julian->
							current ) ) != 0 )
			{
				new_aggregate = 1;
			}
		}
		else
		if ( aggregate_level == weekly )
		{
			if ( ( julian_week_number( old_date_julian->
							current ) !=
		     	       julian_week_number( new_date_julian->
							current ))
			||
		    	   ( julian_year_number( old_date_julian->
							current ) != 
		     	     julian_year_number( new_date_julian->
							current )))
			{
				new_aggregate = 1;
			}
		}
		else
		if ( aggregate_level == monthly )
		{
			if ( ( julian_month_number( old_date_julian->
							current ) !=
		     	       julian_month_number( new_date_julian->
							current ))
			||  (  julian_year_number( old_date_julian->
							current ) != 
		     	       julian_year_number( new_date_julian->
							current )))
			{
				new_aggregate = 1;
			}
		}
		else
		if ( aggregate_level == annually )
		{
			if ( julian_year_number( old_date_julian->
							current ) != 
		     	     julian_year_number( new_date_julian->
							current ) )
			{
				new_aggregate = 1;
			}
		}

		if ( new_aggregate )
		{
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
				statistic->statistic_number_array
					/* in/out */ );

			statistic->statistic_quartile =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				statistic_quartile_new(
					statistic->statistic_number_array );

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
					statistic,
					append_low_high,
					end_date_julian,
					statistic->weight_count );
			}

			statistic_free( statistic );

			statistic =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				statistic_new(
					value_piece_offset,
					-1 /* weight_piece */,
					delimiter );

			statistic_accumulate(
				statistic /* in/out */,
				statistic->statistic_number_array /* in/out */,
				input_buffer );

			julian_copy( old_date_julian, new_date_julian );

			new_aggregate = 0;
		}
		else
		{
			statistic_accumulate(
				statistic /* in/out */,
				statistic->statistic_number_array /* in/out */,
				input_buffer );

		}
		strcpy( last_input_buffer, input_buffer );
		first_time = 0;
	}

	if ( !first_time )
	{
		statistic->statistic_calculate =
			statistic_calculate_new(
				statistic->statistic_number_array,
				statistic->weight_count,
				statistic->sum,
				statistic->row_count,
				statistic->value_count,
				statistic->non_zero_count );

		statistic_number_array_sort(
			statistic->statistic_number_array
				/* in/out */ );

		statistic->statistic_quartile =
			statistic_quartile_new(
				statistic->statistic_number_array );

		output(	last_input_buffer,
			old_date_julian,
			aggregate_level,
			aggregate_statistic,
			date_piece_offset,
			time_piece_offset,
			value_piece_offset,
			delimiter,
			statistic,
			append_low_high,
			end_date_julian,
			statistic->weight_count );
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
		STATISTIC *statistic,
		boolean append_low_high,
		JULIAN *end_date_julian,
		unsigned long weight_count )
{
	char results_string[ 128 ];
	char time_string[ 16 ];
	char output_buffer[ 1024 ];

	if ( !statistic
	||   !statistic->statistic_calculate
	||   !statistic->statistic_quartile )
	{
		fprintf(stderr,
			"statistic is empty or incomplete.\n" );
	}

	strcpy( output_buffer, input_buffer );

	if ( weight_count == 0 )
	{
		strcpy( results_string, "null" );
	}
	else
	if ( strcmp( aggregate_statistic, "sum" ) == 0 )
		sprintf(results_string,
			"%.4lf",
			statistic->sum );
	else
	if ( strcmp( aggregate_statistic, "total" ) == 0 )
		sprintf(results_string,
			"%.4lf",
			statistic->sum );
	else
	if ( strcmp( aggregate_statistic, "average" ) == 0 )
		sprintf(results_string,
			"%.4lf",
			statistic->statistic_calculate->average );
	else
	if ( strcmp( aggregate_statistic, "minimum" ) == 0 )
		sprintf(results_string,
			"%.4lf",
			statistic->statistic_quartile->minimum );
	else
	if ( strcmp( aggregate_statistic, "median" ) == 0 )
		sprintf(results_string,
			"%.4lf",
			statistic->statistic_quartile->median );
	else
	if ( strcmp( aggregate_statistic, "maximum" ) == 0 )
		sprintf(results_string,
			"%.4lf",
			statistic->statistic_quartile->maximum );
	else
	if ( strcmp( aggregate_statistic, "range" ) == 0 )
		sprintf(results_string,
			"%.4lf",
			statistic->statistic_quartile->range );
	else
	if ( strcmp( aggregate_statistic, "standard_deviation" ) == 0 )
		sprintf(results_string,
			"%.4lf",
			statistic->statistic_calculate->standard_deviation );
	else
	if ( strcmp( aggregate_statistic, "non_zero_percent" ) == 0 )
	{
		sprintf(results_string,
			"%.1lf",
			statistic->statistic_calculate->non_zero_percent );
	}
	else
	{
		sprintf(results_string,
			"%.4lf",
			statistic->statistic_calculate->average );
	}

	if ( end_date_julian )
	{
		date_julian->current =
			julian_consistent_date_aggregation(
				date_julian->current,
				aggregate_level,
				end_date_julian );
	}

	replace_piece(	output_buffer,
			delimiter, 
			julian_yyyy_mm_dd_string(
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
				julian_hhmm_string(
					date_julian->current ) );
	
			replace_piece(	output_buffer,
					delimiter,
					time_string,
					time_piece_offset );
		}
		else
		if ( aggregate_level == hourly )
		{
			strcpy(	time_string,
				julian_hhmm_string(
					date_julian->current ) );
	
			/* --------------------------------------------- */
			/* Have 10 and 15 minute measurements truncate   */
			/* the minutes.					 */
			/* --------------------------------------------- */
			*(time_string + 2) = '0';
			*(time_string + 3) = '0';

			replace_piece(	output_buffer,
					delimiter,
					time_string,
					time_piece_offset );
		}
		else
		{
			replace_piece(	output_buffer,
					delimiter, 
					"null",
					time_piece_offset );
		}
	}

	replace_piece(	output_buffer,
			delimiter, 
			results_string,
			value_piece_offset );

	printf( "%s%c%ld",
		output_buffer,
		delimiter,
		statistic->value_count );

	if ( append_low_high )
	{
		printf( "%c%.4lf",
			delimiter,
			statistic->statistic_quartile->minimum );
		printf( "%c%.4lf",
			delimiter,
			statistic->statistic_quartile->maximum );
	}

	printf( "\n" );
}

