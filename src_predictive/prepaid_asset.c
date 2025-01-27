/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/prepaid_asset.c			*/
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
#include "prepaid_asset.h"

PREPAID_ASSET *prepaid_asset_new(
			char *asset_name )
{
	PREPAID_ASSET *prepaid_asset;

	if ( ! ( prepaid_asset =
			calloc( 1, sizeof( PREPAID_ASSET ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	prepaid_asset->asset_name = asset_name;
	return prepaid_asset;
}

