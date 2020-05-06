/* $APPASERVER_HOME/utility/average.c				*/
/* ------------------------------------------------------------ */
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	char delimiter;
	char value_string[ 128 ];
	int count = 0;
	double sum = 0.0;
	double value;
	double average;

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s delimiter\n",
			argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];

	while( get_line( value_string, stdin ) )
	{
		if ( !*value_string ) continue;

		count++;
		value = atof( value_string );

		if ( count == 1 )
		{
			sum = value;
			printf( "%.3lf%c\n", value, delimiter );
			continue;
		}

		sum += atof( value_string );

		average = sum / (double)count;

		printf( "%.3lf%c%.3lf\n",
			value,
			delimiter,
			average );
	}

	return 0;

} /* main() */

