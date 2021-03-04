/* ----------------------------------------------------------------	*/
/* src_predictive/post_cash_expense_transaction.c			*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "environ.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "date.h"
#include "folder_menu.h"
#include "accrual.h"
#include "boolean.h"
#include "journal.h"
#include "transaction.h"
#include "account.h"
#include "bank_upload.h"
#include "predictive.h"

/* Constants */
/* --------- */
#define OUTPUT_MEDIUM_TABLE			"table"
#define OUTPUT_MEDIUM_TEXT			"text"
#define BANK_UPLOAD_TRANSACTION_FOLDER_NAME	"bank_upload_transaction"

/* Prototypes */
/* ---------- */
TRANSACTION *post_cash_expense_transaction(
				FILE *output_pipe,
				char *full_name,
				char *street_address,
				char *bank_date,
				char *bank_description_embedded,
				char *debit_account,
				char *memo );

/* Global variables */
/* ---------------- */
enum bank_upload_exception bank_upload_exception = {0};

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *session;
	char *role_name;
	char *full_name;
	char *street_address;
	char *bank_date;
	char *bank_description_embedded;
	char *debit_account;
	char *memo;
	char *output_medium;
	boolean execute;
	char title[ 128 ];
	DOCUMENT *document = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *heading = {0};
	char *justify = {0};
	char sys_string[ 512 ];
	FILE *output_pipe = {0};
	TRANSACTION *transaction;

	/* This exits if it fails. */
	/* ----------------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 12 )
	{
		fprintf( stderr,
"Usage: %s process session role full_name street_address bank_date bank_description_embedded debit_account memo output_medium execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	session = argv[ 2 ];
	role_name = argv[ 3 ];
	full_name = argv[ 4 ];
	street_address = argv[ 5 ];
	bank_date = argv[ 6 ];
	bank_description_embedded = argv[ 7 ];
	debit_account = argv[ 8 ];
	memo = argv[ 9 ];
	output_medium = argv[ 10 ];
	execute = (*argv[ 11 ] == 'y');

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( !*output_medium || strcmp( output_medium, "output_medium" ) == 0 )
	{
		output_medium = OUTPUT_MEDIUM_TABLE;
	}

	if ( strcmp( output_medium, OUTPUT_MEDIUM_TABLE ) == 0 )
	{
		format_initial_capital( title, process_name );
		document = document_new( title, application_name );
		document->output_content_type = 1;
	
		document_output_heading(
				document->application_name,
				document->title,
				document->output_content_type,
				appaserver_parameter_file->
					appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );
	
		document_output_body(	document->application_name,
					document->onload_control_string );
	
		printf( "<h1>%s</h1>\n", title );
		fflush( stdout );
	}

	if ( strcmp( output_medium, OUTPUT_MEDIUM_TABLE ) == 0 )
	{
		heading = "Transaction,Account,Debit,Credit";
		justify = "left,left,right,right";

		sprintf(sys_string,
			"html_table.e '' %s '^' %s",
			heading,
			justify );
	}
	else
	if ( strcmp( output_medium, OUTPUT_MEDIUM_TEXT ) == 0 )
	{
		heading = "Transaction^Account^Debit^Credit";

		sprintf(sys_string,
			"(echo \"%s\"; cat)			|"
			"delimiter2padded_columns.e '^' 2	 ",
			heading );
	}
	else
	{
		strcpy( sys_string, "cat" );
	}

	output_pipe = popen( sys_string, "w" );

	transaction =
		post_cash_expense_transaction(
				output_pipe,
				full_name,
				street_address,
				bank_date,
				bank_description_embedded,
				debit_account,
				memo );

	pclose( output_pipe );

	if ( transaction && execute )
	{
		transaction->transaction_date_time =
			transaction_journal_insert(
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				transaction->transaction_amount,
				transaction->memo,
				transaction->check_number,
				transaction->lock_transaction,
				transaction->journal_list,
				0 /* not replace */ );

		bank_upload_transaction_direct_insert(
			bank_date,
			bank_description_embedded,
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			(char *)0 /* fund_name */ );

		bank_upload_transaction_balance_propagate(
			bank_date,
			transaction->transaction_date_time );

		folder_menu_refresh_row_count(
			application_name,
			TRANSACTION_FOLDER_NAME,
			session,
			appaserver_parameter_file->
				appaserver_data_directory,
			role_name );

		folder_menu_refresh_row_count(
			application_name,
			JOURNAL_TABLE,
			session,
			appaserver_parameter_file->
				appaserver_data_directory,
			role_name );

		folder_menu_refresh_row_count(
			application_name,
			BANK_UPLOAD_TRANSACTION_FOLDER_NAME,
			session,
			appaserver_parameter_file->
				appaserver_data_directory,
			role_name );

		printf( "<h3>Posted.</h3>\n" );
	}
	else
	{
		printf( "<h3>Not posted.</h3>\n" );
	}

	return 0;
}

TRANSACTION *post_cash_expense_transaction(
				FILE *output_pipe,
				char *full_name,
				char *street_address,
				char *bank_date,
				char *bank_description,
				char *debit_account,
				char *memo )
{
	TRANSACTION *transaction;
	double bank_amount;
	char *credit_account;

	if ( bank_upload_transaction_exists(
				bank_date,
				bank_description ) )
	{
		fprintf( output_pipe,
			 "<h3>Error: Bank Upload already posted.</h3>\n" );
		return (TRANSACTION *)0;
	}

	credit_account = account_cash( (char *)0 );

	if ( ! ( bank_amount =
			bank_upload_bank_amount(
				bank_date,
				bank_description ) ) )
	{
		fprintf( output_pipe, "<h3>Error: empty bank amount.</h3>\n" );
		return (TRANSACTION *)0;
	}

	if ( ! ( transaction =
			transaction_binary(
				full_name,
				street_address,
				predictive_transaction_date_time(
					bank_date ),
				0.0 - bank_amount /* transaction_amount */,
				memo,
				debit_account,
				credit_account ) ) )
	{
		fprintf( output_pipe,
		"<h3>Error: an unknown error occurred. Check log.</h3>" );

		return (TRANSACTION *)0;
	}

	transaction_journal_list_pipe_display(
		output_pipe,
		transaction->full_name,
		transaction->street_address,
		transaction->transaction_date_time,
		transaction->memo,
		transaction->journal_list );

	return transaction;
}

