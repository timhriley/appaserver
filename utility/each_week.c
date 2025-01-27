/* ------------------------------------------------------------ */
/* each_week.c 							*/
/* ------------------------------------------------------------ */
/* Input:	1) year						*/
/*		2) day in January to start (1-31)		*/
/* Output:	for each week in the year			*/
/*			output the date 			*/
/* Note:	the format of date is MM DD WK			*/
/* ------------------------------------------------------------ */
/* Tim Riley							*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "date.h"

void display_start_time_to_stdout( char *key, time_t now );

int main( int argc, char **argv )
{
	DATE *date;
	char *year, *day_to_start;
	int week_number, month, day;

	if ( argc != 3 )
	{
		fprintf( stderr, "Usage: %s year day_to_start\n", argv[ 0 ] );
		fprintf( stderr, "Note : day_to_start is (1 - 31)\n" );
		exit( 1 );
	}

	year = argv[ 1 ];
	day_to_start = argv[ 2 ];

	date = date_new(
		atoi( year ),
		1,
		atoi( day_to_start ) );

	for( week_number = 1; week_number <= 52; week_number++ )
	{
		month = date_month_integer( date );
		day = date_day_of_month( date );
		printf( "%2.2d %2.2d %d\n", month, day, week_number );
		increment_week( date );
	}
	return 0;

}


void display_start_time_to_stdout( char *key, time_t now )
{
	struct tm *struct_tm;

	struct_tm = localtime( &now );
	fprintf( stdout, 
		 "Start time (%s):\t%d:%.2d\n",
		 key,
		 struct_tm->tm_hour,
		 struct_tm->tm_min );

}

