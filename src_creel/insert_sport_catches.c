/* src_creel/insert_sport_catches.c */
/* -------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "dictionary.h"
#include "date_convert.h"
#include "date.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "creel_library.h"
#include "creel_load_library.h"

#define CATCHES_FIELD_LIST	"fishing_purpose,census_date,interview_location,interview_number,family,genus,species,kept_count,released_count"

int main( int argc, char **argv )
{
	char *application_name;
	char *input_filename;
	char really_yn;
	FILE *input_file;
	FILE *catches_output_pipe;
	char input_string[ 1024 ];
	char census_date_mdyy[ 16 ];
	char interview_location_code_string[ 16 ];
	char *interview_location_string;
	char census_date_international[ 16 ];
	char *now_date_international;
	char interview_time_string[ 16 ];
	char sys_string[ 1024 ];
	char interview_number_string[ 16 ];
	char florida_code_string[ 16 ];
	char family[ 128 ];
	char genus[ 128 ];
	char species[ 128 ];
	char kept_count_string[ 16 ];
	char released_count_string[ 16 ];
	int species_offset;
	char error_filename[ 128 ];
	FILE *error_file;
	int line_number = 0;

	if ( argc != 4 )
	{
		fprintf( stderr, 
"Usage: %s application filename really_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	input_filename = argv[ 2 ];
	really_yn = *argv[ 3 ];

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	sprintf( error_filename, "/tmp/catches_error_%d.txt", getpid() );
	if ( ! ( error_file = fopen( error_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	if ( really_yn == 'y' )
	{
		char *table_name =
			get_table_name( application_name, "catches" );

		sprintf( sys_string,
			 "sort -u					|"
			 "insert_statement t=%s f=%s d='|' replace=y	|"
			 "sql 2>&1					|"
			 "cat						 ",
			 table_name,
			 CATCHES_FIELD_LIST );

		catches_output_pipe = popen( sys_string, "w" );
	}
	else
	{
		sprintf( sys_string,
"sort -u | queue_top_bottom_lines.e 50 | html_table.e 'Insert into Catches' %s '|'",
			 CATCHES_FIELD_LIST );
		catches_output_pipe = popen( sys_string, "w" );
	}

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

		if ( !isdigit( *census_date_mdyy ) )
		{
			fprintf(error_file,
			"Warning: invalid date format = (%s)\n",
				census_date_mdyy );
			continue;
		}

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
/*
			fprintf(error_file,
			"Warning: date is in the future = (%s)\n",
				census_date_mdyy );
*/
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

		species_offset = 0;
		while( 1 )
		{
			if ( !piece_double_quoted(
					florida_code_string,
					',',
					input_string,
					SPORT_CATCH_FLORIDA_CODE_PIECE +
					species_offset ) )
			{
				break;
			}

			if ( !atoi( florida_code_string ) )
			{
				species_offset += 3;
				continue;
			}

			*family = '\0';
			*species = '\0';
			*family = '\0';

			if ( !creel_load_library_get_family_genus_species(
						family,
						genus,
						species,
						florida_code_string,
						application_name ) )
			{
				fprintf(error_file,
		"Warning in line %d: cannot find florida code = %s in (%s)\n",
						line_number,
						florida_code_string,
						input_string );
				break;
			}

			if ( !piece_double_quoted(
					kept_count_string,
					',',
					input_string,
					SPORT_CATCH_FLORIDA_CODE_PIECE +
					species_offset + 1 ) )
			{
				break;
			}

			if ( !piece_double_quoted(
					released_count_string,
					',',
					input_string,
					SPORT_CATCH_FLORIDA_CODE_PIECE +
					species_offset + 2 ) )
			{
				break;
			}

			if ( ! ( atoi( kept_count_string ) +
				 atoi( released_count_string ) ) )
			{
				fprintf(error_file,
		"Warning in line %d: catches is zero for %s in (%s)\n",
						line_number,
						florida_code_string,
						input_string );
			}
			else
			{
				fprintf(catches_output_pipe,
					"%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
					CREEL_CENSUS_SPORT,
					census_date_international,
					interview_location_string,
					interview_number_string,
					family,
					genus,
					species,
					kept_count_string,
					released_count_string );
			}

			species_offset += 3;
		}
	}

	fclose( input_file );
	pclose( catches_output_pipe );

	fclose( error_file );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Catches Errors' '' '|'",
			 error_filename );
		if ( system( sys_string ) );
	}

	return 0;
} /* main() */

