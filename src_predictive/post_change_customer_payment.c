/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/post_change_customer_payment.c	*/
/* --------------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------------	*/

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
#include "subsidiary_transaction.h"

/* Constants */
/* --------- */
#define CUSTOMER_PAYMENT_FOLDER_NAME	"customer_payment"

/* Prototypes */
/* ---------- */
void post_change_customer_payment_amount_update(
				char *application_name,
				CUSTOMER_SALE *customer_sale,
				CUSTOMER_PAYMENT *customer_payment );

LIST *post_change_customer_payment_get_primary_data_list(
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time );

void post_change_customer_payment_date_time_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time,
				char *preupdate_payment_date_time );

void post_change_customer_payment_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time,
				char *preupdate_payment_date_time,
				char *preupdate_payment_amount );

void post_change_customer_payment_delete(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time );

void post_change_customer_payment_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *payment_date_time;
	char *state;
	char *preupdate_payment_date_time;
	char *preupdate_payment_amount;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 9 )
	{
		fprintf( stderr,
"Usage: %s ignored full_name street_address sale_date_time payment_date_time state preupdate_payment_date_time preupdate_payment_amount\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	sale_date_time = argv[ 4 ];
	payment_date_time = argv[ 5 ];
	state = argv[ 6 ];
	preupdate_payment_date_time = argv[ 7 ];
	preupdate_payment_amount = argv[ 8 ];

	if ( strcmp( sale_date_time, "sale_date_time" ) == 0 ) exit( 0 );

	if ( strcmp( payment_date_time, "payment_date_time" ) == 0 ) exit( 0 );

	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_customer_payment_insert(
				application_name,
				full_name,
				street_address,
				sale_date_time,
				payment_date_time );
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_customer_payment_update(
				application_name,
				full_name,
				street_address,
				sale_date_time,
				payment_date_time,
				preupdate_payment_date_time,
				preupdate_payment_amount );
	}
	else
	if ( strcmp( state, "predelete" ) == 0 )
	/* -------------------------------------------- */
	/* Need to execute on predelete to get		*/
	/* CUSTOMER_PAYMENT.transaction_date_time. 	*/
	/* -------------------------------------------- */
	{
		post_change_customer_payment_delete(
				application_name,
				full_name,
				street_address,
				sale_date_time,
				payment_date_time );
	}

	return 0;
}

void post_change_customer_payment_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time )
{
	CUSTOMER_SALE *customer_sale;
	CUSTOMER_PAYMENT *customer_payment;
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;
	LIST *primary_data_list;
	TRANSACTION *transaction;

	if ( !( customer_sale =
			customer_sale_new(
				application_name,
				full_name,
				street_address,
				sale_date_time ) ) )
	{
		return;
	}

	if ( ! ( customer_payment =
			customer_payment_seek(
				customer_sale->payment_list,
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

	if ( customer_payment->transaction_date_time )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: not expecting transaction_date_time = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 customer_payment->transaction_date_time );
		exit( 1 );
	}

	primary_data_list =
		post_change_customer_payment_get_primary_data_list(
			full_name,
			street_address,
			sale_date_time,
			payment_date_time );

	subsidiary_transaction =
		subsidiary_new(	application_name,
				CUSTOMER_PAYMENT_FOLDER_NAME,
				primary_data_list,
				full_name,
				street_address,
				customer_payment->payment_amount
					/* transaction_amount */ );

#ifdef NOT_DEFINED
	subsidiary_transaction =
		subsidiary_new(	application_name,
				CUSTOMER_PAYMENT_FOLDER_NAME,
				primary_data_list,
				full_name,
				street_address,
				0.0 /* transaction_amount */ );
#endif

	if ( !subsidiary_transaction )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot build transaction for (%s/%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address );
		exit( 1 );
	}

	subsidiary_transaction->output.transaction =
		subsidiary_get_transaction(
			subsidiary_transaction->input.full_name,
			subsidiary_transaction->input.street_address,
			subsidiary_transaction->process.debit_account_name,
			subsidiary_transaction->process.credit_account_name,
			subsidiary_transaction->process.transaction_amount,
			subsidiary_transaction->process.memo );

	if ( !subsidiary_transaction->output.transaction )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot build transaction for (%s/%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address );
		exit( 1 );
	}

	transaction = subsidiary_transaction->output.transaction;

	transaction->transaction_date_time =
		customer_payment->payment_date_time;

	transaction->check_number = customer_payment->check_number,

	transaction->transaction_date_time =
		customer_payment->transaction_date_time =
			ledger_transaction_journal_ledger_insert(
				application_name,
				transaction->full_name,
				transaction->street_address,
				transaction->transaction_date_time,
				transaction->transaction_amount,
				transaction->memo,
				transaction->check_number,
				transaction->lock_transaction,
				transaction->journal_ledger_list );

	customer_payment_update(
		application_name,
		full_name,
		street_address,
		sale_date_time,
		payment_date_time,
		customer_payment->transaction_date_time,
		customer_payment->database_transaction_date_time );

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

}

void post_change_customer_payment_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *payment_date_time )
{
	CUSTOMER_SALE *customer_sale;
	CUSTOMER_PAYMENT *customer_payment;
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;

	customer_sale =
		customer_sale_new(
			application_name,
			full_name,
			street_address,
			sale_date_time );

	if ( !customer_sale )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot find customer sale.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	if ( ! ( customer_payment =
			customer_payment_seek(
				customer_sale->payment_list,
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

	list_delete_current( customer_sale->payment_list );

	customer_sale->total_payment =
		customer_get_total_payment(
			customer_sale->payment_list );

	customer_sale->invoice_amount =
		customer_sale_get_invoice_amount(
			&customer_sale->sum_inventory_extension,
			&customer_sale->sum_fixed_service_extension,
			&customer_sale->sum_hourly_service_extension,
			&customer_sale->sum_extension,
			&customer_sale->sales_tax,
			customer_sale->shipping_revenue,
			customer_sale->inventory_sale_list,
			customer_sale->specific_inventory_sale_list,
			customer_sale->fixed_service_sale_list,
			customer_sale->hourly_service_sale_list,
			customer_sale->full_name,
			customer_sale->street_address,
			application_name );

	customer_sale->amount_due =
		CUSTOMER_GET_AMOUNT_DUE(
			customer_sale->invoice_amount,
			customer_sale->total_payment );

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

	if ( !customer_payment->transaction )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot have empty transaction for payment_date_time = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 payment_date_time );
		exit( 1 );
	}

	ledger_delete(	application_name,
			TRANSACTION_FOLDER_NAME,
			customer_payment->transaction->full_name,
			customer_payment->transaction->street_address,
			customer_payment->
				transaction->
				transaction_date_time );

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			customer_payment->transaction->full_name,
			customer_payment->transaction->street_address,
			customer_payment->
				transaction->
				transaction_date_time );

	subsidiary_transaction = subsidiary_transaction_calloc();

	if ( !subsidiary_transaction_fetch(
		&subsidiary_transaction->process.attribute_name,
		&subsidiary_transaction->process.debit_account_name,
		&subsidiary_transaction->process.credit_account_name,
		(char **)0 /* debit_account_folder_name */,
		application_name,
		CUSTOMER_PAYMENT_FOLDER_NAME ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot subsidiary_transaction_fetch (%s).\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__,
		 	CUSTOMER_PAYMENT_FOLDER_NAME );
		exit( 1 );
	}

	ledger_propagate(
		application_name,
		customer_payment->transaction_date_time,
		subsidiary_transaction->process.debit_account_name );

	ledger_propagate(
		application_name,
		customer_payment->transaction_date_time,
		subsidiary_transaction->process.credit_account_name );
}

void post_change_customer_payment_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time,
				char *preupdate_payment_date_time,
				char *preupdate_payment_amount )
{
	enum preupdate_change_state payment_date_time_change_state;
	enum preupdate_change_state payment_amount_change_state;
	CUSTOMER_SALE *customer_sale;
	CUSTOMER_PAYMENT *customer_payment;

	customer_sale =
		customer_sale_new(
			application_name,
			full_name,
			street_address,
			sale_date_time );

	if ( !customer_sale )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot find customer sale.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	if ( ! ( customer_payment =
			customer_payment_seek(
				customer_sale->payment_list,
				payment_date_time ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot find customer payment.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	if ( !customer_payment->transaction )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot empty transaction for payment_date_time = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 payment_date_time );
		exit( 1 );
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

	payment_date_time_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_payment_date_time,
			customer_payment->transaction_date_time
				/* postupdate_data */,
			"preupdate_payment_date_time" );

	payment_amount_change_state =
		appaserver_library_preupdate_change_state(
			preupdate_payment_amount,
			(char *)0 /* postupdate_data */,
			"preupdate_payment_amount" );

	if (	payment_date_time_change_state ==
		from_something_to_something_else )
	{
		post_change_customer_payment_date_time_update(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			payment_date_time,
			preupdate_payment_date_time );
	}

	if (	payment_amount_change_state ==
		from_something_to_something_else )
	{
		post_change_customer_payment_amount_update(
			application_name,
			customer_sale,
			customer_payment );
	}

	if ( customer_payment->check_number
	!=   customer_payment->transaction->check_number )
	{
		char check_number_string[ 16 ];

		sprintf( check_number_string,
			 "%d",
			 customer_payment->check_number );

		ledger_transaction_generic_update(
			application_name,
			customer_sale->full_name,
			customer_sale->street_address,
			customer_payment->transaction_date_time,
			"check_number" /* attribute_name */,
			check_number_string /* data */ );
	}

}

void post_change_customer_payment_amount_update(
				char *application_name,
				CUSTOMER_SALE *customer_sale,
				CUSTOMER_PAYMENT *customer_payment )
{
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;

	subsidiary_transaction = subsidiary_transaction_calloc();

	if ( !subsidiary_transaction_fetch(
		&subsidiary_transaction->process.attribute_name,
		&subsidiary_transaction->process.debit_account_name,
		&subsidiary_transaction->process.credit_account_name,
		(char **)0 /* debit_account_folder_name */,
		application_name,
		CUSTOMER_PAYMENT_FOLDER_NAME ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot subsidiary_transaction_fetch (%s).\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__,
		 	CUSTOMER_PAYMENT_FOLDER_NAME );
		exit( 1 );
	}

	ledger_transaction_amount_update(
		application_name,
		customer_sale->full_name,
		customer_sale->street_address,
		customer_payment->transaction_date_time,
		customer_payment->payment_amount,
		0.0 /* database_payment_amount */ );

	ledger_debit_credit_update(
		application_name,
		customer_sale->full_name,
		customer_sale->street_address,
		customer_payment->transaction_date_time,
		subsidiary_transaction->process.debit_account_name,
		subsidiary_transaction->process.credit_account_name,
		customer_payment->payment_amount
			/* transaction_amount  */ );

	ledger_propagate(
		application_name,
		customer_payment->transaction_date_time,
		subsidiary_transaction->process.debit_account_name );

	ledger_propagate(
		application_name,
		customer_payment->transaction_date_time,
		subsidiary_transaction->process.credit_account_name );
}

void post_change_customer_payment_date_time_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time,
				char *preupdate_payment_date_time )
{
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;
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

	subsidiary_transaction = subsidiary_transaction_calloc();

	if ( !subsidiary_transaction_fetch(
		&subsidiary_transaction->process.attribute_name,
		&subsidiary_transaction->process.debit_account_name,
		&subsidiary_transaction->process.credit_account_name,
		(char **)0 /* debit_account_folder_name */,
		application_name,
		CUSTOMER_PAYMENT_FOLDER_NAME ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot subsidiary_transaction_fetch (%s).\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__,
		 	CUSTOMER_PAYMENT_FOLDER_NAME );
		exit( 1 );
	}

	ledger_propagate(
		application_name,
		propagate_transaction_date_time,
		subsidiary_transaction->process.debit_account_name );

	ledger_propagate(
		application_name,
		propagate_transaction_date_time,
		subsidiary_transaction->process.credit_account_name );

	customer_payment_update(
		application_name,
		full_name,
		street_address,
		sale_date_time,
		payment_date_time,
		payment_date_time
			/* transaction_date_time */,
		preupdate_payment_date_time
			/* database_transaction_date_time */ );

}

LIST *post_change_customer_payment_get_primary_data_list(
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time )
{
	LIST *primary_data_list = list_new();
	list_append_pointer( primary_data_list, full_name );
	list_append_pointer( primary_data_list, street_address );
	list_append_pointer( primary_data_list, sale_date_time );
	list_append_pointer( primary_data_list, payment_date_time );

	return primary_data_list;

}
