/* single_quotes_around.c */
/* ---------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"

void local_single_quotes_around( char *string );

int main( int argc, char **argv )
{
	char buffer[ 65536 ];

	if ( argc == 2 )
		local_single_quotes_around( argv[ 1 ] );
	else
	{
		while( get_line( buffer, stdin ) )
			local_single_quotes_around( buffer );
	}

	return 0;

} /* main() */

void local_single_quotes_around( char *string )
{
	char buffer[ 1024 ];
	int i;

	for( i = 0; piece( buffer, ',', string, i ); i++ )
	{
		if ( i == 0 )
			printf( "'%s'", buffer );
		else
			printf( ",'%s'", buffer );
	}
	printf( "\n" );

} /* local_single_quotes_around() */

