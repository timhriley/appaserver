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
#include "sale.h"
#include "subsidiary_transaction.h"
#include "purchase_update.h"

PURCHASE_UPDATE *purchase_update_new(
		FIXED_ASSET_PURCHASE_LIST *
			fixed_asset_purchase_list,
		INVENTORY_PURCHASE_LIST *
			inventory_purchase_list,
		SPECIFIC_INVENTORY_PURCHASE_LIST *
			specific_inventory_purchase_list,
		SUPPLY_PURCHASE_LIST *
			supply_purchase_list,
		char *purchase_update_system_string,
		LIST *purchase_update_string_list,
		PURCHASE_TRANSACTION *purchase_transaction )
{
	PURCHASE_UPDATE *purchase_update;

	if ( !fixed_asset_purchase_list
	||   !inventory_purchase_list
	||   !specific_inventory_purchase_list
	||   !supply_purchase_list )
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

	purchase_update->purchase_update_system_string =
		purchase_update_system_string;

	purchase_update->purchase_update_string_list =
		purchase_update_string_list;

	purchase_update->purchase_transaction =
		purchase_transaction;

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
		PURCHASE_UPDATE *purchase_update )
{
	char *transaction_date_time = {0};

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

	if ( purchase_update->purchase_update_system_string )
	{
		purchase_update_table_execute(
			purchase_update->purchase_update_string_list,
			purchase_update->purchase_update_system_string );
	}

	if ( purchase_update->purchase_transaction )
	{
		/* ------------------------------------ */
		/* Updates the many table.		*/
		/* Returns transaction_date_time.	*/
		/* ------------------------------------ */
		transaction_date_time =
			subsidiary_transaction_execute(
				application_name,
				purchase_update->
					purchase_transaction->
					subsidiary_transaction->
					delete_transaction,
				purchase_update->
					purchase_transaction->
					subsidiary_transaction->
					insert_transaction,
				purchase_update->
					purchase_transaction->
					subsidiary_transaction->
					update_template,
				purchase_update->
					purchase_transaction->
					subsidiary_transaction->
					update_null_sql,
				purchase_update->
					purchase_transaction->
					subsidiary_transaction->
					predictive_fund_boolean,
				purchase_update->
					purchase_transaction->
					subsidiary_transaction->
					entity_contact_key_boolean );
	}

	return transaction_date_time;
}

void purchase_update_table_execute(
		LIST *update_string_list,
		char *update_system_string )
{
	/* Borrow SALE’s */
	/* ------------- */
	(void)sale_update(
		(char *)0 /* application_name for transaction update */,
		update_string_list,
		update_system_string,
		(SALE_TRANSACTION *)0,
		(SALE_LOSS_TRANSACTION *)0 );
}
