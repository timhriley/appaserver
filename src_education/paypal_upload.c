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
#include "folder_menu.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "date.h"
#include "process.h"
#include "environ.h"
#include "list.h"
#include "transaction.h"
#include "journal.h"
#include "tuition_payment.h"
#include "tuition_payment_fns.h"
#include "tuition_refund_fns.h"
#include "program_payment.h"
#include "program_payment_fns.h"
#include "product_payment_fns.h"
#include "product_refund_fns.h"
#include "paypal.h"
#include "deposit.h"
#include "education.h"
#include "paypal_upload.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void paypal_upload_event_insert(
			char *spreadsheet_filename,
			char *login_name,
			char *maximum_date );

void paypal_upload_display(
			LIST *education_deposit_list,
			char *season_name,
			int year );

/* Returns education_deposit_list() */
/* -------------------------------- */
LIST *paypal_upload_deposit_list(
			LIST *not_exists_course_name_list,
			char **maximum_date,
			char *spreadsheet_filename,
			char *season_name,
			int year );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *season_name;
	int year;
	char *login_name;
	char *spreadsheet_filename;
	boolean execute;
	char *role_name;
	char *session;
	boolean nohtml;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char buffer[ 128 ];
	char *maximum_date = {0};
	LIST *deposit_list;
	LIST *not_exists_course_name_list;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 9 )
	{
		fprintf( stderr,
"Usage: %s process_name session login_name role season_name year filename execute_yn|nohtml\n",
			 argv[ 0 ] );

		exit ( 1 );
	}

	process_name = argv[ 1 ];
	session = argv[ 2 ];
	if ( ( login_name = argv[ 3 ] ) ){};
	role_name = argv[ 4 ];
	season_name = argv[ 5 ];
	year = atoi( argv[ 6 ] );
	spreadsheet_filename = argv[ 7 ];
	execute = (*argv[ 8 ] == 'y');
	nohtml = ( strcmp( argv[ 8 ], "nohtml" ) == 0 );

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( !nohtml )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf( "<h1>%s</h1>",
			format_initial_capital(
				buffer,
				process_name ) );

		fflush( stdout );
		if ( system( timlib_system_date_string() ) ){}
	}

	if (	!*spreadsheet_filename
	||	strcmp( spreadsheet_filename, "filename" ) == 0 )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !*season_name || strcmp( season_name, "season_name" ) == 0 )
	{
		printf( "<h3>Please choose a semester.</h3>\n" );
		document_close();
		exit( 0 );
	}

	not_exists_course_name_list = list_new();

	deposit_list =
		deposit_list_steady_state(
			/* -------------------------------- */
			/* Returns education_deposit_list() */
			/* -------------------------------- */
			paypal_upload_deposit_list(
				not_exists_course_name_list,
				&maximum_date,
				spreadsheet_filename,
				season_name,
				year ),
		semester_offering_list(
			season_name,
			year ) );

	if ( !maximum_date )
	{
		printf( "<h3>Invalid spreadsheet.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( list_length( not_exists_course_name_list ) )
	{
		printf(
		"<h3>Can't execute with non-existing offering:</h3>\n" );

		fflush( stdout );
		list_html_display( not_exists_course_name_list );

		execute = 0;
	}

	if ( execute )
	{
		LIST *account_name_list;
		char *first_transaction_date_time;

		account_name_list =
			/* Sets transaction->transaction_date_time */
			/* --------------------------------------- */
			transaction_list_journal_program_insert(
				&first_transaction_date_time,
				deposit_list_transaction_list(
					deposit_list ),
				1 /* replace */ );

		if ( list_length( account_name_list ) )
		{
			journal_account_name_list_propagate(
				first_transaction_date_time,
				account_name_list );

			deposit_list =
				education_deposit_list_insert(
					deposit_list );

			deposit_list =
				deposit_list_registration_fetch_update(
					deposit_list,
					season_name,
					year );

			deposit_list =
				deposit_list_offering_fetch_update(
					deposit_list,
					season_name,
					year );

			if ( session && role_name )
			{
				folder_menu_refresh_role(
					application_name,
					session,
					role_name );
			}
/*
			paypal_upload_event_insert(
				spreadsheet_filename,
				login_name,
				maximum_date );
*/

			printf(
		"<p>Process complete with deposit count %d.\n",
				list_length( deposit_list ) );

			process_execution_count_increment(
				process_name );
		}
		else
		{
			printf(
"<p>Process not executed because no generated transactions; deposit count %d.\n",
				list_length( deposit_list ) );
		}
	}
	else
	{
		paypal_upload_display(
			deposit_list,
			season_name,
			year );

		printf(
		"<p>Process did not execute with deposit count %d.\n",
				list_length( deposit_list ) );
	}

	if ( !nohtml ) document_close();

	return 0;
}

LIST *paypal_upload_deposit_list(
			LIST *not_exists_course_name_list,
			char **maximum_date,
			char *spreadsheet_filename,
			char *season_name,
			int year )
{
	EDUCATION *education;
	char *minimum_date;
	LIST *deposit_list;

	/* Only a stub. Doesn't work yet. */
	/* ------------------------------ */
	if ( ! ( minimum_date =
			spreadsheet_minimum_date(
				maximum_date,
				spreadsheet_filename ) ) )
	{
		return (LIST *)0;
	}

	if ( ! ( education =
			education_fetch(
				season_name,
				year,
				spreadsheet_filename,
				"date" /* heading_label */ ) ) )
	{
		return (LIST *)0;
	}

	if ( !education->semester )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty semester.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !education->paypal )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty paypal.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	deposit_list =
		education_deposit_list(
			not_exists_course_name_list,
			season_name,
			year,
			spreadsheet_filename,
			education->paypal->spreadsheet,
			paypal_dataset_calloc(),
			education->semester->semester_offering_list,
			( education->education_program_list =
				education_program_list() ),
			( education->education_product_list =
				education_product_list() ) );

	return deposit_list;
}

void paypal_upload_display(
			LIST *deposit_list,
			char *season_name,
			int year )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	DEPOSIT *deposit;
	char buffer[ 128 ];
	char sub_title[ 128 ];
	char *heading;
	char *justification;

	sprintf(	sub_title,
			"Semester: %s/%d",
			format_initial_capital( buffer, season_name ),
			year );

	heading =	"row_number,"		\
			"payor,"		\
			"deposit_date_time,"	\
			"deposit_amount,"	\
			"transaction_fee,"	\
			"net_revenue,"		\
			"account_balance,"	\
			"payments/refunds";

	justification =	"right,"	\
			"left,"		\
			"left,"		\
			"right,"	\
			"right,"	\
			"right,"	\
			"right,"	\
			"left";

	sprintf(sys_string,
		"html_table '^%s' '%s' '^' '%s'",
		sub_title,
		heading,
		justification );

	if ( !list_rewind( deposit_list ) )
	{
		output_pipe = popen( sys_string, "w" );
		pclose( output_pipe );
		return;
	}

	do {
		deposit =
			list_get(
				deposit_list );

		output_pipe = popen( sys_string, "w" );

		fprintf(output_pipe,
			"%d^%s^%s^%.2lf^%.2lf^%.2lf^%.2lf^%s %s %s %s %s\n",
			deposit->row_number,
			entity_name_display(
				deposit->payor_entity->full_name,
				deposit->payor_entity->street_address ),
			deposit->deposit_date_time,
			deposit->deposit_amount,
			deposit->transaction_fee,
			deposit->net_revenue,
			deposit->account_balance,
			tuition_payment_list_display(
				deposit->deposit_tuition_payment_list ),
			program_payment_list_display(
				deposit->deposit_program_payment_list ),
			product_payment_list_display(
				deposit->deposit_product_payment_list ),
			tuition_refund_list_display(
				deposit->deposit_tuition_refund_list ),
			product_refund_list_display(
				deposit->deposit_product_refund_list ) );

		pclose( output_pipe );

		transaction_list_html_display(
			deposit_transaction_list(
				deposit->deposit_tuition_payment_list,
				deposit->deposit_program_payment_list,
				deposit->deposit_product_payment_list,
				deposit->deposit_tuition_refund_list,
				deposit->deposit_product_refund_list ) );

	} while ( list_next( deposit_list ) );
}

void paypal_upload_event_insert(
			char *spreadsheet_filename,
			char *login_name,
			char *maximum_date )
{
if ( spreadsheet_filename ){}
if ( login_name ){}
if ( maximum_date ){}
}

