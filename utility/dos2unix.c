/* utility/dos2unix.c */
/* ------------------ */

#include <stdio.h>
#include <stdlib.h>

int main( void )
{
	int c;

	while( ( c = getchar() ) != EOF )
		if ( c != 13 )
			putchar( c );
	return 0;
}
