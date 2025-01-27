/* ---------------------------------------------------------------	*/
/* src_predictive/post_change_purchase_order.c				*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "column.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "entity.h"
#include "date.h"
#include "vendor_payment.h"
#include "equipment_purchase.h"
#include "purchase.h"
#include "account.h"
#include "predictive.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_purchase_predelete(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

void post_change_purchase_insert_update(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

int main( int argc, char **argv )
{
	char *application_name = {0};
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *state;
	char *preupdate_full_name;
	char *preupdate_street_address;
	char *preupdate_title_passage_rule;
	char *preupdate_shipped_date;
	char *preupdate_arrived_date_time;
	char *preupdate_sales_tax;
	char *preupdate_freight_in;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 12 )
	{
		fprintf( stderr,
"Usage: %s full_name street_address purchase_date_time state preupdate_full_name preupdate_street_address preupdate_title_passage_rule preupdate_shipped_date preupdate_arrived_date_time preupdate_sales_tax preupdate_freight_in\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	purchase_date_time = argv[ 3 ];
	state = argv[ 4 ];
	if ( ( preupdate_full_name = argv[ 5 ] ) ){};
	if ( ( preupdate_street_address = argv[ 6 ] ) ){};
	if ( ( preupdate_title_passage_rule = argv[ 7 ] ) ){};
	if ( ( preupdate_shipped_date = argv[ 8 ] ) ){};
	if ( ( preupdate_arrived_date_time = argv[ 9 ] ) ){};
	if ( ( preupdate_sales_tax = argv[ 10 ] ) ){};
	if ( ( preupdate_freight_in = argv[ 11 ] ) ){};

	/* -------------------------------------------- */
	/* Only execute state=predelete because we have	*/
	/* PURCHASE_ORDER.transaction_date_time.	*/
	/* -------------------------------------------- */
	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	/* ------------------------------------------------------------ */
	/* If changed ENTITY.full_name or ENTITY.street address,	*/
	/* then this process is executed because of propagation.	*/
	/* ------------------------------------------------------------ */
	if ( strcmp( purchase_date_time, "purchase_date_time" ) == 0 )
		exit( 0 );

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) == 0 )
	{
		post_change_purchase_insert_update(
				full_name,
				street_address,
				purchase_date_time );
	}
	if ( strcmp( state, "predelete" ) == 0 )
	{
		post_change_purchase_predelete(
				full_name,
				street_address,
				purchase_date_time );
	}

	return 0;
}

void post_change_purchase_insert_update(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	PURCHASE *purchase;
	TRANSACTION *transaction;
	char *transaction_date_time = {0};

	if ( ! ( purchase =
			purchase_fetch(
				full_name,
				street_address,
				purchase_date_time ) ) )
	{
		return;
	}

	purchase->purchase_amount_due =
		Purchase_amount_due(
			purchase->purchase_invoice_amount,
			( purchase->purchase_vendor_payment_total =
				vendor_payment_total(
					purchase->
					     purchase_vendor_payment_list ) ) );

	if ( purchase->purchase_transaction )
	{
		if ( !purchase->arrived_date_time
		||   !*purchase->arrived_date_time )
		{
			TRANSACTION *transaction;

			transaction = purchase->purchase_transaction;

			journal_account_name_list_propagate(
				transaction->transaction_date_time,
				/* ------------------------- */
				/* Returns account_name_list */
				/* ------------------------- */
				journal_delete(
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time ) );

			transaction_delete(
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time );

			purchase->purchase_transaction = (TRANSACTION *)0;
			transaction_date_time = (char *)0;
		}
		else
		{
			/* Refresh the TRANSACTION */
			/* ----------------------- */
			purchase->purchase_transaction =
				/* ---------------------------------- */
				/* Includes transaction->journal_list */
				/* ---------------------------------- */
				purchase_transaction(
					purchase->vendor_entity->full_name,
					purchase->vendor_entity->street_address,
					purchase->arrived_date_time,
					purchase->purchase_invoice_amount,
					purchase_asset_account_name(
						purchase->
						     purchase_equipment_list ),
					account_payable( (char *)0 ) );

			transaction = purchase->purchase_transaction;

			transaction_date_time =
			transaction->transaction_date_time =
				transaction_journal_refresh(
					transaction->full_name,
					transaction->street_address,
					transaction->transaction_date_time,
					transaction->transaction_amount,
					transaction->memo,
					transaction->check_number,
					transaction->lock_transaction,
					transaction->journal_list );
		}
	}

	purchase_update(
		purchase->purchase_equipment_total,
		purchase->purchase_invoice_amount,
		purchase->purchase_vendor_payment_total,
		purchase->purchase_amount_due,
		transaction_date_time,
		purchase->vendor_entity->full_name,
		purchase->vendor_entity->street_address,
		purchase->purchase_date_time );
}

void post_change_purchase_predelete(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	PURCHASE *purchase;
	TRANSACTION *transaction;

	if ( ! ( purchase =
			purchase_fetch(
				full_name,
				street_address,
				purchase_date_time ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot find purchase.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	if ( !purchase->purchase_transaction ) return;

	transaction = purchase->purchase_transaction;

	journal_account_name_list_propagate(
		transaction->transaction_date_time,
		/* ------------------------- */
		/* Returns account_name_list */
		/* ------------------------- */
		journal_delete(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time ) );

	transaction_delete(
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time );
}

#ifdef NOT_DEFINED
void post_change_purchase_order_insert(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	if ( purchase_order->title_passage_rule == FOB_shipping )
	{
		post_change_purchase_order_insert_FOB_shipping(
			purchase_order,
			application_name );
	}
	else
	if ( purchase_order->title_passage_rule == FOB_destination )
	{
		post_change_purchase_order_insert_FOB_destination(
			purchase_order,
			application_name );
	}
	else
	{
		post_change_purchase_order_insert_title_passage_null(
			purchase_order,
			application_name );
	}

} /* post_change_purchase_order_insert() */

void post_change_purchase_order_just_arrived(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	inventory_purchase_arrived_quantity_update_with_propagate(
		purchase_order->inventory_purchase_list,
		application_name,
		0 /* don't force_not_latest */ );

	if ( purchase_order->transaction )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: not expecting a transaction for this purchase_order.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	post_change_purchase_order_new_transaction(
			purchase_order,
			purchase_order->arrived_date_time,
			application_name );

} /* post_change_purchase_order_just_arrived() */

void post_change_purchase_order_FOB_shipping_just_arrived(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	/* If it arrived, then it must have been shipped. */
	/* ---------------------------------------------- */
	if ( !purchase_order->shipped_date )
	{
		char buffer[ 16 ];

		if ( purchase_order->transaction )
		{
			fprintf( stderr,
	"ERROR in %s/%s()/%d: A transaction exists without a shipped_date.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		column( buffer, 0, purchase_order->arrived_date_time );
		purchase_order->shipped_date = strdup( buffer );
	}

	inventory_purchase_arrived_quantity_update_with_propagate(
		purchase_order->inventory_purchase_list,
		application_name,
		0 /* don't force_not_latest */ );

	if ( !purchase_order->transaction )
	{
		purchase_order->transaction_date_time =
			predictive_transaction_date_time(
				purchase_order->shipped_date );
	
		purchase_order->transaction =
			ledger_transaction_new(
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				PURCHASE_ORDER_MEMO );
	
		purchase_order->transaction_date_time =
		purchase_order->transaction->transaction_date_time =
		ledger_transaction_insert(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			0.0 /* transaction_amount */,
			purchase_order->transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */ );
	}
}

void post_change_purchase_order_FOB_shipping_just_shipped(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	purchase_order->transaction_date_time =
		predictive_transaction_date_time(
			purchase_order->shipped_date );
	
	purchase_order->transaction =
		ledger_transaction_new(
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			PURCHASE_ORDER_MEMO );

	purchase_order->transaction_date_time =
	purchase_order->transaction->transaction_date_time =
	ledger_transaction_insert(
		application_name,
		purchase_order->transaction->full_name,
		purchase_order->transaction->street_address,
		purchase_order->transaction->transaction_date_time,
		0.0 /* transaction_amount */,
		purchase_order->transaction->memo,
		0 /* check_number */,
		1 /* lock_transaction */ );

} /* post_change_purchase_order_FOB_shipping_just_shipped() */

void post_change_purchase_order_FOB_shipping_mistakenly_shipped(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	purchase_order_transaction_delete_with_propagate(
		application_name,
		purchase_order->fund_name,
		purchase_order->transaction->full_name,
		purchase_order->transaction->street_address,
		purchase_order->transaction->transaction_date_time );

	purchase_order->transaction = (TRANSACTION *)0;
	purchase_order->transaction_date_time = (char *)0;

} /* post_change_purchase_order_FOB_shipping_mistakenly_shipped() */

void post_change_purchase_order_mistakenly_arrived(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	INVENTORY_PURCHASE *inventory_purchase;

	if ( list_rewind( purchase_order->inventory_purchase_list ) )
	{
		do {
			inventory_purchase =
				list_get(
					purchase_order->
						inventory_purchase_list );

			inventory_purchase->arrived_quantity = 0;

			if (	inventory_purchase->arrived_quantity !=
				inventory_purchase->database_arrived_quantity )
			{
				inventory_purchase_arrived_quantity_update(
					application_name,
					inventory_purchase->full_name,
					inventory_purchase->street_address,
					inventory_purchase->purchase_date_time,
					inventory_purchase->inventory_name,
					inventory_purchase->arrived_quantity,
					inventory_purchase->
						database_arrived_quantity );

				inventory_purchase->database_arrived_quantity =
					inventory_purchase->arrived_quantity;
			}

		} while( list_next( purchase_order->inventory_purchase_list ) );

		inventory_purchase_layers_propagate(
			application_name,
			purchase_order->inventory_purchase_list,
			1 /* force_not_latest */ );
	}

	if ( purchase_order->title_passage_rule != FOB_shipping )
	{
		purchase_order_transaction_delete_with_propagate(
			application_name,
			purchase_order->fund_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time );

		purchase_order->transaction_date_time = (char *)0;
		purchase_order->transaction = (TRANSACTION *)0;
	}

	if ( list_length( purchase_order->fixed_asset_purchase_list ) )
	{
/*
		purchase_fixed_asset_list_depreciation_delete(
				purchase_order->fixed_asset_purchase_list,
				application_name,
				purchase_order->fund_name );
*/
	}

} /* post_change_purchase_order_mistakenly_arrived() */

void post_change_purchase_order_FOB_shipping_fixed_shipped_date(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	char *new_transaction_date_time;
	char *propagate_transaction_date_time;
	char sys_string[ 1024 ];

	if ( !purchase_order->transaction )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty purchase_order->transaction.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	new_transaction_date_time =
		predictive_transaction_date_time(
			purchase_order->shipped_date );

	ledger_transaction_generic_update(
		application_name,
		purchase_order->transaction->full_name,
		purchase_order->transaction->street_address,
		purchase_order->transaction->transaction_date_time,
		"transaction_date_time",
		new_transaction_date_time );

	ledger_journal_generic_update(
		application_name,
		purchase_order->transaction->full_name,
		purchase_order->transaction->street_address,
		purchase_order->transaction->transaction_date_time,
		"transaction_date_time",
		new_transaction_date_time );

	if ( strcmp(	purchase_order->transaction_date_time,
			new_transaction_date_time ) < 0 )
	{
		propagate_transaction_date_time =
			purchase_order->transaction_date_time;
	}
	else
	{
		propagate_transaction_date_time = new_transaction_date_time;
	}

	sprintf( sys_string,
		 "propagate_purchase_order_accounts ignored \"%s\" \"%s\"",
		 (purchase_order->fund_name)
			? purchase_order->fund_name
			: "fund",
		 propagate_transaction_date_time );

	if ( system( sys_string ) ) {};

	purchase_order->transaction_date_time =
	purchase_order->transaction->transaction_date_time =
		new_transaction_date_time;

} /* post_change_purchase_order_FOB_shipping_fixed_shipped_date() */

void post_change_purchase_order_insert_FOB_shipping(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	/* If it arrived, then it must have been shipped. */
	/* ---------------------------------------------- */
	if ( purchase_order->arrived_date_time
	&&   !purchase_order->shipped_date )
	{
		char buffer[ 16 ];

		column( buffer, 0, purchase_order->arrived_date_time );
		purchase_order->shipped_date = strdup( buffer );
	}

	/* ---------------------------------------------------- */
	/* For propagate inventory see:				*/
	/* post_change_inventory_purchase_insert_FOB_shipping() */
	/* ---------------------------------------------------- */

	if ( purchase_order->shipped_date )
	{
		purchase_order->transaction_date_time =
			predictive_transaction_date_time(
				purchase_order->shipped_date );

		purchase_order->transaction =
			ledger_transaction_new(
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				PURCHASE_ORDER_MEMO );

		purchase_order->transaction_date_time =
		purchase_order->transaction->transaction_date_time =
		ledger_transaction_insert(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			0.0 /* transaction_amount */,
			purchase_order->transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */ );

		/* ---------------------------------------------------- */
		/* For propagate accounts see:				*/
		/* post_change_inventory_purchase_insert_FOB_shipping() */
		/* ---------------------------------------------------- */
	}

} /* post_change_purchase_order_insert_FOB_shipping() */

void post_change_purchase_order_insert_FOB_destination(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	if ( purchase_order->arrived_date_time )
	{
		purchase_order->transaction_date_time =
			purchase_order->arrived_date_time;

		purchase_order->transaction =
			ledger_transaction_new(
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				PURCHASE_ORDER_MEMO );

		purchase_order->transaction_date_time =
		purchase_order->transaction->transaction_date_time =
		ledger_transaction_insert(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			0.0 /* transaction_amount */,
			purchase_order->transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */ );
	}

} /* post_change_purchase_order_insert_FOB_destination() */

void post_change_purchase_order_insert_title_passage_null(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	if ( !purchase_order->arrived_date_time
	||   !*purchase_order->arrived_date_time )
	{
		return;
	}

	purchase_order->transaction_date_time =
		purchase_order->arrived_date_time;

	purchase_order->transaction =
		ledger_transaction_new(
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			PURCHASE_ORDER_MEMO );

	purchase_order->transaction_date_time =
	purchase_order->transaction->transaction_date_time =
	ledger_transaction_insert(
		application_name,
		purchase_order->transaction->full_name,
		purchase_order->transaction->street_address,
		purchase_order->transaction->transaction_date_time,
		0.0 /* transaction_amount */,
		purchase_order->transaction->memo,
		0 /* check_number */,
		1 /* lock_transaction */ );

} /* post_change_purchase_order_insert_title_passage_null() */

void post_change_purchase_order_update(
			PURCHASE_ORDER *purchase_order,
			char *preupdate_full_name,
			char *preupdate_street_address,
			char *preupdate_title_passage_rule,
			char *preupdate_shipped_date,
			char *preupdate_arrived_date_time,
			char *preupdate_sales_tax,
			char *preupdate_freight_in,
			char *application_name )
{
	enum preupdate_change_state full_name_change_state;
	enum preupdate_change_state street_address_change_state;
	enum preupdate_change_state shipped_date_change_state;
	enum preupdate_change_state title_passage_rule_change_state;
	enum preupdate_change_state arrived_date_time_change_state;
	enum preupdate_change_state sales_tax_change_state;
	enum preupdate_change_state freight_in_change_state;

	full_name_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_full_name,
			purchase_order->full_name,
			"preupdate_full_name" );

	street_address_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_street_address,
			purchase_order->street_address,
			"preupdate_street_address" );

	shipped_date_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_shipped_date,
			(purchase_order->shipped_date)
				? purchase_order->shipped_date
				: "",
			"preupdate_shipped_date" );

	title_passage_rule_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_title_passage_rule,
			entity_get_title_passage_rule_string(
				purchase_order->title_passage_rule ),
			"preupdate_title_passage_rule" );

	arrived_date_time_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_arrived_date_time,
			(purchase_order->arrived_date_time)
				? purchase_order->arrived_date_time
				: "",
			"preupdate_arrived_date_time" );

	sales_tax_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_sales_tax,
			(char *)0 /* postupdate_data */,
			"preupdate_sales_tax" );

	freight_in_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_freight_in,
			(char *)0 /* postupdate_data */,
			"preupdate_freight_in" );

	if ( full_name_change_state == from_something_to_something_else
	||   street_address_change_state == from_something_to_something_else )
	{
		if ( purchase_order->transaction_date_time )
		{
			ledger_entity_update(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				preupdate_full_name,
				preupdate_street_address );
		}
	}

	if ( shipped_date_change_state == from_null_to_something
	&&   purchase_order->title_passage_rule == FOB_shipping )
	{
		post_change_purchase_order_FOB_shipping_just_shipped(
			purchase_order,
			application_name );
	}

	if ( shipped_date_change_state == from_something_to_something_else
	&&   purchase_order->title_passage_rule == FOB_shipping )
	{
		post_change_purchase_order_FOB_shipping_fixed_shipped_date(
			purchase_order,
			application_name );
	}

	if ( shipped_date_change_state == from_something_to_null
	&&   purchase_order->title_passage_rule == FOB_shipping )
	{
		post_change_purchase_order_FOB_shipping_mistakenly_shipped(
			purchase_order,
			application_name );
	}

	if ( arrived_date_time_change_state == from_null_to_something )
	{
		if ( purchase_order->title_passage_rule == FOB_shipping )
		{
			post_change_purchase_order_FOB_shipping_just_arrived(
				purchase_order,
				application_name );
		}
		else
		{
			post_change_purchase_order_just_arrived(
				purchase_order,
				application_name );
		}
	}

	if (	arrived_date_time_change_state ==
		from_something_to_something_else )
	{
		post_change_purchase_order_fixed_arrived_date_time(
			purchase_order,
			application_name );
	}

	if ( arrived_date_time_change_state == from_something_to_null )
	{
		post_change_purchase_order_mistakenly_arrived(
			purchase_order,
			application_name );
	}

	if ( title_passage_rule_change_state == from_null_to_something
	&&   purchase_order->title_passage_rule == FOB_shipping )
	{
		post_change_purchase_order_FOB_shipping_new_title_passage_rule(
			purchase_order,
			application_name );
	}

	if (	title_passage_rule_change_state ==
		from_something_to_something_else )
	{
		if ( purchase_order->title_passage_rule == FOB_shipping )
		{
			post_change_purchase_order_changed_to_FOB_shipping(
				purchase_order,
				application_name );
		}
		else
		{
			post_change_purchase_order_changed_to_FOB_destination(
				purchase_order,
				application_name );
		}
	}

	if ( title_passage_rule_change_state == from_something_to_null )
	{
		post_change_purchase_order_changed_rule_to_null(
			purchase_order,
			application_name );
	}

	/* Note: numbers can't know if from_something_to_null */
	/* -------------------------------------------------- */
	if ( sales_tax_change_state == from_something_to_something_else
	||   sales_tax_change_state == from_null_to_something
	||   freight_in_change_state == from_something_to_something_else
	||   freight_in_change_state == from_null_to_something )
	{
		post_change_purchase_order_change_sales_tax_transaction(
			purchase_order,
			application_name );

		if ( list_length( purchase_order->inventory_purchase_list ) )
		{
			post_change_purchase_order_change_sales_tax_inventory(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->purchase_date_time,
				purchase_order->inventory_purchase_list );
		}
	}

} /* post_change_purchase_order_update() */

void post_change_purchase_order_changed_rule_to_null(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	char *transaction_date_time;

	if ( !purchase_order->arrived_date_time )
	{
		purchase_order->arrived_date_time =
			purchase_order->purchase_date_time;
	}

	if ( purchase_order->transaction_date_time )
	{
		char *propagate_transaction_date_time;

		transaction_date_time =
			purchase_order->arrived_date_time;

		ledger_transaction_generic_update(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->
				transaction->
				transaction_date_time,
			"transaction_date_time",
			transaction_date_time );

		ledger_journal_generic_update(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->
				transaction->
				transaction_date_time,
			"transaction_date_time",
			transaction_date_time );

		if ( strcmp(	purchase_order->transaction_date_time,
				transaction_date_time ) < 0 )
		{
			propagate_transaction_date_time =
				purchase_order->transaction_date_time;
		}
		else
		{
			propagate_transaction_date_time =
				transaction_date_time;
		}

		entity_propagate_purchase_order_ledger_accounts(
			application_name,
			purchase_order->fund_name,
		 	propagate_transaction_date_time );

		purchase_order->transaction_date_time =
		purchase_order->transaction->transaction_date_time =
			transaction_date_time;
	}
	else
	{
		char *memo;

		if ( purchase_order->transaction )
			memo = purchase_order->transaction->memo;
		else
			memo = PURCHASE_ORDER_MEMO;

		purchase_order->transaction_date_time =
			purchase_order->arrived_date_time;

		if ( list_length( purchase_order->inventory_purchase_list ) )
		{
			purchase_order->transaction =
			     purchase_inventory_build_transaction(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				memo,
				purchase_order->inventory_purchase_list,
				purchase_order->fund_name );
		}
		else
		if ( list_length( purchase_order->
					specific_inventory_purchase_list ) )
		{
			purchase_order->transaction =
			     purchase_specific_inventory_build_transaction(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				memo,
				purchase_order->
					specific_inventory_purchase_list,
				purchase_order->fund_name );
		}
		else
		{
			purchase_order->transaction =
				purchase_order_build_transaction(
					application_name,
					purchase_order->full_name,
					purchase_order->street_address,
					purchase_order->transaction_date_time,
					memo,
					purchase_order->sales_tax,
					purchase_order->freight_in,
					purchase_order->supply_purchase_list,
					purchase_order->service_purchase_list,
					purchase_order->
					     fixed_asset_purchase_list,
					purchase_order->
						prepaid_asset_purchase_list,
					purchase_order->fund_name );
		}

		ledger_transaction_refresh(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			purchase_order->transaction->transaction_amount,
			purchase_order->transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */,
			purchase_order->transaction->journal_ledger_list );
	}

	if ( purchase_order->shipped_date )
	{
		purchase_order->shipped_date = (char *)0;
	}

} /* post_change_purchase_order_changed_rule_to_null() */

void post_change_purchase_order_changed_to_FOB_destination(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	char *transaction_date_time;

	if ( purchase_order->transaction )
	{
		if ( purchase_order->arrived_date_time )
		{
			char *propagate_transaction_date_time;

			transaction_date_time =
				purchase_order->arrived_date_time;

			ledger_transaction_generic_update(
				application_name,
				purchase_order->transaction->full_name,
				purchase_order->transaction->street_address,
				purchase_order->
					transaction->
					transaction_date_time,
				"transaction_date_time",
				transaction_date_time );

			ledger_journal_generic_update(
				application_name,
				purchase_order->transaction->full_name,
				purchase_order->transaction->street_address,
				purchase_order->
					transaction->
					transaction_date_time,
				"transaction_date_time",
				transaction_date_time );

			if ( strcmp(	purchase_order->transaction_date_time,
					transaction_date_time ) < 0 )
			{
				propagate_transaction_date_time =
					purchase_order->transaction_date_time;
			}
			else
			{
				propagate_transaction_date_time =
					transaction_date_time;
			}

			entity_propagate_purchase_order_ledger_accounts(
				application_name,
				purchase_order->fund_name,
		 		propagate_transaction_date_time );

			purchase_order->transaction_date_time =
			purchase_order->transaction->transaction_date_time =
				transaction_date_time;
		}
		else
		{
			purchase_order_transaction_delete_with_propagate(
				application_name,
				purchase_order->fund_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time );

			purchase_order->transaction = (TRANSACTION *)0;
			purchase_order->transaction_date_time = (char *)0;
		}
	}

} /* post_change_purchase_order_changed_to_FOB_destination() */

void post_change_purchase_order_changed_to_FOB_shipping(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	char *transaction_date_time;

	if ( purchase_order->transaction )
	{
		char *propagate_transaction_date_time;
		char sys_string[ 1024 ];

		transaction_date_time =
			predictive_transaction_date_time(
				purchase_order->shipped_date );

		ledger_transaction_generic_update(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			"transaction_date_time",
			transaction_date_time );

		ledger_journal_generic_update(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			"transaction_date_time",
			transaction_date_time );

		if ( strcmp( purchase_order->transaction_date_time,
				transaction_date_time ) < 0 )
		{
			propagate_transaction_date_time =
				purchase_order->transaction_date_time;
		}
		else
		{
			propagate_transaction_date_time =
				transaction_date_time;
		}

		sprintf(sys_string,
		 "propagate_purchase_order_accounts ignored \"%s\" \"%s\"",
			(purchase_order->fund_name)
				? purchase_order->fund_name
				: "fund",
		 	propagate_transaction_date_time );

		if ( system( sys_string ) ) {};

		purchase_order->transaction_date_time =
		purchase_order->transaction->transaction_date_time =
			transaction_date_time;
	}
	else
	if ( purchase_order->shipped_date
	&&   *purchase_order->shipped_date )
	{
		char *memo;

		if ( purchase_order->transaction )
			memo = purchase_order->transaction->memo;
		else
			memo = PURCHASE_ORDER_MEMO;

		purchase_order->transaction_date_time =
			predictive_transaction_date_time(
				purchase_order->shipped_date );

		if ( list_length( purchase_order->inventory_purchase_list ) )
		{
			purchase_order->transaction =
			     purchase_inventory_build_transaction(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				memo,
				purchase_order->inventory_purchase_list,
				purchase_order->fund_name );
		}
		else
		if ( list_length( purchase_order->
					specific_inventory_purchase_list ) )
		{
			purchase_order->transaction =
			     purchase_specific_inventory_build_transaction(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				memo,
				purchase_order->
					specific_inventory_purchase_list,
				purchase_order->fund_name );
		}
		else
		{
			purchase_order->transaction =
				purchase_order_build_transaction(
					application_name,
					purchase_order->full_name,
					purchase_order->street_address,
					purchase_order->transaction_date_time,
					memo,
					purchase_order->sales_tax,
					purchase_order->freight_in,
					purchase_order->supply_purchase_list,
					purchase_order->service_purchase_list,
					purchase_order->
					     fixed_asset_purchase_list,
					purchase_order->
						prepaid_asset_purchase_list,
					purchase_order->fund_name );
		}

		ledger_transaction_refresh(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			purchase_order->transaction->transaction_amount,
			purchase_order->transaction->memo,
			0 /* check_number */,
			1 /* lock_transaction */,
			purchase_order->transaction->journal_ledger_list );
	}

} /* post_change_purchase_order_changed_to_FOB_shipping() */

void post_change_purchase_order_FOB_shipping_new_title_passage_rule(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	char *propagate_transaction_date_time;
	char sys_string[ 1024 ];

	if ( !purchase_order->shipped_date )
	{
		char shipped_date[ 16 ];

		if ( !purchase_order->arrived_date_time )
		{
			purchase_order->arrived_date_time =
				purchase_order->purchase_date_time;
		}

		column( shipped_date, 0, purchase_order->arrived_date_time );

		purchase_order->shipped_date = strdup( shipped_date );
	}

	purchase_order->transaction_date_time =
		predictive_transaction_date_time(
			purchase_order->shipped_date );

	if ( purchase_order->transaction )
	{
		ledger_transaction_generic_update(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			"transaction_date_time",
			purchase_order->transaction_date_time );
	
		ledger_journal_generic_update(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			"transaction_date_time",
			purchase_order->transaction_date_time );
	
		if ( strcmp( purchase_order->
				transaction->
				transaction_date_time,
				purchase_order->transaction_date_time ) < 0 )
		{
			propagate_transaction_date_time =
				purchase_order->
					transaction->
					transaction_date_time;
		}
		else
		{
			propagate_transaction_date_time =
				purchase_order->transaction_date_time;
		}

		sprintf(sys_string,
		 "propagate_purchase_order_accounts ignored \"%s\" \"%s\"",
			(purchase_order->fund_name)
				? purchase_order->fund_name
				: "fund",
		 	propagate_transaction_date_time );

		if ( system( sys_string ) ) {};

		purchase_order->transaction->transaction_date_time =
			purchase_order->transaction_date_time;
	}
	else
	{
		post_change_purchase_order_new_transaction(
			purchase_order,
			purchase_order->transaction_date_time,
			application_name );
	}

} /* post_change_purchase_order_FOB_shipping_new_title_passage_rule() */

void post_change_purchase_order_fixed_arrived_date_time(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	inventory_purchase_arrived_quantity_update_with_propagate(
		purchase_order->inventory_purchase_list,
		application_name,
		1 /* force_not_latest */ );

	if ( purchase_order->title_passage_rule != FOB_shipping )
	{
		char *propagate_transaction_date_time;
		char sys_string[ 1024 ];

		if ( !purchase_order->transaction )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: empty purchase_order->transaction.\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__ );
			exit( 1 );
		}

		ledger_transaction_generic_update(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			"transaction_date_time",
			purchase_order->arrived_date_time );

		ledger_journal_generic_update(
			application_name,
			purchase_order->transaction->full_name,
			purchase_order->transaction->street_address,
			purchase_order->transaction->transaction_date_time,
			"transaction_date_time",
			purchase_order->arrived_date_time );

		if ( strcmp(	purchase_order->transaction_date_time,
				purchase_order->arrived_date_time ) < 0 )
		{
			propagate_transaction_date_time =
				purchase_order->transaction_date_time;
		}
		else
		{
			propagate_transaction_date_time =
				purchase_order->arrived_date_time;
		}

		sprintf(sys_string,
		 "propagate_purchase_order_accounts ignored \"%s\" \"%s\"",
			(purchase_order->fund_name)
				? purchase_order->fund_name
				: "fund",
		 	propagate_transaction_date_time );

		if ( system( sys_string ) ) {};

		purchase_order->transaction_date_time =
		purchase_order->transaction->transaction_date_time =
			purchase_order->arrived_date_time;
	}

	if ( list_length( purchase_order->fixed_asset_purchase_list ) )
	{
/*
		purchase_fixed_asset_list_depreciation_method_update(
				purchase_order->fixed_asset_purchase_list,
				purchase_order->arrived_date_time,
				application_name,
				purchase_order->fund_name );
*/
	}

} /* post_change_purchase_order_fixed_arrived_date_time() */

void post_change_purchase_order_new_transaction(
			PURCHASE_ORDER *purchase_order,
			char *transaction_date_time,
			char *application_name )
{
	purchase_order->transaction_date_time = transaction_date_time;

	if ( list_length( purchase_order->inventory_purchase_list ) )
	{
		purchase_order->transaction =
		     purchase_inventory_build_transaction(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			PURCHASE_ORDER_MEMO,
			purchase_order->inventory_purchase_list,
			purchase_order->fund_name );
	}
	else
	if ( list_length( purchase_order->
				specific_inventory_purchase_list ) )
	{
		purchase_order->transaction =
		     purchase_specific_inventory_build_transaction(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			PURCHASE_ORDER_MEMO,
			purchase_order->
				specific_inventory_purchase_list,
			purchase_order->fund_name );
	}
	else
	{
		purchase_order->transaction =
			purchase_order_build_transaction(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				PURCHASE_ORDER_MEMO,
				purchase_order->sales_tax,
				purchase_order->freight_in,
				purchase_order->supply_purchase_list,
				purchase_order->service_purchase_list,
				purchase_order->
				     fixed_asset_purchase_list,
				purchase_order->
					prepaid_asset_purchase_list,
				purchase_order->fund_name );
	}

	ledger_transaction_refresh(
		application_name,
		purchase_order->full_name,
		purchase_order->street_address,
		purchase_order->transaction_date_time,
		purchase_order->transaction->transaction_amount,
		purchase_order->transaction->memo,
		0 /* check_number */,
		1 /* lock_transaction */,
		purchase_order->transaction->journal_ledger_list );

} /* post_change_purchase_order_new_transaction() */

void post_change_purchase_order_change_sales_tax_transaction(
			PURCHASE_ORDER *purchase_order,
			char *application_name )
{
	if ( !purchase_order->transaction_date_time ) return;

	if ( list_length( purchase_order->inventory_purchase_list ) )
	{
		purchase_order->transaction =
		     purchase_inventory_build_transaction(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			purchase_order->transaction->memo,
			purchase_order->inventory_purchase_list,
			purchase_order->fund_name );
	}
	else
	if ( list_length( purchase_order->
				specific_inventory_purchase_list ) )
	{
		purchase_order->transaction =
		     purchase_specific_inventory_build_transaction(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->transaction_date_time,
			purchase_order->transaction->memo,
			purchase_order->
				specific_inventory_purchase_list,
			purchase_order->fund_name );
	}
	else
	{
		purchase_order->transaction =
			purchase_order_build_transaction(
				application_name,
				purchase_order->full_name,
				purchase_order->street_address,
				purchase_order->transaction_date_time,
				purchase_order->transaction->memo,
				purchase_order->sales_tax,
				purchase_order->freight_in,
				purchase_order->supply_purchase_list,
				purchase_order->service_purchase_list,
				purchase_order->
				     fixed_asset_purchase_list,
				purchase_order->
					prepaid_asset_purchase_list,
				purchase_order->fund_name );
	}

	ledger_transaction_refresh(
		application_name,
		purchase_order->full_name,
		purchase_order->street_address,
		purchase_order->transaction_date_time,
		purchase_order->transaction->transaction_amount,
		purchase_order->transaction->memo,
		0 /* check_number */,
		1 /* lock_transaction */,
		purchase_order->transaction->journal_ledger_list );

} /* post_change_purchase_order_change_sales_tax_transaction() */

void post_change_purchase_order_change_sales_tax_inventory(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			LIST *inventory_purchase_list )
{
	char sys_string[ 1024 ];
	INVENTORY_PURCHASE *inventory_purchase;

	if ( !list_rewind( inventory_purchase_list ) ) return;

	do {
		inventory_purchase =
			list_get_pointer(
				inventory_purchase_list );

		sprintf( sys_string,
		"propagate_inventory_purchase_layers %s '%s' '%s' '%s' '%s' n",
			 application_name,
			 full_name,
			 street_address,
			 purchase_date_time,
			 inventory_purchase->inventory_name );

		if ( system( sys_string ) ) {};

	} while( list_next( inventory_purchase_list ) );

} /* post_change_purchase_order_change_sales_tax_inventory() */
#endif
