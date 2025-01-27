/* -------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/prior_fixed_asset_trigger.c		*/
/* -------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "environ.h"
#include "piece.h"
#include "sql.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "prior_fixed_asset.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *asset_name;
	char *state;
	char *preupdate_full_name;
	char *preupdate_street_address;
	char *preupdate_purchase_date_time;
	char *preupdate_fixed_asset_cost;
	char *preupdate_asset_account;
	PRIOR_FIXED_ASSET *prior_fixed_asset;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf(stderr,
"Usage: %s asset_name state preupdate_full_name preupdate_street_address preupdate_purchase_date_time preupdate_fixed_asset_cost preupdate_asset_account\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	asset_name = argv[ 1 ];
	state = argv[ 2 ];
	preupdate_full_name = argv[ 3 ];
	preupdate_street_address = argv[ 4 ];
	preupdate_purchase_date_time = argv[ 5 ];
	preupdate_fixed_asset_cost = argv[ 6 ];
	preupdate_asset_account = argv[ 7 ];

	if ( strcmp( state, APPASERVER_DELETE_STATE ) == 0 ) exit( 0 );

	prior_fixed_asset =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		prior_fixed_asset_fetch(
			asset_name,
			state,
			preupdate_full_name,
			preupdate_street_address,
			preupdate_purchase_date_time,
			preupdate_fixed_asset_cost,
			preupdate_asset_account );

	subsidiary_transaction_execute(
		application_name,
		prior_fixed_asset->
			subsidiary_transaction->
				delete_transaction,
		prior_fixed_asset->
			subsidiary_transaction->
				insert_transaction,
		prior_fixed_asset->
			subsidiary_transaction->
			update_template );

	return 0;
}

