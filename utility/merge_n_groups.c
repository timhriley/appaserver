/* $APPASERVER_HOME/utility/merge_n_groups.c				*/
/* -------------------------------------------------------------------- */
/* This program takes every n groups of lines and appends them together */
/* on one line with a "|" separating them.                              */
/*                                                                      */
/* -------------------------------------------------------------------- */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "name_arg.h"
#include "timlib.h"
 
#define MAX_LINES	32767

void setup_arg(	NAME_ARG *arg, int argc, char **argv );
char **get_entire_array( int *len );
void merge_n_groups( int number_groups, char *delimiter );

int main( int argc, char **argv )
{
        NAME_ARG *arg = init_arg( argv[ 0 ] );
	int n, max_length;
	char *delimiter;

	/* Get command line arguments */
	/* -------------------------- */
        setup_arg( arg, argc, argv );
        n = atoi( fetch_arg( arg, "n" ) );
	delimiter=fetch_arg( arg, "delimiter" );

        merge_n_groups( n, delimiter );
	return 0;
}
 
void merge_n_groups( int number_groups, char *delimiter )
{
	int x,y;
	int number_rows, array_len;
	char **entire_array = get_entire_array( &array_len );

	number_rows = array_len / number_groups;

	for( y = 0; y < number_rows; y++ )
	{
		for( x = 0; x < number_groups; x++ )
		{
			if ( x == 0 )
				printf( "%s", 
					entire_array[ y + x * number_rows ] );
			else
				printf( "|%s", 
					entire_array[ y + x * number_rows ] );
		}
		printf( "\n" );
	}

}

char **get_entire_array( int *len )
{
        char instring[ 1024 ];
	static char *array[ MAX_LINES ];
        int i;
 
	/* Load the entire file into memory */
	/* -------------------------------- */
	for( i = 0; get_line( instring, stdin ); i++ )
	{
		if ( i == MAX_LINES )
		{
			fprintf( stderr, 
				 "ERROR: max lines exceeded: %d\n",
				 MAX_LINES );
			exit( 1 );
		}
		else
			array[ i ] = strdup( instring );
        }
	*len = i;
	return array;

}
 
void setup_arg( NAME_ARG *arg, int argc, char **argv )
{
        int ticket;

        ticket = add_valid_option( arg, "n" );

        ticket = add_valid_option( arg, "delimiter" );
        set_default_value( arg, ticket, "|" );

        ins_all( arg, argc, argv );

}

