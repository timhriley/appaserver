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
#include "list.h"
#include "boolean.h"
#include "transaction.h"
#include "entity.h"
#include "sale.h"
#include "journal.h"
#include "customer_payment.h"

CUSTOMER_PAYMENT *customer_payment_new(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *payment_date_time )
{
	CUSTOMER_PAYMENT *customer_payment;

	if ( ! ( customer_payment = calloc( 1, sizeof( CUSTOMER_PAYMENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	customer_payment->customer_entity =
		entity_new(
			full_name,
			street_address );

	customer_payment->sale_date_time = sale_date_time;
	customer_payment->payment_date_time = payment_date_time;
	
	return customer_payment;
}

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
			customer_payment_select,
			customer_payment_table,
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
		char *string_input )
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

