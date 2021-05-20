/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_asset_purchase.c		*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "piece.h"
#include "boolean.h"
#include "list.h"
#include "sql.h"
#include "html_table.h"
#include "folder.h"
#include "environ.h"
#include "tax_recovery.h"
#include "entity.h"
#include "account.h"
#include "fixed_asset.h"
#include "purchase.h"
#include "depreciation.h"
#include "predictive.h"
#include "fixed_asset_purchase.h"

FIXED_ASSET_PURCHASE *fixed_asset_purchase_new(
			char *asset_name,
			char *serial_label )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( ! ( fixed_asset_purchase =
			calloc( 1, sizeof( FIXED_ASSET_PURCHASE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	fixed_asset_purchase->fixed_asset =
		fixed_asset_new(
			asset_name,
			serial_label );

	return fixed_asset_purchase;
}

/* Returns program memory */
/* ---------------------- */
char *fixed_asset_purchase_select( void )
{
	return
	"asset_name,"
	"serial_label,"
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

FIXED_ASSET_PURCHASE *fixed_asset_purchase_parse( char *input )
{
	char asset_name[ 128 ];
	char serial_label[ 128 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char service_placement_date[ 128 ];
	char piece_buffer[ 1024 ];
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( !input ) return (FIXED_ASSET_PURCHASE *)0;

	piece( asset_name, SQL_DELIMITER, input, 0 );
	piece( serial_label, SQL_DELIMITER, input, 1 );

	fixed_asset_purchase =
		fixed_asset_purchase_new(
			strdup( asset_name ),
			strdup( serial_label ) );

	piece( service_placement_date, SQL_DELIMITER, input, 2 );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	fixed_asset_purchase->service_placement_date = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	fixed_asset_purchase->fixed_asset_cost = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	fixed_asset_purchase->estimated_useful_life_years =
		atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	fixed_asset_purchase->estimated_useful_life_units =
		atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	fixed_asset_purchase->estimated_residual_value = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 8 );
	fixed_asset_purchase->declining_balance_n = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 9 );
	fixed_asset_purchase->depreciation_method = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 10 );
	fixed_asset_purchase->tax_cost_basis = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 11 );
	fixed_asset_purchase->tax_recovery_period = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 12 );
	fixed_asset_purchase->disposal_date = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 13 );
	fixed_asset_purchase->finance_accumulated_depreciation =
		atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 14 );
	fixed_asset_purchase->tax_accumulated_depreciation =
		atoi( piece_buffer );

	fixed_asset_purchase->depreciation_list =
		depreciation_list(
			fixed_asset_purchase->asset_name,
			fixed_asset_purchase->serial_label,
			depreciate_folder_name );

	return fixed_asset_purchase;
}

LIST *fixed_asset_system_list( char *sys_string )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *fixed_asset_purchase_list = list_new();

	input_pipe = popen( sys_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(	fixed_asset_purchase_list,
				fixed_asset_purchase_parse( input ) );
	}
	pclose( input_pipe );
	return fixed_asset_purchase_list;
}

char *fixed_asset_purchase_sys_string(
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
		 fixed_asset_purchase_select(),
		 asset_folder_name,
		 where,
		 order_clause );

	return strdup( sys_string );
}

LIST *fixed_asset_purchase_list_fetch(
			char *asset_folder_name,
			char *depreciate_folder_name,
			char *where )
{
	return fixed_asset_system_list(
		fixed_asset_purchase_sys_string(
			asset_folder_name,
			where,
			"service_placement_date"
				/* order */ ) );
}

FILE *fixed_asset_purchase_update_open( void )
{
	char sys_string[ 1024 ];
	char *key;

	key =	"asset_name,"
		"serial_label,"
		"full_name,"
		"street_address,"
		"service_placement_date";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y | sql",
		 "fixed_asset_purchase",
		 key );

	return popen( sys_string, "w" );
}

void fixed_asset_purchase_update(
			double finance_accumulated_depreciation,
			double tax_accumulated_depreciation,
			char *asset_name,
			char *serial_label,
			char *full_name,
			char *street_address,
			char *service_placement_date )
{
	FILE *update_pipe = fixed_asset_purchase_update_open();

	fprintf(update_pipe,
		"%s^%s^%s^%s^%s^finance_accumulated_depreciation^%.2lf\n",
		fixed_asset_purchase_escape_asset_name( asset_name ),
		serial_label,
		entity_escape_full_name( full_name ),
		street_address,
		service_placement_date,
		finance_accumulated_depreciation );

	fprintf(update_pipe,
		"%s^%s^%s^%s^%s^tax_accumulated_depreciation^%.2lf\n",
		fixed_asset_purchase_escape_asset_name( asset_name ),
		serial_label,
		entity_escape_full_name( full_name ),
		street_address,
		service_placement_date,
		tax_accumulated_depreciation );

	pclose( update_pipe );
}

FIXED_ASSET_PURCHASE *fixed_asset_purchase_fetch(
			char *asset_name,
			char *serial_label,
			char *full_name,
			char *street_address,
			char *service_placement_date )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"select.sh 
		 "echo \"select %s from %s where %s;\" | sql",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 fixed_asset_purchase_select(),
		 "fixed_asset_purchase",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 fixed_asset_purchase_primary_where(
			asset_name,
			serial_label ) );

	return fixed_asset_purchase_parse( pipe2string( sys_string ) );
}

char *fixed_asset_purchase_primary_where(
			char *asset_name,
			char *serial_label )
{
	static char where[ 256 ];

	sprintf( where,
		 "asset_name = '%s' and		"
		 "serial_label = '%s' 		",
		 /* --------------------- */
		 /* Returns static memory */
		 /* --------------------- */
		 fixed_asset_name_escape( asset_name ),
		 serial_label );

	return strdup( where );
}

double fixed_asset_purchase_total(
			LIST *fixed_asset_purchase_list )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;
	LIST *l = fixed_asset_purchase_list;
	double purchase_total = 0.0;

	if ( !list_rewind( l ) ) return 0.0;

	do {
		fixed_asset_purchase = list_get( l );
		purchase_total += fixed_asset_purchase->fixed_asset_cost;

	} while ( list_next( l ) );

	return purchase_total;
}

DEPRECIATION *fixed_asset_purchase_depreciation(
			FIXED_ASSET_PURCHASE *fixed_asset_purchase,
			char *depreciation_date,
			char *prior_depreciation_date,
			char *transaction_date_time,
			int units_produced )
{
	DEPRECIATION *depreciation;

	depreciation =
		depreciation_new(
			fixed_asset_purchase->asset_name,
			fixed_asset_purchase->serial_label,
			fixed_asset_purchase->vendor_entity->full_name,
			fixed_asset_purchase->vendor_entity->street_address,
			fixed_asset_purchase->service_placement_date,
			depreciation_date );

	depreciation->depreciation_amount =
		depreciation_amount(
			fixed_asset_purchase->depreciation_method,
			fixed_asset_purchase->fixed_asset_cost,
			fixed_asset_purchase->estimated_residual_value,
			fixed_asset_purchase->estimated_useful_life_years,
			fixed_asset_purchase->estimated_useful_life_units,
			fixed_asset_purchase->declining_balance_n,
			prior_depreciation_date,
			depreciation_date,
			fixed_asset_purchase->finance_accumulated_depreciation,
			fixed_asset_purchase->service_placement_date,
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
				fixed_asset_purchase->
					vendor_entity->
					full_name,
				fixed_asset_purchase->
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

LIST *fixed_asset_purchase_depreciation_list(
			LIST *fixed_asset_purchase_list )
{
	LIST *depreciation_list;
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( !list_rewind( fixed_asset_purchase_list ) ) return (LIST *)0;

	depreciation_list = list_new();

	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_list );

		if ( fixed_asset_purchase->fixed_asset_purchase_depreciation )
		{
			list_set(
				depreciation_list,
				fixed_asset_purchase->
					fixed_asset_purchase_depreciation );
		}

	} while ( list_next( fixed_asset_purchase_list ) );
	return depreciation_list;
}

LIST *fixed_asset_purchase_list_depreciate(
			LIST *fixed_asset_purchase_list,
			char *depreciation_date,
			boolean set_depreciation_transaction )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;
	char *prior_depreciation_date;
	char *transaction_date_time = {0};

	if ( !list_rewind( fixed_asset_purchase_list ) ) return (LIST *)0;

	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_list );

		if ( list_length( fixed_asset_purchase->depreciation_list ) )
		{
			DEPRECIATION *prior_depreciation;

			prior_depreciation =
				list_last(
					fixed_asset_purchase->
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
			fixed_asset_purchase->depreciation_list,
			( fixed_asset_purchase->
				fixed_asset_purchase_depreciation =
				fixed_asset_purchase_depreciation(
					fixed_asset_purchase,
					depreciation_date,
					prior_depreciation_date,
					/* Null value omits transaction */
					/* ---------------------------- */
					transaction_date_time,
					0 /* units_produced */ ) ) );

	} while ( list_next( fixed_asset_purchase_list ) );

	return fixed_asset_purchase_list;
}

void fixed_asset_purchase_depreciation_table(
			LIST *fixed_asset_purchase_list )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *heading_list_string;
	char *justify_list_string;
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	heading_list_string =
		"asset_name,"
		"serial_label,"
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

	if ( !list_rewind( fixed_asset_purchase_list ) )
	{
		pclose( output_pipe );
		return;
	}

	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_list );

		if ( !fixed_asset_purchase->
			fixed_asset_purchase_depreciation )
		{
			continue;
		}

		fprintf( output_pipe,
			 "%s^%s^%s/%s^%s^%s^%.2lf\n",
			 fixed_asset_purchase->asset_name,
			 fixed_asset_purchase->serial_label,
			 fixed_asset_purchase->vendor_entity->full_name,
			 fixed_asset_purchase->vendor_entity->street_address,
			 fixed_asset_purchase->service_placement_date,
			 fixed_asset_purchase->
				fixed_asset_purchase_depreciation->
				depreciation_date,
			 fixed_asset_purchase->
				fixed_asset_purchase_depreciation->
				depreciation_amount );

	} while( list_next( fixed_asset_purchase_list ) );
	pclose( output_pipe );
}

