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
#include "fixed_service_sale.h"
#include "customer_sale.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_fixed_service_sale_delete(
			char *full_name,
			char *street_address,
			char *sale_date_time );

void post_change_fixed_service_sale_insert_update(
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

	if ( strcmp( state, "delete" ) == 0 )
	{
		post_change_fixed_service_sale_delete(
			full_name,
			street_address,
			sale_date_time );
	}
	else
	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) == 0 )
	{
		post_change_fixed_service_sale_insert_update(
			full_name,
			street_address,
			sale_date_time,
			service_name );
	}

	return 0;
}

void post_change_fixed_service_sale_insert_update(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name )
{
	CUSTOMER_SALE *customer_sale;
	FIXED_SERVICE_SALE *fixed_service_sale;

	if ( ! (  customer_sale =
			customer_sale_fetch(
				full_name,
				street_address,
				sale_date_time ) ) )
	{
		return;
	}

	if ( ! ( fixed_service_sale =
			customer_sale_fixed_service_sale_seek(
				customer_sale->fixed_service_sale_list,
				customer_sale->
					customer_entity->
					full_name,
				customer_sale->
					customer_entity->
					street_address,
				sale_date_time,
				service_name ) ) )
	{
		return;
	}

	fixed_service_sale->fixed_service_work_hours =
		fixed_service_work_hours(
			fixed_service_work_list(
				customer_sale->
					customer_entity->
					full_name,
				customer_sale->
					customer_entity->
					street_address,
				sale_date_time,
				service_name ) );

	fixed_service_sale_update(
		fixed_service_sale->
			fixed_service_work_hours,
		customer_sale->
			customer_entity->
			full_name,
		customer_sale->
			customer_entity->
			street_address,
		sale_date_time,
		service_name );

	if ( customer_sale->customer_sale_transaction )
	{
		transaction_date_time =
			transaction_refresh(
			)
	}
	else
	{
		transaction_date_time = (char *)0;
	}

	customer_sale_update(
		( customer_sale->
		  customer_sale_extended_price_total =
		  customer_sale_extended_price_total(
			( customer_sale->
				inventory_sale_list =
				inventory_sale_list(
				customer_sale->
					customer_entity->
					full_name,
				customer_sale->
					customer_entity->
					street_address,
				customer_sale->sale_date_time ) ),
			( customer_sale->
			  fixed_service_sale_list =
			  fixed_service_sale_list(
				customer_sale->
					customer_entity->
					full_name,
				customer_sale->
					customer_entity->
					street_address,
				customer_sale->sale_date_time ) ) ),
			( customer_sale->
			  hourly_service_sale_list =
			  hourly_service_sale_list(
				customer_sale->
					customer_entity->
					full_name,
				customer_sale->
					customer_entity->
					street_address,
				customer_sale->sale_date_time ) ) ),
		( customer_sale->
		  customer_sale_sales_tax =
		  customer_sale_sales_tax(
			customer_sale->
				inventory_sale_list,
			entity_self_sales_tax_rate() ) ),
		customer_sale_invoice_amount(
			customer_sale->
				customer_sale_extended_price_total ),
		double payment_total,
		double amount_due,
		transaction_date_time,
		char *full_name,
		char *street_address,
		char *sale_date_time );

}

void post_change_fixed_service_sale_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	CUSTOMER_SALE *customer_sale;

	if ( ! (  customer_sale =
			customer_sale_fetch(
				full_name,
				street_address,
				sale_date_time ) ) )
	{
		return;
	}

}

