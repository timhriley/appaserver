#include <stdio.h>

main()
{
	char c;

	while( ( c = getchar() ) != EOF )
	{
		if ( c != 10 ) printf ("%d\n", c );
	}
}
