/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/reverse_nominal_accounts.c		*/
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
#include "reverse_nominal.h"

void reverse_nominal_accounts_do(
		REVERSE_NOMINAL_DO *reverse_nominal_do,
		boolean execute );

void reverse_nominal_accounts_undo(
		CLOSE_NOMINAL_UNDO *close_nominal_undo,
		boolean execute );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *reverse_date_string;
	boolean undo;
	boolean execute;
	REVERSE_NOMINAL *reverse_nominal;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf(stderr,
		"Usage: %s process reverse_date_string undo execute_yn\n",
			argv[ 0 ] );

		exit ( 1 );
	}

	process_name = argv[ 1 ];
	reverse_date_string = argv[ 2 ];
	undo = (*argv[ 3 ]) == 'y';
	execute = (*argv[ 4 ] == 'y');

	reverse_nominal =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		reverse_nominal_fetch(
			application_name,
			process_name,
			reverse_date_string,
			undo );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		(char *)0 /* title */ );

	printf( "%s\n",
		reverse_nominal->
			statement_caption->
			frame_title );

	if ( reverse_nominal->undo_no_transaction_message )
	{
		printf( "%s\n",
			reverse_nominal->
				undo_no_transaction_message );
	}
	else
	if ( reverse_nominal->do_no_transaction_message )
	{
		printf( "%s\n",
			reverse_nominal->
				do_no_transaction_message );
	}
	else
	if ( reverse_nominal->do_empty_date_message )
	{
		printf( "%s\n",
			reverse_nominal->
				do_empty_date_message );
	}
	else
	if ( reverse_nominal->do_transaction_exists_message )
	{
		printf( "%s\n",
			reverse_nominal->
				do_transaction_exists_message );
	}
	else
	if ( reverse_nominal->reverse_nominal_do )
	{
		reverse_nominal_accounts_do(
			reverse_nominal->reverse_nominal_do,
			execute );

		if ( execute )
		{
			printf( "%s\n",
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				reverse_nominal_do_execute_message(
				    reverse_nominal->
				      reverse_nominal_do->
				      transaction_date_reverse_nominal_do->
				      transaction_date_reverse_date_time ) );
		}
		else
		{
			printf( "%s\n",
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				reverse_nominal_do_no_execute_message() );
		}
	}
	else
	if ( reverse_nominal->close_nominal_undo )
	{
		if ( !execute )
		{
			printf( "%s\n",
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				close_nominal_undo_no_execute_message() );
		}

		reverse_nominal_accounts_undo(
			reverse_nominal->close_nominal_undo,
			execute );

		if ( execute )
		{
			char *transaction_date_time = {0};

			if ( reverse_nominal->
				close_nominal_undo->
				transaction )
			{
				transaction_date_time =
					reverse_nominal->
						close_nominal_undo->
						transaction->
						transaction_date_time;
			}

			printf( "%s\n",
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				reverse_nominal_undo_execute_message(
					transaction_date_time ) );
		}
	}

	document_close();

	return 0;
}

void reverse_nominal_accounts_do(
		REVERSE_NOMINAL_DO *reverse_nominal_do,
		boolean execute )
{
	if ( !reverse_nominal_do )
	{
		char message[ 128 ];

		sprintf(message, "reverse_nominal_do is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}
	else
	if ( execute )
	{
		if ( !reverse_nominal_do->reverse_transaction
		||   !reverse_nominal_do->reverse_transaction->transaction )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"reverse_transaction is empty or incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		transaction_insert(
			reverse_nominal_do->
				reverse_transaction->
				transaction->
				full_name,
			reverse_nominal_do->
				reverse_transaction->
				transaction->
				street_address,
			reverse_nominal_do->
				reverse_transaction->
				transaction->
				transaction_date_time,
			reverse_nominal_do->
				reverse_transaction->
				transaction->
				transaction_amount,
			0 /* check_number */,
			reverse_nominal_do->
				reverse_transaction->
				transaction->
				memo,
			'n' /* transaction_lock_yn */,
			reverse_nominal_do->
				reverse_transaction->
				transaction->
				journal_list,
			1 /* insert_journal_list_boolean */ );
	}
	else
	{
		journal_list_sum_html_display(
			reverse_nominal_do->
				reverse_transaction->
				transaction->
				journal_list,
			reverse_nominal_do->
				reverse_transaction->
				transaction->
				transaction_date_time,
			reverse_nominal_do->
				reverse_transaction->
				transaction->
				memo,
			reverse_nominal_do->
				journal_debit_sum,
			reverse_nominal_do->
				journal_credit_sum );
	}
}

void reverse_nominal_accounts_undo(
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

		snprintf(
			message,
			sizeof ( message ),
			"close_nominal_undo->transaction is empty." );

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

