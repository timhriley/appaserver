/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/post_change_customer_sale.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "environ.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "sale.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *sale_date_time;
	char *state;
	char *preupdate_fund_name;
	char *preupdate_full_name;
	char *preupdate_contact_key;
	char *preupdate_uncollectible_date_time;
	SALE *sale;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 10  )
	{
		fprintf(stderr,
"Usage: %s fund_name full_name contact_key sale_date_time state preupdate_fund_name preupdate_full_name preupdate_contact_key preupdate_uncollectible_date_time\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	fund_name = argv[ 1 ];
	full_name = argv[ 2 ];
	contact_key = argv[ 3 ];
	sale_date_time = argv[ 4 ];
	state = argv[ 5 ];
	preupdate_fund_name = argv[ 6 ];
	preupdate_full_name = argv[ 7 ];
	preupdate_contact_key = argv[ 8 ];
	preupdate_uncollectible_date_time = argv[ 9 ];

	if ( strcmp( state, APPASERVER_DELETE_STATE ) == 0 ) exit( 0 );

	sale =
		sale_trigger_new(
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			state,
			preupdate_fund_name,
			preupdate_full_name,
			preupdate_contact_key,
			preupdate_uncollectible_date_time );

	if ( !sale ) exit( 0 );

	sale_update(
		SALE_TABLE,
		application_name /* for transaction_update */,
		sale->update_string_list,
		sale->sale_fetch->primary_key_list,
		sale->sale_transaction,
		sale->sale_loss_transaction );

	return 0;
}
