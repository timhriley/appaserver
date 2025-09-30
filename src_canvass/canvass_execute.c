/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_canvass/canvass_execute.c			*/
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
#include "canvass.h"

int main( int argc, char **argv )
{
	char *start_street_name;
	char *city;
	char *state_code;
	char *canvass_name;
	int radius_yards;
	int utm_zone;
	CANVASS *canvass;

	if ( argc != 7 )
	{
		fprintf(stderr,
"Usage: %s start_street_name city state_code canvass_name radius_yards utm_zone\n",
			argv[ 0 ] );
		exit( 1 );
	}

	start_street_name = argv[ 1 ];
	city = argv[ 2 ];
	state_code = argv[ 3 ];
	canvass_name = argv[ 4 ];
	radius_yards = atoi( argv[ 5 ] );
	utm_zone = atoi( argv[ 6 ] );

	canvass =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		canvass_new(
			start_street_name,
			city,
			state_code,
			canvass_name,
			radius_yards,
			utm_zone );

	canvass_output(
		canvass->
			canvass_waypoint->
			radius_utm_list );

	return 0;
}

