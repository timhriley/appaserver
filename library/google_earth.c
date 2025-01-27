/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/google_earth.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "google_earth.h"
#include "application.h"
#include "appaserver_link.h"

GOOGLE_EARTH *google_earth_new(
			char *document_name,
			char *folder_name,
			char *folder_description,
			double center_latitude,
			double center_longitude )
{
	GOOGLE_EARTH *google_earth;

	google_earth = calloc( 1, sizeof ( GOOGLE_EARTH ) );

	google_earth->document_name = document_name;
	google_earth->folder_name = folder_name;
	google_earth->folder_description = folder_description;
	google_earth->center_latitude = center_latitude;
	google_earth->center_longitude = center_longitude;
	google_earth->placemark_list = list_new();

	return google_earth;
}

GOOGLE_EARTH_PLACEMARK *google_earth_placemark_new( void )
{
	GOOGLE_EARTH_PLACEMARK *google_earth_placemark;

	if ( ! ( google_earth_placemark =
			calloc( 1, sizeof ( GOOGLE_EARTH_PLACEMARK ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return google_earth_placemark;
}

void google_earth_set_placemark(
			LIST *placemark_list,
			char *placemark_name,
			char *placemark_description,
			double latitude,
			double longitude )
{
	GOOGLE_EARTH_PLACEMARK *google_earth_placemark;

	google_earth_placemark = google_earth_placemark_new();

	google_earth_placemark->placemark_name = placemark_name;
	google_earth_placemark->placemark_description = placemark_description;
	google_earth_placemark->latitude = latitude;
	google_earth_placemark->longitude = longitude;
	list_append_pointer( placemark_list, google_earth_placemark );
}

void google_earth_output_heading(
			FILE *output_file,
			char *document_name,
			char *folder_name,
			char *folder_description,
			double center_latitude,
			double center_longitude )
{
	fprintf( output_file,
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<kml xmlns=\"http://earth.google.com/kml/2.1\">\n"
"<Document>\n"
"<name>%s</name>\n"
"\n"
"<Folder>\n"
"\t<name>%s</name>\n"
"\t<description>%s</description>\n"
"\n"
"<LookAt>\n"
"	<latitude>%.5lf</latitude>\n"
"	<longitude>%.5lf</longitude>\n"
"	<altitude>%d</altitude>\n"
"	<range>%d</range>\n"
"	<tilt>%d</tilt>\n"
"	<heading>0</heading>\n"
"	<altitudeMode>%s</altitudeMode>\n"
"</LookAt>\n",
		 document_name,
		 folder_name,
		 folder_description,
		 center_latitude,
		 center_longitude,
		 GOOGLE_EARTH_ALTITUDE,
		 GOOGLE_EARTH_RANGE,
		 GOOGLE_EARTH_TILT,
		 GOOGLE_EARTH_ALTITUDE_MODE );
}

void google_earth_output_placemark_list(
			FILE *output_file,
			LIST *placemark_list )
{
	GOOGLE_EARTH_PLACEMARK *placemark;

	if ( !list_rewind( placemark_list ) ) return;

	do {
		placemark = list_get( placemark_list );

		google_earth_output_placemark(
			output_file,
			placemark->placemark_name,
			placemark->placemark_description,
			placemark->latitude,
			placemark->longitude );

	} while( list_next( placemark_list ) );
}

void google_earth_output_placemark(
			FILE *output_file,
				char *placemark_name,
				char *placemark_description,
				double latitude,
				double longitude )
{
	static int counter = 0;

	fprintf( output_file,
"<Placemark>\n"
"\t<name>%s</name>\n"
"\t<Style id=\"inline_copy%d\">\n"
"\t\t<BalloonStyle><text>\n"
"\t\t<![CDATA[\n"
"\t%s\n"
"\t\t]]>\n"
"\t\t</text></BalloonStyle>\n"
"\t\t<IconStyle>\n"
"\t\t<scale>%.1lf</scale>\n"
"\t\t<color>%s</color>\n"
"\t\t<colorMode>%s</colorMode>\n"
"\t\t<Icon>%s</Icon>\n"
"\t\t</IconStyle>\n"
"\t</Style>\n"
"\n"
"\t<Point>\n"
"\t\t<extrude>%d</extrude>\n"
"\t\t<altitudeMode>%s</altitudeMode>\n"
"\t\t<coordinates>%.5lf,%.5lf,0</coordinates>\n"
"\t</Point>\n"
"</Placemark>\n",
		 placemark_name,
		 counter++,
		 placemark_description,
		 GOOGLE_EARTH_ICON_SCALE,
		 GOOGLE_EARTH_ICON_COLOR,
		 GOOGLE_EARTH_ICON_COLOR_MODE,
		 GOOGLE_EARTH_ICON_IMAGE,
		 GOOGLE_EARTH_POINT_EXTRUDE,
		 GOOGLE_EARTH_ALTITUDE_MODE,
		 longitude,
		 latitude );
}

void google_earth_output_closing( FILE *output_file )
{
	fprintf(output_file,
		"</Folder>\n"
		"</Document>\n"
		"</kml>\n" );
}

#ifdef NOT_DEFINED
void google_earth_filenames(
			char **output_uncompressed_kml_filename,
			char **output_uncompressed_kmz_filename,
			char **output_kmz_filename,
			char **prompt_kml_filename,
			char **prompt_kmz_filename,
			char *application_name,
			char *data_directory,
			char *session_key )
{
	APPASERVER_LINK *appaserver_link;

	appaserver_link =
		appaserver_link_new(
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
			data_directory,
			(char *)0 /* filename_stem */,
			application_name,
			0 /* process_id */,
			session_key,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			(char *)0 /* extension */ );

/*
#define GOOGLE_EARTH_OUTPUT_UNCOMPRESSED_KML_TEMPLATE	\
					"%s/%s/google_earth_kml_%s.kml"
#define GOOGLE_EARTH_OUTPUT_UNCOMPRESSED_KMZ_TEMPLATE	\
					"%s/%s/google_earth_kmz_%s.kml"
#define GOOGLE_EARTH_OUTPUT_KMZ_TEMPLATE	"%s/%s/google_earth_kmz_%s.kmz"
#define GOOGLE_EARTH_PROMPT_KML_TEMPLATE	"/%s/google_earth_kml_%s.kml"
#define GOOGLE_EARTH_PROMPT_KMZ_TEMPLATE	"/%s/google_earth_kmz_%s.kmz"
#define GOOGLE_EARTH_HTTP_PROMPT_KML_TEMPLATE	"%s://%s/%s/google_earth_kml_%s.kml"
#define GOOGLE_EARTH_HTTP_PROMPT_KMZ_TEMPLATE	"%s://%s/%s/google_earth_kmz_%s.kmz"
*/

	appaserver_link->filename_stem = FILENAME_STEM_KML;
	appaserver_link->extension = "kml";

	*output_uncompressed_kml_filename =
		appaserver_link_output_filename(
			appaserver_link->document_root_directory,
			appaserver_link_output_tail_half(
				appaserver_link->application_name,
				appaserver_link->filename_stem,
				appaserver_link->begin_date_string,
				appaserver_link->end_date_string,
				appaserver_link->process_id,
				appaserver_link->session_key,
				appaserver_link->extension ) );

	appaserver_link->filename_stem = FILENAME_STEM_KMZ;
	appaserver_link->extension = "kml";

	*output_uncompressed_kmz_filename =
		appaserver_link_output_filename(
			appaserver_link->document_root_directory,
			appaserver_link_output_tail_half(
				appaserver_link->application_name,
				appaserver_link->filename_stem,
				appaserver_link->begin_date_string,
				appaserver_link->end_date_string,
				appaserver_link->process_id,
				appaserver_link->session_key,
				appaserver_link->extension ) );

	appaserver_link->filename_stem = FILENAME_STEM_KMZ;
	appaserver_link->extension = "kmz";

	*output_kmz_filename =
		appaserver_link_output_filename(
			appaserver_link->document_root_directory,
			appaserver_link_output_tail_half(
				appaserver_link->application_name,
				appaserver_link->filename_stem,
				appaserver_link->begin_date_string,
				appaserver_link->end_date_string,
				appaserver_link->process_id,
				appaserver_link->session_key,
				appaserver_link->extension ) );

	appaserver_link->filename_stem = FILENAME_STEM_KML;
	appaserver_link->extension = "kml";

	*prompt_kml_filename =
		appaserver_link_prompt_filename(
			appaserver_link_prompt_link_half(
				appaserver_link->http_prefix,
				appaserver_link->server_address ),
			appaserver_link->application_name,
			appaserver_link->filename_stem,
			appaserver_link->begin_date_string,
			appaserver_link->end_date_string,
			appaserver_link->process_id,
			appaserver_link->session_key,
			appaserver_link->extension );

	appaserver_link->filename_stem = FILENAME_STEM_KMZ;
	appaserver_link->extension = "kmz";

	*prompt_kmz_filename =
		appaserver_link_prompt_filename(
			appaserver_link_prompt_link_half(
				appaserver_link->http_prefix,
				appaserver_link->server_address ),
			appaserver_link->application_name,
			appaserver_link->filename_stem,
			appaserver_link->begin_date_string,
			appaserver_link->end_date_string,
			appaserver_link->process_id,
			appaserver_link->session_key,
			appaserver_link->extension );
}
#endif

