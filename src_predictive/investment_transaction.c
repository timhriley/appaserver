/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/investment_transaction.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "date.h"
#include "float.h"
#include "journal.h"
#include "investment.h"
#include "investment_transaction.h"

INVESTMENT_TRANSACTION *investment_transaction_new(
		double investment_account_sum )
{
	INVESTMENT_TRANSACTION *investment_transaction;

	investment_transaction = investment_transaction_calloc();

	investment_transaction->account_key_account_name =
		/* ------------------------------------ */
		/* Returns heap memory from static list */
		/* ------------------------------------ */
		account_key_account_name(
			INVESTMENT_ACCOUNT_KEY,
			__FUNCTION__ );

	investment_transaction->date_now19 =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_now19( date_utc_offset() );

	investment_transaction->account_journal_latest =
		account_journal_latest(
			JOURNAL_TABLE,
			investment_transaction->account_key_account_name,
			investment_transaction->date_now19
				/* end_date_time_string */,
			0 /* not fetch_transaction */ );

	investment_transaction->current_balance =
		investment_transaction_current_balance(
			investment_transaction->
				account_journal_latest );

	investment_transaction->delta =
		investment_transaction_delta(
			investment_account_sum,
			investment_transaction->current_balance );


	if ( float_virtually_same(
		investment_transaction->delta,
		0.0 ) )
	{
		return investment_transaction;
	}

	if ( ! ( investment_transaction->entity_self =
			entity_self_fetch(
				0 /* not fetch_entity_boolean */ ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"Entity Self is not set." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	if ( investment_transaction->delta < 0.0 )
	{
		investment_transaction->transaction =
			investment_negative_transaction(
				investment_transaction->
					account_key_account_name
					/* investment_account_name */,
				investment_transaction->date_now19
					/* transaction_date_time */,
				-investment_transaction->delta
					/* transaction_amount */,
				investment_transaction->entity_self );
	}
	else
	/* ------------------------------------------- */
	/* Must be investment_transaction->delta > 0.0 */
	/* ------------------------------------------- */
	{
		investment_transaction->transaction =
			investment_positive_transaction(
				investment_transaction->
					account_key_account_name
					/* investment_account_name */,
				investment_transaction->date_now19
					/* transaction_date_time */,
				investment_transaction->delta
					/* transaction_amount */,
				investment_transaction->entity_self );
	}

	return investment_transaction;
}

INVESTMENT_TRANSACTION *investment_transaction_calloc( void )
{
	INVESTMENT_TRANSACTION *investment_transaction;

	if ( ! ( investment_transaction =
			calloc( 1,
				sizeof ( INVESTMENT_TRANSACTION ) ) ) )
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

	return investment_transaction;
}

double investment_transaction_current_balance(
		ACCOUNT_JOURNAL *account_journal_latest )
{
	if ( !account_journal_latest )
		return 0.0;
	else
		return account_journal_latest->balance;
}

double investment_transaction_delta(
		double investment_account_sum,
		double current_balance )
{
	return investment_account_sum - current_balance;
}

TRANSACTION *investment_negative_transaction(
		char *investment_account_name,
		char *transaction_date_time,
		double transaction_amount,
		ENTITY_SELF *entity_self )
{
	if ( !investment_account_name
	||   !transaction_date_time
	||   float_virtually_same( transaction_amount, 0.0 )
	||   !entity_self )
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

	return
	transaction_binary(
		entity_self->full_name,
		entity_self->street_address,
		transaction_date_time,
		transaction_amount,
		INVESTMENT_TRANSACTION_MEMO,
		/* ------------------------------------ */
		/* Returns heap memory from static list */
		/* ------------------------------------ */
		account_key_account_name(
			INVESTMENT_LOSS_ACCOUNT_KEY,
			__FUNCTION__ )
			/* debit_account_name */,
		investment_account_name
			/* credit_account_name */ );
}

TRANSACTION *investment_positive_transaction(
		char *investment_account_name,
		char *transaction_date_time,
		double transaction_amount,
		ENTITY_SELF *entity_self )
{
	if ( !investment_account_name
	||   !transaction_date_time
	||   float_virtually_same( transaction_amount, 0.0 )
	||   !entity_self )
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

	return
	transaction_binary(
		entity_self->full_name,
		entity_self->street_address,
		transaction_date_time,
		transaction_amount,
		INVESTMENT_TRANSACTION_MEMO,
		investment_account_name
			/* debit_account_name */,
		/* ------------------------------------ */
		/* Returns heap memory from static list */
		/* ------------------------------------ */
		account_key_account_name(
			INVESTMENT_GAIN_ACCOUNT_KEY,
			__FUNCTION__ )
			/* credit_account_name */ );
}
