/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/close_nominal_accounts.c		*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "transaction.h"
#include "close_nominal.h"

void close_nominal_accounts(
			CLOSE_NOMINAL_DO *close_nominal_do,
			boolean execute );

void close_nominal_accounts_undo(
			CLOSE_NOMINAL_UNDO *close_nominal_undo,
			boolean execute );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *as_of_date;
	boolean undo;
	boolean execute;
	CLOSE_NOMINAL *close_nominal;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf(stderr,
			"Usage: %s process as_of_date undo execute_yn\n",
			argv[ 0 ] );

		exit ( 1 );
	}

	process_name = argv[ 1 ];
	as_of_date = argv[ 2 ];
	undo = (*argv[ 3 ]) == 'y';
	execute = (*argv[ 4 ] == 'y');

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

	close_nominal =
		close_nominal_fetch(
			application_name,
			process_name,
			as_of_date,
			undo );

	if ( !close_nominal )
	{
		printf( "<h3>Nothing to display.</h3>\n" );
		document_close();
		exit( 0 );
	}

	printf( "%s\n",
		close_nominal->
			statement_caption->
			frame_title );

	if ( close_nominal->close_nominal_do )
	{
		close_nominal_accounts(
			close_nominal->close_nominal_do,
			execute );
	}
	else
	if ( close_nominal->close_nominal_undo )
	{
		close_nominal_accounts_undo(
			close_nominal->close_nominal_undo,
			execute );
	}
	else
	{
		printf( "<h3>An error occurred. Check log.</h3>\n" );
	}

	document_close();

	return 0;
}

void close_nominal_accounts(
			CLOSE_NOMINAL_DO *close_nominal_do,
			boolean execute )
{
	if ( !close_nominal_do )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: close_nominal_do is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( close_nominal_do->transaction_date_time_exists )
	{
		printf( "%s\n",
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			close_nominal_do_transaction_exists_message() );
	}
	else
	if ( execute )
	{
		if ( !close_nominal_do->close_nominal_transaction
		||   !close_nominal_do->close_nominal_transaction->transaction )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: transaction is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		transaction_insert(
			close_nominal_do->
				close_nominal_transaction->
				transaction->
				full_name,
			close_nominal_do->
				close_nominal_transaction->
				transaction->
				street_address,
			close_nominal_do->
				close_nominal_transaction->
				transaction->
				transaction_date_time,
			close_nominal_do->
				close_nominal_transaction->
				transaction->
				transaction_amount,
			close_nominal_do->
				close_nominal_transaction->
				transaction->
				check_number,
			close_nominal_do->
				close_nominal_transaction->
				transaction->
				memo,
			TRANSACTION_LOCK_Y,
			close_nominal_do->
				close_nominal_transaction->
				transaction->
				journal_list,
			1 /* insert_journal_list_boolean */ );

		printf( "<h3>Process complete.</h3>\n" );
	}
	else
	{
		journal_list_sum_html_display(
			close_nominal_do->
				close_nominal_transaction->
				transaction->
				journal_list,
			close_nominal_do->
				close_nominal_transaction->
				transaction->
				transaction_date_time,
			close_nominal_do->
				close_nominal_transaction->
				transaction->
				memo,
			close_nominal_do->
				journal_debit_sum,
			close_nominal_do->
				journal_credit_sum );
	}
}

void close_nominal_accounts_undo(
			CLOSE_NOMINAL_UNDO *close_nominal_undo,
			boolean execute )
{
	if ( !close_nominal_undo )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: close_nominal_undo is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !close_nominal_undo->transaction )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: transaction is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !execute )
	{
		close_nominal_undo_display(
			close_nominal_undo->transaction );
	}
	else
	{
		close_nominal_undo_execute(
			close_nominal_undo->transaction );
	}
}

