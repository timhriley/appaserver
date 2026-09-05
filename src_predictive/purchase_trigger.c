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
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *purchase_date_time;
	char *state;
	int row_number;
	int row_count;
	PURCHASE *purchase;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 11 )
	{
		fprintf(stderr,
"Usage: %s preupdate_fund_name preupdate_full_name preupdate_contact_key fund_name full_name contact_key purchase_date_time state row_number row_count\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	preupdate_fund_name = argv[ 1 ];
	preupdate_full_name = argv[ 2 ];
	preupdate_contact_key = argv[ 3 ];
	fund_name = argv[ 4 ];
	full_name = argv[ 5 ];
	contact_key = argv[ 6 ];
	purchase_date_time = argv[ 7 ];
	state = argv[ 8 ];
	row_number = atoi( argv[ 9 ] );
	row_count = atoi( argv[ 10 ] );

	if ( row_number == row_count )
	{
		purchase =
			purchase_trigger_new(
				preupdate_fund_name,
				preupdate_full_name,
				preupdate_contact_key,
				fund_name,
				full_name,
				contact_key,
				purchase_date_time,
				state );

		if ( purchase )
		{
			(void)purchase_update_execute(
				application_name
					/* for update_statement_execute */,
				purchase->purchase_update );
		}
	}

	return 0;
}

