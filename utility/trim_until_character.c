/* trim_until_character.c */
/* ---------------------- */
/*			  */
/* Tim Riley		  */
/* ---------------------- */
#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main( int argc, char **argv )
{
	char in_buffer[ 1024 ];
	char out_buffer[ 1024 ];
	char character;
	int num2trim;			/* one based */

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
		printf( "%s\n",
			trim_until_character(
				out_buffer,
				character,
				in_buffer,
				num2trim ) );

#ifdef NOT_DEFINED
	int current_field;		/* one based */
	char *ptr, *hold;
		hold = ptr = in_buffer;
		current_field = 1;
		while( *ptr )
		{
			/* Might pass a zero on the command line */
			/* ------------------------------------- */
			if ( num2trim == 0 )
			{
				printf( "%s\n", ptr );
				break;
			}
			if ( *ptr == character )
			{
				if ( current_field == num2trim )
				{
					printf( "%s\n", ptr + 1 );
					break;
				}
				current_field++;
			}
			ptr++;
		}
		/* If the character is not there then print the line. */
		/* -------------------------------------------------- */
		if ( !*ptr ) printf( "%s\n", hold );
#endif
	}

	return 0;
} /* main() */
