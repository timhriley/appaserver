/* trim_first_ff.c */
/* --------------- */
/* Tim Riley	   */
/* --------------- */

#include <stdio.h>
#include <stdlib.h>

main()
{
	unsigned int c;
	int first_character = 1;

	while( 1 )
	{
		c = getchar();
		if ( c == EOF ) break;

		if( first_character )
		{
			if ( c == 12 )
			{
				first_character = 0;
				continue;
			}
		}
		putchar( c );
	}
} /* main() */
