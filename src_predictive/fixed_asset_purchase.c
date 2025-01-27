/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_asset_purchase.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
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
#include "entity.h"
#include "entity_self.h"
#include "account.h"
#include "fixed_asset.h"
#include "depreciation.h"
#include "purchase.h"
#include "recovery.h"
#include "fixed_asset_purchase.h"

FIXED_ASSET_PURCHASE *fixed_asset_purchase_calloc( void )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( ! ( fixed_asset_purchase =
			calloc( 1, sizeof ( FIXED_ASSET_PURCHASE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return fixed_asset_purchase;
}

FIXED_ASSET_PURCHASE *fixed_asset_purchase_new(
		char *asset_name,
		char *serial_label )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( !asset_name
	||   !serial_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	fixed_asset_purchase = fixed_asset_purchase_calloc();

	fixed_asset_purchase->asset_name = asset_name;
	fixed_asset_purchase->serial_label = serial_label;

	fixed_asset_purchase->fixed_asset =
		fixed_asset_fetch(
			fixed_asset_purchase->asset_name );

	return fixed_asset_purchase;
}

FIXED_ASSET_PURCHASE *fixed_asset_purchase_parse(
		char *input,
		boolean fetch_last_depreciation,
		boolean fetch_last_recovery )
{
	char asset_name[ 128 ];
	char serial_label[ 128 ];
	char full_name[ 128 ];
	char street_address[ 128 ];
	char piece_buffer[ 1024 ];
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( !input || !*input ) return (FIXED_ASSET_PURCHASE *)0;

	/* See attribute_list fixed_asset_purchase */
	/* --------------------------------------- */
	piece( asset_name, SQL_DELIMITER, input, 0 );
	piece( serial_label, SQL_DELIMITER, input, 1 );

	fixed_asset_purchase =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		fixed_asset_purchase_new(
			strdup( asset_name ),
			strdup( serial_label ) );

	if ( !fixed_asset_purchase->fixed_asset )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: fixed_asset_purchase->fixed_asset is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	piece( full_name, SQL_DELIMITER, input, 2 );
	piece( street_address, SQL_DELIMITER, input, 3 );

	fixed_asset_purchase->vendor_entity =
		entity_new(
			strdup( full_name ),
			strdup( street_address ) );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	if ( *piece_buffer )
		fixed_asset_purchase->purchase_date_time =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 5 );
	if ( *piece_buffer )
		fixed_asset_purchase->service_placement_date =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 6 );
	if ( *piece_buffer )
		fixed_asset_purchase->fixed_asset_cost =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 7 );
	if ( *piece_buffer )
		fixed_asset_purchase->units_produced_so_far =
			atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 8 );
	if ( *piece_buffer )
		fixed_asset_purchase->disposal_date =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 9 );
	if ( *piece_buffer )
		fixed_asset_purchase->recovery_class_year_string =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 10 );
	if ( *piece_buffer )
		fixed_asset_purchase->recovery_method =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 11 );
	if ( *piece_buffer )
		fixed_asset_purchase->recovery_convention =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 12 );
	if ( *piece_buffer )
		fixed_asset_purchase->recovery_system =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 13 );
	if ( *piece_buffer )
		fixed_asset_purchase->depreciation_method =
			depreciation_method_evaluate( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 14 );
	if ( *piece_buffer )
		fixed_asset_purchase->estimated_useful_life_years =
			atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 15 );
	if ( *piece_buffer )
		fixed_asset_purchase->estimated_useful_life_units =
			atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 16 );
	if ( *piece_buffer )
		fixed_asset_purchase->estimated_residual_value =
			atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 17 );
	if ( *piece_buffer )
		fixed_asset_purchase->declining_balance_n =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 18 );
	if ( *piece_buffer )
		fixed_asset_purchase->cost_basis =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 19 );
	if ( *piece_buffer )
		fixed_asset_purchase->finance_accumulated_depreciation =
			atof( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 20 );
	if ( *piece_buffer )
		fixed_asset_purchase->tax_adjusted_basis =
			atof( piece_buffer );

	if ( fetch_last_depreciation )
	{
		fixed_asset_purchase->last_depreciation =
			depreciation_fetch(
				fixed_asset_purchase->
					fixed_asset->
					asset_name,
				fixed_asset_purchase->
					serial_label,
				depreciation_prior_depreciation_date(
					DEPRECIATION_TABLE,
					fixed_asset_purchase->
						fixed_asset->
						asset_name,
					fixed_asset_purchase->
						serial_label,
					(char *)0
					    /* depreciation_date */ ) );
	}

	if ( fetch_last_recovery )
	{
		fixed_asset_purchase->last_recovery =
			recovery_fetch(
				fixed_asset_purchase->
					fixed_asset->
					asset_name,
				fixed_asset_purchase->
					serial_label,
				recovery_prior_tax_year(
					RECOVERY_TABLE ) );
	}

	return fixed_asset_purchase;
}

char *fixed_asset_purchase_system_string(
		char *fixed_asset_purchase_table,
		char *where,
		char *order )
{
	char system_string[ 2048 ];

	if ( !fixed_asset_purchase_table
	||   !where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh '*' %s \"%s\" \"%s\"",
		 fixed_asset_purchase_table,
		 where,
		 (order) ? order : "" );

	return strdup( system_string );
}

LIST *fixed_asset_purchase_list_fetch(
		char *where,
		boolean fetch_last_depreciation,
		boolean fetch_last_recovery )
{
	return fixed_asset_purchase_system_list(
			fixed_asset_purchase_system_string(
				FIXED_ASSET_PURCHASE_TABLE,
				where,
				"service_placement_date" /* order */ ),
		fetch_last_depreciation,
		fetch_last_recovery );
}

FILE *fixed_asset_purchase_input_pipe( char *system_string )
{
	return popen( system_string, "r" );
}

LIST *fixed_asset_purchase_system_list(
		char *system_string,
		boolean fetch_last_depreciation,
		boolean fetch_last_recovery )
{
	char input[ 2048 ];
	FILE *input_pipe;
	LIST *list = list_new();

	input_pipe =
		fixed_asset_purchase_input_pipe(
			system_string );

	while ( string_input( input, input_pipe, 2048 ) )
	{
		list_set(
			list,
			fixed_asset_purchase_parse(
				input,
				fetch_last_depreciation,
				fetch_last_recovery ) );
	}

	pclose( input_pipe );

	return list;
}

FILE *fixed_asset_purchase_update_pipe(
		char *table,
		char *primary_key )
{
	char system_string[ 1024 ];

	sprintf( system_string,
		 "update_statement.e table=%s key=%s carrot=y | sql",
		 table,
		 primary_key );

	return popen( system_string, "w" );
}

void fixed_asset_purchase_update_execute(
		FILE *update_pipe,
		double cost_basis,
		double finance_accumulated_depreciation,
		double tax_adjusted_basis,
		char *asset_name,
		char *serial_label )
{
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
		"%s^%s^tax_adjusted_basis^%.2lf\n",
		escape,
		serial_label,
		tax_adjusted_basis );
}

FIXED_ASSET_PURCHASE *fixed_asset_purchase_fetch(
		char *asset_name,
		char *serial_label,
		boolean fetch_last_depreciation,
		boolean fetch_last_recovery )
{
	return
	fixed_asset_purchase_parse(
		string_pipe_fetch(
			fixed_asset_purchase_system_string(
				FIXED_ASSET_PURCHASE_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				fixed_asset_purchase_primary_where(
					asset_name,
					serial_label ),
				(char *)0 /* order */ ) ),
		fetch_last_depreciation,
		fetch_last_recovery );
}

char *fixed_asset_purchase_primary_where(
		char *asset_name,
		char *serial_label )
{
	static char where[ 256 ];

	if ( !asset_name
	||   !serial_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

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
		LIST *fixed_asset_purchase_list,
		char *depreciation_date )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;
	ENTITY_SELF *entity_self;
	char *depreciation_expense;
	char *accumulated_depreciation;

	if ( !list_rewind( fixed_asset_purchase_list ) ) return (LIST *)0;

	entity_self =
		entity_self_fetch(
			0 /* not fetch_entity_boolean */ );

	if ( !entity_self )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: entity_self_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	depreciation_expense =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		account_depreciation_expense(
			ACCOUNT_DEPRECIATION_KEY );

	accumulated_depreciation =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		account_accumulated_depreciation(
			ACCOUNT_ACCUMULATED_KEY );

	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_list );

		fixed_asset_purchase->depreciation =
			depreciation_evaluate(
				fixed_asset_purchase->fixed_asset->asset_name,
				fixed_asset_purchase->serial_label,
				fixed_asset_purchase->depreciation_method,
				fixed_asset_purchase->service_placement_date,
				/* ------------------------------------ */
				/* Returns last_depreciation->		*/
				/*	   depreciation_date or null	*/
				/* ------------------------------------ */
				fixed_asset_purchase_prior_depreciation_date(
					fixed_asset_purchase->
						last_depreciation ),
				depreciation_date,
				fixed_asset_purchase->cost_basis,
				fixed_asset_purchase->units_produced_so_far,
				fixed_asset_purchase->estimated_residual_value,
				fixed_asset_purchase->
					estimated_useful_life_years,
				fixed_asset_purchase->
					estimated_useful_life_units,
				fixed_asset_purchase->declining_balance_n,
				fixed_asset_purchase->
					finance_accumulated_depreciation
					/* prior_accumulated_depreciation */ );

		if ( !fixed_asset_purchase->depreciation ) continue;

		fixed_asset_purchase->
			depreciation_transaction =
				depreciation_transaction(
					entity_self->full_name,
					entity_self->street_address,
					depreciation_date,
					fixed_asset_purchase->
						depreciation->
						amount,
					depreciation_expense,
					accumulated_depreciation );

		if ( !fixed_asset_purchase->
			depreciation_transaction )
		{
			continue;
		}

	} while ( list_next( fixed_asset_purchase_list ) );

	return fixed_asset_purchase_list;
}

void fixed_asset_purchase_list_update(
		LIST *fixed_asset_purchase_list )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( !list_rewind( fixed_asset_purchase_list ) ) return;

	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_list );

		fixed_asset_purchase_update( fixed_asset_purchase );

	} while( list_next( fixed_asset_purchase_list ) );
}

void fixed_asset_purchase_depreciation_display(
		LIST *fixed_asset_purchase_depreciation_list )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;
	FILE *output_pipe;
	char system_string[ 1024 ];
	char *heading;
	char *justification;
	char buffer[ 128 ];
	char cost[ 16 ];
	char prior_accumulated[ 16 ];
	char depreciation[ 16 ];
	char post_accumulated[ 16 ];

	if ( !list_rewind( fixed_asset_purchase_depreciation_list ) ) return;

	heading =
"Asset,Serial,Service,Cost,Prior Accumulated,Depreciation,Post Accumulated";

	justification = "left,left,left,right";

	sprintf(system_string,
		"html_table.e '' '%s' '^' '%s'",
		heading,
		justification );

	fflush( stdout );
	output_pipe = popen( system_string, "w" );

	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_depreciation_list );

		if ( !fixed_asset_purchase->depreciation ) continue;

		strcpy( cost,
			/* --------------------- */
			/* Returns static memory */
			/* Doesn't trim pennies  */
			/* --------------------- */
			commas_in_money(
				fixed_asset_purchase->
					cost_basis ) );

		strcpy(	prior_accumulated,
			commas_in_money(
				fixed_asset_purchase->
					finance_accumulated_depreciation ) );

		strcpy(	depreciation,
			commas_in_money(
				fixed_asset_purchase->
					depreciation->
					amount ) );

		strcpy(	post_accumulated,
			commas_in_money(
				fixed_asset_purchase->
					finance_accumulated_depreciation +
				fixed_asset_purchase->
					depreciation->
					amount ) );

		fprintf(output_pipe,
			"%s^%s^%s^%s^%s^%s^%s\n",
			format_initial_capital(
				buffer,
				fixed_asset_purchase->
					fixed_asset->
					asset_name ),
			fixed_asset_purchase->serial_label,
			fixed_asset_purchase->service_placement_date,
			cost,
			prior_accumulated,
			depreciation,
			post_accumulated );

	} while( list_next( fixed_asset_purchase_depreciation_list ) );

	pclose( output_pipe );
}

void fixed_asset_purchase_finance_fetch_update(
		char *asset_name,
		char *serial_label )
{
	char system_string[ 1024 ];
	char set[ 512 ];

	sprintf(set,
		"finance_accumulated_depreciation = 			"
		"( select sum( depreciation_amount )			"
		"	  from %s					"
		"	  where %s.asset_name = %s.asset_name		"
		"	    and %s.serial_label = %s.serial_label )	",
		DEPRECIATION_TABLE,
		FIXED_ASSET_PURCHASE_TABLE,
		DEPRECIATION_TABLE,
		FIXED_ASSET_PURCHASE_TABLE,
		DEPRECIATION_TABLE );

	sprintf(system_string,
		"echo \"update %s set %s where %s;\" | sql",
		FIXED_ASSET_PURCHASE_TABLE,
		set,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		fixed_asset_purchase_primary_where(
			asset_name,
			serial_label ) );

	if ( system( system_string ) ){}
}

void fixed_asset_purchase_cost_fetch_update(
		char *asset_name,
		char *serial_label )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"recovery_adjusted_basis_fetch_update.sh \"%s\" \"%s\"",
		asset_name,
		serial_label );

	if ( system( system_string ) ){};
}

LIST *fixed_asset_purchase_list_cost_recover(
		LIST *fixed_asset_purchase_list,
		int tax_year )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( !list_rewind( fixed_asset_purchase_list ) ) return (LIST *)0;

	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_list );

		if ( !fixed_asset_purchase->fixed_asset )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty fixed_asset.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		fixed_asset_purchase->recovery =
			recovery_evaluate(
				fixed_asset_purchase->fixed_asset->asset_name,
				fixed_asset_purchase->serial_label,
				tax_year,
				fixed_asset_purchase->cost_basis,
				fixed_asset_purchase->
					service_placement_date
					/* service_placement_date_string */,
				fixed_asset_purchase->
					disposal_date
					/* disposal_date_string */,
				fixed_asset_purchase->
					recovery_class_year_string,
				fixed_asset_purchase->
					recovery_method,
				fixed_asset_purchase->
					recovery_convention,
				fixed_asset_purchase->
					recovery_system );

	} while ( list_next( fixed_asset_purchase_list ) );

	return fixed_asset_purchase_list;
}

LIST *fixed_asset_purchase_cost_recovery_list(
		LIST *fixed_asset_purchase_list )
{
	LIST *recovery_list;
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( !list_rewind( fixed_asset_purchase_list ) ) return (LIST *)0;

	recovery_list = list_new();

	do {
		fixed_asset_purchase = list_get( fixed_asset_purchase_list );

		if ( fixed_asset_purchase->recovery
		&&   fixed_asset_purchase->recovery->amount )
		{
			list_set(
				recovery_list,
				fixed_asset_purchase->recovery );
		}

	} while ( list_next( fixed_asset_purchase_list ) );

	return recovery_list;
}

void fixed_asset_purchase_recovery_display(
		LIST *fixed_asset_purchase_list )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;
	FILE *output_pipe;
	char sys_string[ 1024 ];
	char *heading;
	char *justification;
	char buffer[ 128 ];
	char cost[ 128 ];
	char prior_adjusted[ 128 ];
	char amount[ 128 ];
	char adjusted_basis[ 128 ];

	if ( !list_rewind( fixed_asset_purchase_list ) ) return;

	heading =
"Asset,Serial,Service,Cost,Prior Adjusted,Amount,Rate,Adjusted Basis";

	justification = "left,left,right";

	sprintf( sys_string,
		 "html_table.e '' '%s' '^' '%s'",
		 heading,
		 justification );

	fflush( stdout );
	output_pipe = popen( sys_string, "w" );

	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_list );

		if ( !fixed_asset_purchase->recovery ) continue;

		strcpy(	cost,
			/* --------------------- */
			/* Returns static memory */
			/* Doesn't trim pennies  */
			/* --------------------- */
			commas_in_money(
				fixed_asset_purchase->cost_basis ) );

		strcpy(	prior_adjusted,
			commas_in_money(
				fixed_asset_purchase->tax_adjusted_basis ) );

		strcpy(	amount,
			commas_in_money(
				fixed_asset_purchase->
					recovery->
					amount ) );

		sprintf(adjusted_basis,
			"%s",
			commas_in_money(
				fixed_asset_purchase->
					tax_adjusted_basis -
				fixed_asset_purchase->
					recovery->
					amount ) );
		fprintf(output_pipe,
			"%s^%s^%s^%s^%s^%s^%.5lf^%s\n",
			format_initial_capital(
				buffer,
				fixed_asset_purchase->
					fixed_asset->
					asset_name ),
			fixed_asset_purchase->
				serial_label,
			fixed_asset_purchase->
				service_placement_date,
			cost,
			prior_adjusted,
			amount,
			fixed_asset_purchase->
				recovery->
				rate,
			adjusted_basis );

	} while( list_next( fixed_asset_purchase_list ) );

	pclose( output_pipe );
}

void fixed_asset_purchase_list_add_depreciation_amount(
			LIST *fixed_asset_purchase_list )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( !list_rewind( fixed_asset_purchase_list ) ) return;

	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_list );

		if ( fixed_asset_purchase->depreciation )
		{
			fixed_asset_purchase->
				finance_accumulated_depreciation +=
					fixed_asset_purchase->
						depreciation->
						amount;
		}

	} while( list_next( fixed_asset_purchase_list ) );
}

void fixed_asset_purchase_list_subtract_recovery_amount(
			LIST *fixed_asset_purchase_list )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( !list_rewind( fixed_asset_purchase_list ) ) return;

	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_list );

		if ( fixed_asset_purchase->recovery )
		{
			fixed_asset_purchase_subtract_recovery_amount(
				fixed_asset_purchase );

		}

	} while( list_next( fixed_asset_purchase_list ) );
}

void fixed_asset_purchase_subtract_recovery_amount(
		FIXED_ASSET_PURCHASE *fixed_asset_purchase )
{
	if ( fixed_asset_purchase->recovery )
	{
		fixed_asset_purchase->
			tax_adjusted_basis -=
				fixed_asset_purchase->
					recovery->
					amount;
	}
}

void fixed_asset_purchase_negate_depreciation_amount(
		LIST *fixed_asset_purchase_list )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( list_rewind( fixed_asset_purchase_list ) )
	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_list );

		if ( fixed_asset_purchase->depreciation )
		{
			fixed_asset_purchase->
				finance_accumulated_depreciation -=
					fixed_asset_purchase->
						depreciation->
						amount;
		}

	} while( list_next( fixed_asset_purchase_list ) );
}

void fixed_asset_purchase_list_negate_recovery_amount(
		LIST *fixed_asset_purchase_list )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;

	if ( list_rewind( fixed_asset_purchase_list ) )
	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_list );

		if ( fixed_asset_purchase->recovery )
		{
			fixed_asset_purchase->
				tax_adjusted_basis +=
					fixed_asset_purchase->
						recovery->
						amount;
		}

	} while( list_next( fixed_asset_purchase_list ) );
}

char *fixed_asset_purchase_depreciation_where( char *depreciation_date )
{
	char where[ 1024 ];

	if ( !depreciation_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: depreciation_date is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
	"ifnull(finance_accumulated_depreciation,0) -		"
	"ifnull(estimated_residual_value,0) < cost_basis	"
	" and disposal_date is null				"
	" and not %s						",
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		depreciation_subquery_exists_where(
			DEPRECIATION_TABLE,
			FIXED_ASSET_PURCHASE_TABLE,
			depreciation_date ) );

	return strdup( where );
}

char *fixed_asset_purchase_depreciation_date( void )
{
	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	date_now_yyyy_mm_dd( date_utc_offset() );
}

void fixed_asset_purchase_update(
		FIXED_ASSET_PURCHASE *fixed_asset_purchase )
{
	FILE *update_pipe;

	if ( !fixed_asset_purchase )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: fixed_asset_purchase is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	update_pipe =
		fixed_asset_purchase_update_pipe(
			FIXED_ASSET_PURCHASE_TABLE,
			FIXED_ASSET_PURCHASE_PRIMARY_KEY );

	fixed_asset_purchase_update_execute(
		update_pipe,
		fixed_asset_purchase->cost_basis,
		fixed_asset_purchase->finance_accumulated_depreciation,
		fixed_asset_purchase->tax_adjusted_basis,
		fixed_asset_purchase->fixed_asset->asset_name,
		fixed_asset_purchase->serial_label );

	pclose( update_pipe );
}

double fixed_asset_purchase_cost_basis( double fixed_asset_cost )
{
	return fixed_asset_cost;
}

double fixed_asset_purchase_tax_adjusted_basis( double fixed_asset_cost )
{
	return fixed_asset_cost;
}

char *fixed_asset_purchase_prior_depreciation_date(
		DEPRECIATION *last_depreciation )
{
	if ( last_depreciation )
		return last_depreciation->depreciation_date;
	else
		return (char *)0;
}

LIST *fixed_asset_purchase_depreciation_list_extract(
		LIST *fixed_asset_purchase_depreciation_list )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;
	LIST *depreciation_list;

	if ( !list_rewind( fixed_asset_purchase_depreciation_list ) )
		return (LIST *)0;

	depreciation_list = list_new();

	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_depreciation_list );

		if ( fixed_asset_purchase->depreciation
		&&   fixed_asset_purchase->depreciation_transaction )
		{
			fixed_asset_purchase->
				depreciation->
				transaction_date_time =
					fixed_asset_purchase->
						depreciation_transaction->
						transaction_date_time;

			list_set(
				depreciation_list,
				fixed_asset_purchase->depreciation );
		}

	} while ( list_next( fixed_asset_purchase_depreciation_list ) );

	return depreciation_list;
}

LIST *fixed_asset_purchase_transaction_list_extract(
		LIST *fixed_asset_purchase_depreciation_list )
{
	FIXED_ASSET_PURCHASE *fixed_asset_purchase;
	LIST *transaction_list;

	if ( !list_rewind( fixed_asset_purchase_depreciation_list ) )
		return (LIST *)0;

	transaction_list = list_new();

	do {
		fixed_asset_purchase =
			list_get(
				fixed_asset_purchase_depreciation_list );

		if ( fixed_asset_purchase->depreciation_transaction )
		{
			list_set(
				transaction_list,
				fixed_asset_purchase->
					depreciation_transaction );
		}

	} while ( list_next( fixed_asset_purchase_depreciation_list ) );

	return transaction_list;
}

void fixed_asset_purchase_transaction_list_insert(
		LIST *transaction_list_extract )
{
	/* May reset transaction->transaction_date_time */
	/* -------------------------------------------- */
	transaction_list_insert(
		transaction_list_extract,
		1 /* insert_journal_list_boolean */,
		1 /* transaction_lock_boolean */ );
}

void fixed_asset_purchase_depreciation_list_insert(
		LIST *depreciation_list_extract )
{
	depreciation_list_insert( depreciation_list_extract );
}

void fixed_asset_purchase_depreciation_insert(
		LIST *fixed_asset_purchase_depreciation_list )
{
	LIST *transaction_list_extract;

	transaction_list_extract =
		fixed_asset_purchase_transaction_list_extract(
			fixed_asset_purchase_depreciation_list );

	if ( list_length( transaction_list_extract ) )
	{
		LIST *depreciation_list_extract;

		/* May reset transaction->transaction_date_time */
		/* -------------------------------------------- */
		fixed_asset_purchase_transaction_list_insert(
			transaction_list_extract );

		depreciation_list_extract =
			fixed_asset_purchase_depreciation_list_extract(
				fixed_asset_purchase_depreciation_list );

		fixed_asset_purchase_depreciation_list_insert(
			depreciation_list_extract );
	}

	transaction_list_html_display( transaction_list_extract );
}

