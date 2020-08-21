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
#include "customer.h"
#include "predictive.h"
#include "date.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_customer_sale_changed_to_FOB_shipping(
				CUSTOMER_SALE *customer_sale,
				char *application_name );

void post_change_customer_sale_title_rule_null(
				CUSTOMER_SALE *customer_sale,
				char *application_name );

void post_change_customer_sale_changed_to_FOB_destination(
				CUSTOMER_SALE *customer_sale,
				char *application_name );

void post_change_customer_sale_transaction_date_time_update(
				CUSTOMER_SALE *customer_sale,
				char *transaction_date_time,
				char *application_name );

void post_change_customer_sale_new_transaction(
				CUSTOMER_SALE *customer_sale,
				char *transaction_date_time,
				char *application_name );

void post_change_customer_sale_FOB_destination_new_rule(
				CUSTOMER_SALE *customer_sale,
				char *application_name );

void post_change_customer_sale_FOB_shipping_new_rule(
				CUSTOMER_SALE *customer_sale,
				char *application_name );

void post_change_customer_sale_delete(
				CUSTOMER_SALE *customer_sale,
				char *application_name );

void post_change_customer_sale_shipping_revenue_update(
				CUSTOMER_SALE *customer_sale,
				char *application_name );

void post_change_customer_sale_mistakenly_completed(
				CUSTOMER_SALE *customer_sale,
				char *application_name );

void post_change_customer_sale_just_shipped_FOB_shipping(
				CUSTOMER_SALE *customer_sale,
				char *application_name );

void post_change_customer_sale_just_completed(
				CUSTOMER_SALE *customer_sale,
				char *application_name );

void post_change_customer_sale_mistakenly_shipped_FOB_shipping(
				CUSTOMER_SALE *customer_sale,
				char *application_name );

void post_change_customer_sale_mistakenly_arrived_FOB_destination(
				CUSTOMER_SALE *customer_sale,
				char *application_name );

void post_change_customer_sale_just_arrived_FOB_destination(
				CUSTOMER_SALE *customer_sale,
				char *application_name );

void post_change_customer_sale_fixed_completed_date_time(
				CUSTOMER_SALE *customer_sale,
				char *preupdate_completed_date_time,
				char *application_name );

void post_change_customer_sale_fixed_arrived_date(
				CUSTOMER_SALE *customer_sale,
				char *application_name );

void post_change_customer_sale_fixed_shipped_date_time(
				CUSTOMER_SALE *customer_sale,
				char *application_name );

void post_change_customer_sale_insert_FOB_destination(
				CUSTOMER_SALE *customer_sale,
				char *application_name );

void post_change_customer_sale_insert_FOB_shipping(
				CUSTOMER_SALE *customer_sale,
				char *application_name );

void post_change_customer_sale_insert_title_passage_null(
				CUSTOMER_SALE *customer_sale,
				char *application_name );

char *post_change_customer_sale_get_completed_date_time(
				char **shipped_date_time,
				char *arrived_date,
				char *completed_date_time );

void post_change_customer_sale_update(
				CUSTOMER_SALE *customer_sale,
				char *preupdate_full_name,
				char *preupdate_street_address,
				char *preupdate_title_passage_rule,
				char *preupdate_completed_date_time,
				char *preupdate_shipped_date_time,
				char *preupdate_arrived_date,
				char *preupdate_shipping_revenue,
				char *application_name );

void post_change_customer_sale_insert(
				CUSTOMER_SALE *customer_sale,
				char *application_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *state;
	CUSTOMER_SALE *customer_sale;
	char *preupdate_full_name;
	char *preupdate_street_address;
	char *preupdate_title_passage_rule;
	char *preupdate_completed_date_time;
	char *preupdate_shipped_date_time;
	char *preupdate_arrived_date;
	char *preupdate_shipping_revenue;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 13 )
	{
		fprintf( stderr,
"Usage: %s ignored full_name street_address sale_date_time state preupdate_full_name preupdate_street_address preupdate_title_passage_rule preupdate_completed_date_time preupdate_shipped_date_time preupdate_arrived_date preupdate_shipping_revenue\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	sale_date_time = argv[ 4 ];
	state = argv[ 5 ];
	preupdate_full_name = argv[ 6 ];
	preupdate_street_address = argv[ 7 ];
	preupdate_title_passage_rule = argv[ 8 ];
	preupdate_completed_date_time = argv[ 9 ];
	preupdate_shipped_date_time = argv[ 10 ];
	preupdate_arrived_date = argv[ 11 ];
	preupdate_shipping_revenue = argv[ 12 ];

	if ( strcmp( sale_date_time, "sale_date_time" ) == 0 ) exit( 0 );

	/* Execute the predelete because CUSTOMER_SALE.transaction_date_time */
	/* ----------------------------------------------------------------- */
	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	customer_sale =
		customer_sale_new(
			application_name,
			full_name,
			street_address,
			sale_date_time );

	if ( !customer_sale )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot get customer_sale for %s/%s/%s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address,
			 sale_date_time );
		exit( 1 );
	}

	if ( strcmp( state, "predelete" ) == 0 )
	{
		post_change_customer_sale_delete(
			customer_sale,
			application_name );
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_customer_sale_update(
			customer_sale,
			preupdate_full_name,
			preupdate_street_address,
			preupdate_title_passage_rule,
			preupdate_completed_date_time,
			preupdate_shipped_date_time,
			preupdate_arrived_date,
			preupdate_shipping_revenue,
			application_name );
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_customer_sale_insert(
			customer_sale,
			application_name );
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


	if ( strcmp( state, "predelete" ) != 0 )
	{
		customer_sale_update(
			customer_sale->sum_extension,
			customer_sale->database_sum_extension,
			customer_sale->sales_tax,
			customer_sale->database_sales_tax,
			customer_sale->invoice_amount,
			customer_sale->database_invoice_amount,
			customer_sale->completed_date_time,
			customer_sale->database_completed_date_time,
			customer_sale->shipped_date_time,
			customer_sale->database_shipped_date_time,
			customer_sale->arrived_date,
			customer_sale->database_arrived_date,
			customer_sale->total_payment,
			customer_sale->database_total_payment,
			customer_sale->amount_due,
			customer_sale->database_amount_due,
			customer_sale->transaction_date_time,
			customer_sale->database_transaction_date_time,
			customer_sale->full_name,
			customer_sale->street_address,
			customer_sale->sale_date_time,
			application_name );

		/* Update inventory_sale->cost_of_goods_sold */
		/* ----------------------------------------- */
		inventory_sale_list_update(
			customer_sale->inventory_sale_list,
			application_name );
	}

	return 0;

} /* main() */

void post_change_customer_sale_update(
			CUSTOMER_SALE *customer_sale,
			char *preupdate_full_name,
			char *preupdate_street_address,
			char *preupdate_title_passage_rule,
			char *preupdate_completed_date_time,
			char *preupdate_shipped_date_time,
			char *preupdate_arrived_date,
			char *preupdate_shipping_revenue,
			char *application_name )
{
	enum preupdate_change_state full_name_change_state;
	enum preupdate_change_state street_address_change_state;
	enum preupdate_change_state title_passage_rule_change_state;
	enum preupdate_change_state completed_date_time_change_state;
	enum preupdate_change_state shipped_date_time_change_state;
	enum preupdate_change_state arrived_date_change_state;
	enum preupdate_change_state shipping_revenue_change_state;

	full_name_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_full_name,
			customer_sale->full_name,
			"preupdate_full_name" );

	street_address_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_street_address,
			customer_sale->street_address,
			"preupdate_street_address" );

	if ( full_name_change_state == from_something_to_something_else
	||   street_address_change_state == from_something_to_something_else )
	{
		if ( customer_sale->transaction_date_time )
		{
			ledger_entity_update(
				application_name,
				customer_sale->full_name,
				customer_sale->street_address,
				customer_sale->transaction_date_time,
				preupdate_full_name,
				preupdate_street_address );
		}
	}

	title_passage_rule_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_title_passage_rule,
			entity_get_title_passage_rule_string(
				customer_sale->title_passage_rule ),
			"preupdate_title_passage_rule" );

	completed_date_time_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_completed_date_time,
			(customer_sale->completed_date_time)
				? customer_sale->completed_date_time
				: "",
			"preupdate_completed_date_time" );

	shipped_date_time_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_shipped_date_time,
			(customer_sale->shipped_date_time)
				? customer_sale->shipped_date_time
				: "",
			"preupdate_shipped_date_time" );

	arrived_date_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_arrived_date,
			(customer_sale->arrived_date)
				? customer_sale->arrived_date
				: "",
			"preupdate_arrived_date" );

	shipping_revenue_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_shipping_revenue,
			(char *)0 /* postupdate_data */,
			"preupdate_shipping_revenue" );

	if ( arrived_date_change_state == from_null_to_something
	&&   customer_sale->title_passage_rule == FOB_destination )
	{
		post_change_customer_sale_just_arrived_FOB_destination(
			customer_sale,
			application_name );
	}

	if ( arrived_date_change_state == from_something_to_something_else
	&&   customer_sale->title_passage_rule == FOB_destination )
	{
		post_change_customer_sale_fixed_arrived_date(
			customer_sale,
			application_name );
	}

	if ( arrived_date_change_state == from_something_to_null
	&&   customer_sale->title_passage_rule == FOB_destination )
	{
		post_change_customer_sale_mistakenly_arrived_FOB_destination(
			customer_sale,
			application_name );
	}

	if ( shipped_date_time_change_state == from_null_to_something
	&&   customer_sale->title_passage_rule == FOB_destination )
	{
		post_change_customer_sale_just_shipped_FOB_shipping(
			customer_sale,
			application_name );
	}

	if ( shipped_date_time_change_state == from_something_to_something_else
	&&   customer_sale->title_passage_rule == FOB_shipping )
	{
		post_change_customer_sale_fixed_shipped_date_time(
			customer_sale,
			application_name );
	}

	if ( shipped_date_time_change_state == from_something_to_null
	&&   customer_sale->title_passage_rule == FOB_shipping )
	{
		post_change_customer_sale_mistakenly_shipped_FOB_shipping(
			customer_sale,
			application_name );
	}

	if ( completed_date_time_change_state == from_null_to_something )
	{
		post_change_customer_sale_just_completed(
			customer_sale,
			application_name );
	}

	if (	completed_date_time_change_state ==
		from_something_to_something_else )
	{
		post_change_customer_sale_fixed_completed_date_time(
			customer_sale,
			preupdate_completed_date_time,
			application_name );
	}

	if ( completed_date_time_change_state == from_something_to_null )
	{
		post_change_customer_sale_mistakenly_completed(
			customer_sale,
			application_name );
	}

	if ( shipping_revenue_change_state == from_something_to_something_else
	||   shipping_revenue_change_state == from_null_to_something )
	{
		post_change_customer_sale_shipping_revenue_update(
			customer_sale,
			application_name );
	}

	if ( title_passage_rule_change_state == from_null_to_something )
	{
		if ( customer_sale->completed_date_time )
		{
			char *sales_tax_payable_full_name = {0};
			char *sales_tax_payable_street_address = {0};

			if ( customer_sale->sales_tax )
			{
				ENTITY *sales_tax_payable_entity;

				if ( ! ( sales_tax_payable_entity =
					    entity_get_sales_tax_payable_entity(
							application_name ) ) )
				{
					fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot get sales_tax_payable_entity.\n",
			 			__FILE__,
			 			__FUNCTION__,
			 			__LINE__ );
					exit( 1 );
				}

				sales_tax_payable_full_name =
					sales_tax_payable_entity->
						full_name;

				sales_tax_payable_street_address =
					sales_tax_payable_entity->
						street_address;

			}

			customer_sale_transaction_delete_with_propagate(
				application_name,
				customer_sale->fund_name,
				customer_sale->full_name,
				customer_sale->street_address,
				sales_tax_payable_full_name,
				sales_tax_payable_street_address,
				customer_sale->transaction_date_time );

			customer_sale->transaction = (TRANSACTION *)0;
			customer_sale->transaction_date_time = (char *)0;
		}

		if ( customer_sale->title_passage_rule == FOB_shipping )
		{
			post_change_customer_sale_FOB_shipping_new_rule(
				customer_sale,
				application_name );
		}
		else
		{
			post_change_customer_sale_FOB_destination_new_rule(
				customer_sale,
				application_name );
		}
	}

	if (	title_passage_rule_change_state ==
		from_something_to_something_else )
	{
		if ( customer_sale->title_passage_rule == FOB_shipping )
		{
			post_change_customer_sale_changed_to_FOB_shipping(
				customer_sale,
				application_name );
		}
		else
		{
			post_change_customer_sale_changed_to_FOB_destination(
				customer_sale,
				application_name );
		}
	}

	if ( title_passage_rule_change_state == from_something_to_null )
	{
		post_change_customer_sale_title_rule_null(
			customer_sale,
			application_name );
	}

} /* post_change_customer_sale_update() */

void post_change_customer_sale_fixed_shipped_date_time(
			CUSTOMER_SALE *customer_sale,
			char *application_name )
{
	if ( !customer_sale->transaction )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: empty customer_sale->transaction.\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__ );
		exit( 1 );
	}

	customer_sale->transaction_date_time =
		customer_sale->shipped_date_time;

	post_change_customer_sale_transaction_date_time_update(
			customer_sale,
			customer_sale->transaction_date_time,
			application_name );

} /* post_change_customer_sale_fixed_shipped_date_time() */

void post_change_customer_sale_fixed_arrived_date(
			CUSTOMER_SALE *customer_sale,
			char *application_name )
{
	if ( !customer_sale->transaction )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: empty customer_sale->transaction.\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__ );
		exit( 1 );
	}

	customer_sale->transaction_date_time =
		predictive_transaction_date_time(
			customer_sale->arrived_date );

	post_change_customer_sale_transaction_date_time_update(
		customer_sale,
		customer_sale->transaction_date_time,
		application_name );

} /* post_change_customer_sale_fixed_arrived_date() */

void post_change_customer_sale_mistakenly_completed(
			CUSTOMER_SALE *customer_sale,
			char *application_name )
{
	INVENTORY_SALE *inventory_sale;
	char sys_string[ 1024 ];

	if ( list_rewind( customer_sale->inventory_sale_list )  )
	{
		do {
			inventory_sale =
				list_get(
					customer_sale->inventory_sale_list );

			sprintf( sys_string,
"propagate_inventory_sale_layers %s \"%s\" \"%s\" \"%s\" \"%s\" '' n",
			 	application_name,
			 	customer_sale->full_name,
			 	customer_sale->street_address,
			 	customer_sale->sale_date_time,
			 	inventory_sale->inventory_name );

			inventory_sale->cost_of_goods_sold =
				atof( pipe2string( sys_string ) );

		} while( list_next( customer_sale->inventory_sale_list ) );
	}

	if ( customer_sale->title_passage_rule == title_passage_rule_null )
	{
		char *sales_tax_payable_full_name = {0};
		char *sales_tax_payable_street_address = {0};

		if ( !customer_sale->transaction )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: empty customer_sale->transaction.\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__ );
			exit( 1 );
		}


		if ( customer_sale->sales_tax )
		{
			ENTITY *sales_tax_payable_entity;

			if ( ! ( sales_tax_payable_entity =
				    entity_get_sales_tax_payable_entity(
						application_name ) ) )
			{
				fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot get sales_tax_payable_entity.\n",
		 			__FILE__,
		 			__FUNCTION__,
		 			__LINE__ );
				exit( 1 );
			}

			sales_tax_payable_full_name =
				sales_tax_payable_entity->
					full_name;

			sales_tax_payable_street_address =
				sales_tax_payable_entity->
					street_address;

		}

		customer_sale_transaction_delete_with_propagate(
			application_name,
			customer_sale->fund_name,
			customer_sale->full_name,
			customer_sale->street_address,
			sales_tax_payable_full_name,
			sales_tax_payable_street_address,
			customer_sale->transaction_date_time );

		customer_sale->transaction = (TRANSACTION *)0;
		customer_sale->transaction_date_time = (char *)0;
	}

} /* post_change_customer_sale_mistakenly_completed() */

void post_change_customer_sale_fixed_completed_date_time(
			CUSTOMER_SALE *customer_sale,
			char *preupdate_completed_date_time,
			char *application_name )
{
	INVENTORY_SALE *inventory_sale;
	char *propagate_completed_date_time;
	char sys_string[ 1024 ];

	if ( strcmp(	customer_sale->completed_date_time,
			preupdate_completed_date_time ) < 0 )
	{
		propagate_completed_date_time =
			customer_sale->completed_date_time;
	}
	else
	{
		propagate_completed_date_time =
			preupdate_completed_date_time;
	}

	if ( customer_sale->title_passage_rule == title_passage_rule_null )
	{
		if ( !customer_sale->transaction )
		{
			fprintf( stderr,
		"Warning in %s/%s()/%d: empty customer_sale->transaction.\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__ );

			customer_sale->transaction_date_time = (char *)0;
			return;
		}

		customer_sale->transaction_date_time =
			customer_sale->completed_date_time;

		post_change_customer_sale_transaction_date_time_update(
			customer_sale,
			customer_sale->transaction_date_time,
			application_name );
	}

	if ( list_rewind( customer_sale->inventory_sale_list )  )
	{
		do {
			inventory_sale =
				list_get(
					customer_sale->inventory_sale_list );

			sprintf( sys_string,
"propagate_inventory_sale_layers %s \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" n",
			 	application_name,
			 	customer_sale->full_name,
			 	customer_sale->street_address,
			 	customer_sale->sale_date_time,
			 	inventory_sale->inventory_name,
				propagate_completed_date_time );

			inventory_sale->cost_of_goods_sold =
				atof( pipe2string( sys_string ) );

		} while( list_next( customer_sale->inventory_sale_list ) );
	}

	if ( customer_sale->title_passage_rule == title_passage_rule_null )
	{
		if ( !customer_sale->transaction )
		{
			fprintf( stderr,
		"Warning in %s/%s()/%d: empty customer_sale->transaction.\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__ );

			customer_sale->transaction_date_time = (char *)0;
			return;
		}

		customer_sale->transaction_date_time =
			customer_sale->completed_date_time;

		post_change_customer_sale_transaction_date_time_update(
			customer_sale,
			customer_sale->transaction_date_time,
			application_name );
	}

} /* post_change_customer_sale_fixed_completed_date_time() */

void post_change_customer_sale_mistakenly_shipped_FOB_shipping(
			CUSTOMER_SALE *customer_sale,
			char *application_name )
{
	char *sales_tax_payable_full_name = {0};
	char *sales_tax_payable_street_address = {0};

	if ( customer_sale->transaction )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: not expecting a transaction for this customer_sale.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}


	if ( customer_sale->sales_tax )
	{
		ENTITY *sales_tax_payable_entity;

		if ( ! ( sales_tax_payable_entity =
			    entity_get_sales_tax_payable_entity(
					application_name ) ) )
		{
			fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot get sales_tax_payable_entity.\n",
	 			__FILE__,
	 			__FUNCTION__,
	 			__LINE__ );
			exit( 1 );
		}

		sales_tax_payable_full_name =
			sales_tax_payable_entity->
				full_name;

		sales_tax_payable_street_address =
			sales_tax_payable_entity->
				street_address;

	}

	customer_sale_transaction_delete_with_propagate(
		application_name,
		customer_sale->fund_name,
		customer_sale->full_name,
		customer_sale->street_address,
		sales_tax_payable_full_name,
		sales_tax_payable_street_address,
		customer_sale->transaction_date_time );

	customer_sale->transaction = (TRANSACTION *)0;
	customer_sale->transaction_date_time = (char *)0;

} /* post_change_customer_sale_mistakenly_shipped_FOB_shipping() */

void post_change_customer_sale_mistakenly_arrived_FOB_destination(
			CUSTOMER_SALE *customer_sale,
			char *application_name )
{
	char *sales_tax_payable_full_name = {0};
	char *sales_tax_payable_street_address = {0};

	if ( customer_sale->transaction )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: not expecting a transaction for this customer_sale.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}


	if ( customer_sale->sales_tax )
	{
		ENTITY *sales_tax_payable_entity;

		if ( ! ( sales_tax_payable_entity =
			    entity_get_sales_tax_payable_entity(
					application_name ) ) )
		{
			fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot get sales_tax_payable_entity.\n",
	 			__FILE__,
	 			__FUNCTION__,
	 			__LINE__ );
			exit( 1 );
		}

		sales_tax_payable_full_name =
			sales_tax_payable_entity->
				full_name;

		sales_tax_payable_street_address =
			sales_tax_payable_entity->
				street_address;

	}

	customer_sale_transaction_delete_with_propagate(
		application_name,
		customer_sale->fund_name,
		customer_sale->full_name,
		customer_sale->street_address,
		sales_tax_payable_full_name,
		sales_tax_payable_street_address,
		customer_sale->transaction_date_time );

	customer_sale->transaction = (TRANSACTION *)0;
	customer_sale->transaction_date_time = (char *)0;

} /* post_change_customer_sale_mistakenly_arrived_FOB_destination() */

void post_change_customer_sale_just_completed(
			CUSTOMER_SALE *customer_sale,
			char *application_name )
{
	INVENTORY_SALE *inventory_sale;
	char sys_string[ 1024 ];

	if ( list_rewind( customer_sale->inventory_sale_list )  )
	{
		do {
			inventory_sale =
				list_get_pointer(
					customer_sale->
						inventory_sale_list );

			sprintf( sys_string,
"propagate_inventory_sale_layers %s \"%s\" \"%s\" \"%s\" \"%s\" '' %c",
			 	application_name,
			 	customer_sale->full_name,
			 	customer_sale->street_address,
			 	customer_sale->sale_date_time,
			 	inventory_sale->inventory_name,
			 	( customer_sale_inventory_is_latest(
					application_name,
					inventory_sale->inventory_name,
					customer_sale->sale_date_time ) )
			 		? 'y' : 'n' );

			inventory_sale->cost_of_goods_sold =
				atof( pipe2string( sys_string ) );

		} while( list_next( customer_sale->inventory_sale_list ) );
	}

	if ( customer_sale->title_passage_rule == title_passage_rule_null )
	{
		if ( customer_sale->transaction )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: not expecting a transaction for this customer_sale.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		if ( list_length( customer_sale->fixed_service_sale_list )
		&&   customer_fixed_service_open(
			customer_sale->fixed_service_sale_list ) )
		{
			customer_fixed_service_sale_list_close(
				customer_sale->fixed_service_sale_list,
				customer_sale->completed_date_time );

			customer_fixed_service_sale_list_update(
				customer_sale->fixed_service_sale_list,
				application_name,
				customer_sale->full_name,
				customer_sale->street_address,
				customer_sale->sale_date_time );
		}

		if ( list_length( customer_sale->hourly_service_sale_list )
		&&   customer_hourly_service_open(
			customer_sale->hourly_service_sale_list ) )
		{
			/* Sets hourly_service->extension */
			/* ------------------------------ */
			customer_hourly_service_sale_list_close(
				customer_sale->hourly_service_sale_list,
				customer_sale->completed_date_time );

			customer_sale->invoice_amount =
				customer_sale_get_invoice_amount(
					&customer_sale->
						sum_inventory_extension,
					&customer_sale->
						sum_fixed_service_extension,
					&customer_sale->
						sum_hourly_service_extension,
					&customer_sale->sum_extension,
					&customer_sale->sales_tax,
					customer_sale->shipping_revenue,
					customer_sale->
						inventory_sale_list,
					customer_sale->
						specific_inventory_sale_list,
					customer_sale->fixed_service_sale_list,
					customer_sale->hourly_service_sale_list,
					customer_sale->full_name,
					customer_sale->street_address,
					application_name );

			customer_sale->amount_due =
				CUSTOMER_GET_AMOUNT_DUE(
					customer_sale->invoice_amount,
					customer_sale->total_payment );

			customer_hourly_service_sale_list_update(
				customer_sale->hourly_service_sale_list,
				application_name,
				customer_sale->full_name,
				customer_sale->street_address,
				customer_sale->sale_date_time );
		}

		post_change_customer_sale_new_transaction(
			customer_sale,
			customer_sale->completed_date_time
				/* transaction_date_time */,
			application_name );
	}

} /* post_change_customer_sale_just_completed() */

void post_change_customer_sale_just_shipped_FOB_shipping(
			CUSTOMER_SALE *customer_sale,
			char *application_name )
{
	if ( customer_sale->transaction )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: not expecting a transaction for this customer_sale.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	post_change_customer_sale_new_transaction(
		customer_sale,
		customer_sale->shipped_date_time
			/* transaction_date_time */,
		application_name );

} /* post_change_customer_sale_just_shipped_FOB_shipping() */

void post_change_customer_sale_just_arrived_FOB_destination(
			CUSTOMER_SALE *customer_sale,
			char *application_name )
{
	if ( customer_sale->transaction )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: not expecting a transaction for this customer_sale.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	post_change_customer_sale_new_transaction(
		customer_sale,
		predictive_transaction_date_time(
			customer_sale->arrived_date ),
		application_name );

} /* post_change_customer_sale_just_arrived_FOB_destination() */

void post_change_customer_sale_insert(
			CUSTOMER_SALE *customer_sale,
			char *application_name )
{
	customer_sale->completed_date_time =
		post_change_customer_sale_get_completed_date_time(
			&customer_sale->shipped_date_time,
			customer_sale->arrived_date,
			customer_sale->completed_date_time );

	if ( customer_sale->title_passage_rule == FOB_shipping )
	{
		post_change_customer_sale_insert_FOB_shipping(
			customer_sale,
			application_name );
	}
	else
	if ( customer_sale->title_passage_rule == FOB_destination )
	{
		post_change_customer_sale_insert_FOB_destination(
			customer_sale,
			application_name );
	}
	else
	{
		post_change_customer_sale_insert_title_passage_null(
			customer_sale,
			application_name );
	}

} /* post_change_customer_sale_insert() */

char *post_change_customer_sale_get_completed_date_time(
			char **shipped_date_time,
			char *arrived_date,
			char *completed_date_time )
{
	if ( arrived_date && *arrived_date )
	{
		if ( !*shipped_date_time || !**shipped_date_time )
		{
			char buffer[ 32 ];

			strcpy( buffer,
				date_append_hhmmss(
					arrived_date ) );

			*shipped_date_time = strdup( buffer );
		}
	}

	if ( completed_date_time && *completed_date_time )
	{
		return completed_date_time;
	}
	else
	if ( *shipped_date_time && **shipped_date_time )
	{
		return *shipped_date_time;
	}
	else
	{
		return (char *)0;
	}

} /* post_change_customer_sale_get_completed_date_time() */

void post_change_customer_sale_insert_FOB_destination(
			CUSTOMER_SALE *customer_sale,
			char *application_name )
{
	if ( !customer_sale->arrived_date ) return;

	customer_sale->transaction_date_time =
		predictive_transaction_date_time(
			customer_sale->arrived_date );

	customer_sale->transaction =
		ledger_transaction_new(
			customer_sale->full_name,
			customer_sale->street_address,
			customer_sale->transaction_date_time,
			customer_sale_get_memo(
				customer_sale->full_name ) );

	customer_sale->transaction_date_time =
	customer_sale->transaction->transaction_date_time =
	ledger_transaction_insert(
		application_name,
		customer_sale->transaction->full_name,
		customer_sale->transaction->street_address,
		customer_sale->transaction->transaction_date_time,
		customer_sale->invoice_amount /* transaction_amount */,
		customer_sale->transaction->memo,
		0 /* check_number */,
		1 /* lock_transaction */ );

} /* post_change_customer_sale_insert_FOB_destination() */

void post_change_customer_sale_insert_FOB_shipping(
			CUSTOMER_SALE *customer_sale,
			char *application_name )
{
	if ( !customer_sale->shipped_date_time ) return;

	customer_sale->transaction_date_time =
		customer_sale->shipped_date_time;

	customer_sale->transaction =
		ledger_transaction_new(
			customer_sale->full_name,
			customer_sale->street_address,
			customer_sale->transaction_date_time,
			customer_sale_get_memo(
				customer_sale->full_name ) );

	customer_sale->transaction_date_time =
	customer_sale->transaction->transaction_date_time =
	ledger_transaction_insert(
		application_name,
		customer_sale->transaction->full_name,
		customer_sale->transaction->street_address,
		customer_sale->transaction->transaction_date_time,
		customer_sale->invoice_amount /* transaction_amount */,
		customer_sale->transaction->memo,
		0 /* check_number */,
		1 /* lock_transaction */ );

} /* post_change_customer_sale_insert_FOB_shipping() */

void post_change_customer_sale_insert_title_passage_null(
			CUSTOMER_SALE *customer_sale,
			char *application_name )
{
	if ( !customer_sale->completed_date_time ) return;

	customer_sale->transaction_date_time =
		customer_sale->completed_date_time;

	customer_sale->transaction =
		ledger_transaction_new(
			customer_sale->full_name,
			customer_sale->street_address,
			customer_sale->transaction_date_time,
			customer_sale_get_memo(
				customer_sale->full_name ) );

	customer_sale->transaction_date_time =
	customer_sale->transaction->transaction_date_time =
	ledger_transaction_insert(
		application_name,
		customer_sale->transaction->full_name,
		customer_sale->transaction->street_address,
		customer_sale->transaction->transaction_date_time,
		customer_sale->invoice_amount /* transaction_amount */,
		customer_sale->transaction->memo,
		0 /* check_number */,
		1 /* lock_transaction */ );

} /* post_change_customer_sale_insert_title_passage_null() */

void post_change_customer_sale_shipping_revenue_update(
			CUSTOMER_SALE *customer_sale,
			char *application_name )
{
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

} /* post_change_customer_sale_shipping_revenue_update() */

void post_change_customer_sale_delete(
				CUSTOMER_SALE *customer_sale,
				char *application_name )
{
	INVENTORY_SALE *inventory_sale;
	char sys_string[ 1024 ];
	char *where;

	if ( customer_sale->transaction )
	{
		char *sales_tax_payable_full_name = {0};
		char *sales_tax_payable_street_address = {0};
		ENTITY *sales_tax_payable_entity;

		if ( ! ( sales_tax_payable_entity =
			    entity_get_sales_tax_payable_entity(
					application_name ) ) )
		{
			fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot get sales_tax_payable_entity.\n",
		 		__FILE__,
		 		__FUNCTION__,
		 		__LINE__ );
			exit( 1 );
		}

		sales_tax_payable_full_name =
			sales_tax_payable_entity->
				full_name;

		sales_tax_payable_street_address =
			sales_tax_payable_entity->
				street_address;

		customer_sale_transaction_delete_with_propagate(
			application_name,
			customer_sale->fund_name,
			customer_sale->full_name,
			customer_sale->street_address,
			sales_tax_payable_full_name,
			sales_tax_payable_street_address,
			customer_sale->transaction_date_time );

		customer_sale->transaction = (TRANSACTION *)0;
		customer_sale->transaction_date_time = (char *)0;

	} /* if transation */

	if ( !list_rewind( customer_sale->inventory_sale_list )  ) return;

	where = ledger_get_transaction_where(
			customer_sale->full_name,
			customer_sale->street_address,
			customer_sale->sale_date_time
				/* transaction_date_time */,
			"inventory_sale" /* folder_name */,
			"sale_date_time" /* date_time_column */ );

	sprintf( sys_string,
		 "echo \"delete from inventory_sale where %s;\" | sql.e",
		 where );

	system( sys_string );

	do {
		inventory_sale =
			list_get(
				customer_sale->inventory_sale_list );

		sprintf( sys_string,
		"propagate_inventory_sale_layers %s '' '' '' \"%s\" '' n",
	 	 	application_name,
		 	inventory_sale->inventory_name );

		system( sys_string );

	} while( list_next( customer_sale->inventory_sale_list ) );

} /* post_change_customer_sale_delete() */

void post_change_customer_sale_FOB_shipping_new_rule(
				CUSTOMER_SALE *customer_sale,
				char *application_name )
{
	if ( customer_sale->shipped_date_time )
	{
		post_change_customer_sale_new_transaction(
			customer_sale,
			customer_sale->shipped_date_time
				/* transaction_date_time */,
			application_name );
	}

} /* post_change_customer_sale_FOB_shipping_new_rule() */

void post_change_customer_sale_FOB_destination_new_rule(
				CUSTOMER_SALE *customer_sale,
				char *application_name )
{
	if ( customer_sale->arrived_date )
	{
		customer_sale->transaction_date_time =
			predictive__transaction_date_time(
				customer_sale->arrived_date );

		customer_sale->sum_inventory_extension =
			customer_sale_get_sum_inventory_extension(
				customer_sale->inventory_sale_list );

		customer_sale->sum_inventory_extension +=
			customer_sale_get_sum_specific_inventory_extension(
				customer_sale->specific_inventory_sale_list );

		customer_sale->transaction =
			ledger_customer_sale_build_transaction(
				application_name,
				customer_sale->full_name,
				customer_sale->street_address,
				customer_sale->transaction_date_time,
				customer_sale_get_memo(
					customer_sale->full_name ),
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

	} /* if arrived_date */

} /* post_change_customer_sale_FOB_destination_new_rule() */

void post_change_customer_sale_transaction_date_time_update(
			CUSTOMER_SALE *customer_sale,
			char *transaction_date_time,
			char *application_name )
{
	char *propagate_transaction_date_time;
	char sys_string[ 1024 ];

	if ( !customer_sale->transaction )
	{
		fprintf( stderr,
		"Warning in %s/%s()/%d: empty customer_sale->transaction.\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__ );
		return;
	}

	/* --------------------------------------------------------------- */
	/* Note: customer_sale->transaction->transaction_date_time has the */
	/*       old value.						   */
	/* --------------------------------------------------------------- */

	ledger_transaction_generic_update(
		application_name,
		customer_sale->transaction->full_name,
		customer_sale->transaction->street_address,
		customer_sale->transaction->transaction_date_time,
		"transaction_date_time",
		transaction_date_time );

	ledger_journal_generic_update(
		application_name,
		customer_sale->transaction->full_name,
		customer_sale->transaction->street_address,
		customer_sale->transaction->transaction_date_time,
		"transaction_date_time",
		transaction_date_time );

	if (	strcmp(	transaction_date_time,
			customer_sale->
				transaction->
				transaction_date_time ) < 0  )
	{
		propagate_transaction_date_time = transaction_date_time;
	}
	else
	{
		propagate_transaction_date_time =
			customer_sale->
				transaction->
				transaction_date_time;
	}

	sprintf(sys_string,
 		"propagate_customer_sale_accounts %s \"%s\" \"%s\"",
 		application_name,
		"" /* fund_name */,
 		(propagate_transaction_date_time)
			? propagate_transaction_date_time
			: "" );

	system( sys_string );

	sprintf(sys_string,
 		"propagate_purchase_order_accounts ignored fund \"%s\"",
 		(propagate_transaction_date_time)
			? propagate_transaction_date_time
			: "" );

	system( sys_string );

	customer_sale->transaction->transaction_date_time =
		customer_sale->transaction_date_time;

} /* post_change_customer_sale_transaction_date_time_update() */

void post_change_customer_sale_changed_to_FOB_shipping(
				CUSTOMER_SALE *customer_sale,
				char *application_name )
{
	if ( customer_sale->arrived_date )
	{
		if ( !customer_sale->transaction )
		{
			fprintf( stderr,
		"Warning in %s/%s()/%d: empty customer_sale->transaction.\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__ );
		}

		customer_sale->transaction_date_time =
			customer_sale->shipped_date_time;

		post_change_customer_sale_transaction_date_time_update(
			customer_sale,
			customer_sale->transaction_date_time,
			application_name );
	}
	else
	{
		if ( customer_sale->shipped_date_time )
		{
			post_change_customer_sale_new_transaction(
				customer_sale,
				customer_sale->shipped_date_time
					/* transaction_date_time */,
				application_name );
		}
	}

} /* post_change_customer_sale_changed_to_FOB_shipping() */

void post_change_customer_sale_changed_to_FOB_destination(
				CUSTOMER_SALE *customer_sale,
				char *application_name )
{
	if ( customer_sale->shipped_date_time )
	{
		if ( customer_sale->arrived_date )
		{
			customer_sale->transaction_date_time =
				predictive_transaction_date_time(
					customer_sale->arrived_date );

			post_change_customer_sale_transaction_date_time_update(
				customer_sale,
				customer_sale->transaction_date_time,
				application_name );
		}
		else
		{
			char *sales_tax_payable_full_name = {0};
			char *sales_tax_payable_street_address = {0};

			if ( !customer_sale->transaction )
			{
				fprintf( stderr,
		"Warning in %s/%s()/%d: empty customer_sale->transaction.\n",
			 		__FILE__,
			 		__FUNCTION__,
			 		__LINE__ );
				return;
			}


			if ( customer_sale->sales_tax )
			{
				ENTITY *sales_tax_payable_entity;

				if ( ! ( sales_tax_payable_entity =
					    entity_get_sales_tax_payable_entity(
							application_name ) ) )
				{
					fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot get sales_tax_payable_entity.\n",
			 			__FILE__,
			 			__FUNCTION__,
			 			__LINE__ );
					exit( 1 );
				}

				sales_tax_payable_full_name =
					sales_tax_payable_entity->
						full_name;

				sales_tax_payable_street_address =
					sales_tax_payable_entity->
						street_address;

			}

			customer_sale_transaction_delete_with_propagate(
				application_name,
				customer_sale->fund_name,
				customer_sale->full_name,
				customer_sale->street_address,
				sales_tax_payable_full_name,
				sales_tax_payable_street_address,
				customer_sale->transaction_date_time );

			customer_sale->transaction = (TRANSACTION *)0;
			customer_sale->transaction_date_time = (char *)0;

		} /* if not arrived */

	} /* if shipped_date_time */

} /* post_change_customer_sale_changed_to_FOB_destination() */

void post_change_customer_sale_title_rule_null(
			CUSTOMER_SALE *customer_sale,
			char *application_name )
{
	if ( customer_sale->completed_date_time )
	{
		if ( customer_sale->transaction )
		{
			customer_sale->transaction_date_time =
				customer_sale->completed_date_time;

			post_change_customer_sale_transaction_date_time_update(
				customer_sale,
				customer_sale->transaction_date_time,
				application_name );
		}
		else
		{
			post_change_customer_sale_new_transaction(
				customer_sale,
				customer_sale->completed_date_time
					/* transaction_date_time */,
				application_name );
		}
	}

} /* post_change_customer_sale_title_rule_null() */

void post_change_customer_sale_new_transaction(
			CUSTOMER_SALE *customer_sale,
			char *transaction_date_time,
			char *application_name )
{
	if ( customer_sale->transaction_date_time )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: not expecting a transaction_date_time.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	customer_sale->transaction_date_time = transaction_date_time;

	customer_sale->transaction =
		ledger_customer_sale_build_transaction(
			application_name,
			customer_sale->full_name,
			customer_sale->street_address,
			customer_sale->transaction_date_time,
			CUSTOMER_SALE_MEMO,
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
			customer_sale->transaction->journal_ledger_list );
	}

} /* post_change_customer_sale_new_transaction() */

