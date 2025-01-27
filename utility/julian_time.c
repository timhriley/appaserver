/* $APPASERVER_HOME/utility/julian_time.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "timlib.h"
#include "julian.h"
#include "piece.h"

/* Returns and populates heap memory. */
/* ---------------------------------- */
char *julian_time_now(	char **time_buffer );

int main( int argc, char **argv )
{
	JULIAN *j;
	int days_offset;
	char *date_buffer;
	char *time_buffer;

	if ( argc == 0 )
	{
		fprintf( stderr,
			 "Usage: %s days_offset\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	days_offset = atoi( argv[ 1 ] );

	/* Returns and populates heap memory. */
	/* ---------------------------------- */
	if ( ! ( date_buffer =
			julian_time_now(
				&time_buffer ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: julian_time_now() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	j = julian_yyyy_mm_dd_hhmm_new( date_buffer, time_buffer );

	j->current =
		julian_increment_hours(
			j->current,
			days_offset * 24 );

	printf( "%s:%s\n",
		julian_display_yyyy_mm_dd( j->current ),
		julian_display_hhmm( j->current ) );

	return 0;
}

/* Returns and populates heap memory. */
/* ---------------------------------- */
char *julian_time_now(	char **time_buffer )
{
	char *date_time;
	char local_date_buffer[ 128 ];
	char local_time_buffer[ 128 ];

	date_time = pipe2string( "date.e 0" );

	piece( local_date_buffer, ':', date_time, 0 );

	if ( !piece( local_time_buffer, ':', date_time, 1 ) )
	{
		fprintf( stderr,
		"WARNING in %s/%s()/%d: date.e 0 didn't return a ':'\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (char *)0;
	}

	*time_buffer = strdup( local_time_buffer );
	return strdup( local_date_buffer );
}

