/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/make_vendor_payment.c		*/
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
#include "date.h"
#include "document.h"
#include "environ.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "purchase.h"
#include "vendor_payment.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
boolean total_payment_amount_exceeds_amount_due(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			double payment_amount,
			double payment_amount_total );

void display_purchase_order(
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
	char title[ 128 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	double payment_amount_total;
	char sys_string[ 1024 ];

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf( stderr, 
"Usage: %s process full_name street_address purchase_date_time payment_amount check_number paid_amount_due_yn\n",
		argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	purchase_date_time = argv[ 4 ];
	payment_amount = atof( argv[ 5 ] );
	check_number = atoi( argv[ 6 ] );
	paid_amount_due = ( *argv[ 7 ] == 'y' );

	payment_date_time = date_time_now( date_utc_offset() );

	appaserver_parameter_file = appaserver_parameter_file_new();

	document_quick_output( application_name );

	format_initial_capital( title, process_name );
	printf( "<h1>%s</h1>\n", title );

	if ( !*purchase_date_time
	||   strcmp( purchase_date_time, "purchase_date_time" ) == 0 )
	{
		printf(
		"<h3>Error: please select a purchase order.</h3>\n" );
		document_tag_close();
		exit( 0 );
	}

	if ( paid_amount_due )
	{
		payment_amount = 
			purchase_fetch_amount_due(
				full_name,
				street_address,
				purchase_date_time );
	}

	payment_amount_total =
		vendor_payment_total(
			vendor_payment_list_fetch(
				full_name,
				street_address,
				purchase_date_time ) );

	if ( paid_amount_due && !payment_amount )
	{
		printf(
	"<h3>Error: the purchase order is already paid in full.</h3>\n" );

		document_tag_close();
		exit( 0 );
	}
	else
	if ( !payment_amount )
	{
		printf(
"<h3>Error: the payment amount is missing. Total existing payments = %.2lf</h3>\n",
			payment_amount_total );

		document_tag_close();
		exit( 0 );
	}

	if ( total_payment_amount_exceeds_amount_due(
				full_name,
				street_address,
				purchase_date_time,
				payment_amount,
				payment_amount_total ) )
	{
		if ( payment_amount_total )
		{
			printf(
"<h3>Error: the payment amount plus existing payments of %.2lf exceed amount due.</h3>\n",
				payment_amount_total );
		}
		else
		{
			printf(
"<h3>Error: the payment amount exceeds amount due.</h3>\n" );
		}
		document_tag_close();
		exit( 0 );
	}

	if ( payment_amount )
	{
		vendor_payment_insert(
			full_name,
			street_address,
			purchase_date_time,
			payment_date_time,
			payment_amount,
			check_number,
			(char *)0 /* transaction_date_time */ );

		/* Builds the transaction */
		/* ---------------------- */
		sprintf( sys_string,
	"post_change_vendor_payment %s \"%s\" \"%s\" '%s' '%s' insert ''",
		 	application_name,
		 	full_name,
		 	street_address,
		 	purchase_date_time,
		 	payment_date_time );

		if ( system( sys_string ) ){};

		display_purchase_order(	full_name,
					street_address,
					purchase_date_time );

		printf( "<p>Payment stored.\n" );
	}
	else
	{
		printf( "<p>No payment amount.\n" );
	}

	document_tag_close();
	return 0;
}

void display_purchase_order(
			char *full_name,
			char *street_address,
			char *purchase_date_time )
{
	char sys_string[ 1024 ];
	char *justify;
	char *select;

	select =
"full_name,street_address,purchase_date_time,purchase_amount,amount_due";

	justify = "left,left,left,right,right";

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\"	|"
		 "sql							|"
		 "html_table.e '' '%s' '%c' '%s'		 	 ",
		 select,
		 "purchase_order",
		 purchase_primary_where(
			full_name,
			street_address,
			purchase_date_time ),
		 select,
		 select,
		 SQL_DELIMITER,
		 justify );

	fflush( stdout );
	if ( system( sys_string ) ){};
	fflush( stdout );
}

boolean total_payment_amount_exceeds_amount_due(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			double payment_amount,
			double payment_amount_total )
{
	double amount_due;
	double local_payment_amount_total;

	amount_due =
		purchase_fetch_amount_due(
			full_name,
			street_address,
			purchase_date_time );

	local_payment_amount_total = payment_amount + payment_amount_total;

	if ( timlib_double_virtually_same(
			amount_due,
			local_payment_amount_total )
	|| ( local_payment_amount_total < amount_due ) )
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

