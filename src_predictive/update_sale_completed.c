/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/update_sale_completed.c		*/
/* Linked to update_sale_not_completed					*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "document.h"
#include "environ.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "journal.h"
#include "sale.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void update_sale_completed(	SALE *sale );

void update_sale_not_completed(	SALE *sale );

void display_sale(		SALE *sale );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char title[ 128 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	SALE *sale;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf( stderr, 
"Usage: %s process full_name street_address sale_date_time\n",
		argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	sale_date_time = argv[ 4 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output( appliction_name );

	format_initial_capital( title, process_name );
	printf( "<h1>%s</h1>\n", title );

	sale =
		/* Returns sale_steady_state() */
		/* --------------------------- */
		sale_fetch(
			full_name,
			street_address,
			sale_date_time );

	if ( !sale )
	{
		printf(
		"<h3>Error: please select a sale.</h3>\n" );
		document_tag_close();
		exit( 0 );
	}

	if ( strcmp( argv[ 0 ], "update_sale_completed" ) == 0 )
	{
		update_sale_completed( sale );
	}
	else
	if ( strcmp( argv[ 0 ], "update_sale_not_completed" ) == 0 )
	{
		update_sale_not_completed( sale );
	}

	document_tag_close( stdout );
	return 0;
}

void display_sale( SALE *sale )
{
if ( sale ){}
/*
	select =
	"full_name,street_address,completed_date_time,invoice_amount";
		 "html_table.e '' '%s' '%c' 'left,left,left,right'	 ",
		 select,
		 FOLDER_DATA_DELIMITER );
*/
}

void update_sale_completed( SALE *sale )
{
	if ( sale->completed_date_time )
	{
		printf(
		"<h3>Error: this customer sale is already completed.</h3>\n" );
		document_close();
		exit( 0 );
	}

	sale->completed_date_time = timlib_now_date_time();

	sale->sale_transaction =
		sale_transaction(
			sale->
				customer_entity->
				full_name,
			sale->
				customer_entity->
				street_address,
			sale->sale_date_time,
			sale->sale_invoice_amount,
			sale->sale_gross_revenue,
			sale->sales_tax,
			sale->shipping_charge,
			sale->account_receivable,
			sale->account_revenue,
			sale->account_shipping_revenue,
			sale->account_sales_tax_payable );

	sale->sale_transaction->transaction_date_time =
		transaction_refresh(
			sale->sale_transaction->full_name,
			sale->sale_transaction->street_address,
			sale->sale_transaction->transaction_date_time,
			sale->sale_transaction->transaction_amount,
			sale->sale_transaction->memo,
			0 /* check_number */,
			sale->sale_transaction->lock_transaction,
			sale->sale_transaction->journal_list );

	sale_update(
			sale->inventory_sale_total,
			sale->fixed_service_sale_total,
			sale->hourly_service_sale_total,
			sale->sale_gross_revenue,
			sale->sales_tax,
			sale->sale_invoice_amount,
			sale->customer_payment_total,
			sale->sale_amount_due,
			sale->sale_transaction->transaction_date_time,
			sale->customer_entity->full_name,
			sale->customer_entity->street_address,
			sale->sale_date_time );

	display_sale( sale );

	journal_list_html_display(
		sale->sale_transaction->journal_list,
		sale->sale_transaction->transaction_date_time,
		sale->sale_transaction->memo );

	printf( "<p>Marked sale completed.\n" );

	document_close();
}

void update_sale_not_completed(	SALE *sale )
{
	if ( !sale->completed_date_time )
	{
		printf(
	"<h3>Error: this customer sale is currently not completed.</h3>\n" );
		document_close();
		exit( 0 );
	}

	sale->completed_date_time = (char *)0;

	/* Performs journal_propagate() */
	/* ---------------------------- */
	transaction_delete(
		sale->
			sale_transaction->
			full_name,
		sale->
			sale_transaction->
			street_address,
		sale->
			sale_transaction->
			transaction_date_time );

	sale->sale_transaction = (TRANSACTION *)0;

	display_sale( sale );

	printf( "<p>Marked sale not completed.\n" );

	document_close();
}

