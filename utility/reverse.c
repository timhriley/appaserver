#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"

char *reverse( char *d, char *s );

int main( int argc, char **argv )
{
	char input_buffer[ 1024 ];
	char output_buffer[ 1024 ];

	if ( argc == 2 )
	{
		printf( "%s\n", reverse( output_buffer, argv[ 1 ] ) );
	}
	else
	{
		while( get_line( input_buffer, stdin ) )
			printf( "%s\n",
				reverse( output_buffer, input_buffer ) );
	}

	return 0;
}

char *reverse( char *d, char *s )
{
	char *ptr;
	char *d_hold = d;

	ptr = s + strlen( s ) - 1;
	while( ptr >= s )
		*d++ = *ptr--;
	*d = '\0';
	return d_hold;
}

