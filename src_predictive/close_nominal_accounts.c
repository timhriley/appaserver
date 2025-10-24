/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/close_nominal_accounts.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "environ.h"
#include "appaserver_error.h"
#include "document.h"
#include "transaction.h"
#include "close_account.h"
#include "close_nominal.h"

void close_nominal_accounts_do(
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

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
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

	close_nominal =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		close_nominal_fetch(
			application_name,
			process_name,
			as_of_date,
			undo );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		(char *)0 /* title */ );

	printf( "%s\n",
		close_nominal->
			statement_caption->
			frame_title );

	if ( close_nominal->undo_no_transaction_message )
	{
		printf( "%s\n",
			close_nominal->
				undo_no_transaction_message );
	}
	else
	if ( close_nominal->do_no_transaction_message )
	{
		printf( "%s\n",
			close_nominal->
				do_no_transaction_message );
	}
	else
	if ( close_nominal->do_empty_date_message )
	{
		printf( "%s\n",
			close_nominal->
				do_empty_date_message );
	}
	else
	if ( close_nominal->do_transaction_exists_message )
	{
		printf( "%s\n",
			close_nominal->
				do_transaction_exists_message );
	}
	else
	if ( close_nominal->close_nominal_do )
	{
		close_nominal_accounts_do(
			close_nominal->close_nominal_do,
			execute );

		if ( execute )
		{
			printf( "%s\n",
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				close_nominal_do_execute_message(
				    close_nominal->
				      close_nominal_do->
				      transaction_date_close_nominal_do->
				      transaction_date_close_date_time ) );
		}
		else
		{
			printf( "%s\n",
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				close_nominal_do_no_execute_message() );
		}
	}
	else
	if ( close_nominal->close_nominal_undo )
	{
		if ( !execute )
		{
			printf( "%s\n",
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				close_nominal_undo_no_execute_message() );
		}

		close_nominal_accounts_undo(
			close_nominal->close_nominal_undo,
			execute );

		if ( execute )
		{
			char *transaction_date_time = {0};

			if ( close_nominal->
				close_nominal_undo->
				transaction )
			{
				transaction_date_time =
					close_nominal->
						close_nominal_undo->
						transaction->
						transaction_date_time;
			}

			printf( "%s\n",
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				close_nominal_undo_execute_message(
					transaction_date_time ) );
		}
	}

	document_close();

	return 0;
}

void close_nominal_accounts_do(
		CLOSE_NOMINAL_DO *close_nominal_do,
		boolean execute )
{
	if ( !close_nominal_do )
	{
		char message[ 128 ];

		sprintf(message, "close_nominal_do is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}
	else
	if ( execute )
	{
		if ( !close_nominal_do->close_transaction
		||   !close_nominal_do->close_transaction->transaction )
		{
			char message[ 128 ];

			sprintf(message,
			"close_transaction is empty or incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		transaction_insert(
			close_nominal_do->
				close_transaction->
				transaction->
				fund_name,
			close_nominal_do->
				close_transaction->
				transaction->
				full_name,
			close_nominal_do->
				close_transaction->
				transaction->
				street_address,
			close_nominal_do->
				close_transaction->
				transaction->
				transaction_date_time,
			close_nominal_do->
				close_transaction->
				transaction->
				transaction_amount,
			0 /* check_number */,
			close_nominal_do->
				close_transaction->
				transaction->
				memo,
			close_nominal_do->
				close_transaction->
				transaction->
				journal_list,
			1 /* insert_journal_list_boolean */ );
	}
	else
	{
		journal_list_sum_html_display(
			close_nominal_do->
				close_transaction->
				transaction->
				journal_list,
			close_nominal_do->
				close_transaction->
				transaction->
				transaction_date_time,
			close_nominal_do->
				close_transaction->
				transaction->
				memo,
			close_nominal_do->
				journal_debit_sum,
			close_nominal_do->
				journal_credit_sum );

		if ( list_length(
			close_nominal_do->
				close_transaction->
				close_equity_list ) )
		{
			close_account_list_display(
				close_nominal_do->
					close_transaction->
					close_account_list );
		}
	}
}

void close_nominal_accounts_undo(
		CLOSE_NOMINAL_UNDO *close_nominal_undo,
		boolean execute )
{
	if ( !close_nominal_undo )
	{
		char message[ 128 ];

		sprintf(message, "close_nominal_undo is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !close_nominal_undo->transaction )
	{
		char message[ 128 ];

		sprintf(message, "close_nominal_undo->transaction is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( execute )
	{
		close_nominal_undo_execute(
			close_nominal_undo->transaction );
	}
	else
	{
		close_nominal_undo_display(
			close_nominal_undo->transaction );
	}
}

