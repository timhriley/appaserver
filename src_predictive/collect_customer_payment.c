/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/collect_customer_payment.c		*/
/* ----------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

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
boolean total_payment_amount_exceeds_invoice_amount(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			double payment_amount,
			double total_payment_amount );

void insert_customer_payment(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *payment_date_time,
			double payment_amount,
			int check_number );

void display_customer_sale(
			char *application_name,
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
	char *payment_date_time;
	double payment_amount;
	int check_number;
	boolean paid_amount_due;
	DOCUMENT *document;
	char title[ 128 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	double total_payment;
	char sys_string[ 1024 ];

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 9 )
	{
		fprintf( stderr, 
"Usage: %s ignored process full_name street_address sale_date_time payment_amount check_number paid_amount_due_yn\n",
		argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 2 ];
	full_name = argv[ 3 ];
	street_address = argv[ 4 ];
	sale_date_time = argv[ 5 ];
	payment_amount = atof( argv[ 6 ] );
	check_number = atoi( argv[ 7 ] );
	paid_amount_due = ( *argv[ 8 ] == 'y' );

	appaserver_parameter_file = appaserver_parameter_file_new();

	payment_date_time = timlib_get_now_date_time();

	if ( paid_amount_due )
	{
		payment_amount = 
			customer_sale_amount_due(
				application_name,
				full_name,
				street_address,
				sale_date_time );
	}

	total_payment =
		customer_sale_total_payment(
			application_name,
			full_name,
			street_address,
			sale_date_time );

	document_quick_output( application_name );

	format_initial_capital( title, process_name );
	printf( "<h1>%s</h1>\n", title );

	if ( !*sale_date_time
	||   strcmp( sale_date_time, "sale_date_time" ) == 0 )
	{
		printf(
		"<h3>Error: please select a customer sale.</h3>\n" );
		document_tag_close();
		exit( 0 );
	}

	if ( paid_amount_due && !payment_amount )
	{
		printf(
"<h3>Error: the customer sale is already paid in full.</h3>\n" );

		document_tag_close();
		exit( 0 );
	}
	else
	if ( !payment_amount )
	{
		printf(
"<h3>Error: the payment amount is missing. Total existing payments = %.2lf</h3>\n",
			total_payment );

		document_tag_close();
		exit( 0 );
	}

	if ( total_payment_amount_exceeds_invoice_amount(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			payment_amount,
			total_payment ) )
	{
		if ( total_payment )
		{
			printf(
"<h3>Error: the payment amount plus existing payments of %.2lf exceed invoice amount.</h3>\n",
				total_payment );
		}
		else
		{
			printf(
"<h3>Error: the payment amount exceeds invoice amount.</h3>\n" );
		}
		document_tag_close();
		exit( 0 );
	}

	if ( payment_amount )
	{
		insert_customer_payment(
				application_name,
				full_name,
				street_address,
				sale_date_time,
				payment_date_time,
				payment_amount,
				check_number );
	}

	sprintf( sys_string,
"post_change_customer_payment %s \"%s\" \"%s\" \"%s\" \"%s\" insert preupdate_payment_date_time preupdate_payment_amount",
		 application_name,
		 full_name,
		 street_address,
		 sale_date_time,
		 payment_date_time );

	if ( system( sys_string ) ){};

	display_customer_sale(
		application_name,
		full_name,
		street_address,
		sale_date_time );

	printf( "<h3>Payment of $%.2lf stored.</h3>\n", payment_amount );

	document_tag_close();
	return 0;
}

void display_customer_sale(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time )
{
	char sys_string[ 1024 ];
	char *select;
	char *where;
	char *justify;

	select =
"full_name,street_address,sale_date_time,invoice_amount,total_payment";

	where = ledger_get_transaction_where(
				full_name,
				street_address,
				sale_date_time,
				(char *)0 /* folder_name */,
				"sale_date_time" );

	justify = "left,left,left,right,right";

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=%s		 "
		 "			folder=customer_sale	 "
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
}

void insert_customer_payment(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *payment_date_time,
			double payment_amount,
			int check_number )
{
	char sys_string[ 1024 ];
	char *field_list_string;
	FILE *output_pipe;
	char *table_name;

	field_list_string =
"full_name,street_address,sale_date_time,payment_date_time,payment_amount,check_number";

	table_name = get_table_name( application_name, "customer_payment" );

	sprintf( sys_string,
		 "insert_statement.e table=%s field=%s | sql.e",
		 table_name,
		 field_list_string );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
		 "%s|%s|%s|%s|%.2lf",
		 full_name,
		 street_address,
		 sale_date_time,
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
}

boolean total_payment_amount_exceeds_invoice_amount(
			char *application_name,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			double payment_amount,
			double total_payment )
{
	char *table_name;
	char sys_string[ 1024 ];
	char *results_string;
	double invoice_amount;
	double local_total_payment;
	char *where;

	table_name = get_table_name( application_name, "customer_sale" );

	where = ledger_get_transaction_where(
					full_name,
					street_address,
					sale_date_time,
					(char *)0 /* folder_name */,
					"sale_date_time" );

	sprintf( sys_string,
		 "echo \"	select invoice_amount		 "
		 "		from %s				 "
		 "		where %s;\"			|"
		 "sql.e						 ",
		 table_name,
		 where );

	results_string = pipe2string( sys_string );

	if ( results_string && *results_string )
	{
		invoice_amount = atof( results_string );

		local_total_payment = payment_amount + total_payment;

		if ( timlib_double_virtually_same(
				invoice_amount,
				local_total_payment )
		|| ( local_total_payment < invoice_amount ) )
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
}

