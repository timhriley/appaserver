/* $APPASERVER_HOME/utility/or_sequence.c    		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "timlib.h"
#include "appaserver_error.h"
#include "query.h"

int main( int argc, char **argv )
{
	QUERY_OR_SEQUENCE *query_or_sequence;
	LIST *attribute_name_list;
	LIST *full_data_list;
	LIST *partial_data_list;
	int remainder;
	int division_results;
	int length_attribute_list;
	int length_data_list;
	int i, j;
	char *full_data_list_pointer;

	/* appaserver_error_stderr( argc, argv ); */

	if ( argc != 3 )
	{
		fprintf(	stderr,
				"Usage: %s attribute_list data_list\n",
				argv[ 0 ] );
		exit( 1 );
	}

	attribute_name_list = list_string2list( argv[ 1 ], ',' );
	full_data_list = list_string2list( argv[ 2 ], ',' );

	length_attribute_list = list_length( attribute_name_list );
	length_data_list = list_length( full_data_list );

	remainder = length_data_list % length_attribute_list;

	if ( remainder )
	{
		fprintf( stderr,
		"Error in %s: data_list not matchable to attribute_list.\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	division_results = length_data_list / length_attribute_list;

	query_or_sequence = query_or_sequence_new( attribute_name_list );

	list_rewind( full_data_list );

	for( i = 0; i < length_attribute_list; i++ )
	{
		partial_data_list = list_new();

		for( j = 0; j < division_results; j++ )
		{
			full_data_list_pointer =
				list_get(
					full_data_list );

			list_set(	partial_data_list,
					full_data_list_pointer );

			list_next( full_data_list );
		}

		query_or_sequence_set_data_list(
				query_or_sequence->data_list_list,
				partial_data_list );
	}

	printf( "(%s)\n",
		query_or_sequence_where_clause(
				query_or_sequence->attribute_name_list,
				query_or_sequence->data_list_list,
				0 /* not with_and_prefix */ ) );

	return 0;
}

