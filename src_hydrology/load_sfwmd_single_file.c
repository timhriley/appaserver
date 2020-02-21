/* -------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/load_sfwmd_single_file.c	*/
/* -------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* -------------------------------------------------------	*/

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
#include "boolean.h"
#include "process.h"
#include "dictionary.h"
#include "session.h"
#include "hydrology.h"
#include "application.h"
#include "basename.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void output_bad_records(
		 	char *bad_parse_file,
			char *bad_frequency_file,
		 	char *bad_insert_file );

void load_sfwmd_single_file(
			char *appaserver_data_directory,
			char *filename,
			boolean change_existing_data,
			boolean execute );

int main( int argc, char **argv )
{
	char *application_name;
	boolean change_existing_data;
	boolean execute;
	char *filename;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *process_name;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf( stderr, 
	"Usage: %s process filename change_existing_data_yn execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	filename = argv[ 2 ];
	change_existing_data = ( *argv[ 3 ] == 'y' );
	execute = ( *argv[ 4 ] == 'y' );

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	load_sfwmd_single_file(
		appaserver_parameter_file->
			appaserver_data_directory,
		filename,
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

void load_sfwmd_single_file(
			char *appaserver_data_directory,
			char *filename,
			boolean change_existing_data,
			boolean execute )
{
	char sys_string[ 1024 ];
	char bad_parse[ 128 ];
	char bad_frequency[ 128 ];
	char bad_insert[ 128 ];
	pid_t pid;
	char *dir;

	pid = getpid();
	dir = appaserver_data_directory;

	sprintf( bad_parse, "%s/sfwmd_parse_%d.dat", dir, pid );
	sprintf( bad_frequency, "%s/sfwmd_frequency_%d.dat", dir, pid );
	sprintf( bad_insert, "%s/sfwmd_insert_%d.dat", dir, pid );

	sprintf( sys_string,
"sfwmd_spreadsheet_parse \"%s\" 2>%s					|"
"measurement_frequency_reject '^' 2>%s					|"
"measurement_insert bypass=y replace=%c execute=%c 2>%s			|"
"cat									 ",
		 filename,
		 bad_parse,
		 bad_frequency,
		 (change_existing_data) ? 'y' : 'n',
		 (execute) ? 'y' : 'n',
		 bad_insert );

	if ( system( sys_string ) ) {};

	output_bad_records(
		bad_parse,
		bad_frequency,
		bad_insert );

} /* load_sfwmd_single_file() */

void output_bad_records(
		 	char *bad_parse_file,
			char *bad_frequency_file,
		 	char *bad_insert_file )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
	"cat %s %s %s | html_table.e '^^Bad Records' '' ''",
	 	bad_parse_file,
		bad_frequency_file,
	 	bad_insert_file );

	if ( system( sys_string ) ){};

} /* output_bad_records() */

