/* ----------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/post_change_hourly_service_sale.c 	   */
/* ----------------------------------------------------------------------- */
/* 									   */
/* Freely available software: see Appaserver.org			   */
/* ----------------------------------------------------------------------- */

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
#include "hourly_service_sale.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_hourly_service_sale_insert_update_delete(
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *service_name,
				char *description );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *service_name;
	char *description;
	char *state;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s full_name street_address sale_date_time service_name description state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	sale_date_time = argv[ 3 ];
	service_name = argv[ 4 ];
	description = argv[ 5 ];
	state = argv[ 6 ];

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
		post_change_hourly_service_sale_insert_update_delete(
			full_name,
			street_address,
			sale_date_time,
			service_name,
			description );
	}

	return 0;
}

void post_change_hourly_service_sale_insert_update_delete(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *description )
{
	SALE *sale;
	HOURLY_SERVICE_SALE *hourly_service_sale;
	char *transaction_date_time = {0};

	if ( ! ( sale =
			/* Returns steady_state() */
			/* ---------------------- */
			sale_fetch(
				full_name,
				street_address,
				sale_date_time ) ) )
	{
		return;
	}

	if ( ( hourly_service_sale =
			hourly_service_sale_seek(
				sale->hourly_service_sale_list,
				service_name,
				description ) ) )
	{
		hourly_service_sale_update(
			hourly_service_sale->
				hourly_service_sale_work_hours,
			hourly_service_sale->
				hourly_service_sale_net_revenue,
			hourly_service_sale->
				customer_entity->
				full_name,
			hourly_service_sale->
				customer_entity->
				street_address,
			hourly_service_sale->
				sale_date_time,
			hourly_service_sale->
				service_name,
			hourly_service_sale->
				service_description );
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

