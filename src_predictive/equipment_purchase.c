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
#include "html_table.h"
#include "piece.h"
#include "folder.h"
#include "environ.h"
#include "boolean.h"
#include "tax_recovery.h"
#include "entity.h"
#include "account.h"
#include "purchase.h"
#include "depreciation.h"
#include "predictive.h"
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
	char service_placement_date[ 128 ];
	char piece_buffer[ 1024 ];
	EQUIPMENT_PURCHASE *equipment_purchase;

	if ( !input ) return (EQUIPMENT_PURCHASE *)0;

	piece( asset_name, SQL_DELIMITER, input, 0 );
	piece( serial_number, SQL_DELIMITER, input, 1 );

	equipment_purchase =
		equipment_purchase_new(
			strdup( asset_name ),
			strdup( serial_number ) );

	piece( service_placement_date, SQL_DELIMITER, input, 2 );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	equipment_purchase->service_placement_date = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	equipment_purchase->equipment_cost = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	equipment_purchase->estimated_useful_life_years =
		atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	equipment_purchase->estimated_useful_life_units =
		atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	equipment_purchase->estimated_residual_value = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 8 );
	equipment_purchase->declining_balance_n = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 9 );
	equipment_purchase->depreciation_method = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 10 );
	equipment_purchase->tax_cost_basis = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 11 );
	equipment_purchase->tax_recovery_period = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 12 );
	equipment_purchase->disposal_date = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 13 );
	equipment_purchase->finance_accumulated_depreciation =
		atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 14 );
	equipment_purchase->tax_accumulated_depreciation =
		atoi( piece_buffer );

	equipment_purchase->depreciation_list =
		depreciation_list(
			equipment_purchase->asset_name,
			equipment_purchase->serial_number,
			depreciate_folder_name );

	return equipment_purchase;
}

LIST *equipment_system_list( char *sys_string )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *equipment_purchase_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(	equipment_purchase_list,
				equipment_purchase_parse( input ) );
	}
	pclose( input_pipe );
	return equipment_purchase_list;
}

char *equipment_purchase_sys_string(
			char *asset_folder_name,
			char *where,
			char *order )
{
	char sys_string[ 1024 ];
	char order_clause[ 128 ];

	if ( order && *order )
	{
		sprintf( order_clause,
			 "order by %s",
			 order );
	}
	else
	{
		*order_clause = '\0';
	}
		
	sprintf( sys_string,
		 "echo \"select %s from %s where %s %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 equipment_purchase_select(),
		 asset_folder_name,
		 where,
		 order_clause );

	return strdup( sys_string );
}

LIST *equipment_purchase_list_fetch(
			char *asset_folder_name,
			char *depreciate_folder_name,
			char *where )
{
	return equipment_system_list(
		equipment_purchase_sys_string(
			asset_folder_name,
			where,
			"service_placement_date"
				/* order */ ) );
}

FILE *equipment_purchase_update_open( void )
{
	char sys_string[ 1024 ];
	char *key;

	key =	"asset_name,"
		"serial_number,"
		"full_name,"
		"street_address,"
		"service_placement_date";

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
			char *service_placement_date )
{
	FILE *update_pipe = equipment_purchase_update_open();

	fprintf(update_pipe,
		"%s^%s^%s^%s^%s^finance_accumulated_depreciation^%.2lf\n",
		equipment_purchase_escape_asset_name( asset_name ),
		serial_number,
		entity_escape_full_name( full_name ),
		street_address,
		service_placement_date,
		finance_accumulated_depreciation );

	fprintf(update_pipe,
		"%s^%s^%s^%s^%s^tax_accumulated_depreciation^%.2lf\n",
		equipment_purchase_escape_asset_name( asset_name ),
		serial_number,
		entity_escape_full_name( full_name ),
		street_address,
		service_placement_date,
		tax_accumulated_depreciation );

	pclose( update_pipe );
}

EQUIPMENT_PURCHASE *equipment_purchase_fetch(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *service_placement_date )
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
			serial_number ) );

	return equipment_purchase_parse( pipe2string( sys_string ) );
}

/* Safely returns heap memory */
/* -------------------------- */
char *equipment_purchase_primary_where(
			char *asset_name,
			char *serial_number,
			char *full_name,
			char *street_address,
			char *service_placement_date )
{
	char where[ 1024 ];

	sprintf( where,
		 "asset_name = '%s' and		"
		 "serial_number = '%s' and	"
		 "full_name = '%s' and		"
		 "street_address = '%s' and	"
		 "service_placement_date = '%s'	",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 equipment_purchase_escape_asset_name( asset_name ),
		 serial_number,
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 entity_escape_full_name( full_name ),
		 street_address,
		 service_placement_date );

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

DEPRECIATION *equipment_purchase_depreciation(
			EQUIPMENT_PURCHASE *equipment_purchase,
			char *depreciation_date,
			char *prior_depreciation_date,
			char *transaction_date_time,
			int units_produced )
{
	DEPRECIATION *depreciation;

	depreciation =
		depreciation_new(
			equipment_purchase->asset_name,
			equipment_purchase->serial_number,
			equipment_purchase->vendor_entity->full_name,
			equipment_purchase->vendor_entity->street_address,
			equipment_purchase->service_placement_date,
			depreciation_date );

	depreciation->depreciation_amount =
		depreciation_amount(
			equipment_purchase->depreciation_method,
			equipment_purchase->equipment_cost,
			equipment_purchase->estimated_residual_value,
			equipment_purchase->estimated_useful_life_years,
			equipment_purchase->estimated_useful_life_units,
			equipment_purchase->declining_balance_n,
			prior_depreciation_date,
			depreciation_date,
			equipment_purchase->finance_accumulated_depreciation,
			equipment_purchase->service_placement_date,
			units_produced );

	if ( timlib_dollar_virtually_same(
		depreciation->depreciation_amount,
		0.0 ) )
	{
		return (DEPRECIATION *)0;
	}

	depreciation->depreciation_accumulated_depreciation =
		depreciation_accumulated_depreciation(
			depreciation->depreciation_accumulated_depreciation
				/* prior_accumulated_depreciation */,
			depreciation->depreciation_amount );

	if ( transaction_date_time )
	{
		depreciation->depreciation_transaction =
			depreciation_transaction(
				equipment_purchase->
					vendor_entity->
					full_name,
				equipment_purchase->
					vendor_entity->
					street_address,
				transaction_date_time,
				depreciation->depreciation_amount,
				account_depreciation_expense(
					(char *)0 /* fund_name */ ),
				account_accumulated_depreciation(
					(char *)0 /* fund_name */ ) );
	}
	return depreciation;
}

LIST *equipment_purchase_depreciation_list(
			LIST *equipment_purchase_list )
{
	LIST *depreciation_list;
	EQUIPMENT_PURCHASE *equipment_purchase;

	if ( !list_rewind( equipment_purchase_list ) ) return (LIST *)0;

	depreciation_list = list_new();

	do {
		equipment_purchase =
			list_get(
				equipment_purchase_list );

		if ( equipment_purchase->equipment_purchase_depreciation )
		{
			list_set(
				depreciation_list,
				equipment_purchase->
					equipment_purchase_depreciation );
		}

	} while ( list_next( equipment_purchase_list ) );
	return depreciation_list;
}

LIST *equipment_purchase_list_depreciate(
			LIST *equipment_purchase_list,
			char *depreciation_date,
			boolean set_depreciation_transaction )
{
	EQUIPMENT_PURCHASE *equipment_purchase;
	char *prior_depreciation_date;
	char *transaction_date_time = {0};

	if ( !list_rewind( equipment_purchase_list ) ) return (LIST *)0;

	do {
		equipment_purchase =
			list_get(
				equipment_purchase_list );

		if ( list_length( equipment_purchase->depreciation_list ) )
		{
			DEPRECIATION *prior_depreciation;

			prior_depreciation =
				list_last(
					equipment_purchase->
						depreciation_list );

			prior_depreciation_date =
				prior_depreciation->depreciation_date;
		}
		else
		{
			prior_depreciation_date = (char *)0;
		}

		if ( set_depreciation_transaction )
		{
			transaction_date_time =
				/* ---------------------------------- */
				/* Returns heap memory		      */
				/* Increments seconds each invocation */
				/* ---------------------------------- */
				predictive_transaction_date_time(
					depreciation_date );
		}

		list_set(
			equipment_purchase->depreciation_list,
			( equipment_purchase->
				equipment_purchase_depreciation =
				equipment_purchase_depreciation(
					equipment_purchase,
					depreciation_date,
					prior_depreciation_date,
					/* Null value omits transaction */
					/* ---------------------------- */
					transaction_date_time,
					0 /* units_produced */ ) ) );

	} while ( list_next( equipment_purchase_list ) );

	return equipment_purchase_list;
}

void equipment_purchase_depreciation_table(
			LIST *equipment_purchase_list )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *heading_list_string;
	char *justify_list_string;
	EQUIPMENT_PURCHASE *equipment_purchase;

	heading_list_string =
		"asset_name,"
		"serial_number,"
		"vendor,"
		"purchase,"
		"depreciation,"
		"depreciation_amount";

	justify_list_string = "left,left,left,left,left,right";

	sprintf( sys_string,
		 "html_table.e '^%s' '%s' '^' '%s'",
		 "Depreciate Equipment",
		 heading_list_string,
		 justify_list_string );

	output_pipe = popen( sys_string, "w" );

	if ( !list_rewind( equipment_purchase_list ) )
	{
		pclose( output_pipe );
		return;
	}

	do {
		equipment_purchase =
			list_get(
				equipment_purchase_list );

		if ( !equipment_purchase->
			equipment_purchase_depreciation )
		{
			continue;
		}

		fprintf( output_pipe,
			 "%s^%s^%s/%s^%s^%s^%.2lf\n",
			 equipment_purchase->asset_name,
			 equipment_purchase->serial_number,
			 equipment_purchase->vendor_entity->full_name,
			 equipment_purchase->vendor_entity->street_address,
			 equipment_purchase->service_placement_date,
			 equipment_purchase->
				equipment_purchase_depreciation->
				depreciation_date,
			 equipment_purchase->
				equipment_purchase_depreciation->
				depreciation_amount );

	} while( list_next( equipment_purchase_list ) );
	pclose( output_pipe );
}

