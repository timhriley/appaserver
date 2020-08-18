/* ------------------------------------------		*/
/* $APPASERVER_HOME/src_hydrology/LT_upload.c		*/
/* ------------------------------------------		*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "list.h"
#include "environ.h"
#include "date.h"
#include "process.h"
#include "date_convert.h"
#include "application.h"
#include "basename.h"
#include "hydrology.h"
#include "application_constants.h"

/* Constants */
/* --------- */
#define QUEUE_TOP_BOTTOM_LINES		80

/* Prototypes */
/* ---------- */
boolean reject_non_zero_seconds(char *input_buffer );

void output_bad_records(
			 	char *bad_parse_file,
				char *bad_time_file,
			 	char *bad_insert_file );

void LT_upload(		
				char *filename,
				char *station,
				boolean change_existing_data,
				boolean execute,
				char *appaserver_data_directory );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *station;
	boolean change_existing_data;
	boolean execute;
	char *filename;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char buffer[ 128 ];

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
"Usage: %s process_name station filename change_existing_data_yn execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	station = argv[ 2 ];
	filename = argv[ 3 ];
	change_existing_data = (*argv[ 4 ] == 'y');
	execute = (*argv[ 5 ] == 'y');

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	printf( "<h2>%s\n", format_initial_capital( buffer, process_name ) );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ){};
	printf( "</h2>\n" );
	fflush( stdout );

	if ( !*filename || strcmp( filename, "filename" ) == 0 )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	LT_upload(
		filename,
		station,
		change_existing_data,
		execute,
		appaserver_parameter_file->
			appaserver_data_directory );

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
		printf( "<p>Process did not execute.\n" );
	}

	document_close();

	return 0;

} /* main() */

void LT_upload(		char *filename,
			char *station,
			boolean change_existing_data,
			boolean execute,
			char *appaserver_data_directory )
{
	char sys_string[ 1024 ];
	char input_buffer[ 65536 ];
	char measurement_insert[ 512 ];
	char bad_parse[ 128 ];
	char bad_time[ 128 ];
	char bad_insert[ 128 ];
	pid_t pid;
	char *dir;
	FILE *input_file;
	FILE *output_pipe;
	FILE *bad_time_file;

	pid = getpid();
	dir = appaserver_data_directory;

	sprintf( bad_parse, "%s/parse_%d.dat", dir, pid );
	sprintf( bad_time, "%s/time_%d.dat", dir, pid );
	sprintf( bad_insert, "%s/insert_%d.dat", dir, pid );

	/* Open input file */
	/* --------------- */
	if ( ! ( input_file = fopen( filename, "r" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for read.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 filename );

		printf( "<h3>An internal error occurred. Check log.</h3>\n" );
		document_close();
		exit( 0 );
	}

	/* Open bad time file */
	/* ------------------ */
	if ( ! ( bad_time_file = fopen( bad_time, "w" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for write.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 bad_time );

		printf( "<h3>An internal error occurred. Check log.</h3>\n" );
		document_close();
		exit( 0 );
	}

	sprintf( measurement_insert,
		 "measurement_insert replace=%c execute=%c",
		 (change_existing_data) ? 'y' : 'n',
		 (execute) ? 'y' : 'n' );

	/* Open output pipe */
	/* --------------- */
	sprintf( sys_string,
"spreadsheet_parse file=\"%s\" station=\"%s\" time=no stdin=yes 2>%s	|"
"%s 2>%s								|"
"cat									 ",
		 filename,
		 station,
		 bad_parse,
		 measurement_insert,
		 bad_insert );

	output_pipe = popen( sys_string, "w" );

	while( timlib_get_line( input_buffer, input_file, 65536 ) )
	{
		if ( reject_non_zero_seconds( input_buffer ) )
		{
			fprintf( bad_time_file,
				 "Bad time in: %s\n",
				 input_buffer );
			continue;
		}

		fprintf( output_pipe, "%s\n", input_buffer );
	}

	fclose( input_file );
	fclose( bad_time_file );
	pclose( output_pipe );

	output_bad_records(
		bad_parse,
		bad_time,
		bad_insert );

} /* LT_upload() */

void output_bad_records(
		 	char *bad_parse_file,
			char *bad_time_file,
		 	char *bad_insert_file )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
	"cat %s %s %s					|"
	"queue_top_bottom_lines.e %d			|"
	"html_table.e '^^Bad Records' '' ''		 ",
	 	bad_parse_file,
		bad_time_file,
	 	bad_insert_file,
	QUEUE_TOP_BOTTOM_LINES );

	if ( system( sys_string ) ){};
}

boolean reject_non_zero_seconds( char *input_buffer )
{
	char measurement_date_time_string[ 128 ];
	char time_piece[ 128 ];
	char seconds_piece[ 16 ];

	piece_quoted(	measurement_date_time_string,
			',',
			input_buffer,
			0,
			'"' );

	/* Ignore heading line. */
	/* -------------------- */
	if ( !isdigit( *measurement_date_time_string ) )
		return 0;

	if ( !piece( time_piece, 'T', measurement_date_time_string, 1 ) )
		return 1;

	if ( !piece( seconds_piece, ':', time_piece, 2 ) )
		return 1;

	/* Reject if seconds are not zero. */
	/* ------------------------------- */
	if ( atoi( seconds_piece ) ) return 1;

	/* Don't reject */
	/* ------------ */
	return 0;

} /* reject_non_zero_seconds() */

