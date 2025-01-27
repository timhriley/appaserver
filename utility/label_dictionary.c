/* $APPASERVER_HOME/utility/label_dictionary.c		*/
/* ---------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "list.h"

void display(		LIST *header_list,
			LIST *row_list,
			char delimiter );

int main( int argc, char **argv )
{
	char header_row[ 65536 ];
	char delimited_row[ 65536 ];
	char delimiter;
	LIST *header_list;
	LIST *row_list;

	if ( argc != 2 )
	{
		fprintf( stderr, 
			 "Usage: %s delimiter\n",
			argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];

	if ( !timlib_get_line( header_row, stdin, 65536 ) ) exit( 0 );

	header_list = list_string_list( header_row, delimiter );

	if ( !list_length( header_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%d: empty header_list in [%s]\n",
			 argv[ 0 ],
			 __LINE__,
			 header_row );
		exit( 1 );
	}

	while( timlib_get_line( delimited_row, stdin, 65536 ) )
	{
		row_list = list_string_list( delimited_row, delimiter );

		if ( list_length( row_list ) != list_length( header_list ) )
		{
			fprintf( stderr,
				 "WARNING in %s/%d: invalid row in [%s]\n",
				 argv[ 0 ],
				 __LINE__,
				 delimited_row );
		}

		display( header_list, row_list, delimiter );

		list_free_container( row_list );
	}

	return 0;
}

void display(		LIST *header_list,
			LIST *row_list,
			char delimiter )
{
	if ( !list_rewind( header_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty header_list\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( row_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty row_list\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	printf( ".begin_row\n" );

	do {
		printf( "%s%c%s\n",
			(char *)list_get_pointer( header_list ),
			delimiter,
			(char *)list_get_pointer( row_list ) );

		list_next( row_list );
	} while ( list_next( header_list ) ); 

	printf( ".end_row\n" );
}

