/* decode_html_post.c */
/* ------------------ */

#include <stdio.h>
#include <stdlib.h>
#include "decode_html_post.h"
#include "timlib.h"

int main( void )
{
	char input[ 65536 ];
	char output[ 65536 ];
	
	while( get_line( input, stdin ) )
		printf( "%s\n", 
			decode_html_post( output, input ) );

	return 0;
} /* main() */
