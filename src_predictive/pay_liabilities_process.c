/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/pay_liabilities_process.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "document.h"
#include "sql.h"
#include "liability.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *session_key;
	char *fund_name;
	char *cash_account_name;
	char *full_name_list_string;
	char *contact_key_list_string;
	int starting_check_number;
	char *memo;
	double dialog_box_payment_amount;
	boolean execute;
	APPASERVER_PARAMETER *appaserver_parameter;
	LIST *full_name_list;
	LIST *contact_key_list;
	LIABILITY_PAYMENT *liability_payment;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 11 )
	{
		fprintf( stderr,
"Usage: %s process session fund cash_account full_name[^full_name] street_address[^street_address] starting_check_number memo payment_amount execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	session_key = argv[ 2 ];
	fund_name = argv[ 3 ];
	cash_account_name = argv[ 4 ];
	full_name_list_string = argv[ 5 ];
	contact_key_list_string = argv[ 6 ];
	starting_check_number = atoi( argv[ 7 ] );
	memo = argv[ 8 ];
	dialog_box_payment_amount = atof( argv[ 9 ] );
	execute = ( *argv[ 10 ] == 'y' );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name /* title */ );

	full_name_list =
		list_string_to_list(
			full_name_list_string,
			SQL_DELIMITER );

	contact_key_list =
		list_string_to_list(
			contact_key_list_string,
			SQL_DELIMITER );

	if ( ( !list_length( full_name_list ) )
	||   ( list_length( full_name_list ) == 1
	&& 	strcmp(
			/* --------------------------------- */
			/* Returns the first element or null */
			/* --------------------------------- */
			list_first( full_name_list ),
			"full_name" ) == 0 ) )
	{
		printf( "<h3>Please choose an Entity</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( list_length( full_name_list ) !=
	     list_length( contact_key_list ) )
	{
		printf(
"<h3>An internal error occurred. The list lengths don't match: %d vs. %d.</h3>\n",
		list_length( full_name_list ),
		list_length( contact_key_list ) );
		document_close();
		exit( 0 );
	}

	appaserver_parameter = appaserver_parameter_new();

	liability_payment =
		liability_payment_new(
			application_name,
			cash_account_name,
			fund_name,
			dialog_box_payment_amount,
			starting_check_number,
			memo /* dialog_box_memo */,
			appaserver_parameter->data_directory,
			process_name,
			session_key,
			entity_full_contact_list(
				full_name_list,
				contact_key_list ) );

	if ( liability_payment->error_message )
	{
		printf(	"<h3>%s</h3>\n",
			liability_payment->error_message );
		document_close();
		exit( 0 );
	}

	if ( liability_payment->check_list )
	{
		printf( "%s\n",
			liability_payment->
				check_list->
				check_link->
				pdf_anchor_html );

		printf( "<p>\n" );
	}

	if ( execute )
	{
		/* May reset transaction_date_time */
		/* ------------------------------- */
		liability_transaction_list_insert(
			liability_payment->liability_transaction_list );
	}

	liability_transaction_list_html_display(
		liability_payment->liability_transaction_list );

	if ( execute )
	{
		printf( "<h3>Posting complete.</h3>\n" );
	}

	document_close();

	return 0;
}

