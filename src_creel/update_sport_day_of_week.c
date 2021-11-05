/* ----------------------------------------------------- */
/* $APPASERVER_HOME/src_creel/update_sport_day_of_week.c */
/* ----------------------------------------------------- */
/* Freely available software: see Appaserver.org	 */
/* ----------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "list.h"
#include "environ.h"
#include "process.h"
#include "dictionary.h"
#include "boolean.h"
#include "application.h"
#include "date_convert.h"
#include "piece.h"
#include "boolean.h"
#include "creel_load_library.h"
#include "creel_library.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void update_sport_day_of_week(
			char *application_name,
			char *input_filename,
			boolean execute );

int main( int argc, char **argv )
{
	char *application_name;
	char *input_filename;
	boolean execute;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 3 )
	{
		fprintf(stderr, 
			"Usage: %s filename execute_yn\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	input_filename = argv[ 1 ];
	execute = ( *argv[ 2 ] == 'y' );

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	printf( "<h2>Update Sport Day of Week\n" );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ){};
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !*input_filename
	||   strcmp( input_filename, "filename" ) == 0 )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	update_sport_day_of_week(
		application_name,
		input_filename,
		execute );

	if ( execute )
		printf( "<p>Process complete\n" );
	else
		printf( "<p>Process not executed\n" );

	document_close();

	return 0;
}

#define KEY_LIST	"fishing_purpose,"	\
			"census_date,"		\
			"interview_location"

void update_sport_day_of_week(
			char *application_name,
			char *input_filename,
			boolean execute )
{
	FILE *input_file;
	FILE *output_pipe;
	char input_string[ 1024 ];
	char census_date_mdyy[ 128 ];
	char interview_location_code_string[ 128 ];
	char *interview_location_string;
	char census_date_international[ 16 ];
	char sys_string[ 1024 ];
	char error_filename[ 128 ];
	char day_of_week[ 128 ];
	FILE *error_file;

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
			 "update_statement t=%s k=%s carrot=y		|"
			 "tee_appaserver_error.sh			|"
			 "sql 2>&1					|"
			 "html_paragraph_wrapper.e			 ",
			 "creel_census",
			 KEY_LIST );
	}
	else
	{
		sprintf(
		   sys_string,
		   "sort -u						|"
		   "html_table.e 'Update Creel Census' %s,%s '^' %s 	",
		   KEY_LIST,
		   "day_of_week",
		   "left,left,left,right" );
	}

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
				day_of_week,
				',',
				input_string,
				SPORT_DAY_OF_WEEK_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece day of week in (%s)\n",
				input_string );
			continue;
		}

		if ( !isdigit( *day_of_week ) )
		{
			fprintf(error_file,
			"Warning: Invalid day of week in (%s)\n",
				input_string );
			continue;
		}

		if ( execute )
		{
			fprintf(output_pipe,
				"%s^%s^%s^day_of_week^%s\n",
				CREEL_CENSUS_SPORT,
				census_date_international,
				interview_location_string,
				day_of_week );
		}
		else
		{
			fprintf(output_pipe,
				"%s^%s^%s^%s\n",
				CREEL_CENSUS_SPORT,
				census_date_international,
				interview_location_string,
				day_of_week );
		}
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
}

