/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/post_change_fixed_service_sale.c	*/
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
#include "fixed_service_sale.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *sale_date_time;
	char *service_name;
	char *state;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s fund_name full_name contact_key sale_date_time service_name state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	fund_name = argv[ 1 ];
	full_name = argv[ 2 ];
	contact_key = argv[ 3 ];
	sale_date_time = argv[ 4 ];
	service_name = argv[ 5 ];
	state = argv[ 6 ];

	/* If change 1:m full_name, contact_key, or service_name. */
	/* ------------------------------------------------------ */
	if ( strcmp( sale_date_time, "sale_date_time" ) == 0 ) exit( 0 );

	/* If change 1:m sale_date_time. */
	/* ----------------------------- */
	if ( strcmp( service_name, "service_name" ) == 0 ) exit( 0 );

	fixed_service_sale_trigger(
		application_name,
		fund_name,
		full_name,
		contact_key,
		sale_date_time,
		service_name,
		state );

	return 0;
}

