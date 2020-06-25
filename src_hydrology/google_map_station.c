/* google_map_station.c					*/
/* ---------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "application_constants.h"
#include "environ.h"
#include "google_map.h"
#include "google_map_station.h"

GOOGLE_MAP_STATION *google_map_station_new(	
					char *application_name,
					char *login_name,
					char *station_type )
{
	GOOGLE_MAP_STATION *google_map_station;

	google_map_station =
		(GOOGLE_MAP_STATION *)
			calloc( 1, sizeof( GOOGLE_MAP_STATION ) );
	google_map_station->application_name = application_name;
	google_map_station->login_name = login_name;

	if (	login_name
	&&	*login_name
	&&	strcmp( login_name, "waterquality" ) != 0 )
	{
/*
		google_map_station->station_list =
			station_get_login_name_station_list(
				application_name,
				login_name );
*/
		google_map_station->station_list =
			station_get_agency_station_list(
				application_name,
				"NPS_ENP" /* agency */ );
	}
	else
	if (	login_name
	&&	*login_name
	&&	strcmp( login_name, "waterquality" ) == 0 )
	{
		google_map_station->station_list =
			station_get_waterquality_station_list(
				application_name );
	}
	else
	{
		google_map_station->station_list =
			station_get_enp_station_list(
				application_name );
	}

	if ( station_type
	&&   *station_type
	&&   strcmp( station_type, "station_type" ) != 0 )
	{
		google_map_station->station_list =
			station_get_station_type_station_list(
				google_map_station->station_list,
				station_type );
	}

	return google_map_station;

} /* google_map_station_new() */


void google_map_station_output_heading( FILE *output_file,
					char *title,
					char *google_map_key_data,
					char *balloon_click_parameter,
					char *balloon_click_function,
					char *application_name,
					int absolute_position_top,
					int absolute_position_left )
{
	google_map_output_heading(	output_file,
					title,
					google_map_key_data,
					balloon_click_parameter,
					balloon_click_function,
					application_name,
					absolute_position_top,
					absolute_position_left,
					GOOGLE_MAP_CENTER_LATITUDE,
					GOOGLE_MAP_CENTER_LONGITUDE,
					GOOGLE_MAP_STARTING_ZOOM,
					GOOGLE_MAP_WIDTH,
					GOOGLE_MAP_HEIGHT );

} /* google_map_station_output_heading() */

void google_map_station_output_selectable_station(
					FILE *output_file,
					char *station,
					double longitude,
					double latitude )
{
	google_map_output_selectable_map_thumbtack(
					output_file,
					station,
					latitude,
					longitude );

} /* google_map_station_output_selectable_station() */

void google_map_station_output_station(
					FILE *output_file,
					char *station,
					double longitude,
					double latitude )
{
	fprintf(output_file,
"		point = new GLatLng( %.7lf, %.7lf );\n"
"		map.addOverlay( create_station( point, \"%s\", map ) );\n"
"\n",
			latitude,
			longitude,
			station );
} /* google_map_station_output_station() */

void google_map_station_output_head_close(
					FILE *output_file )
{
	google_map_output_heading_close( output_file );
}

void google_map_station_output_body(	FILE *output_file,
					boolean with_table,
					char *additional_javascript )
{
	google_map_output_body(	output_file,
					with_table,
					additional_javascript );

} /* google_map_station_output_body() */

char *google_map_station_get_map_key(
			char *application_name )
{
	return google_map_get_map_key( application_name );

} /* google_map_station_get_map_key() */

