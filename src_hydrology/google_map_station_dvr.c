/* google_map_station_dvr.c				*/
/* ---------------------------------------------------	*/
/* Retired: 2020-07-10.					*/
/* This is the test driver program for the google map   */
/* station ADT.						*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "google_map_station.h"
#include "appaserver_error.h"

#define APPLICATION_NAME	"hydrology"

int main( int argc, char **argv )
{
	GOOGLE_MAP_STATION *google_map_station;
	char *google_map_key_data;
	STATION *station;

	output_starting_argv_stderr( argc, argv );

	google_map_key_data =
		google_map_station_get_map_key(
			APPLICATION_NAME );

	google_map_station_output_heading(
				stdout,
				(char *)0 /* title */,
				google_map_key_data,
				(char *)0 /* ballon_click_parameter */,
				(char *)0 /* ballon_click_function */,
				APPLICATION_NAME,
				0 /* absolute_position_top */,
				0 /* absolute_position_left */ );

	google_map_station =
		google_map_station_new( APPLICATION_NAME,
					"NPS_ENP" );

	if ( list_rewind( google_map_station->station_list ) )
	{
		do {
			station = list_get_pointer(
					google_map_station->station_list );

			if ( station->long_nad83 && station->lat_nad83 )
			{
				google_map_station_output_selectable_station(
						stdout,
						station->station_name,
						station->long_nad83,
						station->lat_nad83 );
			}
		} while( list_next( google_map_station->station_list ) );
	}

	google_map_station_output_head_close( stdout );

	google_map_station_output_body(
			stdout,
			0 /* not with_table */,
			(char *)0 /* additional_javascript */ );
	printf( "</body>\n" );
	printf( "</html>\n" );
	return 0;
}
