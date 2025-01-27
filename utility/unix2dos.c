/* utility/unix2dos.c */
/* ------------------ */

#include <stdio.h>
#include "timlib.h"

int main( void )
{
	char buffer[ 4096 ];

	while( get_line( buffer, stdin ) )
		printf( "%s\r\n", buffer );
	return 0;
}
