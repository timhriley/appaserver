/* /usr2/utility/nl_ff.c */
/* --------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main( void )
{
	char buffer[ 1024 ];

	while( get_line( buffer, stdin ) )
	{
		printf( "%s\r\n", buffer );
	}

	printf( "%c", 12 );
	return 0;
}
