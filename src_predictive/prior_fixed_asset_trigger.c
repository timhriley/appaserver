/* -------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/prior_fixed_asset_trigger.c		*/
/* -------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------	*/

/* --------------------------------
Has a 1:1 TRANSACTION
1) Need preupdate_full_name
2) Need preupdate_street_address
3) Need preupdate_foreign_date_time
4) Ignore the delete state
----------------------------------- */

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
	PRIOR_FIXED_ASSET *prior_fixed_asset;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf(stderr,
"Usage: %s asset_name state preupdate_full_name preupdate_street_address preupdate_purchase_date_time\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	asset_name = argv[ 1 ];
	state = argv[ 2 ];
	preupdate_full_name = argv[ 3 ];
	preupdate_street_address = argv[ 4 ];
	preupdate_purchase_date_time = argv[ 5 ];

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
			preupdate_purchase_date_time );

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

