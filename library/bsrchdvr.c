/* bsrchdvr.c  */
/* ---------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "bsrch.h"

int main( int argc, char **argv )
{

        B_SEARCH *b;
        char *return_val;

        if( argc != 4 )
        {
                fprintf( stderr,
                        "Usage: bsrchdvr filename record_len search_key\n");
                exit( 1 );
        }

        b = init_bsearch( argv[ 1 ], atoi( argv[ 2 ] ) );

        if( reset_fetch_bsearch( b, argv[ 3 ] ) == -1 )
                fprintf( stderr, "bsrchdvr Cant find key\n" );

        while(( return_val = fetch_each_bsearch( b ) ) != NULL )
        {
                printf( "%s\n", trim( b->return_val ) );
        }

        free_bsearch( b );

	return 0;
} /* end main */
