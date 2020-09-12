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
#include "bank_upload.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
/* Returns either file_row_count or table_insert_count */
/* --------------------------------------------------- */
int paypall_upload(		int *transaction_count,
				char **minimum_bank_date,
				char *login_name,
				char *fund_name,
				char *feeder_account,
				char *input_filename,
				boolean reverse_order,
				int date_piece_offset,
				int description_piece_offset,
				int debit_piece_offset,
				int credit_piece_offset,
				int balance_piece_offset,
				boolean execute );

/* Global variables */
/* ---------------- */
enum bank_upload_exception bank_upload_exception = {0};

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *login_name;
	char *fund_name;
	char *feeder_account;
	char *input_filename;
	char *date_column_string;
	char *description_column_string;
	char *debit_column_string;
	char *credit_column_string;
	char *balance_column_string;
	int date_piece_offset;
	int description_piece_offset;
	int debit_piece_offset;
	int credit_piece_offset;
	int balance_piece_offset;
	boolean reverse_order;
	boolean execute;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	int load_count = 0;
	int transaction_count = 0;
	char *minimum_bank_date = {0};
	char buffer[ 128 ];

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 14 )
	{
		fprintf( stderr,
"Usage: %s process_name login_name fund feeder_account filename date_column description_column debit_column credit_column balance_column ignored reverse_order_yn execute_yn\n",
			 argv[ 0 ] );

		fprintf( stderr,
"\nNotes: Column numbers are one based. Delimiters are either comma only, or quote-comma.\n" );

		exit ( 1 );
	}

	process_name = argv[ 1 ];
	login_name = argv[ 2 ];
	fund_name = argv[ 3 ];
	feeder_account = argv[ 4 ];
	input_filename = argv[ 5 ];
	date_column_string = argv[ 6 ];
	description_column_string = argv[ 7 ];
	debit_column_string = argv[ 8 ];
	credit_column_string = argv[ 9 ];
	balance_column_string = argv[ 10 ];
	/* transactions_only = (*argv[ 11 ] == 'y'); */
	reverse_order = (*argv[ 12 ] == 'y' );
	execute = (*argv[ 13 ] == 'y');

	if ( *date_column_string
	&&   strcmp( date_column_string, "date_column" ) != 0 )
	{
		date_piece_offset = atoi( date_column_string ) - 1;
	}
	else
	{
		date_piece_offset = 0;
	}

	if ( *description_column_string
	&&   strcmp( description_column_string, "description_column" ) != 0 )
	{
		description_piece_offset =
			atoi( description_column_string ) - 1;
	}
	else
	{
		description_piece_offset = 1;
	}

	if ( *debit_column_string
	&&   strcmp( debit_column_string, "debit_column" ) != 0 )
	{
		debit_piece_offset = atoi( debit_column_string ) - 1;
	}
	else
	{
		debit_piece_offset = 2;
	}

	if ( *credit_column_string
	&&   strcmp( credit_column_string, "credit_column" ) != 0 )
	{
		credit_piece_offset = atoi( credit_column_string ) - 1;
	}
	else
	{
		/* The same as debit_piece_offset */
		/* ------------------------------ */
		credit_piece_offset = 2;
	}

	if ( *balance_column_string
	&&   strcmp( balance_column_string, "balance_column" ) != 0 )
	{
		balance_piece_offset = atoi( balance_column_string ) - 1;
	}
	else
	{
		balance_piece_offset = 3;
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

	if ( !*input_filename || strcmp( input_filename, "filename" ) == 0 )
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

	load_count =
		paypall_upload(
			&transaction_count,
			&minimum_bank_date,
			login_name,
			fund_name,
			feeder_account,
			input_filename,
			reverse_order,
			date_piece_offset,
			description_piece_offset,
			debit_piece_offset,
			credit_piece_offset,
			balance_piece_offset,
			execute );

	if ( !minimum_bank_date )
	{
		printf(
		"<h3>Error: could not fetch the minimum bank date.</h3>\n" );
		document_close();
		exit( 1 );
	}

	if ( execute )
	{
		process_increment_execution_count(
			application_name,
			process_name,
			appaserver_parameter_file_get_dbms() );

		printf(
	"<p>Process complete as of %s with %d rows and %d transactions.\n",
			minimum_bank_date,
			load_count,
			transaction_count );
	}
	else
	{
		printf(
		"<p>Process did not load %d rows nor %d transactions.\n",
			load_count,
			transaction_count );
	}

	document_close();

	exit( 0 );

} /* main() */

/* ---------------------------------------------------- */
/* If display then it returns file_row_count.		*/
/* If execute then it returns table_insert_count.	*/
/* ---------------------------------------------------- */
int paypall_upload(
			int *transaction_count,
			char **minimum_bank_date,
			char *login_name,
			char *fund_name,
			char *feeder_account,
			char *input_filename,
			boolean reverse_order,
			int date_piece_offset,
			int description_piece_offset,
			int debit_piece_offset,
			int credit_piece_offset,
			int balance_piece_offset,
			boolean execute )
{
	PAYPAL_UPLOAD *paypal_upload;

	*transaction_count = 0;

	paypal_upload =
		paypal_upload_new(
			fund_name,
			feeder_account,
			input_filename,
			reverse_order,
			date_piece_offset,
			description_piece_offset,
			debit_piece_offset,
			credit_piece_offset,
			balance_piece_offset );

	if ( !paypal_upload ) return 0;

/* -------------------------------------------------------------------- */
/* Sets bank_upload->feeder_check_number_existing_journal		*/
/* or									*/
/* Sets bank_upload->feeder_phrase_match_build_transaction		*/
/* or									*/
/* Sets bank_upload->feeder_match_sum_existing_journal_list		*/
/* -------------------------------------------------------------------- */
	bank_upload_set_transaction(
		paypal_upload->file.bank_upload_list,
		paypal_upload->
			reoccurring_structure->
			reoccurring_transaction_list,
		paypal_upload->
			existing_cash_journal_list );

	if ( bank_upload_sha256sum_exists(
			paypal_upload->file.file_sha256sum ) )
	{
		char *msg;

		if ( execute )
			msg = "<h3>ERROR: duplicated file.</h3>";
		else
			msg = "<h3>Warning: duplicated file.</h3>";

		printf( "%s\n", msg );

		bank_upload_exception = duplicated_spreadsheet_file;

		execute = 0;
	}

	*minimum_bank_date =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		bank_upload_minimum_bank_date(
			paypal_upload->
				file.
				minimum_bank_date,
			paypal_upload->
				file.
				bank_upload_list );

	if ( !execute )
	{
		bank_upload_table_display(
			paypal_upload->
				file.
				bank_upload_list );

		bank_upload_transaction_table_display(
			paypal_upload->
				file.
				bank_upload_list );

		*transaction_count =
			bank_upload_feeder_phrase_match_transaction_count(
				paypal_upload->
				file.
				bank_upload_list );
	}
	else
	/* ------------ */
	/* Else execute */
	/* ------------ */
	{
		LIST *transaction_list;

		if ( ! ( paypal_upload->file.table_insert_count =
				bank_upload_insert(
					fund_name,
					paypal_upload->
						file.
						bank_upload_list
						   /* bank_upload_list */,
					paypal_upload->
						bank_upload_date_time ) ) )
		{
			return 0;
		}

		bank_upload_event_insert(
			paypal_upload->bank_upload_date_time,
			login_name,
			paypal_upload->
				file.
				input_filename
					/* bank_upload_filename */,
			paypal_upload->
				file.
				file_sha256sum,
			paypal_upload->fund_name,
			paypal_upload->feeder_account );

		bank_upload_archive_insert(
			fund_name,
			paypal_upload->
				file.
				bank_upload_list,
			paypal_upload->
				bank_upload_date_time );

		transaction_list =
			bank_upload_transaction_list(
				paypal_upload->
					file.
					bank_upload_list );

		/* transaction_list_stderr( transaction_list ); */

		/* ------------------------------------ */
		/* Insert into TRANSACTION and JOURNAL	*/
		/* ------------------------------------ */
		/* Note: this is the bottleneck.	*/
		/* ------------------------------------ */
		transaction_list_insert( transaction_list );

		bank_upload_transaction_table_display(
			paypal_upload->file.bank_upload_list );

		*transaction_count =
			bank_upload_feeder_phrase_match_transaction_count(
				paypal_upload->
					file.
					bank_upload_list );

		/* Insert into BANK_UPLOAD_TRANSACTION */
		/* ----------------------------------- */
		bank_upload_direct_bank_upload_transaction_insert(
			paypal_upload->
				file.
				bank_upload_list );

		/* ------------------------ */
		/* Update JOURNAL.account   */
		/* Does journal_propagate() */
		/* ------------------------ */
		bank_upload_cleared_checks_update(
			paypal_upload->fund_name,
			paypal_upload->
				file.
				bank_upload_list );

		bank_upload_transaction_balance_propagate(
			*minimum_bank_date );
	}

	if ( list_length( paypal_upload->file.error_line_list ) )
	{
		printf( "<h3>Errors:</h3>\n" );

		list_display_lines(
			paypal_upload->
				file.
				error_line_list );

		printf( "\n" );
	}

	if ( bank_upload_exception == duplicated_spreadsheet_file )
		return 0;
	else
	if ( !execute )
		return paypal_upload->file.file_row_count;
	else
		return paypal_upload->file.table_insert_count;
}

