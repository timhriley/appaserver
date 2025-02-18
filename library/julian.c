/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/julian.c					*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "String.h"
#include "piece.h"
#include "minutes_in_day.h"
#include "hours_in_day.h"
#include "julian.h"

JULIAN *julian_new( double current )
{
	return julian_new_julian( current );
}

JULIAN *julian_new_julian( double current )
{
	JULIAN *j;

	if ( ! ( j = (JULIAN *)calloc( 1, sizeof( JULIAN ) ) ) )
	{
		fprintf(stderr,
			"ERROR: memory allocation error in %s/%s()\n",
			__FILE__,
			__FUNCTION__ );
		exit( 1 );
	}
	j->current = current;
	return j;
}

double julian_yyyy_mm_dd_to_julian( char *yyyy_mm_dd )
{
	return julian_yyyy_mm_dd_time_hhmm_to_julian( yyyy_mm_dd, "0000" );
}

void julian_set_time_hhmm(	JULIAN *julian,
				char *hhmm )
{
	char *yyyy_mm_dd;

	yyyy_mm_dd = julian_yyyy_mm_dd_string( julian->current );
	julian->current =
		julian_yyyy_mm_dd_time_hhmm_to_julian( yyyy_mm_dd, hhmm );
}

void julian_set_yyyy_mm_dd(	JULIAN *julian,
				char *yyyy_mm_dd )
{
	julian->current =
		julian_yyyy_mm_dd_time_hhmm_to_julian( yyyy_mm_dd, "0000" );

}

void julian_set_yyyy_mm_dd_hhmm(	JULIAN *julian,
					char *yyyy_mm_dd,
					char *hhmm )
{
	julian->current =
		julian_yyyy_mm_dd_time_hhmm_to_julian( yyyy_mm_dd, hhmm );

}

JULIAN *julian_new_yyyy_mm_dd( char *yyyy_mm_dd )
{
	return julian_yyyy_mm_dd_hhmm_new( yyyy_mm_dd, "0000" );
}

JULIAN *julian_date_time_new(	int year,
				int month,
				int day,
				int hour,
				int minute )
{
	char yyyy_mm_dd[ 16 ];
	char hhmm[ 16 ];

	sprintf( yyyy_mm_dd,
		 "%.2d-%.2d-%.4d",
		 year, month, day );

	sprintf( hhmm,
		 "%.2d%.2d",
		 hour, minute );

	return julian_yyyy_mm_dd_hhmm_new( yyyy_mm_dd, hhmm );

}

JULIAN *julian_yyyy_mm_dd_hhmm_new( char *yyyy_mm_dd, char *hhmm )
{
	JULIAN *julian;

	if ( string_character_count( '-', yyyy_mm_dd ) != 2 )
	{
		return (JULIAN *)0;
	}

	julian = julian_new_julian( 0.0 );

	/* Returns hhmm */
	/* ------------ */
	hhmm = julian_clean_hhmm( hhmm );

	julian->current =
		julian_yyyy_mm_dd_time_hhmm_to_julian( yyyy_mm_dd, hhmm );

	/* if ( !julian->current ) return (JULIAN *)0; */

	return julian;
}

JULIAN *julian_yyyy_mm_dd_new( char *yyyy_mm_dd )
{
	JULIAN *julian;
	char time_string[ 128 ];
	int position_one_based;

	if ( string_character_count( '-', yyyy_mm_dd ) != 2 )
	{
		return (JULIAN *)0;
	}

	julian = julian_new_julian( 0.0 );

	if ( ( position_one_based =
		string_character_exists(
			yyyy_mm_dd,
			':' ) ) )
	{
		char tmp[ 128 ];

		piece( time_string, ':', yyyy_mm_dd, 1 );
		string_strncpy(	tmp,
				yyyy_mm_dd,
				position_one_based - 1 );
		strcpy( yyyy_mm_dd, tmp );
	}
	else
	{
		strcpy( time_string, "0000" );
	}

	julian->current =
		julian_yyyy_mm_dd_time_hhmm_to_julian(
			yyyy_mm_dd,
			time_string );

/*
	if ( !julian->current ) return (JULIAN *)0;
*/

	return julian;
}

void julian_free( JULIAN *julian )
{
	if ( julian ) free( julian );
}

char *julian_yyyy_mm_dd( double current )
{
	return julian_to_yyyy_mm_dd( current );
}

char *julian_display_yyyymmdd( double current )
{
	return julian_to_yyyy_mm_dd( current );
}

char *julian_display( double current )
{
	return julian_to_yyyy_mm_dd( current );
}

#ifdef NOT_DEFINED
char *julian_display_oracle_format( double current )
{
	char *yyyy_mm_dd;
	char *oracle_format;

	yyyy_mm_dd = julian_to_yyyy_mm_dd( current );

	oracle_format = timlib_yyyymmdd_to_oracle_format( yyyy_mm_dd );
	free( yyyy_mm_dd );
	return oracle_format;
}
#endif

char *julian_yyyy_mm_dd_string( double current )
{
	return julian_to_yyyy_mm_dd( current );
}

char *julian_hhmm( double current )
{
	return julian_display_hhmm( current );
}

char *julian_hhmm_string( double current )
{
	return julian_display_hhmm( current );
}

char *julian_display_yyyy_mm_dd_hhmm( double current )
{
	char buffer[ 128 ];
	char *yyyy_mm_dd;
	char *hhmm;

	yyyy_mm_dd = julian_display_yyyy_mm_dd( current );

/*
fprintf( stderr, "%s(): for current = %.5lf, got yyyy_mm_dd = (%s)\n",
__FUNCTION__,
current,
yyyy_mm_dd );
*/
	hhmm = julian_display_hhmm( current );

	sprintf( buffer, "%s:%s", yyyy_mm_dd, hhmm );
	free( yyyy_mm_dd );
	free( hhmm );
	return strdup( buffer );

}

char *julian_display_hhmm( double current )
{
	char buffer[ 128 ];
	int year, month, day, hour, minute;
	double seconds;
	jul2greg( current, &month, &day, &year, &hour, &minute, &seconds );
	sprintf( buffer, "%.2d%.2d", hour, minute );
	return strdup( buffer );
}

char *julian_display_hhmmss( double current )
{
	char buffer[ 128 ];
	int year, month, day, hour, minute;
	double seconds;
	jul2greg( current, &month, &day, &year, &hour, &minute, &seconds );
	sprintf( buffer, "%.2d:%.2d:%.2d", hour, minute, (int)seconds );
	return strdup( buffer );
}

char *julian_display_yyyy_mm_dd( double current )
{
	return julian_to_yyyy_mm_dd( current );
}

double julian_yyyymmdd_time_hhmm_to_julian( char *yyyymmdd, char *hhmm )
{
	int year, month, day, hour, minute;
	char buffer[ 128 ];

	/* Returns hhmm */
	/* ------------ */
	hhmm = julian_clean_hhmm( hhmm );

	if ( strlen( yyyymmdd ) != 8
	||   strlen( hhmm ) != 4 )
	{
		return 0.0;
	}

	/* Get year */
	/* -------- */
	*(buffer + 4) = '\0';
	*buffer = *yyyymmdd;
	*(buffer + 1) = *(yyyymmdd + 1);
	*(buffer + 2) = *(yyyymmdd + 2);
	*(buffer + 3) = *(yyyymmdd + 3);
	year = atoi( buffer );

	/* Get month */
	/* --------- */
	*(buffer + 2) = '\0';
	*(buffer) = *(yyyymmdd + 4);
	*(buffer + 1) = *(yyyymmdd + 5);
	month = atoi( buffer );

	/* Get day */
	/* --------- */
	*(buffer + 2) = '\0';
	*(buffer) = *(yyyymmdd + 6);
	*(buffer + 1) = *(yyyymmdd + 7);
	day = atoi( buffer );

	julian_hour_minute( &hour, &minute, hhmm );

	return greg2jul( month, day, year, hour, minute, 0.0 /* seconds */ );
}

void julian_set_year_month_day( JULIAN *julian, int year, int month, int day )
{
	julian->current = 
		greg2jul(	month,
				day,
				year,
				0 /* hour */,
				0 /* minute */,
				0.0 /* seconds */ );
}

void julian_hour_minute( int *hour, int *minute, char *hhmm )
{
	char buffer[ 3 ];

	/* Returns hhmm */
	/* ------------ */
	hhmm = julian_clean_hhmm( hhmm );

	*(buffer + 2) = '\0';
	*buffer = *hhmm;
	*(buffer + 1) = *(hhmm + 1);
	*hour = atoi( buffer );
	*minute = atoi( hhmm + 2 );

}

double julian_yyyy_mm_dd_time_hhmm_to_julian( char *yyyy_mm_dd, char *hhmm )
{
	int year, month, day, hour, minute;
	char buffer[ 128 ];
	char year_buffer[ 8 ];
	double current;

	/* Returns hhmm */
	/* ------------ */
	hhmm = julian_clean_hhmm( hhmm );

	if ( string_character_count( '-', yyyy_mm_dd ) != 2
	||    strlen( hhmm ) != 4 )
	{
		return 0.0;
	}

	piece( year_buffer, '-', yyyy_mm_dd, 0 );

	if ( strlen( year_buffer ) == 2 )
	{
		string_strcpy(	year_buffer,
				julian_make_y2k_year(
					year_buffer ),
				8 );
	}

	year = atoi( year_buffer );
	month = atoi( piece( buffer, '-', yyyy_mm_dd, 1 ) );
	day = atoi( piece( buffer, '-', yyyy_mm_dd, 2 ) );

	julian_hour_minute( &hour, &minute, hhmm );

	/* if ( !month || !day || !year ) return 0.0; */

/*
fprintf( stderr, "%s(): setting month = %d and day = %d and year = %d and hour = %d and minute = %d\n",
__FUNCTION__,
month, day, year, hour, minute );
*/

	current = greg2jul( month, day, year, hour, minute, 0.0 /* seconds */ );

/*
fprintf( stderr, "%s(): got resulting julian = %.5lf\n",
__FUNCTION__,
current );
*/

	return current;
}

char *julian_to_yyyy_mm_dd( double julian )
{
	char buffer[ 128 ];
	int year, month, day, hour, minute;
	double seconds;

	if ( !julian )
	{
		strcpy( buffer, "0000-00-00" );
	}
	else
	{
		jul2greg(	julian,
				&month,
				&day,
				&year,
				&hour,
				&minute,
				&seconds );
		sprintf( buffer, "%d-%.2d-%.2d", year, month, day );
	}

/*
fprintf( stderr, "%s(): for julian = %.5lf, returning yyyy_mm_dd = (%s)\n",
__FUNCTION__,
julian,
buffer );
*/

	return strdup( buffer );

}

int julian_year_number( double current )
{
	int year, month, day, hour, minute;
	double seconds;
	jul2greg( current, &month, &day, &year, &hour, &minute, &seconds );
	return year;
}

char *julian_to_yyyymmdd( double julian )
{
	return julian_to_yyyy_mm_dd( julian );
}

char *julian_to_hhmm( double julian )
{
	char buffer[ 128 ];
	int year, month, day, hour, minute;
	double seconds;
	jul2greg( julian, &month, &day, &year, &hour, &minute, &seconds );
/*
printf( "1) got minute = %d\n", julian_minute_number( julian ) );
printf( "2) got minute = %d\n", minute );
*/
	sprintf( buffer, "%.2d%.2d", hour, minute );
	return strdup( buffer );
}

double greg2jul( int mon, int day, int year, int h, int mi, double se )
{
/*
** Takes a date, and returns a Julian day. A Julian day is the number of
** days since some base date  (in the very distant past).
** Handy for getting date of x number of days after a given Julian date
** (use jdate to get that from the Gregorian date).
** Author: Robert G. Tantzen, translator: Nat Howard
** Translated from the algol original in Collected Algorithms of CACM
** (This and jdate are algorithm 199).
*/

	long m = mon, d = day, y = year;
	long c, ya, j;
	double seconds = h * 3600.0 + mi * 60 + se;
	double current;

/*
fprintf( stderr, "%s/%s() got mon = %d, day = %d, year = %d\n",
__FILE__,
__FUNCTION__,
mon, day, year );
fflush( stderr );
*/

    if ( ! ( mon + day + year + h + mi + se ) ) return 0.0;

    if (m > 2)
	m -= 3;
    else {
	m += 9;
	--y;
    }
    c = y / 100L;
    ya = y - (100L * c);
    j = (146097L * c) / 4L + (1461L * ya) / 4L + (153L * m + 2L) / 5L + d + 1721119L;
    if (seconds < 12 * 3600.0) {
	j--;
	seconds += 12.0 * 3600.0;
    }
    else {
	seconds = seconds - 12.0 * 3600.0;
    }

	current = (j + (seconds / 3600.0) / 24.0);

/*
fprintf( stderr, "%s/%s() returning = %.5lf\n",
__FILE__,
__FUNCTION__,
current );
fflush( stderr );
*/

	return current;
}


void jul2greg( double jd, int *m, int *d, int *y, int *h, int *mi, double *sec )
{
/* Julian date converter. Takes a julian date (the number of days since
** some distant epoch or other).
** These are Gregorian.
** Copied from Algorithm 199 in Collected algorithms of the CACM
** Author: Robert G. Tantzen, Translator: Nat Howard
*/
    long j = jd;
    double tmp, frac = jd - j;

	/* For some unknown reason, the time is a little short */
	/* --------------------------------------------------- */
	frac += JULIAN_ADJUSTMENT;

    if (frac >= 0.5) {
	frac = frac - 0.5;
        j++;
    }
    else {
	frac = frac + 0.5;
    }

    j -= 1721119L;
    *y = (4L * j - 1L) / 146097L;
    j = 4L * j - 1L - 146097L * *y;
    *d = j / 4L;
    j = (4L * *d + 3L) / 1461L;
    *d = 4L * *d + 3L - 1461L * j;
    *d = (*d + 4L) / 4L;
    *m = (5L * *d - 3L) / 153L;
    *d = 5L * *d - 3 - 153L * *m;
    *d = (*d + 5L) / 5L;
    *y = 100L * *y + j;
    if (*m < 10)
	*m += 3;
    else {
	*m -= 9;
	*y += 1;
    }
    tmp = 3600.0 * (frac * 24.0);
    *h = (int) (tmp / 3600.0);
    tmp = tmp - *h * 3600.0;
    *mi = (int) (tmp / 60.0);
    *sec = tmp - *mi * 60.0;
}


void julian_decrement_days( JULIAN *julian, double number_of_days )
{
	julian->current -= number_of_days;
}

double julian_decrement_day( double current )
{
	return current - 1.0;
}

double julian_increment_day( double current )
{
	return current + 1.0;
}

double julian_increment_month( double current )
{
	int month_number;

	month_number = julian_month_number( current );

	while( month_number == julian_month_number( current ) )
		current++;

	return current;

}

double julian_increment_year( double current )
{
	int year_number;

	year_number = julian_year_number( current );

	while( year_number == julian_year_number( current ) )
		current++;

	return current;

}

double julian_increment_seconds( double julian, int seconds )
{
	double julian_increment;

	julian_increment = (double)seconds / SECONDS_PER_DAY;
	return julian + julian_increment;
}

double julian_increment_minutes( double julian, int minutes )
{
	double julian_increment;

	julian_increment = (double)minutes / MINUTES_PER_DAY;
	return julian + julian_increment;
}

double julian_increment_hours_double( double julian, double hours )
{
	double julian_increment;

	julian_increment = hours / 24.0 /* hours_per_day */;
	return julian + julian_increment;
}

double julian_increment_hours( double julian, int hours )
{
	double julian_increment;

	julian_increment = (double)hours / 24.0 /* hours_per_day */;
	return julian + julian_increment;
}

double julian_add_minutes( double julian, int minutes )
{
	return julian_increment_minutes( julian, minutes );
}

void julian_add_julian( JULIAN *julian, double add_julian )
{
	julian->current += add_julian;
}

#ifdef NOT_DEFINED
JULIAN *julian_oracle_format_new( char *oracle_date_string )
{
	return julian_yyyy_mm_dd_new(
			timlib_oracle_date2mysql_date_string(
				oracle_date_string ) );
}
#endif

char *julian_make_y2k_year( char *two_digit_year_buffer )
{
	static char year_buffer[ 5 ];
	char *ptr = year_buffer;
	char current_century[ 3 ];
	char prior_century[ 16 ];
	int current_two_digit_year;

	if ( strlen( two_digit_year_buffer ) == 4 )
	{
		return two_digit_year_buffer;
	}

	if ( strlen( two_digit_year_buffer ) != 2 ) return "";

	julian_current_century(
		current_century,
		prior_century /* in/out */,
		&current_two_digit_year );

	if ( atoi( two_digit_year_buffer ) > current_two_digit_year )
	{
		*ptr++ = *prior_century;
		*ptr++ = *(prior_century + 1);
	}
	else
	{
		*ptr++ = *current_century;
		*ptr++ = *(current_century + 1);
	}

	*ptr++ = *two_digit_year_buffer++;
	*ptr++ = *two_digit_year_buffer++;
	*ptr = '\0';
	return year_buffer;

}

char *julian_clean_hhmm( char *hhmm )
{
	if ( hhmm && strlen( hhmm ) >= 5 )
	{
		if ( *(hhmm + 2) == ':' ) strcpy( hhmm + 2, hhmm + 3 );
		*(hhmm + 4) = '\0';
	}

	if ( !hhmm || !*hhmm || !atoi( hhmm ) || (strlen( hhmm ) != 4 ) )
	{
		hhmm = "0000";
	}
	return hhmm;

}

void julian_copy( JULIAN *destination_julian, JULIAN *source_julian )
{
	destination_julian->current = source_julian->current;
}

double julian_subtract( JULIAN *begin_julian, JULIAN *end_julian )
{
	return begin_julian->current - end_julian->current;
}

int julian_month_number( double current )
{
	int year, month, day, hour, minute;
	double seconds;
	jul2greg( current, &month, &day, &year, &hour, &minute, &seconds );
	return month;
}

int julian_hour_number( double current )
{
	int year, month, day, hour, minute;
	double seconds;
	jul2greg( current, &month, &day, &year, &hour, &minute, &seconds );
	return hour;
}

int julian_half_hour_number( double current )
{
	int year, month, day, hour, minute;
	double seconds;

	jul2greg( current, &month, &day, &year, &hour, &minute, &seconds );

	if ( minute >= 0 && minute <= 29 )
		return 0;
	else
		return 30;
}

double julian_round_to_half_hour( double current )
{
	int year, month, day, hour, minute, half_hour_number;
	double seconds;
	jul2greg( current, &month, &day, &year, &hour, &minute, &seconds );
	half_hour_number = julian_half_hour_number( current );
	minute = half_hour_number;
	return greg2jul( month, day, year, hour, minute, 0.0 /* seconds */ );
}

int julian_minute_number( double current )
{
	int year, month, day, hour, minute;
	double seconds;
	jul2greg( current, &month, &day, &year, &hour, &minute, &seconds );
	return minute;
}

int julian_week_number( double current )
{
	int year, month, day, hour, minute;
	double seconds;
	JULIAN *tmp_julian;
	int week;

	jul2greg( current, &month, &day, &year, &hour, &minute, &seconds );
	tmp_julian = julian_new_julian( 0 );
	julian_set_year_month_day( tmp_julian, year, 1, 1 );
	week = ( ( current - tmp_julian->current ) / 7.0 ) + 1;
	julian_free( tmp_julian );
	return week;
}

int julian_day_of_month_number( double current )
{
	int year, month, day, hour, minute;
	double seconds;

	jul2greg( current, &month, &day, &year, &hour, &minute, &seconds );
	return day;

}

int julian_day_number( double current )
{
	int year, month, day, hour, minute;
	double seconds;
	JULIAN *tmp_julian;
	int day_number;
	int divide_by;

	jul2greg( current, &month, &day, &year, &hour, &minute, &seconds );
	tmp_julian = julian_new_julian( 0 );
	julian_set_year_month_day( tmp_julian, year, 1, 1 );

	if ( year % 4 == 0 )
		divide_by = 366;
	else
		divide_by = 365;

	day_number = ( (int)( current - tmp_julian->current ) % divide_by ) + 1;
	julian_free( tmp_julian );
	return day_number;
}

double julian_consistent_date_aggregation(
		double current,
		enum aggregate_level aggregate_level,
		JULIAN *end_date_julian )
{
	if ( aggregate_level == aggregate_level_none
	||   aggregate_level == real_time
	||   aggregate_level == half_hour
	||   aggregate_level == hourly
	||   aggregate_level == daily )
	{
		return current;
	}

	if ( aggregate_level == weekly )
	{
		int current_week_number;
		int next_week_number;

		for(	current_week_number = julian_week_number( current );
			;
			current++ )
		{
			next_week_number = julian_week_number( current );

			if ( next_week_number != current_week_number )
				return current - 1.0;

			if ( end_date_julian
			&&   current == end_date_julian->current )
			{
				return current;
			}
		}
	}
	else
	if ( aggregate_level == monthly )
	{
		int current_month_number;
		int next_month_number;

		for( current_month_number = julian_month_number( current );
		     ;
		     current++ )
		{
			next_month_number = julian_month_number( current );
			if ( next_month_number != current_month_number )
				return current - 1.0;

			if ( end_date_julian
			&&   current == end_date_julian->current )
			{
				return current;
			}
		}
	}
	else
	if ( aggregate_level == annually )
	{
		int current_year_number;
		int next_year_number;

		for( current_year_number = julian_year_number( current );
		     ;
		     current++ )
		{
			next_year_number = julian_year_number( current );
			if ( next_year_number != current_year_number )
				return current - 1.0;

			if ( end_date_julian
			&&   current == end_date_julian->current )
			{
				return current;
			}
		}
	}
	return current;
}

void julian_current_century(
		char *current_century,
		char *prior_century /* in/out */,
		int *current_two_digit_year )
{
	char *current_year;

	current_year = string_pipe_fetch( "/bin/date '+%Y'" );

	*current_century = *current_year;
	*(current_century + 1) = *(current_year + 1);
	*(current_century + 2) = '\0';

	sprintf( prior_century, "%.2d", atoi( current_century ) - 1 );

	*current_two_digit_year = atoi( current_year + 2 );
	free( current_year );

}

