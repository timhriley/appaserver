/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/journal_propagate.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "float.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "String.h"
#include "list.h"
#include "date.h"
#include "sql.h"
#include "piece.h"
#include "environ.h"
#include "boolean.h"
#include "entity.h"
#include "predictive.h"
#include "account.h"
#include "transaction.h"
#include "transaction_date.h"
#include "journal.h"
#include "journal_propagate.h"

JOURNAL_PROPAGATE *journal_propagate_new(
		char *fund_name,
		char *transaction_date_time,
		char *account_name )
{
	JOURNAL_PROPAGATE *journal_propagate;

	if ( !account_name )
	{
		char message[ 128 ];

		sprintf(message, "account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	journal_propagate = journal_propagate_calloc();

	/* If doing period of record */
	/* ------------------------- */
	if ( !string_atoi( transaction_date_time ) )
	{
		transaction_date_time =
			journal_min_transaction_date_time(
				JOURNAL_TABLE,
				fund_name,
				account_name );
	}

	/* If no journal entries for this account */
	/* -------------------------------------- */
	if ( !transaction_date_time ) return NULL;

	journal_propagate->journal_prior =
		journal_prior(
			fund_name,
			transaction_date_time,
			account_name,
			1 /* fetch_subclassification */,
			1 /* fetch_element */,
			1 /* fetch_account */ );

	journal_propagate->prior_transaction_date_time =
		journal_propagate_prior_transaction_date_time(
			journal_propagate->journal_prior );

	journal_propagate->prior_previous_balance =
		journal_propagate_prior_previous_balance(
			journal_propagate->journal_prior );

	journal_propagate->journal_list =
		journal_propagate_journal_list(
			fund_name,
			account_name,
			journal_propagate->prior_transaction_date_time,
			journal_propagate->prior_previous_balance );

	if ( list_length( journal_propagate->journal_list ) )
	{
		journal_propagate->accumulate_debit =
			journal_propagate_accumulate_debit(
				account_name,
				journal_propagate->journal_prior );

		journal_propagate_balance_set(
			journal_propagate->journal_list /* in/out */,
			journal_propagate->accumulate_debit );

		journal_propagate->update_statement_list =
			journal_propagate_update_statement_list(
				JOURNAL_TABLE,
				journal_propagate->journal_list );
	}

	return journal_propagate;
}

JOURNAL_PROPAGATE *journal_propagate_calloc( void )
{
	JOURNAL_PROPAGATE *journal_propagate;

	if ( ! ( journal_propagate =
			calloc( 1, sizeof ( JOURNAL_PROPAGATE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return journal_propagate;
}

char *journal_propagate_prior_transaction_date_time(
		JOURNAL *journal_prior )
{
	if ( journal_prior )
		return journal_prior->transaction_date_time;
	else
		return (char *)0;
}

double journal_propagate_prior_previous_balance(
		JOURNAL *journal_prior )
{
	if ( journal_prior )
		return journal_prior->previous_balance;
	else
		return 0.0;
}

boolean journal_propagate_accumulate_debit(
		char *account_name,
		JOURNAL *journal_prior )
{
	boolean accumulate_debit;

	if ( journal_prior )
	{
		accumulate_debit =
			journal_prior->
				account->
				subclassification->
				element->
				accumulate_debit;
	}
	else
	{
		accumulate_debit =
			account_accumulate_debit(
				account_name );
	}

	return accumulate_debit;
}

LIST *journal_propagate_journal_list(
		char *fund_name,
		char *account_name,
		char *prior_transaction_date_time,
		double prior_previous_balance )
{
	LIST *journal_list;

	if ( !account_name )
	{
		char message[ 128 ];

		sprintf(message, "account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	journal_list =
		journal_system_list(
			fund_name,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			journal_system_string(
				JOURNAL_SELECT,
				JOURNAL_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				journal_propagate_greater_equal_where(
					fund_name,
					account_name,
					prior_transaction_date_time ) ),
			0 /* not fetch_account */,
			0 /* not fetch_subclassification */,
			0 /* not fetch_element */,
			0 /* not fetch_transaction */ );

	if ( list_length( journal_list ) )
	{
		JOURNAL *first_journal;

		first_journal =
			list_first(
				journal_list );

		first_journal->previous_balance =	
			prior_previous_balance;
	}

	return journal_list;
}

void journal_propagate_balance_set(
		LIST *journal_list,
		boolean accumulate_debit )
{
	JOURNAL *journal;
	JOURNAL *prior_journal = {0};

	if ( list_rewind( journal_list ) )
	do {
		journal = list_get( journal_list );

		if ( prior_journal )
		{
			journal->previous_balance = prior_journal->balance;
		}

		journal->balance =
			journal_propagate_balance(
				accumulate_debit,
				journal->previous_balance,
				journal->debit_amount,
				journal->credit_amount );

		prior_journal = journal;

	} while( list_next( journal_list ) );
}

double journal_propagate_balance(
		boolean accumulate_debit,
		double previous_balance,
		double debit_amount,
		double credit_amount )
{
	double balance = 0.0;

	if ( accumulate_debit )
	{
		if ( debit_amount )
		{
			balance =
				previous_balance +
				debit_amount;
		}
		else
		if ( credit_amount )
		{
			balance =
				previous_balance -
				credit_amount;
		}
	}
	else
	{
		if ( credit_amount )
		{
			balance =
				previous_balance +
				credit_amount;
		}
		else
		if ( debit_amount )
		{
			balance =
				previous_balance -
				debit_amount;
		}
	}

	return balance;
}

void journal_propagate_previous_balance_set(
		LIST *journal_list /* in/out */,
		double end_balance )
{
	JOURNAL *journal;
	JOURNAL *prior_journal = {0};

	if ( list_go_tail( journal_list ) )
	do {
		journal = list_get( journal_list );

		if ( !prior_journal )
		{
			journal->balance = end_balance;
		}
		else
		{
			journal->balance = prior_journal->previous_balance;
		}

		journal->previous_balance =
			journal_propagate_previous_balance(
				journal->debit_amount,
				journal->credit_amount,
				journal->balance );

		prior_journal = journal;

	} while( list_prior( journal_list ) );
}

double journal_propagate_previous_balance(
		double debit_amount,
		double credit_amount,
		double balance )
{
	double previous_balance = 0.0;

	if ( debit_amount )
	{
		previous_balance =
			balance -
			debit_amount;
	}
	else
	if ( credit_amount )
	{
		previous_balance =
			balance +
			credit_amount;
	}

	return previous_balance;
}

char *journal_propagate_greater_equal_where(
		char *fund_name,
		char *account_name,
		char *prior_transaction_date_time )
{
	static char where[ 128 ];
	char *account_where;
	char *return_where = where;

	account_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		journal_account_where(
			fund_name,
			account_name );

	if ( prior_transaction_date_time )
	{
		snprintf(
			where,
			sizeof ( where ),
			"%s and transaction_date_time >= '%s'",
			account_where,
			prior_transaction_date_time );
	}
	else
	{
		return_where = account_where;
	}

	return return_where;
}

LIST *journal_propagate_update_statement_list(
		const char *journal_table,
		LIST *journal_propagate_journal_list )
{
	LIST *update_statement_list = list_new();
	JOURNAL *journal;
	char *update_statement;

	if ( list_rewind( journal_propagate_journal_list ) )
	do {
		journal =
			list_get(
				journal_propagate_journal_list );

		update_statement =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			journal_propagate_update_statement(
				journal_table,
				journal->fund_name,
				journal->full_name,
				journal->street_address,
				journal->transaction_date_time,
				journal->account_name,
				journal->previous_balance,
				journal->balance );

		list_set(
			update_statement_list,
			update_statement );

	} while ( list_next( journal_propagate_journal_list ) );

	if ( !list_length( update_statement_list ) )
	{
		list_free( update_statement_list );
		update_statement_list = NULL;
	}

	return update_statement_list;
}

char *journal_propagate_update_statement(
		const char *journal_table,
		char *fund_name,
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *account_name,
		double previous_balance,
		double balance )
{
	char *primary_where;
	char update_statement[ 2048 ];

	if ( !full_name
	||   !street_address
	||   !transaction_date_time
	||   !account_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	primary_where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		journal_primary_where(
			fund_name,
			full_name,
			street_address,
			transaction_date_time,
			account_name );

	snprintf(
		update_statement,
		sizeof ( update_statement ),
		"update %s "
		"set previous_balance = %.2lf, "
		"balance = %.2lf "
		"where %s;",
		journal_table,
		previous_balance,
		balance,
		primary_where );

	free( primary_where );

	return strdup( update_statement );
}

void journal_propagate_account_list(
		char *fund_name,
		char *transaction_date_time,
		LIST *journal_extract_account_list )
{
	ACCOUNT *account;
	JOURNAL_PROPAGATE *journal_propagate;

	if ( list_rewind( journal_extract_account_list ) )
	do {
		account =
			list_get(
				journal_extract_account_list );

		journal_propagate =
			journal_propagate_new(
				fund_name,
				transaction_date_time,
				account->account_name );

		if ( journal_propagate )
		{
			journal_list_update(
				journal_propagate->
					update_statement_list );
		}

	} while ( list_next( journal_extract_account_list ) );
}

