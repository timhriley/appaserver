/* ---------------------------------------------------------------	*/
/* src_predictive/make_vendor_payment.c					*/
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
#include "purchase.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
boolean total_payment_amount_exceeds_amount_due(
					char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time,
					double payment_amount,
					double total_payment_amount );

void insert_vendor_payment(	char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *payment_date_time,
				double payment_amount,
				int check_number );

void display_purchase_order(	char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *full_name;
	char *street_address;
	char *purchase_date_time;
	char *payment_date_time;
	double payment_amount;
	int check_number;
	boolean paid_amount_due;
	DOCUMENT *document;
	char title[ 128 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	double total_payment;
	char sys_string[ 1024 ];

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 9 )
	{
		fprintf( stderr, 
"Usage: %s ignored process full_name street_address purchase_date_time payment_amount check_number paid_amount_due_yn\n",
		argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	full_name = argv[ 3 ];
	street_address = argv[ 4 ];
	purchase_date_time = argv[ 5 ];
	payment_amount = atof( argv[ 6 ] );
	check_number = atoi( argv[ 7 ] );
	paid_amount_due = ( *argv[ 8 ] == 'y' );

	payment_date_time = timlib_get_now_date_time();

	appaserver_parameter_file = appaserver_parameter_file_new();

	format_initial_capital( title, process_name );
	document = document_new( title, application_name );
	document->output_content_type = 1;

	document_output_head_stream(
			stdout,
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->
				appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */,
			1 /* with close_head */ );

	document_output_body(	document->application_name,
				document->onload_control_string );

	printf( "<h1>%s</h1>\n", title );

	if ( !*purchase_date_time
	||   strcmp( purchase_date_time, "purchase_date_time" ) == 0 )
	{
		printf(
		"<h3>Error: please select a purchase order.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( paid_amount_due )
	{
		payment_amount = 
			purchase_order_get_amount_due(
					application_name,
					full_name,
					street_address,
					purchase_date_time );
	}

	total_payment =
		purchase_order_get_total_payment(
					application_name,
					full_name,
					street_address,
					purchase_date_time );

	if ( paid_amount_due && !payment_amount )
	{
		printf(
"<h3>Error: the purchase order is already paid in full.</h3>\n" );

		document_close();
		exit( 0 );
	}
	else
	if ( !payment_amount )
	{
		printf(
"<h3>Error: the payment amount is missing. Total existing payments = %.2lf</h3>\n",
			total_payment );

		document_close();
		exit( 0 );
	}

	if ( total_payment_amount_exceeds_amount_due(
				application_name,
				full_name,
				street_address,
				purchase_date_time,
				payment_amount,
				total_payment ) )
	{
		if ( total_payment )
		{
			printf(
"<h3>Error: the payment amount plus existing payments of %.2lf exceed amount due.</h3>\n",
				total_payment );
		}
		else
		{
			printf(
"<h3>Error: the payment amount exceeds amount due.</h3>\n" );
		}
		document_close();
		exit( 0 );
	}

	if ( payment_amount )
	{
		insert_vendor_payment(
				application_name,
				full_name,
				street_address,
				purchase_date_time,
				payment_date_time,
				payment_amount,
				check_number );
	}

	sprintf( sys_string,
	"post_change_vendor_payment %s \"%s\" \"%s\" '%s' '%s' insert ''",
		 application_name,
		 full_name,
		 street_address,
		 purchase_date_time,
		 payment_date_time );

	system( sys_string );

	display_purchase_order(	application_name,
				full_name,
				street_address,
				purchase_date_time );

	printf( "<p>Payment stored.\n" );

	document_close();

	return 0;

} /* main() */

void display_purchase_order(	char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time )
{
	char sys_string[ 1024 ];
	char *select;
	char *where;
	char *justify;

	select =
"full_name,street_address,purchase_date_time,purchase_amount,amount_due";

	where = ledger_get_transaction_where(
				full_name,
				street_address,
				purchase_date_time,
				(char *)0 /* folder_name */,
				"purchase_date_time" );

	justify = "left,left,left,right,right";

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=%s		 "
		 "			folder=purchase_order	 "
		 "			where=\"%s\"		|"
		 "html_table.e '' '%s' '%c' '%s'		 ",
		 application_name,
		 select,
		 where,
		 select,
		 FOLDER_DATA_DELIMITER,
		 justify );

	fflush( stdout );
	system( sys_string );
	fflush( stdout );

} /* display_purchase_order() */

void insert_vendor_payment(	char *application_name,
				char *full_name,
				char *street_address,
				char *purchase_date_time,
				char *payment_date_time,
				double payment_amount,
				int check_number )
{
	
	purchase_vendor_payment_insert(
		application_name,
		full_name,
		street_address,
		purchase_date_time,
		payment_date_time,
		payment_amount,
		check_number,
		(char *)0 /* transaction_date_time */ );

/*
	char sys_string[ 1024 ];
	char *field_list_string;
	FILE *output_pipe;
	char *table_name;
	char buffer[ 128 ];

	field_list_string =
"full_name,street_address,purchase_date_time,payment_date_time,payment_amount,check_number";

	table_name = get_table_name( application_name, "vendor_payment" );

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s | sql.e",
		 table_name,
		 field_list_string );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
		 "%s|%s|%s|%s|%.2lf",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 street_address,
		 purchase_date_time,
		 payment_date_time,
		 payment_amount );

	if ( check_number )
	{
		fprintf( output_pipe,
			 "|%d\n",
			 check_number );
	}
	else
	{
		fprintf( output_pipe, "|\n" );
	}

	pclose( output_pipe );
*/

} /* insert_vendor_payment() */

boolean total_payment_amount_exceeds_amount_due(
					char *application_name,
					char *full_name,
					char *street_address,
					char *purchase_date_time,
					double payment_amount,
					double total_payment )
{
	char *table_name;
	char sys_string[ 1024 ];
	char *results_string;
	double amount_due;
	double local_total_payment;
	char *where;

	table_name = get_table_name( application_name, "purchase_order" );

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					purchase_date_time,
					(char *)0 /* folder_name */,
					"purchase_date_time" );

	sprintf( sys_string,
		 "echo \"	select amount_due		 "
		 "		from %s				 "
		 "		where %s;\"			|"
		 "sql.e						 ",
		 table_name,
		 where );

	results_string = pipe2string( sys_string );

	if ( results_string && *results_string )
	{
		amount_due = atof( results_string );

		local_total_payment = payment_amount + total_payment;

		if ( timlib_double_virtually_same(
				amount_due,
				local_total_payment )
		|| ( local_total_payment < amount_due ) )
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 1;
	}

} /* total_payment_amount_exceeds_amount_due() */

