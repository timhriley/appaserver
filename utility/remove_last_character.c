#include <stdio.h>
#include "timlib.h"

main()
{
	char buffer[ 4096 ];

	get_line( buffer, stdin );
	*(buffer + strlen( buffer ) - 1 ) = '\0';
	printf( "%s\n", buffer );
}
