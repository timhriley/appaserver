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
#include "environ.h"
#include "tax_recovery.h"
#include "entity.h"
#include "entity_self.h"
#include "account.h"
#include "fixed_asset.h"
#include "depreciation.h"
#include "purchase.h"
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

FIXED_ASSET_PURCHASE *fixed_asset_purchase_parse(
			char *input,
			boolean fetch_depreciation_list )
{
	char asset_name[ 128 ];
	char serial_label[ 128 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char piece_buffer[ 1024 ];
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( !input || !*input ) return (FIXED_ASSET_PURCHASE *)0;

	piece( asset_name, SQL_DELIMITER, input, 0 );
	piece( serial_label, SQL_DELIMITER, input, 1 );

	fixed_asset_purchase =
		fixed_asset_purchase_new(
			strdup( asset_name ),
			strdup( serial_label ) );

	piece( full_name, SQL_DELIMITER, input, 2 );
	piece( street_address, SQL_DELIMITER, input, 3 );

	fixed_asset_purchase->vendor_entity =
		entity_new(
			strdup( full_name ),
			strdup( street_address ) );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	fixed_asset_purchase->purchase_date_time = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	fixed_asset_purchase->service_placement_date = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	fixed_asset_purchase->fixed_asset_cost = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	fixed_asset_purchase->disposal_date = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 8 );
	fixed_asset_purchase->depreciation_method =
		depreciation_method_resolve( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 9 );
	fixed_asset_purchase->estimated_useful_life_years =
		atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 10 );
	fixed_asset_purchase->estimated_useful_life_units =
		atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 11 );
	fixed_asset_purchase->estimated_residual_value = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 12 );
	fixed_asset_purchase->declining_balance_n = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 13 );
	fixed_asset_purchase->cost_basis = atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 14 );
	fixed_asset_purchase->finance_accumulated_depreciation =
		atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 15 );
	fixed_asset_purchase->tax_accumulated_depreciation =
		atoi( piece_buffer );

	if ( fetch_depreciation_list )
	{
		fixed_asset_purchase->depreciation_list =
			depreciation_list_fetch(
				fixed_asset_purchase->
					fixed_asset->
					asset_name,
				fixed_asset_purchase->
					fixed_asset->
					serial_label );
	}

	return fixed_asset_purchase;
}

char *fixed_asset_purchase_system_string(
			char *where,
			char *order )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '*' %s \"%s\" \"%s\"",
		 FIXED_ASSET_PURCHASE_TABLE,
		 where,
		 (order) ? order : "" );

	return strdup( system_string );
}

LIST *fixed_asset_purchase_list_fetch(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			boolean fetch_depreciation_list )
{
	return fixed_asset_purchase_system_list(
		fixed_asset_purchase_system_string(
			purchase_primary_where(
				full_name,
				street_address,
				purchase_date_time ),
			"service_placement_date"
				/* order */ ),
		fetch_depreciation_list );
}

LIST *fixed_asset_purchase_system_list(
			char *system_string,
			boolean fetch_depreciation_list )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *list = list_new();

	input_pipe = popen( system_string, "r" );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			list,
			fixed_asset_purchase_parse(
				input,
				fetch_depreciation_list ) );
	}
	pclose( input_pipe );
	return list;
}

FILE *fixed_asset_purchase_update_open( void )
{
	char system_string[ 1024 ];
	char *key;

	key =	"asset_name,"
		"serial_label";

	sprintf( system_string,
		 "update_statement.e table=%s key=%s carrot=y | sql",
		 FIXED_ASSET_PURCHASE_TABLE,
		 key );

	return popen( system_string, "w" );
}

void fixed_asset_purchase_update(
			double cost_basis,
			double finance_accumulated_depreciation,
			double tax_accumulated_depreciation,
			char *asset_name,
			char *serial_label )
{
	FILE *update_pipe = fixed_asset_purchase_update_open();
	char *escape = fixed_asset_name_escape( asset_name );

	fprintf(update_pipe,
		"%s^%s^cost_basis^%.2lf\n",
		escape,
		serial_label,
		cost_basis );

	fprintf(update_pipe,
		"%s^%s^finance_accumulated_depreciation^%.2lf\n",
		escape,
		serial_label,
		finance_accumulated_depreciation );

	fprintf(update_pipe,
		"%s^%s^tax_accumulated_depreciation^%.2lf\n",
		escape,
		serial_label,
		tax_accumulated_depreciation );

	pclose( update_pipe );
}

FIXED_ASSET_PURCHASE *fixed_asset_purchase_fetch(
			char *asset_name,
			char *serial_label,
			boolean fetch_depreciation_list )
{
	return fixed_asset_purchase_parse(
		string_pipe_fetch(
			fixed_asset_purchase_system_string(
				fixed_asset_purchase_primary_where(
					asset_name,
					serial_label ),
				(char *)0 /* order */ ) ),
		fetch_depreciation_list );
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
	double purchase_total;

	if ( !list_rewind( fixed_asset_purchase_list ) ) return 0.0;

	purchase_total = 0.0;

	do {
		fixed_asset_purchase = list_get( fixed_asset_purchase_list );
		purchase_total += fixed_asset_purchase->fixed_asset_cost;

	} while ( list_next( fixed_asset_purchase_list ) );

	return purchase_total;
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

		if ( fixed_asset_purchase->depreciation )
		{
			list_set(
				depreciation_list,
				fixed_asset_purchase->depreciation );
		}

	} while ( list_next( fixed_asset_purchase_list ) );
	return depreciation_list;
}

LIST *fixed_asset_purchase_list_depreciate(
			LIST *fixed_asset_purchase_list,
			char *depreciation_date )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;
	char *prior_depreciation_date;
	char *transaction_date_time = {0};
	ENTITY_SELF *entity_self;

	if ( !list_rewind( fixed_asset_purchase_list ) ) return (LIST *)0;

	entity_self = entity_self_fetch();

	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_list );

		fixed_asset_purchase->depreciation =
			depreciation_evaluate(
				fixed_asset_purchase->fixed_asset->asset_name,
				fixed_asset_purchase->fixed_asset->serial_label,
				fixed_asset_purchase->depreciation_method,
				fixed_asset_purchase->service_placement_date,
				depreciation_prior_depreciation_date(
					fixed_asset_purchase->
						fixed_asset->
						asset_name,
					fixed_asset_purchase->
						fixed_asset->
						serial_label ),
				depreciation_date,
				fixed_asset_purchase->cost_basis,
				fixed_asset_purchase->estimated_residual_value,
				fixed_asset_purchase->
					estimated_useful_life_years,
				fixed_asset_purchase->
					estimated_useful_life_units,
				fixed_asset_purchase->declining_balance_n,
				0 /* units_produced */,
				fixed_asset_purchase->
					finance_accumulated_depreciation
					/* prior_accumulated_depreciation */ );

		if ( !fixed_asset_purchase->depreciation ) continue;

		depreciation->depreciation_transaction =
			depreciation_transaction(
				entity_self->entity->full_name,
				entity_self->entity->street_address,
				depreciation_date,
				depreciation->depreciation_amount,
				account_depreciation_expense(
					(char *)0 /* fund_name */ ),
				account_accumulated_depreciation(
					(char *)0 /* fund_name */ ) );

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
		"service_placement_date",
		"depreciation_date",
		"depreciation_amount";

	justify_list_string = "left,left,left,left,right";

	sprintf( sys_string,
		 "html_table.e '^%s' '%s' '^' '%s'",
		 "Depreciate Fixed Asset",
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

		if ( !fixed_asset_purchase->depreciation )
		{
			continue;
		}

		fprintf( output_pipe,
			 "%s^%s^%s^%s^%.2lf\n",
			 fixed_asset_purchase->fixed_asset->asset_name,
			 fixed_asset_purchase->fixed_asset->serial_label,
			 fixed_asset_purchase->service_placement_date,
			 fixed_asset_purchase->
				depreciation->
				depreciation_date,
			 fixed_asset_purchase->
				depreciation->
				depreciation_amount );

	} while( list_next( fixed_asset_purchase_list ) );
	pclose( output_pipe );
}

