/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/equipment_depreciation.c		*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "list.h"
#include "sql.h"
#include "piece.h"
#include "list.h"
#include "boolean.h"
#include "entity.h"
#include "equipment_depreciation.h"

EQUIPMENT_DEPRECIATION *equipment_depreciation_new(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *depreciation_date )
{
	return (EQUIPMENT_DEPRECIATION *)0;
}

EQUIPMENT_DEPRECIATION *equipment_depreciation_parse( char *input )
{
	return (EQUIPMENT_DEPRECIATION *)0;
}

EQUIPMENT_DEPRECIATION *equipment_depreciation_fetch(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *depreciation_date_time )
{
	return (EQUIPMENT_DEPRECIATION *)0;
}

char *equipment_depreciation_select( void )
{
	return (char *)0;
}

LIST *equipment_depreciation_list_fetch( char *where )
{
	return (LIST *)0;
}

LIST *equipment_depreciation_list(
			char *equipment_purchase_primary_where )
{
	return (LIST *)0;
}

FILE *equipment_depreciation_update_open( void )
{
	return (FILE *)0;
}

void equipment_depreciation_update(
			double depreciation_amount,
			char *transaction_date_time,
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *depreciation_date_time )
{
}

double equipment_depreciation_amount(
			LIST *equipment_depreciation_list )
{
	return 0.0;
}

