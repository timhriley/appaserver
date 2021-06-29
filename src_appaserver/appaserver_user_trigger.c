/* ---------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/appaserver_user_trigger.c	*/
/* ---------------------------------------------------------	*/
/* 								*/
/* Freely available software: see Appaserver.org		*/
/* ---------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_error.h"
#include "appaserver_user.h"

/* Prototypes */
/* ---------- */
void appaserver_user_trigger_insert_update(
			APPASERVER_USER *appaserver_user );

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *state;
	APPASERVER_USER *appaserver_user;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s login_name state\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	state = argv[ 2 ];

	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) == 0 )
	{
		if ( ! ( appaserver_user =
				appaserver_user_fetch(
					login_name ) ) )
		{
			printf(
		"<h3>Warning: appaserver_user_fetch() returned empty.</h3>\n" );

			exit( 0 );
		}

		appaserver_user_trigger_insert_update( appaserver_user );
	}

	return 0;
}

void appaserver_user_trigger_insert_update(
			APPASERVER_USER *appaserver_user )
{
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

