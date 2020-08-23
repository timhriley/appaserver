/* -----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/post_change_customer_sale.c		*/
/* -----------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* -----------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "date.h"
#include "list.h"
#include "inventory.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "predictive.h"
#include "transaction.h"
#include "customer_sale.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_customer_sale_predelete(
			CUSTOMER_SALE *customer_sale );

void post_change_customer_sale_insert_update(
			CUSTOMER_SALE *customer_sale );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *state;
	CUSTOMER_SALE *customer_sale;
	char *preupdate_completed_date_time = {0};
	char *preupdate_shipped_date_time = {0};
	char *preupdate_arrived_date = {0};

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 5  )
	{
		fprintf( stderr,
"Usage: %s full_name street_address sale_date_time state [preupdate_completed_date_time preupdate_shipped_date_time preupdate_arrived_date]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	sale_date_time = argv[ 3 ];
	state = argv[ 4 ];

	if ( argc == 8 )
	{
		/* Future work */
		/* ----------- */
		if ( ( preupdate_completed_date_time = argv[ 5 ] ) ){}
		if ( ( preupdate_shipped_date_time = argv[ 6 ] ) ){}
		if ( ( preupdate_arrived_date = argv[ 7 ] ) ){}
	}

	if ( strcmp( sale_date_time, "sale_date_time" ) == 0 ) exit( 0 );

	/* Execute the predelete because CUSTOMER_SALE.transaction_date_time */
	/* ----------------------------------------------------------------- */
	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	if ( ! ( customer_sale =
			customer_sale_fetch(
				full_name,
				street_address,
				sale_date_time ) ) )
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: customer_sale_fetch(%s,%s,%s) returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address,
			 sale_date_time );
		exit( 1 );
	}

	if ( strcmp( state, "predelete" ) == 0 )
	{
		post_change_customer_sale_predelete(
			customer_sale );
	}
	else
	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) == 0 )
	{
		post_change_customer_sale_insert_update(
			customer_sale );
	}
	else
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: unrecognized state = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 state );
		exit( 1 );
	}

	return 0;
}

void post_change_customer_sale_insert_update(
			CUSTOMER_SALE *customer_sale )
{
	char *transaction_date_time;

	if ( !customer_sale ) return;

	customer_sale->customer_sale_invoice_amount =
	customer_sale->customer_sale_extended_price_total =
		customer_sale_extended_price_total(
			(LIST *)0 /* inventory_sale_list */,
			customer_fixed_service_sale_list(
				customer_sale->
					customer_entity->
					full_name,
				customer_sale->
					customer_entity->
					street_address,
				customer_sale->sale_date_time ) );

	customer_sale->customer_sale_payment_total =
		customer_sale_payment_total(
			customer_sale_payment_list(
				customer_sale->
					customer_entity->
					full_name,
				customer_sale->
					customer_entity->
					street_address,
				customer_sale->sale_date_time ) );

	customer_sale->customer_sale_amount_due =
		customer_sale->customer_sale_invoice_amount,
		customer_sale->customer_sale_payment_total );

	if ( customer_sale->completed_date_time
	&&   *customer_sale->completed_date_time )
	{
		customer_sale->customer_sale_transaction =
			customer_sale_transaction(
				customer_sale->
					customer_entity->
					full_name,
				customer_sale->
					customer_entity->
					street_address,
				customer_sale->
					sale_date_time,
				customer_sale->customer_sale_invoice_amount,
				0.0 /* sales_tax_amount */,
				account_receivable(),
				account_revenue(),
				account_sales_tax_payable() );
	}
	else
	{
		if ( customer_sale->customer_sale_transaction )
		{
			/* Also calls journal_propagate() */
			/* ------------------------------ */
			transaction_delete(
				customer_sale->
					customer_sale_transaction->
					full_name,
				customer_sale->
					customer_sale_transaction->
					street_address,
				customer_sale->
					customer_sale_transaction->
					transaction_date_time );

			customer_sale->customer_sale_transaction =
				(TRANSACTION *)0;
		}
	}

	if ( customer_sale->customer_sale_transaction )
	{
		transaction_date_time =
			transaction_refresh(
				customer_sale->
					customer_sale_transaction->
					full_name,
				customer_sale->
					customer_sale_transaction->
					street_address;
				customer_sale->
					customer_sale_transaction->
					transaction_date_time,
				customer_sale->
					customer_sale_transaction->
					transaction_amount,
				customer_sale->
					customer_sale_transaction->
					memo,
				0 /* check_number */,
				1 /* lock_transaction */,
				customer_sale->
					customer_sale_transaction->
					journal_list );
	}
	else
	{
		transaction_date_time = (char *)0;
	}

	customer_sale_update(
			customer_sale->customer_sale_extended_price_total,
			customer_sale->customer_sale_sales_tax,
			customer_sale->customer_sale_invoice_amount,
			customer_sale->customer_sale_payment_total,
			customer_sale->customer_sale_amount_due,
			transaction_date_time,
			customer_sale->
				customer_entity->
				full_name,
			customer_sale->
				customer_entity->
				street_address,
			customer_sale->sale_date_time );
}

void post_change_customer_sale_predelete(
			CUSTOMER_SALE *customer_sale )
{
}

