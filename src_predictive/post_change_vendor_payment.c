/* ------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/post_change_vendor_payment.c		*/
/* ------------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ------------------------------------------------------------------	*/

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
#include "purchase.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void post_change_vendor_payment_amount_update(
				PURCHASE_ORDER *purchase_order,
				VENDOR_PAYMENT *vendor_payment,
				char *application_name );

void post_change_vendor_payment_entity(
				char *full_name,
				char *street_address,
				LIST *vendor_payment_list,
				char *preupdate_full_name,
				char *preupdate_street_address,
				char *application_name );

void post_change_vendor_payment_date_time_update(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *payment_date_time,
				char *preupdate_payment_date_time );

void post_change_vendor_payment_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *payment_date_time,
				char *preupdate_full_name,
				char *preupdate_street_address,
				char *preupdate_payment_date_time,
				char *preupdate_payment_amount );

void post_change_vendor_payment_delete(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *payment_date_time );

void post_change_vendor_payment_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *payment_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *payment_date_time;
	char *state;
	char *preupdate_full_name;
	char *preupdate_street_address;
	char *preupdate_payment_date_time;
	char *preupdate_payment_amount;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 11 )
	{
		fprintf( stderr,
"Usage: %s ignored full_name street_address purchase_date_time payment_date_time state preupdate_full_name preupdate_street_address preupdate_payment_date_time preupdate_payment_amount\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	purchase_date_time = argv[ 4 ];
	payment_date_time = argv[ 5 ];
	state = argv[ 6 ];
	preupdate_full_name = argv[ 7 ];
	preupdate_street_address = argv[ 8 ];
	preupdate_payment_date_time = argv[ 9 ];
	preupdate_payment_amount = argv[ 10 ];

	if ( strcmp( purchase_date_time, "purchase_date_time" ) == 0 )
		exit( 0 );

	if ( strcmp( payment_date_time, "payment_date_time" ) == 0 )
		exit( 0 );

	/* Execute on predelete because VENDOR_PAYMENT.transaction_date_time */
	/* ----------------------------------------------------------------- */
	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_vendor_payment_insert(
				application_name,
				full_name,
				street_address,
				purchase_date_time,
				payment_date_time );
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_vendor_payment_update(
				application_name,
				full_name,
				street_address,
				purchase_date_time,
				payment_date_time,
				preupdate_full_name,
				preupdate_street_address,
				preupdate_payment_date_time,
				preupdate_payment_amount);
	}
	else
	if ( strcmp( state, "predelete" ) == 0 )
	{
		post_change_vendor_payment_delete(
				application_name,
				full_name,
				street_address,
				purchase_date_time,
				payment_date_time );
	}

	return 0;

} /* main() */

void post_change_vendor_payment_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *payment_date_time )
{
	PURCHASE_ORDER *purchase_order;
	VENDOR_PAYMENT *vendor_payment;
	LIST *propagate_account_list;

	if ( !( purchase_order =
		purchase_order_new(
			application_name,
			full_name,
			street_address,
			purchase_date_time ) ) )
	{
		return;
	}

	/* amount_due set in purchase_order_new(). */
	/* --------------------------------------- */
	purchase_order_update(
		application_name,
		purchase_order->full_name,
		purchase_order->street_address,
		purchase_order->purchase_date_time,
		purchase_order->sum_extension,
		purchase_order->database_sum_extension,
		purchase_order->purchase_amount,
		purchase_order->database_purchase_amount,
		purchase_order->amount_due,
		purchase_order->database_amount_due,
		purchase_order->transaction_date_time,
		purchase_order->database_transaction_date_time,
		purchase_order->arrived_date_time,
		purchase_order->database_arrived_date_time,
		purchase_order->shipped_date,
		purchase_order->database_shipped_date );

	if ( ! ( vendor_payment =
			purchase_vendor_payment_seek(
				purchase_order->vendor_payment_list,
				payment_date_time ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot seek payment_date_time = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 payment_date_time );
		exit( 1 );
	}

	if ( vendor_payment->transaction_date_time )
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: not expecting transaction_date_time = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 vendor_payment->transaction_date_time );
	}
	else
	{
		vendor_payment->transaction_date_time =
			vendor_payment->payment_date_time;
	}

	/* Insert the TRANSACTION */
	/* ---------------------- */
	vendor_payment->transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			vendor_payment->transaction_date_time,
			PURCHASE_VENDOR_PAYMENT_MEMO );
	
	vendor_payment->transaction_date_time =
	vendor_payment->transaction->transaction_date_time =
	ledger_transaction_insert(
		application_name,
		vendor_payment->transaction->full_name,
		vendor_payment->transaction->street_address,
		vendor_payment->transaction->transaction_date_time,
		vendor_payment->payment_amount /* transaction_amount */,
		vendor_payment->transaction->memo,
		vendor_payment->check_number,
		1 /* lock_transaction */ );

	purchase_vendor_payment_update(
		application_name,
		full_name,
		street_address,
		purchase_date_time,
		payment_date_time,
		vendor_payment->transaction_date_time,
		vendor_payment->database_transaction_date_time );

	if ( ( propagate_account_list =
		purchase_vendor_payment_journal_ledger_refresh(
			application_name,
			purchase_order->fund_name,
			vendor_payment->transaction->full_name,
			vendor_payment->transaction->street_address,
			vendor_payment->
				transaction->
				transaction_date_time,
			vendor_payment->payment_amount,
			vendor_payment->check_number ) ) )
	{
		ledger_account_list_propagate(
			propagate_account_list,
			application_name );
	}

} /* post_change_vendor_payment_insert() */

void post_change_vendor_payment_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *payment_date_time )
{
	PURCHASE_ORDER *purchase_order;
	VENDOR_PAYMENT *vendor_payment;
	char *checking_account = {0};
	char *uncleared_checks_account = {0};
	char *account_payable_account = {0};

	/* Update purchase_order->amount_due */
	/* --------------------------------- */
	purchase_order =
		purchase_order_new(
			application_name,
			full_name,
			street_address,
			purchase_date_time );

	if ( !purchase_order )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot find purchase order.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	if ( ! ( vendor_payment =
			purchase_vendor_payment_seek(
				purchase_order->vendor_payment_list,
				payment_date_time ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot seek payment_date_time = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 payment_date_time );
		exit( 1 );
	}

	list_delete_current( purchase_order->vendor_payment_list );

	purchase_order->sum_payment_amount =
		purchase_get_sum_payment_amount(
			purchase_order->vendor_payment_list );

	purchase_order->amount_due =
		PURCHASE_GET_AMOUNT_DUE(
			purchase_order->purchase_amount,
			purchase_order->sum_payment_amount );

	purchase_order_update(
			application_name,
			purchase_order->full_name,
			purchase_order->street_address,
			purchase_order->purchase_date_time,
			purchase_order->sum_extension,
			purchase_order->database_sum_extension,
			purchase_order->purchase_amount,
			purchase_order->database_purchase_amount,
			purchase_order->amount_due,
			purchase_order->database_amount_due,
			purchase_order->transaction_date_time,
			purchase_order->database_transaction_date_time,
			purchase_order->arrived_date_time,
			purchase_order->database_arrived_date_time,
			purchase_order->shipped_date,
			purchase_order->database_shipped_date );

	/* Delete TRANSACTION and JOURNAL_LEDGER */
	/* ------------------------------------- */
	if ( !vendor_payment->transaction )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: empty transaction for payment_date_time = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 payment_date_time );
		exit( 1 );
	}

	ledger_delete(	application_name,
			TRANSACTION_FOLDER_NAME,
			vendor_payment->transaction->full_name,
			vendor_payment->transaction->street_address,
			vendor_payment->transaction->transaction_date_time );

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			vendor_payment->transaction->full_name,
			vendor_payment->transaction->street_address,
			vendor_payment->transaction->transaction_date_time );

	ledger_get_vendor_payment_account_names(
		&checking_account,
		&uncleared_checks_account,
		&account_payable_account,
		application_name,
		purchase_order->fund_name );

	ledger_propagate(
		application_name,
		vendor_payment->transaction_date_time,
		checking_account );

	ledger_propagate(
		application_name,
		vendor_payment->transaction_date_time,
		uncleared_checks_account );

	ledger_propagate(
		application_name,
		vendor_payment->transaction_date_time,
		account_payable_account );

} /* post_change_vendor_payment_delete() */

void post_change_vendor_payment_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *payment_date_time,
				char *preupdate_full_name,
				char *preupdate_street_address,
				char *preupdate_payment_date_time,
				char *preupdate_payment_amount )
{
	enum preupdate_change_state full_name_change_state;
	enum preupdate_change_state street_address_change_state;
	enum preupdate_change_state payment_date_time_change_state = -1;
	enum preupdate_change_state payment_amount_change_state;
	PURCHASE_ORDER *purchase_order;
	VENDOR_PAYMENT *vendor_payment;

	purchase_order =
		purchase_order_new(
			application_name,
			full_name,
			street_address,
			purchase_date_time );

	if ( !purchase_order )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot find purchase order.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	if ( ! ( vendor_payment =
			purchase_vendor_payment_seek(
				purchase_order->vendor_payment_list,
				payment_date_time ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot seek vendor payment = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 payment_date_time );
		exit( 0 );
	}

	if ( !vendor_payment->transaction )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: empty transaction for payment_date_time = (%s).\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__,
		 	payment_date_time );
		exit( 1 );
	}

	full_name_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_full_name,
			full_name /* postupdate_data */,
			"preupdate_full_name" );

	street_address_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_street_address,
			street_address /* postupdate_data */,
			"preupdate_street_address" );

	payment_date_time_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_payment_date_time,
			vendor_payment->transaction_date_time
				/* postupdate_data */,
			"preupdate_payment_date_time" );

	payment_amount_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_payment_amount,
			(char *)0 /* postupdate_data */,
			"preupdate_payment_amount" );

	if ( full_name_change_state == from_something_to_something_else
	||   street_address_change_state == from_something_to_something_else )
	{
		post_change_vendor_payment_entity(
			full_name,
			street_address,
			purchase_order->vendor_payment_list,
			preupdate_full_name,
			preupdate_street_address,
			application_name );
	}

	if (	payment_date_time_change_state ==
		from_something_to_something_else )
	{
		post_change_vendor_payment_date_time_update(
			application_name,
			purchase_order->fund_name,
			full_name,
			street_address,
			purchase_date_time,
			payment_date_time,
			preupdate_payment_date_time );
	}

	if (	payment_amount_change_state ==
		from_something_to_something_else )
	{
		post_change_vendor_payment_amount_update(
			purchase_order,
			vendor_payment,
			application_name );
	}

} /* post_change_vendor_payment_update() */

void post_change_vendor_payment_date_time_update(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *payment_date_time,
				char *preupdate_payment_date_time )
{
	char *checking_account = {0};
	char *uncleared_checks_account = {0};
	char *account_payable_account = {0};
	char *propagate_transaction_date_time;

	ledger_transaction_generic_update(
		application_name,
		full_name,
		street_address,
		preupdate_payment_date_time,
		"transaction_date_time",
		payment_date_time );

	ledger_journal_generic_update(
		application_name,
		full_name,
		street_address,
		preupdate_payment_date_time,
		"transaction_date_time",
		payment_date_time );

	if ( strcmp(	preupdate_payment_date_time,
			payment_date_time ) < 0 )
	{
		propagate_transaction_date_time =
			preupdate_payment_date_time;
	}
	else
	{
		propagate_transaction_date_time =
			payment_date_time;
	}

	ledger_get_vendor_payment_account_names(
		&checking_account,
		&uncleared_checks_account,
		&account_payable_account,
		application_name,
		fund_name );

	ledger_propagate(	application_name,
				propagate_transaction_date_time,
				checking_account );

	ledger_propagate(	application_name,
				propagate_transaction_date_time,
				account_payable_account );

	purchase_vendor_payment_update(
		application_name,
		full_name,
		street_address,
		purchase_date_time,
		payment_date_time,
		payment_date_time
			/* transaction_date_time */,
		preupdate_payment_date_time
			/* database_transaction_date_time */ );

} /* post_change_vendor_payment_date_time_update() */

void post_change_vendor_payment_entity(
			char *full_name,
			char *street_address,
			LIST *vendor_payment_list,
			char *preupdate_full_name,
			char *preupdate_street_address,
			char *application_name )
{
	VENDOR_PAYMENT *vendor_payment;

	if ( !list_rewind( vendor_payment_list ) ) return;

	do {
		vendor_payment = list_get_pointer( vendor_payment_list );


		if ( !vendor_payment->transaction )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: empty transaction for payment_date_time = (%s).\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	vendor_payment->payment_date_time );
			exit( 1 );
		}


		ledger_entity_update(	application_name,
					full_name,
					street_address,
					vendor_payment->transaction_date_time,
					preupdate_full_name,
					preupdate_street_address );

	} while( list_next( vendor_payment_list ) );

} /* post_change_vendor_payment_entity() */

void post_change_vendor_payment_amount_update(
			PURCHASE_ORDER *purchase_order,
			VENDOR_PAYMENT *vendor_payment,
			char *application_name )
{
	LIST *propagate_account_list;

	/* amount_due set in purchase_order_new(). */
	/* --------------------------------------- */
	purchase_order_update(
		application_name,
		purchase_order->full_name,
		purchase_order->street_address,
		purchase_order->purchase_date_time,
		purchase_order->sum_extension,
		purchase_order->database_sum_extension,
		purchase_order->purchase_amount,
		purchase_order->database_purchase_amount,
		purchase_order->amount_due,
		purchase_order->database_amount_due,
		purchase_order->transaction_date_time,
		purchase_order->database_transaction_date_time,
		purchase_order->arrived_date_time,
		purchase_order->database_arrived_date_time,
		purchase_order->shipped_date,
		purchase_order->database_shipped_date );

	if ( !vendor_payment->transaction_date_time )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty transaction_date_time.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	ledger_transaction_amount_update(
		application_name,
		vendor_payment->transaction->full_name,
		vendor_payment->transaction->street_address,
		vendor_payment->transaction->transaction_date_time,
		vendor_payment->payment_amount,
		0.0 /* database_transaction_amount */ );

	if ( ( propagate_account_list =
		purchase_vendor_payment_journal_ledger_refresh(
			application_name,
			purchase_order->fund_name,
			vendor_payment->transaction->full_name,
			vendor_payment->transaction->street_address,
			vendor_payment->
				transaction->
				transaction_date_time,
			vendor_payment->payment_amount,
			vendor_payment->check_number ) ) )
	{
		ledger_account_list_propagate(
			propagate_account_list,
			application_name );
	}

} /* post_change_vendor_payment_amount_update() */

