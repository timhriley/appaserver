/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/utility/time_ticker.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "expected_count.h"
#include "julian.h"
#include "date.h"
#include "aggregate_level.h"

#define DAYS_PER_HOUR		(1.0 / 24.0)
#define DAYS_PER_HALF_HOUR	(1.0 / 48.0)

char *display_julian(
		enum aggregate_level aggregate_level,
		double current,
		char delimiter,
		EXPECTED_COUNT *expected_count );

void output_weekly(
		char delimiter,
		char *begin_date_string,
		char *end_date_string );

void output_monthly(
		char delimiter,
		char *begin_date_string,
		char *end_date_string );

void output_annually(
		char delimiter,
		char *begin_date_string,
		char *end_date_string );

void output_real_time_halfhour_hourly_daily(
		char delimiter,
		enum aggregate_level aggregate_level,
		char *begin_date_string,
		char *begin_time_string,
		char *end_date_string,
		char *end_time_string,
		char *expected_count_list_string,
		char *argv_0 );

time_t get_expected_x(
		LIST *expected_count_list,
		enum aggregate_level,
		time_t current );

double get_julian_next_expected_x(
		EXPECTED_COUNT *expected_count,
		enum aggregate_level,
		double current );

int parse_date_time(
		char *month,
		char *day,
		char *year,
		char *hour,
		char *minute,
		char *date_string,
		char *time_string );

int main( int argc, char **argv )
{
	char delimiter;
	enum aggregate_level aggregate_level;
	char begin_date_string[ 16 ];
	char begin_time_string[ 16 ] = {0};
	char end_date_string[ 16 ];
	char end_time_string[ 16 ] = {0};
	char *expected_count_list_string;

	/* ------------------------------------ */
	/* appaserver_error_argv( argc, argv );	*/
	/* ------------------------------------ */

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s delimiter aggregate_level begin_date[:begin_time] end_date[:end_time] expected_count_list_string\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];
	aggregate_level = aggregate_level_get( argv[ 2 ] );

	if ( character_exists( argv[ 3 ], ':' ) )
	{
		piece( begin_date_string, ':', argv[ 3 ], 0 );
		piece( begin_time_string, ':', argv[ 3 ], 1 );
	}
	else
	{
		strcpy( begin_date_string, argv[ 3 ] );
	}

	if ( strcmp( begin_date_string, "begin_date" ) == 0 )
		strcpy( begin_date_string, "1901-01-01" );

	if ( !*begin_time_string )
		strcpy( begin_time_string, "0000" );

	if ( character_exists( argv[ 4 ], ':' ) )
	{
		piece( end_date_string, ':', argv[ 4 ], 0 );
		piece( end_time_string, ':', argv[ 4 ], 1 );
	}
	else
	{
		strcpy( end_date_string, argv[ 4 ] );
	}

	if ( strcmp( end_date_string, "end_date" ) == 0 )
		strcpy( end_date_string,
			date_now_yyyy_mm_dd(
				date_utc_offset() ) );

	if ( !*end_time_string )
		strcpy( end_time_string, "2359" );

	expected_count_list_string = argv[ 5 ];

	if ( !*expected_count_list_string )
	{
		expected_count_list_string = "0000-00-00^0000^24";
	}

	/* If only the count per day. */
	/* -------------------------- */
	if ( !character_exists( expected_count_list_string, '^' ) )
	{
		char buffer[ 128 ];

		sprintf(buffer,
			"0000-00-00^0000^%s",
			expected_count_list_string );

		expected_count_list_string = strdup( buffer );
	}

	if ( aggregate_level == real_time
	||   aggregate_level == half_hour
	||   aggregate_level == hourly
	||   aggregate_level == daily )
	{
		output_real_time_halfhour_hourly_daily(
			delimiter,
			aggregate_level,
			begin_date_string,
			begin_time_string,
			end_date_string,
			end_time_string,
			expected_count_list_string,
			argv[ 0 ] );
	}
	else
	if ( aggregate_level == weekly )
	{
		output_weekly(
			delimiter,
			begin_date_string,
			end_date_string );
	}
	else
	if ( aggregate_level == monthly )
	{
		output_monthly(
			delimiter,
			begin_date_string,
			end_date_string );
	}
	else
	if ( aggregate_level == annually )
	{
		output_annually(
			delimiter,
			begin_date_string,
			end_date_string );
	}

	return 0;
}

char *display_julian(
		enum aggregate_level aggregate_level,
		double current,
		char delimiter,
		EXPECTED_COUNT *expected_count )
{
	static char display[ 128 ];
	char *date_display_string;
	char *time_display_string;

	if ( aggregate_level == real_time
	||   aggregate_level == half_hour
	||   aggregate_level == hourly )
	{
		date_display_string =
		 	julian_display_yyyy_mm_dd(
				current );

		if ( expected_count
		&&   expected_count->expected_count == 1 )
		{
			time_display_string = strdup( "null" );
		}
		else
		{
			time_display_string =
		 		julian_display_hhmm(
					current );
		}

		sprintf(display,
			"%s%c%s",
		 	date_display_string,
		 	delimiter,
		 	time_display_string );

		free( date_display_string );
		free( time_display_string );
	}
	else
	{
		date_display_string =
		 	julian_display_yyyy_mm_dd(
				current );

		sprintf(display,
			"%s%cnull",
		 	date_display_string,
		 	delimiter );

		free( date_display_string );
	}

	return display;
}

int parse_date_time(
		char *month,
		char *day,
		char *year,
		char *hour,
		char *minute,
		char *date_string,
		char *time_string )
{
	if ( !*date_string ) return 0;

	piece( year, '-', date_string, 0 );

	if ( !piece( month, '-', date_string, 1 ) ) return 0;
	if ( !piece( day, '-', date_string, 2 ) ) return 0;

	if ( strlen( time_string ) != 4 ) return 0;

	*hour = *time_string;
	*(hour + 1) = *(time_string + 1);
	*(hour + 2) = '\0';

	*(minute) = *(time_string + 2);
	*(minute + 1) = *(time_string + 3);
	*(minute + 2) = '\0';

	return 1;
}

time_t get_expected_x(
		LIST *expected_count_list,
		enum aggregate_level aggregate_level,
		time_t current )
{
	EXPECTED_COUNT *expected_count;
	int x_increment_seconds;

	if ( aggregate_level == half_hour )
		return current + SECONDS_IN_HALF_HOUR;

	if ( aggregate_level == hourly )
		return current + SECONDS_IN_HOUR;

	if ( aggregate_level == daily )
		return current + SECONDS_IN_DAY;

	if ( !list_at_end( expected_count_list ) )
	{
		list_push( expected_count_list );
		list_next( expected_count_list );
		expected_count = list_get_pointer( expected_count_list );

		if (	current >=
			expected_count->expected_count_change_time_t )
		{
			list_pop( expected_count_list );
			list_next( expected_count_list );
		}
		else
		{
			list_pop( expected_count_list );
		}
	}

	expected_count = list_get_pointer( expected_count_list );

	x_increment_seconds =
		timlib_get_x_increment_seconds(
				expected_count->expected_count,
				aggregate_level );

	return x_increment_seconds + current;
}

double get_julian_next_expected_x(
		EXPECTED_COUNT *expected_count,
		enum aggregate_level aggregate_level,
		double current )
{
	double x_increment_days = 0.0;

	if ( aggregate_level == half_hour )
		return current + DAYS_PER_HALF_HOUR;
	else
	if ( aggregate_level == hourly )
		return current + DAYS_PER_HOUR;
	else
	if ( aggregate_level == daily )
		return current + 1.0;

	if ( expected_count )
	{
		x_increment_days =
			timlib_get_x_increment_days(
					expected_count->expected_count,
					aggregate_level );
	}

	return x_increment_days + current;
}

void output_weekly(
		char delimiter,
		char *begin_date_string,
		char *end_date_string )
{
	JULIAN *start_date_julian;
	JULIAN *end_date_julian;

	start_date_julian = julian_new_julian( 0 );
	julian_set_yyyy_mm_dd( start_date_julian, begin_date_string );

	end_date_julian = julian_new_julian( 0 );
	julian_set_yyyy_mm_dd( end_date_julian, end_date_string );

	while( start_date_julian->current < end_date_julian->current )
	{
		start_date_julian->current =
			julian_consistent_date_aggregation(
				start_date_julian->current,
				weekly,
				end_date_julian );

		printf( "%s%cnull\n",
			julian_yyyy_mm_dd_string(
				start_date_julian->current ),
			delimiter );
		start_date_julian->current += 7.0;
	}
}

void output_monthly(
		char delimiter,
		char *begin_date_string,
		char *end_date_string )
{
	JULIAN *start_date_julian;
	JULIAN *end_date_julian;

	start_date_julian = julian_new_julian( 0 );
	julian_set_yyyy_mm_dd( start_date_julian, begin_date_string );

	end_date_julian = julian_new_julian( 0 );
	julian_set_yyyy_mm_dd( end_date_julian, end_date_string );

	while( start_date_julian->current < end_date_julian->current )
	{
		start_date_julian->current =
			julian_consistent_date_aggregation(
				start_date_julian->current,
				monthly,
				end_date_julian );

		printf( "%s%cnull\n",
			julian_yyyy_mm_dd_string(
				start_date_julian->current ),
			delimiter );

		start_date_julian->current =
			julian_increment_month(
				start_date_julian->current );
	}
}

void output_annually(
		char delimiter,
		char *begin_date_string,
		char *end_date_string )
{
	JULIAN *start_date_julian;
	JULIAN *end_date_julian;

	start_date_julian = julian_new_julian( 0 );
	julian_set_yyyy_mm_dd( start_date_julian, begin_date_string );

	end_date_julian = julian_new_julian( 0 );
	julian_set_yyyy_mm_dd( end_date_julian, end_date_string );

	while( start_date_julian->current < end_date_julian->current )
	{
		start_date_julian->current =
			julian_consistent_date_aggregation(
				start_date_julian->current,
				annually,
				end_date_julian );

		printf( "%s%cnull\n",
			julian_yyyy_mm_dd_string(
				start_date_julian->current ),
			delimiter );

		start_date_julian->current =
			julian_increment_year(
				start_date_julian->current );
	}
}

void output_real_time_halfhour_hourly_daily(
		char delimiter,
		enum aggregate_level aggregate_level,
		char *begin_date_string,
		char *begin_time_string,
		char *end_date_string,
		char *end_time_string,
		char *expected_count_list_string,
		char *argv_0 )
{
	LIST *expected_count_list = {0};
	char month[ 16 ];
	char day[ 16 ];
	char year[ 16 ];
	char hour[ 16 ];
	char minute[ 16 ];
	JULIAN *current_date;
	JULIAN *end_date;
	EXPECTED_COUNT *expected_count = {0};

	if ( !parse_date_time(	month,
				day,
				year,
				hour,
				minute,
				begin_date_string,
				begin_time_string ) )
	{
		fprintf( stderr,
			 "ERROR in %s: invalid start date (%s)\n",
			 argv_0,
			 begin_date_string );
		exit( 1 );
	}

	current_date =
		julian_date_time_new(
			atoi( year ),
			atoi( month ),
			atoi( day ),
			atoi( hour ),
			atoi( minute ) );

	if ( !parse_date_time(	month,
				day,
				year,
				hour,
				minute,
				end_date_string,
				end_time_string ) )
	{
		fprintf( stderr,
			 "ERROR in %s: invalid end date (%s)\n",
			 argv_0,
			 end_date_string );
		exit( 1 );
	}

	end_date =
		julian_date_time_new(
			atoi( year ),
			atoi( month ),
			atoi( day ),
			atoi( hour ),
			atoi( minute ) );

	if ( aggregate_level == real_time )
	{
		expected_count_list =
			expected_count_with_string_get_expected_count_list(
					expected_count_list_string,
					real_time );

		expected_count = expected_count_fetch(
					expected_count_list,
					current_date->current );

	}

	printf( "%s\n", display_julian(	aggregate_level,
					current_date->current,
					delimiter,
					expected_count ) );

	while( 1 )
	{
		current_date->current =
			get_julian_next_expected_x(
					expected_count,
					aggregate_level,
					current_date->current );

		expected_count =
			expected_count_fetch(
				expected_count_list,
				current_date->current );

		if ( double_virtually_same(	current_date->current,
						end_date->current ) )
		{
			printf( "%s\n",
				display_julian(	aggregate_level,
						current_date->current,
						delimiter,
						expected_count ) );
			break;
		}

		if ( current_date->current > end_date->current )
		{
			break;
		}

		if ( current_date->current < end_date->current )
		{
			printf( "%s\n",
				display_julian(	aggregate_level,
						current_date->current,
						delimiter,
						expected_count ) );
		}
	}
}

