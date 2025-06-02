/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/hourly_service_sale_trigger.c	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "environ.h"
#include "appaserver_error.h"
#include "sale.h"
#include "hourly_service_sale.h"

void hourly_service_sale_trigger(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		char *state );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *service_name;
	char *service_description;
	char *state;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
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
		hourly_service_sale_trigger(
			full_name,
			street_address,
			sale_date_time,
			service_name,
			description );
	}

	return 0;
}

void hourly_service_sale_trigger(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *description )
{
	SALE *sale;
	HOURLY_SERVICE_SALE *hourly_service_sale;

	if ( ! ( sale =
		   sale_trigger_new(
			full_name,
			street_address,
			sale_date_time,
			(char *)0 /* uncollectible_date_time */,
			(char *)0 /* state */,
			(char *)0 /* preupdate_full_name */,
			(char *)0 /* preupdate_street_address */,
			(char *)0 /* preupdate_uncollectible_date_time */ ) ) )
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
			HOURLY_SERVICE_SALE_TABLE,
			full_name,
			street_address,
			sale_date_time,
			service_name,
			service_description,
			hourly_service_sale->
				hourly_service_sale_estimated_revenue,
			hourly_service_sale->
				hourly_service_sale_work_hours,
			hourly_service_sale->
				hourly_service_sale_net_revenue );
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

