/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/prepaid_asset_purchase.c		*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "piece.h"
#include "date.h"
#include "sql.h"
#include "prepaid_asset_purchase.h"

PREPAID_ASSET_PURCHASE *prepaid_asset_purchase_new(
			char *full_name,
			char *street_address,
			char *asset_name,
			char *purchase_date_time )
{
	PREPAID_ASSET_PURCHASE *prepaid_asset_purchase;

	if ( ! ( prepaid_asset_purchase =
			calloc( 1, sizeof( PREPAID_ASSET_PURCHASE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	prepaid_asset_purchase->vendor_entity =
		entity_new( full_name, street_address );

	prepaid_asset_purchase->asset_name = asset_name;
	prepaid_asset_purchase->purchase_date_time = purchase_date_time;
	return prepaid_asset_purchase;
}

PREPAID_ASSET_ACCRUAL *prepaid_asset_accrual_new(
			char *full_name,
			char *street_address,
			char *asset_name,
			char *purchase_date_time,
			char *accrual_date )
{
	PREPAID_ASSET_ACCRUAL *prepaid_asset_accrual;

	if ( ! ( prepaid_asset_accrual =
			calloc( 1, sizeof( PREPAID_ASSET_ACCRUAL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	prepaid_asset_accrual->vendor_entity =
		entity_new( full_name, street_address );

	prepaid_asset_accrual->asset_name = asset_name;
	prepaid_asset_accrual->purchase_date_time = purchase_date_time;
	prepaid_asset_accrual->accrual_date = accrual_date;
	return prepaid_asset_accrual;
}

double prepaid_asset_purchase_total(
			LIST *prepaid_asset_purchase_list )
{
	PREPAID_ASSET_PURCHASE *prepaid_asset_purchase;
	double total;

	if ( !list_rewind( prepaid_asset_purchase_list ) ) return 0.0;

	total = 0.0;

	do {

		prepaid_asset_purchase =
			list_get(
				prepaid_asset_purchase_list );

		total += prepaid_asset_purchase->cost;

	} while ( list_next( prepaid_asset_purchase_list ) );

	return total;
}
