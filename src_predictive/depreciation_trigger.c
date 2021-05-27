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
#include "fixed_asset_purchase.h"
#include "depreciation.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void depreciation_trigger_insert_update(
			DEPRECIATION *depreciation );

void depreciation_trigger_predelete(
			DEPRECIATION *depreciation );

int main( int argc, char **argv )
{
	char *application_name;
	char *asset_name;
	char *serial_label;
	char *depreciation_date;
	char *state;
	DEPRECIATION *depreciation;

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

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) == 0 )
	{
		depreciation_trigger_insert_update( depreciation );
	}
	else
	if ( strcmp( state, "predelete" ) == 0 )
	{
		depreciation_trigger_predelete( depreciation );
	}

	return 0;
}

void depreciation_trigger_insert_update(
			DEPRECIATION *depreciation )
{
	ENTITY_SELF *entity_self;

	entity_self = entity_self_fetch();

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
	if ( depreciation->depreciation_transaction )
	{
		/* Performs journal_propagate() */
		/* ---------------------------- */
		transaction_delete(
			depreciation->
				depreciation_transaction->
				full_name,
			depreciation->
				depreciation_transaction->
				street_address,
			depreciation->
				depreciation_transaction->
				transaction_date_time );

		depreciation->depreciation_transaction = (TRANSACTION *)0;
	}
}

