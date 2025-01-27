#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"

#define LENGTH 500

int main( int argc, char **argv )
{
        char buffer[ LENGTH ];
        int  fix_len;

        if ( argc != 2 )
        {
                fprintf(	stderr, 
		"Usage: padspace length\nNote: The CR is included\n" );
                exit( 1 );
        }

        fix_len = atoi( argv[ 1 ] );

        while( get_line( buffer, stdin ) )
        {
                if ( (int)strlen( buffer ) >= fix_len )
                {
                        fprintf( stderr,
				"String length is too long: %s\n",
                                buffer );
                }

                printf( "%-*s\n", fix_len - 1, buffer );
        }

	return 0;
}
