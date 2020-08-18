/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/post_reoccurring_transaction.c	*/
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
#include "boolean.h"
#include "bank_upload.h"
#include "transaction.h"
#include "reoccurring.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_reoccurring_transaction_batch(
			FILE *output_pipe,
			char *transaction_date_time,
			boolean execute );

void post_reoccurring_transaction_entity(
			FILE *output_pipe,
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			char *memo,
			boolean execute );

char *reoccurring_last_transaction_date(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account );

TRANSACTION *post_reoccurring_get_accrued_monthly_transaction(
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account,
			double accrued_monthly_amount );

TRANSACTION *post_reoccurring_get_accrued_daily_transaction(
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account,
			double accrued_daily_amount );

/*
TRANSACTION *post_reoccurring_get_recent_transaction(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account,
			double transaction_amount,
			char *memo );
*/

char *post_reoccurring_transaction(
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			char *memo );

void post_reoccurring_transaction_display(
			FILE *output_pipe,
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time );

/* Global variables */
/* ---------------- */
enum bank_upload_exception bank_upload_exception = {0};

int main( int argc, char **argv )
{
	char *application_name;
	char *session;
	char *role_name;
	char *process_name;
	char *full_name;
	char *street_address;
	char *transaction_description;
	char *transaction_date;
	char *transaction_date_time;
	char *memo;
	boolean execute;
	boolean with_html;
	char title[ 128 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *heading = {0};
	char *justify = {0};
	char sys_string[ 512 ];
	FILE *output_pipe = {0};

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 12 )
	{
		fprintf( stderr,
"Usage: %s session role process full_name street_address transaction_description transaction_date ignored memo execute_yn with_html_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	session = argv[ 1 ];
	role_name = argv[ 2 ];
	process_name = argv[ 3 ];
	full_name = argv[ 4 ];
	street_address = argv[ 5 ];
	transaction_description = argv[ 6 ];
	transaction_date = argv[ 7 ];
	/* transaction_amount = atof( argv[ 8 ] ); */
	memo = argv[ 9 ];
	execute = (*argv[ 10 ] == 'y');
	with_html = (*argv[ 11 ] == 'y');

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( with_html )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf( "<h1>%s</h1>\n", 
			format_initial_capital(
				title,
				process_name ) );
		fflush( stdout );
	}

	if ( !*transaction_date
	||   strcmp( transaction_date, "transction_date" ) == 0 )
	{
		transaction_date =
			date_get_now_yyyy_mm_dd(
				date_get_utc_offset() );
	}

	transaction_date_time =
		transaction_generate_date_time(
			transaction_date );

	if ( with_html )
	{
		heading = "Transaction,Account,Debit,Credit";
		justify = "left,left,right,right";

		sprintf(sys_string,
			"html_table.e '' %s '^' %s",
			heading,
			justify );
	}
	else
	{
		strcpy( sys_string, "cat" );
	}

	output_pipe = popen( sys_string, "w" );

	/* --------------------- */
	/* If doing for in batch */
	/* --------------------- */
	if ( !atoi( session ) )
	{
		post_reoccurring_transaction_batch(
				output_pipe,
				transaction_date_time,
				execute );
	}
	else
	/* ---------------------------- */
	/* If doing for a single entity */
	/* ---------------------------- */
	if ( strcmp( full_name, "full_name" ) != 0 )
	{
		post_reoccurring_transaction_entity(
				output_pipe,
				full_name,
				street_address,
				transaction_description,
				transaction_date_time,
				memo,
				execute );
	}

	if ( output_pipe ) pclose( output_pipe );

	if ( execute )
	{
		folder_menu_refresh_row_count(
			application_name,
			TRANSACTION_FOLDER_NAME,
			session,
			appaserver_parameter_file->
				appaserver_data_directory,
			role_name );

		folder_menu_refresh_row_count(
			application_name,
			JOURNAL_FOLDER_NAME,
			session,
			appaserver_parameter_file->
				appaserver_data_directory,
			role_name );

		if ( with_html )
		{
			printf( "<h3>Process complete.</h3>\n" );
		}
	}

	exit( 0 );

} /* main() */

void post_reoccurring_transaction_batch(
				FILE *output_pipe,
				char *transaction_date_time,
				boolean execute )
{
	FILE *input_pipe;
	char input_buffer[ 512 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char transaction_description[ 128 ];

	input_pipe =
		popen( "populate_reoccurring_transaction_accrual.sh", "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( character_count(
			FOLDER_DATA_DELIMITER,
			input_buffer ) != 2 )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: not two delimiters in (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );
			pclose( input_pipe );
			exit( 1 );
		}

		piece( full_name, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		piece( street_address, FOLDER_DATA_DELIMITER, input_buffer, 1 );

		piece(	transaction_description,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		post_reoccurring_transaction_entity(
				output_pipe,
				full_name,
				street_address,
				transaction_description,
				transaction_date_time,
				(char *)0 /* memo */,
				execute );
	}
}

void post_reoccurring_transaction_entity(
				FILE *output_pipe,
				char *full_name,
				char *street_address,
				char *transaction_description,
				char *transaction_date_time,
				char *memo,
				boolean execute )
{
	if ( !execute )
	{
		post_reoccurring_transaction_display(
			output_pipe,
			full_name,
			street_address,
			transaction_description,
			transaction_date_time );
	}
	else
	{
		transaction_date_time =
			post_reoccurring_transaction(
				full_name,
				street_address,
				transaction_description,
				transaction_date_time,
				memo );
	}
}

void post_reoccurring_transaction_display(
			FILE *output_pipe,
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time )
{
	REOCCURRING_TRANSACTION *reoccurring_transaction;
	TRANSACTION *transaction = {0};

	if ( ! ( reoccurring_transaction =
			reoccurring_transaction_fetch(
					full_name,
					street_address,
					transaction_description ) ) )
	{
		printf(
		"<h3>Warning: no valid reoccurring transaction found.</h3>\n" );
		return;
	}

	if ( reoccurring_transaction->accrued_daily_amount )
	{
		transaction =
			post_reoccurring_get_accrued_daily_transaction(
				reoccurring_transaction->full_name,
				reoccurring_transaction->street_address,
				reoccurring_transaction->
					transaction_description,
				transaction_date_time,
				reoccurring_transaction->debit_account,
				reoccurring_transaction->credit_account,
				reoccurring_transaction->accrued_daily_amount );
	}
	else
	if ( reoccurring_transaction->accrued_monthly_amount )
	{
		transaction =
			post_reoccurring_get_accrued_monthly_transaction(
				reoccurring_transaction->full_name,
				reoccurring_transaction->street_address,
				reoccurring_transaction->
					transaction_description,
				transaction_date_time,
				reoccurring_transaction->debit_account,
				reoccurring_transaction->credit_account,
				reoccurring_transaction->
					accrued_monthly_amount );
	}
	else
	{
/*
		transaction =
			post_reoccurring_get_recent_transaction(
				reoccurring_transaction->full_name,
				reoccurring_transaction->street_address,
				transaction_date_time,
				reoccurring_transaction->debit_account,
				reoccurring_transaction->credit_account,
				reoccurring_transaction->transaction_amount,
				memo );
*/
	}

	if ( !transaction ) return;

	transaction_journal_list_pipe_display(
		output_pipe,
		transaction->full_name,
		transaction->street_address,
		transaction->transaction_date_time,
		transaction->journal_list );
}

/* Returns transaction_date_time */
/* ----------------------------- */
char *post_reoccurring_transaction(
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			char *memo )
{
	TRANSACTION *transaction = {0};
	REOCCURRING_TRANSACTION *reoccurring_transaction;

	if ( ! ( reoccurring_transaction =
			reoccurring_transaction_fetch(
					full_name,
					street_address,
					transaction_description ) ) )
	{
		return (char *)0;
	}

	if ( reoccurring_transaction->accrued_daily_amount )
	{
		transaction =
			post_reoccurring_get_accrued_daily_transaction(
				reoccurring_transaction->full_name,
				reoccurring_transaction->street_address,
				reoccurring_transaction->
					transaction_description,
				transaction_date_time,
				reoccurring_transaction->debit_account,
				reoccurring_transaction->credit_account,
				reoccurring_transaction->accrued_daily_amount );
	}
	else
	if ( reoccurring_transaction->accrued_monthly_amount )
	{
		transaction =
			post_reoccurring_get_accrued_monthly_transaction(
				reoccurring_transaction->full_name,
				reoccurring_transaction->street_address,
				reoccurring_transaction->
					transaction_description,
				transaction_date_time,
				reoccurring_transaction->debit_account,
				reoccurring_transaction->credit_account,
				reoccurring_transaction->
					accrued_monthly_amount );
	}
	else
	{
/*
		transaction =
			post_reoccurring_get_recent_transaction(
				reoccurring_transaction->full_name,
				reoccurring_transaction->street_address,
				transaction_date_time,
				reoccurring_transaction->debit_account,
				reoccurring_transaction->credit_account,
				reoccurring_transaction->transaction_amount,
				memo );
*/
	}

	if ( !transaction ) return (char *)0;

	transaction->memo = memo;

	transaction->transaction_date_time =
		transaction_journal_insert(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			transaction->transaction_amount,
			transaction->memo,
			0 /* check_number */,
			0 /* not lock_transaction */,
			transaction->journal_list );

	return transaction->transaction_date_time;
}

TRANSACTION *post_reoccurring_get_recent_transaction(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account,
			double transaction_amount,
			char *memo )
{
	TRANSACTION *transaction;
	JOURNAL *journal;

	transaction =
		transaction_new(
			full_name,
			street_address,
			transaction_date_time );

	transaction->memo = memo;

	transaction->transaction_amount = transaction_amount;
	transaction->journal_list = list_new();

	journal =
		journal_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			debit_account );

	journal->debit_amount = transaction->transaction_amount;

	list_set( transaction->journal_list, journal );

	journal =
		journal_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			credit_account );

	journal->credit_amount = transaction->transaction_amount;

	list_set( transaction->journal_list, journal );

	return transaction;
}

TRANSACTION *post_reoccurring_get_accrued_daily_transaction(
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account,
			double accrued_daily_amount )
{
	TRANSACTION *transaction;
	JOURNAL *journal;
	int days_between;
	double accrued_amount;
	char *memo;

	if ( ! ( days_between =
			reoccurring_days_between_last_transaction(
				full_name,
				street_address,
				transaction_date_time,
				debit_account,
				credit_account ) ) )
	{
		return (TRANSACTION *)0;
	}

	if ( days_between < 0.0 ) return (TRANSACTION *)0;

	accrued_amount = (double)days_between * accrued_daily_amount;

	if ( timlib_dollar_virtually_same( accrued_amount, 0.0 ) )
	{
		return (TRANSACTION *)0;
	}

	memo =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		reoccurring_memo(
			transaction_description,
			credit_account );

	transaction =
		transaction_new(
			full_name,
			street_address,
			transaction_date_time );

	transaction->memo = strdup( memo );

	transaction->transaction_amount = accrued_amount;

	transaction->journal_list = list_new();

	journal =
		journal_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			debit_account );

	journal->debit_amount = transaction->transaction_amount;

	list_set( transaction->journal_list, journal );

	journal =
		journal_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			credit_account );

	journal->credit_amount = transaction->transaction_amount;

	list_set( transaction->journal_list, journal );

	return transaction;
}

TRANSACTION *post_reoccurring_get_accrued_monthly_transaction(
			char *full_name,
			char *street_address,
			char *transaction_description,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account,
			double accrued_monthly_amount )
{
	TRANSACTION *transaction;
	JOURNAL *journal;
	double accrued_amount;
	char *begin_date_string;
	char end_date_string[ 16 ];
	char *memo;

	begin_date_string =
			reoccurring_last_transaction_date(
				full_name,
				street_address,
				transaction_date_time,
				debit_account,
				credit_account );

	column( end_date_string, 0, transaction_date_time );

	accrued_amount =
		transaction_monthly_accrue(
			begin_date_string,
			end_date_string,
			accrued_monthly_amount
				/* monthly_accrual */ );

	if ( timlib_dollar_virtually_same( accrued_amount, 0.0 ) )
	{
		return (TRANSACTION *)0;
	}

	memo =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		reoccurring_memo(
			transaction_description,
			credit_account );

	transaction =
		transaction_new(
			full_name,
			street_address,
			transaction_date_time );

	transaction->memo = strdup( memo );

	transaction->transaction_amount = accrued_amount;

	transaction->journal_list = list_new();

	journal =
		journal_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			debit_account );

	journal->debit_amount = transaction->transaction_amount;

	list_set( transaction->journal_list, journal );

	journal =
		journal_new(
			full_name,
			street_address,
			transaction->transaction_date_time,
			credit_account );

	journal->credit_amount = transaction->transaction_amount;

	list_set( transaction->journal_list, journal );

	return transaction;
}

char *reoccurring_last_transaction_date(
			char *full_name,
			char *street_address,
			char *transaction_date_time,
			char *debit_account,
			char *credit_account )
{
	char sys_string[ 2048 ];
	int current_year;
	char where[ 1024 ];
	char sub_query[ 1024 ];
	char name_buffer[ 256 ];
	char *select;
	char *folder;
	char *last_transaction_date_time;
	char last_transaction_date[ 16 ];

	if ( !transaction_date_time
	||   ! ( current_year = atoi( transaction_date_time ) ) )
	{
		return 0;
	}

	select = "max( transaction_date_time )";
	folder = "transaction";

	reoccurring_transaction_subquery(
		sub_query,
		debit_account,
		credit_account );

	sprintf( where,
		 "full_name = '%s' and				"
		 "street_address = '%s' and			"
		 "%s						",
		 escape_character(	name_buffer,
					full_name,
					'\'' ),
		 street_address,
		 sub_query );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\"			|"
		 "sql							|"
		 "column.e 0						 ",
		 select,
		 folder,
		 where );

	last_transaction_date_time = pipe2string( sys_string );

	if ( !timlib_strlen( last_transaction_date_time ) )
		return (char *)0;

	column( last_transaction_date, 0, last_transaction_date_time );

	return strdup( last_transaction_date );
}

