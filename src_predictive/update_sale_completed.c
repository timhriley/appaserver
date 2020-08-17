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
#include "customer.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void update_sale_completed(	CUSTOMER_SALE *customer_sale,
				char *application_name );

void update_sale_not_completed(	CUSTOMER_SALE *customer_sale,
				char *application_name );

void display_customer_sale(	char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *full_name;
	char *street_address;
	char *sale_date_time;
	DOCUMENT *document;
	char title[ 128 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	CUSTOMER_SALE *customer_sale;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s ignored process full_name street_address sale_date_time\n",
		argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	full_name = argv[ 3 ];
	street_address = argv[ 4 ];
	sale_date_time = argv[ 5 ];

	appaserver_parameter_file = appaserver_parameter_file_new();

	format_initial_capital( title, process_name );
	document = document_new( title, application_name );
	document->output_content_type = 1;

	document_output_head_stream(
			stdout,
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */,
			1 /* with close_head */ );

	document_output_body(	document->application_name,
				document->onload_control_string );

	printf( "<h1>%s</h1>\n", title );

	customer_sale =
		customer_sale_new(
			application_name,
			full_name,
			street_address,
			sale_date_time );

	if ( !customer_sale )
	{
		printf(
		"<h3>Error: please select a customer sale.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( argv[ 0 ], "update_sale_completed" ) == 0 )
	{
		update_sale_completed(	customer_sale,
					application_name );
	}
	else
	if ( strcmp( argv[ 0 ], "update_sale_not_completed" ) == 0 )
	{
		update_sale_not_completed(
					customer_sale,
					application_name );
	}
	else
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: invalid argv[ 0 ].\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return 0;

} /* main() */

void display_customer_sale(	char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time )
{
	char sys_string[ 1024 ];
	char *select;
	char *where;

	select =
	"full_name,street_address,completed_date_time,invoice_amount";

	where = ledger_get_transaction_where(
				full_name,
				street_address,
				sale_date_time,
				(char *)0 /* folder_name */,
				"sale_date_time" );

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			select=%s			 "
		 "			folder=customer_sale		 "
		 "			where=\"%s\"			|"
		 "html_table.e '' '%s' '%c' 'left,left,left,right'	 ",
		 application_name,
		 select,
		 where,
		 select,
		 FOLDER_DATA_DELIMITER );

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

} /* display_customer_sale() */

void update_sale_completed(	CUSTOMER_SALE *customer_sale,
				char *application_name )

{
	char sys_string[ 1024 ];

	if ( customer_sale->completed_date_time )
	{
		printf(
		"<h3>Error: this customer sale is already completed.</h3>\n" );
		document_close();
		exit( 0 );
	}

	customer_sale->completed_date_time = timlib_get_now_date_time();

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

	sprintf( sys_string,
"post_change_customer_sale %s \"%s\" \"%s\" \"%s\" update preupdate_full_name preupdate_street_address preupdate_title_passage_rule '' preupdate_shipped_date_time preupdate_arrived_date preupdate_shipping_revenue",
		 application_name,
		 customer_sale->full_name,
		 customer_sale->street_address,
		 customer_sale->sale_date_time );

	system( sys_string );

	customer_sale =
		customer_sale_new(
			application_name,
			customer_sale->full_name,
			customer_sale->street_address,
			customer_sale->sale_date_time );

	display_customer_sale(	application_name,
				customer_sale->full_name,
				customer_sale->street_address,
				customer_sale->sale_date_time );

	if ( !customer_sale->transaction )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty transaction.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	ledger_list_html_display(
		(char *)0 /* transaction_memo */,
		customer_sale->transaction->journal_ledger_list );

	printf( "<p>Marked sale completed.\n" );

	document_close();

} /* update_sale_completed() */

void update_sale_not_completed(	CUSTOMER_SALE *customer_sale,
				char *application_name )
{
	char sys_string[ 1024 ];
	char *preupdate_completed_date_time;

	if ( !customer_sale->completed_date_time )
	{
		printf(
	"<h3>Error: this customer sale is currently not completed.</h3>\n" );
		document_close();
		exit( 0 );
	}

	preupdate_completed_date_time =
		customer_sale->completed_date_time;

	customer_sale->completed_date_time = (char *)0;

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

	sprintf( sys_string,
"post_change_customer_sale %s \"%s\" \"%s\" \"%s\" update preupdate_full_name preupdate_street_address preupdate_title_passage_rule \"%s\" preupdate_shipped_date_time preupdate_arrived_date preupdate_shipping_revenue",
		 application_name,
		 customer_sale->full_name,
		 customer_sale->street_address,
		 customer_sale->sale_date_time,
		 preupdate_completed_date_time );

	system( sys_string );

	customer_sale =
		customer_sale_new(
			application_name,
			customer_sale->full_name,
			customer_sale->street_address,
			customer_sale->sale_date_time );

	display_customer_sale(	application_name,
				customer_sale->full_name,
				customer_sale->street_address,
				customer_sale->sale_date_time );

	printf( "<p>Marked sale not completed.\n" );

	document_close();

} /* update_sale_not_completed() */

