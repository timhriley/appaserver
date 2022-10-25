/* $APPASERVER_HOME/library/date.c			*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "piece.h"
#include "column.h"
#include "environ.h"
#include "timlib.h"
#include "julian.h"
#include "date_convert.h"
#include "date.h"

/* Arrays */
/* ------ */
char *date_military_month_array[] =
			{ 	"JAN",
				"FEB",
				"MAR",
				"APR",
				"MAY",
				"JUN",
				"JUL",
				"AUG",
				"SEP",
				"OCT",
				"NOV",
				"DEC",
				(char *)0 };

char *date_month_array[] =
			{	"January",
				"February",
				"March",
				"April",
				"May",
				"June",
				"July",
				"August",
				"September",
				"October",
				"November",
				"December",
				(char *)0 };

char *date_day_array[] =
			{	"Sunday",
				"Monday",
				"Tuesday",
				"Wednesday",
				"Thursday",
				"Friday",
				"Saturday",
				(char *)0 };

char *date_month_military( int month_offset )
{
	if ( month_offset < 0 || month_offset >= 12 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid month_offset = (%d)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			month_offset );
		exit( 1 );
	}
	else
	{
		return date_military_month_array[ month_offset ];
	}

}

char *date_month_string( int month_offset )
{
	if ( month_offset < 0 || month_offset > 11 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid month_offset = (%d)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			month_offset );
		exit( 1 );
	}
	else
	{
		return date_month_array[ month_offset ];
	}

}

char *date_day_string( int day_offset )
{
	if ( day_offset < 0 || day_offset > 6 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid day_offset = (%d)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			day_offset );
		exit( 1 );
	}
	else
	{
		return date_day_array[ day_offset ];
	}
}

int date_month_name_integer( char *month_name )
{
	char **array_ptr = date_month_array;
	register int i = 0;

	while( *array_ptr )
	{
		if ( strcasecmp( month_name, *array_ptr ) == 0 )
		{
			return i;
		}
		else
		{
			i++;
			array_ptr++;
		}
	}
	return -1;
}

DATE *date_calloc( void )
{
	DATE *d = (DATE *)calloc( 1, sizeof( DATE ) );

	if ( !d )
	{
		fprintf(stderr,
			"ERROR: memory allocation failed in %s/%s()/%d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	d->tm = ( struct tm *)calloc( 1, sizeof( struct tm ) );

	if ( !d->tm )
	{
		fprintf(stderr,
			"ERROR: memory allocation failed in %s/%s()/%d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	d->is_daylight_time = date_is_daylight_time();
	date_set_TZ( DATE_GMT );

	return d;
}

DATE *date_new_date( void )
{
	return date_calloc();
}

DATE *date_current_new(	time_t current,
			int utc_offset )
{
	DATE *date;

	date = date_calloc();

	date_set_tm_structures( date, current, utc_offset );
	date->current = current;

	return date;

}

time_t date_yyyy_mm_dd_time_hhmm_to_time_t(
			char *date_string,
			char *time_string )
{
	DATE *date;
	time_t current;

	date = date_yyyy_mm_dd_hhmm_new(
		date_string,
		time_string );

	current = date->current;
	date_free( date );
	return current;

}

DATE *date_time_new(	int year,
			int month,
			int day,
			int hour,
			int minute )
{
	return date_new_date_time(
			year,
			month,
			day,
			hour,
			minute,
			0 /* seconds */,
			0 /* utc_offset */ );
}

DATE *date_new_date_time(
			int year,
			int month,
			int day,
			int hour,
			int minutes,
			int seconds,
			int utc_offset )
{
	DATE *d;

	if ( year >= 2038 && sizeof( time_t ) == 4 )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: year must be earlier than 2038 for 32 bit hardware.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	d = date_calloc();
	d->tm->tm_year = year - 1900;
	d->tm->tm_mon = month - 1;
	d->tm->tm_mday = day;
	d->tm->tm_hour = hour;
	d->tm->tm_min = minutes;
	d->tm->tm_sec = seconds;

	d->current =
		date_tm_to_current(
			d->tm,
			utc_offset );

	date_set_tm_structures(
			d,
			d->current,
			utc_offset );

	return d;

}

DATE *date_new(	int year,
		int month,
		int day )
{
	DATE *d;

	d = date_new_date_time(
			year,
			month,
			day,
			0 /* hour */,
			0 /* minutes */,
			0 /* seconds */,
			0 /* utm_offset */ );

	return d;
}

time_t date_tm_to_current(	struct tm *tm,
				int utc_offset )
{
	if ( tm->tm_year < 70 )
	{
		return date_tm_to_current_pre_1970( tm, utc_offset );
	}
	else
	{
		return date_mktime( tm, utc_offset );
	}
}

time_t date_tm_to_current_pre_1970(	struct tm *tm,
					int utc_offset )
{
	struct tm future_tm = {0};
	time_t future_time_t;
	struct tm current_tm = {0};

	future_tm.tm_year = 2026 - 1900;
	future_tm.tm_mon = 0;

	/* 2017-10-06 */
	/* ---------- */
	future_tm.tm_mday = 1;

	future_tm.tm_hour = 0;
	future_tm.tm_min = 0;
	future_tm.tm_sec = 0;

	future_time_t = date_mktime( &future_tm, utc_offset );

	current_tm.tm_year = tm->tm_year + 56;
	current_tm.tm_mon = tm->tm_mon;
	current_tm.tm_mday = tm->tm_mday;
	current_tm.tm_hour = tm->tm_hour;
	current_tm.tm_min = tm->tm_min;

	return date_mktime( &current_tm, utc_offset ) - future_time_t;

}

void date_set_date_time_integers(
				DATE *date,
				int year,
				int month,
				int day,
				int hours,
				int minutes,
				int seconds,
				int utc_offset )
{
	date->tm->tm_year = year - 1900;
	date->tm->tm_mon = month - 1;
	date->tm->tm_mday = day;
	date->tm->tm_hour = hours;
	date->tm->tm_min = minutes;
	date->tm->tm_sec = seconds;

	date->current = date_tm_to_current( date->tm, utc_offset );
	date_set_tm_structures( date, date->current, utc_offset );

}

DATE *date_set_year(		DATE *date,
				int year,
				int utc_offset )
{
	date->tm->tm_year = year - 1900;

	date->current = date_tm_to_current( date->tm, utc_offset );
	date_set_tm_structures( date, date->current, utc_offset );
	return date;
}

DATE *date_set_day(		DATE *date,
				int day,
				int utc_offset )
{
	date->tm->tm_mday = day;

	date->current = date_tm_to_current( date->tm, utc_offset );
	date_set_tm_structures( date, date->current, utc_offset );
	return date;
}

DATE *date_set_time_integers(	DATE *date,
				int hour,
				int minute,
				int seconds,
				int utc_offset )
{
	date->tm->tm_hour = hour;
	date->tm->tm_min = minute;
	date->tm->tm_sec = seconds;

	date->current = date_tm_to_current( date->tm, utc_offset );
	date_set_tm_structures( date, date->current, utc_offset );
	return date;
}

DATE *date_set_date_integers(	DATE *date,
				int year,
				int month,
				int day,
				int utc_offset )
{
	date->tm->tm_year = year - 1900;
	date->tm->tm_mon = month - 1;
	date->tm->tm_mday = day;

	date->current = date_tm_to_current( date->tm, utc_offset );
	date_set_tm_structures( date, date->current, utc_offset );
	return date;
}

boolean date_set_yyyy_mm_dd_hhmm_delimited(
				DATE *date,
				char *yyyy_mm_dd_hhmm,
				int date_piece,
				int time_piece,
				char delimiter,
				int utc_offset )
{
	char yyyy_mm_dd[ 128 ];
	char hhmm[ 128 ];

	if ( !piece( yyyy_mm_dd, delimiter, yyyy_mm_dd_hhmm, date_piece ) )
	{
		fprintf( stderr,
			 "Warning in %s/%s()/%d: cannot piece(%d) in (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 date_piece,
			 yyyy_mm_dd );
		return 0;
	}

	if ( time_piece == -1 )
	{
		*hhmm = '\0';
	}
	else
	if ( !piece( hhmm, delimiter, yyyy_mm_dd_hhmm, time_piece ) )
	{
		fprintf( stderr,
			 "Warning in %s/%s()/%d: cannot piece(%d) in (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 time_piece,
			 yyyy_mm_dd );
		return 0;
	}

	if ( !*hhmm || strcasecmp( hhmm, "null" ) == 0 )
	{
		return date_set_yyyy_mm_dd(
				date,
				yyyy_mm_dd );
	}

	return date_set_yyyy_mm_dd_hhmm(
		date,
		yyyy_mm_dd,
		hhmm,
		utc_offset );
}

boolean date_set_yyyy_mm_dd_hhmm(	DATE *date,
					char *yyyy_mm_dd,
					char *hhmm,
					int utc_offset )
{
	char year_string[ 128 ];
	char month_string[ 128 ];
	char day_string[ 128 ];
	char delimiter;
	int hours, minutes;
	char buffer[ 3 ];

	if ( count_character( '-', yyyy_mm_dd ) == 2 )
		delimiter = '-';
	else
	if ( count_character( '.', yyyy_mm_dd ) == 2 )
		delimiter = '.';
	else
		return 0;

	piece( year_string, delimiter, yyyy_mm_dd, 0 );
	piece( month_string, delimiter, yyyy_mm_dd, 1 );
	piece( day_string, delimiter, yyyy_mm_dd, 2 );

	if ( strlen( hhmm ) != 4 ) return 0;

	*(buffer + 2) = '\0';
	*buffer = *hhmm;
	*(buffer + 1) = *(hhmm + 1);
	hours = atoi( buffer );

	*buffer = *(hhmm + 2);
	*(buffer + 1) = *(hhmm + 3);
	minutes = atoi( buffer );

	date_set_date_time_integers(
				date,
				atoi( year_string ),
				atoi( month_string ),
				atoi( day_string ),
				hours,
				minutes,
				0 /* seconds */,
				utc_offset );

	return 1;
}

boolean date_set_yyyy_mm_dd(	DATE *date,
				char *yyyy_mm_dd )
{
	char year_string[ 128 ];
	char month_string[ 128 ];
	char day_string[ 128 ];
	char delimiter;

	if ( count_character( '-', yyyy_mm_dd ) == 2 )
		delimiter = '-';
	else
	if ( count_character( '.', yyyy_mm_dd ) == 2 )
		delimiter = '.';
	else
		return 0;

	piece( year_string, delimiter, yyyy_mm_dd, 0 );
	piece( month_string, delimiter, yyyy_mm_dd, 1 );
	piece( day_string, delimiter, yyyy_mm_dd, 2 );

	date_set_date_integers(	date,
				atoi( year_string ),
				atoi( month_string ),
				atoi( day_string ),
				0 /* utc_offset */ );

	date_set_time_integers(	date,
				0 /* hour */,
				0 /* minute */,
				0 /* seconds */,
				0 /* utc_offset */ );

	return 1;
}

DATE *date_set_time(	DATE *date,
			int hour,
			int minutes )
{
	return date_set_time_integers(
		date,
		hour,
		minutes,
		0 /* seconds */,
		0 /* utc_offset */ );
}

DATE *date_set_time_seconds(
			DATE *date,
			int hour,
			int minutes,
			int seconds )
{
	return date_set_time_integers(
		date,
		hour,
		minutes,
		seconds,
		0 /* utc_offset */ );
}

void date_free( DATE *d )
{
	free( d->tm );
	free( d );
}

DATE *date_increment( DATE *d )
{
	return date_increment_day( d );
}

DATE *date_decrement_minute( DATE *d )
{
	return date_increment_minutes( d, -1 );
}

DATE *date_decrement_hour( DATE *d )
{
	return date_increment_hours( d, -1.0 );
}

DATE *date_increment_hour( DATE *d )
{
	return date_increment_hours( d, 1.0 );
}

DATE *date_increment_minute( DATE *d )
{
	return date_increment_minutes( d, 1 );
}

DATE *date_decrement_day( DATE *d )
{
	return date_decrement_days( d, 1.0 );
}

DATE *date_decrement_days(	DATE *d,
				double days )
{
	double minus_days = -days;

	date_increment_days(
		d,
		minus_days );
	return d;
}

DATE *date_increment_months(	DATE *d,
				int months )
{
	int year;
	int month;
	int day;

	year = date_year( d );
	month = date_month( d );
	day = date_day( d );
	month += months;

	date_set_date_integers(
		d,
		year,
		month,
		day,
		0 /* utc_offset */ );
	return d;
}

DATE *date_increment_years(	DATE *d,
				int years )
{
	return date_decrement_years(d, -years );
}

DATE *date_subtract_year(
			DATE *date,
			int years )
{
	return
		date_decrement_years(
				date,
				years );
}

DATE *date_decrement_years(	DATE *d,
				int years )
{
	int year;
	int month;
	int day;

	year = date_year( d );
	month = date_month( d );
	day = date_day( d );
	year -= years;

	date_set_date_integers(
		d,
		year,
		month,
		day,
		0 /* utc_offset */ );

	return d;
}

DATE *date_increment_day( DATE *d )
{
	d->current += SECONDS_IN_DAY;
	date_set_tm_structures( d, d->current, date_utc_offset() );
	return d;
}

DATE *date_increment_week( DATE *d )
{
	increment_week( d );
	return d;
}

DATE *increment_week( DATE *d )
{
	d->current += SECONDS_IN_WEEK;
	date_set_tm_structures( d, d->current, date_utc_offset() );
	return d;
}

int date_get_month( DATE *d )
{
	return date_month( d );
}

int get_month( DATE *d )
{
	return date_get_month( d );
}

int date_month_integer( DATE *d )
{
	return date_month( d );
}

int date_month( DATE *d )
{
	return d->tm->tm_mon + 1;
}

int date_day_offset( DATE *date )
{
	return date_day_of_week( date );
}

int get_day_of_month( DATE *d )
{
	return date_day_of_month( d );
}

int date_get_day_of_month( DATE *d )
{
	return date_day_of_month( d );
}

int date_day_integer( DATE *date )
{
	return date_day_of_month( date );
}

int date_hour_time( DATE *date )
{
	int hour_integer = date_hour_integer( date );

	if ( hour_integer <= 12 )
		return hour_integer;
	else
		return hour_integer - 12;
}

int date_hour_integer( DATE *date )
{
	return date->tm->tm_hour;
}

int date_day_of_month( DATE *d )
{
	return d->tm->tm_mday;
}

int date_get_day_of_week( DATE *d )
{
	return date_day_of_week( d );
}

int date_day_of_week( DATE *d )
{
	return d->tm->tm_wday;
}

char *date_get_day_of_week_string( DATE *d )
{
	return date_day_string( date_day_offset( d ) );

/* 
	if ( d->tm->tm_wday == WDAY_SUNDAY )
		return "Sunday";
	else
	if ( d->tm->tm_wday == WDAY_MONDAY )
		return "Monday";
	else
	if ( d->tm->tm_wday == WDAY_TUESDAY )
		return "Tuesday";
	else
	if ( d->tm->tm_wday == WDAY_WEDNESDAY )
		return "Wednesday";
	else
	if ( d->tm->tm_wday == WDAY_THURSDAY )
		return "Thursday";
	else
	if ( d->tm->tm_wday == WDAY_FRIDAY )
		return "Friday";
	else
	if ( d->tm->tm_wday == WDAY_SATURDAY )
		return "Saturday";
	else
		return "Unknown";
*/
}

int date_year( DATE *d )
{
	return d->tm->tm_year + 1900;
}

int date_get_year( DATE *d )
{
	return date_year( d );
}

int get_year( DATE *d )
{
	return date_year( d );
}

int date_hour( DATE *d )
{
	return date_get_hour( d );
}

int get_hour( DATE *d )
{
	return date_get_hour( d );
}

int date_get_hour( DATE *d )
{
	return date_hour_integer( d );
}

int date_minutes( DATE *d )
{
	return date_get_minutes( d );
}

int get_minutes( DATE *d )
{
	return date_get_minutes( d );
}

int date_seconds( DATE *d )
{
	return date_get_seconds( d );
}

int get_seconds( DATE *d )
{
	return date_get_seconds( d );
}

int date_get_minutes( DATE *d )
{
	return d->tm->tm_min;
}

int date_get_seconds( DATE *d )
{
	return d->tm->tm_sec;
}

int date_months_between(	DATE *from_date,
				DATE *to_date )
{
	int from_year;
	int to_year;
	int from_month;
	int to_month;

	from_year = date_year( from_date );
	to_year = date_year( to_date );

	from_month = date_month( from_date );
	to_month = date_month( to_date );

	return ( ( to_year - from_year ) * 12 ) + ( to_month - from_month );
}

/* ------------------------------------------------------------- */
/* Sample input: from_date = "2017-03-01" to_date = "2017-04-16" */
/* ------------------------------------------------------------- */
int date_days_between(	char *from_date_string,
			char *to_date_string )
{
	DATE *from_date;
	DATE *to_date;
	time_t difference;

	/* If first time, then return 1 */
	/* ---------------------------- */
	if ( !from_date_string ) return 1;

	if ( !to_date_string ) return 0;

	if ( ! ( from_date =
			/* ------------------- */
			/* Trims trailing time */
			/* ------------------- */
			date_yyyy_mm_dd_new(
				from_date_string ) ) )
	{
		return 0;
	}

	if ( ! ( to_date =
			/* ------------------- */
			/* Trims trailing time */
			/* ------------------- */
			date_yyyy_mm_dd_new(
				to_date_string ) ) )
	{
		return 0;
	}

	difference = to_date->current - from_date->current;

	date_free( from_date );
	date_free( to_date );

	return (int) (difference / SECONDS_IN_DAY);
}

/* --------------------------------------------------------- */
/* Sample input: from_date = "10/06/60" to_date = "08/04/11" */
/* --------------------------------------------------------- */
int date_years_between( char *from_date, char *to_date )
{
	int from_year, from_month, from_day;
	int to_year, to_month, to_day;
	int diff_year, diff_month, diff_day;

	if ( !date_parse(	&from_year,
				&from_month,
				&from_day,
				from_date ) )
	{
		return -1;
	}

	if ( !date_parse(	&to_year,
				&to_month,
				&to_day,
				to_date ) )
	{
		return -1;
	}

	diff_year = to_year - from_year;
	diff_month = to_month - from_month;
	diff_day = to_day - from_day;

	if ( diff_day < 0 ) diff_month--;
	if ( diff_month < 0 ) diff_year--;

	if ( diff_year < 0 ) diff_year = 0;

	return diff_year;
}

void date_time_parse(		int *hours,
				int *minutes,
				char *hhmm )
{
	char buffer[ 3 ];
	char *ptr;

	*hours = 0;
	*minutes = 0;

	if ( timlib_strlen( hhmm ) != 4 ) return;

	ptr = buffer;
	*ptr++ = *hhmm++;
	*ptr++ = *hhmm++;
	*ptr = '\0';

	*hours = atoi( buffer );

	ptr = buffer;
	*ptr++ = *hhmm++;
	*ptr++ = *hhmm++;
	*ptr = '\0';

	*minutes = atoi( buffer );

}

boolean date_parse(	int *year,
			int *month,
			int *day,
			char *date_string )
{
	char piece_buffer[ 128 ];

	*month = 0;
	*day = 0;
	*year = 0;

	if ( number_occurrences_char( '/', date_string ) == 2 )
	{
		*month = atoi( piece( piece_buffer, '/', date_string, 0 ) );
		*day = atoi( piece( piece_buffer, '/', date_string, 1 ) );
		piece( piece_buffer, '/', date_string, 2 );

		timlib_strcpy(	piece_buffer,
				julian_make_y2k_year( piece_buffer ),
				128 );

		*year = atoi( piece_buffer );
		return 1;
	}
	else
	if ( number_occurrences_char( '-', date_string ) == 2 )
	{
		*month = atoi( piece( piece_buffer, '-', date_string, 1 ) );
		*day = atoi( piece( piece_buffer, '-', date_string, 2 ) );
		piece( piece_buffer, '-', date_string, 0 );
	
		timlib_strcpy(	piece_buffer,
				julian_make_y2k_year( piece_buffer ),
				128 );

		*year = atoi( piece_buffer );
		return 1;
	}
	return 0;
}

int date_minutes_between(	char *from_date_string,
				char *from_time_string,
				char *to_date_string,
	       			char *to_time_string,
				boolean add_one )
{
	DATE *from_date;
	DATE *to_date;
	time_t difference;

	if ( strcmp( from_time_string, "null" ) == 0
	||   strcmp( to_time_string, "null" ) == 0 )
	{
		return 24 * 60;
	}

	from_date =
		date_yyyy_mm_dd_new(
			from_date_string );

	date_set_time_hhmm( from_date, from_time_string );

	to_date =
		date_yyyy_mm_dd_new(
			to_date_string );

	date_set_time_hhmm( to_date, to_time_string );

	difference = to_date->current - from_date->current;

	date_free( from_date );
	date_free( to_date );

	if ( add_one )
		return ( difference / SECONDS_IN_MINUTE ) + 1;
	else
		return difference / SECONDS_IN_MINUTE;

}

int age( char *birth_date )
{
	char today[ 64 ];
	char with_slashes[ 64 ];
	FILE *p;

	p = popen( "date '+%m/%d/%y'", "r" );
	if ( !p )
	{
		fprintf(stderr,
			"ERROR: age() cannot open pipe to get today's date\n" );
		exit( 1 );
	}

	get_line( today, p );
	pclose( p );

	add_slashes_maybe( with_slashes, birth_date );

	return date_years_between( with_slashes, today );

}

void add_slashes_maybe( char *d, char *s )
{
	char buf1[ 32 ], buf2[ 32 ], buf3[ 32 ];

	if ( strlen( s ) != 6 )
	{
		strcpy( d, s );
		return;
	}

	sprintf( d, 
		 "%s/%s/%s",
		 midstr( buf1, s, 0, 2 ),
		 midstr( buf2, s, 2, 2 ),
		 midstr( buf3, s, 4, 2 ) );

}

DATE *date_19new( char *date_time_string )
{
	return date_yyyy_mm_dd_hms_new(	date_time_string );
}

DATE *date_yyyy_mm_dd_hms_new(	char *date_time_string )
{
	char year_string[ 16 ];
	char month_string[ 16 ];
	char day_string[ 16 ];
	char hour_string[ 16 ];
	char minute_string[ 16 ];
	char second_string[ 16 ];
	char date_half[ 32 ];
	char time_half[ 32 ];
	DATE *date;

	if ( count_character( '-', date_time_string ) != 2 )
		return (DATE *)0;

	if ( count_character( ' ', date_time_string ) != 1 )
		return (DATE *)0;

	if ( count_character( ':', date_time_string ) == 0 )
		return (DATE *)0;

	column( date_half, 0, date_time_string );
	column( time_half, 1, date_time_string );

	piece( year_string, '-', date_half, 0 );
	piece( month_string, '-', date_half, 1 );
	piece( day_string, '-', date_half, 2 );

	piece( hour_string, ':', time_half, 0 );
	piece( minute_string, ':', time_half, 1 );

	if ( !piece( second_string, ':', time_half, 2 ) )
	{
		strcpy( second_string, "0" );
	}

	date = date_new_date_time(
			atoi( year_string ),
			atoi( month_string ),
			atoi( day_string ),
			atoi( hour_string ),
			atoi( minute_string ),
			atoi( second_string ),
			0 /* utm_offset */ );

	return date;

}

DATE *date_yyyy_mm_dd_colon_hm_new( char *date_time_string )
{
	char year_string[ 16 ];
	char month_string[ 16 ];
	char day_string[ 16 ];
	char hour_string[ 3 ] = {0};
	char minute_string[ 3 ] = {0};
	char date_half[ 32 ];
	char time_half[ 32 ];
	DATE *date;

	if ( count_character( '-', date_time_string ) != 2 )
		return (DATE *)0;

	if ( count_character( ':', date_time_string ) != 1 )
		return (DATE *)0;

	piece( date_half, ':', date_time_string, 0 );
	piece( time_half, ':', date_time_string, 1 );

	piece( year_string, '-', date_half, 0 );
	piece( month_string, '-', date_half, 1 );
	piece( day_string, '-', date_half, 2 );

	*hour_string = *time_half;
	*( hour_string + 1 ) = *( time_half + 1 );

	*minute_string = *( time_half + 2 );
	*( minute_string + 1 ) = *( time_half + 3 );

	date = date_new_date_time(
			atoi( year_string ),
			atoi( month_string ),
			atoi( day_string ),
			atoi( hour_string ),
			atoi( minute_string ),
			0 /* seconds */,
			0 /* utm_offset */ );

	return date;

}

DATE *date_yyyy_mm_dd_hm_new( char *date_time_string )
{
	char year_string[ 16 ];
	char month_string[ 16 ];
	char day_string[ 16 ];
	char hour_string[ 16 ];
	char minute_string[ 16 ];
	char date_half[ 32 ];
	char time_half[ 32 ];
	DATE *date;

	if ( count_character( '-', date_time_string ) != 2 )
		return (DATE *)0;

	if ( count_character( ' ', date_time_string ) != 1 )
		return (DATE *)0;

	if ( count_character( ':', date_time_string ) == 0 )
		return (DATE *)0;

	column( date_half, 0, date_time_string );
	column( time_half, 1, date_time_string );

	piece( year_string, '-', date_half, 0 );
	piece( month_string, '-', date_half, 1 );
	piece( day_string, '-', date_half, 2 );

	piece( hour_string, ':', time_half, 0 );
	piece( minute_string, ':', time_half, 1 );

	date = date_new_date_time(
			atoi( year_string ),
			atoi( month_string ),
			atoi( day_string ),
			atoi( hour_string ),
			atoi( minute_string ),
			0 /* seconds */,
			0 /* utm_offset */ );

	return date;

}

DATE *date_yyyy_mm_dd_new( char *date_string )
{
	char year_string[ 128 ];
	char month_string[ 128 ];
	char day_string[ 128 ];
	DATE *date;
	char delimiter;
	char local_date_string[ 32 ];

	/* Ignore the appended time */
	/* ------------------------ */
	if ( count_character( ' ', date_string ) == 1 )
	{
		column( local_date_string, 0, date_string );
		date_string = local_date_string;
	}

	if ( count_character( '-', date_string ) == 2 )
		delimiter = '-';
	else
	if ( count_character( '.', date_string ) == 2 )
		delimiter = '.';
	else
		return (DATE *)0;

	piece( year_string, delimiter, date_string, 0 );
	piece( month_string, delimiter, date_string, 1 );
	piece( day_string, delimiter, date_string, 2 );

	date = date_new( 	atoi( year_string ),
				atoi( month_string ),
				atoi( day_string ) );
	return date;

}

DATE *date_increment_days(	DATE *d,
				double days )
{
	d->current += (long)((double)SECONDS_IN_DAY * days);
	date_set_tm_structures( d, d->current, 0 /* utc_offset */ );
	return d;
}

char *date_yyyy_mm_dd( DATE *date )
{
	static char destination[ 16 ];
	return date_get_yyyy_mm_dd( destination, date );
}

/* Returns static memory */
/* --------------------- */
char *date_get_month_number_string(
				DATE *date )
{
	static char number_string[ 16 ];

	sprintf( 	number_string, 
			"%02d",
			date_month( date ) );

	return number_string;
}

/* Returns static memory */
/* --------------------- */
char *date_get_day_number_string(
				DATE *date )
{
	static char number_string[ 16 ];

	sprintf( 	number_string, 
			"%02d",
			date_get_day( date ) );

	return number_string;
}

char *date_get_yyyy_mm_dd( char *destination, DATE *date )
{
	sprintf( 	destination, 
			"%d-%02d-%02d",
			date_year( date ),
			date_month( date ),
			date_day_of_month( date ) );
	return destination;
}

char *date_get_colon_hhmmss( char *destination, DATE *date )
{
	sprintf( 	destination, 
			"%02d:%02d:%02d",
			date_get_hour( date ),
			date_get_minutes( date ),
			date_get_seconds( date ) );
	return destination;
}

char *date_get_hhmmss( char *destination, DATE *date )
{
	sprintf( 	destination, 
			"%02d%02d:%02d",
			date_get_hour( date ),
			date_get_minutes( date ),
			date_get_seconds( date ) );
	return destination;
}

char *date_get_hhmm( char *destination, DATE *date )
{
	sprintf( 	destination, 
			"%02d%02d",
			date_get_hour( date ),
			date_get_minutes( date ) );
	return destination;
}

char *date_display_yyyymmdd( DATE *date )
{
	return date_display_yyyy_mm_dd( date );
}

char *date_display( DATE *date )
{
	return date_display_yyyy_mm_dd( date );
}

char *date_yyyy_mm_dd_display( DATE *date )
{
	return  date_static_display_yyyy_mm_dd( date );
}

char *date_static_display_yyyy_mm_dd( DATE *date )
{
	static char buffer[ 16 ];

	if ( !date ) return "";

	sprintf( 	buffer, 
			"%d-%02d-%02d",
			date_year( date ),
			date_month( date ),
			date_day_of_month( date ) );

	return buffer;
}

char *date_static_display( DATE *date )
{
	return date_static_display_yyyy_mm_dd( date );
}

char *date_yyyy_mm_dd_hhmm_display( DATE *date )
{
	char buffer[ 128 ];
	
	sprintf( buffer,
		 "%s:",
		 date_static_display_yyyy_mm_dd( date ) );

	date_get_hhmm(
		 buffer + strlen( buffer ), date );

	return strdup( buffer );
}

char *date_display_yyyy_mm_dd_hh_mm(
				DATE *date )
{
	return date_yyyy_mm_dd_hhmm_display( date );
}

char *date_display_yyyy_mm_dd_colon_hm( DATE *date )
{
	return date_yyyy_mm_dd_hhmm_display( date );
}

char *date_hms( DATE *date )
{
	return date_get_hms( date );
}

char *date_get_hms( DATE *date )
{
	char buffer[ 128 ];
	
	date_get_colon_hhmmss( buffer, date );
	return strdup( buffer );

}

char *date_display19( DATE *date )
{
	return date_display_yyyy_mm_dd_colon_hms( date );
}

char *date_display_19( DATE *date )
{
	return date_display_yyyy_mm_dd_colon_hms( date );
}

char *date_display_yyyy_mm_dd_colon_hms( DATE *date )
{
	char buffer[ 128 ];
	
	sprintf( buffer,
		 "%s ",
		 date_static_display_yyyy_mm_dd( date ) );

	date_get_colon_hhmmss(
		 buffer + strlen( buffer ), date );

	return strdup( buffer );
}

char *date_display_yyyy_mm_dd( DATE *date )
{
	char buffer[ 128 ];

	if ( !date ) return "";

	sprintf( 	buffer, 
			"%d-%02d-%02d",
			date_year( date ),
			date_month( date ),
			date_day_of_month( date ) );

	return strdup( buffer );
}

char *date_display_hhmm( DATE *date )
{
	char buffer[ 128 ];

	if ( !date ) return "";

	date_get_hhmm( buffer, date );
	return strdup( buffer );
}

char *date_display_hhmmss( DATE *date )
{
	char buffer[ 128 ];

	if ( !date ) return "";

	date_get_hhmmss( buffer, date );
	return strdup( buffer );
}

char *date_get_yyyy_mm_dd_string( DATE *date )
{
	return date_display_yyyy_mm_dd( date );
}

char *date_day_of_week_yyyy_mm_dd(	int wday_of_week,
					int utc_offset )
{
	char *date_string;
	DATE *date = date_today_new( utc_offset );

	while( date->tm->tm_wday != wday_of_week )
		date_increment_days( date, -1.0 );

	date_string = date_get_yyyy_mm_dd_string( date );

	date_free( date );

	return date_string;
}

char *date_yesterday_yyyy_mm_dd( int utc_offset )
{
	return date_get_yesterday_yyyy_mm_dd_string( utc_offset );
}

char *date_get_yesterday_yyyy_mm_dd_string(
					int utc_offset )
{
	DATE *date;

	date = date_today_new( utc_offset );
	date_increment_days( date, -1.0 );
	return date_get_yyyy_mm_dd_string( date );
}

DATE *date_now_new( int utc_offset )
{
	return date_today_new( utc_offset );
}

boolean date_is_daylight_time( void )
{
	time_t now;
	struct tm *tm;

	unsetenv( "TZ" );

	now = time( (time_t *)0 );
	tzset();
	tm = localtime( &now );

	date_set_TZ( DATE_GMT );
	return (boolean)tm->tm_isdst;
}

DATE *date_set_now(	DATE *date,
			int utc_offset )
{
	if ( date ) date_free( date );

	return date_now( utc_offset );
}

DATE *date_now( int utc_offset )
{
	time_t now;
	struct tm *tm;
	DATE *return_date;

	now = time( (time_t *)0 );
	tm = gmtime( &now );

	return_date =
		date_new_date_time(
			tm->tm_year + 1900,
			tm->tm_mon + 1,
			tm->tm_mday,
			tm->tm_hour,
			tm->tm_min,
			tm->tm_sec,
			utc_offset );

	if ( return_date->is_daylight_time )
	{
		date_increment_hour( return_date );
	}

	return return_date;
}

DATE *date_today_new( int utc_offset )
{
	return date_now( utc_offset );
}

DATE *date_get_today_new( int utc_offset )
{
	return date_today_new( utc_offset );
}

char *date_get_now_hhmm( int utc_offset )
{
	return date_get_now_date_hhmm( utc_offset );
}

char *date_now_yyyy_mm_dd( int utc_offset )
{
	DATE *date = date_today_new( utc_offset );

	if ( !date )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: date_today_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	/* Returns heap memory or "" */
	/* ------------------------- */
	return date_display_yyyy_mm_dd( date );
}

char *date_get_now_yyyy_mm_dd( int utc_offset )
{
	return date_get_now_date_yyyy_mm_dd( utc_offset );
}

char *date_today_yyyy_mm_dd( int utc_offset )
{
	return date_get_now_date_yyyy_mm_dd( utc_offset );
}

char *date_get_current_hhmm( int utc_offset )
{
	return date_get_now_date_hhmm( utc_offset );
}

char *date_get_current_yyyy_mm_dd( int utc_offset )
{
	return date_get_now_date_yyyy_mm_dd( utc_offset );
}

char *date_get_now_date_hhmm( int utc_offset )
{
	DATE *date = date_today_new( utc_offset );
	return date_display_hhmm( date );
}

char *date_get_now_date_oracle_format( int utc_offset )
{
	char *oracle_format;

	oracle_format =
		timlib_yyyymmdd_to_oracle_format(
			date_get_now_date_yyyy_mm_dd(
				utc_offset ) );

	return oracle_format;
}

char *date_get_now_date_yyyy_mm_dd( int utc_offset )
{
	DATE *date = date_today_new( utc_offset );
	return date_display_yyyy_mm_dd( date );
}

char *date_get_now_time_hhmm( int utc_offset )
{
	char buffer[ 128 ];
	DATE *d;

	d = date_today_new( utc_offset );

	sprintf( buffer, "%02d%02d", d->tm->tm_hour, d->tm->tm_min );

	return strdup( buffer );
}

char *date_now_hhmmss( int utc_offset )
{
	char *return_string;

	/* Returns heap memory. */
	/* -------------------- */
	return_string = date_get_now_hhmm_colon_ss( utc_offset );

	return return_string;
}

char *date_now_hhmm_colon_ss( int utc_offset )
{
	/* Returns heap memory. */
	/* -------------------- */
	return date_get_now_time_hhmm_colon_ss( utc_offset );
}

char *date_now_hh_colon_mm_colon_ss( int utc_offset )
{
	return date_get_now_hh_colon_mm_colon_ss( utc_offset );
}

char *date_now_time_second( int utc_offset )
{
	return date_get_now_hh_colon_mm_colon_ss( utc_offset );
}

char *date_get_now_yyyy_mm_dd_hh_mm(
				int utc_offset )
{
	DATE *d;

	d = date_today_new( utc_offset );
	return date_display_yyyy_mm_dd_hh_mm( d );
}

char *date_get_yyyy_mm_dd_hh_mm_ss( DATE *date_time )
{
	char buffer[ 32 ];

	sprintf( 	buffer, 
			"%d-%02d-%02d %02d:%02d:%02d",
			date_year( date_time ),
			date_month( date_time ),
			date_day_of_month( date_time ),
			date_hour( date_time ),
			date_minutes( date_time ),
			date_seconds( date_time ) );

	return strdup( buffer );
}

char *date_get_now19( int utc_offset )
{
	return date_now19( utc_offset );
}

char *date_now19( int utc_offset )
{
	DATE *d;

	d = date_today_new( utc_offset );
	return date_get_yyyy_mm_dd_hh_mm_ss( d );
}

char *date_time_now( int utc_offset )
{
	return date_now16( utc_offset );
}

char *date_get_now16( int utc_offset )
{
	return date_now16( utc_offset );
}

char *date_now16( int utc_offset )
{
	DATE *d;

	d = date_today_new( utc_offset );
	return date_display_yyyy_mm_dd_hh_colon_mm( d );
}

char *date_get_now_hh_colon_mm( int utc_offset )
{
	char buffer[ 128 ];
	DATE *d;

	d = date_today_new( utc_offset );

	sprintf(	buffer,
			"%02d:%02d",
			d->tm->tm_hour,
			d->tm->tm_min );

	return strdup( buffer );

}

char *date_get_now_hh_colon_mm_colon_ss( int utc_offset )
{
	char buffer[ 128 ];
	DATE *d;

	d = date_today_new( utc_offset );

	sprintf(	buffer,
			"%02d:%02d:%02d",
			d->tm->tm_hour,
			d->tm->tm_min,
			d->tm->tm_sec );

	return strdup( buffer );
}

/* Safely returns heap memory. */
/* --------------------------- */
char *date_time_now19( int utc_offset )
{
	DATE *d;

	d = date_now_new( utc_offset );
	return date_get_yyyy_mm_dd_hh_mm_ss( d );
}

/* Safely returns heap memory. */
/* --------------------------- */
char *date_get_now_time_hhmm_colon_ss( int utc_offset )
{
	char buffer[ 128 ];
	DATE *d;

	d = date_today_new( utc_offset );

	sprintf(	buffer,
			"%02d%02d:%02d",
			d->tm->tm_hour,
			d->tm->tm_min,
			d->tm->tm_sec );

	return strdup( buffer );
}

void date_set_tm_structures(	DATE *d,
				time_t current,
				int utc_offset )
{
	if ( current < 0 )
	{
		date_set_tm_structures_pre_1970(
				d,
				current,
				utc_offset );
	}
	else
	{
		memcpy( d->tm, gmtime( &current ), sizeof( struct tm ) );
	}
}

void date_set_tm_structures_pre_1970(	DATE *d,
					time_t current,
					int utc_offset )
{
	struct tm future_tm = {0};
	time_t future_time_t;
	time_t new_current;

	future_tm.tm_year = 2026 - 1900;
	future_tm.tm_mon = 0;
	future_tm.tm_mday = 1;
	future_tm.tm_hour = 0;
	future_tm.tm_min = 0;
	future_tm.tm_sec = 0;

	future_time_t = date_mktime( &future_tm, utc_offset );

	new_current = future_time_t + current;

	memcpy( d->tm, gmtime( &new_current ), sizeof( struct tm ) );
	d->tm->tm_year -= 56;
}

time_t date_mktime(	struct tm *tm,
			int utc_offset )
{
	time_t return_value;

	return_value = mktime( tm );
	return_value += (SECONDS_IN_HOUR * utc_offset);

	return return_value;
}

char *date_hhmm( DATE *d )
{
	static char destination[ 16 ];
	char *tmp;

	tmp = date_get_hhmm_string( d );
	strcpy( destination, tmp );
	free( tmp );
	return destination;
}

char *date_get_hhmm_string( DATE *d )
{
	char buffer[ 128 ];
	sprintf( buffer, "%02d%02d", d->tm->tm_hour, d->tm->tm_min );
	return strdup( buffer );
}

DATE *date_new_yyyy_mm_dd_hhmm(	char *date_string,
				char *time_string )
{
	return date_yyyy_mm_dd_hhmm_new(
		date_string,
		time_string );
}

DATE *date_new_yyyy_mm_dd( char *date_string )
{
	return date_yyyy_mm_dd_new( date_string );
}

DATE *date_new_yyyy_mm_dd_date(	char *date_string )
{
	return date_yyyy_mm_dd_new( date_string );
}

boolean date_is_day_of_week(	DATE *d,
				int day_of_week )
{
	int wday;
	
	wday = d->tm->tm_wday;
	return( wday == day_of_week );
}

boolean date_is_sunday( DATE *d )
{
	return date_is_day_of_week( d, WDAY_SUNDAY );
}

boolean date_is_monday( DATE *d )
{
	return date_is_day_of_week( d, WDAY_MONDAY );
}

boolean date_is_tuesday( DATE *d )
{
	return date_is_day_of_week( d, WDAY_TUESDAY );
}

boolean date_is_wednesday( DATE *d )
{
	return date_is_day_of_week( d, WDAY_WEDNESDAY );
}

boolean date_is_thursday( DATE *d )
{
	return date_is_day_of_week( d, WDAY_THURSDAY );
}

boolean date_is_friday( DATE *d )
{
	return date_is_day_of_week( d, WDAY_FRIDAY );
}

boolean date_is_saturday( DATE *d )
{
	return date_is_day_of_week( d, WDAY_SATURDAY );
}

DATE *date_forward_to_first_month(	DATE *d,
					int utc_offset )
{
	DATE *return_date;

	return_date = date_back_to_first_month( d, utc_offset );
	date_increment_months( return_date, 1 );

	return return_date;
}

DATE *date_back_to_first_month(		DATE *d,
					int utc_offset )
{
	int mday;
	time_t current;

	mday = d->tm->tm_mday;
	current = d->current;

	while( mday > 1 )
	{
		mday--;
		current -= SECONDS_IN_DAY;
	}
	return date_current_new( current, utc_offset );
}

DATE *date_get_prior_day(
			DATE *date,
			int wday_parameter )
{
	int wday;
	time_t current;

	wday = date->tm->tm_wday;
	current = date->current;

	wday--;
	current -= SECONDS_IN_DAY;

	if ( wday == -1 ) wday = WDAY_SATURDAY;

	while( wday != wday_parameter )
	{
		wday--;
		current -= SECONDS_IN_DAY;
		if ( wday == -1 ) wday = WDAY_SATURDAY;
	}

	return date_current_new( current, 0 /* utc_offset */ );
}

DATE *date_get_prior_saturday(	DATE *date )
{
	return date_get_prior_day( date, WDAY_SATURDAY );
}

DATE *date_get_prior_sunday(	DATE *date )
{
	return date_get_prior_day( date, WDAY_SUNDAY );
}

DATE *date_yyyy_mm_dd_hhmm_new(	char *date_string,
				char *time_string )
{
	DATE *date;
	int year, month, day, hour, minutes;
	char buffer[ 3 ];

	year = atoi( date_string );
	month = atoi( date_string + 5 );
	day = atoi( date_string + 8 );

	*buffer = *time_string;
	*(buffer + 1) = *(time_string + 1);
	*(buffer + 2) = '\0';

	hour = atoi( buffer );

	*buffer = *(time_string + 2);
	*(buffer + 1) = *(time_string + 3);

	minutes = atoi( buffer );

	date = date_new( year, month, day );

	date_set_time( date, hour, minutes );

	return date;
}

boolean date_set_time_hhmmss(	DATE *date,
				/* --------------------- */
				/* Looks like "23:59:59" */
				/* --------------------- */
				char *hhmmss )
{
	int hours, minutes, seconds;
	char piece_buffer[ 128 ];


	if ( character_count( ':', hhmmss ) != 2 )
	{
		return 0;
	}

	piece( piece_buffer, ':', hhmmss, 0 );
	hours = atoi( piece_buffer );

	piece( piece_buffer, ':', hhmmss, 1 );
	minutes = atoi( piece_buffer );

	piece( piece_buffer, ':', hhmmss, 2 );
	seconds = atoi( piece_buffer );

	date_set_time_seconds( date, hours, minutes, seconds );

	return 1;
}

boolean date_set_time_hhmm(	DATE *date,
				char *hhmm )
{
	int hours, minutes;
	char buffer[ 128 ];

	/* Remove spaces and colons. */
	/* ------------------------- */
	trim_character(	buffer /* destination */,
			':',
			hhmm /* source */ );

	strcpy( hhmm, buffer );

	trim_character(	buffer /* destination */,
			' ' ,
			hhmm /* source */ );

	strcpy( hhmm, buffer );

	/* Put leading zero, maybe */
	/* ----------------------- */
	if ( strlen( hhmm ) == 3 )
	{
		char buffer[ 5 ];

		sprintf( buffer, "0%s", hhmm );
		strcpy( hhmm, buffer );
	}

	/* Allow for trailing seconds */
	/* -------------------------- */
	if ( strlen( hhmm ) < 4 ) return 0;

	*(buffer + 2) = '\0';
	*buffer = *hhmm;
	*(buffer + 1) = *(hhmm + 1);
	hours = atoi( buffer );

	*buffer = *(hhmm + 2);
	*(buffer + 1) = *(hhmm + 3);
	minutes = atoi( buffer );

	date_set_time( date, hours, minutes );

	return 1;
}

DATE *date_increment_hours(	DATE *d,
				double hours )
{
	d->current += (long)((float)SECONDS_IN_HOUR * hours);
	date_set_tm_structures( d, d->current, date_utc_offset() );
	return d;
}

DATE *date_add_minutes(	DATE *d,
			int minutes )
{
	return date_increment_minutes( d, minutes );
}

DATE *date_increment_minutes(	DATE *d,
				int minutes )
{
	d->current += (long)(SECONDS_IN_MINUTE * minutes);
	date_set_tm_structures( d, d->current, date_utc_offset() );
	return d;
}

DATE *date_add_seconds(
			DATE *d,
			int seconds )
{
	return date_increment_seconds( d, seconds );
}

DATE *date_decrement_second(
			DATE *date,
			int seconds )
{
	return date_increment_seconds(
			date,
			-seconds );
}

DATE *date_increment_seconds(
			DATE *d,
			int seconds )
{
	d->current += (long)seconds;
	date_set_tm_structures( d, d->current, 0 /* utc_offset */ );
	return d;
}

int date_subtract_minutes( DATE *later_date, DATE *earlier_date )
{
	time_t difference;
	int results;

	difference = later_date->current - earlier_date->current;
	results = (int)((double)difference / (double)SECONDS_IN_MINUTE);
	return results;
}

int date_subtract_days( DATE *later_date, DATE *earlier_date )
{
	time_t difference;
	int results;

	difference = later_date->current - earlier_date->current;
	results = (int)((double)difference / (double)SECONDS_IN_DAY);
	return results;
}

int date_get_month_day_number( DATE *date )
{
	return date->tm->tm_mday;
}

int date_get_day( DATE *date )
{
	return date_get_day_number( date );
}

int date_day( DATE *date )
{
	return date->tm->tm_mday;
}

int date_get_day_number( DATE *date )
{
	return date_day( date );
}

int date_get_week_number( DATE *date )
{
	return ( date->tm->tm_yday / 7 ) + 1;
}

int date_month_offset( DATE *date )
{
	return date->tm->tm_mon;
}

int date_get_month_number( DATE *date )
{
	return date->tm->tm_mon + 1;
}

int date_get_year_number( DATE *date )
{
	return date->tm->tm_year + 1900;
}

int date_get_hour_number( DATE *date )
{
	return date_hour_integer( date );
}

int date_get_minutes_number( DATE *date )
{
	return date->tm->tm_min;
}

DATE *date_round2five_minutes( DATE *date )
{
	int hour;
	int minutes;
	int remainder;

	hour = date_get_hour_number( date );
	minutes = date_get_minutes_number( date );
	remainder = minutes % 5;

	if ( !remainder ) return date;

	if ( remainder <= 2 )
		minutes = minutes - remainder;
	else
	{
		minutes = (5 - remainder) + minutes;
		if ( minutes == 60 )
		{
			minutes = 0;
			hour++;

			if ( hour == 24 )
			{
				hour = 0;
				date_increment_day( date );
			}
		}
	}
	
	date_set_time( date, hour, minutes );
	return date;
}

double date_yyyy_mm_dd_hhmm_to_julian( char *yyyy_mm_dd, char *hhmm )
{
	return julian_yyyy_mm_dd_time_hhmm_to_julian( yyyy_mm_dd, hhmm );
}

double date_yyyy_mm_dd_to_julian( char *yyyy_mm_dd )
{
	return julian_yyyy_mm_dd_time_hhmm_to_julian( yyyy_mm_dd, "0000" );
}

boolean date_copy( DATE *d1, DATE *d2 )
{
	if ( !d2 ) return 0;

	d1->current = d2->current;
	memcpy( d1->tm, d2->tm, sizeof( struct tm ) );
	return 1;
}

char *date_display_mdy( DATE *date )
{
	char buffer[ 32 ];

	sprintf( 	buffer, 
			"%02d/%02d/%04d",
			date_month( date ),
			date_day_of_month( date ),
			date_year( date ) );

	return strdup( buffer );
}

char *date_get_yyyy_mm_dd_hhmm( DATE *date )
{
	return date_display_yyyy_mm_dd_hhmm( date );
}

char *date_display_yyyy_mm_dd_hhmm( DATE *date )
{
	char buffer[ 32 ];

	sprintf( 	buffer, 
			"%d-%02d-%02d:%02d%02d",
			date_year( date ),
			date_month( date ),
			date_day_of_month( date ),
			date_hour( date ),
			date_minutes( date ) );

	return strdup( buffer );
}

char *date_time_display( DATE *date )
{
	return date_display_yyyy_mm_dd_hh_colon_mm( date );
}

char *date_display_yyyy_mm_dd_hh_colon_mm( DATE *date )
{
	char buffer[ 32 ];

	sprintf( 	buffer, 
			"%d-%02d-%02d %02d:%02d",
			date_year( date ),
			date_month( date ),
			date_day_of_month( date ),
			date_hour( date ),
			date_minutes( date ) );

	return strdup( buffer );
}

char *date_display_yyyy_mm_dd_hh_colon_mmss( DATE *date )
{
	char buffer[ 32 ];

	sprintf( 	buffer, 
			"%d-%02d-%02d %02d:%02d%02d",
			date_year( date ),
			date_month( date ),
			date_day_of_month( date ),
			date_hour( date ),
			date_minutes( date ),
			date_seconds( date ) );

	return strdup( buffer );
}

void date_increment_weekly_ceiling(	DATE *date )
{
	while( date_get_day_of_week( date ) != DATE_END_OF_WEEK_INDICATOR )
		date_increment_day( date );
}

boolean date_compare(		DATE *date1,
				DATE *date2 )
{
	char buffer1[ 128 ];
	char buffer2[ 128 ];

	if ( !date1 || !date2 ) return 0;

	return ( strcmp(
			date_get_yyyy_mm_dd( buffer1, date1 ),
			date_get_yyyy_mm_dd( buffer2, date2 ) ) );
}

boolean date_same_day(		DATE *old_date,
				DATE *new_date )
{
	char buffer1[ 128 ];
	char buffer2[ 128 ];

	if ( !old_date || !new_date ) return 0;

	return ( strcmp(
			date_get_yyyy_mm_dd( buffer1, old_date ),
			date_get_yyyy_mm_dd( buffer2, new_date ) ) == 0 );
}

boolean date_tomorrow( 		DATE *old_date, 
				DATE *new_date )
{
	int return_value;
	char buffer1[ 128 ];
	char buffer2[ 128 ];
	DATE *tomorrow_date;

	tomorrow_date = date_calloc();
	date_copy( tomorrow_date, old_date );

	date_increment_day( tomorrow_date );

	return_value =
		( strcmp(
			date_get_yyyy_mm_dd( buffer1, new_date ),
			date_get_yyyy_mm_dd( buffer2, tomorrow_date ) ) == 0 );
	date_free( tomorrow_date );
	return return_value;
}

boolean date_year_in_future(	DATE *date,
				int year )
{
	return ( year > date_get_year_number( date ) );
}

void date_output_hour_error(	char *function_name,
				int hour )
{
	fprintf(stderr,
		"ERROR in %s(): daylight savings time hour problem = %d.\n",
		function_name,
		hour );
}

int date_days_in_month(	int month,
			int year )
{
	return date_get_last_month_day(	month, year );
}

char *date_current_to_static_yyyy_mm_dd_string(
				time_t current,
				int utc_offset )
{
	DATE *d = date_calloc();
	static char yyyy_mm_dd_string[ 16 ];

	date_set_tm_structures( d, current, utc_offset );
	d->current = date_tm_to_current( d->tm, utc_offset );

	strcpy( yyyy_mm_dd_string, date_static_display_yyyy_mm_dd( d ) );
	date_free( d );
	return yyyy_mm_dd_string;
}

char *date_prior_day_of_week_yyyy_mm_dd_string(	DATE *d,
						int day_of_week,
						int utc_offset )
{
	int wday;
	time_t current;

	wday = d->tm->tm_wday;
	current = d->current;

	while( wday != day_of_week )
	{
		wday--;
		current -= SECONDS_IN_DAY;
		if ( wday == -1 ) wday = 6;
	}

	return date_current_to_static_yyyy_mm_dd_string(
			current,
			utc_offset );
}

char *date_get_unix_now_string(	void )
{
	char sys_string[ 128 ];

	sprintf( sys_string,
		 "date +'%cF:%cH%cM'",
		 '%', '%', '%' );

	return pipe2string( sys_string );
}

boolean date_is_greatgreatgrandfather( DATE *d )
{
	int month;
	int day;

	month = date_month( d );
	day = date_get_day( d );

	if ( month == 1 && day == 1 )
		return 1;
	else
		return 0;
}

boolean date_is_greatgrandfather( DATE *d )
{
	int month;
	int day;

	month = date_month( d );
	day = date_get_day( d );

	if ( month == 1 && day == 1 )
		return 1;
	else
	if ( month == 4 && day == 1 )
		return 1;
	else
	if ( month == 7 && day == 1 )
		return 1;
	else
	if ( month == 10 && day == 1 )
		return 1;
	else
		return 0;
}

boolean date_is_grandfather( DATE *d )
{
	int day;

	day = date_get_day( d );

	if ( day == 1 )
		return 1;
	else
		return 0;
}

boolean date_is_father( DATE *d )
{
	return date_is_sunday( d );
}

char *date_get_colon_now_time( void )
{
	char *sys_string;

	sys_string = "now.sh hhmmss | sed 's/\\([0-9][0-9]$\\)/:&/'";
	return pipe2string( sys_string );
}

void date_remove_colon_in_time( char *time_string )
{
	char hour[ 3 ];
	char minute[ 3 ];

	if ( timlib_strlen( time_string ) != 5 ) return;

	strncpy( hour, time_string, 2 );
	*(hour + 2) = '\0';

	strncpy( minute, time_string + 3, 2 );
	*(minute + 2) = '\0';

	sprintf( time_string, "%s%s", hour, minute );
}

void date_place_colon_in_time( char *time_string )
{
	char hour[ 3 ];
	char minute[ 3 ];

	if ( timlib_strlen( time_string ) != 4 ) return;
	if ( timlib_strcmp( time_string, "null" ) == 0 ) return;

	strncpy( hour, time_string, 2 );
	*(hour + 2) = '\0';

	strncpy( minute, time_string + 2, 2 );
	*(minute + 2) = '\0';

	sprintf( time_string, "%s:%s", hour, minute );
}

char *date_append_hhmmss( char *date_string )
{
	static char date_hhmmss[ 32 ];
	char *hhmmss;

	if ( !date_string || !*date_string ) return "";

	hhmmss =
		date_get_now_hh_colon_mm_colon_ss(
			date_utc_offset() );

	sprintf( date_hhmmss,
		 "%s %s",
		 date_string,
		 hhmmss );

	free( hhmmss );
	return date_hhmmss;
}

char *date_remove_colon_from_time( char *time_string )
{
	static char buffer[ 128 ];
	int str_len;
	char count;

	*buffer = '\0';

	if ( timlib_strcmp( time_string, "null" ) == 0 ) return time_string;

	count = character_count( ':', time_string );

	if ( count == 0 || count > 2 ) return time_string;

	if ( count == 2 )
	{
		piece_inverse( time_string, ':', 2 );
	}

	str_len = strlen( time_string );

	/* Example: 1:15 */
	/* ------------- */
	if ( str_len == 4 )
	{
		*buffer = '0';
		strncpy( buffer + 1, time_string, 1 );
		strncpy( buffer + 2, time_string + 2, 2 );
		*(buffer + 4) = '\0';
		return buffer;
	}
	else
	/* -------------- */
	/* Example: 01:15 */
	/* -------------- */
	if ( str_len == 5 )
	{
		strncpy( buffer, time_string, 2 );
		strncpy( buffer + 2, time_string + 3, 2 );
		*(buffer + 4) = '\0';
		return buffer;
	}
	else
	{
		return (char *)0;
	}
}

int date_get_week_of_year( DATE *date )
{
	char week_of_year_string[ 16 ];
	int week_of_year;
	int year;
	DATE *january_1st;
	DATE *december_23rd;
	boolean january_1st_sunday;
	boolean december_23rd_friday_or_saturday;

	if ( !date )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty date.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	year = date_year( date );

	january_1st = date_new( year, 1, 1 );

	january_1st_sunday =
		( date_get_day_of_week( january_1st ) == WDAY_SUNDAY );

	/* Returns 00-53 */
	/* ------------- */
	strftime( week_of_year_string, 16, "%U", date->tm );

	week_of_year = atoi( week_of_year_string );

	if ( january_1st_sunday )
	{
		if ( week_of_year == 53 ) week_of_year = 1;
	}
	else
	{
		week_of_year++;

		if ( week_of_year == 54 )
		{
			week_of_year = 1;
		}
		if ( week_of_year == 53 )
		{
			december_23rd =
				date_new(
					year,
					12,
					23 );

			strftime(	week_of_year_string,
					16,
					"%U",
					december_23rd->tm );

			december_23rd_friday_or_saturday =
				( ( date_get_day_of_week( december_23rd ) ==
				    WDAY_FRIDAY )
			||        ( date_get_day_of_week( december_23rd ) ==
				    WDAY_SATURDAY ) );

			if ( !december_23rd_friday_or_saturday )
				week_of_year = 1;
		}
	}

	return week_of_year;
}

int date_get_last_month_day(	int month,
				int year )
{
	if ( month == 1
	||   month == 3
	||   month == 5
	||   month == 7
	||   month == 8
	||   month == 10
	||   month == 12 )
	{
		return 31;
	}

	if ( month != 2 )
	{
		return 30;
	}

	if ( year % 4 )
	{
		return 28;
	}
	else
	{
		return 29;
	}
}

int date_utc_offset( void )
{
	char *utc_offset;
	static int utc_offset_integer = -99;

	if ( utc_offset_integer != -99 ) return utc_offset_integer;

	utc_offset = date_get_environment( "UTC_OFFSET" );

	if ( utc_offset )
		utc_offset_integer = atoi( utc_offset );
	else
		utc_offset_integer = DATE_DEFAULT_UTC_OFFSET;

	return utc_offset_integer;
}

int date_get_utc_offset( void )
{
	return date_utc_offset();
}

char *date_get_environment( char *variable_name )
{
	static char datum[ 1024 ];

	if ( date_name_to_value( datum, variable_name ) )
		return datum;
	else
		return (char *)0;
}

boolean date_name_to_value( char *variable_value, char *variable_name )
{
	char *value;

	/* Skip the '$' */
	/* ------------ */
	if ( *variable_name == '$' )
		variable_name++;

 	value = getenv( variable_name );

	if ( value )
	{
		strcpy( variable_value, value );
		return 1;
	}
	else
	{
		*variable_value = '\0';
		return 0;
	}
}

int date_get_days_in_year( int year )
{
	return date_days_in_year( year );
}

int date_days_in_year( int year )
{
	if ( year % 100 == 0 )
		return 365;
	else
	if ( year % 4 == 0 )
		return 366;
	else
		return 365;
}

void date_set_TZ( char *TZ )
{
	char statement[ 128 ];

	sprintf( statement, "TZ=%s", TZ );
	putenv( strdup( statement ) );
}

boolean date_parse_american_date_time(
			char *date_international,
			char *time_hhmm,
			char **error_message,
			char *american_date_time )
{
	char date_american[ 16 ];
	char time_colon_second[ 16 ];

	if ( !timlib_character_exists( american_date_time, ' ' ) ) return 0;

	column( date_american, 0, american_date_time );
	column( time_colon_second, 1, american_date_time );

	if ( strcmp( time_colon_second, "nu:ll" ) != 0
	&&   strcmp( time_colon_second, "nu:ll:00" ) != 0 )
	{
		timlib_strcpy(
			time_hhmm,
			date_remove_colon_from_time(
				time_colon_second ),
			16 /* buffer_size */ );
	}

	*date_international = '\0';

	if ( !date_convert_source_unknown(
			date_international,
			international,
			date_american ) )
	{
		if ( error_message )
		{
			char buffer[ 256 ];

			sprintf( buffer,
				 "cannot translate %s to international",
				 date_american );

			*error_message = strdup( buffer );
		}
		return 0;
	}

	if ( !*time_hhmm )
	{
		strcpy( time_hhmm, "null" );
	}

	return 1;
}

boolean date_set_yyyy_mm_dd_hh_mm_ss_colon(
			DATE *date,
			/* ------------------- */
			/* 2017-12-01 11:00:00 */
			/* ------------------- */
			char *date_time_string )
{
	char year_string[ 16 ];
	char month_string[ 16 ];
	char day_string[ 16 ];
	char hour_string[ 16 ];
	char minute_string[ 16 ];
	char second_string[ 16 ];
	char date_half[ 32 ];
	char time_half[ 32 ];

	if ( count_character( '-', date_time_string ) != 2 )
		return 0;

	if ( count_character( ' ', date_time_string ) != 1 )
		return 0;

	if ( count_character( ':', date_time_string ) != 2 )
		return 0;

	column( date_half, 0, date_time_string );
	column( time_half, 1, date_time_string );

	piece( year_string, '-', date_half, 0 );
	piece( month_string, '-', date_half, 1 );
	piece( day_string, '-', date_half, 2 );

	piece( hour_string, ':', time_half, 0 );
	piece( minute_string, ':', time_half, 1 );
	piece( second_string, ':', time_half, 2 );

	date_set_date_integers(	date,
				atoi( year_string ),
				atoi( month_string ),
				atoi( day_string ),
				0 /* utc_offset */ );

	date_set_time_integers(	date,
				atoi( hour_string ),
				atoi( minute_string ),
				atoi( second_string ),
				0 /* utc_offset */ );

	return 1;
}

char *date_display_day_name( DATE *date )
{
	return date_day_string( date_day_offset( date ) );
}

char *date_display_month_name( DATE *date )
{
	return date_month_string( date_month_offset( date ) );
}

char *date_display_time_ampm( DATE *date )
{
	char static buffer[ 128 ];

	if ( date->tm->tm_hour == 0
	&&   date->tm->tm_min == 0 )
	{
		sprintf(buffer,
			"12:00am" );
	}
	else
	if ( date->tm->tm_hour <= 11 )
	{
		sprintf(buffer,
			"%d:%.2dam",
			date->tm->tm_hour,
			date->tm->tm_min );
	}
	else
	{
		sprintf(buffer,
			"%d:%.2dpm",
			date->tm->tm_hour - 12,
			date->tm->tm_min );
	}
	return buffer;
}

char *date_display_ampm( DATE *date )
{
	if ( date->tm->tm_hour == 0
	&&   date->tm->tm_min == 0 )
	{
		return "am";
	}
	else
	if ( date->tm->tm_hour <= 11 )
	{
		return "am";
	}
	else
	{
		return "pm";
	}
}

char *date_display_th_st_rd_nd( DATE *date )
{
	char day_of_month_string[ 16 ];
	int day_of_month;
	int last_digit;

	if ( ! ( day_of_month = date_day_of_month( date ) ) )
	{
		return "";
	}

	sprintf(day_of_month_string,
		"%.2d",
		day_of_month );

	last_digit = atoi( day_of_month_string + 1 );

	if ( last_digit == 0 )
		return "th";
	else
	if ( last_digit == 1 )
		return "st";
	else
	if ( last_digit == 2 )
		return "nd";
	else
	if ( last_digit == 3 )
		return "rd";
	else
		return "th";
}

