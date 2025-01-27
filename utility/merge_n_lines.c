/* $APPASERVER_HOME/utility/merge_n_lines.c				*/
/* -------------------------------------------------------------------- */
/* This program takes every n lines and appends them together on one    */
/* one line with a "|" separating them.                                 */
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "name_arg.h"
#include "timlib.h"
 
void setup_arg(	NAME_ARG *arg, int argc, char **argv );
void merge_n_lines( int n, int max_length, char *delimiter );

int main( int argc, char **argv )
{
        NAME_ARG *arg = init_arg( argv[ 0 ] );
	int n, max_length;
	char *delimiter;

	/* Get command line arguments */
	/* -------------------------- */
        setup_arg( arg, argc, argv );
        n = atoi( fetch_arg( arg, "n" ) );
        max_length = atoi( fetch_arg( arg, "max_length" ) );
	delimiter=fetch_arg( arg, "delimiter" );

        merge_n_lines( n, max_length, delimiter );
	return 0;
}
 
void merge_n_lines( int n, int max_length, char *delimiter )
{
        char instring[ 500 ];
        int i, all_done = 0;
 
        while( !all_done )
        {
                for( i = 0; i < n; i++ )
                {
			if ( !get_line( instring, stdin ) )
			{
				all_done = 1;
				break;
			}

			/* ----------------------------------- */
			/* If a line is longer than max_length */
			/* then it must be a new record.       */
			/* ----------------------------------- */
			if ( i > 0 && ( strlen( instring ) > max_length ) )
			{
                                printf( "\n%s", instring );
				i = 0;
			}
			else
			/* ----------------------- */
			/* Otherwise, keep merging */
			/* ----------------------- */
			{
                        	if ( i == 0 )
                                	printf( "%s", instring );
                        	else
                                	printf( "%s%s", delimiter, instring );
			}
                }
                printf( "\n" );
        }
}
 
void setup_arg( NAME_ARG *arg, int argc, char **argv )
{
        int ticket;

        ticket = add_valid_option( arg, "n" );

        ticket = add_valid_option( arg, "max_length" );
        set_default_value( arg, ticket, "1024" );

        ticket = add_valid_option( arg, "delimiter" );
        set_default_value( arg, ticket, "|" );

        ins_all( arg, argc, argv );

}

