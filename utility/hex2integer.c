/* hex2integer.c */
/* ------------- */
/* Tim Riley     */
/* ------------- */

#include <stdio.h>

int hex2integer( char *hex );

int main( int argc, char **argv )
{
	if ( argc < 2 )
	{
		fprintf( stderr, "Usage: %s hex\n", argv[ 0 ] );
		exit( 1 );
	}
	while( --argc )
		printf( " %d", hex2integer( *++argv ) );
	printf( "\n" );
}

int hex2integer( char *hex )
{
	int i;

	sscanf( hex, "%0x", &i );
	return i;
}
