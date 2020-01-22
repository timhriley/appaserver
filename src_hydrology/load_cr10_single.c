/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/load_cr10_single.c	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

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
#include "list.h"
#include "environ.h"
#include "date.h"
#include "process.h"
#include "dictionary.h"
#include "session.h"
#include "basename.h"
#include "application.h"
#include "appaserver_link_file.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void parse_begin_end_dates(
			char **begin_date_string,
			char **end_date_string,
			char *filename,
			char *station );

void output_bad_records(
		 	char *bad_parse_file,
		 	char *bad_insert_file );

void load_cr10_single(	char *appaserver_data_directory,
			char *filename,
			char *station,
			boolean change_existing_data,
			boolean execute );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *filename;
	char *station;
	boolean change_existing_data;
	boolean execute;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char buffer[ 512 ];

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
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
	change_existing_data = (*argv[ 4 ] == 'y');
	execute = (*argv[ 5 ] == 'y');

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	printf( "<h1>%s<br></h1>\n",
		format_initial_capital(
			buffer,
			process_name ) );
	
	printf( "<h2>\n" );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
	fflush( stdout );
	printf( " Station: %s</h2>\n",
		station );
	fflush( stdout );

	if ( !*filename || strcmp( filename, "filename" ) == 0 )
	{
		printf( "<h3> Please upload a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	load_cr10_single(
			appaserver_parameter_file->
				appaserver_data_directory,
			filename,
			station,
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
		printf( "<p>Process NOT executed.\n" );
	}

	document_close();
	return 0;

} /* main() */

void load_cr10_single(	char *appaserver_data_directory,
			char *filename,
			char *station,
			boolean change_existing_data,
			boolean execute )
{
	char sys_string[ 2048 ];
	char measurement_insert[ 1024 ];
	char bad_parse[ 128 ];
	char bad_insert[ 128 ];
	pid_t pid;
	char *dir;
	char *begin_date_string = {0};
	char *end_date_string = {0};

	parse_begin_end_dates(
		&begin_date_string,
		&end_date_string,
		filename,
		station );

	if ( !begin_date_string )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot get begin_date_string.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	pid = getpid();
	dir = appaserver_data_directory;

	sprintf( bad_parse, "%s/parse_%d.dat", dir, pid );
	sprintf( bad_insert, "%s/insert_%d.dat", dir, pid );

	sprintf( measurement_insert,
		 "measurement_insert			"
		 "	begin=%s			"
		 "	end=%s				"
		 "	replace=%c			"
		 "bypass_reject=%c			"
		 "execute=%c				"
		 "delimiter=','				",
		 begin_date_string,
		 end_date_string,
		 (change_existing_data) ? 'y' : 'n',
		 'y' /* Until Gordon's file can load. */,
		 (execute) ? 'y' : 'n' );


	sprintf( sys_string,
"cr10_parse \"%s\" \"%s\" n 2>%s	|"
"%s 2>%s				|"
"cat					 ",
		 filename,
		 station,
		 bad_parse,
		 measurement_insert,
		 bad_insert );

	if ( system( sys_string ) ) {};

	output_bad_records(
		 bad_parse,
		 bad_insert );

} /* load_cr10_file() */

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

void parse_begin_end_dates(
			char **begin_date_string,
			char **end_date_string,
			char *filename,
			char *station )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 128 ];

	sprintf( sys_string,
		 "cr10_parse \"%s\" \"%s\" y",
		 filename,
		 station );

	input_pipe = popen( sys_string, "r" );

	if ( get_line( input_buffer, input_pipe ) )
	{
		*begin_date_string = strdup( input_buffer );
	}

	if ( get_line( input_buffer, input_pipe ) )
	{
		*end_date_string = strdup( input_buffer );
	}

	pclose( input_pipe );

} /* parse_begin_end_dates() */

