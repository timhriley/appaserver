/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/customer_accrual.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include "String.h"
#include "appaserver_error.h"
#include "sql.h"
#include "account.h"
#include "subclassification.h"
#include "sale.h"
#include "customer_accrual.h"

CUSTOMER_ACCRUAL *customer_accrual_fetch(
		char *full_name,
		char *contact_key,
		boolean contact_key_boolean,
		char *primary_data_string,
		boolean receivable_expected_boolean,
		boolean payable_due_boolean )
{
	CUSTOMER_ACCRUAL *customer_accrual;

	if ( !full_name
	||   !primary_data_string )
	{
		char message[ 1024 ];

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

	customer_accrual = customer_accrual_calloc();
	customer_accrual->update_string_list = list_new();

	if ( receivable_expected_boolean )
	{
		customer_accrual->account_receivable_name_list =
			account_receivable_name_list(
				ACCOUNT_TABLE,
				SUBCLASSIFICATION_RECEIVABLE );

		customer_accrual->receivable =
			receivable_fetch(
				full_name,
				contact_key,
				0 /* not predictive_fund_boolean */,
				contact_key_boolean,
				customer_accrual->
					account_receivable_name_list );

		if ( customer_accrual->receivable )
		{
			list_set(
				customer_accrual->update_string_list,
				/* --------------------------- */
				/* Returns heap memory or null */
				/* (if not set_boolean)        */
				/* --------------------------- */
				sale_update_string(
					SQL_DELIMITER,
					primary_data_string,
					"receivable_expected" /* column_name */,
					customer_accrual->
						receivable->
						expected /* money */,
					1 /* set_boolean */ ) );
		}
	}

	if ( payable_due_boolean )
	{
		customer_accrual->exclude_account_name_list = list_new();

		list_set(
			customer_accrual->exclude_account_name_list,
			ACCOUNT_UNCLEARED_CHECKS );

		customer_accrual->account_current_liability_name_list =
			account_current_liability_name_list(
				ACCOUNT_TABLE,
				SUBCLASSIFICATION_CURRENT_LIABILITY,
				ACCOUNT_CREDIT_CARD_KEY,
				customer_accrual->exclude_account_name_list );

		customer_accrual->liability =
			liability_entity_fetch(
				full_name,
				contact_key,
				0 /* not predictive_fund_boolean */,
				contact_key_boolean,
				customer_accrual->
					account_current_liability_name_list );

		if ( customer_accrual->liability )
		{
			list_set(
				customer_accrual->update_string_list,
				/* --------------------------- */
				/* Returns heap memory or null */
				/* (if not set_boolean)        */
				/* --------------------------- */
				sale_update_string(
					SQL_DELIMITER,
					primary_data_string,
					"payable_due" /* column_name */,
					customer_accrual->
						liability->
						due /* money */,
					1 /* set_boolean */ ) );
		}
	}

	if ( !list_length( customer_accrual->update_string_list ) )
	{
		list_free( customer_accrual->update_string_list );
		customer_accrual->update_string_list = NULL;
	}

	return customer_accrual;
}

CUSTOMER_ACCRUAL *customer_accrual_calloc( void )
{
	CUSTOMER_ACCRUAL *customer_accrual;

	if ( ! ( customer_accrual = calloc( 1, sizeof ( CUSTOMER_ACCRUAL ) ) ) )
	{
		char message[ 1024 ];

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

	return customer_accrual;
}

