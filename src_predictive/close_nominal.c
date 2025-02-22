/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/close_nominal.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "appaserver_error.h"
#include "account.h"
#include "close_nominal.h"

CLOSE_NOMINAL_DO *close_nominal_do_calloc( void )
{
	CLOSE_NOMINAL_DO *close_nominal_do;

	if ( ! ( close_nominal_do = calloc( 1, sizeof ( CLOSE_NOMINAL_DO ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return close_nominal_do;
}

CLOSE_NOMINAL_DO *close_nominal_do_fetch( char *as_of_date_string )
{
	CLOSE_NOMINAL_DO *close_nominal_do;

	if ( !as_of_date_string )
	{
		char message[ 128 ];

		sprintf(message, "as_of_date_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	close_nominal_do = close_nominal_do_calloc();

	close_nominal_do->
		transaction_date_close_nominal_do =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			transaction_date_close_nominal_do_new(
				as_of_date_string );

	if ( close_nominal_do->
		transaction_date_close_nominal_do->
		transaction_date_close_boolean )
	{
		return close_nominal_do;
	}

	close_nominal_do->element_name_list =
		close_nominal_do_element_name_list(
			ELEMENT_REVENUE,
			ELEMENT_EXPENSE,
			ELEMENT_GAIN,
			ELEMENT_LOSS );

	close_nominal_do->element_statement_list =
		element_statement_list(
			close_nominal_do->element_name_list,
			close_nominal_do->
				transaction_date_close_nominal_do->
				transaction_date_close_date_time,
			1 /* fetch_subclassification_list */,
			1 /* fetch_account_list */,
			1 /* fetch_journal_latest */,
			0 /* not fetch_transaction */ );

	if ( !list_length( close_nominal_do->element_statement_list ) )
	{
		char message[ 128 ];

		sprintf(message,
			"element_statement_list(%s) returned empty.",
			close_nominal_do->
				transaction_date_close_nominal_do->
				transaction_date_close_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	element_list_sum_set( close_nominal_do->element_statement_list );

	close_nominal_do->revenue_element =
		element_seek(
			ELEMENT_REVENUE,
			close_nominal_do->element_statement_list );

	close_nominal_do->revenue_sum =
		element_sum(
			close_nominal_do->revenue_element );

	close_nominal_do->gain_element =
		element_seek(
			ELEMENT_GAIN,
			close_nominal_do->element_statement_list );

	close_nominal_do->gain_sum =
		element_sum(
			close_nominal_do->gain_element );

	close_nominal_do->account_drawing =
		account_drawing(
			ACCOUNT_DRAWING_KEY );

	close_nominal_do->drawing_sum =
		close_nominal_do_drawing_sum(
			close_nominal_do->
				transaction_date_close_nominal_do->
				transaction_date_close_date_time,
			close_nominal_do->account_drawing );

	close_nominal_do->debit_sum =
		close_nominal_do_debit_sum(
			close_nominal_do->revenue_sum,
			close_nominal_do->gain_sum,
			close_nominal_do->drawing_sum );

	close_nominal_do->expense_element =
		element_seek(
			ELEMENT_EXPENSE,
			close_nominal_do->element_statement_list );

	close_nominal_do->expense_sum =
		element_sum(
			close_nominal_do->expense_element );

	close_nominal_do->loss_element =
		element_seek(
			ELEMENT_LOSS,
			close_nominal_do->element_statement_list );

	close_nominal_do->loss_sum =
		element_sum(
			close_nominal_do->loss_element );

	close_nominal_do->credit_sum =
		close_nominal_do_credit_sum(
			close_nominal_do->expense_sum,
			close_nominal_do->loss_sum );

	close_nominal_do->transaction_amount =
		close_nominal_do_transaction_amount(
			close_nominal_do->debit_sum,
			close_nominal_do->credit_sum );

	close_nominal_do->no_transactions_boolean =
		close_nominal_do_no_transactions_boolean(
			close_nominal_do->transaction_amount );

	if ( close_nominal_do->no_transactions_boolean )
	{
		return close_nominal_do;
	}

	close_nominal_do->retained_earnings =
		close_nominal_do_retained_earnings(
			close_nominal_do->revenue_sum,
			close_nominal_do->gain_sum,
			close_nominal_do->expense_sum,
			close_nominal_do->loss_sum,
			close_nominal_do->drawing_sum );

	close_nominal_do->account_closing_entry =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		account_closing_entry(
			__FUNCTION__,
			ACCOUNT_CLOSING_KEY,
			ACCOUNT_EQUITY_KEY );

	close_nominal_do->entity_self =
		entity_self_fetch(
			0 /* not fetch_entity_boolean */ );

	if ( !close_nominal_do->entity_self )
	{
		char message[ 128 ];

		sprintf(message,
			"entity_self_fetch() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	close_nominal_do->close_nominal_transaction =
		close_nominal_transaction_new(
			close_nominal_do->element_statement_list /* in/out */,
			close_nominal_do->
				transaction_date_close_nominal_do->
				transaction_date_close_date_time,
			close_nominal_do->account_drawing,
			close_nominal_do->drawing_sum,
			close_nominal_do->transaction_amount,
			close_nominal_do->retained_earnings,
			close_nominal_do->account_closing_entry,
			close_nominal_do->entity_self->full_name,
			close_nominal_do->entity_self->street_address );

	if ( !close_nominal_do->close_nominal_transaction
	||   !close_nominal_do->close_nominal_transaction->transaction )
	{
		char message[ 128 ];

		sprintf(message,
	"close_nominal_transaction_new(%s) returned empty or incomplete.",
			close_nominal_do->
				transaction_date_close_nominal_do->
				transaction_date_close_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	close_nominal_do->journal_debit_sum =
		journal_debit_sum(
			close_nominal_do->
				close_nominal_transaction->
				transaction->
				journal_list );

	close_nominal_do->journal_credit_sum =
		journal_credit_sum(
			close_nominal_do->
				close_nominal_transaction->
				transaction->
				journal_list );

	return close_nominal_do;
}

LIST *close_nominal_do_element_name_list(
		char *element_revenue,
		char *element_expense,
		char *element_gain,
		char *element_loss )
{
	LIST *list = list_new();

	list_set( list, element_revenue );
	list_set( list, element_expense );
	list_set( list, element_gain );
	list_set( list, element_loss );

	return list;
}

double close_nominal_do_drawing_sum(
		char *transaction_date_time_closing,
		char *account_drawing )
{
	JOURNAL *latest;

	if ( !transaction_date_time_closing )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !account_drawing ) return 0.0;

	latest =
		journal_latest(
			JOURNAL_TABLE,
			account_drawing /* account_name */,
			transaction_date_time_closing,
			0 /* not fetch_transaction */,
			0 /* not zero_balance_boolean */ );

	if ( latest )
		return latest->balance;
	else
		return 0.0;
}

double close_nominal_do_debit_sum(
		double revenue_sum,
		double gain_sum,
		double drawing_sum )
{
	return revenue_sum + gain_sum + drawing_sum;
}

double close_nominal_do_credit_sum(
		double expense_sum,
		double loss_sum )
{
	return expense_sum + loss_sum;
}

double close_nominal_do_retained_earnings(
		double revenue_sum,
		double gain_sum,
		double expense_sum,
		double loss_sum,
		double drawing_sum )
{
	return
	(revenue_sum + gain_sum) -
	(expense_sum + loss_sum) +
	drawing_sum;
}

CLOSE_NOMINAL_TRANSACTION *
	close_nominal_transaction_new(
		LIST *element_statement_list /* in/out */,
		char *transaction_date_close_date_time,
		char *account_drawing,
		double close_nominal_do_drawing_sum,
		double close_nominal_do_transaction_amount,
		double close_nominal_do_retained_earnings,
		char *account_closing_entry,
		char *full_name,
		char *street_address )
{
	CLOSE_NOMINAL_TRANSACTION *close_nominal_transaction;

	if ( !list_length( element_statement_list )
	||   !transaction_date_close_date_time
	||   float_virtually_same(
		close_nominal_do_transaction_amount,
		0.0 )
	||   !account_closing_entry
	||   !full_name
	||   !street_address )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	close_nominal_transaction = close_nominal_transaction_calloc();

	close_nominal_transaction->transaction =
		transaction_new(
			full_name,
			street_address,
			transaction_date_close_date_time );

	close_nominal_transaction->transaction->transaction_amount =
		close_nominal_do_transaction_amount;

	close_nominal_transaction->transaction->memo =
		TRANSACTION_CLOSE_MEMO;

	close_nominal_transaction->transaction->journal_list =
		close_nominal_transaction_journal_list(
			element_statement_list /* in/out */,
			transaction_date_close_date_time,
			account_drawing,
			close_nominal_do_drawing_sum,
			close_nominal_do_retained_earnings,
			account_closing_entry,
			full_name,
			street_address );

	return close_nominal_transaction;
}

CLOSE_NOMINAL_TRANSACTION *close_nominal_transaction_calloc(
		void )
{
	CLOSE_NOMINAL_TRANSACTION *close_nominal_transaction;

	if ( ! ( close_nominal_transaction =
			calloc( 1, sizeof ( CLOSE_NOMINAL_TRANSACTION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return close_nominal_transaction;
}

LIST *close_nominal_transaction_journal_list(
		LIST *element_statement_list /* in/out */,
		char *transaction_date_close_date_time,
		char *account_drawing,
		double drawing_sum,
		double retained_earnings,
		char *account_closing_entry,
		char *full_name,
		char *street_address )
{
	LIST *journal_list;
	ELEMENT *element;
	ACCOUNT *account;
	JOURNAL *journal;

	if ( !list_length( element_statement_list ) /* can't be list_rewind() */
	||   !transaction_date_close_date_time
	||   !account_closing_entry
	||   !full_name
       	||   !street_address )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	element_list_account_statement_list_set(
		element_statement_list );

	journal_list = list_new();

	list_rewind( element_statement_list );

	do {
		element =
			list_get(
				element_statement_list );

		if ( float_virtually_same( element->sum, 0.0 ) ) continue;

		if ( list_rewind( element->account_statement_list ) )
		do {
			account =
				list_get(
					element->account_statement_list );

			if ( !account->account_journal_latest )
			{
				char message[ 128 ];

				sprintf(message,
				"account->account_journal_latest is empty." );

				appaserver_error_stderr_exit(
					__FILE__,
					__FUNCTION__,
					__LINE__,
					message );
			}

			if ( float_virtually_same(
				account->
					account_journal_latest->
					balance,
				0.0 ) )
			{
				continue;
			}

			journal =
				journal_new(
					full_name,
					street_address,
					transaction_date_close_date_time,
					account->account_name );

			journal->account =
				account_fetch(
					journal->account_name,
					1 /* fetch_subclassification */,
					1 /* fetch_element */ );

			journal->debit_amount =
				close_nominal_transaction_debit_amount(
					element->accumulate_debit,
					account->
						account_journal_latest->
						balance );

			journal->credit_amount =
				close_nominal_transaction_credit_amount(
					element->accumulate_debit,
					account->
						account_journal_latest->
						balance );

			list_set(
				journal_list,
				journal );

		} while ( list_next( element->account_statement_list ) );

	} while ( list_next( element_statement_list ) );

	if ( !float_virtually_same( drawing_sum, 0.0 ) )
	{
		list_set(
			journal_list,
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			close_nominal_transaction_drawing_journal(
				transaction_date_close_date_time,
				account_drawing,
				drawing_sum,
				full_name,
				street_address ) );
	}

	if ( !float_virtually_same( retained_earnings, 0.0 ) )
	{
		list_set(
			journal_list,
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			close_nominal_transaction_close_journal(
				transaction_date_close_date_time,
				retained_earnings,
				account_closing_entry,
				full_name,
				street_address ) );
	}

	return journal_list;
}

JOURNAL *close_nominal_transaction_drawing_journal(
		char *transaction_date_time_closing,
		char *account_drawing,
		double drawing_sum,
		char *full_name,
		char *street_address )
{
	JOURNAL *journal;

	if ( !transaction_date_time_closing
	||   !account_drawing
	||   !drawing_sum
	||   !full_name
	||   !street_address )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	journal =
		journal_new(
			full_name,
			street_address,
			transaction_date_time_closing,
			account_drawing );

	journal->account =
		account_fetch(
			journal->account_name,
			1 /* fetch_subclassification */,
			1 /* fetch_element */ );

	journal->debit_amount = drawing_sum;

	return journal;
}

JOURNAL *close_nominal_transaction_close_journal(
		char *transaction_date_time_closing,
		double retained_earnings,
		char *account_closing_entry,
		char *full_name,
		char *street_address )
{
	JOURNAL *journal;

	if ( !transaction_date_time_closing
	||   !retained_earnings
	||   !account_closing_entry
	||   !full_name
	||   !street_address )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	journal =
		journal_new(
			full_name,
			street_address,
			transaction_date_time_closing,
			account_closing_entry );

	journal->account =
		account_fetch(
			journal->account_name,
			1 /* fetch_subclassification */,
			1 /* fetch_element */ );

	if ( retained_earnings > 0.0 )
	{
		journal->credit_amount = retained_earnings;
	}
	else
	{
		journal->debit_amount = -retained_earnings;
	}

	return journal;
}

double close_nominal_transaction_debit_amount(
		boolean element_accumulate_debit,
		double balance )
{
	double debit_amount;

	if ( !element_accumulate_debit )
		debit_amount = balance;
	else
		debit_amount = 0.0;

	return debit_amount;
}

double close_nominal_transaction_credit_amount(
		boolean element_accumulate_debit,
		double balance )
{
	double credit_amount;

	if ( element_accumulate_debit )
		credit_amount = balance;
	else
		credit_amount = 0.0;

	return credit_amount;
}

CLOSE_NOMINAL *close_nominal_fetch(
		char *application_name,
		char *process_name,
		char *as_of_date_string,
		boolean undo )
{
	CLOSE_NOMINAL *close_nominal;

	if ( !application_name
	||   !process_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	close_nominal = close_nominal_calloc();

	if ( undo )
	{
		close_nominal->close_nominal_undo =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			close_nominal_undo_fetch();

		if ( !close_nominal->
			close_nominal_undo->
			transaction_date_close_nominal_undo->
			transaction_date_time_memo_maximum_string )
		{
			close_nominal->undo_no_transaction_message =
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				close_nominal_undo_no_transaction_message();

			as_of_date_string = (char *)0;

			goto set_statement_caption;
		}

		as_of_date_string =
		    close_nominal->
			close_nominal_undo->
			transaction_date_close_nominal_undo->
			transaction_date_time_memo_maximum_string;
	}
	else
	{
		if ( !transaction_date_as_of_date_populated(
			as_of_date_string ) )
		{
			close_nominal->empty_date_message =
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				close_nominal_empty_date_message();

			as_of_date_string = (char *)0;

			goto set_statement_caption;
		}

		close_nominal->close_nominal_do =
			close_nominal_do_fetch(
				as_of_date_string );

		if ( close_nominal->
			close_nominal_do->
			transaction_date_close_nominal_do->
			transaction_date_close_boolean )
		{
			close_nominal->do_transaction_exists_message =
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				close_nominal_do_transaction_exists_message(
				   close_nominal->
					close_nominal_do->
					transaction_date_close_nominal_do->
					transaction_date_close_date_time );

			goto set_statement_caption;
		}

		if ( close_nominal->
			close_nominal_do->
			no_transactions_boolean )
		{
			close_nominal->do_no_transaction_message =
				/* ---------------------- */
				/* Returns program memory */
				/* ---------------------- */
				close_nominal_do_no_transaction_message();
		}
	}

set_statement_caption:

	close_nominal->statement_caption =
		statement_caption_new(
			application_name,
			process_name,
			(char *)0 /* begin_date_string */,
			as_of_date_string /* end_date_string */ );

	return close_nominal;
}

CLOSE_NOMINAL *close_nominal_calloc( void )
{
	CLOSE_NOMINAL *close_nominal;

	if ( ! ( close_nominal = calloc( 1, sizeof ( CLOSE_NOMINAL ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return close_nominal;
}

char *close_nominal_do_transaction_exists_message(
		char *transaction_date_close_date_time )
{
	static char message[ 128 ];

	sprintf(message,
		"<h3>ERROR: a closing entry exists as transaction %s.</h3>",
		transaction_date_close_date_time );

	return message;
}

CLOSE_NOMINAL_UNDO *close_nominal_undo_fetch( void )
{
	CLOSE_NOMINAL_UNDO *close_nominal_undo =
		close_nominal_undo_calloc();

	close_nominal_undo->entity_self =
		entity_self_fetch(
			0 /* not fetch_entity_boolean */ );

	if ( !close_nominal_undo->entity_self )
	{
		char message[ 128 ];

		sprintf(message, "entity_self_fetch() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	close_nominal_undo->
		transaction_date_close_nominal_undo =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			transaction_date_close_nominal_undo_new(
				close_nominal_undo->
					entity_self->
					full_name,
				close_nominal_undo->
					entity_self->
					street_address );

	if ( !close_nominal_undo->
		transaction_date_close_nominal_undo->
		transaction_date_time_memo_maximum_string )
	{
		return close_nominal_undo;
	}

	if ( ! ( close_nominal_undo->transaction =
		    transaction_fetch(
			close_nominal_undo->
				entity_self->
				full_name,
			close_nominal_undo->
				entity_self->
				street_address,
			close_nominal_undo->
				transaction_date_close_nominal_undo->
				transaction_date_time_memo_maximum_string,
			1 /* fetch_journal_list */ ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"transaction_fetch(%s) returned empty.",
			close_nominal_undo->
				transaction_date_close_nominal_undo->
				transaction_date_time_memo_maximum_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return close_nominal_undo;
}

CLOSE_NOMINAL_UNDO *close_nominal_undo_calloc( void )
{
	CLOSE_NOMINAL_UNDO *close_nominal_undo;

	if ( ! ( close_nominal_undo =
			calloc( 1, sizeof ( CLOSE_NOMINAL_UNDO ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return close_nominal_undo;
}

void close_nominal_undo_display( TRANSACTION *transaction )
{
	if ( !transaction )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: transaction is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	transaction_html_display( transaction );
}

void close_nominal_undo_execute( TRANSACTION *transaction )
{
	if ( !transaction )
	{
		char message[ 128 ];

		sprintf(message, "transaction is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	transaction_delete(
		transaction->full_name,
		transaction->street_address,
		transaction->transaction_date_time );
}

char *close_nominal_undo_execute_message( char *transaction_date_time )
{
	static char message[ 128 ];

	if ( !transaction_date_time )
	{
		strcpy( message, "<h3>No closing entry to delete.</h3>" );
	}
	else
	{
		snprintf(
			message,
			sizeof ( message ),
			"<h3>Transaction delete of %s complete.</h3>",
			transaction_date_time );
	}

	return message;
}

char *close_nominal_undo_no_transaction_message( void )
{
	return
	"<h3>ERROR: there are no closing entries to undo.</h3>";
}

char *close_nominal_do_no_transaction_message( void )
{
	return
	"<h3>ERROR: there are no transactions to close.</h3>";
}

char *close_nominal_empty_date_message( void )
{
	return
	"<h3>Please enter the as of date to close.</h3>";
}

char *close_nominal_do_execute_message(
		char *transaction_date_close_date_time )
{
	static char message[ 128 ];

	sprintf(message,
	"<h3>Closing entry inserted with transaction %s</h3>",
		transaction_date_close_date_time );

	return message;
}

char *close_nominal_do_no_execute_message( void )
{
	return
	"<h3>Closing entry display complete.</h3>";
}

char *close_nominal_undo_no_execute_message( void )
{
	return
	"<h3>Will delete this transaction:</h3>";
}

double close_nominal_do_transaction_amount(
		double close_nominal_do_debit_sum,
		double close_nominal_do_credit_sum )
{
	if ( close_nominal_do_debit_sum > close_nominal_do_credit_sum )
		return close_nominal_do_debit_sum;
	else
		return close_nominal_do_credit_sum;
}

boolean close_nominal_do_no_transactions_boolean(
		double transaction_amount )
{
	return
	float_virtually_same( transaction_amount, 0.0 );
}

