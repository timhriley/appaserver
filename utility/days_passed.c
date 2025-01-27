/* $APPASERVER_HOME/utility/days_passed.c */
/* -------------------------------------- */
/* This program outputs 'yes' if now is   */
/* has passed YYYYMMDDHHMM + days.	  */
/* -------------------------------------- */ 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "timlib.h"
#include "date.h"

void output_days_passed( int days, char *date );

int main( int argc, char **argv )
{
	if ( argc != 3 )
	{
		fprintf(stderr, "Usage: %s days YYMMDDHHMM\n", argv[ 0 ] );
		exit( 1 );
	}

	output_days_passed( atoi( argv[ 1 ] ), argv[ 2 ] );

	return 0;

} /* main() */


void output_days_passed( int days, char *date )
{
	char buffer[ 128 ];
	time_t now, past;
	struct tm tm;

	now = time( 0 );

	memset( &tm, 0, sizeof( struct tm ) );

	tm.tm_year = atoi( midstr( buffer, date, 0, 2 ) );
	tm.tm_mon = atoi( midstr( buffer, date, 2, 2 ) ) - 1;
	tm.tm_mday = atoi( midstr( buffer, date, 4, 2 ) );

	past = mktime( &tm);
	if ( past == -1 )
	{
		fprintf( stderr, 
			 "ERROR: days_passed.e Cannot parse date: (%s)\n",
			 date );
		exit( 1 );
	}

	if ( now - ( days * SECONDS_IN_DAY ) > past )
		printf( "yes\n" );
	else
		printf( "no\n" );

/*
	printf( "past = (%s)\n", ctime( &past ) );
	printf( "now = (%s)\n", ctime( &now ) );
*/

} /* output_days_passed() */


