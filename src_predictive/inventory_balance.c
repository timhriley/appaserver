/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_balance.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include "all.h"
#include "String.h"
#include "appaserver_error.h"
#include "inventory_balance.h"

LIST *inventory_balance_list(
		LIST *inventory_purchase_list,
		LIST *inventory_sale_list )
{
	INVENTORY_PURCHASE *inventory_purchase;
	INVENTORY_SALE *inventory_sale;
	INVENTORY_BALANCE *inventory_balance;
	LIST *balance_list = list_new();

	if (	!list_length( inventory_purchase_list )
	&&	!list_length( inventory_sale_list ) )
	{
		return NULL;
	}

	list_rewind( inventory_purchase_list );
	list_rewind( inventory_sale_list );

	while( 1 )
	{
		if ( !list_past_end( inventory_purchase_list ) )
		{
			inventory_purchase =
				list_get(
					inventory_purchase_list );
		}
		else
		{
			inventory_purchase = NULL;
		}

		if ( !list_past_end( inventory_sale_list ) )
		{
			inventory_sale =
				list_get(
				inventory_sale_list );
		}
		else
		{
			inventory_sale = NULL;
		}

		/* All done */
		/* -------- */
		if ( !inventory_purchase && !inventory_sale ) break;

		inventory_balance = inventory_balance_calloc();

		/* If out of sales */
		/* --------------- */
		if ( !inventory_sale )
		{
			inventory_balance->inventory_purchase =
				inventory_purchase;
	
			list_next( inventory_purchase_list );
		}
		else
		/* ------------------- */
		/* If out of purchases */
		/* ------------------- */
		if ( !inventory_purchase )
		{
			inventory_balance->inventory_sale =
				inventory_sale;
	
			list_next( inventory_sale_list );
		}
		else
		if ( string_strcmp(
				inventory_purchase->purchase_date_time,
	     			inventory_sale->sale_date_time ) <= 0 )
		{
			inventory_balance->inventory_purchase =
				inventory_purchase;
	
			list_next( inventory_purchase_list );
		}
		else
		{
			inventory_balance->inventory_sale =
				inventory_sale;
	
			list_next( inventory_sale_list );
		}
	
		list_set( balance_list, inventory_balance );
	}

	if ( !list_length( balance_list ) )
	{
		list_free( balance_list );
		balance_list = NULL;
	}

	return balance_list;
}

INVENTORY_BALANCE *inventory_balance_calloc( void )
{
	INVENTORY_BALANCE *inventory_balance;

	if ( ! ( inventory_balance =
			calloc( 1, sizeof ( INVENTORY_BALANCE ) ) ) )
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

	return inventory_balance;
}

