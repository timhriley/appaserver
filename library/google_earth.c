/* library/google_earth.c */
/* ---------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "google_earth.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_link_file.h"

GOOGLE_EARTH *google_earth_new(		char *document_name,
					char *folder_name,
					char *folder_description,
					double center_latitude,
					double center_longitude )
{
	GOOGLE_EARTH *google_earth;

	google_earth =
		(GOOGLE_EARTH *)
			calloc( 1, sizeof( GOOGLE_EARTH ) );

	google_earth->document_name = document_name;
	google_earth->folder_name = folder_name;
	google_earth->folder_description = folder_description;
	google_earth->center_latitude = center_latitude;
	google_earth->center_longitude = center_longitude;
	google_earth->placemark_list = list_new();

	return google_earth;

} /* google_earth_new() */

GOOGLE_EARTH_PLACEMARK *google_earth_placemark_new( void )
{
	GOOGLE_EARTH_PLACEMARK *google_earth_placemark;

	if ( ! ( google_earth_placemark =
		(GOOGLE_EARTH_PLACEMARK *)
			calloc( 1, sizeof( GOOGLE_EARTH_PLACEMARK ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return google_earth_placemark;

} /* google_earth_placemark_new() */

void google_earth_set_placemark(	LIST *placemark_list,
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

} /* google_earth_set_placemark() */

void google_earth_output_heading(	FILE *output_file,
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

} /* google_earth_output_heading() */

void google_earth_output_placemark_list(FILE *output_file,
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

} /* google_earth_output_placemark_list() */

void google_earth_output_placemark(	FILE *output_file,
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

} /* google_earth_output_placemark() */

#ifdef NOT_DEFINED
void google_earth_output_placemark(	FILE *output_file,
					char *placemark_name,
					char *placemark_description,
					double latitude,
					double longitude )
{
	static int counter = 0;

	fprintf( output_file,
"<Placemark>\n"
"\t<name>%s</name>\n"
"\t<description>\n"
"\t<![CDATA[\n"
"\t%s\n"
"\t]]>\n"
"\t</description>\n"
"\n"
"\t<Style id=\"inline_copy%d\">\n"
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
		 placemark_description,
		 counter++,
		 GOOGLE_EARTH_ICON_SCALE,
		 GOOGLE_EARTH_ICON_COLOR,
		 GOOGLE_EARTH_ICON_COLOR_MODE,
		 GOOGLE_EARTH_ICON_IMAGE,
		 GOOGLE_EARTH_POINT_EXTRUDE,
		 GOOGLE_EARTH_ALTITUDE_MODE,
		 longitude,
		 latitude );

} /* google_earth_output_placemark() */
#endif

void google_earth_output_closing( FILE *output_file )
{
	fprintf( output_file,
"</Folder>\n"
"</Document>\n"
"</kml>\n" );

} /* google_earth_output_closing() */

void google_earth_get_filenames(
			char **output_uncompressed_kml_filename,
			char **output_uncompressed_kmz_filename,
			char **output_kmz_filename,
			char **prompt_kml_filename,
			char **prompt_kmz_filename,
			char *application_name,
			char *document_root_directory,
			char *session )
{
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			(char *)0 /* filename_stem */,
			application_name,
			0 /* process_id */,
			session,
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
/*
	sprintf(	local_output_uncompressed_kml_filename,
			GOOGLE_EARTH_OUTPUT_UNCOMPRESSED_KML_TEMPLATE,
			appaserver_mount_point,
			application_name,
			session );
*/

	appaserver_link_file->filename_stem = FILENAME_STEM_KML;
	appaserver_link_file->extension = "kml";

	*output_uncompressed_kml_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

/*
	sprintf(	local_output_uncompressed_kmz_filename,
			GOOGLE_EARTH_OUTPUT_UNCOMPRESSED_KMZ_TEMPLATE,
			appaserver_mount_point,
			application_name,
			session );
*/

	appaserver_link_file->filename_stem = FILENAME_STEM_KMZ;
	appaserver_link_file->extension = "kml";

	*output_uncompressed_kmz_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

/*
	sprintf(	local_output_kmz_filename,
			GOOGLE_EARTH_OUTPUT_KMZ_TEMPLATE,
			appaserver_mount_point,
			application_name,
			session );
*/

	appaserver_link_file->filename_stem = FILENAME_STEM_KMZ;
	appaserver_link_file->extension = "kmz";

	*output_kmz_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	appaserver_link_file->filename_stem = FILENAME_STEM_KML;
	appaserver_link_file->extension = "kml";

	*prompt_kml_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	appaserver_link_file->filename_stem = FILENAME_STEM_KMZ;
	appaserver_link_file->extension = "kmz";

	*prompt_kmz_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

} /* google_earth_get_filenames() */

