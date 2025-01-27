/* library/expected_count_dvr.c */
/* ======================================================= */
/* Freely available software: see Appaserver.org	   */
/* ======================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_library.h"
#include "expected_count.h"
#include "list.h"

#define STATION		"1AAA"
#define DATATYPE	"stage"

int main( int argc, char **argv )
{
	LIST *expected_count_list;

	expected_count_list =
		expected_count_get_expected_count_list(
			"everglades",
			STATION,
			DATATYPE );

	if ( !list_length( expected_count_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): empty expected_count_list\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	printf( "for station = %s and datatype = %s:\n", STATION, DATATYPE );
	printf( "%s\n", expected_count_display( expected_count_list ) );
/*
	do {
		expected_count = list_get_pointer( expected_count_list );
		printf( "for date = %s:%s got expected_count = %d\n",
			expected_count->begin_measurement_date,
			expected_count->begin_measurement_time,
			expected_count->expected_count );

	} while( list_next( expected_count_list ) );
*/
	return 0;
}

