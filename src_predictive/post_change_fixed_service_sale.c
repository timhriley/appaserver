/* ---------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/post_change_fixed_service_sale.c	  */
/* ---------------------------------------------------------------------- */
/* 									  */
/* Freely available software: see Appaserver.org			  */
/* ---------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "entity.h"
#include "inventory_sale.h"
#include "sale.h"
#include "fixed_service_sale.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_fixed_service_sale_insert_update_delete(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *service_name;
	char *state;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s full_name street_address sale_date_time service_name state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	sale_date_time = argv[ 3 ];
	service_name = argv[ 4 ];
	state = argv[ 5 ];

	/* If change full_name or street address only. */
	/* --------------------------------------------- */
	if ( strcmp( sale_date_time, "sale_date_time" ) == 0 ) exit( 0 );

	/* If change sale_date_time. */
	/* ------------------------- */
	if ( strcmp( service_name, "service_name" ) == 0 ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) == 0
	||   strcmp( state, "delete" ) == 0 )
	{
		post_change_fixed_service_sale_insert_update_delete(
			full_name,
			street_address,
			sale_date_time,
			service_name );
	}

	return 0;
}

void post_change_fixed_service_sale_insert_update_delete(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name )
{
	SALE *sale;
	FIXED_SERVICE_SALE *fixed_service_sale;
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

	if ( ( fixed_service_sale =
			fixed_service_sale_seek(
				sale->fixed_service_sale_list,
				service_name ) ) )
	{
		fixed_service_sale_update(
			fixed_service_sale->
				fixed_service_work_hours,
			fixed_service_sale->
				customer_entity->
				full_name,
			fixed_service_sale->
				customer_entity->
				street_address,
			fixed_service_sale->sale_date_time,
			fixed_service_sale->service_name );
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

