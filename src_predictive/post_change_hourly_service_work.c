/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/post_change_hourly_service_work.c	*/
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
#include "hourly_service_work.h"

void hourly_service_work_trigger(
		char *application_name,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		char *begin_work_date_time,
		char *state );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *service_name;
	char *service_description;
	char *begin_work_date_time;
	char *state;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s full_name street_address sale_date_time service_name service_description begin_work_date_time state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	sale_date_time = argv[ 3 ];
	service_name = argv[ 4 ];
	service_description = argv[ 5 ];
	begin_work_date_time = argv[ 6 ];
	state = argv[ 7 ];

	/* If change 1:m full_name or street address. */
	/* ------------------------------------------ */
	if ( strcmp( sale_date_time, "sale_date_time" ) == 0 ) exit( 0 );

	/* If change 1:m sale_date_time. */
	/* ----------------------------- */
	if ( strcmp( service_name, "service_name" ) == 0 ) exit( 0 );

	hourly_service_work_trigger(
		application_name,
		full_name,
		street_address,
		sale_date_time,
		service_name,
		service_description,
		begin_work_date_time,
		state );

	return 0;
}

void hourly_service_work_trigger(
		char *application_name,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		char *begin_work_date_time,
		char *state )
{
	HOURLY_SERVICE_WORK *hourly_service_work;
	SALE *sale;

	if ( strcmp( state, APPASERVER_PREDELETE_STATE ) == 0 ) return;

	if ( strcmp(
		state,
		APPASERVER_INSERT_STATE ) == 0
	||   strcmp(
		state,
		APPASERVER_UPDATE_STATE ) == 0 )
	{
		hourly_service_work =
			hourly_service_work_fetch(
				HOURLY_SERVICE_WORK_SELECT,
				HOURLY_SERVICE_WORK_TABLE,
				full_name,
				street_address,
				sale_date_time,
				service_name,
				service_description,
				begin_work_date_time );
	
		hourly_service_work_update(
			HOURLY_SERVICE_WORK_TABLE,
			full_name,
			street_address,
			sale_date_time,
			service_name,
			service_description,
			begin_work_date_time,
			hourly_service_work->sale_work_hours );
	}
	
	sale =
		sale_trigger_new(
			full_name,
			street_address,
			sale_date_time,
			state,
			(char *)0 /* preupdate_full_name */,
			(char *)0 /* preupdate_street_address */,
			(char *)0 /* preupdate_uncollectible_date_time */ );

	if ( !sale ) return;

	sale_update(
		SALE_TABLE,
		full_name,
		street_address,
		sale_date_time,
		sale->sale_fetch->inventory_sale_boolean,
		sale->sale_fetch->specific_inventory_sale_boolean,
		sale->sale_fetch->fixed_service_sale_boolean,
		sale->sale_fetch->hourly_service_sale_boolean,
		sale->inventory_sale_total,
		sale->specific_inventory_sale_total,
		sale->fixed_service_sale_total,
		sale->hourly_service_sale_total,
		sale->gross_revenue,
		sale->sales_tax,
		sale->invoice_amount,
		sale->customer_payment_total,
		sale->amount_due,
		sale->sale_transaction );
	
	if ( sale->sale_transaction )
	{
		subsidiary_transaction_execute(
			application_name,
			sale->
				sale_transaction->
				subsidiary_transaction->
				delete_transaction,
			sale->
				sale_transaction->
				subsidiary_transaction->
				insert_transaction,
			sale->
				sale_transaction->
				subsidiary_transaction->
				update_template );
	}
}

