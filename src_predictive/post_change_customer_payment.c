/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/post_change_customer_payment.c	*/
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
#include "customer_payment.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *sale_date_time;
	char *payment_date_time;
	char *state;
	char *preupdate_fund_name;
	char *preupdate_full_name;
	char *preupdate_contact_key;
	char *preupdate_payment_date_time;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 11 )
	{
		fprintf( stderr,
"Usage: %s fund_name full_name contact_key sale_date_time payment_date_time state preupdate_fund_name preupdate_full_name preupdate_contact_key preupdate_payment_date_time\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	fund_name = argv[ 1 ];
	full_name = argv[ 2 ];
	contact_key = argv[ 3 ];
	sale_date_time = argv[ 4 ];
	payment_date_time = argv[ 5 ];
	state = argv[ 6 ];
	preupdate_fund_name = argv[ 7 ];
	preupdate_full_name = argv[ 8 ];
	preupdate_contact_key = argv[ 9 ];
	preupdate_payment_date_time = argv[ 10 ];

	/* If changed 1:m primary keys */
	/* --------------------------- */
	if ( strcmp( sale_date_time, "sale_date_time" ) == 0 ) exit( 0 );

	if ( strcmp( payment_date_time, "payment_date_time" ) == 0 ) exit( 0 );

	customer_payment_trigger(
		application_name,
		fund_name,
		full_name,
		contact_key,
		sale_date_time,
		payment_date_time,
		state,
		preupdate_fund_name,
		preupdate_full_name,
		preupdate_contact_key,
		preupdate_payment_date_time );

	return 0;
}

