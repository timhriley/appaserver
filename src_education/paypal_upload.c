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
#include "date.h"
#include "process.h"
#include "environ.h"
#include "list.h"
#include "payment.h"
#include "paypal.h"
#include "deposit.h"
#include "education.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void paypal_upload_event_insert(
			char *login_name,
			char *maximum_date );

void paypal_upload_display(
			LIST *education_deposit_list,
			char *season_name,
			int year );

/* Returns education_deposit_list() */
/* -------------------------------- */
LIST *paypal_upload_deposit_list(
			char **maximum_date,
			char *spreadsheet_name,
			char *spreadsheet_filename,
			char *season_name,
			int year,
			char *fund_name );

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
	boolean execute;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char buffer[ 128 ];
	char *maximum_date = {0};
	LIST *deposit_list;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 9 )
	{
		fprintf( stderr,
"Usage: %s process_name spreadsheet_name season_name year login_name fund filename execute_yn\n",
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

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	printf( "<h1>%s</h1><h2>",
		format_initial_capital( buffer, process_name ) );
	fflush( stdout );
	if ( system( timlib_system_date_string() ) ){}
	fflush( stdout );
	printf( "</h2>\n" );

	if (	!*spreadsheet_filename
	||	strcmp( spreadsheet_filename, "filename" ) == 0 )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	deposit_list =
		/* -------------------------------- */
		/* Returns education_deposit_list() */
		/* -------------------------------- */
		paypal_upload_deposit_list(
			&maximum_date,
			spreadsheet_name,
			spreadsheet_filename,
			season_name,
			year,
			fund_name );

	if ( !maximum_date || !list_length( deposit_list ) )
	{
		printf( "<h3>Invalid spreadsheet.</h3>\n" );
	}
	else
	if ( execute )
	{
		paypal_upload_display(
			deposit_list,
			season_name,
			year );

		education_deposit_list_insert( deposit_list );

		paypal_upload_event_insert(
			login_name,
			maximum_date );

			printf(
		"<p>Process complete as of %s with %d spreadsheet rows.\n",
				maximum_date,
				list_length( deposit_list ) );

		process_execution_count_increment(
			process_name );

	}
	else
	{
		paypal_upload_display(
			deposit_list,
			season_name,
			year );

			printf(
		"<p>Process did not load %d spreadsheet rows as of %s.\n",
				list_length( 
					deposit_list ),
				maximum_date );
	}

	document_close();
	return 0;
}

LIST *paypal_upload_deposit_list(
			char **maximum_date,
			char *spreadsheet_name,
			char *spreadsheet_filename,
			char *season_name,
			int year,
			char *fund_name )
{
	PAYPAL *paypal;
	EDUCATION *education;
	char *minimum_date;

	if ( ! ( minimum_date =
			spreadsheet_minimum_date(
				maximum_date,
				spreadsheet_filename ) ) )
	{
		return (LIST *)0;
	}

	if ( ! ( paypal =
			paypal_fetch(
				spreadsheet_name,
				spreadsheet_filename ) ) )
	{
		return (LIST *)0;
	}

	if ( ! ( education =
			education_fetch(
				season_name,
				year,
				spreadsheet_name,
				spreadsheet_filename ) ) )
	{
		return (LIST *)0;
	}

	if ( !education->semester )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: emnpty semester.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	education->education_deposit_list =
		education_deposit_list(
			season_name,
			year,
			spreadsheet_filename,
			paypal->spreadsheet,
			paypal->paypal_dataset,
			education->semester->semester_offering_list,
			education->semester->semester_registration_list,
			fund_name );

	return education->education_deposit_list;
}

void paypal_upload_display(
			LIST *education_deposit_list,
			char *season_name,
			int year )
{
if ( education_deposit_list ){}
if ( season_name ){}
if ( year ){}
}

void paypal_upload_event_insert(
			char *login_name,
			char *maximum_date )
{
if ( login_name ){}
if ( maximum_date ){}
}

