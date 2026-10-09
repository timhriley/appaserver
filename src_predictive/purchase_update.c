/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/purchase_update.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "sql.h"
#include "float.h"
#include "update.h"
#include "sale.h"
#include "purchase.h"
#include "subsidiary_transaction.h"
#include "purchase_update.h"

PURCHASE_UPDATE *purchase_update_new(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *state,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		FIXED_ASSET_PURCHASE_LIST *
			fixed_asset_purchase_list
				/* Sets each update_string_list */,
		INVENTORY_PURCHASE_LIST *
			inventory_purchase_list
				/* Sets each update_string_list */,
		SPECIFIC_INVENTORY_PURCHASE_LIST *
			specific_inventory_purchase_list
				/* Sets each update_string_list */,
		SUPPLY_PURCHASE_LIST *
			supply_purchase_list
				/* Sets each update_string_list */,
		LIST *primary_key_list,
		double fixed_asset_purchase_list_total,
		double inventory_purchase_list_total,
		double specific_inventory_purchase_list_total,
		double supply_purchase_list_total,
		double service_purchase_list_total,
		double prepaid_asset_purchase_list_total,
		double purchase_return_list_total,
		double purchase_calculate_invoice_amount )
{
	PURCHASE_UPDATE *purchase_update;

	if ( !fixed_asset_purchase_list
	||   !inventory_purchase_list
	||   !specific_inventory_purchase_list
	||   !supply_purchase_list
	||   !state )
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

	purchase_update = purchase_update_calloc();

	purchase_update->fixed_asset_purchase_list =
		fixed_asset_purchase_list;

	purchase_update->inventory_purchase_list =
		inventory_purchase_list;

	purchase_update->specific_inventory_purchase_list =
		specific_inventory_purchase_list;

	purchase_update->supply_purchase_list =
		supply_purchase_list;

	if ( fixed_asset_purchase_list->list )
	{
		fixed_asset_purchase_list_set_update_string(
			sql_delimiter,
			fund_name,
			full_name,
			contact_key,
			purchase_date_time,
			predictive_fund_boolean,
			entity_contact_key_boolean,
			fixed_asset_purchase_list->list
				/* Set each update_string_list */ );

		fixed_asset_purchase_list->update_string_list =
			fixed_asset_purchase_list_update_string_list(
				fixed_asset_purchase_list->list );
	}

	if ( inventory_purchase_list->list )
	{
		/* Need to set it here b/c cost basis was calculated */
		/* ------------------------------------------------- */
		inventory_purchase_list_set_update_string_list(
			sql_delimiter,
			fund_name,
			full_name,
			contact_key,
			purchase_date_time,
			predictive_fund_boolean,
			entity_contact_key_boolean,
			inventory_purchase_list->list
				/* Set each update_string_list */ );

		inventory_purchase_list->update_string_list =
			inventory_purchase_list_update_string_list(
				inventory_purchase_list->list );
	}

	if ( specific_inventory_purchase_list->list )
	{
		specific_inventory_purchase_list_set_update_string(
			sql_delimiter,
			fund_name,
			full_name,
			contact_key,
			purchase_date_time,
			predictive_fund_boolean,
			entity_contact_key_boolean,
			specific_inventory_purchase_list->list
				/* Set each update_string */ );

		specific_inventory_purchase_list->update_string_list =
			specific_inventory_purchase_list_update_string_list(
				specific_inventory_purchase_list->list );
	}

	/* Note: don't need to set each supply update_string */

	if ( strcmp( state, APPASERVER_DELETE_STATE ) != 0 )
	{
		purchase_update->system_string =
			/* Returns heap memory */
			/* ------------------- */
			purchase_update_system_string(
				PURCHASE_TABLE,
				primary_key_list );

		purchase_update->string_list =
			purchase_update_string_list(
				fund_name,
				full_name,
				contact_key,
				purchase_date_time,
				predictive_fund_boolean,
				entity_contact_key_boolean,
				fixed_asset_purchase_list_total,
				inventory_purchase_list_total,
				specific_inventory_purchase_list_total,
				supply_purchase_list_total,
				service_purchase_list_total,
				prepaid_asset_purchase_list_total,
				purchase_return_list_total,
				purchase_calculate_invoice_amount );
	}

	return purchase_update;
}

PURCHASE_UPDATE *purchase_update_calloc( void )
{
	PURCHASE_UPDATE *purchase_update;

	if ( ! ( purchase_update = calloc( 1, sizeof ( PURCHASE_UPDATE ) ) ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return purchase_update;
}

char *purchase_update_execute(
		char *application_name,
		PURCHASE_TRANSACTION *purchase_transaction,
		PURCHASE_UPDATE *purchase_update )
{
	char *transaction_date_time = {0};

	if ( purchase_transaction )
	{
		/* ------------------------------------ */
		/* Updates the many table.		*/
		/* Returns transaction_date_time.	*/
		/* ------------------------------------ */
		transaction_date_time =
			subsidiary_transaction_execute(
				application_name,
				purchase_transaction->
					subsidiary_transaction->
					delete_transaction,
				purchase_transaction->
					subsidiary_transaction->
					insert_transaction,
				purchase_transaction->
					subsidiary_transaction->
					update_template,
				purchase_transaction->
					subsidiary_transaction->
					update_null_sql,
				purchase_transaction->
					subsidiary_transaction->
					predictive_fund_boolean,
				purchase_transaction->
					subsidiary_transaction->
					entity_contact_key_boolean );
	}

	if ( purchase_update->
		fixed_asset_purchase_list->
		update_system_string )
	{
		purchase_update_table_execute(
			purchase_update->
				fixed_asset_purchase_list->
				update_string_list,
			purchase_update->
				fixed_asset_purchase_list->
				update_system_string );
	}

	if ( purchase_update->
		inventory_purchase_list->
		update_system_string )
	{
		purchase_update_table_execute(
			purchase_update->
				inventory_purchase_list->
				update_string_list,
			purchase_update->
				inventory_purchase_list->
				update_system_string );

		if ( list_rewind(
			purchase_update->
				inventory_purchase_list->
				list ) )
		{
			INVENTORY_PURCHASE *inventory_purchase;

			inventory_purchase =
				list_get(
					purchase_update->
						inventory_purchase_list->
						list );

			inventory_average_purchase_save(
				inventory_purchase->inventory_name,
				inventory_purchase->inventory_average_purchase
				/* In/out sets inventory_average_list */ );
		}
	}

	if ( purchase_update->
		specific_inventory_purchase_list->
		update_system_string )
	{
		purchase_update_table_execute(
			purchase_update->
				specific_inventory_purchase_list->
				update_string_list,
			purchase_update->
				specific_inventory_purchase_list->
				update_system_string );
	}

	if ( purchase_update->
		supply_purchase_list->
		update_system_string )
	{
		purchase_update_table_execute(
			purchase_update->
				supply_purchase_list->
				update_string_list,
			purchase_update->
				supply_purchase_list->
				update_system_string );
	}

	if ( purchase_update->system_string )
	{
		purchase_update_table_execute(
			purchase_update->string_list,
			purchase_update->system_string );
	}

	return transaction_date_time;
}

void purchase_update_table_execute(
		LIST *update_string_list,
		char *update_system_string )
{
	update_string_list_execute(
		update_system_string,
		update_string_list );
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

	if ( !float_money_virtually_zero( fixed_asset_purchase_list_total ) )
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

	if ( !float_money_virtually_zero( inventory_purchase_list_total ) )
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

	if ( !float_money_virtually_zero(
		specific_inventory_purchase_list_total ) )
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

	if ( !float_money_virtually_zero( supply_purchase_list_total ) )
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

	if ( !float_money_virtually_zero( service_purchase_list_total ) )
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

	if ( !float_money_virtually_zero( prepaid_asset_purchase_list_total ) )
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

	if ( !float_money_virtually_zero( purchase_return_list_total ) )
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
