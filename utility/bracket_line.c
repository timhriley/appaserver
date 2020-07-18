#include <stdio.h>
#include <string.h>

char *translate_decimal(	char *source );

int main( int argc, char **argv )
{
	char buffer[ 2048 ];
	char open = '[';
	char close = ']';

	if ( argc == 3 )
	{
		open = *argv[ 1 ];
		close = *argv[ 2 ];
	}

	while( fgets( buffer, 2047, stdin ) )
	{
		buffer[ strlen( buffer ) - 1 ] = '\0';
		printf( "%c%s%c\n", open, buffer, close );
		printf( "%c%s%c\n", open, translate_decimal( buffer ), close );
	}

	return 0;
}

char *translate_decimal( char *source )
{
	char buffer[ 2048 ];
	char *ptr = buffer;

	*ptr = '\0';

	while ( *source )
	{
		ptr += sprintf( ptr,
				" %02d",
				(int)*source );
		source++;
	}

	return strdup( buffer );
}
