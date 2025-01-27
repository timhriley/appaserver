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
#include "account.h"
#include "sale.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_customer_sale_predelete(
			char *full_name,
			char *street_address,
			char *sale_date_time );

void post_change_customer_sale_insert_update(
			char *full_name,
			char *street_address,
			char *sale_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *state;
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

	if ( strcmp( state, "predelete" ) == 0 )
	{
		post_change_customer_sale_predelete(
			full_name,
			street_address,
			sale_date_time );
	}
	else
	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) == 0 )
	{
		post_change_customer_sale_insert_update(
			full_name,
			street_address,
			sale_date_time );
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
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	SALE *sale;
	char *transaction_date_time;

	if ( ! ( sale =
			/* Returns sale_steady_state() */
			/* --------------------------- */
			sale_fetch(
				full_name,
				street_address,
				sale_date_time ) ) )
	{
		return;
	}

	if ( sale->sale_transaction )
	{
		transaction_date_time =
		sale->sale_transaction->transaction_date_time =
			transaction_refresh(
				sale->sale_transaction->full_name,
				sale->sale_transaction->street_address,
				sale->sale_transaction->transaction_date_time,
				sale->sale_transaction->transaction_amount,
				sale->sale_transaction->memo,
				0 /* check_number */,
				sale->sale_transaction->lock_transaction,
				sale->sale_transaction->journal_list );
	}
	else
	{
		transaction_date_time = (char *)0;
	}

	sale_update(
			sale->inventory_sale_total,
			sale->fixed_service_sale_total,
			sale->hourly_service_sale_total,
			sale->sale_gross_revenue,
			sale->sales_tax,
			sale->sale_invoice_amount,
			sale->customer_payment_total,
			sale->sale_amount_due,
			transaction_date_time,
			sale->customer_entity->full_name,
			sale->customer_entity->street_address,
			sale->sale_date_time );
}

void post_change_customer_sale_predelete(
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	SALE *sale;

	if ( ! ( sale =
			/* Returns sale_steady_state() */
			/* --------------------------- */
			sale_fetch(
				full_name,
				street_address,
				sale_date_time ) ) )
	{
		return;
	}

	if ( sale->sale_transaction )
	{
		/* Performs journal_propagate() */
		/* ---------------------------- */
		transaction_delete(
			sale->
				sale_transaction->
				full_name,
			sale->
				sale_transaction->
				street_address,
			sale->
				sale_transaction->
				transaction_date_time );

		sale->sale_transaction = (TRANSACTION *)0;
	}
}

