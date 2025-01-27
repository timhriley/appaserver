/* joinlines.c					*/
/* -------------------------------------------- */
/* Input: 	delimiter (command line)	*/
/*		lines (input stream)		*/
/* Output:	line(s) containing		*/
/*		each of the lines.		*/
/* 						*/
/* Tim Riley					*/
/* -------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"

#define BUFFER_SIZE 65536

char *joinlines( char *delimiter, int group_count );

char *joinlines( char *delimiter, int group_count )
{
	static char return_string[ BUFFER_SIZE ];
	char buffer[ BUFFER_SIZE ];
	char previous_line[ BUFFER_SIZE ];
	int first_time = 1;
	static int line_number = 0;

	while( get_line( buffer, stdin ) )
	{
		if ( first_time )
		{
			strcpy( previous_line, buffer );
			*return_string = '\0';
			first_time = 0;
		}
		else
		{
			strcat( return_string, previous_line );
			strcat( return_string, delimiter );
			strcpy( previous_line, buffer );
		}

		if ( group_count )
		{
			if ( ! ( ++line_number % group_count ) )
			{
				break;
			}
		}
	}
	if ( first_time )
		return (char *)0;
	else
	{
		strcat( return_string, previous_line );
		return return_string;
	}
} /* joinlines() */


int main( int argc, char **argv )
{
	int group_count;
	char *results;

	if ( argc < 2 )
	{
		fprintf( stderr,
			 "Usage: %s delimiter [group_count]\n", argv[ 0 ] );
		exit( 1 );
	}

	if ( argc == 3 )
		group_count = atoi( argv[ 2 ] );
	else
		group_count = 0;

	while( 1 )
	{
		if ( ! ( results = joinlines( argv[ 1 ], group_count ) ) )
			break;
		printf( "%s\n", results );
	}
	return 0;

} /* main() */

