/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/customer_payment.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "sql.h"
#include "piece.h"
#include "float.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "sale.h"
#include "customer_payment.h"

LIST *customer_payment_list(
		const char *customer_payment_select,
		const char *customer_payment_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	LIST *list = list_new();
	char *where;
	char *system_string;
	FILE *pipe;
	char input[ 1024 ];
	CUSTOMER_PAYMENT *customer_payment;

	if ( !full_name
	||   !sale_date_time )
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

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		sale_primary_where(
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			fund_boolean,
			contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)customer_payment_select,
			(char *)customer_payment_table,
			where );

	/* -------------- */
	/* Safely returns */
	/* -------------- */
	pipe = appaserver_input_pipe( system_string );

	free( system_string );

	while ( string_input( input, pipe, sizeof ( input ) ) )
	{
		customer_payment = customer_payment_parse( input );
		list_set( list, customer_payment );
	}

	return list;
}

CUSTOMER_PAYMENT *customer_payment_parse(
		char *input )
{
	char payment_date_time[ 128 ];
	char buffer[ 1024 ];
	CUSTOMER_PAYMENT *customer_payment;

	if ( !input || !*input ) return NULL;

	/* See CUSTOMER_PAYMENT_SELECT */
	/* --------------------------- */
	piece( payment_date_time, SQL_DELIMITER, input, 0 );

	customer_payment =
		customer_payment_new(
			strdup( payment_date_time ) );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) customer_payment->cash_account = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) customer_payment->payment_amount = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) customer_payment->check_number = atoi( buffer );

	return customer_payment;
}

double customer_payment_total(
		char *cash_account,
		char *completed_date_time,
		double invoice_amount,
		LIST *customer_payment_list )
{
	CUSTOMER_PAYMENT *customer_payment;
	double total = 0.0;

	if ( completed_date_time )
	{
		if ( cash_account )
		{
			total = invoice_amount;
		}
		else
		if ( list_rewind( customer_payment_list ) )
		do {
			customer_payment = list_get( customer_payment_list );
			total += customer_payment->payment_amount;
	
		} while ( list_next( customer_payment_list ) );
	}

	return total;
}

char *customer_payment_primary_where(
		const char *sale_payment_date_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *payment_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	static char where[ 288 ];

	if ( !full_name
	||   !sale_date_time
	||   !payment_date_time )
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

	snprintf(
		where,
		sizeof ( where ),
		"%s and %s = '%s'",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		sale_primary_where(
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			fund_boolean,
			contact_key_boolean ),
		sale_payment_date_column,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_escape_date_time( payment_date_time ) );

	return where;
}

CUSTOMER_PAYMENT *customer_payment_fetch(
		const char *customer_payment_select,
		const char *customer_payment_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *payment_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *primary_where;
	char *system_string;
	char *input;
	CUSTOMER_PAYMENT *customer_payment;

	if ( !full_name
	||   !sale_date_time
	||   !payment_date_time )
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
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		customer_payment_primary_where(
			SALE_PAYMENT_DATE_COLUMN,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			payment_date_time,
			fund_boolean,
			contact_key_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)customer_payment_select,
			(char *)customer_payment_table,
			primary_where );

	if ( ! ( input = string_system_input( system_string ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"string_system_input(%s) returned empty.",
			system_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	free( system_string );
	customer_payment = customer_payment_parse( input );
	free( input );

	if ( !customer_payment )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"customer_payment_parse(%s) returned empty.",
			input );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return customer_payment;
}

CUSTOMER_PAYMENT *customer_payment_trigger_new(
		const char *customer_payment_select,
		const char *customer_payment_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *payment_date_time,
		char *state,
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key,
		char *preupdate_payment_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean)
{
	CUSTOMER_PAYMENT *customer_payment;

	if ( !full_name
	||   !sale_date_time
	||   !payment_date_time
	||   !state )
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

	customer_payment =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		customer_payment_fetch(
			customer_payment_select,
			customer_payment_table,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			payment_date_time,
			fund_boolean,
			contact_key_boolean );

	if ( float_money_virtually_same( customer_payment->payment_amount, 0.0 )
	||   customer_payment->cash_account )
	{
		return customer_payment;
	}

	customer_payment->customer_payment_transaction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		customer_payment_transaction_new(
			fund_name,
			full_name,
			contact_key,
			payment_date_time,
			state,
			preupdate_fund_name,
			preupdate_full_name,
			preupdate_contact_key,
			preupdate_payment_date_time,
			fund_boolean,
			contact_key_boolean,
			customer_payment->cash_account
				/* cash_account_string */,
			customer_payment->payment_amount );

	return customer_payment;
}

CUSTOMER_PAYMENT *customer_payment_new(
		char *payment_date_time )
{
	CUSTOMER_PAYMENT *customer_payment;

	customer_payment = customer_payment_calloc();
	customer_payment->payment_date_time = payment_date_time;

	return customer_payment;
}

CUSTOMER_PAYMENT *customer_payment_calloc( void )
{
	CUSTOMER_PAYMENT *customer_payment;

	if ( ! ( customer_payment =
			calloc( 1,
				sizeof ( CUSTOMER_PAYMENT ) ) ) )
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

	return customer_payment;
}

void customer_payment_trigger(
		char *application_name,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *payment_date_time,
		char *state,
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key,
		char *preupdate_payment_date_time )
{
	CUSTOMER_PAYMENT *customer_payment;
	SALE *sale;

	if ( strcmp(
		state,
		APPASERVER_PREDELETE_STATE ) == 0
	||   strcmp(
		state,
		APPASERVER_INSERT_STATE ) == 0 
	||   strcmp(
		state,
		APPASERVER_UPDATE_STATE ) == 0 )
	{
		boolean fund_boolean;
		boolean contact_key_boolean;

		fund_boolean =
			predictive_fund_boolean(
				PREDICTIVE_FUND_TABLE,
				PREDICTIVE_FUND_COLUMN );

		contact_key_boolean =
			entity_contact_key_boolean(
				ENTITY_TABLE,
				ENTITY_CONTACT_KEY_COLUMN );

		customer_payment =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			customer_payment_trigger_new(
				CUSTOMER_PAYMENT_SELECT,
				CUSTOMER_PAYMENT_TABLE,
				fund_name,
				full_name,
				contact_key,
				sale_date_time,
				payment_date_time,
				state,
				preupdate_fund_name,
				preupdate_full_name,
				preupdate_contact_key,
				preupdate_payment_date_time,
				fund_boolean,
				contact_key_boolean );

		if ( customer_payment->customer_payment_transaction )
		{
			subsidiary_transaction_execute(
				application_name,
				customer_payment->
					customer_payment_transaction->
					subsidiary_transaction->
					delete_transaction,
				customer_payment->
					customer_payment_transaction->
					subsidiary_transaction->
					insert_transaction,
				customer_payment->
					customer_payment_transaction->
					subsidiary_transaction->
					update_template,
				customer_payment->
					customer_payment_transaction->
					subsidiary_transaction->
					update_null_sql,
				customer_payment->
					customer_payment_transaction->
					subsidiary_transaction->
					predictive_fund_boolean,
				customer_payment->
					customer_payment_transaction->
					subsidiary_transaction->
					entity_contact_key_boolean );
		}
	}

	if ( strcmp(
		state,
		APPASERVER_PREDELETE_STATE ) != 0 )
	{
		sale =
			sale_trigger_new(
				fund_name,
				full_name,
				contact_key,
				sale_date_time,
				state,
				(char *)0 /* preupdate_fund_name */,
				(char *)0 /* preupdate_full_name */,
				(char *)0 /* preupdate_street_address */,
				(char *)0 /* preupdate_uncollectib...time */ );

		if ( !sale ) return;

		sale_update(
			application_name,
			sale->update_string_list,
			sale->update_system_string,
			sale->sale_transaction,
			(SALE_LOSS_TRANSACTION *)0 );
	}
}

