/* $APPASERVER_HOME/utility/where.c		 */
/* --------------------------------------------- */
/* Input: a stream of delimited primary keys.	 */
/* Output: a stream of where clauses.		 */
/* --------------------------------------------- */
/* Freely available software: see Appaserver.org */
/* ----------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "query.h"

void where(		LIST *primary_key_list,
			char delimiter );

int main( int argc, char **argv )
{
	LIST *primary_key_list;
	char delimiter;

	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s primary_key_comma_list delimiter\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	primary_key_list = list_string2list( argv[ 1 ], ',' );
	delimiter = *argv[ 2 ];

	where( primary_key_list, delimiter );

	return 0;
}

void where(	LIST *primary_key_list,
		char delimiter )
{
	char input_buffer[ 1024 ];
	char *where_clause;

	while( get_line( input_buffer, stdin ) )
	{
		where_clause = 
			query_where_clause(
				primary_key_list,
				input_buffer,
				delimiter );

		printf( "%s\n", where_clause );

		free( where_clause );
	}
}

