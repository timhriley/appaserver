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

	if ( argc != 8 )
	{
		fprintf(stderr,
"Usage: %s fund_name full_name contact_key purchase_date_time state row_number row_count\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	fund_name = argv[ 1 ];
	full_name = argv[ 2 ];
	contact_key = argv[ 3 ];
	purchase_date_time = argv[ 4 ];
	state = argv[ 5 ];
	row_number = atoi( argv[ 6 ] );
	row_count = atoi( argv[ 7 ] );

	/* If just changed 1:m primary key */
	/* ------------------------------- */
	if ( strcmp(
		purchase_date_time,
		"purchase_date_time" ) == 0 )
			exit( 0 );

	if ( strcmp( state, APPASERVER_PREDELETE_STATE ) == 0 ) exit( 0 );

	if ( row_number == row_count )
	{
		purchase =
			purchase_trigger_new(
				fund_name,
				full_name,
				contact_key,
				purchase_date_time,
				state,
				(char *)0 /* preupdate_fund_name */,
				(char *)0 /* preupdate_full_name */,
				(char *)0 /* preupdate_contact_key */ );

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

