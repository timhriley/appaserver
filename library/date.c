/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/date.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "String.h"
#include "piece.h"
#include "column.h"
#include "environ.h"
#include "julian.h"
#include "appaserver_error.h"
#include "date_convert.h"
#include "date.h"

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

int date_military_month_integer( char *military_month )
{
	char **array_ptr = date_military_month_array;
	register int i = 1;

	while( *array_ptr )
	{
		if ( strcasecmp( military_month, *array_ptr ) == 0 )
		{
			return i;
		}
		else
		{
			i++;
			array_ptr++;
		}
	}
	return 0;
}

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
	DATE *d = (DATE *)calloc( 1, sizeof ( DATE ) );

	if ( !d )
	{
		fprintf(stderr,
			"ERROR: memory allocation failed in %s/%s()/%d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	d->tm = ( struct tm *)calloc( 1, sizeof ( struct tm ) );

	if ( !d->tm )
	{
		fprintf(stderr,
			"ERROR: memory allocation failed in %s/%s()/%d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	d->is_daylight_time = date_now_daylight_time();

	date_environ_set_TZ( DATE_GMT );

	return d;
}

DATE *date_new_date( void )
{
	return date_calloc();
}

char *date_current_yyyy_mm_dd_string( int utc_offset )
{
	DATE *date = date_now_new( utc_offset );

	return
	/* --------------------- */
	/* Returns static memory */
	/* --------------------- */
	date_yyyy_mm_dd_display( date );
}

DATE *date_current_new(
		time_t current,
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

DATE *date_time_new(
		int year,
		int month,
		int day,
		int hour,
		int minute,
		int second )
{
	return date_new_date_time(
			year,
			month,
			day,
			hour,
			minute,
			second,
			0 /* utc_offset */ );
}

DATE *date_new_date_time(
		int year,
		int month,
		int day,
		int hour,
		int minutes,
		int second,
		int utc_offset )
{
	DATE *d;

	if ( year >= 2038 && sizeof ( time_t ) == 4 )
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
	d->tm->tm_sec = second;

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

DATE *date_set_day(
		DATE *date,
		int day,
		int utc_offset )
{
	date->tm->tm_mday = day;

	date->current = date_tm_to_current( date->tm, utc_offset );
	date_set_tm_structures( date, date->current, utc_offset );

	return date;
}

DATE *date_set_date_integers(
		DATE *date,
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

boolean date_set_yyyy_mm_dd_hhmm(
		DATE *date,
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

	if ( string_character_count( '-', yyyy_mm_dd ) == 2 )
		delimiter = '-';
	else
	if ( string_character_count( '.', yyyy_mm_dd ) == 2 )
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

boolean date_set_yyyy_mm_dd(
		DATE *date,
		char *yyyy_mm_dd )
{
	char year_string[ 128 ];
	char month_string[ 128 ];
	char day_string[ 128 ];
	char delimiter;

	if ( string_character_count( '-', yyyy_mm_dd ) == 2 )
		delimiter = '-';
	else
	if ( string_character_count( '.', yyyy_mm_dd ) == 2 )
		delimiter = '.';
	else
		return 0;

	piece( year_string, delimiter, yyyy_mm_dd, 0 );
	piece( month_string, delimiter, yyyy_mm_dd, 1 );
	piece( day_string, delimiter, yyyy_mm_dd, 2 );

	date_set_date_integers(
		date,
		atoi( year_string ),
		atoi( month_string ),
		atoi( day_string ),
		0 /* utc_offset */ );

	date_set_time_integers(
		date,
		0 /* hour */,
		0 /* minute */,
		0 /* second */,
		0 /* utc_offset */ );

	return 1;
}

DATE *date_set_time_integers(
		DATE *date,
		int hour,
		int minute,
		int second,
		int utc_offset )
{
	date->tm->tm_hour = hour;
	date->tm->tm_min = minute;
	date->tm->tm_sec = second;

	date->current = date_tm_to_current( date->tm, utc_offset );
	date_set_tm_structures( date, date->current, utc_offset );
	return date;
}

DATE *date_set_time(
		DATE *date,
		int hour,
		int minutes )
{
	return
	date_set_time_integers(
		date,
		hour,
		minutes,
		0 /* seconds */,
		0 /* utc_offset */ );
}

DATE *date_set_time_second(
		DATE *date,
		int hour,
		int minute,
		int second )
{
	return
	date_set_time_integers(
		date,
		hour,
		minute,
		second,
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

DATE *date_decrement_days(
		DATE *date,
		double days )
{
	date_increment_days(
		date,
		-days );

	return date;
}

DATE *date_increment_months(
		DATE *date,
		int months )
{
	int year;
	int month;
	int day;

	year = date_year( date );
	month = date_month( date );
	day = date_day( date );
	month += months;

	date_set_date_integers(
		date,
		year,
		month,
		day,
		0 /* utc_offset */ );

	return date;
}

DATE *date_increment_years(
		DATE *d,
		int years )
{
	return
	date_decrement_years(d, -years );
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

DATE *date_decrement_years(
		DATE *date,
		int years )
{
	int year;
	int month;
	int day;

	year = date_year( date );
	month = date_month( date );
	day = date_day( date );
	year -= years;

	date_set_date_integers(
		date,
		year,
		month,
		day,
		0 /* utc_offset */ );

	return date;
}

DATE *date_increment_day( DATE *date )
{
	date->current += SECONDS_IN_DAY;
	date_set_tm_structures( date, date->current, date_utc_offset() );
	return date;
}

DATE *date_increment_week( DATE *date )
{
	increment_week( date );
	return date;
}

DATE *increment_week( DATE *date )
{
	date->current += SECONDS_IN_WEEK;
	date_set_tm_structures( date, date->current, date_utc_offset() );
	return date;
}

int date_month_integer( DATE *date )
{
	return date_month( date );
}

int date_month( DATE *date )
{
	return date->tm->tm_mon + 1;
}

int date_day_offset( DATE *date )
{
	return date_day_of_week( date );
}

int get_day_of_month( DATE *d )
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

int date_day_of_month( DATE *date )
{
	return date->tm->tm_mday;
}

int date_day_of_week( DATE *date )
{
	return date->tm->tm_wday;
}

int date_year( DATE *date )
{
	return date->tm->tm_year + 1900;
}

int date_hour( DATE *date )
{
	return date_hour_integer( date );
}

int date_minute( DATE *date )
{
	return date->tm->tm_min;
}

int date_minutes( DATE *date )
{
	return
	date_minute( date );
}

int date_seconds( DATE *date )
{
	return date_second( date );
}

int date_second( DATE *date )
{
	return date->tm->tm_sec;
}

int date_months_between(
		DATE *from_date,
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

int date_days_between(
		char *early_date_string,
		char *late_date_string )
{
	DATE *early_date;
	DATE *late_date;
	time_t difference;

	if ( !early_date_string
	||   !*early_date_string
	||   !late_date_string
	||   !*late_date_string )
	{
		return 0;
	}

	if ( ! ( late_date =
			/* ------------------- */
			/* Trims trailing time */
			/* ------------------- */
			date_yyyy_mm_dd_new(
				late_date_string ) ) )
	{
		return 0;
	}

	if ( ! ( early_date =
			/* ------------------- */
			/* Trims trailing time */
			/* ------------------- */
			date_yyyy_mm_dd_new(
				early_date_string ) ) )
	{
		return 0;
	}

	difference = late_date->current - early_date->current;

	date_free( late_date );
	date_free( early_date );

	return (int) (difference / SECONDS_IN_DAY);
}

int date_years_between( char *early_date, char *late_date )
{
	int early_year, early_month, early_day;
	int late_year, late_month, late_day;
	int diff_year, diff_month, diff_day;

	if ( !date_parse(
		&early_year,
		&early_month,
		&early_day,
		early_date ) )
	{
		return -1;
	}

	if ( !date_parse(
		&late_year,
		&late_month,
		&late_day,
		late_date ) )
	{
		return -1;
	}

	diff_year = late_year - early_year;
	diff_month = late_month - early_month;
	diff_day = late_day - early_day;

	if ( diff_day < 0 ) diff_month--;
	if ( diff_month < 0 ) diff_year--;

	if ( diff_year < 0 ) diff_year = 0;

	return diff_year;
}

void date_time_parse(
		int *hours,
		int *minutes,
		char *hhmm )
{
	char buffer[ 3 ];
	char *ptr;

	*hours = 0;
	*minutes = 0;

	if ( string_strlen( hhmm ) != 4 ) return;

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

boolean date_parse(
		int *year,
		int *month,
		int *day,
		char *date_string )
{
	char piece_buffer[ 128 ];

	*month = 0;
	*day = 0;
	*year = 0;

	if ( string_character_count( '/', date_string ) == 2 )
	{
		*month = atoi( piece( piece_buffer, '/', date_string, 0 ) );
		*day = atoi( piece( piece_buffer, '/', date_string, 1 ) );
		piece( piece_buffer, '/', date_string, 2 );

		string_strcpy(
			piece_buffer,
			julian_make_y2k_year( piece_buffer ),
			128 );

		*year = atoi( piece_buffer );
		return 1;
	}
	else
	if ( string_character_count( '-', date_string ) == 2 )
	{
		*month = atoi( piece( piece_buffer, '-', date_string, 1 ) );
		*day = atoi( piece( piece_buffer, '-', date_string, 2 ) );
		piece( piece_buffer, '-', date_string, 0 );
	
		string_strcpy(
			piece_buffer,
			julian_make_y2k_year( piece_buffer ),
			128 );

		*year = atoi( piece_buffer );
		return 1;
	}
	return 0;
}

int date_minutes_between(
		char *early_date_string,
		char *early_time_string,
		char *late_date_string,
	       	char *late_time_string,
		boolean add_one )
{
	DATE *early_date;
	DATE *late_date;
	time_t difference;

	if ( strcmp( early_time_string, "null" ) == 0
	||   strcmp( late_time_string, "null" ) == 0 )
	{
		return 24 * 60;
	}

	early_date =
		date_yyyy_mm_dd_new(
			early_date_string );

	date_set_time_hhmm( early_date, early_time_string );

	late_date =
		date_yyyy_mm_dd_new(
			late_date_string );

	date_set_time_hhmm( late_date, late_time_string );

	difference = late_date->current - early_date->current;

	date_free( early_date );
	date_free( late_date );

	if ( add_one )
		return ( difference / SECONDS_IN_MINUTE ) + 1;
	else
		return difference / SECONDS_IN_MINUTE;

}

int date_age( char *birth_date )
{
	char today[ 64 ];
	char with_slashes[ 64 ];
	FILE *p;

	p = popen( "date '+%m/%d/%y'", "r" );
	string_input( today, p, 64 );
	pclose( p );

	date_add_slashes_maybe( with_slashes, birth_date );

	return date_years_between( with_slashes, today );

}

void date_add_slashes_maybe( char *d, char *s )
{
	char buf1[ 32 ], buf2[ 32 ], buf3[ 32 ];

	if ( strlen( s ) != 6 )
	{
		strcpy( d, s );
		return;
	}

	sprintf( d, 
		 "%s/%s/%s",
		 string_midstr( buf1, s, 0, 2 ),
		 string_midstr( buf2, s, 2, 2 ),
		 string_midstr( buf3, s, 4, 2 ) );
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

	if ( string_character_count( '-', date_time_string ) != 2 )
		return (DATE *)0;

	if ( string_character_count( ' ', date_time_string ) != 1 )
		return (DATE *)0;

	if ( string_character_count( ':', date_time_string ) == 0 )
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

	date =
		date_new_date_time(
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

	if ( string_character_count( '-', date_time_string ) != 2 )
		return (DATE *)0;

	if ( string_character_count( ':', date_time_string ) != 1 )
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
			0 /* second */,
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

	if ( string_character_count( '-', date_time_string ) != 2 )
		return (DATE *)0;

	if ( string_character_count( ' ', date_time_string ) != 1 )
		return (DATE *)0;

	if ( string_character_count( ':', date_time_string ) == 0 )
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
			0 /* second */,
			0 /* utm_offset */ );

	return date;

}

DATE *date_yyyymmdd_new( char *date_string )
{
	char year_string[ 5 ] = {0};
	char month_string[ 3 ] = {0};
	char day_string[ 3 ] = {0};
	DATE *date;

	if ( !date_string
	||   strlen( date_string ) != 8 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid date_string=[%s]\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			(date_string) ? date_string : "" );
	}

	strncpy( year_string, date_string, 4 );
	strncpy( month_string, date_string + 4, 2 );
	strncpy( day_string, date_string + 6, 2 );

	date =
		date_new(
			atoi( year_string ),
			atoi( month_string ),
			atoi( day_string ) );

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
	if ( string_character_count( ' ', date_string ) == 1 )
	{
		column( local_date_string, 0, date_string );
		date_string = local_date_string;
	}

	if ( string_character_count( '-', date_string ) == 2 )
		delimiter = '-';
	else
	if ( string_character_count( '.', date_string ) == 2 )
		delimiter = '.';
	else
		return (DATE *)0;

	piece( year_string, delimiter, date_string, 0 );
	piece( month_string, delimiter, date_string, 1 );
	piece( day_string, delimiter, date_string, 2 );

	date =
		date_new(
			atoi( year_string ),
			atoi( month_string ),
			atoi( day_string ) );

	return date;
}

DATE *date_increment_days(
		DATE *date,
		double days )
{
	if ( !date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: date is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	date->current += (long)((double)SECONDS_IN_DAY * days);
	date_set_tm_structures( date, date->current, 0 /* utc_offset */ );

	return date;
}

char *date_month_number_string( DATE *date )
{
	static char number_string[ 16 ];

	sprintf(number_string, 
		"%02d",
		date_month( date ) );

	return number_string;
}

char *date_day_number_string( DATE *date )
{
	static char number_string[ 16 ];

	sprintf(number_string, 
		"%02d",
		date_day( date ) );

	return number_string;
}

char *date_yyyy_mm_dd( DATE *date )
{
	char buffer[ 128 ];

	snprintf(
		buffer, 
		sizeof ( buffer ),
		"%d-%02d-%02d",
		date_year( date ),
		date_month( date ),
		date_day_of_month( date ) );

	return strdup( buffer );
}

char *date_colon_hhmmss( DATE *date )
{
	char buffer[ 128 ];

	snprintf(
		buffer, 
		sizeof ( buffer ),
		"%02d:%02d:%02d",
		date_hour( date ),
		date_minute( date ),
		date_second( date ) );

	return strdup( buffer );
}

char *date_hhmmss( DATE *date )
{
	char buffer[ 128 ];

	if (!date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: date is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	snprintf(
		buffer, 
		sizeof ( buffer ),
		"%02d:%02d:%02d",
		date_hour( date ),
		date_minute( date ),
		date_second( date ) );

	return strdup( buffer );
}

char *date_hhmm( DATE *date )
{
	char buffer[ 128 ];

	snprintf(
		buffer, 
		sizeof ( buffer ),
		"%02d%02d",
		date_hour( date ),
		date_minute( date ) );

	return strdup( buffer );
}

char *date_now_yyyymmdd( void )
{
	DATE *date;

	date = date_now_new( date_utc_offset() );

	/* Returns static memory */
	/* --------------------- */
	return date_display_yyyymmdd( date );
}

char *date_display_yyyymmdd( DATE *date )
{
	static char buffer[ 32 ];

	if ( !date ) return "";

	sprintf( buffer,
		"%04d%02d%02d",
		date_year( date ),
		date_month( date ),
		date_day_of_month( date ) );

	return buffer;
}

char *date_display( DATE *date )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	date_yyyy_mm_dd_display( date );
}

char *date_display_yyyy_mm_dd( DATE *date )
{
	return
	/* ------------------------- */
	/* Returns heap memory or "" */
	/* ------------------------- */
	date_yyyy_mm_dd_display( date );
}

char *date_yyyy_mm_dd_display( DATE *date )
{
	char buffer[ 32 ];

	if ( !date ) return "";

	sprintf(buffer, 
		"%d-%02d-%02d",
		date_year( date ),
		date_month( date ),
		date_day_of_month( date ) );

	return strdup( buffer );
}

char *date_static_display_yyyy_mm_dd( DATE *date )
{
	return
	/* Returns static memory or "" */
	/* --------------------------- */
	date_yyyy_mm_dd_static_display(
		date );
}

char *date_yyyy_mm_dd_static_display( DATE *date )
{
	static char buffer[ 32 ];

	if ( !date ) return "";

	sprintf(buffer, 
		"%d-%02d-%02d",
		date_year( date ),
		date_month( date ),
		date_day_of_month( date ) );

	return buffer;
}

char *date_static_display( DATE *date )
{
	return
	date_yyyy_mm_dd_static_display( date );
}

char *date_yyyy_mm_dd_hhmm_display( DATE *date )
{
	char buffer[ 128 ];
	char *hhmm;
	
	sprintf(buffer,
		"%s:",
		date_static_display_yyyy_mm_dd(
			date ) );

	hhmm = date_hhmm( date );

	sprintf( buffer + strlen( buffer ), "%s", hhmm );

	return strdup( buffer );
}

char *date_display_yyyy_mm_dd_colon_hhmm( DATE *date )
{
	/* Returns heap memory */
	/* ------------------- */
	return date_yyyy_mm_dd_hhmm_display( date );
}

char *date_hms( DATE *date )
{
	/* Returns heap memory */
	/* ------------------- */
	return date_colon_hhmmss( date );
}

char *date_display19( DATE *date )
{
	/* Returns heap memory */
	/* ------------------- */
	return date_display_yyyy_mm_dd_colon_hms( date );
}

char *date_display_19( DATE *date )
{
	/* Returns heap memory */
	/* ------------------- */
	return date_display_yyyy_mm_dd_colon_hms( date );
}

char *date_display_yyyy_mm_dd_colon_hms( DATE *date )
{
	char buffer[ 128 ];
	char *hhmmss;
	
	sprintf(buffer,
		"%s ",
		date_static_display_yyyy_mm_dd( date ) );

	hhmmss =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_colon_hhmmss( date );

	sprintf( buffer + strlen( buffer ), "%s", hhmmss );

	free( hhmmss );

	return strdup( buffer );
}

char *date_hhmm_display( DATE *date )
{
	if ( !date ) return "";

	/* Returns heap memory */
	/* ------------------- */
	return date_hhmm( date );
}

char *date_hhmm_static_display( DATE *date )
{
	static char buffer[ 128 ];
	char *hhmm;

	if ( !date ) return "";

	hhmm = date_hhmm( date );

	strcpy( buffer, hhmm );
	free( hhmm );

	return buffer;
}

char *date_display_hhmm( DATE *date )
{
	return date_hhmm_display( date );
}

char *date_display_hhmmss( DATE *date )
{
	if ( !date ) return "";

	/* Returns heap memory */
	/* ------------------- */
	return date_colon_hhmmss( date );
}

char *date_yyyy_mm_dd_string( DATE *date )
{
	/* Returns heap memory or "" */
	/* ------------------------- */
	return date_display_yyyy_mm_dd( date );
}

char *date_day_of_week_yyyy_mm_dd(
		int wday_of_week,
		int utc_offset )
{
	char *date_string;
	DATE *date = date_now_new( utc_offset );

	while( date->tm->tm_wday != wday_of_week )
		date_increment_days( date, -1.0 );

	date_string = date_yyyy_mm_dd_string( date );

	date_free( date );

	return date_string;
}

char *date_yesterday_yyyy_mm_dd_string( int utc_offset )
{
	DATE *date;

	date = date_now_new( utc_offset );
	date_increment_days( date, -1.0 );

	return
	/* Returns heap memory */
	/* ------------------- */
	date_yyyy_mm_dd_string( date );
}

DATE *date_now_new( int utc_offset )
{
	return date_now( utc_offset );
}

boolean date_now_daylight_time( void )
{
	time_t now;
	struct tm *tm;

	unsetenv( "TZ" );

	now = time( (time_t *)0 );
	tzset();
	tm = localtime( &now );

	date_environ_set_TZ( DATE_GMT );

	return (boolean)tm->tm_isdst;
}

DATE *date_set_now(
		DATE *date,
		int utc_offset )
{
	if ( date ) date_free( date );

	return date_now( utc_offset );
}

DATE *date_now( int utc_offset )
{
	time_t now;
	struct tm *tm;
	DATE *date;

	now = time( (time_t *)0 );
	tm = gmtime( &now );

	date =
		date_new_date_time(
			tm->tm_year + 1900,
			tm->tm_mon + 1,
			tm->tm_mday,
			tm->tm_hour,
			tm->tm_min,
			tm->tm_sec,
			utc_offset );

	if ( date->is_daylight_time )
	{
		date_increment_hour( date );
	}

	return date;
}

DATE *date_today_new( int utc_offset )
{
	return date_now( utc_offset );
}

char *date_now_yyyy_mm_dd( int utc_offset )
{
	DATE *date = date_now_new( utc_offset );

	if ( !date )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: date_now_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	/* Returns heap memory or "" */
	/* ------------------------- */
	return date_display_yyyy_mm_dd( date );
}

char *date_now_hhmm( int utc_offset )
{
	DATE *date = date_today_new( utc_offset );

	/* Returns heap memory or "" */
	/* ------------------------- */
	return date_display_hhmm( date );
}

char *date_now_time_hhmm( int utc_offset )
{
	char buffer[ 128 ];
	DATE *date;

	date = date_today_new( utc_offset );

	sprintf(buffer,
		"%02d%02d",
		date->tm->tm_hour,
		date->tm->tm_min );

	return strdup( buffer );
}

char *date_now_time_second( int utc_offset )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	date_now_hhmmss( utc_offset );
}

char *date_now_yyyy_mm_dd_hhmm( int utc_offset )
{
	DATE *date;
	char *string;

	date = date_today_new( utc_offset );

	string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_display_yyyy_mm_dd_hhmm(
			date );

	date_free( date );

	return string;
}

char *date_yyyy_mm_dd_hhmmss( DATE *date )
{
	char buffer[ 64 ];

	snprintf(
		buffer, 
		sizeof ( buffer ),
		"%d-%02d-%02d %02d:%02d:%02d",
		date_year( date ),
		date_month( date ),
		date_day_of_month( date ),
		date_hour( date ),
		date_minute( date ),
		date_second( date ) );

	return strdup( buffer );
}

char *date_now19( int utc_offset )
{
	DATE *date;
	char *string;

	date = date_today_new( utc_offset );

	string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_yyyy_mm_dd_hhmmss( date );

	date_free( date );

	return string;
}

char *date_time_now( int utc_offset )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	date_now16( utc_offset );
}

char *date_now16( int utc_offset )
{
	DATE *date;
	char *string;

	date = date_today_new( utc_offset );

	string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_display_yyyy_mm_dd_hh_colon_mm(
			date );

	date_free( date );

	return string;
}

char *date_now_colon_hhmm( int utc_offset )
{
	/* Returns heap memory */
	/* ------------------- */
	return date_now_hh_colon_mm( utc_offset );
}

char *date_now_hh_colon_mm( int utc_offset )
{
	char buffer[ 128 ];
	DATE *date;

	date = date_today_new( utc_offset );

	snprintf(
		buffer,
		sizeof ( buffer ),
		"%02d:%02d",
		date->tm->tm_hour,
		date->tm->tm_min );

	date_free( date );

	return strdup( buffer );
}

char *date_now_hh_colon_mm_colon_ss( int utc_offset )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	date_now_colon_hhmmss( utc_offset );
}

char *date_time_now19( int utc_offset )
{
	DATE *date;
	char *string;

	date = date_now_new( utc_offset );

	string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_yyyy_mm_dd_hhmmss(
			date );

	date_free( date );

	return string;
}

char *date_now_hhmm_colon_ss( int utc_offset )
{
	char buffer[ 128 ];
	DATE *date;

	date = date_now_new( utc_offset );

	snprintf(
		buffer,
		sizeof ( buffer ),
		"%02d%02d:%02d",
		date->tm->tm_hour,
		date->tm->tm_min,
		date->tm->tm_sec );

	date_free( date );

	return strdup( buffer );
}

char *date_now_hhmmss( int utc_offset )
{
	char buffer[ 128 ];
	DATE *date;

	date = date_now( utc_offset );

	snprintf(
		buffer,
		sizeof ( buffer ),
		"%02d%02d%02d",
		date->tm->tm_hour,
		date->tm->tm_min,
		date->tm->tm_sec );

	date_free( date );

	return strdup( buffer );
}

char *date_set_now_hhmmss(
		char *date_string,
		int utc_offset )
{
	char now_hhmmss[ 128 ];
	char *tmp;

	if ( !date_string ) return NULL;

	if ( string_character_boolean( date_string, ' ' ) ) return NULL;

	snprintf(
		now_hhmmss,
		sizeof ( now_hhmmss ),
		"%s %s",
		date_string,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		( tmp = date_now_colon_hhmmss( utc_offset ) ) );

	free( tmp );

	return strdup( now_hhmmss );
}

char *date_now_colon_hhmmss( int utc_offset )
{
	char buffer[ 128 ];
	DATE *date;

	date = date_now( utc_offset );

	snprintf(
		buffer,
		sizeof ( buffer ),
		"%02d:%02d:%02d",
		date->tm->tm_hour,
		date->tm->tm_min,
		date->tm->tm_sec );

	date_free( date );

	return strdup( buffer );
}

void date_set_tm_structures(
		DATE *date,
		time_t current,
		int utc_offset )
{
	if ( current < 0 )
	{
		date_set_tm_structures_pre_1970(
			date /* in/out */,
			current,
			utc_offset );
	}
	else
	{
		memcpy( date->tm, gmtime( &current ), sizeof ( struct tm ) );
	}
}

void date_set_tm_structures_pre_1970(
		DATE *date /* in/out */,
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

	memcpy( date->tm, gmtime( &new_current ), sizeof ( struct tm ) );
	date->tm->tm_year -= 56;
}

time_t date_mktime(
		struct tm *tm,
		int utc_offset )
{
	time_t return_value;

	return_value = mktime( tm );
	return_value += (SECONDS_IN_HOUR * utc_offset);

	return return_value;
}

char *date_hhmm_string( DATE *date )
{
	char buffer[ 128 ];

	sprintf(buffer,
		"%02d%02d",
		date->tm->tm_hour,
		date->tm->tm_min );

	return strdup( buffer );
}

DATE *date_new_yyyy_mm_dd_hhmm(
		char *date_string,
		char *time_string )
{
	return
	date_yyyy_mm_dd_hhmm_new(
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

boolean date_is_day_of_week(
		DATE *date,
		int day_of_week )
{
	int wday;
	
	wday = date->tm->tm_wday;
	return( wday == day_of_week );
}

boolean date_is_sunday( DATE *date )
{
	return date_is_day_of_week( date, WDAY_SUNDAY );
}

boolean date_is_monday( DATE *date )
{
	return date_is_day_of_week( date, WDAY_MONDAY );
}

boolean date_is_tuesday( DATE *date )
{
	return date_is_day_of_week( date, WDAY_TUESDAY );
}

boolean date_is_wednesday( DATE *date )
{
	return date_is_day_of_week( date, WDAY_WEDNESDAY );
}

boolean date_is_thursday( DATE *date )
{
	return date_is_day_of_week( date, WDAY_THURSDAY );
}

boolean date_is_friday( DATE *date )
{
	return date_is_day_of_week( date, WDAY_FRIDAY );
}

boolean date_is_saturday( DATE *date )
{
	return date_is_day_of_week( date, WDAY_SATURDAY );
}

DATE *date_forward_to_first_month(
		DATE *d,
		int utc_offset )
{
	DATE *return_date;

	return_date = date_back_to_first_month( d, utc_offset );
	date_increment_months( return_date, 1 );

	return return_date;
}

DATE *date_back_to_first_month(
		DATE *d,
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

DATE *date_prior_day(
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

DATE *date_prior_saturday( DATE *date )
{
	return date_prior_day( date, WDAY_SATURDAY );
}

DATE *date_prior_sunday( DATE *date )
{
	return date_prior_day( date, WDAY_SUNDAY );
}

DATE *date_yyyy_mm_dd_hhmm_new(
		char *date_string,
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

DATE *date_yyyy_mm_dd_hhmmss_new(
		char *date_string,
		char *time_string )
{
	DATE *date;
	int year, month, day, hour, minute, second;
	char buffer[ 3 ];

	if ( !date_string
	||   strlen( date_string ) != 10 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid date_string=[%s]\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			(date_string) ? date_string : "" );
	}

	if ( !time_string
	||   strlen( time_string ) != 6 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid time_string=[%s]\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			(time_string) ? time_string : "" );
	}

	year = atoi( date_string );
	month = atoi( date_string + 5 );
	day = atoi( date_string + 8 );

	*(buffer + 2) = '\0';

	*buffer = *time_string;
	*(buffer + 1) = *(time_string + 1);

	hour = atoi( buffer );

	*buffer = *(time_string + 2);
	*(buffer + 1) = *(time_string + 3);

	minute = atoi( buffer );

	*buffer = *(time_string + 4);
	*(buffer + 1) = *(time_string + 5);

	second = atoi( buffer );

	date = date_new( year, month, day );

	date_set_time_second( date, hour, minute, second );

	return date;
}

boolean date_set_time_hhmmss(
		DATE *date,
		/* --------------------- */
		/* Looks like "23:59:59" */
		/* --------------------- */
		char *hhmmss )
{
	int hour, minute, second;
	char piece_buffer[ 128 ];

	if ( string_character_count( ':', hhmmss ) != 2 )
	{
		return 0;
	}

	piece( piece_buffer, ':', hhmmss, 0 );
	hour = atoi( piece_buffer );

	piece( piece_buffer, ':', hhmmss, 1 );
	minute = atoi( piece_buffer );

	piece( piece_buffer, ':', hhmmss, 2 );
	second = atoi( piece_buffer );

	date_set_time_second( date, hour, minute, second );

	return 1;
}

boolean date_set_time_hhmm(
		DATE *date,
		char *hhmm )
{
	int hours, minutes;
	char buffer[ 128 ];

	/* Remove spaces and colons. */
	/* ------------------------- */
	string_trim_character(
		hhmm,
		':' );

	string_trim_character(
		hhmm,
		' ' );

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

DATE *date_increment_hours(
		DATE *d,
		double hours )
{
	d->current += (long)((float)SECONDS_IN_HOUR * hours);
	date_set_tm_structures( d, d->current, date_utc_offset() );
	return d;
}

DATE *date_add_minutes(
		DATE *d,
		int minutes )
{
	return date_increment_minutes( d, minutes );
}

DATE *date_increment_minutes(
		DATE *d,
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

DATE *date_increment_second(
		DATE *d,
		int second )
{
	d->current += (long)second;
	date_set_tm_structures( d, d->current, 0 /* utc_offset */ );
	return d;
}

DATE *date_increment_seconds(
		DATE *d,
		int seconds )
{
	return
	date_increment_second(
		d,
		seconds );
}

int date_subtract_minutes( DATE *later_date, DATE *earlier_date )
{
	time_t difference;
	int result;

	difference = later_date->current - earlier_date->current;
	result = (int)((double)difference / (double)SECONDS_IN_MINUTE);
	return result;
}

int date_subtract_days( DATE *later_date, DATE *earlier_date )
{
	time_t difference;
	int result;

	difference = later_date->current - earlier_date->current;
	result = (int)((double)difference / (double)SECONDS_IN_DAY);
	return result;
}

int date_month_day_number( DATE *date )
{
	return date->tm->tm_mday;
}

int date_day( DATE *date )
{
	return date->tm->tm_mday;
}

int date_day_number( DATE *date )
{
	return date_day( date );
}

int date_week_number( DATE *date )
{
	return ( date->tm->tm_yday / 7 ) + 1;
}

int date_month_offset( DATE *date )
{
	return date->tm->tm_mon;
}

int date_month_number( DATE *date )
{
	return date->tm->tm_mon + 1;
}

int date_year_number( DATE *date )
{
	return date->tm->tm_year + 1900;
}

int date_hour_number( DATE *date )
{
	return date_hour_integer( date );
}

int date_minutes_number( DATE *date )
{
	return date->tm->tm_min;
}

DATE *date_round2five_minutes( DATE *date )
{
	int hour;
	int minutes;
	int remainder;

	hour = date_hour_number( date );
	minutes = date_minutes_number( date );
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

DATE *date_copy( DATE *date_destination, DATE *date_source )
{
	if ( !date_source )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: date_source is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !date_destination ) date_destination = date_calloc();

	date_destination->current = date_source->current;
	memcpy( date_destination->tm, date_source->tm, sizeof ( struct tm ) );

	return date_destination;
}

char *date_display_mdy( DATE *date )
{
	char buffer[ 32 ];

	sprintf(buffer, 
		"%02d/%02d/%04d",
		date_month( date ),
		date_day_of_month( date ),
		date_year( date ) );

	return strdup( buffer );
}

char *date_yyyy_mm_dd_hhmm( DATE *date )
{
	/* Returns heap memory */
	/* ------------------- */
	return date_display_yyyy_mm_dd_hhmm( date );
}

char *date_display_yyyy_mm_dd_hhmm( DATE *date )
{
	char buffer[ 32 ];

	sprintf(buffer, 
		"%d-%02d-%02d:%02d%02d",
		date_year( date ),
		date_month( date ),
		date_day_of_month( date ),
		date_hour( date ),
		date_minute( date ) );

	return strdup( buffer );
}

char *date_time_display( DATE *date )
{
	return date_display_yyyy_mm_dd_hh_colon_mm( date );
}

char *date_display_yyyy_mm_dd_hh_colon_mm( DATE *date )
{
	char buffer[ 64 ];

	sprintf(buffer, 
		"%d-%02d-%02d %02d:%02d",
		date_year( date ),
		date_month( date ),
		date_day_of_month( date ),
		date_hour( date ),
		date_minute( date ) );

	return strdup( buffer );
}

char *date_display_yyyy_mm_dd_hh_colon_mmss( DATE *date )
{
	char buffer[ 128 ];

	sprintf(buffer, 
		"%d-%02d-%02d %02d:%02d%02d",
		date_year( date ),
		date_month( date ),
		date_day_of_month( date ),
		date_hour( date ),
		date_minute( date ),
		date_second( date ) );

	return strdup( buffer );
}

void date_increment_weekly_ceiling( DATE *date )
{
	while( date_day_of_week( date ) != DATE_END_OF_WEEK_INDICATOR )
		date_increment_day( date );
}

boolean date_compare(
		DATE *date1,
		DATE *date2 )
{
	if ( !date1 || !date2 ) return 0;

	return
	( strcmp(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_yyyy_mm_dd( date1 ),
		date_yyyy_mm_dd( date2 ) ) );
}

boolean date_same_day(
		DATE *old_date,
		DATE *new_date )
{
	if ( !old_date || !new_date ) return 0;

	return
	( strcmp(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_yyyy_mm_dd( old_date ),
		date_yyyy_mm_dd( new_date ) ) == 0 );
}

boolean date_tomorrow(
		DATE *old_date, 
		DATE *new_date )
{
	int return_value;
	DATE *tomorrow_date;

	tomorrow_date = date_calloc();
	date_copy( tomorrow_date, old_date );

	date_increment_day( tomorrow_date );

	return_value =
		( strcmp(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			date_yyyy_mm_dd( new_date ),
			date_yyyy_mm_dd( tomorrow_date ) ) == 0 );

	date_free( tomorrow_date );

	return return_value;
}

boolean date_year_in_future(
		DATE *date,
		int year )
{
	return ( year > date_year_number( date ) );
}

void date_output_hour_error(
		char *function_name,
		int hour )
{
	fprintf(stderr,
		"ERROR in %s(): daylight savings time hour problem = %d.\n",
		function_name,
		hour );
}

int date_days_in_month(
		int month,
		int year )
{
	return date_last_month_day( month, year );
}

char *date_current_static_yyyy_mm_dd_string(
		time_t current,
		int utc_offset )
{
	DATE *date = date_calloc();
	static char yyyy_mm_dd_string[ 16 ];

	date_set_tm_structures( date, current, utc_offset );
	date->current = date_tm_to_current( date->tm, utc_offset );

	strcpy( yyyy_mm_dd_string, date_static_display_yyyy_mm_dd( date ) );
	date_free( date );

	return yyyy_mm_dd_string;
}

char *date_prior_day_of_week_yyyy_mm_dd_string(
		DATE *date,
		int day_of_week,
		int utc_offset )
{
	int wday;
	time_t current;

	wday = date->tm->tm_wday;
	current = date->current;

	while( wday != day_of_week )
	{
		wday--;
		current -= SECONDS_IN_DAY;
		if ( wday == -1 ) wday = 6;
	}

	return
	date_current_static_yyyy_mm_dd_string(
		current,
		utc_offset );
}

char *date_unix_now_string( void )
{
	char sys_string[ 128 ];

	sprintf(sys_string,
		"date +'%cF:%cH%cM'",
		'%', '%', '%' );

	return string_pipe_fetch( sys_string );
}

boolean date_is_greatgreatgrandfather( DATE *d )
{
	int month;
	int day;

	month = date_month( d );
	day = date_day( d );

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
	day = date_day( d );

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

	day = date_day( d );

	if ( day == 1 )
		return 1;
	else
		return 0;
}

boolean date_is_father( DATE *d )
{
	return date_is_sunday( d );
}

char *date_colon_now_time( void )
{
	char *sys_string;

	sys_string = "now.sh hhmmss | sed 's/\\([0-9][0-9]$\\)/:&/'";
	return string_pipe_fetch( sys_string );
}

void date_remove_colon_in_time( char *time_string )
{
	char hour[ 3 ];
	char minute[ 3 ];

	if ( string_strlen( time_string ) != 5 ) return;

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

	if ( string_strlen( time_string ) != 4 ) return;
	if ( string_strcmp( time_string, "null" ) == 0 ) return;

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
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_now_hh_colon_mm_colon_ss(
			date_utc_offset() );

	snprintf(
		date_hhmmss,
		sizeof ( date_hhmmss ),
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

	if ( string_strcmp( time_string, "null" ) == 0 ) return time_string;

	count = string_character_count( ':', time_string );

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

int date_week_of_year( DATE *date )
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
		( date_day_of_week( january_1st ) == WDAY_SUNDAY );

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
				( ( date_day_of_week( december_23rd ) ==
				    WDAY_FRIDAY )
			||        ( date_day_of_week( december_23rd ) ==
				    WDAY_SATURDAY ) );

			if ( !december_23rd_friday_or_saturday )
				week_of_year = 1;
		}
	}

	return week_of_year;
}

int date_last_month_day(
		int month,
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

	if ( date_year_leap_boolean( year ) )
		return 29;
	else
		return 28;
}

boolean date_year_leap_boolean( int year )
{
	if ( ( year % 100 ) == 0
	||   ( year % 4 ) == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int date_utc_offset( void )
{
	char *utc_offset;
	static int utc_offset_integer = -99;

	if ( utc_offset_integer != -99 ) return utc_offset_integer;

	utc_offset = date_environment( "UTC_OFFSET" );

	if ( utc_offset )
		utc_offset_integer = atoi( utc_offset );
	else
		utc_offset_integer = DATE_DEFAULT_UTC_OFFSET;

	return utc_offset_integer;
}

char *date_environment( char *variable_name )
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

void date_environ_set_TZ( char *TZ )
{
	char statement[ 128 ];

	sprintf( statement, "TZ=%s", TZ );
	putenv( strdup( statement ) );
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

	if ( string_character_count( '-', date_time_string ) != 2 )
		return 0;

	if ( string_character_count( ' ', date_time_string ) != 1 )
		return 0;

	if ( string_character_count( ':', date_time_string ) != 2 )
		return 0;

	column( date_half, 0, date_time_string );
	column( time_half, 1, date_time_string );

	piece( year_string, '-', date_half, 0 );
	piece( month_string, '-', date_half, 1 );
	piece( day_string, '-', date_half, 2 );

	piece( hour_string, ':', time_half, 0 );
	piece( minute_string, ':', time_half, 1 );
	piece( second_string, ':', time_half, 2 );

	date_set_date_integers(
		date,
		atoi( year_string ),
		atoi( month_string ),
		atoi( day_string ),
		0 /* utc_offset */ );

	date_set_time_integers(
		date,
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
	static char buffer[ 128 ];

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

int date_month_day_integer( DATE *date )
{
	return date->tm->tm_mday;
}

char *date_append_time(
		char *date_string,
		int width )
{
	char append_time[ 64 ];
	char *time_string = {0};

	if ( !date_string ) return date_string;

	if ( string_character_exists( date_string, ' ' ) ) return date_string;

	if ( width == 16 )
	{
		time_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			date_now_colon_hhmm(
				date_utc_offset() );
	}
	else
	if ( width == 19 )
	{
		time_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			date_now_colon_hhmmss(
				date_utc_offset() );
	}
	else
	{
		return date_string;
	}

	snprintf(
		append_time,
		sizeof ( append_time ),
		"%s %s",
		date_string,
		time_string );

	return strdup( append_time );
}

boolean date_greater_year_boolean(
		char *early_yyyy_mm_dd_string,
		char *late_yyyy_mm_dd_string )
{
	int days_between;
	int days_compare;
	boolean year_leap_boolean;

	if ( !early_yyyy_mm_dd_string
	||   !late_yyyy_mm_dd_string )
	{
		return 0;
	}

	days_between =
		/* -------------- */
		/* Starts on zero */
		/* -------------- */
		date_days_between(
			early_yyyy_mm_dd_string,
			late_yyyy_mm_dd_string );

	year_leap_boolean =
		date_year_leap_boolean(
			atoi( early_yyyy_mm_dd_string ) );

	if ( year_leap_boolean )
		days_compare = 366;
	else
		days_compare = 365;

	if ( days_between >= days_compare )
		return 1;
	else
		return 0;
}
