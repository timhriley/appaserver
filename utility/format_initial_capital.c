/* utility/format_initial_capital.c */
/* -------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"

int main( void )
{
	char input_buffer[ 1024 ];
	char format_buffer[ 1024 ];

	while( get_line( input_buffer, stdin ) )
	{
		printf( "%s\n",
			format_initial_capital(
				format_buffer, input_buffer ) );
	}

	return 0;
} /* main() */

