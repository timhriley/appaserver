/* -------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/fixed_asset_home_trigger.c		*/
/* -------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------	*/

/* --------------------------------
1) Has a 1:1 TRANSACTION
2) Need preupdate_full_name
3) Need preupdate_foreign_date_time
4) Ignore the delete state
----------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "environ.h"
#include "piece.h"
#include "sql.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "fixed_asset_home.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *asset_name;
	char *state;
	char *preupdate_full_name;
	char *preupdate_purchase_date_time;
	FIXED_ASSET_HOME *fixed_asset_home;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf(stderr,
"Usage: %s asset_name state preupdate_full_name preupdate_purchase_date_time\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	asset_name = argv[ 1 ];
	state = argv[ 2 ];
	preupdate_full_name = argv[ 3 ];
	preupdate_purchase_date_time = argv[ 4 ];

	if ( strcmp( state, APPASERVER_DELETE_STATE ) == 0 ) exit( 0 );

	fixed_asset_home =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		fixed_asset_home_fetch(
			asset_name,
			state,
			preupdate_full_name,
			preupdate_purchase_date_time );

	(void)subsidiary_transaction_execute(
		application_name,
		fixed_asset_home->
			subsidiary_transaction->
				delete_transaction,
		fixed_asset_home->
			subsidiary_transaction->
				insert_transaction,
		fixed_asset_home->
			subsidiary_transaction->
			update_template,
		fixed_asset_home->
			subsidiary_transaction->
			update_null_sql,
		0 /* not predictive_fund_boolean */,
		0 /* not entity_contact_key_boolean */ );

	return 0;
}

