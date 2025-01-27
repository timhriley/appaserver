/* --------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/cost_recovery_trigger.c	*/
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
#include "fixed_asset_purchase.h"
#include "recovery.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void recovery_trigger_insert(
			FIXED_ASSET_PURCHASE *fixed_asset_purchase,
			int tax_year );

int main( int argc, char **argv )
{
	char *application_name;
	char *asset_name;
	char *serial_label;
	int tax_year;
	char *state;
	FIXED_ASSET_PURCHASE *fixed_asset_purchase = {0};

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf( stderr,
		"Usage: %s asset_name serial_label tax_year state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	asset_name = argv[ 1 ];
	serial_label = argv[ 2 ];
	tax_year = atoi( argv[ 3 ] );
	state = argv[ 4 ];

	if ( strcmp( state, "predelete" ) == 0 ) exit( 0 );

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

	if ( strcmp( state, "insert" ) == 0 )
	{
		recovery_trigger_insert(
			fixed_asset_purchase,
			tax_year );
	}
	else
	if ( strcmp( state, "update" ) == 0
	||   strcmp( state, "delete" ) == 0 )
	{
		fixed_asset_purchase_cost_fetch_update(
			asset_name,
			serial_label );
	}

	return 0;
}

void recovery_trigger_insert(
			FIXED_ASSET_PURCHASE *fixed_asset_purchase,
			int tax_year )
{
	FILE *update_pipe;

	fixed_asset_purchase->recovery =
		recovery_evaluate(
			fixed_asset_purchase->fixed_asset->asset_name,
			fixed_asset_purchase->serial_label,
			tax_year,
			fixed_asset_purchase->cost_basis,
			fixed_asset_purchase->service_placement_date,
			fixed_asset_purchase->disposal_date,
			fixed_asset_purchase->
				cost_recovery_period_string,
			fixed_asset_purchase->
				cost_recovery_method,
			fixed_asset_purchase->
				cost_recovery_conversion );

	if ( !fixed_asset_purchase->recovery
	||   !fixed_asset_purchase->recovery->recovery_amount )
	{
		printf(
		"<h3>Error: the cost recovery wasn't generated.</h3>\n" );

		return;
	}

	recovery_update(
		fixed_asset_purchase->recovery->recovery_amount,
		fixed_asset_purchase->recovery->recovery_rate,
		fixed_asset_purchase->fixed_asset->asset_name,
		fixed_asset_purchase->serial_label,
		tax_year );

	fixed_asset_purchase_subtract_recovery_amount(
		fixed_asset_purchase );

	update_pipe = fixed_asset_purchase_update_open();

	fixed_asset_purchase_update(
		update_pipe,
		fixed_asset_purchase->cost_basis,
		fixed_asset_purchase->finance_accumulated_depreciation,
		fixed_asset_purchase->tax_adjusted_basis,
		fixed_asset_purchase->fixed_asset->asset_name,
		fixed_asset_purchase->serial_label );

	pclose( update_pipe );
}

