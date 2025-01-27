/* utility/format_sql_output.c 		*/
/* ------------------------------------ */
/* Tim Riley				*/
/* ------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"

#define MAX_COLUMNS	1024

/* Function prototypes */
/* ------------------- */
void get_header_array( char **header_array );
void for_each_row_output_each_field( char **header_array );

int main( void )
{
	char *header_array[ MAX_COLUMNS + 1 ];

	get_header_array( header_array );
	for_each_row_output_each_field( header_array );

	return 0;

} /* main() */


void for_each_row_output_each_field( char **header_array )
{
	char input_buffer[ 128 ];
	char piece_buffer[ 128 ];
	int i;

	while( get_line( input_buffer, stdin ) )
	{
		if ( *input_buffer == '|' 
		&&   *(input_buffer + 1) != '-' )
		{
			for( i = 0; header_array[ i ]; i++ )
			{
				piece( piece_buffer, '|', input_buffer, i + 1 );
				printf( "%s|%s\n",
					header_array[ i ],
					piece_buffer );
			}
		}
		else
		{
			/* printf( "ignoring: (%s)\n", input_buffer ); */
		}
	}

} /* for_each_row_output_each_field() */


void get_header_array( char **header_array )
{
	char input_buffer[ 128 ];
	char piece_buffer[ 128 ];
	int i;

	while( get_line( input_buffer, stdin ) )
	{
		if ( strncmp( input_buffer, "SQL", 3 ) == 0 )
		{
			/* A SQL Error occurred */
			/* -------------------- */
			fprintf( stderr, "%s\n", input_buffer );
			exit( 1 );
		}

		if ( *input_buffer == '|'
		&&   isalpha( *( input_buffer + 1 ) ) )
		{
			for( i = 0; 
			     piece( piece_buffer, '|', input_buffer, i + 1 );
			     i++ )
			{
				if ( i == MAX_COLUMNS )
				{
					fprintf(stderr,
						"Max columns exceeded: %d\n",
						MAX_COLUMNS );
					exit( 1 );
				}

				header_array[ i ] = strdup( piece_buffer );
			}

			/* We always get one extra '|' */
			/* --------------------------- */
			header_array[ i - 1 ] = (char *)0;
			return;
		}
	}

	/* No header so just exit success */
	/* ------------------------------ */
	exit( 0 );

} /* get_header_array() */


