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
			purchase->purchase_calculate->invoice_amount );

	if ( strcmp( state, APPASERVER_DELETE_STATE ) != 0 )
	{
		purchase->
			purchase_fetch->
			fixed_asset_purchase_list->
			update_string_list =
				fixed_asset_purchase_list_update_string_list(
					purchase->
						purchase_fetch->
						fixed_asset_purchase_list->
						list );

		purchase->
			purchase_fetch->
			inventory_purchase_list->
			update_string_list =
				inventory_purchase_list_update_string_list(
					purchase->
						purchase_fetch->
						inventory_purchase_list->
						list );

		purchase->
			purchase_fetch->
			specific_inventory_purchase_list->
			update_string_list =
			    specific_inventory_purchase_list_update_string_list(
				purchase->
					purchase_fetch->
					specific_inventory_purchase_list->
					list );

		purchase->update_system_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			purchase_update_system_string(
				PURCHASE_TABLE,
				purchase->purchase_fetch->primary_key_list );

		purchase->update_string_list =
			purchase_update_string_list(
				fund_name,
				full_name,
				contact_key,
				purchase_date_time,
				purchase->
					purchase_fetch->
					predictive_fund_boolean,
				purchase->
					purchase_fetch->
					entity_contact_key_boolean,
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
			purchase->update_system_string,
			purchase->update_string_list,
			purchase->purchase_transaction );

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

char *purchase_update_system_string(
		const char *purchase_table,
		LIST *primary_key_list )
{
	return
	/* -------------------- */
	/* Borrow SALE’s	*/
	/* Returns heap memory	*/
	/* -------------------- */
	sale_update_system_string(
		purchase_table,
		primary_key_list );
}

LIST *purchase_update_string_list(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean,
		double fixed_asset_purchase_list_total,
		double inventory_purchase_list_total,
		double specific_inventory_purchase_list_total,
		double supply_purchase_list_total,
		double service_purchase_list_total,
		double prepaid_asset_purchase_list_total,
		double purchase_return_list_total,
		double purchase_calculate_invoice_amount )
{
	char *primary_data_string;
	char *update_string;
	LIST *list = list_new();

	primary_data_string =
		/* -------------------- */
		/* Borrow SALE’s	*/
		/* Returns heap memory	*/
		/* -------------------- */
		sale_primary_data_string(
			SQL_DELIMITER,
			fund_name,
			full_name,
			contact_key,
			purchase_date_time /* sale_date_time */,
			fund_boolean,
			contact_key_boolean );

	if ( !float_money_virtually_same(
		fixed_asset_purchase_list_total,
		0.0 ) )
	{
		update_string =
			/* ------------------------------------------------ */
			/* Returns heap memory or null (if not set_boolean) */
			/* ------------------------------------------------ */
			sale_update_string(
				SQL_DELIMITER,
				primary_data_string,
				"fixed_asset_total"/* column_name */,
				fixed_asset_purchase_list_total /* money */,
				1 /* set_boolean */ );
		list_set( list, update_string );
	}

	if ( !float_money_virtually_same( inventory_purchase_list_total, 0.0 ) )
	{
		update_string =
			sale_update_string(
				SQL_DELIMITER,
				primary_data_string,
				"inventory_total" /* column_name */,
				inventory_purchase_list_total /* money */,
				1 /* set_boolean */ );
		list_set( list, update_string );
	}

	if ( !float_money_virtually_same(
		specific_inventory_purchase_list_total,
		0.0 ) )
	{
		update_string =
			sale_update_string(
				SQL_DELIMITER,
				primary_data_string,
				"specific_inventory_total" /* column_name */,
				specific_inventory_purchase_list_total
					/* money */,
				1 /* set_boolean */ );
		list_set( list, update_string );
	}

	if ( !float_money_virtually_same( supply_purchase_list_total, 0.0 ) )
	{
		update_string =
			sale_update_string(
				SQL_DELIMITER,
				primary_data_string,
				"supply_total" /* column_name */,
				supply_purchase_list_total /* money */,
				1 /* set_boolean */ );
		list_set( list, update_string );
	}

	if ( !float_money_virtually_same( service_purchase_list_total, 0.0 ) )
	{
		update_string =
			sale_update_string(
				SQL_DELIMITER,
				primary_data_string,
				"service_total" /* column_name */,
				service_purchase_list_total /* money */,
				1 /* set_boolean */ );
		list_set( list, update_string );
	}

	if ( !float_money_virtually_same(
		prepaid_asset_purchase_list_total,
		0.0 ) )
	{
		update_string =
			sale_update_string(
				SQL_DELIMITER,
				primary_data_string,
				"prepaid_asset_total" /* column_name */,
				prepaid_asset_purchase_list_total /* money */,
				1 /* set_boolean */ );
		list_set( list, update_string );
	}

	if ( !float_money_virtually_same( purchase_return_list_total, 0.0 ) )
	{
		update_string =
			sale_update_string(
				SQL_DELIMITER,
				primary_data_string,
				"purchase_return_total" /* column_name */,
				purchase_return_list_total /* money */,
				1 /* set_boolean */ );
		list_set( list, update_string );
	}

	update_string =
		sale_update_string(
			SQL_DELIMITER,
			primary_data_string,
				"invoice_amount" /* column_name */,
				purchase_calculate_invoice_amount /* money */,
				1 /* set_boolean */ );
	list_set( list, update_string );

	return list;
}
