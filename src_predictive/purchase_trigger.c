/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/purchase_trigger.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include "environ.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "purchase.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *preupdate_fund_name;
	char *preupdate_full_name;
	char *preupdate_contact_key;
	char *preupdate_purchase_date_time;
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *purchase_date_time;
	char *table_name;
	char *state;
	int row_number;
	int row_count;
	PURCHASE *purchase;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 13 )
	{
		fprintf(stderr,
"Usage: %s preupdate_fund_name preupdate_full_name preupdate_contact_key preupdate_purchase_date_time fund_name full_name contact_key purchase_date_time table state row_number row_count\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	preupdate_fund_name = argv[ 1 ];
	preupdate_full_name = argv[ 2 ];
	preupdate_contact_key = argv[ 3 ];
	preupdate_purchase_date_time = argv[ 4 ];
	fund_name = argv[ 5 ];
	full_name = argv[ 6 ];
	contact_key = argv[ 7 ];
	purchase_date_time = argv[ 8 ];
	table_name = argv[ 9 ];
	state = argv[ 10 ];
	row_number = atoi( argv[ 11 ] );
	row_count = atoi( argv[ 12 ] );

	if ( row_number == row_count )
	{
		purchase =
			purchase_trigger_new(
				preupdate_fund_name,
				preupdate_full_name,
				preupdate_contact_key,
				preupdate_purchase_date_time,
				fund_name,
				full_name,
				contact_key,
				purchase_date_time,
				table_name,
				state );

		if ( purchase )
		{
			(void)purchase_update_execute(
				application_name
					/* for update_statement_execute */,
				purchase->purchase_transaction,
				purchase->purchase_update );
		}
	}

	return 0;
}

