/* utility/conductivity2salinity.c		*/
/* -------------------------------------------- */
/* Input: 	conductivity			*/
/* Output:	salinity			*/
/* 						*/
/* Tim Riley					*/
/* -------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "conductivity2salinity.h"
#include "timlib.h"

int main( int argc, char **argv )
{
	char buffer[ 1024 ];

	if ( argc == 1 )
	{
		while( get_line( buffer, stdin ) )
			printf( "%.2lf\n", 
				conductivity2salinity( atof( buffer ) ) );
	}
	else
	{
		while( --argc )
			printf( "%.2lf\n", 
				conductivity2salinity( atof( *++argv ) ) );
	}

	return 0;
} /* main() */

