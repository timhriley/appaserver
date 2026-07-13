/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/customer.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "entity.h"
#include "journal.h"
#include "account.h"
#include "customer.h"

CUSTOMER *customer_fetch(
		char *customer_full_name,
		char *customer_contact_key,
		boolean contact_key_boolean,
		boolean fetch_entity_boolean,
		boolean fetch_past_due_boolean )
{
	char *where;
	char *system_string;
	char *input;

	if ( !customer_full_name )
	{
		char message[ 128 ];

		sprintf(message, "customer_full_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_primary_where(
			ENTITY_FULL_NAME_COLUMN,
			ENTITY_CONTACT_KEY_COLUMN,
			(char *)0 /* table_name */,
			customer_full_name,
			customer_contact_key,
			contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			CUSTOMER_SELECT,
			CUSTOMER_TABLE,
			where );

	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	input = string_system_input( system_string );

	return
	customer_parse(
		customer_full_name,
		customer_contact_key,
		contact_key_boolean,
		fetch_entity_boolean,
		fetch_past_due_boolean,
		input );
}

CUSTOMER *customer_parse(
		char *customer_full_name,
		char *customer_contact_key,
		boolean contact_key_boolean,
		boolean fetch_entity_boolean,
		boolean fetch_past_due_boolean,
		char *input )
{
	char buffer[ 128 ];
	CUSTOMER *customer;

	if ( !customer_full_name
	||   !input )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	customer =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		customer_new(
			customer_full_name,
			customer_contact_key );

	/* See CUSTOMER_SELECT */
	/* ------------------- */
	piece( buffer, SQL_DELIMITER, input, 0 );

	if ( *buffer ) customer->sales_tax_exempt_boolean = ( *buffer == 'y' );

	if ( fetch_entity_boolean )
	{
		customer->entity =
			entity_fetch(
				customer_full_name,
				customer_contact_key,
				contact_key_boolean );

		if ( !customer->entity )
		{
			char message[ 1024 ];

			sprintf(message,
				"entity_fetch(%s) returned empty.",
				customer_full_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}
	}

	if ( fetch_past_due_boolean )
	{
		customer->past_due =
			customer_past_due(
				ACCOUNT_PAYABLE_KEY,
				ACCOUNT_RECEIVABLE_KEY,
				customer_full_name,
				customer_contact_key,
				contact_key_boolean );
	}

	return customer;
}

CUSTOMER *customer_new(
		char *customer_full_name,
		char *customer_contact_key )
{
	CUSTOMER *customer;

	if ( !customer_full_name )
	{
		char message[ 128 ];

		sprintf(message, "customer_full_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	customer = customer_calloc();

	customer->customer_full_name = customer_full_name;
	customer->customer_contact_key = customer_contact_key;

	return customer;
}

CUSTOMER *customer_calloc( void )
{
	CUSTOMER *customer;

	if ( ! ( customer = calloc( 1, sizeof ( CUSTOMER ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return customer;
}

double customer_past_due(
		const char *account_payable_key,
		const char *account_receivable_key,
		char *customer_full_name,
		char *customer_contact_key,
		boolean contact_key_boolean )
{

	double payable_balance;
	double receivable_balance;
	LIST *list;

	list =
		journal_entity_list(
			JOURNAL_SELECT,
			JOURNAL_TABLE,
			(char *)0 /* fund_name */,
			customer_full_name,
			customer_contact_key,
			/* ------------------------------------ */
			/* Returns heap memory from static list */
			/* ------------------------------------ */
			account_payable_string(
				account_payable_key,
				__FUNCTION__ ),
			0 /* not predictive_fund_boolean */,
			contact_key_boolean );

	payable_balance =
		journal_debit_credit_sum_difference(
			0 /* not element_accumulate_debit */,
			list );

	list =
		journal_entity_list(
			JOURNAL_SELECT,
			JOURNAL_TABLE,
			(char *)0 /* fund_name */,
			customer_full_name,
			customer_contact_key,
			/* ------------------------------------ */
			/* Returns heap memory from static list */
			/* ------------------------------------ */
			account_receivable_string(
				account_receivable_key,
				__FUNCTION__ ),
			0 /* not predictive_fund_boolean */,
			contact_key_boolean );

	receivable_balance =
		journal_debit_credit_sum_difference(
			1 /* element_accumulate_debit */,
			list );

	return payable_balance - receivable_balance;
}

