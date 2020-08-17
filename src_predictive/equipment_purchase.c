/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/equipment_purchase.c			*/
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
#include "folder.h"
#include "environ.h"
#include "boolean.h"
#include "transaction.h"
#include "account.h"
#include "depreciation.h"
#include "tax_recovery.h"
#include "purchase.h"
#include "equipment_purchase.h"

EQUIPMENT_PURCHASE *equipment_purchase_new(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	EQUIPMENT_PURCHASE *equipment_purchase;

	if ( ! ( equipment_purchase =
			calloc( 1, sizeof( EQUIPMENT_PURCHASE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	equipment_purchase->asset_name = asset_name;
	equipment_purchase->serial_number = serial_number;

	equipment_purchase->vendor_entity =
		entity_new(
			full_name,
			street_address );

	equipment_purchase->purchase_date_time = purchase_date_time;

	return equipment_purchase;
}

/* Returns program memory */
/* ---------------------- */
char *equipment_purchase_select( void )
{
	return
	"asset_name,"
	"serial_number,"
	"full_name,"
	"street_address,"
	"purchase_date_time,"
	"service_placement_date,"
	"purchase_price,"
	"estimated_useful_life_years,"
	"estimated_useful_life_units,"
	"estimated_residual_value,"
	"declining_balance_n,"
	"depreciation_method,"
	"tax_cost_basis,"
	"tax_recovery_period,"
	"disposal_date,"
	"finance_accumulated_depreciation,"
	"tax_accumulated_depreciation";
}

EQUIPMENT_PURCHASE *equipment_purchase_parse( char *input )
{
	char asset_name[ 128 ];
	char serial_number[ 128 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char purchase_date_time[ 128 ];
	char piece_buffer[ 1024 ];
	EQUIPMENT_PURCHASE *equipment_purchase;

	if ( !input ) return (EQUIPMENT_PURCHASE *)0;

	piece( asset_name, SQL_DELIMITER, input, 0 );
	piece( serial_number, SQL_DELIMITER, input, 1 );
	piece( full_name, SQL_DELIMITER, input, 2 );
	piece( street_address, SQL_DELIMITER, input, 3 );
	piece( purchase_date_time, SQL_DELIMITER, input, 4 );

	equipment_purchase =
		equipment_purchase_new(
			strdup( asset_name ),
			strdup( serial_number ),
			strdup( full_name ),
			strdup( street_address ),
			strdup( purchase_date_time ) );

	if ( ! ( equipment_purchase->purchase_order =
			purchase_order_fetch(
				strdup( full_name ),
				strdup( street_address ),
				strdup( purchase_date_time ) ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s/%d: purchase_order_fetch(%s/%s/%s) returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address,
			 purchase_date_time );
		exit( 1 );
	}

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	equipment_purchase->service_placement_date = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	equipment_purchase->equipment_cost = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	equipment_purchase->estimated_useful_life_years =
		atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 8 );
	equipment_purchase->estimated_useful_life_units =
		atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 9 );
	equipment_purchase->estimated_residual_value = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 10 );
	equipment_purchase->declining_balance_n = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 11 );
	equipment_purchase->depreciation_method = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 12 );
	equipment_purchase->tax_cost_basis = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 13 );
	equipment_purchase->tax_recovery_period = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 14 );
	equipment_purchase->disposal_date = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 15 );
	equipment_purchase->finance_accumulated_depreciation =
		atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 16 );
	equipment_purchase->tax_accumulated_depreciation =
		atoi( piece_buffer );

	return equipment_purchase;
}

LIST *equipment_purchase_list(
			char *purchase_order_primary_where )
{
	return
		equipment_purchase_list_fetch(
			purchase_order_primary_where );
}

LIST *equipment_purchase_list_fetch( char *where )
{
	char sys_string[ 1024 ];
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *equipment_purchase_list = list_new();

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 equipment_purchase_select(),
		 "equipment_purchase",
		 where );

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(	equipment_purchase_list,
				equipment_purchase_parse( input ) );
	}
	pclose( input_pipe );
	return equipment_purchase_list;
}

FILE *equipment_purchase_update_open( void )
{
	char sys_string[ 1024 ];
	char *key;

	key =	"asset_name,"
		"serial_number,"
		"full_name,"
		"street_address,"
		"purchase_date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql",
		 "equipment_purchase",
		 key );

	return popen( sys_string, "w" );
}

char *equipment_purchase_escape_asset_name(
			char *asset_name )
{
	static char escape_asset_name[ 256 ];

	string_escape_quote( escape_asset_name, asset_name );
	return escape_asset_name;
}

void equipment_purchase_update(
			double finance_accumulated_depreciation,
			double tax_accumulated_depreciation,
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	FILE *update_pipe = equipment_purchase_update_open();

	fprintf(update_pipe,
		"%s^%s^%s^%s^%s^finance_accumulated_depreciation^%.2lf\n",
		equipment_purchase_escape_asset_name( asset_name ),
		serial_number,
		transaction_escape_full_name( full_name ),
		street_address,
		purchase_date_time,
		finance_accumulated_depreciation );

	fprintf(update_pipe,
		"%s^%s^%s^%s^%s^tax_accumulated_depreciation^%.2lf\n",
		equipment_purchase_escape_asset_name( asset_name ),
		serial_number,
		transaction_escape_full_name( full_name ),
		street_address,
		purchase_date_time,
		tax_accumulated_depreciation );

	pclose( update_pipe );
}

EQUIPMENT_PURCHASE *equipment_purchase_fetch(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 equipment_purchase_select(),
		 "equipment_purchase",
		 /* -------------------------- */
		 /* Safely returns heap memory */
		 /* -------------------------- */
		 equipment_purchase_primary_where(
			asset_name,
			serial_number,
			full_name,
			street_address,
			purchase_date_time ) );

	return equipment_purchase_parse( pipe2string( sys_string ) );
}

/* Safely returns heap memory */
/* -------------------------- */
char *equipment_purchase_primary_where(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	char where[ 1024 ];

	sprintf( where,
		 "asset_name = '%s' and		"
		 "serial_number = '%s' and	"
		 "full_name = '%s' and		"
		 "street_address = '%s' and	"
		 "purchase_date_time = '%s'	",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 equipment_purchase_escape_asset_name( asset_name ),
		 serial_number,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 transaction_escape_full_name( full_name ),
		 street_address,
		 purchase_date_time );

	return strdup( where );
}

double equipment_purchase_total(
			LIST *equipment_purchase_list )
{
	EQUIPMENT_PURCHASE *equipment_purchase;
	LIST *l = equipment_purchase_list;
	double purchase_total = 0.0;

	if ( !list_rewind( l ) ) return 0.0;

	do {
		equipment_purchase = list_get( l );
		purchase_total += equipment_purchase->equipment_cost;

	} while ( list_next( l ) );

	return purchase_total;
}

