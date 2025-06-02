/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/customer_payment.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "sql.h"
#include "piece.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "sale.h"
#include "customer_payment.h"

LIST *customer_payment_list(
		const char *customer_payment_select,
		const char *customer_payment_table,
		char *full_name,
		char *street_address,
		char *sale_date_time )
{
	LIST *list = list_new();
	char *where;
	char *system_string;
	FILE *pipe;
	char input[ 1024 ];
	CUSTOMER_PAYMENT *customer_payment;

	if ( !full_name
	||   !street_address
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
			full_name,
			street_address,
			sale_date_time );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)customer_payment_select,
			(char *)customer_payment_table,
			where );

	pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	free( system_string );

	while ( string_input( input, pipe, sizeof ( input ) ) )
	{
		customer_payment =
			customer_payment_parse(
				full_name,
				street_address,
				sale_date_time,
				input );

		list_set( list, customer_payment );
	}

	return list;
}

CUSTOMER_PAYMENT *customer_payment_parse(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *input )
{
	char payment_date_time[ 128 ];
	char piece_buffer[ 1024 ];
	CUSTOMER_PAYMENT *customer_payment;

	if ( !input || !*input ) return NULL;

	/* See CUSTOMER_PAYMENT_SELECT */
	/* --------------------------- */
	piece( payment_date_time, SQL_DELIMITER, input, 0 );

	customer_payment =
		customer_payment_new(
			full_name,
			street_address,
			sale_date_time,
			strdup( payment_date_time ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	if ( *piece_buffer )
		customer_payment->account =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	if ( *piece_buffer )
		customer_payment->payment_amount =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	if ( *piece_buffer )
		customer_payment->check_number =
			atoi( piece_buffer );

	return customer_payment;
}

double customer_payment_total( LIST *customer_payment_list )
{
	CUSTOMER_PAYMENT *customer_payment;
	double total = 0.0;

	if ( list_rewind( customer_payment_list ) )
	do {
		customer_payment = list_get( customer_payment_list );
		total += customer_payment->payment_amount;

	} while ( list_next( customer_payment_list ) );

	return total;
}

char *customer_payment_primary_where(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *payment_date_time )
{
	static char where[ 256 ];

	if ( !full_name
	||   !street_address
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
		"%s and payment_date_time = '%s'",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		sale_primary_where(
			full_name,
			street_address,
			sale_date_time ),
		payment_date_time );

	return where;
}

CUSTOMER_PAYMENT *customer_payment_fetch(
		const char *customer_payment_select,
		const char *customer_payment_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *payment_date_time )
{
	char *primary_where;
	char *system_string;
	char *fetch;
	CUSTOMER_PAYMENT *customer_payment;

	if ( !full_name
	||   !street_address
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
			full_name,
			street_address,
			sale_date_time,
			payment_date_time );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			(char *)customer_payment_select,
			(char *)customer_payment_table,
			primary_where );

	if ( ! ( fetch = string_pipe_fetch( system_string ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"string_pipe_fetch(%s) returned empty.",
			system_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	free( system_string );

	customer_payment =
		customer_payment_parse(
			full_name,
			street_address,
			sale_date_time,
			fetch /* string_input */ );

	if ( !customer_payment )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"customer_payment_parse(%s) returned empty.",
			fetch );

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
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *payment_date_time,
		char *state,
		char *preupdate_full_name,
		char *preupdate_street_address,
		char *preupdate_payment_date_time )
{
	CUSTOMER_PAYMENT *customer_payment;

	if ( !full_name
	||   !street_address
	||   !sale_date_time
	||   !payment_date_time
	||   !state
	||   !preupdate_full_name
	||   !preupdate_street_address
	||   !preupdate_payment_date_time )
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
			full_name,
			street_address,
			sale_date_time,
			payment_date_time );

	if ( !customer_payment->payment_amount
	||   !customer_payment->account )
	{
		return customer_payment;
	}

	customer_payment->customer_payment_transaction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		customer_payment_transaction_new(
			full_name,
			street_address,
			payment_date_time,
			state,
			preupdate_full_name,
			preupdate_street_address,
			preupdate_payment_date_time,
			customer_payment->
				account /* account_cash_string */,
			customer_payment->
				payment_amount );

	return customer_payment;
}

CUSTOMER_PAYMENT *customer_payment_new(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *payment_date_time )
{
	CUSTOMER_PAYMENT *customer_payment;

	customer_payment = customer_payment_calloc();

	customer_payment->full_name = full_name;
	customer_payment->street_address = street_address;
	customer_payment->sale_date_time = sale_date_time;
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

