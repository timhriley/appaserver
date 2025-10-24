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
#include "journal.h"
#include "account.h"
#include "customer.h"

CUSTOMER *customer_fetch(
		char *customer_full_name,
		char *customer_street_address,
		boolean fetch_entity_boolean,
		boolean fetch_payable_balance_boolean )
{
	char *where;
	char *system_string;
	char *input;

	if ( !customer_full_name
	||   !customer_street_address )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

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
			customer_full_name,
			customer_street_address );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			CUSTOMER_SELECT,
			CUSTOMER_TABLE,
			where );

	input =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe_input( system_string );

	if ( !input ) return NULL;

	return
	customer_parse(
		customer_full_name,
		customer_street_address,
		fetch_entity_boolean,
		fetch_payable_balance_boolean,
		input );
}

CUSTOMER *customer_parse(
		char *customer_full_name,
		char *customer_street_address,
		boolean fetch_entity_boolean,
		boolean fetch_payable_balance_boolean,
		char *input )
{
	char piece_buffer[ 128 ];
	CUSTOMER *customer;

	if ( !customer_full_name
	||   !customer_street_address
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
			customer_street_address );

	/* See CUSTOMER_SELECT */
	/* ------------------- */
	piece( piece_buffer, SQL_DELIMITER, input, 0 );

	if ( *piece_buffer ) customer->sales_tax_exempt_boolean =
		( *piece_buffer == 'y' );

	if ( fetch_entity_boolean )
	{
		customer->entity =
			entity_fetch(
				customer_full_name,
				customer_street_address );

		if ( !customer->entity )
		{
			char message[ 1024 ];

			sprintf(message,
				"entity_fetch(%s,%s) returned empty.",
				customer_full_name,
				customer_street_address );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}
	}

	if ( fetch_payable_balance_boolean )
	{
		LIST *list;

		list =
			journal_entity_list(
				JOURNAL_SELECT,
				JOURNAL_TABLE,
				(char *)0 /* fund_name */,
				customer_full_name,
				customer_street_address,
				/* ------------------------------------ */
				/* Returns heap memory from static list */
				/* ------------------------------------ */
				account_payable_string(
					ACCOUNT_PAYABLE_KEY,
					__FUNCTION__ ) );

		customer->journal_payable_balance =
			journal_debit_credit_sum_difference(
				0 /* not element_accumulate_debit */,
				list );
	}

	return customer;
}

CUSTOMER *customer_new(
		char *customer_full_name,
		char *customer_street_address )
{
	CUSTOMER *customer;

	if ( !customer_full_name
	||   !customer_street_address )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	customer = customer_calloc();

	customer->customer_full_name = customer_full_name;
	customer->customer_street_address = customer_street_address;

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

