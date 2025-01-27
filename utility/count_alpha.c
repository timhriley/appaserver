/* utility/count_alpha.c */
/* ---------------------------- */
/* 				*/
/* -- Tim Riley			*/
/* ---------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"

#define BUFFER_SIZE 4096

char *next_alpha();
char *next_alpha_single( char *alpha );
char *next_alpha_double( char *alpha );

int main( void )
{
	char buffer[ BUFFER_SIZE ];
	char *alpha = "A";

	while ( get_line( buffer, stdin ) )
	{
		printf( "%2.2s) %s\n", alpha, buffer );
		alpha = next_alpha( alpha );
	}

	return 0;

} /* main() */

char *next_alpha( char *alpha )
{
	if ( strlen( alpha ) == 1 )
		return next_alpha_single( alpha );
	else
		return next_alpha_double( alpha );

} /* next_alpha() */

char *next_alpha_single( char *alpha )
{
	static char return_string[ 3 ];
	char alpha_char;

	alpha_char = alpha[ 0 ];
	if ( alpha_char == 'Z' )
	{
		strcpy( return_string, "AA" );
	}
	else
	{
		sprintf( return_string, "%c", alpha_char + 1 );
	}
	return return_string;

} /* next_alpha_single() */

char *next_alpha_double( char *alpha )
{
	static char return_string[ 3 ];
	char first_alpha_char;
	char second_alpha_char;

	first_alpha_char = alpha[ 0 ];
	second_alpha_char = alpha[ 1 ];
	if ( second_alpha_char == 'Z' )
	{
		sprintf( return_string, "%cA", first_alpha_char + 1 );
	}
	else
	{
		sprintf( return_string, "%c%c", 
			 first_alpha_char,
			 second_alpha_char + 1 );
	}
	return return_string;

} /* next_alpha_double() */
