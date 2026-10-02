/* -------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/specific_inventory_sale_trigger.c	*/
/* -------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver.h"
#include "environ.h"
#include "appaserver_error.h"
#include "predictive.h"
#include "entity.h"
#include "sale.h"
#include "specific_inventory_sale.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *sale_date_time;
	char *inventory_name;
	char *serial_key;
	char *state;
	int row_number;
	int row_count;
	boolean fund_boolean;
	boolean contact_key_boolean;
	SPECIFIC_INVENTORY_SALE *specific_inventory_sale;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 10 )
	{
		fprintf( stderr,
"Usage: %s fund_name full_name contact_key sale_date_time inventory_name serial_key state row_number row_count\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	fund_name = argv[ 1 ];
	full_name = argv[ 2 ];
	contact_key = argv[ 3 ];
	sale_date_time = argv[ 4 ];
	inventory_name = argv[ 5 ];
	serial_key = argv[ 6 ];
	state = argv[ 7 ];
	row_number = atoi( argv[ 8 ] );
	row_count = atoi( argv[ 9 ] );

	/* If just changed 1:m primary key */
	/* ------------------------------- */
	if ( strcmp( sale_date_time, "sale_date_time" ) == 0 ) exit( 0 );
	if ( strcmp( inventory_name, "inventory_name" ) == 0 ) exit( 0 );
	if ( strcmp( serial_key, "serial_key" ) == 0 ) exit( 0 );

	if ( strcmp( state, APPASERVER_PREDELETE_STATE ) == 0 ) exit( 0 );

	if ( row_number == row_count )
	{
		SALE *sale;

		sale = sale_trigger_new(
			(char *)0 /* preupdate_fund_name */,
			(char *)0 /* preupdate_full_name */,
			(char *)0 /* preupdate_contact_key */,
			(char *)0 /* preupdate_uncollectible_date_time */,
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			state );

		if ( sale )
		{
			(void)sale_update_execute(
				application_name
					/* for update_statement_execute */,
				sale->sale_update,
				sale->sale_transaction,
				(SALE_LOSS_TRANSACTION *)0 );
		}
	}

	return 0;
}

