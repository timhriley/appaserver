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
			char *serial_number )
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

	equipment_purchase =
		equipment_purchase_new(
			strdup( asset_name ),
			strdup( serial_number ) );

	piece( full_name, SQL_DELIMITER, input, 2 );
	piece( street_address, SQL_DELIMITER, input, 3 );
	piece( purchase_date_time, SQL_DELIMITER, input, 4 );

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

LIST *equipment_purchase_list( char *where_clause )
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
		 where_clause );

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(	equipment_purchase_list,
				equipment_parse( input ) );
	}
	pclose( input_pipe );
	return equipment_purchase_list;
}

EQUIPMENT_PURCHASE *equipment_purchase_seek(
			LIST *equipment_purchase_list,
			char *asset_name,
			char *serial_number )
{
	EQUIPMENT_PURCHASE *equipment_purchase;

	if ( !list_rewind( equipment_purchase_list ) )
		return (EQUIPMENT_PURCHASE *)0;

	do {
		equipment_purchase = list_get( equipment_purchase_list );

		if ( strcmp(	equipment_purchase->asset_name,
				asset_name ) == 0
		&&   strcmp(	equipment_purchase->serial_number,
				serial_number ) == 0 )
		{
			return equipment_purchase;
		}
	} while( list_next( equipment_purchase_list ) );

	return (EQUIPMENT_PURCHASE *)0;
}

FILE *equipment_purchase_update_open( void )
{
	return (FILE *)0;
}

