/* $APPASERVER_HOME/utility/group_trim.c				*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "boolean.h"

void group_trim(char delimiter,
		int column_count );

void output_delimiters(
		char delimiter,
		int column_count );

int main( int argc, char **argv )
{
	char delimiter;
	int column_count;

	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s delimiter column_count\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];
	column_count = atoi( argv[ 2 ] );

	if ( column_count <= 0 )
	{
		fprintf( stderr,
			 "Error in %s: column_count must be positive.\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	group_trim( delimiter, column_count );
	
	return 0;
}

void group_trim( char delimiter, int column_count )
{
	char input_buffer[ 1024 ];
	char compare_buffer[ 1024 ];
	char old_compare_buffer[ 1024 ];
	boolean first_time = 1;

	*old_compare_buffer = '\0';

	while( get_line( input_buffer, stdin ) )
	{
		if ( !piece_multiple(
			compare_buffer,
			delimiter, 
			input_buffer,
			column_count ) )
		{
			printf( "%s\n", input_buffer );
			continue;
		}

		if ( first_time )
		{
			first_time = 0;	
			strcpy( old_compare_buffer, compare_buffer );
			printf( "%s\n", input_buffer );
		}
		else
		if ( strcmp( compare_buffer, old_compare_buffer ) == 0 )
		{
			output_delimiters( delimiter, column_count );
			printf( "%s\n",
				input_buffer + strlen( compare_buffer ) + 1 );
		}
		else
		{
			strcpy( old_compare_buffer, compare_buffer );
			printf( "%s\n", input_buffer );
		}
	}
}

void output_delimiters(
		char delimiter,
		int column_count )
{
	while( column_count-- ) printf( "%c", delimiter );
}

