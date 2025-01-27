/* integer2hex.c */
/* ------------- */
/* Tim Riley     */
/* ------------- */

#include <stdio.h>

char *integer2hex( int integer );

int main( int argc, char **argv )
{
	if ( argc < 2 )
	{
		fprintf( stderr, "Usage: %s hex\n", argv[ 0 ] );
		exit( 1 );
	}
	while( --argc )
		printf( " %s", integer2hex( atoi( *++argv ) ) );
	printf( "\n" );
	return 0;
}

char *integer2hex( int integer )
{
	static char hex[ 128 ];

	sprintf( hex, "%x", integer );
	return hex;
}
