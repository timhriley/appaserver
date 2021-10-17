/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/google_earth_station_operation.c	*/
/* ---------------------------------------------------------------	*/
/* This is the operation process for the google earth			*/
/* station button.							*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "folder.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "date.h"
#include "station.h"
#include "document.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "google_earth.h"
#include "google_earth_station.h"
#include "application_constants.h"
#include "operation.h"
#include "appaserver_link.h"

/* Constants */
/* --------- */
#define FILENAME_STEM		"google_earth_station"

/* #define WITH_WINDOW_OPEN	1 */
/*
#define SPOOL_TEMPLATE		"%s/%s/google_earth_station_%s.dat"
*/

/* Prototypes */
/* ---------- */
void zip_kmz_file(	char *output_kmz_filename,
			char *output_kml_filename,
			LIST *spool_station_name_list,
			char *application_name,
			char *document_root_directory );

void google_earth_station_operation(
			char *application_name,
			char *station_name,
			char *session,
			char *document_root_directory,
			OPERATION_SEMAPHORE *operation_semaphore );

void spool_station(	char *station_name,
			boolean group_first_time,
			char *document_root_directory,
			char *application_name,
			char *parent_process_id_string );

LIST *get_spool_station_name_list(
			char *document_root_directory,
			char *application_name,
			char *parent_process_id_string );

char *get_spool_filename(
			char *document_root_directory,
			char *application_name,
			char *parent_process_id_string );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *station_name;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *parent_process_id_string;
	char *operation_row_count_string;
	char *session;
	OPERATION_SEMAPHORE *operation_semaphore;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s ignored process station process_id operation_row_count session\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	/* application_name = argv[ 1 ]; */
	process_name = argv[ 2 ];
	station_name = argv[ 3 ];
	parent_process_id_string = argv[ 4 ];
	operation_row_count_string = argv[ 5 ];
	session = argv[ 6 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	operation_semaphore =
		operation_semaphore_new(
			application_name,
			process_name,
			parent_process_id_string,
			appaserver_parameter_file->
				appaserver_data_directory,
			operation_row_count_string );

	if ( operation_semaphore->group_first_time )
	{
		DOCUMENT *document;

		document = document_new( "", application_name );
		document_set_output_content_type( document );

		document_output_head(
				document->application_name,
				document->title,
				document->output_content_type,
				appaserver_parameter_file->
					appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );
	}

	google_earth_station_operation(
			application_name,
			station_name,
			session,
			appaserver_parameter_file->
				document_root,
			operation_semaphore );

	spool_station(	station_name,
			operation_semaphore->group_first_time,
			appaserver_parameter_file->
				document_root,
			application_name,
			parent_process_id_string );

	if ( operation_semaphore->group_last_time )
	{
		char *output_uncompressed_kml_filename;
		char *output_uncompressed_kmz_filename;
		char *output_kmz_filename;
		char *prompt_kml_filename;
		char *prompt_kmz_filename;
		LIST *spool_station_name_list;
		char window_name[ 128 ];

		google_earth_get_filenames(
				&output_uncompressed_kml_filename,
				&output_uncompressed_kmz_filename,
				&output_kmz_filename,
				&prompt_kml_filename,
				&prompt_kmz_filename,
				application_name,
				appaserver_parameter_file->
					document_root,
				session );

		spool_station_name_list =
			get_spool_station_name_list(
				appaserver_parameter_file->
					document_root,
				application_name,
				parent_process_id_string );

		zip_kmz_file(
			output_kmz_filename,
			output_uncompressed_kml_filename,
			spool_station_name_list,
			application_name,
			appaserver_parameter_file->
				document_root );

		sprintf( window_name, "%s", process_name );

#ifdef WITH_WINDOW_OPEN
		printf( "<body bgcolor=\"%s\" onload=\"",
			application_background_color( application_name ) );

		printf( "window.open('%s','%s');",
			prompt_kml_filename,
			window_name );
		printf( "\">\n" );
#else
		printf( "<body bgcolor=\"%s\">\n",
			application_background_color( application_name ) );
#endif

		printf( "<h1>Google Earth Station Viewer " );
		fflush( stdout );
		if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
		fflush( stdout );
		printf( "</h1>\n" );

		printf( "<br>\n" );
		printf(
		"<a href='%s' target=%s>Press to load portable file.</a>\n",
			prompt_kmz_filename,
			window_name );

		document_close();

		operation_semaphore_remove_file(
			operation_semaphore->semaphore_filename );
	}

	process_increment_execution_count(
			application_name,
			process_name,
			appaserver_parameter_file_get_dbms() );

	exit( 0 );

}

void google_earth_station_operation(
			char *application_name,
			char *station_name,
			char *session,
			char *document_root_directory,
			OPERATION_SEMAPHORE *operation_semaphore )
{
	char *output_uncompressed_kml_filename;
	char *output_uncompressed_kmz_filename;
	char *output_kmz_filename;
	char *prompt_kml_filename;
	char *prompt_kmz_filename;
	FILE *uncompressed_kml_output_file;
	FILE *uncompressed_kmz_output_file;
	STATION *station;
	char *mode;
	GOOGLE_EARTH *kml_google_earth;
	GOOGLE_EARTH *kmz_google_earth;

	google_earth_get_filenames(
				&output_uncompressed_kml_filename,
				&output_uncompressed_kmz_filename,
				&output_kmz_filename,
				&prompt_kml_filename,
				&prompt_kmz_filename,
				application_name,
				document_root_directory,
				session );

	if ( operation_semaphore->group_first_time )
		mode = "w";
	else
		mode = "a";

	if ( ! ( uncompressed_kml_output_file =
			fopen( output_uncompressed_kml_filename, mode ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot open %s for write.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			output_uncompressed_kml_filename );
		exit( 1 );
	}

	if ( ! ( uncompressed_kmz_output_file =
			fopen( output_uncompressed_kmz_filename, mode ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot open %s for write.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			output_uncompressed_kmz_filename );
		exit( 1 );
	}

	kml_google_earth =
		google_earth_new(
"Everglades National Park Hydrographic Stations",
"Hydro Stations",
"Locations and current conditions at Hydro Stations located in and around Everglades National Park",
		GOOGLE_EARTH_CENTER_LATITUDE,
		GOOGLE_EARTH_CENTER_LONGITUDE );

	kmz_google_earth =
		google_earth_new(
"Everglades National Park Hydrographic Stations",
"Hydro Stations",
"Locations and current conditions at Hydro Stations located in and around Everglades National Park",
		GOOGLE_EARTH_CENTER_LATITUDE,
		GOOGLE_EARTH_CENTER_LONGITUDE );

	if ( operation_semaphore->group_first_time )
	{
		google_earth_output_heading(
				uncompressed_kml_output_file,
				kml_google_earth->document_name,
				kml_google_earth->folder_name,
				kml_google_earth->folder_description,
				kml_google_earth->center_latitude,
				kml_google_earth->center_longitude );

		google_earth_output_heading(
				uncompressed_kmz_output_file,
				kmz_google_earth->document_name,
				kmz_google_earth->folder_name,
				kmz_google_earth->folder_description,
				kmz_google_earth->center_latitude,
				kmz_google_earth->center_longitude );

	}

	station = station_new( station_name );

	if ( !station_load(	
				&station->lat_nad83,
				&station->long_nad83,
				&station->agency,
				&station->ground_surface_elevation_ft,
				&station->ground_surface_elevation_null,
				&station->vertical_datum,
				&station->ngvd29_navd88_conversion,
				&station->station_type,
				&station->comments,
				application_name,
				station->station_name ) )
	{
		printf( "<p>Error: cannot find station = (%s)\n",
			station_name );
		document_close();
		fclose( uncompressed_kml_output_file );
		fclose( uncompressed_kmz_output_file );
		exit( 0 );
	}

	if ( !station->lat_nad83 || !station->long_nad83 )
	{
		printf(
"<h3>Error: either the latitude or longitude is not set for %s</h3>.\n",
			station->station_name ); 
	}
	else
	{
		google_earth_station_populate_placemark_list(
			kml_google_earth->placemark_list,
			station->station_name,
			application_name,
			(char *)0 /* document_root_directory */ );

		google_earth_output_placemark_list(
			uncompressed_kml_output_file,
			kml_google_earth->placemark_list );

		google_earth_station_populate_placemark_list(
			kmz_google_earth->placemark_list,
			station->station_name,
			application_name,
			document_root_directory );

		google_earth_output_placemark_list(
			uncompressed_kmz_output_file,
			kmz_google_earth->placemark_list );

	}

	if ( operation_semaphore->group_last_time )
	{
		google_earth_output_closing( uncompressed_kml_output_file );
		google_earth_output_closing( uncompressed_kmz_output_file );
	}

	fclose( uncompressed_kml_output_file );
	fclose( uncompressed_kmz_output_file );

}

void spool_station(	char *station_name,
			boolean group_first_time,
			char *document_root_directory,
			char *application_name,
			char *parent_process_id_string )
{
	FILE *spool_file;
	char *mode;
	char *spool_filename;

	spool_filename =
		get_spool_filename(
			document_root_directory,
			application_name,
			parent_process_id_string );

	if ( group_first_time )
		mode = "w";
	else
		mode = "a";

	if ( ! ( spool_file = fopen( spool_filename, mode ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: cannot open %s for write.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			spool_filename );
		exit( 1 );
	}

	fprintf( spool_file, "%s\n", station_name );
	fclose( spool_file );

}

char *get_spool_filename(
			char *document_root_directory,
			char *application_name,
			char *parent_process_id_string )
{
	char *spool_filename;
	APPASERVER_LINK *appaserver_link;

	appaserver_link =
		appaserver_link_new(
			application_http_prefix( application_name ),
			appaserver_library_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			FILENAME_STEM,
			application_name,
			0 /* process_id */,
			parent_process_id_string /* session */,
			(char *)0 /* begin_date_string */,
			(char *)0 /* end_date_string */,
			"dat" );

	spool_filename = appaserver_link->output->filename;

	return spool_filename;
}

LIST *get_spool_station_name_list(
			char *document_root_directory,
			char *application_name,
			char *parent_process_id_string )
{
	char *spool_filename;
	char sys_string[ 512 ];

	spool_filename =
		get_spool_filename(
			document_root_directory,
			application_name,
			parent_process_id_string );

	sprintf( sys_string, "cat %s", spool_filename );

	return pipe2list( sys_string );

}


void zip_kmz_file(	char *output_kmz_filename,
			char *output_kml_filename,
			LIST *spool_station_name_list,
			char *application_name,
			char *document_root_directory )
{
	char *station_name;
	char sys_string[ 65536 ];
	char *ptr = sys_string;
	char *jpg_tag;

	if ( !list_rewind( spool_station_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty spool_station_name_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf(	ptr,
			"zip -rq %s %s",
			output_kmz_filename,
			output_kml_filename );

	do {
		station_name = list_get( spool_station_name_list );

		if ( google_earth_station_image_exists(
					application_name,
					station_name ) )
		{
			ptr += sprintf(	ptr,
					" %s",
					google_earth_station_get_image_filename(
				       	     application_name,
				       	     station_name,
				       	     document_root_directory ) );
		}

		ptr += sprintf(	ptr,
				" %s",
				google_earth_station_get_pdf_filename(
				       application_name,
				       station_name,
				       document_root_directory ) );

		jpg_tag = google_earth_station_get_jpg_tag(
			       application_name,
			       station_name,
			       (char *)0 /* uname */,
			       document_root_directory );

		if ( jpg_tag && *jpg_tag )
		{
			ptr += sprintf(	ptr,
					" /%s",
					jpg_tag );
		}

	} while( list_next( spool_station_name_list ) );

	*ptr = '\0';

/*
fprintf( stderr, "%s\n", sys_string );
*/

	if ( system( sys_string ) ) {};

}

