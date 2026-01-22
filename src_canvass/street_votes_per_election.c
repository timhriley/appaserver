/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_canvass/street_votes_per_election.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "piece.h"
#include "appaserver.h"
#include "sql.h"
#include "canvass_street.h"

void street_votes_per_election_output(
		int county_district );

FILE *street_votes_per_election_file(
		int county_district );

int main( int argc, char **argv )
{
	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s county_district\n",
			argv[ 0 ] );
		exit( 1 );
	}

	street_votes_per_election_output(
		atoi( argv[ 1 ] )
			/* county_district */ );

	return 0;
}

void street_votes_per_election_output( int county_district )
{
	FILE *file;
	char input[ 1024 ];
	char street_name[ 128 ];
	char city[ 128 ];

	file = street_votes_per_election_file( county_district );

	while( string_input( input, file, sizeof ( input ) ) )
	{
		piece( street_name, '^', input, 0 );
		piece( city, '^', input, 1 );

		printf( "%s^%.2lf\n",
			input,
			street_votes_per_election(
				street_name,
				city ) );
	}

	pclose( file );
}

FILE *street_votes_per_election_file( int county_district )
{
	char system_string[ 1024 ];
       
	snprintf(
		system_string,
		sizeof ( system_string ),
		"district_street_list.sh %d",
		county_district );

	return popen( system_string, "r" );
}
