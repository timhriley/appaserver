/* google_earth_dvr.c					*/
/* ---------------------------------------------------	*/
/* This is the test driver program for the google       */
/* earth ADT.						*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "google_earth.h"
#include "piece.h"
#include "appaserver_library.h"
#include "appaserver_error.h"

#define APPLICATION		"hydrology"
#define DOCUMENT_ROOT		"/var/www"
#define IMAGE_DIRECTORY		"station_images_medium"
#define CHART_DIRECTORY		"station_plots"

void test1(		void );

boolean image_exists(	char *station );

boolean chart_exists(	char *station );

char *get_images_tag(	char *station,
			char *uname );

char *get_placemark_description(
			char *station,
			char *latitude,
			char *longitude );

void populate_placemark_list(
			LIST *placemark_list,
			char *application_name );

int main( void )
{
	test1();
	return 0;
}

void test1( void )
{
	GOOGLE_EARTH *google_earth;

	google_earth =
		google_earth_new(
		"Everglades National Park Hydrographic Stations",
		"Hydro Stations",
		"Locations and current conditions at Hydro Stations located in and around Everglades National Park",
		25.32745,
		-80.68306 );

	google_earth_output_heading(
				stdout,
				google_earth->document_name,
				google_earth->folder_name,
				google_earth->folder_description,
				google_earth->center_latitude,
				google_earth->center_longitude );

	populate_placemark_list(
			google_earth->placemark_list,
			"hydrology" );

	google_earth_output_placemark_list(
			stdout,
			google_earth->placemark_list );

	google_earth_output_closing( stdout );

} /* test1() */

void populate_placemark_list(
			LIST *placemark_list,
			char *application_name )
{
	char sys_string[ 1024 ];
	char input_buffer[ 128 ];
	char *placemark_description;
	char *where;
	char station[ 16 ];
	char latitude[ 128 ];
	char longitude[ 128 ];
	FILE *input_pipe;
	char *select = "station,lat_nad83,long_nad83";

	where = "lat_nad83 is not null";

	sprintf(sys_string,
		"get_folder_data	application=%s		"
		"			select=%s		"
		"			folder=station		"
		"			where=\"%s\"		",
		application_name,
		select,
		where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	station,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		if ( !chart_exists( station ) ) continue;

		piece(	latitude,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	longitude,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		placemark_description =
			get_placemark_description(
				station,
				latitude,
				longitude );

		google_earth_set_placemark(
					placemark_list,
					strdup( station ),
					strdup( placemark_description ),
					atof( latitude ),
					atof( longitude ) );
	}

	pclose( input_pipe );

} /* populate_placemark_list() */

char *get_placemark_description(	char *station,
					char *latitude,
					char *longitude )
{
	static char placemark_description[ 2048 ];
	char *uname;
	char *images_tag;

	uname = pipe2string( "uname -n" );

	images_tag = get_images_tag( station, uname );

	sprintf( placemark_description,
"<table align=center border=0 cellpadding=0 cellspacing=0>\n"
"<tr><td align=center><big><big><big><bold>%s</bold></big></big></big>\n"
"%s\n"
"<tr><td align=center><a href=http://%s/%s/dailyplots/%s.pdf>\n"
"<img border=2 src=http://%s/%s/%s/%s.jpg width=400></a>\n"
"<tr><table align=center border=0 cellpadding=0 cellspacing=0>\n"
"<tr><td>Station Located at:\n"
"<tr><td>Latitude<td>%s\n"
"<tr><td>Longitude<td>%s\n"
"</table>\n"
"<tr><td><a href=\"mailto:EVER_data_request@nps.gov?subject=GoogleEarth Everglades data request for %s\">DataForEVER data request</a>\n"
"</table>\n",
		 station,
		 images_tag,
		 uname,
		 APPLICATION,
		 station,
		 uname,
		 APPLICATION,
		 CHART_DIRECTORY,
		 station,
		 latitude,
		 longitude,
		 station );

	return placemark_description;

} /* get_placemark_description() */

char *get_images_tag(	char *station,
			char *uname )
{
	static char images_tag[ 256 ];

	*images_tag = '\0';

	if ( image_exists( station ) )
	{
		sprintf( images_tag,
"<tr><td align=center><img border=2 src=http://%s/%s/%s/%s.png width=200>",
		 	uname,
			APPLICATION,
			IMAGE_DIRECTORY,
		 	station );
	}

	return images_tag;
}

boolean image_exists( char *station )
{
	char filename[ 256 ];
	
	sprintf( filename,
		 "%s/%s/%s/%s.png",
		 DOCUMENT_ROOT,
		 APPLICATION,
		 IMAGE_DIRECTORY,
		 station );
	return timlib_file_exists( filename );
}

boolean chart_exists( char *station )
{
	char filename[ 256 ];
	
	sprintf( filename,
		 "%s/%s/%s/%s.jpg",
		 DOCUMENT_ROOT,
		 APPLICATION,
		 CHART_DIRECTORY,
		 station );

	return timlib_file_exists( filename );
}
