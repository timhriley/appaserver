/* html_paragraph_wrapper.c */
/* ------------------------ */
/*			    */
/* Tim Riley		    */
/* ------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

int main( void )
{
	char buffer[ 4096 ];

	while( get_line( buffer, stdin ) ) 
		if ( *buffer )
			printf( "<p>%s\n", buffer );
	return 0;

} /* main() */
