/* /usr2/utility/format_stamp.c */
/* ---------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	char year[ 32 ];
	char month[ 32 ];
	char day[ 32 ];

	if ( argc != 2 || strlen( argv[ 1 ] ) < 6 )
	{
		fprintf( stderr, "Usage: %s YYMMDDHHMM\n", argv[ 0 ] );
		exit ( 1 );
	}

	printf( "%s/%s/%s\n",
		midstr( month, argv[ 1 ], 2, 2 ),
		midstr( day, argv[ 1 ], 4, 2 ),
		midstr( year, argv[ 1 ], 0, 2 ) );

	return 0;
} /* main() */

