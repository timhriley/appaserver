/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_creel/creel_spreadsheet_file.c	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
#include "piece.h"
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
#include "creel_library.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define FILENAME_STEM_WITH_CODES		"spreadsheet_with_codes"
#define FILENAME_STEM_NO_CODES			"spreadsheet_no_codes"

/* Prototypes */
/* ---------- */
void output_spreadsheet_fishing_trip_horizontally(
				FILE *output_codes_file,
				FILE *output_no_codes_file,
				FISHING_TRIPS fishing_trips,
				DATE *date,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *preferred_species_code,
				char *species_display );

void output_spreadsheet_fishing_trip_vertically(
				FILE *output_codes_file,
				FILE *output_no_codes_file,
				FISHING_TRIPS fishing_trips,
				DATE *date,
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *preferred_species_code,
				char *species_display );

void get_title_and_sub_title(
				char *title,
				char *sub_title,
				char *fishing_purpose,
				char *begin_date_string,
				char *end_date_string );

void output_spreadsheet(
				char *application_name,
				char *begin_date_string,
				char *end_date_string,
				char *fishing_purpose,
				char *document_root_directory,
				boolean with_between,
				char *display_data,
				pid_t process_id );

int main( int argc, char **argv )
{
	char *application_name;
	char *fishing_purpose;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *begin_date_string;
	char *end_date_string;
	boolean with_between;
	char *display_data;

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s application process_name fishing_purpose begin_date end_date %s|%s(display_data)\n",
			 argv[ 0 ],
			 DISPLAY_CATCHES_HORIZONTALLY,
			 DISPLAY_CATCHES_VERTICALLY );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	fishing_purpose = argv[ 3 ];

	begin_date_string = argv[ 4 ];
	end_date_string = argv[ 5 ];

	if ( !*end_date_string
	||   strcmp( end_date_string, "end_date" ) == 0 )
	{
		with_between = 0;
	}
	else
	{
		with_between = 1;
	}

	display_data = argv[ 6 ];

	if (	!*display_data
	||	strcmp( display_data, "display_data" ) == 0 )
	{
		display_data = DISPLAY_CATCHES_HORIZONTALLY;
	}

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

	if ( !*fishing_purpose
	||   strcmp( fishing_purpose, "fishing_purpose" ) == 0
	||   strcmp( fishing_purpose, "select" ) == 0 )
	{
		printf("<p>Please select a fishing purpose.</p>\n" );
		document_close();
		exit( 0 );
	}

	output_spreadsheet(
			application_name,
			begin_date_string,
			end_date_string,
			fishing_purpose,
			appaserver_parameter_file->
				document_root,
			with_between,
			display_data,
			getpid() );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
}


void output_spreadsheet(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *fishing_purpose,
			char *document_root_directory,
			boolean with_between,
			char *display_data,
			pid_t process_id )
{
	char *ftp_codes_filename;
	char *output_codes_filename;
	FILE *output_codes_file;
	char *ftp_no_codes_filename;
	char *output_no_codes_filename;
	FILE *output_no_codes_file;
	FISHING_TRIPS fishing_trips;
	char title[ 128 ];
	char sub_title[ 128 ];
	DATE *date;
	char species_display[ 128 ];
	char *preferred_species_code;
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
		   application_http_prefix(
				application_name ),
		   appaserver_library_get_server_address(),
		   ( application_prepend_http_protocol_yn(
			application_name ) == 'y' ),
		   document_root_directory,
		   (char *)0 /* filename_stem */,
		   application_name,
		   process_id,
		   (char *)0 /* session */,
		   "csv" );

	if ( with_between )
	{
		appaserver_link_file->begin_date_string = begin_date_string;
		appaserver_link_file->end_date_string = end_date_string;
	}
	else
	{
		appaserver_link_file->begin_date_string = begin_date_string;
	}

	/* With codes */
	/* ========== */
	appaserver_link_file->filename_stem = FILENAME_STEM_WITH_CODES;

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

	/* Without codes */
	/* ============= */
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

	if ( !*display_data
	||   strcmp( display_data, DISPLAY_CATCHES_HORIZONTALLY ) == 0 )
	{
		fprintf( output_codes_file,
"Interview,Date MDY,Day/Week,Trip Hours,No People,Hrs Fishing,Spp Prefer,Area Fish,Spec Location,Int Location,Int Time,Interviewer,Party Comp,Trip Origin,Fish Reside,Catch spp,Catch # Kept,Catch # Rel,Catch spp,Catch # Kept,Catch # Rel,Catch spp,Catch # Kept,Catch # Rel,Catch spp,Catch # Kept,Catch # Rel,Catch spp,Catch # Kept,Catch # Rel,Catch spp,Catch # Kept,Catch # Rel,Catch spp,Catch # Kept,Catch # Rel,Catch spp,Catch # Kept,Catch # Rel,Catch spp,Catch # Kept,Catch # Rel,Catch spp,Catch # Kept,Catch # Rel,Catch spp,Catch # Kept,Catch # Rel,Catch spp,Catch # Kept,Catch # Rel,Catch spp,Catch # Kept,Catch # Rel,Catch spp,Catch # Kept,Catch # Rel\n" );

		fprintf( output_no_codes_file,
"Interview,Date,Day,Trip Hours,No. People,Fishing Hours,Preferred,Area,Specific Location,Interview Location,Time,Interviewer,Party Comp.,Trip Origin,Fish Reside,Family/Species,Kept,Released,Family/Species,Kept,Released,Family/Species,Kept,Released,Family/Species,Kept,Released,Family/Species,Kept,Released,Family/Species,Kept,Released,Family/Species,Kept,Released,Family/Species,Kept,Released,Family/Species,Kept,Released,Family/Species,Kept,Released,Family/Species,Kept,Released,Family/Species,Kept,Released,Family/Species,Kept,Released,Family/Species,Kept,Released\n" );
	}
	else
	if ( strcmp( display_data, DISPLAY_CATCHES_VERTICALLY ) == 0 )
	{
		fprintf( output_codes_file,
"Interview,Date MDY,Day/Week,Trip Hours,No People,Hrs Fishing,Spp Prefer,Area Fish,Spec Location,Int Location,Int Time,Interviewer,Party Comp,Trip Origin,Fish Reside,Catch spp,Catch # Kept,Catch # Rel\n" );

		fprintf( output_no_codes_file,
"Interview,Date,Day,Trip Hours,No. People,Fishing Hours,Preferred,Area,Specific Location,Interview Location,Time,Interviewer,Party Comp.,Trip Origin,Fish Reside,Family/Species,Kept,Released\n" );
	}
	else
	{
		fprintf( stderr,
			"ERROR in %s/%s()/%d: invalid display_data = (%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			display_data );
		exit( 1 );
	}

	while( creel_library_get_fishing_trips_between_dates(
					&fishing_trips,
					application_name,
					fishing_purpose,
					begin_date_string,
					end_date_string ) )
	{
		*(fishing_trips.interview_time + 2) = '\0';

		date =
			date_yyyy_mm_dd_new(
				fishing_trips.census_date );

		preferred_species_code = "";

		if ( *fishing_trips.species_preferred )
		{
			if ( strcmp( fishing_trips.genus, "unknown" ) == 0 )
			{
				strcpy(	species_display,
					fishing_trips.family );
			}
			else
			{
				sprintf(species_display,
					"%s %s %s",
					fishing_trips.family,
					fishing_trips.genus,
					fishing_trips.species_preferred );
			}

			if ( ! ( preferred_species_code =
					creel_library_get_species(
					application_name,
					fishing_trips.family,
					fishing_trips.genus,
					fishing_trips.species_preferred,
					(char *)0 /* species_code */ ) ) )
			{
				preferred_species_code = species_display;
			}
		}
		else
		{
			*species_display = '\0';
		}

		if ( !*display_data
		||   strcmp(	display_data,
				DISPLAY_CATCHES_HORIZONTALLY ) == 0 )
		{
			output_spreadsheet_fishing_trip_horizontally(
				output_codes_file,
				output_no_codes_file,
				fishing_trips,
				date,
				application_name,
				begin_date_string,
				end_date_string,
				preferred_species_code,
				species_display );
		}
		else
		if ( strcmp( display_data,
				DISPLAY_CATCHES_VERTICALLY ) == 0 )
		{
			output_spreadsheet_fishing_trip_vertically(
				output_codes_file,
				output_no_codes_file,
				fishing_trips,
				date,
				application_name,
				begin_date_string,
				end_date_string,
				preferred_species_code,
				species_display );
		}
		else
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: invalid display_data = (%s)\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				display_data );
			exit( 1 );
		}

		date_free( date );

	} /* while( creel_library_get_fishing_trips_between_dates() ) */

	fclose( output_codes_file );
	fclose( output_no_codes_file );

	get_title_and_sub_title(
			title,
			sub_title,
			fishing_purpose,
			begin_date_string,
			end_date_string );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ){};
	fflush( stdout );
	printf( "</h2>\n" );
		
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

	printf( "<br>\n" );

}

void get_title_and_sub_title(
			char *title,
			char *sub_title,
			char *fishing_purpose,
			char *begin_date_string,
			char *end_date_string )
{
	char buffer[ 128 ];

	sprintf(title,
		"Output %s Spreadsheet File",
		format_initial_capital( buffer, fishing_purpose ) );

	sprintf(	sub_title,
			"From %s To %s",
			begin_date_string,
			end_date_string );
}

void output_spreadsheet_fishing_trip_horizontally(
			FILE *output_codes_file,
			FILE *output_no_codes_file,
			FISHING_TRIPS fishing_trips,
			DATE *date,
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *preferred_species_code,
			char *species_display )
{
	CATCHES catches;
	char *catch_species_code;

	fprintf(output_codes_file,
		"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
		fishing_trips.interview_number,
		date_display_mdy( date ),
		creel_library_day_of_week(
			application_name,
			fishing_trips.fishing_purpose,
			fishing_trips.census_date,
			fishing_trips.interview_location ),
		fishing_trips.fishing_trip_duration_hours,
		fishing_trips.number_of_people_fishing,
		fishing_trips.fishing_duration_hours,
		preferred_species_code,
		fishing_trips.fishing_area,
		"",
		creel_library_get_interview_location(
			application_name,
			fishing_trips.interview_location,
			(char *)0 /* interview_location_code */ ),
		fishing_trips.interview_time,
		creel_library_get_census_researcher_code(
			application_name,
			fishing_trips.fishing_purpose,
			fishing_trips.census_date,
			fishing_trips.interview_location ),
		creel_library_get_fishing_composition(
			application_name,
			fishing_trips.fishing_party_composition,
			(char *)0 /* fishing_party_composition_code */),
		creel_library_get_trip_origin(
			application_name,
			fishing_trips.trip_origin_location,
			(char *)0 /* trip_origin_location_code */ ),
		creel_library_get_recreational_angler_reside_code(
			application_name,
			fishing_trips.recreational_angler_reside ) );

	fprintf(output_no_codes_file,
	"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
		fishing_trips.interview_number,
		fishing_trips.census_date,
		creel_library_day_of_week(
			application_name,
			fishing_trips.fishing_purpose,
			fishing_trips.census_date,
			fishing_trips.interview_location ),
		fishing_trips.fishing_trip_duration_hours,
		fishing_trips.number_of_people_fishing,
		fishing_trips.fishing_duration_hours,
		species_display,
		fishing_trips.fishing_area,
		"",
		fishing_trips.interview_location,
		fishing_trips.interview_time,
		creel_library_get_census_researcher(
			application_name,
			fishing_trips.fishing_purpose,
			fishing_trips.census_date,
			fishing_trips.interview_location ),
		fishing_trips.fishing_party_composition,
		fishing_trips.trip_origin_location,
		fishing_trips.recreational_angler_reside );

	while( creel_library_census_date_get_catches(
			&catches,
			application_name,
			fishing_trips.fishing_purpose,
			fishing_trips.census_date,
			fishing_trips.interview_location,
			fishing_trips.interview_number,
			begin_date_string,
			end_date_string ) )
	{

		if ( ! ( catch_species_code =
				creel_library_get_species(
				application_name,
			 	catches.family,
			 	catches.genus,
			 	catches.species,
				(char *)0 /* species_code */ ) ) )
		{
			catch_species_code = catches.species;
		}

		fprintf( output_codes_file,
			 ",%s,%d,%d",
			 catch_species_code,
			 catches.kept_integer,
			 catches.released_integer );

		if ( strcmp( catches.genus, "unknown" ) == 0 )
		{
			fprintf( output_no_codes_file,
			 	",%s,%d,%d",
			 	catches.family,
			 	catches.kept_integer,
			 	catches.released_integer );
		}
		else
		{
			fprintf( output_no_codes_file,
			 	",%s %s %s,%d,%d",
			 	catches.family,
			 	catches.genus,
			 	catches.species,
			 	catches.kept_integer,
			 	catches.released_integer );
		}
	}

	fprintf(output_codes_file, "\n" );
	fprintf(output_no_codes_file, "\n" );
}

void output_spreadsheet_fishing_trip_vertically(
			FILE *output_codes_file,
			FILE *output_no_codes_file,
			FISHING_TRIPS fishing_trips,
			DATE *date,
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			char *preferred_species_code,
			char *species_display )
{
	CATCHES catches;
	int got_one = 0;
	char *catch_species_code;

	while( creel_library_census_date_get_catches(
			&catches,
			application_name,
			fishing_trips.fishing_purpose,
			fishing_trips.census_date,
			fishing_trips.interview_location,
			fishing_trips.interview_number,
			begin_date_string,
			end_date_string ) )
	{
		got_one = 1;

		fprintf(output_codes_file,
			"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
			fishing_trips.interview_number,
			date_display_mdy( date ),
			creel_library_day_of_week(
				application_name,
				fishing_trips.fishing_purpose,
				fishing_trips.census_date,
				fishing_trips.interview_location ),
			fishing_trips.fishing_trip_duration_hours,
			fishing_trips.number_of_people_fishing,
			fishing_trips.fishing_duration_hours,
			preferred_species_code,
			fishing_trips.fishing_area,
			"",
			creel_library_get_interview_location(
				application_name,
				fishing_trips.interview_location,
				(char *)0 /* interview_location_code */ ),
			fishing_trips.interview_time,
			creel_library_get_census_researcher_code(
				application_name,
				fishing_trips.fishing_purpose,
				fishing_trips.census_date,
				fishing_trips.interview_location ),
			creel_library_get_fishing_composition(
				application_name,
				fishing_trips.fishing_party_composition,
				(char *)0 /* fishing_party_composition_code */),
			creel_library_get_trip_origin(
				application_name,
				fishing_trips.trip_origin_location,
				(char *)0 /* trip_origin_location_code */ ),
			creel_library_get_recreational_angler_reside_code(
				application_name,
				fishing_trips.recreational_angler_reside ) );
	
		fprintf(output_no_codes_file,
		"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s",
			fishing_trips.interview_number,
			fishing_trips.census_date,
			creel_library_day_of_week(
				application_name,
				fishing_trips.fishing_purpose,
				fishing_trips.census_date,
				fishing_trips.interview_location ),
			fishing_trips.fishing_trip_duration_hours,
			fishing_trips.number_of_people_fishing,
			fishing_trips.fishing_duration_hours,
			species_display,
			fishing_trips.fishing_area,
			"",
			fishing_trips.interview_location,
			fishing_trips.interview_time,
			creel_library_get_census_researcher(
				application_name,
				fishing_trips.fishing_purpose,
				fishing_trips.census_date,
				fishing_trips.interview_location ),
			fishing_trips.fishing_party_composition,
			fishing_trips.trip_origin_location,
			fishing_trips.recreational_angler_reside );

		if ( ! ( catch_species_code =
				creel_library_get_species(
				application_name,
			 	catches.family,
			 	catches.genus,
			 	catches.species,
				(char *)0 /* species_code */ ) ) )
		{
			catch_species_code = catches.species;
		}

		fprintf( output_codes_file,
			 ",%s,%d,%d\n",
			 catch_species_code,
			 catches.kept_integer,
			 catches.released_integer );

		if ( strcmp( catches.genus, "unknown" ) == 0 )
		{
			fprintf( output_no_codes_file,
			 	",%s,%d,%d\n",
			 	catches.family,
			 	catches.kept_integer,
			 	catches.released_integer );
		}
		else
		{
			fprintf( output_no_codes_file,
			 	",%s %s %s,%d,%d\n",
			 	catches.family,
			 	catches.genus,
			 	catches.species,
			 	catches.kept_integer,
			 	catches.released_integer );
		}
	}

	if ( !got_one )
	{
		fprintf(output_codes_file,
			"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
			fishing_trips.interview_number,
			date_display_mdy( date ),
			creel_library_day_of_week(
				application_name,
				fishing_trips.fishing_purpose,
				fishing_trips.census_date,
				fishing_trips.interview_location ),
			fishing_trips.fishing_trip_duration_hours,
			fishing_trips.number_of_people_fishing,
			fishing_trips.fishing_duration_hours,
			preferred_species_code,
			fishing_trips.fishing_area,
			"",
			creel_library_get_interview_location(
				application_name,
				fishing_trips.interview_location,
				(char *)0 /* interview_location_code */ ),
			fishing_trips.interview_time,
			creel_library_get_census_researcher_code(
				application_name,
				fishing_trips.fishing_purpose,
				fishing_trips.census_date,
				fishing_trips.interview_location ),
			creel_library_get_fishing_composition(
				application_name,
				fishing_trips.fishing_party_composition,
				(char *)0 /* fishing_party_composition_code */),
			creel_library_get_trip_origin(
				application_name,
				fishing_trips.trip_origin_location,
				(char *)0 /* trip_origin_location_code */ ),
			creel_library_get_recreational_angler_reside_code(
				application_name,
				fishing_trips.recreational_angler_reside ) );
	
		fprintf(output_no_codes_file,
		"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
			fishing_trips.interview_number,
			fishing_trips.census_date,
			creel_library_day_of_week(
				application_name,
				fishing_trips.fishing_purpose,
				fishing_trips.census_date,
				fishing_trips.interview_location ),
			fishing_trips.fishing_trip_duration_hours,
			fishing_trips.number_of_people_fishing,
			fishing_trips.fishing_duration_hours,
			species_display,
			fishing_trips.fishing_area,
			"",
			fishing_trips.interview_location,
			fishing_trips.interview_time,
			creel_library_get_census_researcher(
				application_name,
				fishing_trips.fishing_purpose,
				fishing_trips.census_date,
				fishing_trips.interview_location ),
			fishing_trips.fishing_party_composition,
			fishing_trips.trip_origin_location,
			fishing_trips.recreational_angler_reside );
	}

}

