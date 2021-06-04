/* --------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/depreciation_trigger.c	*/
/* --------------------------------------------------------	*/
/* 								*/
/* Freely available software: see Appaserver.org		*/
/* --------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_error.h"
#include "entity_self.h"
#include "account.h"
#include "predictive.h"
#include "fixed_asset_purchase.h"
#include "depreciation.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void depreciation_trigger_insert(
			FIXED_ASSET_PURCHASE *fixed_asset_purchase,
			char *depreciation_date );

void depreciation_trigger_update(
			DEPRECIATION *depreciation );

void depreciation_trigger_predelete(
			DEPRECIATION *depreciation );

void depreciation_trigger_delete(
			char *asset_name,
			char *serial_label );

int main( int argc, char **argv )
{
	char *application_name;
	char *asset_name;
	char *serial_label;
	char *depreciation_date;
	char *state;
	DEPRECIATION *depreciation = {0};
	FIXED_ASSET_PURCHASE *fixed_asset_purchase = {0};

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf( stderr,
		"Usage: %s asset_name serial_label depreciation_date state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	asset_name = argv[ 1 ];
	serial_label = argv[ 2 ];
	depreciation_date = argv[ 3 ];
	state = argv[ 4 ];

	if ( strcmp( state, "insert" ) == 0 )
	{
		if ( ! ( fixed_asset_purchase =
				fixed_asset_purchase_fetch(
					asset_name,
					serial_label,
					0 /* not fetch_last_depreciation */,
					0 /* not fetch_last_recovery */  ) ) )
		{
			printf(
		"<h3>ERROR: fixed_asset_purchase() returned empty.</h3>\n" );

			exit( 0 );
		}
	}

	if ( strcmp( state, "update" ) == 0
	||   strcmp( state, "predelete" ) == 0 )
	{
		if ( ! ( depreciation =
				depreciation_fetch(
					asset_name,
					serial_label,
					depreciation_date ) ) )
		{
			printf(
		"<h3>ERROR: depreciation_fetch() returned empty.</h3>\n" );

			exit( 0 );
		}
	}

	if ( strcmp( state, "insert" ) == 0 )
	{
		depreciation_trigger_insert(
			fixed_asset_purchase,
			depreciation_date );
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		depreciation_trigger_update( depreciation );
	}
	else
	if ( strcmp( state, "predelete" ) == 0 )
	{
		depreciation_trigger_predelete( depreciation );
	}
	else
	if ( strcmp( state, "delete" ) == 0 )
	{
		depreciation_trigger_delete(
			asset_name,
			serial_label );
	}
	else

	return 0;
}

void depreciation_trigger_insert(
			FIXED_ASSET_PURCHASE *fixed_asset_purchase,
			char *depreciation_date )
{
	ENTITY_SELF *entity_self;

	if ( ! ( entity_self = entity_self_fetch() ) )
	{
		printf(
		"<h3>Error: entity_self_fetch() returned empty.</h3>\n" );
		return;
	}

	fixed_asset_purchase->depreciation =
		depreciation_evaluate(
			fixed_asset_purchase->fixed_asset->asset_name,
			fixed_asset_purchase->serial_label,
			fixed_asset_purchase->depreciation_method,
			fixed_asset_purchase->service_placement_date,
			depreciation_prior_depreciation_date(
				fixed_asset_purchase->
					fixed_asset->
					asset_name,
				fixed_asset_purchase->
					serial_label,
				depreciation_date
					/* current_depreciation_date */ ),
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

	if ( !fixed_asset_purchase->depreciation )
	{
		printf( "<h3>Depreciation not generated.</h3>\n" );
		return;
	}

	fixed_asset_purchase->
		depreciation->
		depreciation_transaction =
			depreciation_transaction(
				entity_self->entity->full_name,
				entity_self->entity->street_address,
				depreciation_date,
				fixed_asset_purchase->
					depreciation->
					depreciation_amount,
				account_depreciation_expense(
					(char *)0 /* fund_name */ ),
				account_accumulated_depreciation(
					(char *)0 /* fund_name */ ) );

	if ( !fixed_asset_purchase->
		depreciation->
		depreciation_transaction )
	{
		printf( "<h3>Transaction not generated.</h3>\n" );
		return;
	}

	transaction_refresh(
		fixed_asset_purchase->
			depreciation->
				depreciation_transaction->
				full_name,
		fixed_asset_purchase->
			depreciation->
				depreciation_transaction->
				street_address,
		fixed_asset_purchase->
			depreciation->
				depreciation_transaction->
				transaction_date_time,
		fixed_asset_purchase->
			depreciation->
				depreciation_transaction->
				transaction_amount,
		fixed_asset_purchase->
			depreciation->
				depreciation_transaction->
				memo,
		0 /* check_number */,
		fixed_asset_purchase->
			depreciation->
				depreciation_transaction->
				lock_transaction,
		fixed_asset_purchase->
			depreciation->
				depreciation_transaction->
				journal_list );

	depreciation_update(
		fixed_asset_purchase->
			depreciation->
			units_produced_current,
		fixed_asset_purchase->
			depreciation->
			depreciation_amount,
		fixed_asset_purchase->
			depreciation->
			depreciation_transaction->
			full_name,
		fixed_asset_purchase->
			depreciation->
			depreciation_transaction->
			street_address,
		fixed_asset_purchase->
			depreciation->
			depreciation_transaction->
			transaction_date_time,
		fixed_asset_purchase->fixed_asset->asset_name,
		fixed_asset_purchase->serial_label,
		fixed_asset_purchase->depreciation->depreciation_date );

	fixed_asset_purchase_finance_fetch_update(
		fixed_asset_purchase->depreciation->asset_name,
		fixed_asset_purchase->depreciation->serial_label );
}

void depreciation_trigger_update(
			DEPRECIATION *depreciation )
{
	ENTITY_SELF *entity_self;

	entity_self = entity_self_fetch();

	if ( !depreciation->transaction_date_time
	||   !*depreciation->transaction_date_time )
	{
		depreciation->transaction_date_time =
			predictive_transaction_date_time(
				depreciation->depreciation_date );
	}

	depreciation->
		depreciation_transaction =
			depreciation_transaction(
				entity_self->entity->full_name,
				entity_self->entity->street_address,
				depreciation->transaction_date_time
					/* depreciation_date */,
				depreciation->depreciation_amount,
				account_depreciation_expense(
					(char *)0 /* fund_name */ ),
				account_accumulated_depreciation(
					(char *)0 /* fund_name */ ) );

	if ( !depreciation->depreciation_transaction )
	{
		printf(
	"<h2>ERROR: depreciation_transaction() returned empty.</h2>\n" );
		return;
	}

	transaction_refresh(
		depreciation->depreciation_transaction->full_name,
		depreciation->depreciation_transaction->street_address,
		depreciation->depreciation_transaction->transaction_date_time,
		depreciation->depreciation_transaction->transaction_amount,
		depreciation->depreciation_transaction->memo,
		0 /* check_number */,
		depreciation->depreciation_transaction->lock_transaction,
		depreciation->depreciation_transaction->journal_list );

	fixed_asset_purchase_finance_fetch_update(
		depreciation->asset_name,
		depreciation->serial_label );
}

void depreciation_trigger_predelete(
			DEPRECIATION *depreciation )
{
	if ( depreciation->transaction_date_time
	&&   *depreciation->transaction_date_time )
	{
		/* Performs journal_propagate() */
		/* ---------------------------- */
		transaction_delete(
			depreciation->
				entity_self->
				entity->
				full_name,
			depreciation->
				entity_self->
				entity->
				street_address,
			depreciation->
				transaction_date_time );
	}
}

void depreciation_trigger_delete(
			char *asset_name,
			char *serial_label )
{
	fixed_asset_purchase_finance_fetch_update(
		asset_name,
		serial_label );
}

