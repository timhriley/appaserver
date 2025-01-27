/* ------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/post_change_inventory_sale.c		*/
/* ------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit Appaserver.org	*/
/* ------------------------------------------------------------------	*/

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
#include "inventory.h"
#include "customer.h"

void post_change_inventory_sale_inventory_name_update(
			CUSTOMER_SALE *customer_sale,
			char *inventory_name,
			char *preupdate_inventory_name,
			char *application_name );

void post_change_inventory_sale_discount_amount_update(
			CUSTOMER_SALE *customer_sale,
			char *inventory_name,
			char *application_name );

void post_change_inventory_sale_retail_price_update(
			CUSTOMER_SALE *customer_sale,
			char *inventory_name,
			char *application_name );

void post_change_inventory_sale_quantity_update(
			CUSTOMER_SALE *customer_sale,
			char *inventory_name,
			char *application_name );

void post_change_inventory_sale_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *inventory_name );

void post_change_inventory_sale_insert(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *inventory_name );

void post_change_inventory_sale_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *inventory_name,
			char *preupdate_quantity,
			char *preupdate_retail_price,
			char *preupdate_discount_amount,
			char *preupdate_inventory_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *inventory_name;
	char *state;
	char *preupdate_quantity;
	char *preupdate_retail_price;
	char *preupdate_discount_amount;
	char *preupdate_inventory_name;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 11 )
	{
		fprintf( stderr,
"Usage: %s ignored full_name street_address sale_date_time inventory_name state preupdate_quantity preupdate_retail_price preupdate_discount_amount preupdate_inventory_name\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	sale_date_time = argv[ 4 ];
	inventory_name = argv[ 5 ];
	state = argv[ 6 ];
	preupdate_quantity = argv[ 7 ];
	preupdate_retail_price = argv[ 8 ];
	preupdate_discount_amount = argv[ 9 ];
	preupdate_inventory_name = argv[ 10 ];

	/* If changing the full_name or street_address only, then exit. */
	/* ------------------------------------------------------------ */
	if ( strcmp( sale_date_time, "sale_date_time" ) == 0 ) exit( 0 );

	/* ---------------------------------------------------- */
	/* INVENTORY_SALE.transaction_date_time doesn't exist,	*/
	/* so execute after delete.				*/
	/* ---------------------------------------------------- */
	if ( strcmp( state, "predelete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "delete" ) == 0 )
	{
		post_change_inventory_sale_delete(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			inventory_name );
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_inventory_sale_insert(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			inventory_name );
	}
	else
	{
		post_change_inventory_sale_update(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			inventory_name,
			preupdate_quantity,
			preupdate_retail_price,
			preupdate_discount_amount,
			preupdate_inventory_name );
	}

	return 0;
}

void post_change_inventory_sale_insert(
		char *application_name,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *inventory_name )
{
	CUSTOMER_SALE *customer_sale;

	if ( ! (  customer_sale =
			customer_sale_new(
				application_name,
				full_name,
				street_address,
				sale_date_time ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: customer_sale_new() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Propagate inventory layers */
	/* -------------------------- */
	if ( customer_sale->completed_date_time )
	{
		INVENTORY_SALE *inventory_sale;
		char sys_string[ 1024 ];

		if ( ! ( inventory_sale =
				inventory_sale_list_seek(
					customer_sale->inventory_sale_list,
					inventory_name ) ) )
		{
			fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek inventory_sale.\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__ );
			exit( 1 );
		}

		sprintf( sys_string,
"propagate_inventory_sale_layers %s \"%s\" \"%s\" \"%s\" \"%s\" '' %c",
			 application_name,
			 customer_sale->full_name,
			 customer_sale->street_address,
			 customer_sale->sale_date_time,
			 inventory_name,
			 ( customer_sale_inventory_is_latest(
				application_name,
				inventory_name,
				customer_sale->sale_date_time ) )
			 		? 'y' : 'n' );

		inventory_sale->cost_of_goods_sold =
			atof( pipe2string( sys_string ) );

	} /* if completed_date_time */

	customer_sale->invoice_amount =
		customer_sale_get_invoice_amount(
			&customer_sale->
				sum_inventory_extension,
			&customer_sale->
				sum_fixed_service_extension,
			&customer_sale->
				sum_hourly_service_extension,
			&customer_sale->sum_extension,
			&customer_sale->sales_tax,
			customer_sale->shipping_revenue,
			customer_sale->
			inventory_sale_list,
			customer_sale->
				specific_inventory_sale_list,
			customer_sale->fixed_service_sale_list,
			customer_sale->hourly_service_sale_list,
			customer_sale->full_name,
			customer_sale->street_address,
			application_name );

	customer_sale->amount_due =
		customer_sale->invoice_amount -
		customer_sale->total_payment
			/* set in customer_sale_new() */;

	customer_sale_update(
		customer_sale->sum_extension,
		customer_sale->database_sum_extension,
		customer_sale->sales_tax,
		customer_sale->database_sales_tax,
		customer_sale->invoice_amount,
		customer_sale->database_invoice_amount,
		customer_sale->completed_date_time,
		customer_sale->
			database_completed_date_time,
		customer_sale->shipped_date_time,
		customer_sale->database_shipped_date_time,
		customer_sale->arrived_date,
		customer_sale->database_arrived_date,
		customer_sale->total_payment,
		customer_sale->database_total_payment,
		customer_sale->amount_due,
		customer_sale->database_amount_due,
		customer_sale->transaction_date_time,
		customer_sale->
			database_transaction_date_time,
		customer_sale->full_name,
		customer_sale->street_address,
		customer_sale->sale_date_time,
		application_name );

	if ( customer_sale->transaction_date_time )
	{
		customer_sale->transaction =
			ledger_customer_sale_build_transaction(
				application_name,
				customer_sale->transaction->full_name,
				customer_sale->transaction->street_address,
				customer_sale->transaction->
					transaction_date_time,
				customer_sale->transaction->memo,
				customer_sale->inventory_sale_list,
				customer_sale->specific_inventory_sale_list,
				customer_sale->fixed_service_sale_list,
				customer_sale->hourly_service_sale_list,
				customer_sale->shipping_revenue,
				customer_sale->sales_tax,
				customer_sale->invoice_amount,
				customer_sale->fund_name );

		if ( customer_sale->transaction )
		{
			ledger_transaction_refresh(
				application_name,
				customer_sale->full_name,
				customer_sale->street_address,
				customer_sale->transaction_date_time,
				customer_sale->transaction->transaction_amount,
				customer_sale->transaction->memo,
				0 /* check_number */,
				customer_sale->transaction->lock_transaction,
				customer_sale->
					transaction->
					journal_ledger_list );
		}

	} /* if transaction_date_time */

} /* post_change_inventory_sale_insert() */

void post_change_inventory_sale_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *inventory_name,
			char *preupdate_quantity,
			char *preupdate_retail_price,
			char *preupdate_discount_amount,
			char *preupdate_inventory_name )
{
	CUSTOMER_SALE *customer_sale;
	enum preupdate_change_state quantity_change_state;
	enum preupdate_change_state retail_price_change_state;
	enum preupdate_change_state discount_amount_change_state;
	enum preupdate_change_state inventory_name_change_state;

	quantity_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_quantity,
			(char *)0 /* postupdate_data */,
			"preupdate_quantity" );

	retail_price_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_retail_price,
			(char *)0 /* postupdate_data */,
			"preupdate_retail_price" );

	discount_amount_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_discount_amount,
			(char *)0 /* postupdate_data */,
			"preupdate_discount_amount" );

	inventory_name_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_inventory_name,
			(char *)0 /* postupdate_data */,
			"preupdate_inventory_name" );

	if ( ! (  customer_sale =
			customer_sale_new(
				application_name,
				full_name,
				street_address,
				sale_date_time ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: customer_sale_new() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return;
	}

	if ( quantity_change_state == from_something_to_something_else
	||   quantity_change_state == from_null_to_something )
	{
		post_change_inventory_sale_quantity_update(
			customer_sale,
			inventory_name,
			application_name );
	}

	if ( retail_price_change_state == from_something_to_something_else
	||   retail_price_change_state == from_null_to_something )
	{
		post_change_inventory_sale_retail_price_update(
			customer_sale,
			inventory_name,
			application_name );
	}

	if ( discount_amount_change_state == from_something_to_something_else
	||   discount_amount_change_state == from_null_to_something )
	{
		post_change_inventory_sale_discount_amount_update(
			customer_sale,
			inventory_name,
			application_name );
	}

	if ( inventory_name_change_state == from_something_to_something_else )
	{
		post_change_inventory_sale_inventory_name_update(
			customer_sale,
			inventory_name,
			preupdate_inventory_name,
			application_name );
	}

} /* post_change_inventory_sale_update() */

void post_change_inventory_sale_quantity_update(
			CUSTOMER_SALE *customer_sale,
			char *inventory_name,
			char *application_name )
{
	INVENTORY_SALE *inventory_sale;
	char sys_string[ 1024 ];

	if ( ! ( inventory_sale =
			inventory_sale_list_seek(
				customer_sale->inventory_sale_list,
				inventory_name ) ) )
	{
		fprintf( stderr,
		 "ERROR in %s/%s()/%d: cannot seek inventory_sale.\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__ );
		exit( 1 );
	}

	inventory_sale->extension =
		inventory_sale_get_extension(
			inventory_sale->retail_price,
			inventory_sale->sold_quantity,
			inventory_sale->discount_amount );

	inventory_sale_list_update(
		customer_sale->inventory_sale_list,
		application_name );

	customer_sale->invoice_amount =
		customer_sale_get_invoice_amount(
			&customer_sale->
				sum_inventory_extension,
			&customer_sale->
				sum_fixed_service_extension,
			&customer_sale->
				sum_hourly_service_extension,
			&customer_sale->sum_extension,
			&customer_sale->sales_tax,
			customer_sale->shipping_revenue,
			customer_sale->
			inventory_sale_list,
			customer_sale->
				specific_inventory_sale_list,
			customer_sale->fixed_service_sale_list,
			customer_sale->hourly_service_sale_list,
			customer_sale->full_name,
			customer_sale->street_address,
			application_name );

	customer_sale->amount_due =
		CUSTOMER_GET_AMOUNT_DUE(
			customer_sale->invoice_amount,
			customer_sale->total_payment );

	customer_sale_update(
		customer_sale->sum_extension,
		customer_sale->database_sum_extension,
		customer_sale->sales_tax,
		customer_sale->database_sales_tax,
		customer_sale->invoice_amount,
		customer_sale->database_invoice_amount,
		customer_sale->completed_date_time,
		customer_sale->
			database_completed_date_time,
		customer_sale->shipped_date_time,
		customer_sale->database_shipped_date_time,
		customer_sale->arrived_date,
		customer_sale->database_arrived_date,
		customer_sale->total_payment,
		customer_sale->database_total_payment,
		customer_sale->amount_due,
		customer_sale->database_amount_due,
		customer_sale->transaction_date_time,
		customer_sale->
			database_transaction_date_time,
		customer_sale->full_name,
		customer_sale->street_address,
		customer_sale->sale_date_time,
		application_name );

	if ( customer_sale->completed_date_time )
	{
		sprintf( sys_string,
"propagate_inventory_sale_layers %s \"%s\" \"%s\" \"%s\" \"%s\" '' %c",
			 application_name,
			 customer_sale->full_name,
			 customer_sale->street_address,
			 customer_sale->sale_date_time,
			 inventory_name,
			 'n' );

		inventory_sale->cost_of_goods_sold =
			atof( pipe2string( sys_string ) );

	} /* if completed_date_time */

	if ( customer_sale->transaction_date_time )
	{
		customer_sale->transaction =
			ledger_customer_sale_build_transaction(
				application_name,
				customer_sale->transaction->full_name,
				customer_sale->transaction->street_address,
				customer_sale->transaction->
					transaction_date_time,
				customer_sale->transaction->memo,
				customer_sale->inventory_sale_list,
				customer_sale->specific_inventory_sale_list,
				customer_sale->fixed_service_sale_list,
				customer_sale->hourly_service_sale_list,
				customer_sale->shipping_revenue,
				customer_sale->sales_tax,
				customer_sale->invoice_amount,
				customer_sale->fund_name );

		if ( customer_sale->transaction )
		{
			ledger_transaction_refresh(
				application_name,
				customer_sale->full_name,
				customer_sale->street_address,
				customer_sale->transaction_date_time,
				customer_sale->transaction->transaction_amount,
				customer_sale->transaction->memo,
				0 /* check_number */,
				customer_sale->transaction->lock_transaction,
				customer_sale->
					transaction->
					journal_ledger_list );
		}

	} /* if transaction_date_time */

} /* post_change_inventory_sale_quantity_update() */

void post_change_inventory_sale_retail_price_update(
			CUSTOMER_SALE *customer_sale,
			char *inventory_name,
			char *application_name )
{
	INVENTORY_SALE *inventory_sale;

	if ( ! ( inventory_sale =
			inventory_sale_list_seek(
				customer_sale->inventory_sale_list,
				inventory_name ) ) )
	{
		fprintf( stderr,
		 "ERROR in %s/%s()/%d: cannot seek inventory_sale.\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__ );
		exit( 1 );
	}

	inventory_sale->extension =
		inventory_sale_get_extension(
			inventory_sale->retail_price,
			inventory_sale->sold_quantity,
			inventory_sale->discount_amount );

	customer_sale->invoice_amount =
		customer_sale_get_invoice_amount(
			&customer_sale->
				sum_inventory_extension,
			&customer_sale->
				sum_fixed_service_extension,
			&customer_sale->
				sum_hourly_service_extension,
			&customer_sale->sum_extension,
			&customer_sale->sales_tax,
			customer_sale->shipping_revenue,
			customer_sale->
				inventory_sale_list,
			customer_sale->
				specific_inventory_sale_list,
			customer_sale->fixed_service_sale_list,
			customer_sale->hourly_service_sale_list,
			customer_sale->full_name,
			customer_sale->street_address,
			application_name );

	customer_sale->amount_due =
		customer_sale->invoice_amount -
		customer_sale->total_payment;

	customer_sale_update(
		customer_sale->sum_extension,
		customer_sale->database_sum_extension,
		customer_sale->sales_tax,
		customer_sale->database_sales_tax,
		customer_sale->invoice_amount,
		customer_sale->database_invoice_amount,
		customer_sale->completed_date_time,
		customer_sale->
			database_completed_date_time,
		customer_sale->shipped_date_time,
		customer_sale->database_shipped_date_time,
		customer_sale->arrived_date,
		customer_sale->database_arrived_date,
		customer_sale->total_payment,
		customer_sale->database_total_payment,
		customer_sale->amount_due,
		customer_sale->database_amount_due,
		customer_sale->transaction_date_time,
		customer_sale->
			database_transaction_date_time,
		customer_sale->full_name,
		customer_sale->street_address,
		customer_sale->sale_date_time,
		application_name );

	if ( customer_sale->transaction_date_time )
	{
		customer_sale->transaction =
			ledger_customer_sale_build_transaction(
				application_name,
				customer_sale->transaction->full_name,
				customer_sale->transaction->street_address,
				customer_sale->transaction->
					transaction_date_time,
				customer_sale->transaction->memo,
				customer_sale->inventory_sale_list,
				customer_sale->specific_inventory_sale_list,
				customer_sale->fixed_service_sale_list,
				customer_sale->hourly_service_sale_list,
				customer_sale->shipping_revenue,
				customer_sale->sales_tax,
				customer_sale->invoice_amount,
				customer_sale->fund_name );

		if ( customer_sale->transaction )
		{
			ledger_transaction_refresh(
				application_name,
				customer_sale->full_name,
				customer_sale->street_address,
				customer_sale->transaction_date_time,
				customer_sale->transaction->transaction_amount,
				customer_sale->transaction->memo,
				0 /* check_number */,
				customer_sale->transaction->lock_transaction,
				customer_sale->
					transaction->
					journal_ledger_list );
		}

	} /* if transaction_date_time */

} /* post_change_inventory_sale_retail_price_update() */

void post_change_inventory_sale_discount_amount_update(
			CUSTOMER_SALE *customer_sale,
			char *inventory_name,
			char *application_name )
{
	INVENTORY_SALE *inventory_sale;

	if ( ! ( inventory_sale =
			inventory_sale_list_seek(
				customer_sale->inventory_sale_list,
				inventory_name ) ) )
	{
		fprintf( stderr,
		 "ERROR in %s/%s()/%d: cannot seek inventory_sale.\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__ );
		exit( 1 );
	}

	inventory_sale->extension =
		inventory_sale_get_extension(
			inventory_sale->retail_price,
			inventory_sale->sold_quantity,
			inventory_sale->discount_amount );

	customer_sale->invoice_amount =
		customer_sale_get_invoice_amount(
			&customer_sale->
				sum_inventory_extension,
			&customer_sale->
				sum_fixed_service_extension,
			&customer_sale->
				sum_hourly_service_extension,
			&customer_sale->sum_extension,
			&customer_sale->sales_tax,
			customer_sale->shipping_revenue,
			customer_sale->
				inventory_sale_list,
			customer_sale->
				specific_inventory_sale_list,
			customer_sale->fixed_service_sale_list,
			customer_sale->hourly_service_sale_list,
			customer_sale->full_name,
			customer_sale->street_address,
			application_name );

	customer_sale->amount_due =
		customer_sale->invoice_amount -
		customer_sale->total_payment;

	customer_sale_update(
		customer_sale->sum_extension,
		customer_sale->database_sum_extension,
		customer_sale->sales_tax,
		customer_sale->database_sales_tax,
		customer_sale->invoice_amount,
		customer_sale->database_invoice_amount,
		customer_sale->completed_date_time,
		customer_sale->
			database_completed_date_time,
		customer_sale->shipped_date_time,
		customer_sale->database_shipped_date_time,
		customer_sale->arrived_date,
		customer_sale->database_arrived_date,
		customer_sale->total_payment,
		customer_sale->database_total_payment,
		customer_sale->amount_due,
		customer_sale->database_amount_due,
		customer_sale->transaction_date_time,
		customer_sale->
			database_transaction_date_time,
		customer_sale->full_name,
		customer_sale->street_address,
		customer_sale->sale_date_time,
		application_name );

	if ( customer_sale->transaction_date_time )
	{
		customer_sale->transaction =
			ledger_customer_sale_build_transaction(
				application_name,
				customer_sale->transaction->full_name,
				customer_sale->transaction->street_address,
				customer_sale->transaction->
					transaction_date_time,
				customer_sale->transaction->memo,
				customer_sale->inventory_sale_list,
				customer_sale->specific_inventory_sale_list,
				customer_sale->fixed_service_sale_list,
				customer_sale->hourly_service_sale_list,
				customer_sale->shipping_revenue,
				customer_sale->sales_tax,
				customer_sale->invoice_amount,
				customer_sale->fund_name );

		if ( customer_sale->transaction )
		{
			ledger_transaction_refresh(
				application_name,
				customer_sale->full_name,
				customer_sale->street_address,
				customer_sale->transaction_date_time,
				customer_sale->transaction->transaction_amount,
				customer_sale->transaction->memo,
				0 /* check_number */,
				customer_sale->transaction->lock_transaction,
				customer_sale->
					transaction->
					journal_ledger_list );
		}

	} /* if transaction_date_time */

} /* post_change_inventory_sale_discount_amount_update() */

void post_change_inventory_sale_inventory_name_update(
			CUSTOMER_SALE *customer_sale,
			char *inventory_name,
			char *preupdate_inventory_name,
			char *application_name )
{
	char sys_string[ 1024 ];
	INVENTORY_SALE *inventory_sale;

	sprintf( sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" '' n",
		 application_name,
		 preupdate_inventory_name );

	system( sys_string );

	if ( ! ( inventory_sale =
			inventory_sale_list_seek(
				customer_sale->inventory_sale_list,
				inventory_name ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek inventory_sale.\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__ );
		exit( 1 );
	}

	sprintf( sys_string,
"propagate_inventory_sale_layers %s \"\" \"\" \"\" \"%s\" '' n",
		 application_name,
		 inventory_name );

	inventory_sale->cost_of_goods_sold =
		atof( pipe2string( sys_string ) );

	/* Propagate ledger accounts */
	/* ------------------------- */
	if ( customer_sale->transaction )
	{
		sprintf(sys_string,
			"propagate_customer_sale_accounts %s \"%s\" \"%s\"",
			application_name,
			"" /* fund_name */,
			"" /* transaction_date_time */ );

		system( sys_string );
	}

} /* post_change_inventory_sale_inventory_name_update() */

void post_change_inventory_sale_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *inventory_name )
{
	CUSTOMER_SALE *customer_sale;
	char sys_string[ 1024 ];

	if ( ! (  customer_sale =
			customer_sale_new(
				application_name,
				full_name,
				street_address,
				sale_date_time ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: customer_sale_new() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return;
	}

	customer_sale_update(
		customer_sale->sum_extension,
		customer_sale->database_sum_extension,
		customer_sale->sales_tax,
		customer_sale->database_sales_tax,
		customer_sale->invoice_amount,
		customer_sale->database_invoice_amount,
		customer_sale->completed_date_time,
		customer_sale->
			database_completed_date_time,
		customer_sale->shipped_date_time,
		customer_sale->database_shipped_date_time,
		customer_sale->arrived_date,
		customer_sale->database_arrived_date,
		customer_sale->total_payment,
		customer_sale->database_total_payment,
		customer_sale->amount_due,
		customer_sale->database_amount_due,
		customer_sale->transaction_date_time,
		customer_sale->
			database_transaction_date_time,
		customer_sale->full_name,
		customer_sale->street_address,
		customer_sale->sale_date_time,
		application_name );

	if ( customer_sale->transaction_date_time )
	{
		customer_sale->transaction =
			ledger_customer_sale_build_transaction(
				application_name,
				customer_sale->transaction->full_name,
				customer_sale->transaction->street_address,
				customer_sale->transaction->
					transaction_date_time,
				customer_sale->transaction->memo,
				customer_sale->inventory_sale_list,
				customer_sale->specific_inventory_sale_list,
				customer_sale->fixed_service_sale_list,
				customer_sale->hourly_service_sale_list,
				customer_sale->shipping_revenue,
				customer_sale->sales_tax,
				customer_sale->invoice_amount,
				customer_sale->fund_name );

		if ( customer_sale->transaction )
		{
			ledger_transaction_refresh(
				application_name,
				customer_sale->full_name,
				customer_sale->street_address,
				customer_sale->transaction_date_time,
				customer_sale->transaction->transaction_amount,
				customer_sale->transaction->memo,
				0 /* check_number */,
				customer_sale->transaction->lock_transaction,
				customer_sale->
					transaction->
					journal_ledger_list );
		}

	} /* if transaction_date_time */

	sprintf( sys_string,
		 "propagate_inventory_sale_layers %s '' '' '' \"%s\" '' n",
		 application_name,
		 inventory_name );

	system( sys_string );

} /* post_change_inventory_sale_delete() */
