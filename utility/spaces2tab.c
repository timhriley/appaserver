/* utility/spaces2tab.c */
/* -------------------- */

#include <stdio.h>
#include "timlib.h"

char *spaces2tab( char *d, char *s );

int main( void )
{
	char input_buffer[ 4096 ];
	char output_buffer[ 4096 ];

	while( get_line( input_buffer, stdin ) )
		printf( "%s\n", spaces2tab( output_buffer, input_buffer ) );
	return 0;
}

char *spaces2tab( char *d, char *s )
{
	int space_count = 0;
	int at_beginning = 1;
	char *d_ptr = d;

	while( *s )
	{
		if ( !at_beginning )
		{
			*d_ptr++ = *s;
		}
		else
		if ( *s == ' ' )
		{
			if ( ! ( ++space_count % 8 ) )
			{
				*d_ptr++ = 9;
				space_count = 0;
			}
		}
		else
		{
			if ( space_count )
			{
				while( space_count-- )
					*d_ptr++ = ' ';
			}
			*d_ptr++ = *s;
			at_beginning = 0;
		}
		s++;
	}
	*d_ptr = '\0';
	return d;
} /* spaces2tab() */

