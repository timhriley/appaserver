/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/account_delta.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "date.h"
#include "float.h"
#include "appaserver_error.h"
#include "journal.h"
#include "account.h"
#include "entity_self.h"
#include "transaction.h"
#include "account_delta.h"

ACCOUNT_DELTA *account_delta_new(
		char *fund_name,
		char *debit_account_name,
		char *credit_account_name,
		boolean accumulate_debit_boolean,
		double ending_balance /* if liability, positive amount */,
		char *memo )
{
	ACCOUNT_DELTA *account_delta;

	account_delta = account_delta_calloc();

	account_delta->balance_account_name =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		account_delta_balance_account_name(
			debit_account_name,
			credit_account_name,
			accumulate_debit_boolean );

	/* Returns heap memory */
	/* ------------------- */
	account_delta->date_now19 = date_now19( date_utc_offset() );

	account_delta->account_journal_latest =
		account_journal_latest(
			JOURNAL_TABLE,
			fund_name,
			account_delta->balance_account_name,
			account_delta->date_now19 /* end_date_time_string */,
			0 /* not fetch_transaction */,
			0 /* not latest_zero_balance_boolean */ );

	account_delta->current_balance =
		/* --------------------------------------------- */
		/* Returns a component of account_journal_latest */
		/* --------------------------------------------- */
		account_delta_current_balance(
			account_delta->account_journal_latest );

	account_delta->amount =
		account_delta_amount(
			ending_balance,
			account_delta->current_balance );

	if ( float_virtually_same(
		account_delta->amount,
		0.0 ) )
	{
		return account_delta;
	}

	if ( ! ( account_delta->entity_self_fetch =
			entity_self_fetch(
				0 /* not fetch_entity_boolean */ ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"entity_self_fetch() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	account_delta->negative_boolean =
		account_delta_negative_boolean(
			account_delta->amount );

	account_delta->debit_account_name =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		account_delta_debit_account_name(
			debit_account_name,
			credit_account_name,
			accumulate_debit_boolean,
			account_delta->negative_boolean );

	account_delta->credit_account_name =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		account_delta_credit_account_name(
			debit_account_name,
			credit_account_name,
			accumulate_debit_boolean,
			account_delta->negative_boolean );

	account_delta->transaction_amount =
		account_delta_transaction_amount(
			account_delta->amount,
			account_delta->negative_boolean );

	account_delta->transaction_binary =
		transaction_binary(
			account_delta->entity_self_fetch->full_name,
			account_delta->entity_self_fetch->street_address,
			account_delta->date_now19 /* transaction_date_time */,
			account_delta->transaction_amount,
			memo,
			account_delta->debit_account_name,
			account_delta->credit_account_name );

	return account_delta;
}

ACCOUNT_DELTA *account_delta_calloc( void )
{
	ACCOUNT_DELTA *account_delta;

	if ( ! ( account_delta = calloc( 1, sizeof ( ACCOUNT_DELTA ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return account_delta;
}

char *account_delta_balance_account_name(
		char *debit_account_name,
		char *credit_account_name,
		boolean accumulate_debit_boolean )
{
	char *account_name;

	if ( accumulate_debit_boolean )
		account_name = debit_account_name;
	else
		account_name = credit_account_name;

	return account_name;
}

double account_delta_current_balance( ACCOUNT_JOURNAL *account_journal_latest )
{
	double current_balance = {0};

	if ( account_journal_latest )
		current_balance =
			account_journal_latest->
				balance;

	return current_balance;
}

double account_delta_amount(
		double ending_balance,
		double current_balance )
{
	return current_balance - ending_balance;
}

double account_delta_negative_boolean( double account_delta_amount )
{
	return ( account_delta_amount < 0.0 );
}

char *account_delta_debit_account_name(
		char *debit_account_name,
		char *credit_account_name,
		boolean accumulate_debit_boolean,
		boolean account_delta_negative_boolean )
{
	char *account_name;

	if ( accumulate_debit_boolean )
	{
		if ( !account_delta_negative_boolean )
			account_name = credit_account_name;
		else
			account_name = debit_account_name;
	}
	else
	{
		if ( !account_delta_negative_boolean )
			account_name = debit_account_name;
		else
			account_name = credit_account_name;
	}

	return account_name;
}

char *account_delta_credit_account_name(
		char *debit_account_name,
		char *credit_account_name,
		boolean accumulate_debit_boolean,
		boolean account_delta_negative_boolean )
{
	char *account_name;

	if ( accumulate_debit_boolean )
	{
		if ( !account_delta_negative_boolean )
			account_name = debit_account_name;
		else
			account_name = credit_account_name;
	}
	else
	{
		if ( !account_delta_negative_boolean )
			account_name = credit_account_name;
		else
			account_name = debit_account_name;
	}

	return account_name;
}

double account_delta_transaction_amount(
		double account_delta_amount,
		boolean account_delta_negative_boolean )
{
	if ( account_delta_negative_boolean )
		return -account_delta_amount;
	else
		return account_delta_amount;
}
