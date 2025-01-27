/* $APPASERVER_HOME/utility/date_subtract.c */
/* ---------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "timlib.h"
#include "date.h"

void date_subtract(	char *later_date_string,
			char *earlier_date_string );

int main( int argc, char **argv )
{
	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s later_date earlier_date\n",
			argv[ 0 ] );
		exit( 1 );
	}

	date_subtract( argv[ 1 ], argv[ 2 ] );

	return 0;

} /* main() */

void date_subtract(	char *later_date_string,
			char *earlier_date_string )
{
	DATE *later_date;
	DATE *earlier_date;

	if ( ! ( later_date =
			date_yyyy_mm_dd_new(
				later_date_string ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: invalid date format = (%s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 later_date_string );
		exit( 1 );
	}

	if ( ! ( earlier_date =
			date_yyyy_mm_dd_new(
				earlier_date_string ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: invalid date format = (%s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 earlier_date_string );
		exit( 1 );
	}

	printf( "%d\n",
		date_subtract_days(
		later_date,
		earlier_date ) );

} /* date_subtract() */


