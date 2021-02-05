/* ----------------------------------------------------- */
/* $APPASERVER_HOME/src_creel/load_sport_fishing_trips.c */
/* ----------------------------------------------------- */
/* Freely available software: see Appaserver.org	 */
/* ----------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
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
#include "session.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *login_name;
	char *input_filename;
	boolean replace_existing_data;
	boolean execute;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char sys_string[ 1024 ];

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s process login_name filename replace_existing_data_yn execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	login_name = argv[ 2 ];
	input_filename = argv[ 3 ];
	replace_existing_data = ( *argv[ 4 ] == 'y' );
	execute = ( *argv[ 5 ] == 'y' );

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	printf( "<h2>Load Sport Fishing Trips\n" );
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

/*
	if ( execute && !replace_existing_data )
	{
		sprintf( sys_string,
		 	"delete_sport_creel_census %s \"%s\"",
		 	application_name,
		 	input_filename );
		fflush( stdout );
		if ( system( sys_string ) ){};
		fflush( stdout );
	}
*/

	sprintf( sys_string,
		 "insert_sport_creel_census %s \"%s\" %c %c",
		 application_name,
		 input_filename,
		 (replace_existing_data) ? 'y' : 'n',
		 (execute) ? 'y' : 'n' );

	fflush( stdout );
	if ( system( sys_string ) ){};
	fflush( stdout );

	sprintf( sys_string,
		 "insert_sport_fishing_trips %s %s \"%s\" %c %c",
		 application_name,
		 login_name,
		 input_filename,
		 (replace_existing_data) ? 'y' : 'n',
		 (execute) ? 'y' : 'n' );

	fflush( stdout );
	if ( system( sys_string ) ){};
	fflush( stdout );

	sprintf( sys_string,
		 "insert_sport_catches %s \"%s\" %c %c",
		 application_name,
		 input_filename,
		 (replace_existing_data) ? 'y' : 'n',
		 (execute) ? 'y' : 'n' );

	fflush( stdout );
	if ( system( sys_string ) ){};
	fflush( stdout );

	if ( execute )
		printf( "<p>Process complete\n" );
	else
		printf( "<p>Process not executed\n" );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );

	return 0;
}

