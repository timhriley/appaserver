/* utility/pivot_table.c */
/* --------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "appaserver_error.h"
#include "pivot_table.h"

#define HEADING_LABEL		".heading"

void set_datatype_list(		LIST *datatype_list,
				char *datatype_list_string,
				char *argv_0 );

int main( int argc, char **argv )
{
	PIVOT_TABLE *pivot_table;
	char *datatype_list_string;
	char delimiter;
	char input_buffer[ 65536 ];
	boolean extra_output_needed = 0;

	appaserver_error_stderr( argc, argv );

	if ( argc != 3 )
	{	
		fprintf( stderr,
	"Usage: %s datatype_comma_list delimiter\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	datatype_list_string = argv[ 1 ];
	delimiter = *argv[ 2 ];

	pivot_table = pivot_table_new( delimiter ); 

	set_datatype_list(	pivot_table->datatype_list,
				datatype_list_string,
				argv[ 0 ] );
	
	while( get_line( input_buffer, stdin ) )
	{
		if ( !*input_buffer || *input_buffer == '#' ) continue;

		if ( timlib_strncmp( input_buffer, HEADING_LABEL ) == 0 )
		{
			fprintf( stdout,
				 "%s\n", 
				 input_buffer + strlen( HEADING_LABEL ) );
			continue;
		}

		extra_output_needed =
			pivot_table_set_string(
				stdout,
				&pivot_table->anchor_column_count,
				pivot_table->prior_anchor_string,
				pivot_table->datatype_list,
				pivot_table->delimiter,
				input_buffer );
	}

	if ( extra_output_needed )
	{
		pivot_table_output(	stdout,
					pivot_table->prior_anchor_string,
					pivot_table->datatype_list,
					pivot_table->delimiter );
	}

	return 0;
}

void set_datatype_list(		LIST *datatype_list,
				char *datatype_list_string,
				char *argv_0 )
{
	LIST *datatype_name_list;
	char *datatype_name;

	datatype_name_list = list_string2list( datatype_list_string, ',' );

	if ( !list_length( datatype_name_list ) )
	{
		fprintf( stderr,
			 "Error in %s: empty datatype list.\n",
			 argv_0 );
		exit( 1 );
	}

	list_rewind( datatype_name_list );

	do {
		datatype_name = list_get( datatype_name_list );

		pivot_table_set_datatype(
			datatype_list,
			datatype_name );

	} while( list_next( datatype_name_list ) );
}
