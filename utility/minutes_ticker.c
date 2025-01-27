/* $APPASERVER_HOME/utility/minutes_ticker.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"

void minutes_ticker(	char *argv_0,
			char *begin_date_time_string,
			char *end_date_time_string );

int main( int argc, char **argv )
{
	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s begin_yyyy_mm_dd:hhmm end_yyyy_mm_dd:hhmm\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	minutes_ticker( argv[ 0 ], argv[ 1 ], argv[ 2 ] );

	return 0;

} /* main() */

void minutes_ticker(	char *argv_0,
			char *begin_date_time_string,
			char *end_date_time_string )
{
	DATE *begin_date_time;
	DATE *end_date_time;
	char *date_display;

	if ( ! ( begin_date_time =
			date_yyyy_mm_dd_colon_hm_new(
				begin_date_time_string ) ) )
	{
		fprintf(	stderr,
				"Error %s: invalid begin_date_time.\n",
				argv_0 );
	}

	if ( ! ( end_date_time = 
			date_yyyy_mm_dd_colon_hm_new(
				end_date_time_string ) ) )
	{
		fprintf(	stderr,
				"Error %s: invalid end_date_time.\n",
				argv_0 );
	}

	if ( begin_date_time->current > end_date_time->current )
	{
		fprintf(	stderr,
				"Error %s: the times are reversed.\n",
				argv_0 );
	}

	while( 1 )
	{
		date_display =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			date_display_yyyy_mm_dd_colon_hhmm(
				begin_date_time );

		printf( "%s\n", date_display );

		free( date_display );

		if ( begin_date_time->current >= end_date_time->current )
			break;

		date_increment_minutes(
			begin_date_time,
			1 );
	}

} /* minutes_ticker() */

