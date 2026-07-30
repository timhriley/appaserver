/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/hourly_service_sale_trigger.c	*/
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
#include "hourly_service_sale.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *sale_date_time;
	char *service_name;
	char *service_description;
	char *state;
	HOURLY_SERVICE_SALE *hourly_service_sale;
	SALE *sale;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s fund_name full_name contact_key sale_date_time service_name description state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	fund_name = argv[ 1 ];
	full_name = argv[ 2 ];
	contact_key = argv[ 3 ];
	sale_date_time = argv[ 4 ];
	service_name = argv[ 5 ];
	service_description = argv[ 6 ];
	state = argv[ 7 ];

	/* If only changed 1:m primary key */
	/* ------------------------------- */
	if ( strcmp( sale_date_time, "sale_date_time" ) == 0 ) exit( 0 );
	if ( strcmp( service_name, "service_name" ) == 0 ) exit( 0 );
	if ( strcmp(
		service_description,
		"service_description" ) == 0 )
			exit( 0 );

	hourly_service_sale =
		hourly_service_sale_trigger(
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			service_name,
			service_description,
			state );

	if ( hourly_service_sale )
	{
		hourly_service_sale_update(
			hourly_service_sale->update_string_list,
			hourly_service_sale->sale_update_system_string );
	}

	sale =
		sale_trigger_new(
			fund_name,
			full_name,
			contact_key,
			sale_date_time,
			state,
			(char *)0 /* preupdate_fund_name */,
			(char *)0 /* preupdate_full_name */,
			(char *)0 /* preupdate_contact_key */,
			(char *)0 /* preupdate_uncollectible_date_time */ );

	if ( sale )
	{
		(void)sale_update(
			application_name /* for update_statement_execute */,
			sale->update_string_list,
			sale->update_system_string,
			sale->sale_transaction,
			(SALE_LOSS_TRANSACTION*)0 );
	}

	return 0;
}
