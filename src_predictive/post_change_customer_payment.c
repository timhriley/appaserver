/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/post_change_customer_payment.c	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "environ.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "sale.h"
#include "customer_payment.h"

void customer_payment_trigger(
		char *application_name,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *payment_date_time,
		char *state,
		char *preupdate_full_name,
		char *preupdate_street_address,
		char *preupdate_payment_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *payment_date_time;
	char *state;
	char *preupdate_full_name;
	char *preupdate_street_address;
	char *preupdate_payment_date_time;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 9 )
	{
		fprintf( stderr,
"Usage: %s full_name street_address sale_date_time payment_date_time state preupdate_full_name preupdate_street_address preupdate_payment_date_time\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	sale_date_time = argv[ 3 ];
	payment_date_time = argv[ 4 ];
	state = argv[ 5 ];
	preupdate_full_name = argv[ 6 ];
	preupdate_street_address = argv[ 7 ];
	preupdate_payment_date_time = argv[ 8 ];

	if ( strcmp( sale_date_time, "sale_date_time" ) == 0 ) exit( 0 );

	if ( strcmp( payment_date_time, "payment_date_time" ) == 0 ) exit( 0 );

	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	customer_payment_trigger(
		application_name,
		full_name,
		street_address,
		sale_date_time,
		payment_date_time,
		state,
		preupdate_full_name,
		preupdate_street_address,
		preupdate_payment_date_time );

	return 0;
}

void customer_payment_trigger(
		char *application_name,
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
	SALE *sale;

	if ( strcmp(
		state,
		APPASERVER_INSERT_STATE ) == 0
	||   strcmp(
		state,
		APPASERVER_UPDATE_STATE ) == 0 )
	{
		customer_payment =
			customer_payment_trigger_new(
				CUSTOMER_PAYMENT_SELECT,
				CUSTOMER_PAYMENT_TABLE,
				full_name,
				street_address,
				sale_date_time,
				payment_date_time,
				state,
				preupdate_full_name,
				preupdate_street_address,
				preupdate_payment_date_time );

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
				update_template );
		}
	}

	sale =
		sale_trigger_new(
			full_name,
			street_address,
			sale_date_time,
			state,
			(char *)0 /* preupdate_full_name */,
			(char *)0 /* preupdate_street_address */,
			(char *)0 /* preupdate_uncollectible_date_time */ );

	sale_update(
		SALE_TABLE,
		full_name,
		street_address,
		sale_date_time,
		sale->
			sale_fetch->
			inventory_sale_boolean,
		sale->
			sale_fetch->
			specific_inventory_sale_boolean,
		sale->
			sale_fetch->
			fixed_service_sale_boolean,
		sale->
			sale_fetch->
			hourly_service_sale_boolean,
		sale->inventory_sale_total,
		sale->specific_inventory_sale_total,
		sale->fixed_service_sale_total,
		sale->hourly_service_sale_total,
		sale->gross_revenue,
		sale->sales_tax,
		sale->invoice_amount,
		sale->customer_payment_total,
		sale->amount_due,
		sale->sale_transaction );

	if ( sale->sale_transaction )
	{
		subsidiary_transaction_execute(
			application_name,
			sale->
				sale_transaction->
				subsidiary_transaction->
				delete_transaction,
			sale->
				sale_transaction->
				subsidiary_transaction->
				insert_transaction,
			sale->
				sale_transaction->
				subsidiary_transaction->
				update_template );
	}
}

