/* -----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/post_change_customer_sale.c		*/
/* -----------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* -----------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "inventory.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "predictive.h"
#include "date.h"
#include "customer_sale.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_customer_sale_predelete(
			CUSTOMER_SALE *customer_sale );

void post_change_customer_sale_insert_update(
			CUSTOMER_SALE *customer_sale );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *state;
	CUSTOMER_SALE *customer_sale;
	char *preupdate_completed_date_time = {0};
	char *preupdate_shipped_date_time = {0};
	char *preupdate_arrived_date = {0};

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 5  )
	{
		fprintf( stderr,
"Usage: %s full_name street_address sale_date_time state [preupdate_completed_date_time preupdate_shipped_date_time preupdate_arrived_date]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	sale_date_time = argv[ 3 ];
	state = argv[ 4 ];

	if ( argc == 8 )
	{
		/* Future work */
		/* ----------- */
		if ( ( preupdate_completed_date_time = argv[ 5 ] ) ){}
		if ( ( preupdate_shipped_date_time = argv[ 6 ] ) ){}
		if ( ( preupdate_arrived_date = argv[ 7 ] ) ){}
	}

	if ( strcmp( sale_date_time, "sale_date_time" ) == 0 ) exit( 0 );

	/* Execute the predelete because CUSTOMER_SALE.transaction_date_time */
	/* ----------------------------------------------------------------- */
	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	if ( ! ( customer_sale =
			customer_sale_fetch(
				full_name,
				street_address,
				sale_date_time ) ) )
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: customer_sale_fetch(%s,%s,%s) returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address,
			 sale_date_time );
		return;
	}

	if ( strcmp( state, "predelete" ) == 0 )
	{
		post_change_customer_sale_predelete(
			customer_sale );
	}
	else
	if ( strcmp( state, "insert" ) == 0
	||   strcmp( state, "update" ) == 0 )
	{
		post_change_customer_sale_insert_update(
			customer_sale );
	}
	else
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: unrecognized state = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 state );
		exit( 1 );
	}

	return 0;
}

void post_change_customer_sale_insert_update(
			CUSTOMER_SALE *customer_sale )
{
}

void post_change_customer_sale_predelete(
			CUSTOMER_SALE *customer_sale )
{
}

