/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/post_change_customer_sale.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "environ.h"
#include "appaserver_error.h"
#include "sale.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *state;
	char *preupdate_full_name;
	char *preupdate_street_address;
	char *preupdate_uncollectible_date_time;
	SALE *sale;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8  )
	{
		fprintf(stderr,
"Usage: %s full_name street_address sale_date_time state preupdate_full_name preupdate_street_address preupdate_uncollectible_date_time\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	sale_date_time = argv[ 3 ];
	state = argv[ 4 ];
	preupdate_full_name = argv[ 5 ];
	preupdate_street_address = argv[ 6 ];
	preupdate_uncollectible_date_time = argv[ 7 ];

	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	sale =
		sale_trigger_new(
			full_name,
			street_address,
			sale_date_time,
			state,
			preupdate_full_name,
			preupdate_street_address,
			preupdate_uncollectible_date_time );

	if ( !sale ) exit( 0 );

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

	return 0;
}
