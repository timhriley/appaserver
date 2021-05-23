/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/specific_inventory.c			*/
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
#include "specific_inventory.h"

SPECIFIC_INVENTORY *specific_inventory_new(
			char *inventory_name )
{
	SPECIFIC_INVENTORY *specific_inventory;

	if ( ! ( specific_inventory =
			calloc( 1, sizeof( SPECIFIC_INVENTORY ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	specific_inventory->inventory_name = inventory_name;
	return specific_inventory;
}

