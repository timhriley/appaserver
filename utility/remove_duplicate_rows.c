/* /utility/remove_duplicate_rows.c */
/* -------------------------------- */
/* ------------------------------------------------------------ */
/* Input:	tablename and primarykey(s)			*/
/* Process: 	create a delete statement to remove duplicated	*/
/* Output:	delete statement				*/
/* ------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include "piece.h"

int remove_duplicate_rows( char *tablename, char *primary_key_list );

int main( int argc, char **argv )
{
	if ( argc != 3 )
	{
		fprintf( stderr, 
		"Usage: %s tablename primarykey_1[,...,primary_key_n]\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	if ( remove_duplicate_rows( argv[ 1 ], argv[ 2 ] ) )
		return 0;
	else
		return 1;

} /* main() */


int remove_duplicate_rows( char *tablename, char *primary_key_list )
{
	char piece_buffer[ 128 ];
	int i;

	printf( " delete from %s ttt\n where rowid <> ( select max( rowid )\n 		 from %s\n 		 where 1 = 1\n",
		tablename,
		tablename );

	for( i = 0; piece( piece_buffer, ',', primary_key_list, i ); i++ )
	{
		printf( " 		   and %s.%s = ttt.%s\n",
			tablename,
			piece_buffer,
			piece_buffer );
	}

	printf( " 		);\n" );

	return 1;

} /* remove_duplicate_rows() */

