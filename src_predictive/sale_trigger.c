/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/sale_trigger.c			*/
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
	int row_number;
	int row_count;
	SALE *sale;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 12  )
	{
		fprintf(stderr,
"Usage: %s fund_name full_name contact_key sale_date_time state preupdate_fund_name preupdate_full_name preupdate_contact_key preupdate_uncollectible_date_time row_number row_count\n",
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
	row_number = atoi( argv[ 10 ] );
	row_count = atoi( argv[ 11 ] );

	if ( strcmp( state, APPASERVER_DELETE_STATE ) == 0 ) exit( 0 );

	if ( row_number == row_count )
	{
		sale =
			sale_trigger_new(
				preupdate_fund_name,
				preupdate_full_name,
				preupdate_contact_key,
				preupdate_uncollectible_date_time,
				fund_name,
				full_name,
				contact_key,
				sale_date_time,
				state );
	
		if ( !sale ) exit( 0 );

		(void)sale_update_execute(
			application_name /* for transaction_update */,
			sale->sale_update,
			sale->sale_transaction,
			sale->sale_loss_transaction );
	}

	return 0;
}
