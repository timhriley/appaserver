/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/post_change_customer_sale.c		*/
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

int main( int argc, char **argv )
{
	char *application_name;
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *sale_date_time;
	char *state;
	char *preupdate_fund_name;
	char *preupdate_full_name;
	char *preupdate_contact_key;
	char *preupdate_sale_date_time;
	char *preupdate_uncollectible_date_time;
	SALE *sale;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 10  )
	{
		fprintf(stderr,
"Usage: %s fund_name full_name contact_key sale_date_time state preupdate_fund_name preupdate_full_name preupdate_contact_key preupdate_uncollectible_date_time\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	fund_name = argv[ 1 ];
	full_name = argv[ 2 ];
	contact_key = argv[ 3 ];
	sale_date_time = argv[ 4 ];
	state = argv[ 5 ];
	preupdate_fund_name = argv[ 6 ];
	preupdate_full_name = argv[ 7 ];
	preupdate_contact_key = argv[ 8 ];
	preupdate_uncollectible_date_time = argv[ 9 ];

	if ( strcmp( state, APPASERVER_DELETE_STATE ) == 0 ) exit( 0 );

	sale =
		sale_trigger_new(
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			state,
			preupdate_fund_name,
			preupdate_full_name,
			preupdate_contact_key,
			preupdate_uncollectible_date_time );

	if ( !sale ) exit( 0 );

	sale_update(
		SALE_TABLE,
		full_name,
		contact_key,
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
		sale->sale_transaction,
		sale->sale_loss_transaction );

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

	if ( sale->sale_loss_transaction )
	{
		subsidiary_transaction_execute(
			application_name,
			sale->
				sale_loss_transaction->
				subsidiary_transaction->
				delete_transaction,
			sale->
				sale_loss_transaction->
				subsidiary_transaction->
				insert_transaction,
			sale->
				sale_loss_transaction->
				subsidiary_transaction->
				update_template );
	}

	return 0;
}
