#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "julian.h"
#include "date.h"

int test1( char *begin_time );
int test_weekly( void );
void test_6_minute_increment( void );
void test_minutes_between( void );

int main( void )
{
	/* test1( "0100" ); */
	/* test_weekly(); */
	/* test_6_minute_increment(); */
	/* test_minutes_between(); */
	return 0;
}

int test_weekly( void )
{
	JULIAN *julian;
	DATE *date;
	char sys_string[ 128 ];
	char date_string[ 16 ];
	FILE *input_pipe;

	strcpy(	sys_string,
"julian_time_list.e 1999-06-01 1999-06-30 1 | piece.e ':' 0 | piece.e '=' 1" );

	julian = julian_new_julian( 0 );
	date = date_calloc();

	input_pipe = popen( sys_string, "r" );

	while( get_line( date_string, input_pipe ) )
	{
		julian_set_yyyy_mm_dd( julian, date_string );

		date_set_yyyy_mm_dd( date, date_string );

		printf( "For date = %s, julian week = %d, date week = %d\n",
			date_string,
			julian_get_week_number( julian->current ),
			date_get_week_number( date ) );
	}
	
	return 1;
}

int test1( char *begin_time )
{
	char *date_yyyy_mm_dd;
	char *time_hhmm;
	double begin_julian;

	begin_julian = 
		julian_yyyy_mm_dd_time_hhmm_to_julian( 	"2000-08-14",
							begin_time );

	printf( "got begin_julian = %lf\n", begin_julian );
	date_yyyy_mm_dd = julian_to_yyyy_mm_dd( begin_julian );
	printf( "got date_yyyy_mm_dd = %s\n", date_yyyy_mm_dd );
	time_hhmm = julian_to_hhmm( begin_julian );
	printf( "got time_hhmm = %s\n", time_hhmm );
	return 1;
}

void test_6_minute_increment( void )
{
	JULIAN *julian;
	int i;
	int measurements_per_year = 87600;
	int measurement_count;

	measurement_count = measurements_per_year * 10;

	/* julian = julian_yyyy_mm_dd_hhmm_new( "2010-03-18", "0054" ); */
	/* julian = julian_yyyy_mm_dd_hhmm_new( "2010-03-18", "1000" ); */
	julian = julian_yyyy_mm_dd_hhmm_new( "2010-03-18", "1300" );
	/* julian = julian_yyyy_mm_dd_hhmm_new( "2010-03-18", "1300" ); */

	printf( "%s:%s\n",
		julian_display_yyyy_mm_dd( julian->current ),
		julian_display_hhmm( julian->current ) );

/*
	for ( i = 0; i < 5; i++ )
*/
	for ( i = 0; i < measurement_count; i++ )
	{
		julian->current =
			julian_increment_minutes( julian->current, 6 );
		printf( "%s:%s\n",
			julian_display_yyyy_mm_dd( julian->current ),
			julian_display_hhmm( julian->current ) );
	}
}

#ifdef NOT_DEFINED
void test_minutes_between( void )
{
	double begin_double =
		julian_yyyy_mm_dd_time_hhmm_to_julian( 	"2011-07-20",
							"1530" );
	double end_double =
		julian_yyyy_mm_dd_time_hhmm_to_julian( 	"2011-07-20",
							"1535" );

	printf( "Should be 5 is: %d\n",
		julian_minutes_between(
			begin_double,
			end_double ) );
}
#endif
