/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/customer_payment_transaction.c	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "float.h"
#include "journal.h"
#include "customer_payment.h"
#include "customer_payment_transaction.h"

CUSTOMER_PAYMENT_TRANSACTION *customer_payment_transaction_new(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *payment_date_time,
		char *state,
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key,
		char *preupdate_payment_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean,
		char *cash_account_string,
		double payment_amount )
{
	CUSTOMER_PAYMENT_TRANSACTION *customer_payment_transaction;

	if ( !full_name
	||   !payment_date_time
	||   !state
	||   !cash_account_string )
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

	if ( float_money_virtually_same( payment_amount, 0.0 )
	||   payment_amount < 0.0 )
	{
		return NULL;
	}

	customer_payment_transaction = customer_payment_transaction_calloc();

	if ( ! ( customer_payment_transaction->debit_account =
			account_fetch(
				cash_account_string,
				1 /* fetch_subclassification */,
				1 /* fetch_element */ ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"account_fetch(%s) returned empty.",
			cash_account_string );

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
			(char *)0 /* fund_name */,
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
			"preupdate_fund_name"
				/* preupdate_fund_name_placeholder */,
			"preupdate_full_name"
				/* preupdate_full_name_placeholder */,
			"preupdate_contact_key"
				/* preupdate_contact_key_placeholder */,
			"preupdate_payment_date_time"
				/* preupdate_foreign_date_time_placeholder */,
			state,
			preupdate_fund_name,
			preupdate_full_name,
			preupdate_contact_key,
			preupdate_payment_date_time
				/* preupdate_foreign_date_time */,
			fund_name,
			full_name,
			contact_key,
			payment_date_time
				/* foreign_date_time */,
			fund_boolean,
			contact_key_boolean,
			customer_payment_transaction->journal_binary_list
				/* insert_journal_list */ );

	customer_payment_transaction->subsidiary_transaction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		subsidiary_transaction_new(
			CUSTOMER_PAYMENT_TABLE
				/* foreign_table_name */,
			"fund_name"
				/* foreign_fund_name_column */,
			"full_name"
				/* foreign_full_name_column */,
			"contact_key"
				/* foreign_contact_key_column */,
			"payment_date_time"
				/* foreign_date_time_column */,
			payment_date_time
				/* prior_transaction_date_time */,
			customer_payment_transaction->journal_binary_list
				/* insert_journal_list */,
			payment_amount
				/* foreign_amount */,
			CUSTOMER_PAYMENT_MEMO /* transaction_memo */,
			customer_payment_transaction->
				subsidiary_transaction_state->
				subsidiary_transaction_insert,
			customer_payment_transaction->
				subsidiary_transaction_state->
				subsidiary_transaction_delete,
			fund_boolean,
			contact_key_boolean );

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

