/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/purchase.c				*/
/* -------------------------------------------------------------------- */
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "list.h"
#include "sql.h"
#include "float.h"
#include "piece.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "environ.h"
#include "folder.h"
#include "transaction.h"
#include "journal.h"
#include "entity.h"
#include "inventory_purchase.h"
#include "specific_inventory_purchase.h"
#include "fixed_asset_purchase.h"
#include "supply_purchase.h"
#include "prepaid_asset_purchase.h"
#include "predictive.h"
#include "sale.h"
#include "purchase_transaction.h"
#include "purchase.h"

PURCHASE *purchase_trigger_new(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *state,
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key )
{
	PURCHASE *purchase;

	if ( !full_name
	||   !purchase_date_time )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	purchase = purchase_calloc();

	purchase->purchase_fetch =
		purchase_fetch_new(
			PURCHASE_SELECT,
			PURCHASE_TABLE,
			fund_name,
			full_name,
			contact_key,
			purchase_date_time );

	if ( !purchase->purchase_fetch ) return NULL;

	purchase->fixed_asset_purchase_total =
		fixed_asset_purchase_total(
			purchase->
				purchase_fetch->
				fixed_asset_purchase_list );

	if ( purchase->purchase_fetch->inventory_total_boolean )
	{
		purchase->inventory_purchase_total =
			inventory_purchase_total(
				purchase->
					purchase_fetch->
					inventory_purchase_list );
	}

	if ( purchase->
		purchase_fetch->
		specific_inventory_total_boolean )
	{
		purchase->specific_inventory_purchase_total =
			specific_inventory_purchase_total(
				purchase->
					purchase_fetch->
					specific_inventory_purchase_list );
	}

	purchase->supply_purchase_total =
		supply_purchase_total(
			purchase->
				purchase_fetch->
				supply_purchase_list );

/*
	purchase->service_purchase_total =
		service_purchase_total(
			purchase->
				purchase_fetch->
				service_purchase_list );
*/

/*
	if ( purchase->purchase_fetch->prepaid_asset_total_boolean )
	{
		purchase->prepaid_asset_purchase_total =
			prepaid_asset_purchase_total(
				purchase->
					purchase_fetch->
					prepaid_asset_purchase_list );
	}
*/

/*
	if ( purchase->purchase_fetch->return_total_boolean )
	{
		purchase->return_total =
			purchase_return_total(
				purchase->
					purchase_fetch->
					purchase_return_list );
	}
*/

	purchase->total =
		purchase_total(
			purchase->fixed_asset_purchase_total,
			purchase->inventory_purchase_total,
			purchase->specific_inventory_purchase_total,
			purchase->supply_purchase_total,
			purchase->service_purchase_total,
			purchase->prepaid_asset_purchase_total );

	purchase->invoice_amount =
		purchase_invoice_amount(
			purchase->purchase_fetch->sales_tax,
			purchase->purchase_fetch->freight_in,
			purchase->return_total,
			purchase->total );

	purchase->purchase_transaction =
		purchase_transaction_new(
			fund_name,
			full_name,
			contact_key,
			state,
			preupdate_fund_name,
			preupdate_full_name,
			preupdate_contact_key,
			purchase->purchase_fetch->predictive_fund_boolean,
			purchase->purchase_fetch->entity_contact_key_boolean,
			purchase->purchase_fetch->predictive_title_passage_rule,
			purchase->purchase_fetch->shipped_date,
			purchase->purchase_fetch->arrived_date_time,
			purchase->purchase_fetch->transaction_date_time
				/* prior_transaction_date_time */,
			purchase->purchase_fetch->sales_tax,
			purchase->purchase_fetch->freight_in,
			purchase->fixed_asset_purchase_total,
			purchase->inventory_purchase_total,
			purchase->specific_inventory_purchase_total,
			purchase->supply_purchase_total,
			purchase->service_purchase_total,
			purchase->prepaid_asset_purchase_total,
			purchase->invoice_amount );

	return purchase;
}

PURCHASE *purchase_calloc( void )
{
	PURCHASE *purchase;

	if ( ! ( purchase = calloc( 1, sizeof( PURCHASE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return purchase;
}

char *purchase_primary_where(
		const char *purchase_date_time_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	return
	/* --------------------- */
	/* Returns static memory */
	/* --------------------- */
	sale_primary_where(
		purchase_date_time_column
			/* SALE_DATE_TIME_COLUMN */,
		fund_name,
		full_name,
		contact_key,
		purchase_date_time /* sale_date_time */,
		fund_boolean,
		contact_key_boolean );
}

double purchase_cost_basis_total(
		double fixed_asset_purchase_total,
		double inventory_purchase_total,
		double specific_inventory_purchase_total,
		double supply_purchase_total )
{
	return
	fixed_asset_purchase_total +
	inventory_purchase_total +
	specific_inventory_purchase_total +
	supply_purchase_total;
}

double purchase_total(
		double fixed_asset_purchase_total,
		double inventory_purchase_total,
		double specific_inventory_purchase_total,
		double supply_purchase_total,
		double service_purchase_total,
		double prepaid_asset_purchase_total )
{
	return
	fixed_asset_purchase_total +
	inventory_purchase_total +
	specific_inventory_purchase_total +
	supply_purchase_total +
	service_purchase_total +
	prepaid_asset_purchase_total;
}

double purchase_invoice_amount(
		double sales_tax,
		double freight_in,
		double purchase_return_total,
		double purchase_total )
{
	return
	sales_tax + freight_in - purchase_return_total + purchase_total;
}

