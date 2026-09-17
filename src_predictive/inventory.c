/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include "all.h"
#include "sale.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "sql.h"
#include "piece.h"
#include "security.h"
#include "String.h"
#include "optional_column.h"
#include "folder_attribute.h"
#include "inventory.h"

INVENTORY *inventory_fetch( char *inventory_name )
{
	char *primary_where;
	boolean labor_charge_boolean;
	char *select_string;
	char *system_string;
	char *input;
	INVENTORY *inventory;

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

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		inventory_primary_where(
			SALE_INVENTORY_COLUMN,
			inventory_name );

	labor_charge_boolean =
		inventory_labor_charge_boolean(
			INVENTORY_TABLE,
			INVENTORY_LABOR_CHARGE_COLUMN );

	select_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		inventory_select_string(
			INVENTORY_SELECT,
			INVENTORY_LABOR_CHARGE_COLUMN,
			labor_charge_boolean );

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			select_string,
			INVENTORY_TABLE,
			primary_where );

	free( select_string );

	/* Returns heap memory or null */
	/* --------------------------- */
	input = string_system_input( system_string );

	if ( !input )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"string_system_input(%s) returned empty.",
			system_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	free( system_string );

	inventory =
		/* -------------- */
		/* Should succeed */
		/* -------------- */
		inventory_parse(
			labor_charge_boolean,
			input );

	free( input );

	return inventory;
}

INVENTORY *inventory_new( char *inventory_name )
{
	INVENTORY *inventory;

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

	inventory = inventory_calloc();
	inventory->inventory_name = inventory_name;

	return inventory;
}

INVENTORY *inventory_calloc( void )
{
	INVENTORY *inventory;

	if ( ! ( inventory = calloc( 1, sizeof ( INVENTORY ) ) ) )
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

	return inventory;
}

char *inventory_primary_where(
		const char *sale_inventory_column,
		char *inventory_name )
{
	char *escape;
	static char where[ 128 ];

	/* Returns heap memory */
	/* ------------------- */
	escape = security_escape( inventory_name );

	snprintf(
		where,
		sizeof ( where ),
		"%s = '%s'",
		sale_inventory_column,
		escape );

	free( escape );

	return where;
}

boolean inventory_labor_charge_boolean(
		const char *inventory_table,
		const char *inventory_labor_charge_column )
{
	return
	folder_attribute_boolean(
		(char *)inventory_table,
		(char *)inventory_labor_charge_column );
}

char *inventory_select_string(
		const char *inventory_select,
		const char *inventory_labor_charge_column,
		boolean inventory_labor_charge_boolean )
{
	OPTIONAL_COLUMN *optional_column;

	optional_column =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		optional_column_new(
			',' /* delimiter */,
			(char *)inventory_select /* base_string */,
			(char *)inventory_labor_charge_column /* component */,
			0 /* not escape_boolean */,
			inventory_labor_charge_boolean /* set_boolean */ );

	return optional_column->return_string;
}

INVENTORY *inventory_parse(
		boolean labor_charge_boolean,
		char *input )
{
	char inventory_name[ 128 ];
	char buffer[ 128 ];
	INVENTORY *inventory;

	if ( !input || !*input ) return NULL;

	/* See inventory_select_string() */
	/* ----------------------------- */
	piece( inventory_name, SQL_DELIMITER, input, 0 );

	inventory =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		inventory_new(
			strdup( inventory_name ) );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) inventory->inventory_account = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	if ( *buffer ) inventory->cost_of_goods_sold_account = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	if ( *buffer ) inventory->inventory_category = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer ) inventory->retail_price = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer ) inventory->reorder_quantity = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	if ( *buffer ) inventory->quantity_on_hand = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 7 );
	if ( *buffer ) inventory->average_unit_cost = atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 8 );
	if ( *buffer ) inventory->total_cost_balance = atof( buffer );

	if ( labor_charge_boolean )
	{
		piece( buffer, SQL_DELIMITER, input, 9 );
		if ( *buffer ) inventory->labor_charge = atof( buffer );
	}

	return inventory;
}
