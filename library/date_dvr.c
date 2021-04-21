/* date_dvr.c */
/* ---------- */

#include <stdio.h>
#include <stdlib.h>
#include "date.h"
#include "timlib.h"
#include "column.h"

void test_payroll_period( void );
void test_strftime( int year );
void test_subtract_second( void );
void test_grandfather_father_son();
void test_round();
void test1();
void test2();
void test_today();
void test4();
void test5();
void test6();
void test_week_of_year_1995();
void test_week_of_year_1998();
void test_week_of_year_1999();
void test_week_of_year_2000();
void test_week_of_year_2001();
void test_week_of_year_2037();
void test_week_of_year_2038();
void test_week_of_year_2045();
void test_1969();
void test_richie();
void test_11pm();
void test_1am();
void test_cross_dst();
void test_days_in_month();
void test_minutes_between( void );
void test_years_between( void );
void test_2015_10_26( void );
void test_increment_year( void );

int main()
{

	test_week_of_year_1995();
#ifdef NOT_DEFINED
	test_increment_year();
	test_payroll_period();
	test_week_of_year_1995();
	test_week_of_year_1998();
	test_week_of_year_1999();
	test_week_of_year_2000();
	test_week_of_year_2001();
	test_1969();
	test_cross_dst();
	test_2015_10_26();
	test_subtract_second();
	test1();
	test2();
	test_today();
	test4();
	test5();
	test6();
	test_richie();
	test_11pm();
	test_1am();
	test_days_in_month();
	test_minutes_between();
	test_years_between();
	test_grandfather_father_son();
	test_round();
	test_week_of_year_2037();
	test_week_of_year_2038();
	test_week_of_year_2045();
	test_strftime( 1995 );
	test_strftime( 1996 );
	test_strftime( 2000 );
	test_strftime( 2001 );
#endif

	return 0;
}

void test_round()
{
	DATE *date;
	int hour,minutes;
	char date_buffer[ 128 ];
	char time_buffer[ 128 ];

	for( hour = 0; hour < 24; hour++ )
	{
		for( minutes = 0; minutes < 60; minutes++ )
		{
			date = date_today_new( date_get_utc_offset() );

			date_set_time(
				date,
				hour,
				minutes );

			date_round2five_minutes( date );

			printf( "hour = %d minutes = %d round = %s date = %s\n",
				hour, minutes,
				date_get_hhmm( time_buffer, date ),
				date_get_yyyy_mm_dd( date_buffer, date ) );
			date_free( date );
		}
	}
}

void test4()
{
	DATE *d;
	char *date_string = "1999-12-31";
	char *time_string = "2320";

	d = date_yyyy_mm_dd_hhmm_new(
		date_string,
		time_string );

	printf( "should be 1999-12-31 is (%s)\n", 
		date_display_yyyy_mm_dd( d ) );

	printf( "should be 2320 is (%s)\n", 
		date_display_hhmm( d ) );

	date_increment_hours( d, 1 );

	printf( "should be 2000-01-01 is (%s)\n", 
		date_display_yyyy_mm_dd( d ) );

	printf( "should be 0020 is (%s)\n", 
		date_display_hhmm( d ) );
}

void test2()
{
	DATE *d;

	d = date_yyyy_mm_dd_new( "1999-12-31" );
	printf( "should be 1999-12-31 is (%s)\n", 
		date_display_yyyy_mm_dd( d ) );
	date_increment_days( d, 1.0 );
	printf( "should be 2000-01-01 is (%s)\n", 
		date_display_yyyy_mm_dd( d ) );

	d = date_yyyy_mm_dd_new( "2000-1-1" );
	printf( "should be 2000-01-01 is (%s)\n", 
		date_display_yyyy_mm_dd( d ) );
	date_increment_days( d, 1.0 );
	printf( "should be 2000-01-02 is (%s)\n", 
		date_display_yyyy_mm_dd( d ) );

}

void test1()
{
	DATE *d = date_new( 1997, 1, 4 );
	int month, day;

	month = get_month( d );
	day = get_day_of_month( d );
	printf( "before increment week: %2d %2d\n", month, day );

	increment_week( d );
	month = get_month( d );
	day = get_day_of_month( d );
	printf( "after increment week: %2d %2d\n", month, day );
}

void test_today()
{
	DATE *d = date_today_new( date_get_utc_offset() );
	printf( "got today = (%s)\n",
		date_display_yyyy_mm_dd_colon_hms( d ) );
}

void test5()
{
	DATE *d = date_get_today_new( date_get_utc_offset() );

	date_increment_days( d, -1.0 );
	printf( "yesterday: %s\n", date_get_yyyy_mm_dd_hhmm( d ) );
}

void test6()
{
	char buffer[ 128 ];
	DATE *d;

	d = date_new( 2020, 1, 1 );

	printf( "should be 2020-01-01: %s\n",
		date_get_yyyy_mm_dd( buffer, d ) );

	date_free( d );

	d = date_new( 2130, 1, 1 );
	printf( "should be 2130-01-01: %s\n",
		date_get_yyyy_mm_dd( buffer, d ) );

	d = date_new( 1969, 12, 31 );

	printf( "should be 1969-12-31: %s\n",
		date_get_yyyy_mm_dd( buffer, d ) );

	date_free( d );

	d = date_new( 1970, 1, 1 );

	printf( "should be 1970-01-01: %s\n",
		date_get_yyyy_mm_dd( buffer, d ) );

	date_free( d );

	d = date_new( 1968, 12, 31 );

	printf( "should be 1968-12-31: %s\n",
		date_get_yyyy_mm_dd( buffer, d ) );

	date_decrement_days( d, 1.0 );

	printf( "should be 1968-12-30: %s\n",
		date_get_yyyy_mm_dd( buffer, d ) );

	date_free( d );
}

void test_1am()
{
	char buffer[ 128 ];
	DATE *d;

	d = date_time_new( 2008, 3, 13, 1, 00 );

	printf( "should be 2008-03-13:0100, is %s:%s\n",
		date_get_yyyy_mm_dd( buffer, d ),
		date_display_hhmm( d ) );
	d->current += 600;
	date_set_tm_structures( d, d->current, date_get_utc_offset() );
	printf( "should be 2008-03-13:0110, is %s:%s\n",
		date_get_yyyy_mm_dd( buffer, d ),
		date_display_hhmm( d ) );
	date_free( d );
}

void test_11pm()
{
	char buffer[ 128 ];
	DATE *d;

	d = date_time_new( 2008, 3, 21, 23, 00 );

	printf( "should be 2008-03-21:2300, is %s:%s\n",
		date_get_yyyy_mm_dd( buffer, d ),
		date_display_hhmm( d ) );
	date_free( d );
}

void test_1969()
{
	DATE *d;

	d = date_yyyy_mm_dd_hhmm_new( "1962-01-01", "0000" );
	printf( "should be 1962-01-01 is (%s)\n", 
		date_display_yyyy_mm_dd( d ) );

	d = date_yyyy_mm_dd_hhmm_new( "1970-01-01", "0000" );
	printf( "should be 1970-01-01:0000 is (%s)\n", 
		date_display_yyyy_mm_dd_hhmm( d ) );

	d = date_yyyy_mm_dd_hhmm_new( "1969-12-31", "2359" );
	printf( "should be 1969-12-31:2359 is (%s)\n", 
		date_display_yyyy_mm_dd_hhmm( d ) );

	d = date_yyyy_mm_dd_hhmm_new( "1969-12-31", "2358" );
	printf( "should be 1969-12-31:2358 is (%s)\n", 
		date_display_yyyy_mm_dd_hhmm( d ) );

	d = date_yyyy_mm_dd_hhmm_new( "1969-12-30", "2358" );
	printf( "should be 1969-12-30:2358 is (%s)\n",
		date_display_yyyy_mm_dd_hhmm( d ) );

	date_decrement_hour( d );

	printf( "should be 1969-12-30:2258 is (%s)\n",
		date_display_yyyy_mm_dd_hhmm( d ) );
}

void test_richie()
{
	DATE *d;

	d = date_calloc();
	d->current = -893400000;

	date_set_tm_structures( d, d->current, date_get_utc_offset() );
	printf( "Ritchie's birth time is (%s)\n",
		date_display_yyyy_mm_dd_hhmm( d ) );
}

void test_cross_dst()
{
	DATE *d;

	d = date_time_new( 2009, 3, 8, 1, 0 );

	printf( "should be 2009-03-08:0100 is (%s:%s)\n", 
		date_display_yyyy_mm_dd( d ),
		date_display_hhmm( d ) );

	date_increment_hours( d, 1 );

	printf( "should be 2009-03-08:0200 is (%s:%s)\n", 
		date_display_yyyy_mm_dd( d ),
		date_display_hhmm( d ) );

	date_increment_hours( d, 1 );

	printf( "should be 2009-03-08:0300 is (%s:%s)\n", 
		date_display_yyyy_mm_dd( d ),
		date_display_hhmm( d ) );
}

void test_days_in_month()
{
	printf( "should be 31 is = (%d)\n",
		date_days_in_month( 1, 2010 ) );
	printf( "should be 28 is = (%d)\n",
		date_days_in_month( 2, 2010 ) );
	printf( "should be 29 is = (%d)\n",
		date_days_in_month( 2, 2012 ) );
}

void test_minutes_between( void )
{
	printf( "Should be 6: is %d\n",
		date_minutes_between(
				"2011-07-20",
				"1530",
				"2011-07-20",
	       			"1535",
				1 /* add_one */ ) );
}

void test_years_between( void )
{
	printf( "Should be 50: is %d\n",
		date_years_between(
				"1960-10-06",
				"2011-08-04" ) );

}

void test_grandfather_father_son()
{
	DATE *d;

	d = date_new_yyyy_mm_dd( "2010-01-01" );
	printf( "Should be greatgreatgrandfather: is %d\n",
		date_is_greatgreatgrandfather( d ) );

	d = date_new_yyyy_mm_dd( "2013-01-01" );
	printf( "Should be greatgrandfather: is %d\n",
		date_is_greatgrandfather( d ) );

	d = date_new_yyyy_mm_dd( "2013-02-01" );
	printf( "Should be grandfather: is %d\n",
		date_is_grandfather( d ) );

	d = date_new_yyyy_mm_dd( "2013-04-14" );
	printf( "Should be father: is %d\n",
		date_is_father( d ) );

}

void test_subtract_second( void )
{
	int seconds_offset;
	DATE *transaction_date;

	transaction_date =
		date_yyyy_mm_dd_hms_new(
			"2016-08-31 23:59:59" );

	printf( "got tm_isdst = %d.\n",
		transaction_date->tm->tm_isdst );

	printf( "got is_daylight_time = %d.\n",
		transaction_date->is_daylight_time );

	printf( "Initial = %s\n",
		date_display_yyyy_mm_dd_colon_hms(
			transaction_date ) );

	for(	seconds_offset = 0;
		seconds_offset < 20;
		seconds_offset++ )
	{
		date_increment_seconds(
			transaction_date,
			-1 );

		printf( "%d seconds ago = %s\n",
			seconds_offset + 1,
			date_display_yyyy_mm_dd_colon_hms(
				transaction_date ) );
	}
} /* test_subtract_second() */

void test_2015_10_26()
{
	DATE *date;
	char *s;

	date = date_now_new( date_get_utc_offset() );

	s = "2015-10-26^null";

	date_set_yyyy_mm_dd_hhmm_delimited(
			date,
			s,
			0 /* date_piece */,
			1 /* time_piece */,
			'^',
			date_utc_offset() );

	printf( "expecting (2015-10-26), got (%s)\n",
		date_display( date ) );
}

void test_week_of_year_1995()
{
	DATE *d;

	fflush( stdout );
	system( "ncal -w -3 1 1995" );
	fflush( stdout );

	fflush( stdout );
	system( "ncal -w -3 12 1995" );
	fflush( stdout );

	d = date_yyyy_mm_dd_new( "1995-01-01" );
	printf( "1995-01-01 should be week 1 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "1995-12-26" );
	printf( "1995-12-26 should be week 52 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "1995-12-27" );
	printf( "1995-12-27 should be week 52 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "1995-12-28" );
	printf( "1995-12-28 should be week 52 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "1995-12-29" );
	printf( "1995-12-29 should be week 52 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "1995-12-30" );
	printf( "1995-12-30 should be week 52 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "1995-12-31" );
	printf( "1995-12-31 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "1996-01-01" );
	printf( "1996-01-01 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "1996-01-02" );
	printf( "1996-01-02 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "1996-01-03" );
	printf( "1996-01-03 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "1996-01-07" );
	printf( "1996-01-07 should be week 2 is: %d\n",
		date_get_week_of_year( d ) );

}

void test_week_of_year_1998()
{
	DATE *d;

	fflush( stdout );
	system( "ncal -w -3 12 1998" );
	fflush( stdout );

	d = date_yyyy_mm_dd_new( "1998-01-01" );
	printf( "1998-01-01 should be week 1 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "1998-12-26" );
	printf( "1998-12-26 should be week 52 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "1998-12-27" );
	printf( "1998-12-27 should be week 1 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "1998-12-31" );
	printf( "1998-12-31 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "1999-01-01" );
	printf( "1999-01-01 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "1999-01-02" );
	printf( "1999-01-02 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "1999-01-03" );
	printf( "1999-01-03 should be week 2 is: %d\n",
		date_get_week_of_year( d ) );
}

void test_week_of_year_1999()
{
	DATE *d;

	fflush( stdout );
	system( "ncal -w -3 12 1999" );
	fflush( stdout );

	d = date_yyyy_mm_dd_new( "1999-01-01" );
	printf( "1999-01-01 should be week 1 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "1999-12-25" );
	printf( "1999-12-25 should be week 52 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "1999-12-26" );
	printf( "1999-12-26 should be week 1 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "1999-12-27" );
	printf( "1999-12-27 should be week 1 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "1999-12-27" );
	printf( "1999-12-27 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "1999-12-31" );
	printf( "1999-12-31 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "2000-01-01" );
	printf( "2000-01-01 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "2000-01-02" );
	printf( "2000-01-02 should be week 2 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "2000-01-03" );
	printf( "2000-01-03 should be week 2 is: %d\n",
		date_get_week_of_year( d ) );
}

void test_week_of_year_2000()
{
	DATE *d;

	fflush( stdout );
	system( "ncal -w -3 12 2000" );
	fflush( stdout );

	d = date_yyyy_mm_dd_new( "2000-01-01" );
	printf( "2000-01-01 should be week 1 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2000-12-23" );
	printf( "2000-12-23 should be week 52 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2000-12-24" );
	printf( "2000-12-24 should be week 53 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2000-12-25" );
	printf( "2000-12-25 should be week 53 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2000-12-26" );
	printf( "2000-12-26 should be week 53 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2000-12-27" );
	printf( "2000-12-27 should be week 53 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2000-12-28" );
	printf( "2000-12-28 should be week 53 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2000-12-29" );
	printf( "2000-12-29 should be week 53 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2000-12-30" );
	printf( "2000-12-30 should be week 53 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2000-12-31" );
	printf( "2000-12-31 should be week 1 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2001-01-01" );
	printf( "2001-01-01 should be week 1 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2001-01-06" );
	printf( "2001-01-06 should be week 1 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2001-01-07" );
	printf( "2001-01-07 should be week 2 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );
}

void test_week_of_year_2001()
{
	DATE *d;

	fflush( stdout );
	system( "ncal -w -3 12 2001" );
	fflush( stdout );

	d = date_yyyy_mm_dd_new( "2001-01-01" );
	printf( "2001-01-01 should be week 1 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2001-12-31" );
	printf( "2001-12-31 should be week 1 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2002-01-01" );
	printf( "2002-01-02 should be week 1 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2002-01-06" );
	printf( "2002-01-06 should be week 2 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2002-01-07" );
	printf( "2002-01-07 should be week 2 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

}

void test_strftime( int year )
{
	char week_of_year_string[ 16 ];
	DATE *date;
	char sys_string[ 128 ];

	sprintf( sys_string, "ncal -w -3 1 %d", year );
	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	sprintf( sys_string, "ncal -w -3 12 %d", year );
	fflush( stdout );
	system( sys_string );
	fflush( stdout );

	date = date_new( year, 1, 1 );
	printf( "should be (%d-01-01) is = %s; day_of_week = %s\n",
		year,
		date_display( date ),
		date_get_day_of_week_string( date ) );

	date = date_new( year, 12, 23 );
	strftime( week_of_year_string, 16, "%U", date->tm );
	printf( "should be (%d-12-23) is = %s week_of_year_string = %s\n",
		year,
		date_display( date ),
		week_of_year_string );

	date = date_new( year, 12, 24 );
	strftime( week_of_year_string, 16, "%U", date->tm );
	printf( "should be (%d-12-24) is = %s, week_of_year_string = %s\n",
		year,
		date_display( date ),
		week_of_year_string );

	date = date_new( year, 12, 25 );
	strftime( week_of_year_string, 16, "%U", date->tm );
	printf( "%s week_of_year_string = %s\n",
		date_display( date ),
		week_of_year_string );

	date = date_new( year, 12, 26 );
	strftime( week_of_year_string, 16, "%U", date->tm );
	printf( "%s week_of_year_string = %s\n",
		date_display( date ),
		week_of_year_string );

	date = date_new( year, 12, 27 );
	strftime( week_of_year_string, 16, "%U", date->tm );
	printf( "%s week_of_year_string = %s\n",
		date_display( date ),
		week_of_year_string );

	date = date_new( year, 12, 28 );
	strftime( week_of_year_string, 16, "%U", date->tm );
	printf( "%s week_of_year_string = %s\n",
		date_display( date ),
		week_of_year_string );

	date = date_new( year, 12, 29 );
	strftime( week_of_year_string, 16, "%U", date->tm );
	printf( "%s week_of_year_string = %s\n",
		date_display( date ),
		week_of_year_string );

	date = date_new( year, 12, 30 );
	strftime( week_of_year_string, 16, "%U", date->tm );
	printf( "%s week_of_year_string = %s\n",
		date_display( date ),
		week_of_year_string );

	date = date_new( year, 12, 31 );
	strftime( week_of_year_string, 16, "%U", date->tm );
	printf( "%s week_of_year_string = %s\n",
		date_display( date ),
		week_of_year_string );

	date = date_new( year + 1, 1, 1 );
	strftime( week_of_year_string, 16, "%U", date->tm );
	printf( "%s week_of_year_string = %s\n",
		date_display( date ),
		week_of_year_string );

	date = date_new( year + 1, 1, 2 );
	strftime( week_of_year_string, 16, "%U", date->tm );
	printf( "%s week_of_year_string = %s\n",
		date_display( date ),
		week_of_year_string );
}

void test_week_of_year_2037()
{
	DATE *d;

	fflush( stdout );
	system( "ncal -w -3 12 2037" );
	fflush( stdout );

	d = date_yyyy_mm_dd_new( "2037-01-01" );

	printf( "Should be 2037-01-01 is: %s\n",
		date_display( d ) );

	printf( "2037-01-01 should be week 1 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2037-12-26" );
	printf( "2037-12-26 should be week 52 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2037-12-27" );
	printf( "2037-12-27 should be week 1 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2037-12-28" );
	printf( "2037-12-28 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "2037-12-29" );
	printf( "2037-12-29 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "2037-12-30" );
	printf( "2037-12-30 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "2037-12-31" );
	printf( "2037-12-31 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "2038-01-01" );
	printf( "2038-01-01 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "2038-01-02" );
	printf( "2038-01-02 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "2038-01-03" );
	printf( "2038-01-03 should be week 2 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "2038-01-07" );
	printf( "2038-01-07 should be week 2 is: %d\n",
		date_get_week_of_year( d ) );
}

void test_week_of_year_2038()
{
	DATE *d;

	fflush( stdout );
	system( "ncal -w -3 12 2038" );
	fflush( stdout );

	d = date_yyyy_mm_dd_new( "2038-01-01" );

	printf( "Should be 2038-01-01 is: %s\n",
		date_display( d ) );

	printf( "2038-01-01 should be week 1 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2038-12-25" );
	printf( "2038-12-25 should be week 52 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2038-12-26" );
	printf( "2038-12-26 should be week 1 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2038-12-27" );
	printf( "2038-12-27 should be week 1 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2038-12-28" );
	printf( "2038-12-28 should be week 1 is: %d\n",
		date_get_week_of_year( d ) )

	d = date_yyyy_mm_dd_new( "2038-12-29" );
	printf( "2038-12-29 should be week 1 is: %d\n",
		date_get_week_of_year( d ) )

	d = date_yyyy_mm_dd_new( "2038-12-30" );
	printf( "2038-12-30 should be week 1 is: %d\n",
		date_get_week_of_year( d ) )

	d = date_yyyy_mm_dd_new( "2038-12-31" );
	printf( "2038-12-31 should be week 1 is: %d\n",
		date_get_week_of_year( d ) )

	d = date_yyyy_mm_dd_new( "2039-01-01" );
	printf( "2039-01-01 should be week 1 is: %d\n",
		date_get_week_of_year( d ) )

	d = date_yyyy_mm_dd_new( "2039-01-02" );
	printf( "2039-01-02 should be week 2 is: %d\n",
		date_get_week_of_year( d ) )

	d = date_yyyy_mm_dd_new( "2039-01-03" );
	printf( "2039-01-03 should be week 2 is: %d\n",
		date_get_week_of_year( d ) )

	d = date_yyyy_mm_dd_new( "2039-01-07" );
	printf( "2039-01-07 should be week 2 is: %d\n",
		date_get_week_of_year( d ) )
}

void test_week_of_year_2045()
{
	DATE *d;

	fflush( stdout );
	system( "ncal -w -3 12 2045" );
	fflush( stdout );

	d = date_yyyy_mm_dd_new( "2045-01-01" );

	printf( "Should be 2045-01-01 is: %s\n",
		date_display( d ) );

	printf( "2045-01-01 should be week 1 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2045-12-26" );
	printf( "2045-12-26 should be week 52 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2045-12-27" );
	printf( "2045-12-27 should be week 52 is: %d, Day of week = %s\n",
		date_get_week_of_year( d ),
		date_get_day_of_week_string( d ) );

	d = date_yyyy_mm_dd_new( "2045-12-28" );
	printf( "2045-12-28 should be week 52 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "2045-12-29" );
	printf( "2045-12-29 should be week 52 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "2045-12-30" );
	printf( "2045-12-30 should be week 52 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "2045-12-31" );
	printf( "2045-12-31 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "2046-01-01" );
	printf( "2046-01-01 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "2046-01-02" );
	printf( "2046-01-02 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "2046-01-03" );
	printf( "2046-01-03 should be week 1 is: %d\n",
		date_get_week_of_year( d ) );

	d = date_yyyy_mm_dd_new( "2046-01-07" );
	printf( "2046-01-07 should be week 2 is: %d\n",
		date_get_week_of_year( d ) );

}

void test_payroll_period( void )
{
	FILE *input_pipe;
	char input_buffer[ 128 ];
	int begin_year;
	int end_year;
	int year;
	int period_number;
	char *beginday;
	char *period;
	char sys_string[ 1024 ];
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];

	period = "biweekly";
	begin_year = 1970;
	end_year = 2030;
	beginday = "friday";

	for( year = begin_year; year <= end_year; year++ )
	{
		for(	period_number = 1; ; period_number++ )
		{
			sprintf( sys_string,
				 "payroll_period.e	"
				 "period=%s		"
				 "beginday=%s		"
				 "year=%d		"
				 "number=%d		",
				 period,
				 beginday,
				 year,
				 period_number );

			input_pipe = popen( sys_string, "r" );

			if( !get_line( input_buffer, input_pipe ) )
			{
				pclose( input_pipe );
				break;
			}

			column( begin_date_string, 1, input_buffer );

			get_line( input_buffer, input_pipe );
			column( end_date_string, 1, input_buffer );
			pclose( input_pipe );

			printf(
		"for year=%d/period=%d: days_between (%s and %s) = %d\n",
				year,
				period_number,
				begin_date_string,
				end_date_string,
				date_days_between(
					begin_date_string,
					end_date_string ) );
		}
	}

} /* test_payroll_period() */

void test_increment_year( void )
{
	DATE *annual;
	char beginning[ 16 ];
	int y;
	int start_year = 2007;
	int years = 8;

	sprintf( beginning, "%d-12-31", start_year );

	annual = date_yyyy_mm_dd_new( beginning );

	for( y = 0; y < years; y++ )
	{
		printf( "%.2d: %s\n",
			y + 1,
			date_display( annual ) );

		date_increment_years( annual, 1 );
	}
}
