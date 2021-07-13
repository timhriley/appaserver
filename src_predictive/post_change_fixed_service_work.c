/* ---------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/post_change_fixed_service_work.c 	  */
/* ---------------------------------------------------------------------- */
/* 									  */
/* Freely available software: see Appaserver.org			  */
/* ---------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "entity.h"
#include "inventory.h"
#include "customer.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_fixed_service_work_delete(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *service_name );

void post_change_fixed_service_work_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *service_name,
				char *begin_work_date_time );

void post_change_fixed_service_work_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *service_name,
				char *begin_work_date_time,
				char *preupdate_end_work_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *service_name;
	char *begin_work_date_time;
	char *state;
	char *preupdate_end_work_date_time;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 9 )
	{
		fprintf( stderr,
"Usage: %s ignored full_name street_address sale_date_time service_name begin_work_date_time state preupdate_end_work_date_time\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	sale_date_time = argv[ 4 ];
	service_name = argv[ 5 ];
	begin_work_date_time = argv[ 6 ];
	state = argv[ 7 ];
	preupdate_end_work_date_time = argv[ 8 ];

	/* If change full_name or street address only. */
	/* --------------------------------------------- */
	if ( strcmp( sale_date_time, "sale_date_time" ) == 0 ) exit( 0 );

	/* If change sale_date_time. */
	/* ------------------------- */
	if ( strcmp( service_name, "service_name" ) == 0 ) exit( 0 );

	if ( strcmp( state, "predelete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "delete" ) == 0 )
	{
		post_change_fixed_service_work_delete(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			service_name );
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_fixed_service_work_insert(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			service_name,
			begin_work_date_time );
	}
	else
	{
		post_change_fixed_service_work_update(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			service_name,
			begin_work_date_time,
			preupdate_end_work_date_time );
	}

	return 0;

} /* main() */

void post_change_fixed_service_work_insert(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *begin_work_date_time )
{
	CUSTOMER_SALE *customer_sale;
	FIXED_SERVICE *fixed_service;
	SERVICE_WORK *service_work;

	if ( ! (  customer_sale =
			customer_sale_new(
				application_name,
				full_name,
				street_address,
				sale_date_time ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: customer_sale_new() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return;
	}

	if ( ! ( fixed_service =
			customer_fixed_service_sale_seek(
				customer_sale->fixed_service_sale_list,
				service_name ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 service_name );
		return;
	}

	if ( ! ( service_work =
			customer_service_work_seek(
				fixed_service->service_work_list,
				begin_work_date_time ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 begin_work_date_time );
		return;
	}

	customer_sale_update(
		customer_sale->sum_extension,
		customer_sale->database_sum_extension,
		customer_sale->sales_tax,
		customer_sale->database_sales_tax,
		customer_sale->invoice_amount,
		customer_sale->database_invoice_amount,
		customer_sale->completed_date_time,
		customer_sale->
			database_completed_date_time,
		customer_sale->shipped_date_time,
		customer_sale->database_shipped_date_time,
		customer_sale->arrived_date,
		customer_sale->database_arrived_date,
		customer_sale->total_payment,
		customer_sale->database_total_payment,
		customer_sale->amount_due,
		customer_sale->database_amount_due,
		customer_sale->transaction_date_time,
		customer_sale->
			database_transaction_date_time,
		customer_sale->full_name,
		customer_sale->street_address,
		customer_sale->sale_date_time,
		application_name );

	customer_fixed_service_sale_update(
		application_name,
		customer_sale->full_name,
		customer_sale->street_address,
		customer_sale->sale_date_time,
		fixed_service->service_name,
		fixed_service->extension,
		fixed_service->database_extension,
		fixed_service->work_hours,
		fixed_service->database_work_hours );

	customer_fixed_service_work_update(
		application_name,
		customer_sale->full_name,
		customer_sale->street_address,
		customer_sale->sale_date_time,
		fixed_service->service_name,
		service_work->begin_work_date_time,
		(char *)0 /* end_work_date_time */,
		service_work->work_hours,
		service_work->database_work_hours );

	if ( customer_sale->transaction_date_time )
	{
		customer_sale->transaction =
			ledger_customer_sale_build_transaction(
				application_name,
				customer_sale->transaction->full_name,
				customer_sale->transaction->street_address,
				customer_sale->transaction->
					transaction_date_time,
				customer_sale->transaction->memo,
				customer_sale->inventory_sale_list,
				customer_sale->specific_inventory_sale_list,
				customer_sale->fixed_service_sale_list,
				customer_sale->hourly_service_sale_list,
				customer_sale->shipping_revenue,
				customer_sale->sales_tax,
				customer_sale->invoice_amount,
				customer_sale->fund_name );

		if ( customer_sale->transaction )
		{
			ledger_transaction_refresh(
				application_name,
				customer_sale->full_name,
				customer_sale->street_address,
				customer_sale->transaction_date_time,
				customer_sale->transaction->transaction_amount,
				customer_sale->transaction->memo,
				0 /* check_number */,
				1 /* lock_transaction */,
				customer_sale->
					transaction->
					journal_ledger_list );
		}

	} /* if transaction_date_time */

} /* post_change_fixed_service_work_insert() */

void post_change_fixed_service_work_update(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *begin_work_date_time,
			char *preupdate_end_work_date_time )
{
	CUSTOMER_SALE *customer_sale;
	FIXED_SERVICE *fixed_service;
	SERVICE_WORK *service_work;
	enum preupdate_change_state end_work_date_time_change_state;
	char *preupdate_completed_date_time = "";
	boolean execute_post_change_customer_sale = 0;

	if ( ! (  customer_sale =
			customer_sale_new(
				application_name,
				full_name,
				street_address,
				sale_date_time ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: customer_sale_new() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return;
	}

	if ( ! ( fixed_service =
			customer_fixed_service_sale_seek(
				customer_sale->fixed_service_sale_list,
				service_name ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 service_name );
		return;
	}

	if ( ! ( service_work =
			customer_service_work_seek(
				fixed_service->service_work_list,
				begin_work_date_time ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 begin_work_date_time );
		return;
	}

	end_work_date_time_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_end_work_date_time,
			(service_work->end_work_date_time)
				? service_work->end_work_date_time
				: "",
			"preupdate_end_work_date_time" );

	if ( end_work_date_time_change_state == from_something_to_null
	&&   customer_sale->completed_date_time
	&&   *customer_sale->completed_date_time )
	{
		preupdate_completed_date_time =
			customer_sale->completed_date_time;

		customer_sale->completed_date_time = (char *)0;

		execute_post_change_customer_sale = 1;
	}

	customer_sale_update(
		customer_sale->sum_extension,
		customer_sale->database_sum_extension,
		customer_sale->sales_tax,
		customer_sale->database_sales_tax,
		customer_sale->invoice_amount,
		customer_sale->database_invoice_amount,
		customer_sale->completed_date_time,
		customer_sale->
			database_completed_date_time,
		customer_sale->shipped_date_time,
		customer_sale->database_shipped_date_time,
		customer_sale->arrived_date,
		customer_sale->database_arrived_date,
		customer_sale->total_payment,
		customer_sale->database_total_payment,
		customer_sale->amount_due,
		customer_sale->database_amount_due,
		customer_sale->transaction_date_time,
		customer_sale->
			database_transaction_date_time,
		customer_sale->full_name,
		customer_sale->street_address,
		customer_sale->sale_date_time,
		application_name );

	customer_fixed_service_sale_update(
		application_name,
		customer_sale->full_name,
		customer_sale->street_address,
		customer_sale->sale_date_time,
		fixed_service->service_name,
		fixed_service->extension,
		fixed_service->database_extension,
		fixed_service->work_hours,
		fixed_service->database_work_hours );

	customer_fixed_service_work_update(
		application_name,
		customer_sale->full_name,
		customer_sale->street_address,
		customer_sale->sale_date_time,
		fixed_service->service_name,
		service_work->begin_work_date_time,
		service_work->end_work_date_time,
		service_work->work_hours,
		service_work->database_work_hours );

	if ( customer_sale->transaction_date_time )
	{
		customer_sale->transaction =
			ledger_customer_sale_build_transaction(
				application_name,
				customer_sale->transaction->full_name,
				customer_sale->transaction->street_address,
				customer_sale->transaction->
					transaction_date_time,
				customer_sale->transaction->memo,
				customer_sale->inventory_sale_list,
				customer_sale->specific_inventory_sale_list,
				customer_sale->fixed_service_sale_list,
				customer_sale->hourly_service_sale_list,
				customer_sale->shipping_revenue,
				customer_sale->sales_tax,
				customer_sale->invoice_amount,
				customer_sale->fund_name );

		if ( customer_sale->transaction )
		{
			ledger_transaction_refresh(
				application_name,
				customer_sale->full_name,
				customer_sale->street_address,
				customer_sale->transaction_date_time,
				customer_sale->transaction->transaction_amount,
				customer_sale->transaction->memo,
				0 /* check_number */,
				1 /* lock_transaction */,
				customer_sale->
					transaction->
					journal_ledger_list );
		}

	} /* if transaction_date_time */

	if ( execute_post_change_customer_sale )
	{
		char sys_string[ 1024 ];

		sprintf( sys_string,
"post_change_customer_sale %s \"%s\" \"%s\" \"%s\" update preupdate_full_name preupdate_street_address preupdate_title_passage_rule \"%s\" preupdate_shipped_date_time preupdate_arrived_date preupdate_shipping_revenue",
			 application_name,
			 customer_sale->full_name,
			 customer_sale->street_address,
			 customer_sale->sale_date_time,
			 preupdate_completed_date_time );

		system( sys_string );
	}

} /* post_change_fixed_service_work_update() */

void post_change_fixed_service_work_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name )
{
	CUSTOMER_SALE *customer_sale;
	FIXED_SERVICE *fixed_service;

	if ( ! (  customer_sale =
			customer_sale_new(
				application_name,
				full_name,
				street_address,
				sale_date_time ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: customer_sale_new() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return;
	}

	if ( ! ( fixed_service =
			customer_fixed_service_sale_seek(
				customer_sale->fixed_service_sale_list,
				service_name ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot seek (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 service_name );
		return;
	}

	customer_sale_update(
			customer_sale->sum_extension,
			customer_sale->database_sum_extension,
			customer_sale->sales_tax,
			customer_sale->database_sales_tax,
			customer_sale->invoice_amount,
			customer_sale->database_invoice_amount,
			customer_sale->completed_date_time,
			customer_sale->
				database_completed_date_time,
			customer_sale->shipped_date_time,
			customer_sale->database_shipped_date_time,
			customer_sale->arrived_date,
			customer_sale->database_arrived_date,
			customer_sale->total_payment,
			customer_sale->database_total_payment,
			customer_sale->amount_due,
			customer_sale->database_amount_due,
			customer_sale->transaction_date_time,
			customer_sale->
				database_transaction_date_time,
			customer_sale->full_name,
			customer_sale->street_address,
			customer_sale->sale_date_time,
			application_name );

	customer_fixed_service_sale_update(
		application_name,
		customer_sale->full_name,
		customer_sale->street_address,
		customer_sale->sale_date_time,
		fixed_service->service_name,
		fixed_service->extension,
		fixed_service->database_extension,
		fixed_service->work_hours,
		fixed_service->database_work_hours );

	if ( customer_sale->transaction_date_time )
	{
		customer_sale->transaction =
			ledger_customer_sale_build_transaction(
				application_name,
				customer_sale->transaction->full_name,
				customer_sale->transaction->street_address,
				customer_sale->transaction->
					transaction_date_time,
				customer_sale->transaction->memo,
				customer_sale->inventory_sale_list,
				customer_sale->specific_inventory_sale_list,
				customer_sale->fixed_service_sale_list,
				customer_sale->hourly_service_sale_list,
				customer_sale->shipping_revenue,
				customer_sale->sales_tax,
				customer_sale->invoice_amount,
				customer_sale->fund_name );

		if ( customer_sale->transaction )
		{
			ledger_transaction_refresh(
				application_name,
				customer_sale->full_name,
				customer_sale->street_address,
				customer_sale->transaction_date_time,
				customer_sale->transaction->transaction_amount,
				customer_sale->transaction->memo,
				0 /* check_number */,
				1 /* lock_transaction */,
				customer_sale->
					transaction->
					journal_ledger_list );
		}

	} /* if transaction_date_time */

} /* post_change_fixed_service_work_delete() */

