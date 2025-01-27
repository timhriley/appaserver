#include <stdio.h>
#include "timlib.h"
main()
{
	char buffer[ 100 ][100 ];

	while( get_line( buffer[ 99 ], stdin ) )
		printf( "%s\n", buffer[ 99 ] );
	return 0;
	
}
