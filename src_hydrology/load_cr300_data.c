/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/load_cr300_data.c	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "julian.h"
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
#include "appaserver_link_file.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */
#define QUEUE_TOP_BOTTOM_LINES		100

/* Prototypes */
/* ---------- */
void output_bad_records(
				 	char *bad_parse_file,
				 	char *bad_insert_file );

void load_cr300_filespecification(	char *filename,
					char *station,
					boolean change_existing_data,
					boolean execute,
					char *appaserver_data_directory );

int main( int argc, char **argv )
{
	char *application_name;
	boolean change_existing_data;
	boolean execute;
	char *filename;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *input_directory;
	char *station;
	char *process_name;
	char format_buffer[ 128 ];

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s process filename station change_existing_data_yn execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	filename = argv[ 2 ];
	station = argv[ 3 ];
	change_existing_data = ( *argv[ 4 ] == 'y' );
	execute = ( *argv[ 5 ] == 'y' );

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
		printf(
			"<h3>ERROR: Please choose a station.</h3>\n" );
		document_close();
		exit( 1 );
	}

	load_cr300_filespecification(
		filename,
		station,
		change_existing_data,
		execute,
		appaserver_parameter_file->
			appaserver_data_directory );

	if ( execute )
	{
		printf( "<p>Execution complete.\n" );

		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}
	else
	{
		printf( "<p>Not executed.\n" );
	}

	document_close();

	return 0;

} /* main() */

void load_cr300_filespecification(
			char *filename,
			char *station,
			boolean change_existing_data,
			boolean execute,
			char *appaserver_data_directory )
{
	char sys_string[ 1024 ];
	char bad_parse[ 128 ];
	char bad_insert[ 128 ];
	char *date_heading_label;
	pid_t pid;
	char *dir;

	date_heading_label = "timestamp";
	pid = getpid();
	dir = appaserver_data_directory;

	sprintf( bad_parse, "%s/parse_%d.dat", dir, pid );
	sprintf( bad_insert, "%s/insert_%d.dat", dir, pid );

	sprintf( sys_string,
"spreadsheet_parse	file=\"%s\"					"
"			station=\"%s\"					"
"			date_heading_label=%s				"
"			time=no 2>%s					|"
"measurement_insert replace=%c execute=%c html=y 2>%s			|"
"cat									 ",
		 filename,
		 station,
		 date_heading_label,
		 bad_parse,
		 (change_existing_data) ? 'y' : 'n',
		 (execute) ? 'y' : 'n',
		 bad_insert );

	if ( system( sys_string ) ) {};

	output_bad_records(
		 bad_parse,
		 bad_insert );

} /* load_cr300_filespecification() */

void output_bad_records(
		 	char *bad_parse_file,
		 	char *bad_insert_file )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
	"cat %s %s | html_table.e '^^Bad Records' '' ''",
	 	bad_parse_file,
	 	bad_insert_file );

	if ( system( sys_string ) ){};

} /* output_bad_records() */

