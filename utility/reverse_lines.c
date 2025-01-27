/* $APPASERVER_HOME/utility/reverse_lines.c		*/
/* ---------------------------------------------------- */
/* This process reverses the lines feed in.		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "list.h"

int main( void )
{
	char input_buffer[ 2048 ];
	LIST *list = list_new();

	while( timlib_get_line( input_buffer, stdin, 2048 ) )
	{
		list_append_pointer( list, strdup( input_buffer ) );
	}

	if ( list_go_tail( list ) )
	{
		do {
			printf( "%s\n", (char *)list_get_pointer( list ) );

		} while( list_previous( list ) );
	}

	return 0;
}

char *reverse( char *d, char *s )
{
	char *ptr;
	char *d_hold = d;

	ptr = s + strlen( s ) - 1;
	while( ptr >= s )
		*d++ = *ptr--;
	*d = '\0';
	return d_hold;
}

