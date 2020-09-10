/* src_creel/insert_sport_creel_census.c */
/* ------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "date_convert.h"
#include "date.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "creel_library.h"
#include "creel_load_library.h"

#define FIELD_LIST	"fishing_purpose,"	\
			"census_date,"		\
			"interview_location,"	\
			"researcher"

int main( int argc, char **argv )
{
	char *application_name;
	char *input_filename;
	boolean replace_existing_data;
	boolean execute;
	FILE *input_file;
	FILE *output_pipe;
	char input_string[ 1024 ];
	char census_date_mdyy[ 16 ];
	char interview_location_code_string[ 16 ];
	char *interview_location_string;
	char census_date_international[ 16 ];
	char *now_date_international;
	char sys_string[ 1024 ];
	char error_filename[ 128 ];
	char *researcher;
	char researcher_code[ 16 ];
	FILE *error_file;

	if ( argc != 5 )
	{
		fprintf( stderr, 
	"Usage: %s application filename replace_existing_data_yn execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	input_filename = argv[ 2 ];
	replace_existing_data = ( *argv[ 3 ] == 'y' );
	execute = ( *argv[ 4 ] == 'y' );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	sprintf( error_filename, "/tmp/creel_census_error_%d.txt", getpid() );
	if ( ! ( error_file = fopen( error_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	if ( execute )
	{
		sprintf( sys_string,
			 "sort -u					|"
			 "insert_statement t=%s f=%s d='|' replace=%c	|"
			 "sql 2>&1					|"
			 "cat						 ",
			 "creel_census",
			 FIELD_LIST,
			 (replace_existing_data) ? 'y' : 'n' );
	}
	else
	{
		sprintf( sys_string,
		"sort -u | html_table.e 'Insert into Creel Census' %s '|'",
			 FIELD_LIST );
	}

	now_date_international =
		date_get_now_date_yyyy_mm_dd(
			date_get_utc_offset() );

	output_pipe = popen( sys_string, "w" );

	/* Skip the first line */
	/* ------------------- */
	get_line( input_string, input_file );

	while( get_line( input_string, input_file ) )
	{
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
			"Warning: Invalid census date in (%s)\n",
				input_string );
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
			"Warning: invalid date format = (%s) for (%s)\n",
				census_date_mdyy,
				input_string );
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
				researcher_code,
				',',
				input_string,
				SPORT_RESEARCHER_CODE_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece researcher code in (%s)\n",
				input_string );
			continue;
		}

		researcher = creel_library_get_researcher(
				application_name,
				(char *)0 /* researcher */,
				researcher_code );

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

		fprintf(output_pipe,
			"%s|%s|%s|%s\n",
			CREEL_CENSUS_SPORT,
			census_date_international,
			interview_location_string,
			researcher );
	}

	fclose( input_file );
	fclose( error_file );
	pclose( output_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Creel Census Errors' '' '|'",
			 error_filename );
		if ( system( sys_string ) ){};
	}

	sprintf( sys_string, "rm %s", error_filename );
	if ( system( sys_string ) ){};

	return 0;
}

