/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/sale_loss_transaction.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "journal.h"
#include "sale.h"
#include "float.h"
#include "sale_loss_transaction.h"

SALE_LOSS_TRANSACTION *sale_loss_transaction_new(
		char *full_name,
		char *street_address,
		char *uncollectible_date_time,
		char *state,
		char *preupdate_full_name,
		char *preupdate_street_address,
		char *preupdate_uncollectible_date_time,
		double sale_amount_due )
{
	SALE_LOSS_TRANSACTION *sale_loss_transaction;

	if ( !full_name
	||   !street_address
	||   !uncollectible_date_time
	||   !state
	||   !preupdate_full_name
	||   !preupdate_street_address
	||   !preupdate_uncollectible_date_time )
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

	if ( float_virtually_same( sale_amount_due, 0.0 ) ) return NULL;

	sale_loss_transaction = sale_loss_transaction_calloc();

	sale_loss_transaction->account_loss_string =
		account_loss_string(
			ACCOUNT_LOSS_KEY,
			__FUNCTION__ );

	if ( ! ( sale_loss_transaction->debit_account =
			account_fetch(
				sale_loss_transaction->account_loss_string,
				1 /* fetch_subclassification */,
				1 /* fetch_element */ ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"account_fetch(%s) returned empty.",
			sale_loss_transaction->account_loss_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sale_loss_transaction->account_receivable_string =
		account_receivable_string(
			ACCOUNT_RECEIVABLE_KEY,
			__FUNCTION__ );

	if ( ! ( sale_loss_transaction->credit_account =
			account_fetch(
				sale_loss_transaction->
					account_receivable_string,
				1 /* fetch_subclassification */,
				1 /* fetch_element */ ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"account_fetch(%s) returned empty.",
			sale_loss_transaction->account_receivable_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sale_loss_transaction->journal_binary_list =
		journal_binary_list(
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			(char *)0 /* transaction_date_time */,
			sale_amount_due
				/* transaction_amount */,
			sale_loss_transaction->debit_account,
			sale_loss_transaction->credit_account );

	sale_loss_transaction->subsidiary_transaction_state =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		subsidiary_transaction_state_new(
			"preupdate_full_name"
				/* preupdate_full_name_placeholder */,
			"preupdate_street_address"
				/* preupdate_street_address_placeholder */,
			"preupdate_uncollectible_date_time"
				/* preupdate_foreign_date_time_placeholder */,
			state,
			preupdate_full_name,
			preupdate_street_address,
			preupdate_uncollectible_date_time
				/* preupdate_foreign_date_time */,
			full_name,
			street_address,
			uncollectible_date_time
				/* foreign_date_time */,
			sale_loss_transaction->journal_binary_list
				/* insert_journal_list */ );

	sale_loss_transaction->subsidiary_transaction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		subsidiary_transaction_new(
			SALE_TABLE
				/* foreign_table_name */,
			"full_name"
				/* foreign_full_name_column */,
			"street_address"
				/* foreign_street_address_column */,
			"uncollectible_date_time"
				/* foreign_date_time_column */,
			uncollectible_date_time
				/* transaction_date_time */,
			sale_loss_transaction->journal_binary_list
				/* insert_journal_list */,
			full_name,
			street_address,
			uncollectible_date_time
				/* foreign_date_time */,
			sale_amount_due
				/* foreign_amount */,
			"Uncollectible loss" /* transaction_memo */,
			sale_loss_transaction->
				subsidiary_transaction_state->
				insert_boolean,
			sale_loss_transaction->
				subsidiary_transaction_state->
				delete_boolean,
			sale_loss_transaction->
				subsidiary_transaction_state->
				subsidiary_transaction_delete );

	return sale_loss_transaction;
}

SALE_LOSS_TRANSACTION *sale_loss_transaction_calloc( void )
{
	SALE_LOSS_TRANSACTION *sale_loss_transaction;

	if ( ! ( sale_loss_transaction =
			calloc( 1,
				sizeof ( SALE_LOSS_TRANSACTION ) ) ) )
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

	return sale_loss_transaction;
}

