/* ------------------------------------------------------------ */
/* calendar_week.c						*/
/* ------------------------------------------------------------ */
/* Input:	year						*/
/* Output:	YYYY-MM-DD-WK					*/
/* ------------------------------------------------------------ */
/* Tim Riley							*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "date.h"

main( argc, argv )
int argc;
char **argv;
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
		month = get_month( date );
		day = get_day( date );
		printf( "%0.2d %2d %2d\n", month, day, week_number );
		increment_week( date, date_get_utc_offset() );
	}
	return 0;

} /* main() */


display_start_time_to_stdout( key, now )
char *key;
time_t now;
{
	struct tm *struct_tm;

	struct_tm = localtime( &now );
	fprintf( stdout, 
		 "Start time (%s):\t%d:%.2d\n",
		 key,
		 struct_tm->tm_hour,
		 struct_tm->tm_min );

} /* display_start_time_to_stdout() */

