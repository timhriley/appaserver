/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/google_earth.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef GOOGLE_EARTH_H
#define GOOGLE_EARTH_H

#include <stdio.h>
#include "list.h"

#define FILENAME_STEM_KML		"google_earth_kml"
#define FILENAME_STEM_KMZ		"google_earth_kmz"
#define GOOGLE_EARTH_CENTER_LATITUDE	25.3
#define GOOGLE_EARTH_CENTER_LONGITUDE	-80.7
#define GOOGLE_EARTH_ALTITUDE		20
#define GOOGLE_EARTH_RANGE		100000
#define GOOGLE_EARTH_TILT		0
#define GOOGLE_EARTH_HEADING		0
#define GOOGLE_EARTH_ALTITUDE_MODE	"relativeToGround"
#define GOOGLE_EARTH_ICON_SCALE		0.7
#define GOOGLE_EARTH_ICON_COLOR		"ff33ff00"
#define GOOGLE_EARTH_ICON_COLOR_MODE	"normal"
#define GOOGLE_EARTH_POINT_EXTRUDE	1

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

#define GOOGLE_EARTH_ICON_IMAGE		\
	"<href>http://maps.google.com/mapfiles/kml/pal4/icon24.png</href>"

/* Structures */
/* ---------- */
typedef struct 
{
	char *placemark_name;
	char *placemark_description;
	double latitude;
	double longitude;
} GOOGLE_EARTH_PLACEMARK;

typedef struct
{
	char *document_name;
	char *folder_name;
	char *folder_description;
	double center_latitude;
	double center_longitude;
	LIST *placemark_list;
} GOOGLE_EARTH;

/* Prototypes */
/* ---------- */
GOOGLE_EARTH *google_earth_new(		char *document_name,
					char *folder_name,
					char *folder_description,
					double center_latitude,
					double center_longitude );

GOOGLE_EARTH_PLACEMARK *google_earth_placemark_new(
					void );

void google_earth_set_placemark(	LIST *placemark_list,
					char *placemark_name,
					char *placemark_description,
					double latitude,
					double longitude );

void google_earth_output_heading(	FILE *output_file,
					char *document_name,
					char *folder_name,
					char *folder_description,
					double center_latitude,
					double center_longitude );

void google_earth_output_placemark_list(FILE *output_file,
					LIST *placemark_list );

void google_earth_output_placemark(	FILE *output_file,
					char *placemark_name,
					char *placemark_description,
					double latitude,
					double longitude );

void google_earth_output_closing(	FILE *output_file );

void google_earth_get_filenames(
					char **output_uncompressed_kml_filename,
					char **output_uncompressed_kmz_filename,
					char **output_kmz_filename,
					char **prompt_kml_filename,
					char **prompt_kmz_filename,
					char *application_name,
					char *document_root_directory,
					char *session );

#endif
