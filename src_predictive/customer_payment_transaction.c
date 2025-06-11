/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/customer_payment_transaction.c	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "journal.h"
#include "customer_payment.h"
#include "customer_payment_transaction.h"

CUSTOMER_PAYMENT_TRANSACTION *customer_payment_transaction_new(
		char *full_name,
		char *street_address,
		char *payment_date_time,
		char *state,
		char *preupdate_full_name,
		char *preupdate_street_address,
		char *preupdate_payment_date_time,
		char *account_cash_string,
		double payment_amount )
{
	CUSTOMER_PAYMENT_TRANSACTION *customer_payment_transaction;

	if ( !full_name
	||   !street_address
	||   !payment_date_time
	||   !state
	||   !preupdate_full_name
	||   !preupdate_street_address
	||   !preupdate_payment_date_time
	||   !account_cash_string
	||   !payment_amount )
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

	customer_payment_transaction = customer_payment_transaction_calloc();

	if ( ! ( customer_payment_transaction->debit_account =
			account_fetch(
				account_cash_string,
				1 /* fetch_subclassification */,
				1 /* fetch_element */ ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"account_fetch(%s) returned empty.",
			account_cash_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	customer_payment_transaction->account_receivable_string =
		account_receivable_string(
			ACCOUNT_RECEIVABLE_KEY,
			__FUNCTION__ );

	if ( ! ( customer_payment_transaction->credit_account =
			account_fetch(
				customer_payment_transaction->
					account_receivable_string,
				1 /* fetch_subclassification */,
				1 /* fetch_element */ ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"account_fetch(%s) returned empty.",
			customer_payment_transaction->
				account_receivable_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	customer_payment_transaction->journal_binary_list =
		journal_binary_list(
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			(char *)0 /* transaction_date_time */,
			payment_amount
				/* transaction_amount */,
			customer_payment_transaction->debit_account,
			customer_payment_transaction->credit_account );

	customer_payment_transaction->subsidiary_transaction_state =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		subsidiary_transaction_state_new(
			"preupdate_full_name"
				/* preupdate_full_name_placeholder */,
			"preupdate_street_address"
				/* preupdate_street_address_placeholder */,
			"preupdate_payment_date_time"
				/* preupdate_foreign_date_time_placeholder */,
			state,
			preupdate_full_name,
			preupdate_street_address,
			preupdate_payment_date_time
				/* preupdate_foreign_date_time */,
			full_name,
			street_address,
			payment_date_time
				/* foreign_date_time */,
			customer_payment_transaction->journal_binary_list
				/* insert_journal_list */ );

	customer_payment_transaction->subsidiary_transaction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		subsidiary_transaction_new(
			CUSTOMER_PAYMENT_TABLE
				/* foreign_table_name */,
			"full_name"
				/* foreign_full_name_column */,
			"street_address"
				/* foreign_street_address_column */,
			"payment_date_time"
				/* foreign_date_time_column */,
			payment_date_time
				/* transaction_date_time */,
			customer_payment_transaction->journal_binary_list
				/* insert_journal_list */,
			full_name,
			street_address,
			payment_date_time
				/* foreign_date_time */,
			payment_amount
				/* foreign_amount */,
			CUSTOMER_PAYMENT_MEMO /* transaction_memo */,
			customer_payment_transaction->
				subsidiary_transaction_state->
				insert_boolean,
			customer_payment_transaction->
				subsidiary_transaction_state->
				delete_boolean,
			customer_payment_transaction->
				subsidiary_transaction_state->
				subsidiary_transaction_delete );

	return customer_payment_transaction;
}

CUSTOMER_PAYMENT_TRANSACTION *customer_payment_transaction_calloc( void )
{
	CUSTOMER_PAYMENT_TRANSACTION *customer_payment_transaction;

	if ( ! ( customer_payment_transaction =
			calloc( 1,
				sizeof ( CUSTOMER_PAYMENT_TRANSACTION ) ) ) )
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

	return customer_payment_transaction;
}

