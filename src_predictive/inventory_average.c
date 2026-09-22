/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_average.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "entity.h"
#include "predictive.h"
#include "appaserver_error.h"
#include "inventory.h"
#include "sale.h"
#include "purchase.h"
#include "inventory_purchase.h"
#include "inventory_sale.h"
#include "inventory_balance.h"
#include "inventory_average_cost.h"
#include "inventory_average.h"

INVENTORY_AVERAGE *inventory_average_new(
		char *inventory_name,
		char *purchase_date_time,
		char *sale_date_time )
{
	INVENTORY_AVERAGE *inventory_average;
	INVENTORY_PURCHASE_LIST *inventory_purchase_list;
	INVENTORY_SALE_LIST *inventory_sale_list;

	if ( !inventory_name )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"inventory_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !purchase_date_time
	&&   !sale_date_time )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
		"both purchase_date_time and sale_date_time are empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	inventory_average = inventory_average_calloc();

	inventory_average->predictive_fund_boolean =
		predictive_fund_boolean(
			PREDICTIVE_FUND_TABLE,
			PREDICTIVE_FUND_COLUMN );

	inventory_average->entity_contact_key_boolean =
		entity_contact_key_boolean(
			ENTITY_TABLE,
			ENTITY_CONTACT_KEY_COLUMN );

	inventory_average->cost_date_time =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		inventory_average_cost_date_time(
			purchase_date_time,
			sale_date_time );

	inventory_average->inventory_purchase_cost_where =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		inventory_purchase_cost_where(
			INVENTORY_PURCHASE_TABLE,
			SALE_INVENTORY_COLUMN,
			PURCHASE_DATE_TIME_COLUMN,
			inventory_name,
			inventory_average->cost_date_time
				/* purchase_date_time */ );

	if ( !inventory_average->inventory_purchase_cost_where )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"inventory_purchase_cost_where(%s,%s) returned empty.",
			inventory_name,
			inventory_average->cost_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	inventory_purchase_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		inventory_purchase_list_new(
			INVENTORY_PURCHASE_SELECT,
			INVENTORY_PURCHASE_TABLE,
			inventory_average->predictive_fund_boolean,
			inventory_average->entity_contact_key_boolean,
			inventory_average->inventory_purchase_cost_where );

	inventory_average->purchase_list =
		inventory_average_purchase_list(
			inventory_purchase_list->list
				/* inventory_purchase_list */ );

	inventory_average->inventory_sale_cost_where =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		inventory_sale_cost_where(
			INVENTORY_SALE_TABLE,
			SALE_INVENTORY_COLUMN,
			TRANSACTION_DATE_TIME_COLUMN,
			inventory_name,
			inventory_average->cost_date_time
				/* sale_date_time */ );

	if ( !inventory_average->inventory_sale_cost_where )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"inventory_sale_cost_where(%s,%s) returned empty.",
			inventory_name,
			inventory_average->cost_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	inventory_sale_list =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		inventory_sale_list_new(
			INVENTORY_SALE_SELECT,
			INVENTORY_SALE_TABLE,
			inventory_average->predictive_fund_boolean,
			inventory_average->entity_contact_key_boolean,
			inventory_average->inventory_sale_cost_where );

	inventory_average->sale_list =
		inventory_average_sale_list(
			inventory_sale_list->list
				/* inventory_sale_list */ );

	inventory_average->inventory_balance_list =
		inventory_balance_list(
			inventory_average->purchase_list
				/* inventory_purchase_list */,
			inventory_average->sale_list
				/* inventory_sale_list */ );

	inventory_average->inventory_average_cost_list =
		inventory_average_cost_list(
			inventory_average->inventory_balance_list );

	return inventory_average;
}

INVENTORY_AVERAGE *inventory_average_calloc( void )
{
	INVENTORY_AVERAGE *inventory_average;

	if ( ! ( inventory_average =
			calloc( 1, sizeof ( INVENTORY_AVERAGE ) ) ) )
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

	return inventory_average;
}

char *inventory_average_cost_date_time(
		char *purchase_date_time,
		char *sale_date_time )
{
	if ( purchase_date_time )
		return purchase_date_time;
	else
		return sale_date_time;
}

LIST *inventory_average_purchase_list( LIST *inventory_purchase_list )
{
	return inventory_purchase_list;
}

LIST *inventory_average_sale_list( LIST *inventory_sale_list )
{
	return inventory_sale_list;
}

