/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/specific_inventory_purchase.c	*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
#include "piece.h"
#include "entity.h"
#include "specific_inventory_purchase.h"

SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase_new(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *inventory_name,
			char *serial_label )
{
	SPECIFIC_INVENTORY_PURCHASE *h =
		calloc( 1, sizeof( SPECIFIC_INVENTORY_PURCHASE ) );

	if ( !h )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit(1 );
	}

	h->vendor_entity =
		entity_new(
			full_name,
			street_address );

	h->purchase_date_time = purchase_date_time;

	h->specific_inventory =
		specific_inventory_new(
			inventory_name );

	h->serial_label = serial_label;
	return h;
}

double specific_inventory_purchase_total(
			LIST *specific_inventory_purchase_list )
{
	SPECIFIC_INVENTORY_PURCHASE *specific_inventory_purchase;
	double total;

	if ( !list_rewind( specific_inventory_purchase_list ) ) return 0.0;

	total = 0.0;

	do {

		specific_inventory_purchase =
			list_get(
				specific_inventory_purchase_list );

		total += specific_inventory_purchase->inventory_cost;

	} while ( list_next( specific_inventory_purchase_list ) );

	return total;
}

