#include <stdio.h>
#include <string.h>
#include "timlib.h"

int main( void )
{
	char buffer[ 65536 ];

	while( timlib_get_line( buffer, stdin, 65536 ) )
		printf( "%s|%d\n", buffer, (int)strlen( buffer ) );

	return 0;
}
