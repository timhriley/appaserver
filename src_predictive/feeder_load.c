/* ----------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/feeder_load.c	*/
/* ----------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ----------------------------------------------------	*/

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
#include "feeder.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *login_name;
	char *feeder_account;
	char *feeder_load_filename;
	int date_column;
	int description_column;
	int debit_column;
	int credit_column;
	int balance_column;
	int reference_column;
	boolean reverse_order_boolean;
	double parameter_account_end_balance;
	boolean execute_boolean;
	FEEDER *feeder;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char buffer[ 128 ];

	/* Exits if not found. */
	/* ------------------- */
	application_name = environ_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

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
	feeder_account = argv[ 3 ];
	feeder_load_filename = argv[ 4 ];
	date_column = atoi( argv[ 5 ] );
	description_column = atoi( argv[ 6 ] );
	debit_column = atoi( argv[ 7 ] );
	credit_column = atoi( argv[ 8 ] );
	balance_column = atoi( argv[ 9 ] );
	reference_column = atoi( argv[ 10 ] );
	reverse_order_boolean = (*argv[ 11 ] == 'y' );
	parameter_account_end_balance = atof( argv[ 12 ] );
	execute_boolean = (*argv[ 13 ] == 'y');

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	printf( "<h1>%s\n",
		format_initial_capital(
			buffer,
			process_name ) );

	fflush( stdout );
	if ( system( "TZ=`appaserver_tz.sh` date '+%x %H:%M'" ) ) {};
	printf( "</h1>\n" );
	fflush( stdout );

	if ( !*feeder_load_filename
	||   strcmp( feeder_load_filename, "filename" ) == 0 )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( !*feeder_account
	||   strcmp( feeder_account, "feeder_account" ) == 0 )
	{
		printf( "<h3>Please choose a feeder account.</h3>\n" );
		document_close();
		exit( 0 );
	}

	feeder =
		feeder_fetch(
			login_name,
			feeder_account,
			feeder_load_filename,
			date_column,
			description_column,
			debit_column,
			credit_column,
			balance_column,
			reference_column,
			reverse_order_boolean,
			parameter_account_end_balance );

	if ( !feeder )
	{
		printf(
		"<h3>Error: could not parse the input file.</h3>\n" );
		document_close();
		exit( 1 );
	}

	if ( !feeder->feeder_load_file )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: feeder_fetch() returned incomplete.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( execute_boolean )
	{
		FEEDER_AUDIT *feeder_audit;

		if ( feeder_row_seek_matched_count(
			feeder->feeder_row_list,
			feeder->feeder_row_first_out_balance ) )
		{
			feeder_row_list_insert(
				feeder_account,
				feeder->
					feeder_load_event->
					feeder_load_date_string,
				feeder->feeder_row_list,
				feeder->feeder_row_first_out_balance );

			feeder_row_transaction_insert(
				feeder->feeder_row_list,
				feeder->feeder_row_first_out_balance );

			feeder_load_event_insert(
				FEEDER_LOAD_EVENT_TABLE,
				FEEDER_LOAD_EVENT_INSERT,
				feeder->
					feeder_load_event->
					feeder_account,
				feeder->
					feeder_load_event->
					feeder_load_date_string );

			feeder_load_event_update(
				FEEDER_LOAD_EVENT_TABLE,
				FEEDER_LOAD_EVENT_PRIMARY_KEY,
				feeder->feeder_load_event->feeder_account,
				feeder->
					feeder_load_event->
					feeder_load_date_string,
				feeder->feeder_load_event->login_name,
				feeder->feeder_load_event->basename_filename,
				feeder->account_end_date,
				feeder->account_end_balance );

			process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );

			printf( "%s\n",
				/* ------------------------- */
				/* Returns heap memory or "" */
				/* ------------------------- */
				feeder_parameter_account_end_balance_error(
					parameter_account_end_balance,
					feeder_load_row_account_end_balance(
						(FEEDER_LOAD_ROW *)list_last(
						   feeder->
						      feeder_load_file->
						      feeder_load_row_list ) ),
					feeder->feeder_row_first_out_balance,
					feeder_row_seek_matched_count(
					   feeder->feeder_row_list,
					   feeder->
					    feeder_row_first_out_balance ) ) );

			printf( "<h3>Process complete.</h3>\n" );
		}
		else
		{
			printf( "<h3>No transactions to process.</h3>\n" );
		}

		if ( ( feeder_audit =
			feeder_audit_fetch(
				feeder_account,
				feeder->feeder_load_date_string ) ) )
		{
			html_table_output(
				feeder_audit->html_table,
				HTML_TABLE_ROWS_BETWEEN_HEADING );
		}
	}
	else
	{
		printf( "%s\n",
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			feeder_parameter_account_end_balance_error(
				parameter_account_end_balance,
				feeder_load_row_account_end_balance(
					(FEEDER_LOAD_ROW *)list_last(
						feeder->
						   feeder_load_file->
						   feeder_load_row_list ) ),
				feeder->feeder_row_first_out_balance,
				feeder_row_seek_matched_count(
					feeder->feeder_row_list,
					feeder->
					     feeder_row_first_out_balance ) ) );

		if ( feeder->feeder_row_first_out_balance )
		{
			printf( "<h1>No Transactions</h1>\n" );
			fflush( stdout );

			if ( feeder_row_error_display(
				feeder->feeder_row_list ) )
			{
				feeder_matched_journal_not_taken_display(
					feeder->feeder_matched_journal_list );
			}
		}
				
		printf( "<h1>All Transactions</h1>\n" );
		fflush( stdout );
		feeder_row_list_display(
			feeder->feeder_row_list,
			feeder->feeder_row_first_out_balance );

		printf( "<h3>Execute load count: %d</h3>\n",
			feeder_row_seek_matched_count(
				feeder->feeder_row_list,
				feeder->feeder_row_first_out_balance ) );
	}

	document_close();

	exit( 0 );
}

