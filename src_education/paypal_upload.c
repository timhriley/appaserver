/* -----------------------------------------------	*/
/* $APPASERVER_HOME/src_education/paypall_upload.c	*/
/* -----------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "list.h"
#include "environ.h"
#include "date.h"
#include "process.h"
#include "application.h"
#include "application_constants.h"
#include "paypal.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

PAYPAL *paypal_upload(
			char *spreadsheet_filename,
			char *login_name,
			char *fund_name,
			char *date_heading,
			boolean execute );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *login_name;
	char *fund_name;
	char *spreadsheet_filename;
	char *date_heading = {0};
	boolean execute;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char buffer[ 128 ];
	SPREADSHEET *spreadsheet;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 6 )
	{
		fprintf( stderr,
"Usage: %s process_name login_name fund filename execute_yn [date_heading]\n",
			 argv[ 0 ] );

		exit ( 1 );
	}

	process_name = argv[ 1 ];
	login_name = argv[ 2 ];
	fund_name = argv[ 3 ];
	spreadsheet_filename = argv[ 4 ];
	execute = (*argv[ 5 ] == 'y');

	if ( argc == 7 )
	{
		date_heading = argv[ 6 ];
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	printf( "<h1>%s\n",
		format_initial_capital( buffer, process_name ) );
	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
	printf( "</h1>\n" );
	fflush( stdout );

	if (	!*spreadsheet_filename
	||	strcmp( spreadsheet_filename, "filename" ) == 0 )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( ! ( spreadsheet =
			paypall_upload(
				login_name,
				fund_name,
				spreadsheet_filename,
				date_heading,
				execute ) ) )
	{
		printf( "<h3>Load aborted.</h3>\n" );
	}
	else
	if ( execute )
	{
		process_increment_execution_count(
			application_name,
			process_name,
			appaserver_parameter_file_get_dbms() );

		printf(
	"<p>Process complete as of %s with %d deposits.\n",
			spreadsheet->maximum_date,
			spreadsheet->load_count );
	}
	else
	{
		printf(
		"<p>Process did not load %d deposits.\n",
			spreadsheet->load_count );
	}

	document_close();
	return 0;
}

PAYPAL *paypall_upload(
			char *spreadsheet_filename,
			char *login_name,
			char *fund_name,
			char *date_heading,
			boolean execute )
{
	PAYPAL *paypal;

	paypal =
		paypal_new(
			spreadsheet_filename,
			login_name,
			fund_name,
			date_heading );

if ( login_name ){}
if ( fund_name ){}
if ( date_heading ){}

	spreadsheet =
		spreadsheet_fetch(
			spreadsheet_filename );

	if ( execute )
	{
	}
	else
	{
	}

	return paypal;
}

