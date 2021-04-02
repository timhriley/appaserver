/* ---------------------------------------------------	*/
/* src_alligator/output_visit_spreadsheet.c		*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

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
#include "hash_table.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "appaserver_link_file.h"
#include "alligator.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define FILENAME_STEM_CODES	"visit_spreadsheet_with_codes"
#define FILENAME_STEM_NO_CODES	"visit_spreadsheet_no_codes"

/* Prototypes */
/* ---------- */
void output_visit_spreadsheet(
				char *application_name,
				char *discovery_date_list_string,
				char *primary_researcher_list_string,
				char *document_root_directory,
				int process_id,
				char *process_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *discovery_date_list_string;
	char *primary_researcher_list_string;

	if ( argc != 5 )
	{
		fprintf( stderr, 
"Usage: %s application process_name discovery_date primary_researcher\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	discovery_date_list_string = argv[ 3 ];
	primary_researcher_list_string = argv[ 4 ];

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

	output_visit_spreadsheet(
				application_name,
				discovery_date_list_string,
				primary_researcher_list_string,
				appaserver_parameter_file->
					document_root,
				getpid(),
				process_name );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */


void output_visit_spreadsheet(	char *application_name,
				char *discovery_date_list_string,
				char *primary_researcher_list_string,
				char *document_root_directory,
				int process_id,
				char *process_name )
{
	char *ftp_codes_filename;
	char *output_codes_filename;
	FILE *output_codes_file;
	FILE *output_no_codes_file;
	char *ftp_no_codes_filename;
	char *output_no_codes_filename;
	char title[ 128 ];
	ALLIGATOR *alligator;
	NEST *nest;
	OBSERVATION *observation;
	char total_eggs_string[ 16 ];
	char banded_eggs_string[ 16 ];
	char not_banded_eggs_string[ 16 ];
	APPASERVER_LINK_FILE *appaserver_link_file;

	format_initial_capital( title, process_name );

	printf( "<h1>%s</h1>\n", title );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

	if ( !* discovery_date_list_string
	||   strcmp( discovery_date_list_string, "discovery_date" ) == 0 )
	{
		printf(
		"<h3>Please select one or more alligator censuses.</h3>\n" );
		document_close();
		exit( 0 );
	}

	alligator = alligator_new(
				application_name,
				discovery_date_list_string,
				primary_researcher_list_string,
				(char *)0 /* begin_discovery_date */,
				(char *)0 /* end_discovery_date */,
				0 /* not with_secondary_researchers */,
				1 /* with_nest_list */,
				1 /* with_observation_list */ );

	if ( !list_rewind( alligator->nest_list ) )
	{
		printf(
		"<h3>Warning: no alligator nests to select.</h3>\n" );
		document_close();
		exit( 0 );
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
			"csv" );

	appaserver_link_file->filename_stem = FILENAME_STEM_CODES;

	output_codes_filename =
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

	ftp_codes_filename =
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

	appaserver_link_file->filename_stem = FILENAME_STEM_NO_CODES;

	output_no_codes_filename =
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

	ftp_no_codes_filename =
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

	if ( ! ( output_codes_file = fopen( output_codes_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			output_codes_filename );
		document_close();
		exit( 1 );
	}

	if ( ! ( output_no_codes_file =
			fopen( output_no_codes_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			output_no_codes_filename );
		document_close();
		exit( 1 );
	}

	fprintf( output_codes_file,
"waypoint,year,Nest_visit_date,Survey_method_type,Transect_number,Nest_number,Selected for monitoring,Nest_visit_number,Total_eggs,Banded Eggs,NOT Banded Eggs,Hatched_eggs,Flooded_eggs,Predated_eggs,Crushed_eggs,Other egg mortality,Removed_viable_eggs,Removed_nonviable_eggs,Female_size cm,Female behavior code,Nest Habitat Code,Nest Construction code,Nest Status Code,Hatchlings Observed,Water Depth 1 (cm),Water Depth 2 (cm),Water Depth 3 (cm),Water Depth 4 (cm),Water Depth (avg. cm),Nest_height_cm,Nest_height_above_water_cm,Comments\n" );

	fprintf( output_no_codes_file,
",Year,Visit Date,Observation View,Transect,Nest Number,Monitoring?,Nest Visit Number,Total Eggs,Banded,NOT Banded,Hatched,Flooded,Predated,Crushed,Other Egg Mortality,Removed Viable Eggs,Removed Nonviable Eggs,Female Size (cm),Female Behavior,Nest Habitat,Nest Materials,Nest Status,Hatchlings Observed,Water Depth 1 (cm),Water Depth 2 (cm),Water Depth 3 (cm),Water Depth 4 (cm),Water Depth Avg. (cm),Nest Height (cm),Nest Height Above Water (cm),Comments\n" );


	do {
		nest = list_get_pointer( alligator->nest_list );

		if ( !list_rewind( nest->observation_list ) ) continue;

		nest->monitor_nest_yn = toupper( nest->monitor_nest_yn );

		do {
			observation =
				list_get_pointer(
					nest->observation_list );

			if ( atoi( observation->nest_visit_number ) == 1 )
			{
				strcpy( total_eggs_string,
					nest->total_eggs_string );
				strcpy( banded_eggs_string,
					nest->banded_eggs_string );
				strcpy( not_banded_eggs_string,
					nest->not_banded_eggs_string );
			}
			else
			{
				*total_eggs_string = '\0';
				*banded_eggs_string = '\0';
				*not_banded_eggs_string = '\0';
			}

			fprintf( output_codes_file,
",%s,%s,%s,%s,%d,%c,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
			 nest->alligator_census->discovery_year,
			 observation->observation_date,
			 observation->observation_view,
			 nest->transect_number,
			 nest->nest_number,
			 nest->monitor_nest_yn,
			 observation->nest_visit_number,
			 total_eggs_string,
			 banded_eggs_string,
			 not_banded_eggs_string,
			 observation->hatched_eggs_string,
			 observation->flooded_eggs_string,
			 observation->predated_eggs_string,
			 observation->crushed_eggs_string,
			 observation->other_egg_mortality_string,
			 observation->removed_viable_eggs_string,
			 observation->removed_non_viable_eggs_string,
			 nest->female_size_cm_string,
			 alligator_get_female_behavior_code(
				application_name,
				observation->female_behavior ),
			 alligator_get_habitat_code(
				application_name,
				nest->habitat ),
			 alligator_get_materials_code(
				application_name,
				nest->materials ),
			 alligator_get_nest_status_code(
				application_name,
				observation->nest_status ),
			 observation->hatchlings_count_string );

			fprintf( output_no_codes_file,
",%s,%s,%s,%s,%d,%c,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,\"%s\",\"%s\",\"%s\",\"%s\",%s",
			 nest->alligator_census->discovery_year,
			 observation->observation_date,
			 format_initial_capital(
			 	observation->observation_view,
			 	observation->observation_view ),
			 nest->transect_number,
			 nest->nest_number,
			 nest->monitor_nest_yn,
			 observation->nest_visit_number,
			 total_eggs_string,
			 banded_eggs_string,
			 not_banded_eggs_string,
			 observation->hatched_eggs_string,
			 observation->flooded_eggs_string,
			 observation->predated_eggs_string,
			 observation->crushed_eggs_string,
			 observation->other_egg_mortality_string,
			 observation->removed_viable_eggs_string,
			 observation->removed_non_viable_eggs_string,
			 nest->female_size_cm_string,
			 format_initial_capital(
			 	observation->female_behavior,
			 	observation->female_behavior ),
			 format_initial_capital(
			 	nest->habitat,
			 	nest->habitat ),
			 format_initial_capital(
			 	nest->materials,
			 	nest->materials ),
			 format_initial_capital(
			 	observation->nest_status,
			 	observation->nest_status ),
			 observation->hatchlings_count_string );

			if ( !list_rewind( observation->water_depth_list ) )
			{
				fprintf( output_codes_file,
					 ",,,," );
				fprintf( output_no_codes_file,
					 ",,,," );

			}
			else
			{
				WATER_DEPTH *water_depth;
				int count = 0;

				do {
					water_depth =
						list_get_pointer(
							observation->
							water_depth_list );

					fprintf( output_codes_file,
						 ",%d",
						 water_depth->water_depth_cm );

					fprintf( output_no_codes_file,
						 ",%d",
						 water_depth->water_depth_cm );

					if ( ++count == 4 ) break;

				} while( list_next(	observation->
							water_depth_list ) );

				while( count < 4 )
				{
					fprintf( output_codes_file,
						 "," );
					fprintf( output_no_codes_file,
						 "," );
					count++;
				}
			}

			fprintf( output_codes_file,
				 ",%s,%s,%s,\"%s\"",
			 observation->water_depth_average_cm_string,
			 nest->nest_height_cm_string,
			 observation->nest_height_above_water_cm_string,
			 observation->observation_notepad );

			fprintf( output_no_codes_file,
				 ",%s,%s,%s,\"%s\"",
			 observation->water_depth_average_cm_string,
			 nest->nest_height_cm_string,
			 observation->nest_height_above_water_cm_string,
			 observation->observation_notepad );

			fprintf( output_codes_file, "\n" );
			fprintf( output_no_codes_file, "\n" );

		} while ( list_next( nest->observation_list ) );

	} while( list_next( alligator->nest_list ) );

	fclose( output_codes_file );
	fclose( output_no_codes_file );

	appaserver_library_output_ftp_prompt(
			ftp_codes_filename,
"With codes: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	appaserver_library_output_ftp_prompt(
			ftp_no_codes_filename,
"Without codes: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

} /* output_visit_spreadsheet() */

