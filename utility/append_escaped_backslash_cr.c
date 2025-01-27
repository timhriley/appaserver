/* utility/append_escaped_backslash_cr.c	*/
/* -------------------------------------------- */
/* 						*/
/* Tim Riley					*/
/* -------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"

#define MAX_SIZE	4096

int main( void )
{
	char input_buffer[ MAX_SIZE + 1 ];
	char temp_buffer[ MAX_SIZE + 1 ];
	int inside_escaped_backslash = 0;
	int str_len;

	while( get_line( input_buffer, stdin ) )
	{
		str_len = strlen( input_buffer );

		/* If the last character is a back slash */
		/* ------------------------------------- */
		if ( *(input_buffer + str_len - 1) == '\\' )
		{
			inside_escaped_backslash = 1;

			/* Make it a space */
			/* --------------- */
			*(input_buffer + str_len - 1) = ' ';

			/* Append to the temp buffer */
			/* ------------------------- */
			strcat( temp_buffer, input_buffer );
		}
		else
		/* ------------------------------------------------- */
		/* Otherwise, the last character is not a back slash */
		/* ------------------------------------------------- */
		{
			if ( inside_escaped_backslash )
			{
				printf( "%s%s\n", temp_buffer, input_buffer );
				*temp_buffer = '\0';
				inside_escaped_backslash = 0;
			}
			else
			{
				printf( "%s\n", input_buffer );
				*temp_buffer = '\0';
			}
		}
	}

	if ( inside_escaped_backslash )
		printf( "%s%s\n", temp_buffer, input_buffer );

	return 0;
} /* main() */

