/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_upload_csv.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "document.h"
#include "list.h"
#include "environ.h"
#include "process.h"
#include "application.h"
#include "exchange_csv.h"
#include "feeder.h"
#include "feeder_audit.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *login_name;
	char *feeder_account_name;
	char *csv_format_filename;
	int date_column;
	int description_column;
	int debit_column;
	int credit_column;
	int balance_column;
	int reference_column;
	boolean reverse_order_boolean;
	double balance_amount = 0.0;
	boolean execute_boolean;
	boolean okay_continue = 1;
	EXCHANGE_CSV *exchange_csv = {0};
	FEEDER *feeder = {0};
	FEEDER_AUDIT *feeder_audit;

	application_name =
		environment_exit_application_name(
			argv[ 0 ] );

	if ( argc != 14 )
	{
		fprintf( stderr,
"Usage: %s process_name login_name feeder_account filename date_column description_column debit_column credit_column balance_column reference_column reverse_order_yn account_end_balance execute_yn\n",
			 argv[ 0 ] );

		fprintf( stderr,
"\nNotes: Column numbers are one based. Delimiters are either comma only, or quote-comma.\n" );

		exit ( 1 );
	}

	process_name = argv[ 1 ];
	login_name = argv[ 2 ];
	feeder_account_name = argv[ 3 ];
	csv_format_filename = argv[ 4 ];
	date_column = atoi( argv[ 5 ] );
	description_column = atoi( argv[ 6 ] );
	debit_column = atoi( argv[ 7 ] );
	credit_column = atoi( argv[ 8 ] );
	balance_column = atoi( argv[ 9 ] );
	reference_column = atoi( argv[ 10 ] );
	reverse_order_boolean = (*argv[ 11 ] == 'y');
	balance_amount = atof( argv[ 12 ] );
	execute_boolean = (*argv[ 13 ] == 'y');

	appaserver_error_argv_file(
		argc,
		argv,
		application_name,
		login_name );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name /* title */ );

	if ( !*feeder_account_name
	||   strcmp( feeder_account_name, "feeder_account" ) == 0 )
	{
		printf( "<h3>Please choose a feeder account.</h3>\n" );

		okay_continue = 0;
	}

	if ( okay_continue
	&&   *csv_format_filename
	&&   strcmp(
		csv_format_filename,
		"csv_format_filename" ) != 0 )
	{
		exchange_csv =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			exchange_csv_fetch(
				application_name,
				csv_format_filename,
				date_column /* one based */,
				description_column /* one based */,
				debit_column /* one based */,
				credit_column /* one based */,
				balance_column /* one based */,
				reference_column /* one based */,
				reverse_order_boolean,
				balance_amount /* optional */,
				appaserver_parameter_upload_directory() );
	}

	if ( exchange_csv )
	{
		if ( okay_continue
		&&   !list_length( exchange_csv->exchange_journal_list ) )
		{
			printf(
"<h3>Sorry, but this CSV formatted file doesn't have any transactions.</h3>\n" );

			okay_continue = 0;
		}

		if ( okay_continue )
		{
			feeder =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				feeder_fetch(
					application_name,
					login_name,
					feeder_account_name,
					csv_format_filename,
					exchange_csv->exchange_journal_list,
					exchange_csv->
						exchange_journal_begin_amount,
					exchange_csv->balance_double,
					exchange_csv->
						exchange_minimum_date_string );
		}
	}

	if ( feeder )
	{
		execute_boolean =
			feeder_execute_boolean(
			    execute_boolean,
			    feeder->feeder_row_list_non_match_boolean,
			    feeder->
				feeder_row_list_status_out_of_balance_boolean );

		if ( !feeder->feeder_row_count )
		{
			printf( "<h3>No new feeder rows to process.</h3>\n" );
		}
		else
		if ( !feeder->latest_fetch_match_boolean )
		{
			char message[ 2048 ];

			feeder_display( feeder );

			snprintf(
				message,
				sizeof ( message ),
				FEEDER_INVALID_BEGIN_AMOUNT_TEMPLATE,
				feeder->
					feeder_load_event_latest_fetch->
					feeder_row_account_end_balance,
				exchange_csv->exchange_journal_begin_amount );

			printf( "%s\n", message );
		}
		else
		if ( execute_boolean
		&&   feeder->feeder_row_insert_count )
		{
			feeder_execute(
				process_name,
				(char *)0 /* fund_name */,
				feeder );
		}
		else
		{
			feeder_display( feeder );
		}
	}

	if ( !feeder
	||   !feeder->feeder_row_insert_count
	||   execute_boolean )
	{
		feeder_audit =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			feeder_audit_fetch(
				application_name,
				login_name,
				feeder_account_name );

		if ( !feeder_audit->feeder_load_event )
		{
			printf(
			"<h3>Warning: no feeder load events.</h3>\n" );
		}
		else
		if ( !feeder_audit->html_table )
		{
			printf(
			"<h3>ERROR: html_table is empty.</h3>\n" );
		}
		else
		{
			html_table_output(
				feeder_audit->html_table,
				HTML_TABLE_ROWS_BETWEEN_HEADING );
		}
	}

	document_close();

	return 0;
}

