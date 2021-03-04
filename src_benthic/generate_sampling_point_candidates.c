/* ------------------------------------------------------*/
/* src_benthic/generate_sampling_point_candidates.c	 */
/* ------------------------------------------------------*/
/* Freely available software: see Appaserver.org	 */
/* ------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
#include "date_convert.h"
#include "piece.h"
#include "query.h"
#include "list.h"
#include "dictionary.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "hashtbl.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "benthic_library.h"
#include "google_map.h"
#include "benthic_waypoint.h"
#include "waypoint.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define SHORT_LOCATION_SIZE		3
#define INACCESSIBLE_STRING 		" (inaccessible)"
#define SELECTED_STRING 		" (selected)"

#define FILENAME_STEM_MAPSOURCE		"mapsource"
#define FILENAME_STEM_PRINT		"waypoint_print"

/* Prototypes */
/* ---------- */
char *get_location_code(	char *project,
				char *location,
				char *application_name );

void output_print_rows(		char *print_output_filename,
				LIST *waypoint_site_list );

void output_print_heading(	char *print_output_filename );

void delete_sampling_point_candidates(
				char *application_name,
				char *collection_name,
				char *project,
				char *location );

boolean exists_sampling_point_candidates(
				char *application_name,
				char *collection_name,
				char *project,
				char *location );

boolean generate_sampling_point_candidates(
				char *application_name,
				char *collection_name,
				char *project,
				char *location,
				boolean replace_existing_waypoints );

void output_mapsource_waypoint(
				FILE *output_file,
				BENTHIC_WAYPOINT *waypoint,
				char *location_code,
				int site_number,
				int position_number );

void output_mapsource_heading(	char *mapsource_output_filename );

void output_mapsource_rows(	char *mapsource_output_filename,
				LIST *waypoint_site_list,
				char *location_code );

void output_sampling_point_candidates(
				char *application_name,
				char *document_root_directory,
				pid_t process_id,
				char *collection_name,
				char *project,
				char *location );

void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name,
				char *location,
				char *project,
				char *collection_name );


int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *location;
	char *project;
	char *collection_name;
	boolean replace_existing_waypoints;
	char title[ 128 ];
	char sub_title[ 256 ];

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s application process_name location project collection_name replace_existing_waypoints_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	location = argv[ 3 ];
	project = argv[ 4 ];
	collection_name = argv[ 5 ];
	replace_existing_waypoints = ( *argv[ 6 ] == 'y' );

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

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
	
	document_output_body(
				document->application_name,
				document->onload_control_string );

	get_title_and_sub_title(
			title,
			sub_title,
			process_name,
			location,
			project,
			collection_name );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

	if ( !*collection_name
	||   strcmp( collection_name, "collection_name" ) == 0 )
	{
		printf(
		"<h3>Please choose a collection.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !*location
	||   strcmp( location, "location" ) == 0 )
	{
		printf(
		"<h3>Please choose a location.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !generate_sampling_point_candidates(
				application_name,
				collection_name,
				project,
				location,
				replace_existing_waypoints ) )
	{
		printf(
"<h3>Error: no waypoints exist for this project/site. Contact timriley@appahost.com</h3>\n" );
		document_close();
		exit( 0 );
	}

	output_sampling_point_candidates(
				application_name,
				appaserver_parameter_file->
					document_root,
				getpid(),
				collection_name,
				project,
				location );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

void output_sampling_point_candidates(
				char *application_name,
				char *document_root_directory,
				pid_t process_id,
				char *collection_name,
				char *project,
				char *location )
{
	char *mapsource_output_filename;
	char *mapsource_ftp_filename;
	char *print_output_filename;
	char *print_ftp_filename;
	WAYPOINT_SITE *waypoint_site;
	int site_number;
	char *location_code;
	char location_filename[ 128 ];
	LIST *waypoint_site_list = list_new();
	APPASERVER_LINK_FILE *appaserver_link_file;

	if ( ! ( location_code = get_location_code(
					project,
					location,
					application_name ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot get location_code.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	timlib_strcpy( location_filename, location, 128 );
	search_replace_string( location_filename, " ", "_" );

	for( site_number = 1; site_number <= SITES_PER_LOCATION; site_number++ )
	{
		if ( ! ( waypoint_site = waypoint_site_new(
				application_name,
				location,
				project,
				site_number ) ) )
		{
			printf( "<h3>An Error Occurred.</h3>\n" );
			return;
		}

		if ( !waypoint_get_sampling_point_candidate_array(
				waypoint_site->sampling_point_candidate_array,
				waypoint_site->waypoint_array,
				waypoint_site->project,
				waypoint_site->location,
				waypoint_site->site_number,
				collection_name ) )
		{
				printf( "<h3>An Error Occurred.</h3>\n" );
				return;
		}

		list_append_pointer( waypoint_site_list, waypoint_site );
	}

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			(char *)0 /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			(char *)0 /* extension */ );

	appaserver_link_file->begin_date_string = location_filename;
	appaserver_link_file->end_date_string = collection_name;

	/* Create Mapsource file */
	/* --------------------- */
	appaserver_link_file->filename_stem = FILENAME_STEM_MAPSOURCE;
	appaserver_link_file->extension = "txt";

	mapsource_output_filename =
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

	mapsource_ftp_filename =
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

	/* Create Print file */
	/* ----------------- */
	appaserver_link_file->filename_stem = FILENAME_STEM_PRINT;
	appaserver_link_file->extension = "csv";

	print_output_filename =
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

	print_ftp_filename =
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

	output_mapsource_heading( mapsource_output_filename );

	output_mapsource_rows(	mapsource_output_filename,
				waypoint_site_list,
				location_code );

	appaserver_library_output_ftp_prompt(
			mapsource_ftp_filename,
"Mapsource: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

	output_print_heading( print_output_filename );

	output_print_rows(	print_output_filename,
				waypoint_site_list );

	appaserver_library_output_ftp_prompt(
			print_ftp_filename,
"Print: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

} /* output_sampling_point_candidates() */

void get_title_and_sub_title(
			char *title,
			char *sub_title,
			char *process_name,
			char *collection,
			char *project,
			char *collection_name )
{
	format_initial_capital( title, process_name );

	sprintf( sub_title,
		 "%s/%s/%s",
		 collection,
		 project,
		 collection_name );

} /* get_title_and_sub_title() */

void output_mapsource_heading( char *mapsource_output_filename )
{
	char sys_string[ 1024 ];

	sprintf(	sys_string,
"echo \"F,ID-----,Latitude,Longitude,Symbol-----------,T,Comment\" > %s",
			mapsource_output_filename );
	system( sys_string );
} /* output_mapsource_heading() */

void output_print_heading( char *print_output_filename )
{
	char sys_string[ 1024 ];

	sprintf(	sys_string,
"echo \"Site,,Alternate 3,,Alternate 4,,Alternate 5\" > %s",
			print_output_filename );
	system( sys_string );
} /* output_print_heading() */

void output_print_rows(		char *print_output_filename,
				LIST *waypoint_site_list )
{
	WAYPOINT_SITE *waypoint_site;
	BENTHIC_WAYPOINT *waypoint;
	FILE *output_file;
	int position_offset;
	char longitude_display[ 256 ];

	if ( !list_rewind( waypoint_site_list ) ) return;

	if ( ! ( output_file = fopen( print_output_filename, "a" ) ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: cannot open %s for append.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 print_output_filename );
		return;
	}

	do {
		waypoint_site = list_get_pointer( waypoint_site_list );

		fprintf(	output_file,
				"%s%d",
				waypoint_site->location,
				waypoint_site->site_number );

		for(	position_offset = 2;
			position_offset < CANDIDATES_PER_SITE_COLLECTION;
			position_offset++ )
		{
			waypoint =
				waypoint_site->sampling_point_candidate_array
					[ position_offset ];

			if ( waypoint )
			{
				timlib_strcpy(	longitude_display,
						waypoint->longitude,
						256 );

				if ( *longitude_display == '-' )
					*longitude_display = 'W';

				fprintf(	output_file,
						",,%s %s",
						waypoint->latitude,
						longitude_display );
			}
			else
			{
				fprintf(	output_file,
						",," );
			}
		}

		fprintf( output_file, "\n" );

	} while( list_next( waypoint_site_list ) );

	fclose( output_file );

} /* output_print_rows() */

void output_mapsource_rows(	char *mapsource_output_filename,
				LIST *waypoint_site_list,
				char *location_code )
{
	WAYPOINT_SITE *waypoint_site;
	BENTHIC_WAYPOINT *waypoint;
	FILE *output_file;

	if ( !list_rewind( waypoint_site_list ) ) return;

	if ( ! ( output_file = fopen( mapsource_output_filename, "a" ) ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: cannot open %s for append.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 mapsource_output_filename );
		return;
	}

	do {
		waypoint_site = list_get_pointer( waypoint_site_list );

		/* Primary site */
		/* ------------ */
		waypoint = waypoint_site->sampling_point_candidate_array[ 0 ];

		output_mapsource_waypoint(
				output_file,
				waypoint,
				location_code,
				waypoint_site->site_number,
				1 /* position_number */ );

		/* First alternate */
		/* --------------- */
		waypoint = waypoint_site->sampling_point_candidate_array[ 1 ];

		output_mapsource_waypoint(
				output_file,
				waypoint,
				location_code,
				waypoint_site->site_number,
				2 /* position_number */ );

	} while( list_next( waypoint_site_list ) );

	fclose( output_file );

} /* output_mapsource_rows() */

void output_mapsource_waypoint(
			FILE *output_file,
			BENTHIC_WAYPOINT *waypoint,
			char *location_code,
			int site_number,
			int position_number )
{
	double longitude_degrees_float;
	double latitude_degrees_float;
	char longitude_display[ 128 ];
	char *inaccessible_string;
	char *selected_string;

	if ( waypoint && waypoint->inaccessible )
		inaccessible_string = INACCESSIBLE_STRING;
	else
		inaccessible_string = "";

	selected_string = SELECTED_STRING;

	fprintf( output_file,
		 "W,%s%.2d-%d%s%s",
		 location_code,
		 site_number,
		 position_number,
		 inaccessible_string,
		 selected_string );

	if ( waypoint )
	{
		longitude_degrees_float =
			google_map_convert_base_60_with_float(
				waypoint->longitude );

		sprintf( longitude_display, "%.7lf", longitude_degrees_float );
		search_replace_string( longitude_display, "-", "" );

		latitude_degrees_float =
			google_map_convert_base_60_with_float(
				waypoint->latitude );

		fprintf( output_file,
			 ",N%.7lf,W0%s,Waypoint,I,\n",
			 latitude_degrees_float,
			 longitude_display );
	}
	else
	{
		fprintf( output_file,
			 ",Not Set,Not Set,Waypoint,I,\n" );
	}

} /* output_mapsource_waypoint() */

boolean exists_sampling_point_candidates(
				char *application_name,
				char *collection_name,
				char *project,
				char *location )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char *folder;

	folder = "sampling_point_candidate";

	sprintf( where,
		 "location = '%s' and		"
		 "project = '%s' and		"
		 "collection_name = '%s'	",
		 location,
		 project,
		 collection_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=count		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 folder,
		 where );

	return atoi( pipe2string( sys_string ) );

} /* exists_sampling_point_candidates() */

void delete_sampling_point_candidates(
				char *application_name,
				char *collection_name,
				char *project,
				char *location )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char *table_name;

	table_name = get_table_name(
			application_name, "sampling_point_candidate" );

	sprintf( where,
		 "location = '%s' and		"
		 "project = '%s' and		"
		 "collection_name = '%s'	",
		 location,
		 project,
		 collection_name );

	sprintf( sys_string,
		 "echo \"delete from %s where %s;\" | sql.e",
		 table_name,
		 where );

	system( sys_string );

} /* delete_sampling_point_candidates() */

boolean generate_sampling_point_candidates(
				char *application_name,
				char *collection_name,
				char *project,
				char *location,
				boolean replace_existing_waypoints )
{
	WAYPOINT_SITE *waypoint_site;
	int site_number;
	boolean exists;
	BENTHIC_WAYPOINT **optimized_sampling_point_candidate_array = {0};
	short unsigned int seed;

	if ( ( exists = exists_sampling_point_candidates(
				application_name,
				collection_name,
				project,
				location ) )
	&&   !replace_existing_waypoints )
	{
		return 1;
	}

	seed = timlib_get_seed();
	seed48( &seed );

	if ( exists )
	{
		delete_sampling_point_candidates(
				application_name,
				collection_name,
				project,
				location );
	}

	for( site_number = 1; site_number <= SITES_PER_LOCATION; site_number++ )
	{
		if ( ! ( waypoint_site = waypoint_site_new(
				application_name,
				location,
				project,
				site_number ) ) )
		{
			return 0;
		}

		waypoint_populate_random_sampling_point_candidate_array(
				waypoint_site->sampling_point_candidate_array,
				waypoint_site->waypoint_array );

		optimized_sampling_point_candidate_array =
			waypoint_optimize_random_sampling_point_candidate_array(
			     waypoint_site->sampling_point_candidate_array );

		waypoint_save_random_sampling_point_candidate_array(
				application_name,
				optimized_sampling_point_candidate_array,
				waypoint_site->location,
				waypoint_site->project,
				waypoint_site->site_number,
				collection_name );
	}

	return 1;

} /* generate_sampling_point_candidates() */

char *get_location_code(	char *project,
				char *location,
				char *application_name )
{
	char sys_string[ 1024 ];
	char where[ 128 ];

	sprintf(	where,
			"project = '%s' and location = '%s'",
			project,
			location );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=location_code	"
		 "			folder=location_project	"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	return pipe2string( sys_string );

} /* get_location_code() */

