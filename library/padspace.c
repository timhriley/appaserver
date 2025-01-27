
#include <stdio.h>
#include <string.h>

#define LENGTH 500

void main( int argc, char **argv )
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

        while( gets( buffer ) )
        {
                if( strlen( buffer ) >= fix_len )
                {
                        fprintf( stderr, "String length is too long: %s\n",
                                        buffer );
                }

                printf( "%-*s\n", fix_len - 1, buffer );
        }

} /* main() */
