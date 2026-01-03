/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/utility/yesterday.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "date.h"

int main( int argc, char **argv )
{
	DATE *d;
	double days_to_minus = -1.0;

	if ( argc == 2 )
	{
		days_to_minus = atof( argv[ 1 ] );
	}

	d = date_today_new( date_utc_offset() );
	date_increment_days( d, days_to_minus );
	printf( "%s\n", date_display_yyyy_mm_dd( d ) );
	return 0;
}

