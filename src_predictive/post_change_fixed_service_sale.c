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
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *service_name;
	char *state;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s full_name street_address sale_date_time service_name state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	sale_date_time = argv[ 3 ];
	service_name = argv[ 4 ];
	state = argv[ 5 ];

	/* If change 1:m full_name or street address. */
	/* ------------------------------------------ */
	if ( strcmp( sale_date_time, "sale_date_time" ) == 0 ) exit( 0 );

	/* If change 1:m sale_date_time. */
	/* ----------------------------- */
	if ( strcmp( service_name, "service_name" ) == 0 ) exit( 0 );

	fixed_service_sale_trigger(
		application_name,
		full_name,
		street_address,
		sale_date_time,
		service_name,
		state );

	return 0;
}

