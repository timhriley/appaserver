/* column2pipe.c */
/* ------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "timlib.h"

char *column2pipe(	char *d,
			char *s,
			int number_fields_to_pipe,
			char delimiter );

int main( int argc, char **argv )
{
	char buffer[ 1024 ];
	char input[ 1024 ];
	char delimiter;
	int number_fields_to_pipe;

	if ( argc < 2 )
	{
		fprintf( stderr, 
			 "Usage: %s number_fields_to_pipe [delimiter]\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	number_fields_to_pipe = atoi( argv[ 1 ] );

	if ( argc == 3 )
		delimiter = *argv[ 2 ];
	else
		delimiter = '|';

	while( get_line( input, stdin ) )
	{
		printf( "%s\n", 
			column2pipe(
				buffer,
				input,
				number_fields_to_pipe,
				delimiter ) );
	}
	return 0;
} /* main() */

char *column2pipe( char *d, char *s, int number_fields, char delimiter )
{
	char *anchor = d;
	int first_time = 1;

	*d = '\0';
	while( *s )
	{
		s = skip_spaces( s );

		if ( first_time )
			first_time = 0;
		else
			*d++ = delimiter;

		while( *s && !isspace( *s ) ) *d++ = *s++;
		*d = '\0';

		if ( number_fields == 0 )
		{
			strcpy( d, s );
			break;
		}
		number_fields--;
	}

	while( number_fields-- )
	{
		*d++ = delimiter;
		*d = '\0';
	}

	return anchor;

} /* column2pipe() */

