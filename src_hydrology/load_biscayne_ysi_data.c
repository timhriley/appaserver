/* -------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/load_biscayne_ysi_data.c	*/
/* -------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* -------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "date_convert.h"
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
#include "appaserver_link_file.h"

/* Structures */
/* ---------- */
typedef struct
{
	char *datatype_name;
	int column_number;
} DATATYPE;

/* Constants */
/* --------- */
#define TEMPERATURE_HEADING		"Temp"
#define BOTTOM_TEMPERATURE_DATATYPE	"bottom_temperature"
#define SURFACE_TEMPERATURE_DATATYPE	"surface_temperature"
#define SALINITY_HEADING		"Sal"
#define SALINITY_DATATYPE		"salinity_raw"
#define CONDUCTIVITY_HEADING		"SpCond"
#define CONDUCTIVITY_DATATYPE		"conductivity"
#define DEPTH_HEADING			"Depth"
#define DEPTH_DATATYPE			"depth"
#define FEET_PER_METER			3.281

/* Prototypes */
/* ---------- */
void set_default_dates(			char **begin_date_string,
					char **begin_time_string,
					char **end_date_string,
					char **end_time_string );

void output_bad_records(
		 			char *bad_parse_file,
		 			char *bad_range_file,
		 			char *bad_frequency_file,
		 			char *bad_insert_file );

boolean get_is_odd_station(		char *station );

boolean datatype_exists(		char *application_name,
					char *datatype_name );

void load_biscayne_ysi_data(
					char *appaserver_data_directory,
					char *filename,
					char *station,
					char *begin_date_string,
					char *begin_time_string,
					char *end_date_string,
					char *end_time_string,
					boolean change_existing_data,
					boolean execute );

boolean station_filename_synchronized(	char *station,
					char *filename );

int main( int argc, char **argv )
{
	char *application_name;
	boolean change_existing_data;
	boolean execute;
	char *filename;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *station;
	char *process_name;
	char format_buffer[ 128 ];
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

	if ( argc != 10 )
	{
		fprintf( stderr, 
"Usage: %s process filename station begin_date begin_time end_date end_time change_existing_data_yn execute_yn\n",
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
	change_existing_data = ( *argv[ 8 ] == 'y' );
	execute = ( *argv[ 9 ] == 'y' );

	set_default_dates(	&begin_date_string,
				&begin_time_string,
				&end_date_string,
				&end_time_string );

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	/* Display the title and run stamp. */
	/* -------------------------------- */
	printf( "<h1>%s</h1>\n",
	 	format_initial_capital(
			format_buffer,
			process_name ) );

	printf( "<h2>" );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
	fflush( stdout );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !*station
	||   strcmp( station, "station" ) == 0
	||   strcmp( station, "biscayne_station" ) == 0 )
	{
		printf( "<h3>Error: Please choose a station.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !datatype_exists( application_name, BOTTOM_TEMPERATURE_DATATYPE ) )
	{
		printf(
"<h3>Error: the datatype bottom temperature has changed. It should be (%s).\n",
			BOTTOM_TEMPERATURE_DATATYPE );
		document_close();
		exit( 0 );
	}

	if ( !datatype_exists( application_name, SURFACE_TEMPERATURE_DATATYPE ))
	{
		printf(
"<h3>Error: the datatype bottom temperature has changed. It should be (%s).\n",
			SURFACE_TEMPERATURE_DATATYPE );
		document_close();
		exit( 0 );
	}

	if ( !datatype_exists( application_name, SALINITY_DATATYPE ) )
	{
		printf(
"<h3>Error: the datatype salinity has changed. It should be (%s).\n",
			SALINITY_DATATYPE );
		document_close();
		exit( 0 );
	}

	if ( !datatype_exists( application_name, DEPTH_DATATYPE ) )
	{
		printf(
"<h3>Error: the datatype depth has changed. It should be (%s).\n",
			DEPTH_DATATYPE );
		document_close();
		exit( 0 );
	}

	if ( !datatype_exists( application_name, CONDUCTIVITY_DATATYPE ) )
	{
		printf(
"<h3>Error: the datatype conductivity has changed. It should be (%s).\n",
			CONDUCTIVITY_DATATYPE );
		document_close();
		exit( 0 );
	}

	if ( !station_filename_synchronized(
		station,
		filename ) )
	{
		printf(
"<h3>Warning: the chosen station conflicts with the filename.</h3>\n" );
	}

	if ( !*filename || strcmp( filename, "filename" ) == 0 )
	{
		printf(
"<h3>Error: Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	load_biscayne_ysi_data(
			appaserver_parameter_file->
				appaserver_data_directory,
			filename,
			station,
			begin_date_string,
			begin_time_string,
			end_date_string,
			end_time_string,
			change_existing_data,
			execute );

	if ( execute )
	{
		printf( "<p>Process complete.\n" );

		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}
	else
	{
		printf( "<p>Process not executed.\n" );
	}

	document_close();

	return 0;

} /* main() */

void load_biscayne_ysi_data(
			char *appaserver_data_directory,
			char *filename,
			char *station,
			char *begin_date_string,
			char *begin_time_string,
			char *end_date_string,
			char *end_time_string,
			boolean change_existing_data,
			boolean execute )
{
	char sys_string[ 2048 ];
	char bad_parse[ 128 ];
	char bad_range[ 128 ];
	char bad_frequency[ 128 ];
	char bad_insert[ 128 ];
	pid_t pid;
	char *dir;

	pid = getpid();
	dir = appaserver_data_directory;

	sprintf( bad_parse, "%s/parse_%d.dat", dir, pid );
	sprintf( bad_range, "%s/range_%d.dat", dir, pid );
	sprintf( bad_frequency, "%s/reject_%d.dat", dir, pid );
	sprintf( bad_insert, "%s/insert_%d.dat", dir, pid );

	sprintf( sys_string,
"biscayne_spreadsheet_parse \"%s\" \"%s\" 2>%s				|"
"measurement_date_within_range	begin_date=%s				 "
"				begin_time=%s				 "
"				end_date=%s				 "
"				end_time=%s 2>%s			|"
"measurement_frequency_reject '^' 2>%s					|"
"measurement_insert bypass=y replace=%c execute=%c 2>%s			|"
"cat									 ",
		 filename,
		 station,
		 bad_parse,
		 begin_date_string,
		 begin_time_string,
		 end_date_string,
		 end_time_string,
		 bad_range,
		 bad_frequency,
		 (change_existing_data) ? 'y' : 'n',
		 (execute) ? 'y' : 'n',
		 bad_insert );

	if ( system( sys_string ) ) {};

	output_bad_records(
		 bad_parse,
		 bad_range,
		 bad_frequency,
		 bad_insert );

} /* void load_biscayne_ysi_data */

boolean station_filename_synchronized(
					char *station,
					char *filename )
{
	int station_number;
	int file_number;
	char file_number_string[ 3 ];
	char *base_filename;

	if ( instr( "BISC", station, 1 ) == -1 ) return 1;

	base_filename = basename_get_filename( filename );

	if ( strlen( base_filename ) < 3 ) return 1;

	*file_number_string = *base_filename;
	*(file_number_string + 1 ) = *(base_filename + 1);
	*(file_number_string + 2 ) = '\0';
	file_number = atoi( file_number_string );

	if ( !file_number ) return 1;

	station_number = atoi( station + 4 );

	if ( !station_number ) return 1;

	return ( station_number == file_number );

} /* station_filename_synchronized() */

boolean datatype_exists( char *application_name, char *datatype_name )
{
	char sys_string[ 1024 ];
	char where_clause[ 128 ];

	sprintf( where_clause, "datatype = '%s'", datatype_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=count			"
		 "			folder=datatype			"
		 "			where=\"%s\"			",
		 application_name,
		 where_clause );

	return atoi( pipe2string( sys_string ) );

} /* datatype_exists() */

boolean get_is_odd_station( char *station )
{
	char last_character_string[ 2 ];
	int last_character_integer;
	int str_len = strlen( station );

	if ( !str_len ) return 0;

	*last_character_string = *(station + str_len - 1 );
	*(last_character_string + 1) = '\0';

	if ( isalpha( *last_character_string ) )
	{
		return 0;
	}
	else
	{
		last_character_integer = atoi( last_character_string );
		return (last_character_integer % 2);
	}

} /* get_is_odd_station() */

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

