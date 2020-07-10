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
					char *login_name )
{
	GOOGLE_MAP_STATION *google_map_station;

	google_map_station =
		(GOOGLE_MAP_STATION *)
			calloc( 1, sizeof( GOOGLE_MAP_STATION ) );
	google_map_station->application_name = application_name;
	google_map_station->login_name = login_name;

	return google_map_station;
}

LIST *google_map_station_list(	char *application_name,
				int current_year,
				char *station_type )
{
	char sys_string[ 1024 ];
	LIST *station_name_list;

	sprintf( sys_string,
		 "measurement_year_station.sh %d '%s'",
		 current_year,
		 (station_type) ? station_type : "" );

	station_name_list = pipe2list( sys_string );

	return station_name_list_fetch(
			application_name,
			station_name_list );
}


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

LIST *google_map_fetch_station_list(	char *application_name,
					char *login_name,
					char *station_type,
					int current_year )
{
	LIST *station_list;

	if ( current_year )
	{
		station_list =
			google_map_station_list(
				application_name,
				current_year,
				station_type );
	}
	else
	{
		if (	login_name
		&&	*login_name
		&&	strcmp( login_name, "waterquality" ) != 0 )
		{
			station_list =
				station_get_login_name_station_list(
					application_name,
					login_name );
		}
		else
		if (	login_name
		&&	*login_name
		&&	strcmp( login_name, "waterquality" ) == 0 )
		{
			station_list =
				station_get_waterquality_station_list(
					application_name );
		}
		else
		{
			station_list =
				station_get_enp_station_list(
					application_name );
		}
	}
	return station_list;
}

LIST *google_map_station_type_list_fetch( int current_year )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "measurement_year_station_type.sh %d",
		 current_year );

	return pipe2list( sys_string );
}

LIST *google_map_station_datatype_list(
					char *application_name,
					int current_year,
					char *station_name )
{
	char sys_string[ 1024 ];
	LIST *datatype_name_list;

	if ( !station_name || !*station_name )
	{
		fprintf( stderr,
			 "WARNING in %s/%s()/%d: empty station_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (LIST *)0;
	}

	sprintf( sys_string,
		 "measurement_year_datatype.sh %d %s",
		 current_year,
		 station_name );

	datatype_name_list = pipe2list( sys_string );

	return datatype_name_list_fetch(
			application_name,
			datatype_name_list );
}

