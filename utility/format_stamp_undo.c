/* utility/format_stamp_undo.c */
/* --------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	char year[ 32 ];
	char month[ 32 ];
	char day[ 32 ];

	if ( argc != 2 || strlen( argv[ 1 ] ) < 8 )
	{
		fprintf( stderr, "Usage: %s MM/DD/YY\n", argv[ 0 ] );
		exit ( 1 );
	}

	printf( "%s%s%s\n",
		midstr( year, argv[ 1 ], 6, 2 ),
		midstr( month, argv[ 1 ], 0, 2 ),
		midstr( day, argv[ 1 ], 3, 2 ) );

	return 0;
} /* main() */

