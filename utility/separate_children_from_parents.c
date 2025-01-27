/* utility/separate_children_from_parents.c		*/
/* ---------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "piece.h"
#include "timlib.h"

int main( int argc, char **argv )
{
	char buffer[ 4096 ];
	char delimiter;
	int offset_parent_ends;
	char parent_string[ 1024 ];
	char old_parent_string[ 1024 ];
	int position_child_begin;

	if ( argc != 3 )
	{
		fprintf( stderr, 
			"Usage: %s delimiter offset_parent_ends\n",
			argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];
	offset_parent_ends = atoi( argv[ 2 ] );

	*old_parent_string = '\0';
	while( get_line( buffer, stdin ) )
	{
		if ( ( position_child_begin =
			piece_delimiter_position(
				buffer,
				delimiter,
				offset_parent_ends ) ) == -1 )
		{
			printf( ".ignored %s\n", buffer );
			continue;
		}

		if ( !*old_parent_string )
		{
			strncpy(	old_parent_string,
					buffer,
					position_child_begin );
			*(old_parent_string + position_child_begin) = '\0';
			printf( ".parent %s\n", old_parent_string );
			printf( ".child %s\n",
				 buffer + position_child_begin + 1 );
			continue;
		}

		strncpy(	parent_string,
				buffer,
				position_child_begin );
		*(parent_string + position_child_begin) = '\0';

		if ( strcmp( parent_string, old_parent_string ) == 0 )
		{
			printf( ".child %s\n",
				buffer + position_child_begin + 1 );
		}
		else
		{
			strcpy( old_parent_string, parent_string );
			printf( ".end\n" );
			printf( ".parent %s\n", old_parent_string );
			printf( ".child %s\n",
				buffer + position_child_begin + 1 );
		}
	}
	printf( ".end\n" );
	return 0;
} /* main() */

