/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_purchase_update.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_error.h"
#include "sale.h"
#include "inventory_purchase.h"
#include "inventory_average_cost.h"
#include "inventory_purchase_update.h"

INVENTORY_PURCHASE_UPDATE *inventory_purchase_update_new(
		boolean fund_boolean,
		boolean contact_key_boolean,
		LIST *inventory_average_cost_list )
{
	INVENTORY_PURCHASE_UPDATE *inventory_purchase_update;
	LIST *primary_key_list;

	inventory_purchase_update = inventory_purchase_update_calloc();

	primary_key_list =
		inventory_purchase_list_primary_key_list(
			SALE_INVENTORY_COLUMN,
			fund_boolean,
			contact_key_boolean );

	inventory_purchase_update->update_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		inventory_purchase_list_update_system_string(
			INVENTORY_PURCHASE_TABLE,
			primary_key_list );

	inventory_purchase_update->update_string_list =
		inventory_average_cost_list_purchase_update_string_list(
			fund_boolean,
			contact_key_boolean,
			inventory_average_cost_list,
			0 /* not average_attributes_boolean */ );

	return inventory_purchase_update;
}

INVENTORY_PURCHASE_UPDATE *inventory_purchase_update_calloc( void )
{
	INVENTORY_PURCHASE_UPDATE *inventory_purchase_update;

	if ( ! ( inventory_purchase_update =
			calloc( 1,
				sizeof ( INVENTORY_PURCHASE_UPDATE ) ) ) )
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

	return inventory_purchase_update;
}

