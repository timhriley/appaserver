/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/load_ysi_data.c	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "date_convert.h"
#include "query.h"
#include "basename.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "column.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "date.h"
#include "boolean.h"
#include "process.h"
#include "dictionary.h"
#include "session.h"
#include "application.h"
#include "hydrology_library.h"
#include "station.h"
#include "datatype.h"
#include "units.h"
#include "hydrology.h"

/* Structures */
/* ---------- */
typedef struct
{
	int special_code;
	char *replacement_string;
} SPECIAL_CODE_STRUCTURE;

/* Constants */
/* --------- */
#define STATION_LABEL			"Site"

/* Prototypes */
/* ---------- */
void set_default_dates(		char **begin_date_string,
				char **begin_time_string,
				char **end_date_string,
				char **end_time_string );

void output_bad_records(
		 		char *bad_parse_file,
		 		char *bad_range_file,
		 		char *bad_frequency_file,
		 		char *bad_insert_file );

void load_ysi_filespecification(
					char *filename,
					char *station,
					char change_existing_data_yn,
					char execute_yn,
					char is_exo_yn,
					char *begin_date_string,
					char *begin_time_string,
					char *end_date_string,
					char *end_time_string,
					char *appaserver_data_directory );

char *station_fetch(			char *application_name,
					char *filename );

char *station_label_fetch(		char *application_name,
					char *filename );

/* Check column piece=3 */
/* -------------------- */
char *station_column_fetch(		char *application_name,
					char *filename );

int main( int argc, char **argv )
{
	char *application_name;
	char is_exo_yn;
	char change_existing_data_yn;
	char execute_yn;
	char *filename;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *input_directory;
	char *station;
	char *process_name;
	char format_buffer[ 256 ];
	char *begin_date_string;
	char *begin_time_string;
	char *end_date_string;
	char *end_time_string;

	/* Exits if fails */
	/* -------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 11 )
	{
		fprintf( stderr, 
"Usage: %s process filename station begin_date begin_time end_date end_time is_exo_yn change_existing_data_yn execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	filename = argv[ 2 ];
	station = argv[ 3 ];
	begin_date_string = argv[ 4 ];
	begin_time_string = argv[ 5 ];
	end_date_string = argv[ 6 ];
	end_time_string = argv[ 7 ];
	is_exo_yn = *argv[ 8 ];
	change_existing_data_yn = *argv[ 9 ];
	execute_yn = *argv[ 10 ];

	set_default_dates(	&begin_date_string,
				&begin_time_string,
				&end_date_string,
				&end_time_string );

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	printf( "<h1>%s</h1>\n",
	 	format_initial_capital( format_buffer, process_name ) );
	printf( "<h2>" );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
	printf( "</h2>\n" );
	fflush( stdout );

	input_directory = basename_get_directory( filename );

	if ( !input_directory || !*input_directory )
	{
		printf( "<h3>ERROR: Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !*station
	||   strcmp( station, "station" ) == 0 )
	{
		if ( ! ( station =
				station_fetch(
					application_name,
					filename ) ) )
		{
			printf(
			"<h3>ERROR: Could not identify the station.</h3>\n" );
			document_close();
			exit( 1 );
		}
	}

	if (	!*begin_date_string
	||	strcmp( begin_date_string, "begin_date" ) == 0 )
	{
		printf(
		"<h3>Please enter in a begin date.</h3>\n" );
		document_close();
		exit( 1 );
	}

	if ( !isdigit( *begin_time_string ) ) begin_time_string = "0000";

	if (	!*end_date_string
	||	strcmp( end_date_string, "end_date" ) == 0 )
	{
		printf(
		"<h3>Please enter in an end date.</h3>\n" );
		document_close();
		exit( 1 );
	}

	if ( !isdigit( *end_time_string ) ) end_time_string = "2359";

	load_ysi_filespecification(
			filename,
			station,
			change_existing_data_yn,
			execute_yn,
			is_exo_yn,
			begin_date_string,
			begin_time_string,
			end_date_string,
			end_time_string,
			appaserver_parameter_file->
				appaserver_data_directory );

	if ( execute_yn == 'y' )
		printf( "<h3>Insert complete.</h3>\n" );
	else
		printf( "<h3>Insert not executed.</h3>\n" );

	if ( execute_yn == 'y' )
	{
		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}

	document_close();

	return 0;

} /* main() */

void load_ysi_filespecification(
			char *filename,
			char *station,
			char change_existing_data_yn,
			char execute_yn,
			char is_exo_yn,
			char *begin_date_string,
			char *begin_time_string,
			char *end_date_string,
			char *end_time_string,
			char *appaserver_data_directory )
{
	char sys_string[ 2048 ];
	char bad_parse[ 128 ];
	char bad_range[ 128 ];
	char bad_frequency[ 128 ];
	char bad_insert[ 128 ];
	char *two_lines;
	char *date_heading_label;
	pid_t pid;
	char *dir;

	pid = getpid();
	dir = appaserver_data_directory;

	sprintf( bad_parse, "%s/parse_%d.dat", dir, pid );
	sprintf( bad_range, "%s/range_%d.dat", dir, pid );
	sprintf( bad_frequency, "%s/reject_%d.dat", dir, pid );
	sprintf( bad_insert, "%s/insert_%d.dat", dir, pid );

	if ( is_exo_yn != 'y' )
		two_lines = "yes";
	else
		two_lines = "no";

	date_heading_label = "Date";

	sprintf( sys_string,
"spreadsheet_parse file=\"%s\" station=\"%s\" date=%s two=%s 2>%s	|"
"measurement_date_within_range	begin_date=%s				 "
"				begin_time=%s				 "
"				end_date=%s				 "
"				end_time=%s 2>%s			|"
"measurement_frequency_reject '^' 2>%s					|"
"measurement_insert bypass=y replace=%c execute=%c 2>%s			|"
"cat									 ",
		 filename,
		 station,
		 date_heading_label,
		 two_lines,
		 bad_parse,
		 begin_date_string,
		 begin_time_string,
		 end_date_string,
		 end_time_string,
		 bad_range,
		 bad_frequency,
		 (change_existing_data_yn == 'y') ? 'y' : 'n',
		 (execute_yn == 'y') ? 'y' : 'n',
		 bad_insert );

	if ( system( sys_string ) ) {};

	output_bad_records(
		 bad_parse,
		 bad_range,
		 bad_frequency,
		 bad_insert );

} /* load_ysi_filespecification() */

void output_bad_records(
		 	char *bad_parse_file,
		 	char *bad_range_file,
		 	char *bad_frequency_file,
		 	char *bad_insert_file )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
	"cat %s %s %s %s | html_table.e '^^Bad Records' '' ''",
	 	bad_parse_file,
	 	bad_range_file,
	 	bad_frequency_file,
	 	bad_insert_file );

	if ( system( sys_string ) ){};

} /* output_bad_records() */

char *station_fetch(		char *application_name,
				char *filename )
{
	char *station;

	if ( ( station =
		station_label_fetch(
			application_name,
			filename ) ) )
	{
		return station;
	}
	else
	{
		/* Check column piece=3 */
		/* -------------------- */
		return station_column_fetch(
			application_name,
			filename );
	}

} /* station_fetch() */

/* Check column piece=3 */
/* -------------------- */
char *station_column_fetch(	char *application_name,
				char *filename )
{
	char station[ 128 ];
	FILE *input_file;
	char input_buffer[ 1024 ];
	boolean found_header = 0;

	if ( ! ( input_file = fopen( filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 filename );
		return 0;
	}

	timlib_reset_get_line_check_utf_16();

	while( timlib_get_line( input_buffer, input_file, 1024 ) )
	{
		if ( !piece( station, ',', input_buffer, 3 ) )
		{
			continue;
		}

		if ( found_header )
		{
			fclose( input_file );
			timlib_reset_get_line_check_utf_16();

			if ( !station_exists(
				application_name,
				station ) )
			{
				printf(
		"<h3>Error: station = (%s) doesn't exist in STATION.</h3>\n",
					station );

				return (char *)0;
			}
			else
			{
				return strdup( station );
			}
		}

		if ( timlib_strncmp( station, STATION_LABEL ) == 0 )
		{
			found_header = 1;
			continue;
		}
	}

	fclose( input_file );
	timlib_reset_get_line_check_utf_16();

	return (char *)0;

} /* station_column_fetch() */

char *station_label_fetch(	char *application_name,
				char *filename )
{
	char station[ 128 ];
	FILE *input_file;
	char input_buffer[ 1024 ];

	if ( ! ( input_file = fopen( filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 filename );
		return 0;
	}

	timlib_reset_get_line_check_utf_16();

	while( timlib_get_line( input_buffer, input_file, 1024 ) )
	{
		if ( timlib_strncmp( input_buffer, STATION_LABEL ) != 0 )
			continue;

		fclose( input_file );

		timlib_reset_get_line_check_utf_16();

		if ( timlib_character_exists(
			input_buffer,
			',' ) )
		{
			piece( station, ',', input_buffer, 1 );
		}
		else
		{
			strcpy(	station,
				input_buffer + strlen( STATION_LABEL ) + 1 );

			trim( station );
		}

		if ( !station_exists(
				application_name,
				station ) )
		{
			printf(
		"<h3>Error: station = (%s) doesn't exist in STATION.</h3>\n",
				station );

			fclose( input_file );
			timlib_reset_get_line_check_utf_16();

			return (char *)0;
		}

		return strdup( station );
	}

	fclose( input_file );

	timlib_reset_get_line_check_utf_16();

	return (char *)0;

} /* station_label_fetch() */

void set_default_dates(		char **begin_date_string,
				char **begin_time_string,
				char **end_date_string,
				char **end_time_string )
{
	char *sys_string;

	if ( !**begin_date_string
	||   strcmp( *begin_date_string, "begin_date" ) == 0 )
	{
		sys_string = "now.sh ymd -30";
		*begin_date_string = pipe2string( sys_string );
	}

	if ( !**begin_time_string
	||   strcmp( *begin_time_string, "begin_time" ) == 0 )
	{
		*begin_time_string = "0000";
	}

	if ( !**end_date_string
	||   strcmp( *end_date_string, "end_date" ) == 0 )
	{
		sys_string = "now.sh ymd";
		*end_date_string = pipe2string( sys_string );
	}

	if ( !**end_time_string
	||   strcmp( *end_time_string, "end_time" ) == 0 )
	{
		*end_time_string = "2359";
	}

} /* set_default_dates() */

