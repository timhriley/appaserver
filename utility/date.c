/* $APPASERVER_HOME/utility/date.c			*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "date.h"
#include "boolean.h"

int main( int argc, char **argv )
{
	double days_offset;
	boolean with_seconds = 0;
	DATE *date;

	if ( argc < 2 )
	{
		fprintf(	stderr,
				"%s days_offset [withseconds_yn]\n",
				argv[ 0 ] );
		exit( 1 );
	}

	days_offset = atof( argv[ 1 ] );

	if ( argc == 3 )
	{
		with_seconds =
			( strcmp( argv[ 2 ], "y" ) == 0 ||
			  strcmp( argv[ 2 ], "seconds" ) == 0 );
	}

	if ( !days_offset && !with_seconds )
	{
		date = date_today_new( date_utc_offset() );

		printf( "%s\n",
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			date_display_yyyy_mm_dd_colon_hhmm( date ) );
	}
	else
	{
		date = date_today_new( date_utc_offset() );

		if ( days_offset )
		{
			date_increment_days(
				date,
				days_offset );

			if ( with_seconds )
			{
				printf( "%s:%s\n",
					/* ------------------------- */
					/* Returns heap memory or "" */
					/* ------------------------- */
					date_display_yyyy_mm_dd( date ),
					/* ------------------------- */
					/* Returns heap memory or "" */
					/* ------------------------- */
					date_display_hhmmss( date ) );
			}
			else
			{
				printf( "%s\n",
					/* ------------------------- */
					/* Returns heap memory or "" */
					/* ------------------------- */
					date_display_yyyy_mm_dd( date ) );
			}
		}
		else
		{
			if ( with_seconds )
			{
				printf( "%s:%s\n",
					/* ------------------------- */
					/* Returns heap memory or "" */
					/* ------------------------- */
					date_display_yyyy_mm_dd( date ),
					/* ------------------------- */
					/* Returns heap memory or "" */
					/* ------------------------- */
					date_display_hhmmss( date ) );
			}
			else
			{
				printf( "%s:%s\n",
					/* ------------------------- */
					/* Returns heap memory or "" */
					/* ------------------------- */
					date_display_yyyy_mm_dd( date ),
					/* ------------------------- */
					/* Returns heap memory or "" */
					/* ------------------------- */
					date_display_hhmm( date ) );
			}
		}
	}

	return 0;
}

