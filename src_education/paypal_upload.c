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
#include "process.h"
#include "environ.h"
#include "list.h"
#include "paypal.h"
#include "deposit.h"
#include "education.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

void paypal_upload_display(
			char *spreadsheet_filename,
			char *season_name,
			int year,
			LIST *education_deposit_list );

void paypal_upload_execute(
			char *spreadsheet_filename,
			char *season_name,
			int year,
			LIST *education_deposit_list );

PAYPAL *paypal_upload(	char *spreadsheet_name,
			char *spreadsheet_filename,
			char *season_name,
			int year,
			char *login_name,
			char *fund_name,
			char *date_heading );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *spreadsheet_name;
	char *season_name;
	int year;
	char *login_name;
	char *fund_name;
	char *spreadsheet_filename;
	char *date_heading = {0};
	boolean execute;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char buffer[ 128 ];
	EDUCATION *education;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 9 )
	{
		fprintf( stderr,
"Usage: %s process_name spreadsheet_name season_name year login_name fund filename execute_yn [date_heading]\n",
			 argv[ 0 ] );

		exit ( 1 );
	}

	process_name = argv[ 1 ];
	spreadsheet_name = argv[ 2 ];
	season_name = argv[ 3 ];
	year = atoi( argv[ 4 ] );
	login_name = argv[ 5 ];
	fund_name = argv[ 6 ];
	spreadsheet_filename = argv[ 7 ];
	execute = (*argv[ 8 ] == 'y');

	if ( argc == 10 )
	{
		date_heading = argv[ 9 ];
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

	if ( ! ( education =
			education_fetch(
				season_name,
				year,
				spreadsheet_name,
				spreadsheet_filename ) ) )
	{
		printf(
		"<h3>An internal error occurred. Please check log.</h3>\n" );

		document_close();
		exit( 1 );
	}

	education->deposit_list =
		paypal_upload_deposit_list(
			spreadsheet_name,
			spreadsheet_filename,
			season_name,
			year,
			login_name,
			fund_name,
			date_heading );

	if ( !list_length( deposit_list ) )
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
			paypal->spreadsheet->maximum_date,
			paypal->spreadsheet->load_count );
	}
	else
	{
		printf(
		"<p>Process did not load %d deposits.\n",
			paypal->spreadsheet->load_count );
	}

	document_close();
	return 0;
}

PAYPAL *paypall_upload(
			char *spreadsheet_name,
			char *spreadsheet_filename,
			char *season_name,
			int year,
			char *login_name,
			char *fund_name,
			char *date_heading,
			boolean execute )
{
	PAYPAL *paypal;

	if ( ! ( paypal =
			paypal_fetch(
				spreadsheet_name,
				spreadsheet_filename ) ) )
	{
		return (PAYPAL *)0;
	}

	if ( execute )
	{
		return paypal_upload_execute(
				spreadsheet_filename,
				season_name,
				year,
				/* Don't take anything from here */
				/* ----------------------------- */
				paypal );

	}
	else
	{
		return paypal_upload_display(
				spreadsheet_filename,
				season_name,
				year,
				/* Don't take anything from here */
				/* ----------------------------- */
				paypal );
	}

	return paypal;
}

/* Returns deposit_list */
/* -------------------- */
LIST *paypal_upload_execute(
			char *spreadsheet_filename,
			char *season_name,
			int year,
			PAYPAL *paypal )
{
	EDUCATION *education;
	LIST *deposit_list;
	char input[ 65536 ];

	if ( ! ( education =
			education_fetch(
				season_name,
				year ) ) )
	{
		return (LIST *)0;
	}

	education->deposit_list =
		paypal_deposit_list(
			spreadsheet_file,
			paypal->spreadsheet,
			paypal->paypal_datasheet,
			education->semester_offering_list,
			education->semester_registration_list );

	fclose( spreadsheet_file );

	return education->deposit_list;
}

PAYPAL *paypal_upload_display(
			char *spreadsheet_filename,
			char *season_name,
			int year,
			PAYPAL *paypal )
{
}

