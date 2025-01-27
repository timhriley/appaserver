/* utility/build_date_where.c */
/* -------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "query.h"

int main( int argc, char **argv )
{
	char *date_column_name;
	char *time_column_name;
	char *begin_date;
	char *begin_time;
	char *end_date;
	char *end_time;

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s date_column_name time_column_name begin_date begin_time end_date end_time\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	date_column_name = argv[ 1 ];
	time_column_name = argv[ 2 ];
	begin_date = argv[ 3 ];
	begin_time = argv[ 4 ];
	end_date = argv[ 5 ];
	end_time = argv[ 6 ];

	printf( "and %s\n",
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_date_time_between_where(
			(char *)0 /* table_name */,
			date_column_name /* date_attribute_name */,
			time_column_name /* time_attribute_name */,
			begin_date /* date_from_data */,
			end_date /* date_to_data */,
			begin_time /* time_from_data */,
			end_time /* time_to_data */ ) );

	return 0;
}

