#include <stdio.h>
#include <stdlib.h>
#include "dollar.h"

int main( void )
{
	double dollar;
	char destination[ 128 ];

	dollar = 0.12;
	printf( "%.2lf is %s\n",
		dollar,
		dollar_text( destination, dollar ) );

	dollar = 1.23;
	printf( "%.2lf is %s\n",
		dollar,
		dollar_text( destination, dollar ) );

	dollar = 2.34;
	printf( "%.2lf is %s\n",
		dollar,
		dollar_text( destination, dollar ) );

	dollar = 12.34;
	printf( "%.2lf is %s\n",
		dollar,
		dollar_text( destination, dollar ) );

	dollar = 112.34;
	printf( "%.2lf is %s\n",
		dollar,
		dollar_text( destination, dollar ) );

	dollar = 123.45;
	printf( "%.2lf is %s\n",
		dollar,
		dollar_text( destination, dollar ) );

	dollar = 1234.56;
	printf( "%.2lf is %s\n",
		dollar,
		dollar_text( destination, dollar ) );

	dollar = 9999.99;
	printf( "%.2lf is %s\n",
		dollar,
		dollar_text( destination, dollar ) );

	dollar = 10000.00;
	printf( "%.2lf is %s\n",
		dollar,
		dollar_text( destination, dollar ) );

	dollar = 100000.00;
	printf( "%.2lf is %s\n",
		dollar,
		dollar_text( destination, dollar ) );

	dollar = 123456.78;
	printf( "%.2lf is %s\n",
		dollar,
		dollar_text( destination, dollar ) );

	dollar = 999999.99;
	printf( "%.2lf is %s\n",
		dollar,
		dollar_text( destination, dollar ) );

	return 0;
}

