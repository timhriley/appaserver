/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/close_nominal.c		*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "element.h"
#include "account.h"
#include "journal.h"
#include "close_nominal.h"

CLOSE_NOMINAL_DO *close_nominal_do_calloc( void )
{
	CLOSE_NOMINAL_DO *close_nominal_do;

	if ( ! ( close_nominal_do = calloc( 1, sizeof( CLOSE_NOMINAL_DO ) ) ) )
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

CLOSE_NOMINAL_DO *close_nominal_do_fetch( char *as_of_date )
{
	CLOSE_NOMINAL_DO *close_nominal_do;

	if ( !as_of_date
	||   !*as_of_date
	||   strcmp( as_of_date, "as_of_date" ) == 0 )
	{
		return (CLOSE_NOMINAL_DO *)0;
	}

	close_nominal_do = close_nominal_do_calloc();

	close_nominal_do->transaction_date_time_closing =
		transaction_date_time_closing(
			TRANSACTION_PRECLOSE_TIME,
			TRANSACTION_CLOSE_TIME,
			as_of_date,
			0 /* not preclose_time_boolean */ );

	close_nominal_do->transaction_date_time_exists =
		transaction_date_time_exists(
			TRANSACTION_TABLE,
			TRANSACTION_DATE_TIME_COLUMN,
			close_nominal_do->transaction_date_time_closing );

	if ( close_nominal_do->transaction_date_time_exists )
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
			close_nominal_do->transaction_date_time_closing,
			1 /* fetch_subclassification_list */,
			1 /* fetch_account_list */,
			1 /* fetch_journal_latest */,
			0 /* not fetch_transaction */ );

	if ( !list_length( close_nominal_do->element_statement_list ) )
	{
		return close_nominal_do;
	}

	element_list_sum( close_nominal_do->element_statement_list );

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

	close_nominal_do->debit_sum =
		close_nominal_do_debit_sum(
			close_nominal_do->revenue_sum,
			close_nominal_do->gain_sum );

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

	close_nominal_do->account_drawing =
		account_drawing(
			ACCOUNT_DRAWING_KEY );

	close_nominal_do->drawing_sum =
		close_nominal_do_drawing_sum(
			close_nominal_do->transaction_date_time_closing,
			close_nominal_do->account_drawing );

	close_nominal_do->credit_sum =
		close_nominal_do_credit_sum(
			close_nominal_do->expense_sum,
			close_nominal_do->loss_sum,
			close_nominal_do->drawing_sum );

	close_nominal_do->retained_earnings =
		close_nominal_do_retained_earnings(
			close_nominal_do->debit_sum,
			close_nominal_do->credit_sum );

	close_nominal_do->account_closing_entry =
		account_closing_entry(
			ACCOUNT_CLOSING_KEY );

	if ( !close_nominal_do->account_closing_entry )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: account_closing_entry(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			ACCOUNT_CLOSING_KEY );
		exit( 1 );
	}

	close_nominal_do->entity_self = entity_self_fetch();

	if ( !close_nominal_do->entity_self )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: entity_self_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	close_nominal_do->close_nominal_transaction =
		close_nominal_transaction_new(
			close_nominal_do->element_statement_list /* in/out */,
			close_nominal_do->transaction_date_time_closing,
			close_nominal_do->account_drawing,
			close_nominal_do->drawing_sum,
			close_nominal_do->retained_earnings,
			close_nominal_do->account_closing_entry,
			close_nominal_do->entity_self->entity );

	if ( !close_nominal_do->close_nominal_transaction
	||   !close_nominal_do->close_nominal_transaction->transaction )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: close_nominal_transaction_new() could not generate a transaction.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
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
			account_drawing /* account_name */,
			transaction_date_time_closing,
			0 /* not fetch_transaction */ );

	if ( latest )
		return latest->balance;
	else
		return 0.0;
}

double close_nominal_do_debit_sum(
			double revenue_sum,
			double gain_sum )
{
	return revenue_sum + gain_sum;
}

double close_nominal_do_credit_sum(
			double expense_sum,
			double loss_sum,
			double drawing_sum )
{
	return
	expense_sum +
	loss_sum -
	/* ------------------------------ */
	/* Drawing has a negative balance */
	/* ------------------------------ */
	drawing_sum;
}

double close_nominal_do_retained_earnings(
			double debit_sum,
			double credit_sum )
{
	return debit_sum - credit_sum;
}

CLOSE_NOMINAL_TRANSACTION *
	close_nominal_transaction_new(
			LIST *element_statement_list /* in/out */,
			char *transaction_date_time_closing,
			char *account_drawing,
			double close_nominal_drawing_sum,
			double close_nominal_retained_earnings,
			char *account_closing_entry,
			ENTITY *entity )
{
	CLOSE_NOMINAL_TRANSACTION *close_nominal_transaction;

	if ( !list_length( element_statement_list )
	||   !transaction_date_time_closing
	||   !account_closing_entry
	||   !entity )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	close_nominal_transaction = close_nominal_transaction_calloc();

	close_nominal_transaction->transaction =
		transaction_new(
			entity->full_name,
			entity->street_address,
			transaction_date_time_closing );

	close_nominal_transaction->transaction->transaction_amount =
		float_abs( close_nominal_retained_earnings );

	close_nominal_transaction->transaction->memo =
		TRANSACTION_CLOSING_ENTRY_MEMO;

	close_nominal_transaction->transaction->journal_list =
		close_nominal_transaction_journal_list(
			element_statement_list /* in/out */,
			transaction_date_time_closing,
			account_drawing,
			close_nominal_drawing_sum,
			close_nominal_retained_earnings,
			account_closing_entry,
			entity );
	return close_nominal_transaction;
}

CLOSE_NOMINAL_TRANSACTION *
	close_nominal_transaction_calloc(
			void )
{
	CLOSE_NOMINAL_TRANSACTION *close_nominal_transaction;

	if ( ! ( close_nominal_transaction =
			calloc( 1, sizeof( CLOSE_NOMINAL_TRANSACTION ) ) ) )
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
			char *transaction_date_time_closing,
			char *account_drawing,
			double drawing_sum,
			double retained_earnings,
			char *account_closing_entry,
			ENTITY *entity )
{
	LIST *journal_list;
	ELEMENT *element;
	ACCOUNT *account;
	JOURNAL *journal;

	if ( !list_length( element_statement_list )
	||   !transaction_date_time_closing
	||   !account_closing_entry
	||   !entity )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	element_list_account_statement_list_set(
		element_statement_list );

	journal_list = list_new();

	list_rewind( element_statement_list );

	do {
		element =
			list_get(
				element_statement_list );

		if ( money_virtually_same( element->sum, 0.0 ) ) continue;

		list_rewind( element->account_statement_list );

		do {
			account =
				list_get(
					element->account_statement_list );

			if ( !account->account_journal_latest )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: account_journal_latest is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			if ( money_virtually_same(
				account->
					account_journal_latest->
					balance,
				0.0 ) )
			{
				continue;
			}

			journal =
				journal_new(
					entity->full_name,
					entity->street_address,
					transaction_date_time_closing,
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

	if ( drawing_sum )
	{
		list_set(
			journal_list,
			close_nominal_transaction_drawing_journal(
				transaction_date_time_closing,
				account_drawing,
				drawing_sum,
				entity ) );
	}

	if ( retained_earnings )
	{
		list_set(
			journal_list,
			close_nominal_transaction_close_journal(
				transaction_date_time_closing,
				retained_earnings,
				account_closing_entry,
				entity ) );
	}

	return journal_list;
}

JOURNAL *close_nominal_transaction_drawing_journal(
			char *transaction_date_time_closing,
			char *account_drawing,
			double drawing_sum,
			ENTITY *entity )
{
	JOURNAL *journal;

	if ( !transaction_date_time_closing
	||   !account_drawing
	||   !entity )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( money_virtually_same( drawing_sum, 0.0 ) )
	{
		return (JOURNAL *)0;
	}

	journal =
		journal_new(
			entity->full_name,
			entity->street_address,
			transaction_date_time_closing,
			account_drawing );

	journal->account =
		account_fetch(
			journal->account_name,
			1 /* fetch_subclassification */,
			1 /* fetch_element */ );

	journal->credit_amount =
		/* ------------------------------ */
		/* Drawing has a negative balance */
		/* ------------------------------ */
		0.0 - drawing_sum;

	return journal;
}

JOURNAL *close_nominal_transaction_close_journal(
			char *transaction_date_time_closing,
			double retained_earnings,
			char *account_closing_entry,
			ENTITY *entity )
{
	JOURNAL *journal;

	if ( !transaction_date_time_closing
	||   !account_closing_entry
	||   !entity )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( money_virtually_same( retained_earnings, 0.0 ) )
	{
		return (JOURNAL *)0;
	}

	journal =
		journal_new(
			entity->full_name,
			entity->street_address,
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
		journal->debit_amount = 0.0 - retained_earnings;
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
			char *as_of_date,
			boolean undo )
{
	CLOSE_NOMINAL *close_nominal;

	if ( !application_name
	||   !process_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	close_nominal = close_nominal_calloc();

	close_nominal->statement_caption =
		statement_caption_new(
			application_name,
			process_name,
			(char *)0 /* begin_date_string */,
			as_of_date /* end_date_string */ );

	if ( !undo )
	{
		if ( !as_of_date
		||   !*as_of_date
		||   strcmp( as_of_date, "as_of_date" ) == 0 )
		{
			return (CLOSE_NOMINAL *)0;
		}

		close_nominal->close_nominal_do =
			close_nominal_do_fetch(
				as_of_date );
	}
	else
	{
		close_nominal->close_nominal_undo =
			close_nominal_undo_fetch();
	}

	return close_nominal;
}

CLOSE_NOMINAL *close_nominal_calloc( void )
{
	CLOSE_NOMINAL *close_nominal;

	if ( ! ( close_nominal = calloc( 1, sizeof( CLOSE_NOMINAL ) ) ) )
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

char *close_nominal_do_transaction_exists_message( void )
{
	return
	"<h3>Warning: a closing entry exists for the date.</h3>";
}

CLOSE_NOMINAL_UNDO *close_nominal_undo_fetch( void )
{
	CLOSE_NOMINAL_UNDO *close_nominal_undo =
		close_nominal_undo_calloc();

	close_nominal_undo->entity_self =
		entity_self_fetch();

	if ( !close_nominal_undo->entity_self )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: entity_self_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	close_nominal_undo->transaction_date_time_memo_latest =
		transaction_date_time_memo_latest(
			TRANSACTION_TABLE,
			TRANSACTION_CLOSING_ENTRY_MEMO,
			close_nominal_undo->
				entity_self->
				entity->
				full_name,
			close_nominal_undo->
				entity_self->
				entity->
				street_address );

	if ( !close_nominal_undo->transaction_date_time_memo_latest )
	{
		return close_nominal_undo;
	}

	if ( ! ( close_nominal_undo->transaction =
			transaction_fetch(
				close_nominal_undo->
					entity_self->
					entity->
					full_name,
				close_nominal_undo->
					entity_self->
					entity->
					street_address,
				close_nominal_undo->
					transaction_date_time_memo_latest,
				1 /* fetch_journal_list */ ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: transaction_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return close_nominal_undo;
}

CLOSE_NOMINAL_UNDO *close_nominal_undo_calloc( void )
{
	CLOSE_NOMINAL_UNDO *close_nominal_undo;

	if ( ! ( close_nominal_undo =
			calloc( 1, sizeof( CLOSE_NOMINAL_UNDO ) ) ) )
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

	printf(	"%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		close_nominal_undo_display_message() );

	transaction_html_display( transaction );
}

char *close_nominal_undo_display_message( void )
{
	return
	"<h3>Will delete this transaction:</h3>";
}

void close_nominal_undo_execute( TRANSACTION *transaction )
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

	transaction_delete(
		transaction->full_name,
		transaction->street_address,
		transaction->transaction_date_time );

	printf( "%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		close_nominal_undo_execute_message() );
}

char *close_nominal_undo_execute_message( void )
{
	return
	"<h3>Transaction delete complete:</h3>";
}

