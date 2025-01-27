/* make_key.c */
/* ----------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "timlib.h"

char *make_key( char *s );
int is_strip_character( char c1 );

char strip_character_array[] = { '.', ',', '!', '\0' };

int main( int argc, char **argv )
{
	char buffer[ 2048 ];

	if ( argc == 2 )
	{
		printf( "%s\n", make_key( argv[ 1 ] ) );
	}
	else
	while( get_line( buffer, stdin ) )
		printf( "%s\n", make_key( buffer ) );

	return 0;
} /* main() */

char *make_key( char *s )
{
	static char buffer[ 2048 ]; 
	char *buf_ptr = buffer;
	char c;
	int inside_space = 0;
	int is_space;
	int is_strip;

	while( ( c = *s++ ) )
	{
		is_space = isspace( c );
		is_strip = is_strip_character( c );
		if ( ( is_space && inside_space ) || is_strip ) continue;

		if ( is_space )
		{
			inside_space = 1;
			*buf_ptr++ = ' ';
			continue;
		}
		else
			inside_space = 0;

		*buf_ptr++ = c;
	}

	return trim( up_string( buffer ) );

} /* make_key() */

int is_strip_character( char c1 )
{
	char *ptr = strip_character_array;
	char c2;

	while( ( c2 = *ptr++ ) )
		if ( c2 == c1 ) return 1;

	return 0;

} /* is_strip_character() */
