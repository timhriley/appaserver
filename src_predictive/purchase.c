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
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *state )
{
	PURCHASE *purchase;

	if ( !full_name
	||   !*full_name
	||   !purchase_date_time
	||   !*purchase_date_time )
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

	purchase->purchase_calculate =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		purchase_calculate_new(
			purchase->purchase_fetch->sales_tax,
			purchase->purchase_fetch->freight_in,
			purchase->
				purchase_fetch->
				fixed_asset_purchase_list->
				list /* Sets each cost_basis_fixed_asset */,
			purchase->
				purchase_fetch->
				inventory_total_boolean,
			purchase->
				purchase_fetch->
				inventory_purchase_list->
				list /* Sets each cost_basis_inventory */,
			purchase->
				purchase_fetch->
				specific_inventory_total_boolean,
			purchase->
				purchase_fetch->
				specific_inventory_purchase_list->
				list
				/* Sets each cost_basis_specific_inventory */,
			purchase->purchase_fetch->supply_purchase_list->list,
			purchase->purchase_fetch->service_purchase_list,
			purchase->purchase_fetch->prepaid_asset_total_boolean,
			purchase->purchase_fetch->prepaid_asset_purchase_list,
			purchase->purchase_fetch->return_total_boolean,
			purchase->purchase_fetch->purchase_return_list );

	if ( purchase->purchase_calculate->cost_basis )
	{
		purchase->purchase_transaction =
			purchase_transaction_new(
				preupdate_fund_name,
				preupdate_full_name,
				preupdate_contact_key,
				fund_name,
				full_name,
				contact_key,
				purchase_date_time,
				state,
				purchase->
					purchase_fetch->
					predictive_fund_boolean,
				purchase->
					purchase_fetch->
					entity_contact_key_boolean,
				purchase->
					purchase_fetch->
					predictive_title_passage_rule,
				purchase->
					purchase_fetch->
					shipped_date,
				purchase->
					purchase_fetch->
					arrived_date_time_boolean,
				purchase->purchase_fetch->arrived_date_time,
				purchase->purchase_fetch->transaction_date_time
					/* prior_transaction_date_time */,
				purchase->
					purchase_calculate->
					cost_basis->
					sales_tax_expense,
				purchase->
					purchase_calculate->
					cost_basis->
					freight_in_expense,
				purchase->
					purchase_calculate->
					cost_basis->
					cost_basis_fixed_asset_total,
				purchase->
					purchase_calculate->
					cost_basis->
					cost_basis_inventory_total,
				purchase->
					purchase_calculate->
					cost_basis->
					cost_basis_specific_inventory_total,
				purchase->
					purchase_calculate->
					supply_purchase_list_total,
				purchase->
					purchase_calculate->
					service_purchase_list_total,
				purchase->
					purchase_calculate->
					prepaid_asset_purchase_list_total,
				purchase->
					purchase_calculate->
					return_list_total,
				purchase->purchase_calculate->invoice_amount );
	}

	purchase->purchase_update =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		purchase_update_new(
			SQL_DELIMITER,
			fund_name,
			full_name,
			contact_key,
			purchase_date_time,
			state,
			purchase->purchase_fetch->predictive_fund_boolean,
			purchase->purchase_fetch->entity_contact_key_boolean,
			purchase->
				purchase_fetch->
				fixed_asset_purchase_list
				/* Sets each update_string_list */,
			purchase->
				purchase_fetch->
				inventory_purchase_list
				/* Sets each update_string_list */,
			purchase->
				purchase_fetch->
				specific_inventory_purchase_list,
			purchase->purchase_fetch->supply_purchase_list
				/* Sets each update_string_list */,
			purchase->purchase_fetch->primary_key_list,
			purchase->
				purchase_calculate->
				fixed_asset_purchase_list_total,
			purchase->
				purchase_calculate->
				inventory_purchase_list_total,
			purchase->
				purchase_calculate->
				specific_inventory_purchase_list_total,
			purchase->
				purchase_calculate->
				supply_purchase_list_total,
			purchase->
				purchase_calculate->
				service_purchase_list_total,
			purchase->
				purchase_calculate->
				prepaid_asset_purchase_list_total,
			purchase->
				purchase_calculate->
				return_list_total,
			purchase->
				purchase_calculate->
				invoice_amount );

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

