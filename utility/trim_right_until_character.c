/* trim_right_until_character.c */
/* ---------------------------- */
/*			        */
/* Tim Riley		        */
/* ---------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	char in_buffer[ 1024 ];
	char *end_ptr;
	char character;
	int num2trim;			/* one based */
	int current_field;		/* one based */

	if ( argc < 2 )
	{
		fprintf( stderr, 
		  	 "Usage: %s character [num2trim]\n", 
			 argv[ 0 ] );
		exit( 1 );
	}

	character = *argv[ 1 ];
	if ( argc == 3 )
		num2trim = atoi( argv[ 2 ] );
	else
		num2trim = 1;

	while( get_line( in_buffer, stdin ) )
	{
		end_ptr = in_buffer + strlen( in_buffer );
		current_field = 1;
		while( end_ptr != in_buffer )
		{
			/* Might pass a zero on the command line */
			/* ------------------------------------- */
			if ( num2trim == 0 )
			{
				printf( "%s\n", in_buffer );
				break;
			}
			if ( *end_ptr == character )
			{
				if ( current_field == num2trim )
				{
					*end_ptr = '\0';
					printf( "%s\n", in_buffer );
					break;
				}
				current_field++;
			}
			end_ptr--;
		}
		/* If the character is not there then print the line. */
		/* -------------------------------------------------- */
		if ( end_ptr == in_buffer ) printf( "%s\n", in_buffer );
	}

	return 0;
} /* main() */
