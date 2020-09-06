/* src_creel/insert_sport_fishing_trips.c */
/* -------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "dictionary.h"
#include "date_convert.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "date.h"
#include "creel_library.h"
#include "creel_load_library.h"

#define FISHING_TRIPS_FIELD_LIST "fishing_purpose,census_date,interview_location,interview_number,interview_time,trip_origin_location,family,genus,species_preferred,fishing_area,fishing_party_composition,trip_hours,hours_fishing,number_of_people_fishing,recreational_angler_reside,last_changed_by,validation_date"

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *input_filename;
	char really_yn;
	FILE *input_file;
	FILE *output_pipe;
	char input_string[ 1024 ];
	char census_date_mdyy[ 16 ];
	char interview_location_code_string[ 16 ];
	char *interview_location_string;
	char trip_origin_location_code_string[ 16 ];
	char *trip_origin_location_string;
	char census_date_international[ 16 ];
	char *now_date_international;
	char sys_string[ 1024 ];
	char interview_number_string[ 16 ];
	char interview_time_string[ 16 ];
	char targeted_florida_code_string[ 16 ];
	char targeted_family[ 128 ];
	char targeted_genus[ 128 ];
	char targeted_species[ 128 ];
	char fishing_area_string[ 16 ];
	char fishing_party_composition_code_string[ 16 ];
	char *fishing_party_composition_string;
	char recreational_angler_reside_code_string[ 16 ];
	char *recreational_angler_reside_string;
	char fishing_trip_duration_string[ 16 ];
	char fishing_duration_string[ 16 ];
	char onboard_fishing_count_string[ 16 ];
	char error_filename[ 128 ];
	FILE *error_file;
	int line_number = 0;

	if ( argc != 5 )
	{
		fprintf( stderr, 
"Usage: %s application login_name filename really_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	login_name = argv[ 2 ];
	input_filename = argv[ 3 ];
	really_yn = *argv[ 4 ];

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	sprintf( error_filename, "/tmp/fishing_trips_error_%d.txt", getpid() );
	if ( ! ( error_file = fopen( error_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	if ( really_yn == 'y' )
	{
		char *table_name =
			get_table_name( application_name, "fishing_trips" );

		sprintf( sys_string,
			 "sort -u					|"
			 "insert_statement t=%s f=%s d='|' replace=y	|"
			 "sql 2>&1					|"
			 "cat						 ",
			 table_name,
			 FISHING_TRIPS_FIELD_LIST );
	}
	else
	{
		sprintf( sys_string,
"sort -u | queue_top_bottom_lines.e 50 | html_table.e 'Insert into Fishing Trips' %s '|'",
			 FISHING_TRIPS_FIELD_LIST );
	}

	output_pipe = popen( sys_string, "w" );

	now_date_international =
		date_get_now_date_yyyy_mm_dd(
			date_get_utc_offset() );

	/* Skip the first line */
	/* ------------------- */
	get_line( input_string, input_file );
	line_number++;

	while( get_line( input_string, input_file ) )
	{
		line_number++;
		if ( !piece_double_quoted(
				census_date_mdyy,
				',',
				input_string,
				SPORT_CENSUS_DATE_PIECE ) )
		{
			fprintf(error_file,
				"Warning: Cannot piece census date in (%s)\n",
				input_string );
			continue;
		}

		if ( !isdigit( *census_date_mdyy ) ) continue;

		*census_date_international = '\0';

		date_convert_source_american(
				census_date_international,
				international,
				census_date_mdyy );

		if ( !*census_date_international )
		{
			fprintf(error_file,
			"Warning: invalid date format = (%s)\n",
				census_date_mdyy );
			continue;
		}

		if ( strcmp(	census_date_international,
				now_date_international ) > 0 )
		{
			fprintf(error_file,
			"Warning: date is in the future = (%s)\n",
				census_date_mdyy );
			continue;
		}

		if ( !piece_double_quoted(
				interview_time_string,
				',',
				input_string,
				SPORT_INTERVIEW_TIME_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece interview time in (%s)\n",
				input_string );
			continue;
		}

		creel_load_library_pad_time_four( interview_time_string );

		if ( !piece_double_quoted(
				interview_location_code_string,
				',',
				input_string,
				SPORT_INTERVIEW_LOCATION_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece interview location in (%s)\n",
				input_string );
			continue;
		}

		interview_location_string =
			creel_library_get_interview_location(
				application_name,
				(char *)0 /* interview_location */,
				interview_location_code_string );

		if ( strcmp(	interview_location_string,
				interview_location_code_string ) == 0 )
		{
			fprintf(error_file,
			"Warning: invalid interview location code = (%s)\n",
				interview_location_code_string );
			continue;
		}

		if ( !piece_double_quoted(
				interview_number_string,
				',',
				input_string,
				SPORT_INTERVIEW_NUMBER_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece interview number in (%s)\n",
				input_string );
			continue;
		}

		if ( !piece_double_quoted(
				trip_origin_location_code_string,
				',',
				input_string,
				SPORT_TRIP_ORIGIN_LOCATION_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece trip origin location in (%s)\n",
				input_string );
			continue;
		}

		trip_origin_location_string =
			creel_library_get_trip_origin(
				application_name,
				(char *)0 /* trip_origin_location */,
				trip_origin_location_code_string );

		if ( strcmp(	trip_origin_location_string,
				trip_origin_location_code_string ) == 0 )
		{
			fprintf(error_file,
			"Warning: invalid trip origin location code = (%s)\n",
				trip_origin_location_code_string );
			continue;
		}

		if ( !piece_double_quoted(
				targeted_florida_code_string,
				',',
				input_string,
				SPORT_TARGETED_FLORIDA_CODE_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece targeted florida code in (%s)\n",
				input_string );
			continue;
		}

		if ( !creel_load_library_get_family_genus_species(
				targeted_family,
				targeted_genus,
				targeted_species,
				targeted_florida_code_string,
				application_name ) )
		{
			*targeted_family = '\0';
			*targeted_genus = '\0';
			*targeted_species = '\0';
		}

		if ( !piece_double_quoted(
				fishing_area_string,
				',',
				input_string,
				SPORT_LOCATION_AREA_ZONE_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece location area/zone in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( *fishing_area_string )
		{
			remove_quotes(	fishing_area_string,
					fishing_area_string );
		}

		if ( *fishing_area_string
		&&   !creel_library_exists_fishing_area(
				application_name,
				fishing_area_string ) )
		{
			fprintf(error_file,
		"Warning in line %d: invalid fishing area of (%s) in (%s)\n",
				line_number,
				fishing_area_string,
				input_string );
			continue;
		}

		if ( !piece_double_quoted(
				fishing_party_composition_code_string,
				',',
				input_string,
				SPORT_FISHING_PARTY_COMPOSITION_PIECE ) )
		{
			fprintf(error_file,
		"Warning: Cannot piece fishing party composition in (%s)\n",
				input_string );
			continue;
		}

		fishing_party_composition_string =
			creel_library_get_fishing_composition(
				application_name,
				(char *)0 /* fishing_party_composition */,
				fishing_party_composition_code_string );

		if ( strcmp(	fishing_party_composition_string,
				fishing_party_composition_code_string ) == 0 )
		{
			fishing_party_composition_string = "";
		}

		if ( !piece_double_quoted(
				recreational_angler_reside_code_string,
				',',
				input_string,
				SPORT_RECREATIONAL_ANGLER_RESIDE_PIECE ) )
		{
			fprintf(error_file,
		"Warning: Cannot piece recreational angler reside in (%s)\n",
				input_string );
			continue;
		}

		recreational_angler_reside_string =
			creel_library_get_recreational_angler_reside(
				application_name,
				(char *)0 /* recreational_angler_reside */,
				recreational_angler_reside_code_string );

		if ( strcmp(	recreational_angler_reside_string,
				recreational_angler_reside_code_string ) == 0 )
		{
			recreational_angler_reside_string = "";
		}

		if ( !piece_double_quoted(
				fishing_trip_duration_string,
				',',
				input_string,
				SPORT_FISHING_TRIP_DURATION_PIECE ) )
		{
			fprintf(error_file,
		"Warning: Cannot piece fishing trip duration in (%s)\n",
				input_string );
			continue;
		}

		if ( !piece_double_quoted(
				fishing_duration_string,
				',',
				input_string,
				SPORT_FISHING_DURATION_PIECE ) )
		{
			fprintf(error_file,
		"Warning: Cannot piece fishing duration in (%s)\n",
				input_string );
			continue;
		}

		if ( !piece_double_quoted(
				onboard_fishing_count_string,
				',',
				input_string,
				SPORT_ONBOARD_FISHING_COUNT_PIECE ) )
		{
			fprintf(error_file,
		"Warning: Cannot piece onboard fishing count in (%s)\n",
				input_string );
			continue;
		}

		fprintf(output_pipe,
			"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
			CREEL_CENSUS_SPORT,
			census_date_international,
			interview_location_string,
			interview_number_string,
			interview_time_string,
			trip_origin_location_string,
			targeted_family,
			targeted_genus,
			targeted_species,
			fishing_area_string,
			fishing_party_composition_string,
			fishing_trip_duration_string,
			fishing_duration_string,
			onboard_fishing_count_string,
			recreational_angler_reside_string,
			login_name,
			now_date_international );
	}

	fclose( input_file );
	pclose( output_pipe );
	fclose( error_file );

	if ( timlib_file_populated( error_filename ) )
	{
		int results;

		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Fishing Trips Errors' '' '|'",
			 error_filename );
		results = system( sys_string );
	}


	return 0;

} /* main() */

