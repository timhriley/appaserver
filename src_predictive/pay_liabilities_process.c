/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/pay_liabilities_process.c		*/
/* ---------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "date.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "latex.h"
#include "pay_liabilities.h"
#include "vendor_payment.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define PROMPT				"Press here to view check."

/* Prototypes */
/* ---------- */
void print_checks_vendor_payment_insert(
			LIST *vendor_payment_list );

void output_vendor_payment(
			LIST *vendor_payment_list );

void print_checks_transaction_display(
			LIST *full_name_list,
			LIST *street_address_list,
			int starting_check_number,
			double dialog_box_payment_amount,
			char *fund_name,
			char *memo );

boolean output_html_table(
			LIST *full_name_list,
			LIST *street_address_list,
			double dialog_box_payment_amount,
			char *fund_name );

double print_checks_balance(
			char *fund_name,
			char *full_name,
			char *street_address );

void print_checks_post(
			LIST *full_name_list,
			LIST *street_address_list,
			int starting_check_number,
			double dialog_box_payment_amount,
			char *fund_name,
			char *memo );

char *print_checks_create(
			char *application_name,
			LIST *full_name_list,
			LIST *street_address_list,
			char *memo,
			int starting_check_number,
			double dialog_box_payment_amount,
			char *document_root_directory,
			char *process_name,
			char *session,
			char *fund_name,
			char personal_size_yn );

char *pay_liabilities(	char *application_name,
			LIST *full_name_list,
			LIST *street_address_list,
			int starting_check_number,
			char *memo,
			double dialog_box_payment_amount,
			boolean execute,
			char *document_root_directory,
			char *process_name,
			char *session,
			char *fund_name,
			char personal_size_yn );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *session;
	char *fund_name;
	char *full_name_list_string;
	char *street_address_list_string;
	int starting_check_number;
	char *memo;
	double dialog_box_payment_amount;
	char personal_size_yn;
	boolean execute;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DOCUMENT *document;
	char title[ 128 ];
	LIST *full_name_list;
	LIST *street_address_list;
	char *pdf_filename;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 11 )
	{
		fprintf( stderr,
"Usage: %s process session fund full_name[,full_name] street_address[,street_address] starting_check_number memo payment_amount personal_size_yn execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	session = argv[ 2 ];
	fund_name = argv[ 3 ];
	full_name_list_string = argv[ 4 ];
	street_address_list_string = argv[ 5 ];
	starting_check_number = atoi( argv[ 6 ] );
	memo = argv[ 7 ];
	dialog_box_payment_amount = atof( argv[ 8 ] );
	personal_size_yn = *argv[ 9 ];
	execute = ( *argv[ 10 ] == 'y' );

	appaserver_parameter_file = appaserver_parameter_file_new();

	format_initial_capital( title, process_name ),

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

	printf( "<h1>%s</h1>\n", title );
	fflush( stdout );

	full_name_list =
		list_string_to_list(
			full_name_list_string,
			',' );

	street_address_list =
		list_string_to_list(
			street_address_list_string,
			',' );

	if ( ( !list_length( full_name_list ) )
	||   ( list_length( full_name_list ) == 1
	&& 	strcmp(
			list_get_first_pointer( full_name_list ),
			"full_name" ) == 0 ) )
	{
		printf( "<h3>Please choose an Entity</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( list_length( full_name_list ) !=
	     list_length( street_address_list ) )
	{
		printf(
"<h3>An internal error occurred. The list lengths don't match: %d vs. %d.</h3>\n",
		list_length( full_name_list ),
		list_length( street_address_list ) );
		document_close();
		exit( 0 );
	}

	pdf_filename =
		pay_liabilities(
			application_name,
			full_name_list,
			street_address_list,
			starting_check_number,
			memo,
			dialog_box_payment_amount,
			execute,
			appaserver_parameter_file->document_root,
			process_name,
			session,
			fund_name,
			personal_size_yn );

	if ( !pdf_filename )
	{
		if ( list_length( full_name_list ) > 1 )
		{
			printf(
"<h3>Error: one or more of these liabilities has already been posted.</h3>\n" );
		}
		else
		{
			printf(
		"<h3>Error: liability has already been posted.</h3>\n" );
		}
	}

	document_close();

	return 0;
}

char *pay_liabilities(	char *application_name,
			LIST *full_name_list,
			LIST *street_address_list,
			int starting_check_number,
			char *memo,
			double dialog_box_payment_amount,
			boolean execute,
			char *document_root_directory,
			char *process_name,
			char *session,
			char *fund_name,
			char personal_size_yn )
{
	char *pdf_filename = {0};

	if ( starting_check_number )
	{
		pdf_filename =
			print_checks_create(
				application_name,
				full_name_list,
				street_address_list,
				memo,
				starting_check_number,
				dialog_box_payment_amount,
				document_root_directory,
				process_name,
				session,
				fund_name,
				personal_size_yn );
	}
	else
	{
		pdf_filename = "";
	}

	if ( execute && pdf_filename )
	{
		print_checks_post(
			full_name_list,
			street_address_list,
			starting_check_number,
			dialog_box_payment_amount,
			fund_name,
			memo );
	}

	if ( !execute )
	{
		print_checks_transaction_display(
			full_name_list,
			street_address_list,
			starting_check_number,
			dialog_box_payment_amount,
			fund_name,
			memo );
	}

	return pdf_filename;
}

char *print_checks_create(
			char *application_name,
			LIST *full_name_list,
			LIST *street_address_list,
			char *memo,
			int starting_check_number,
			double dialog_box_payment_amount,
			char *document_root_directory,
			char *process_name,
			char *session,
			char *fund_name,
			char personal_size_yn )
{
	char *full_name;
	char *street_address;
	double balance;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *output_filename;
	char *pdf_filename;
	char *document_root_filename;
	char *ftp_filename;
	APPASERVER_LINK_FILE *appaserver_link_file;

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
			document_root_directory,
			process_name /* filename_stem */,
			application_name,
			getpid(),
			(char *)0 /* session */,
			"dat" );

	/* make_checks.e places the pdf_filename into output_filename. */
	/* ----------------------------------------------------------- */
	output_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	sprintf( sys_string,
		 "make_checks.e stdin %c > %s",
		 personal_size_yn,
		 output_filename );

/*
	sprintf( sys_string,
		 "make_checks.e stdin %c 2>/dev/null > %s",
		 personal_size_yn,
		 output_filename );
*/

	output_pipe = popen( sys_string, "w" );

	if ( !list_rewind( full_name_list ) )
	{
		fprintf(stderr,
			"Error in %s/%s()/%d: empty full_name_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list_rewind( street_address_list );

	do {
		full_name = list_get_pointer( full_name_list );
		street_address = list_get_pointer( street_address_list );

		/* If no balance, then user pressed <Submit> twice. */
		/* ------------------------------------------------ */
		if ( ! ( balance =
				print_checks_balance(
					fund_name,
					full_name,
					street_address ) ) )
		{
			pclose( output_pipe );
			return (char *)0;
		}

		fprintf( output_pipe,
			 "%s^%.2lf^%s^%d\n",
			 full_name,
			 (dialog_box_payment_amount)
				? dialog_box_payment_amount
				: balance,
			 (*memo) ? memo : "",
			 starting_check_number++ );

		list_next( street_address_list );

	} while( list_next( full_name_list ) );

	pclose( output_pipe );

	sprintf( sys_string,
		 "cat %s",
		 output_filename );

	pdf_filename = pipe2string( sys_string );

	appaserver_link_file->extension = "pdf";
	appaserver_link_file->session = session;
	appaserver_link_file->process_id = 0;

	/* Copy pdf_filename to document_root_directory. */
	/* --------------------------------------------- */
	document_root_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	sprintf(	sys_string,
			"cp %s %s",
			pdf_filename,
			document_root_filename );

	if ( system( sys_string ) ){};

	ftp_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	appaserver_library_output_ftp_prompt(
			ftp_filename,
			PROMPT,
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	return pdf_filename;
}

void print_checks_post(
			LIST *full_name_list,
			LIST *street_address_list,
			int starting_check_number,
			double dialog_box_payment_amount,
			char *fund_name,
			char *memo )
{
	PAY_LIABILITIES *pay_liabilities;

	if ( ! ( pay_liabilities =
		pay_liabilities_new(
			fund_name,
			full_name_list,
			street_address_list,
			starting_check_number,
			dialog_box_payment_amount,
			/* --------------------------------------------- */
			/* Returns memo, program memory, or heap memory. */
			/* --------------------------------------------- */
			pay_liabilities_transaction_memo(
				fund_name,
				memo,
				starting_check_number ) ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: pay_liabilities_new() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_length( pay_liabilities->output.transaction_list ) )
	{
		printf( "<h3>Nothing to post.</h3>\n" );
		return;
	}

	pay_liabilities->output.transaction_list =
		transaction_list_insert(
			pay_liabilities->output.transaction_list );

	if ( list_length( pay_liabilities->output.vendor_payment_list ) )
	{
		print_checks_vendor_payment_insert(
			pay_liabilities->output.vendor_payment_list );
	}

	printf( "<h3>Execute Posting to Journal Ledger complete.</h3>\n" );
}

double print_checks_balance(
			char *fund_name,
			char *full_name,
			char *street_address )
{
	char sys_string[ 128 ];
	char *results;
	char input_full_name[ 128 ];
	char input_street_address_balance[ 128 ];
	char input_street_address[ 128 ];
	char input_balance[ 128 ];
	static LIST *entity_list = {0};

	if ( !entity_list )
	{
		sprintf( sys_string,
		 	"populate_print_checks_entity %s '%s'",
		 	environment_application_name(),
			fund_name );

		entity_list = pipe2list( sys_string );
	}

	if ( !list_rewind( entity_list ) ) return 0.0;

	do {
		results = list_get( entity_list );

		if ( character_count(
			SQL_DELIMITER,
			results ) != 1 )
		{
			fprintf( stderr,
		"Warning in %s/%s()/%d: expecting one delimiter in (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 results );

			return 0.0;
		}

		piece(	input_full_name,
			FOLDER_DATA_DELIMITER,
			results,
			0 );

		piece(	input_street_address_balance,
			FOLDER_DATA_DELIMITER,
			results,
			1 );

		piece(	input_street_address,
			'[',
			input_street_address_balance,
			0 );

		piece( input_balance, '[', input_street_address_balance, 1 );

		if ( strcmp( input_full_name, full_name ) == 0
		&&   strcmp( input_street_address, street_address ) == 0 )
		{
			return atof( input_balance );
		}

	} while( list_next( entity_list ) );

	return 0.0;
}

boolean output_html_table(
			LIST *full_name_list,
			LIST *street_address_list,
			double dialog_box_payment_amount,
			char *fund_name )
{
	char *full_name;
	char *street_address;
	double balance;
	char sys_string[ 1024 ];
	FILE *output_pipe;

	if ( !list_length( full_name_list )
	|| ( list_length( full_name_list ) !=
	     list_length( street_address_list ) ) )
	{
		fprintf( stderr,
			 "Warning in %s/%s()/%d: list lengths do not match.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return 0;
	}

	strcpy( sys_string,
"html_table.e '' 'full_name,street_address,payment' '^' 'left,left,right'" );

	output_pipe = popen( sys_string, "w" );

	list_rewind( full_name_list );
	list_rewind( street_address_list );

	do {
		full_name = list_get( full_name_list );
		street_address = list_get( street_address_list );

		/* If no balance, then user pressed <Submit> twice. */
		/* ------------------------------------------------ */
		if ( ! ( balance =
				print_checks_balance(
					fund_name,
					full_name,
					street_address ) ) )
		{
			pclose( output_pipe );
			return 0;
		}

		fprintf( output_pipe,
			 "%s^%s^%.2lf\n",
			 full_name,
			 street_address,
			 (dialog_box_payment_amount)
				? dialog_box_payment_amount
				: balance );

		list_next( street_address_list );

	} while( list_next( full_name_list ) );

	pclose( output_pipe );
	return 1;
}

void print_checks_transaction_display(
			LIST *full_name_list,
			LIST *street_address_list,
			int starting_check_number,
			double dialog_box_payment_amount,
			char *fund_name,
			char *memo )
{
	PAY_LIABILITIES *pay_liabilities;
	TRANSACTION *transaction;
	char transaction_memo[ 256 ];

	if ( ! ( pay_liabilities =
		pay_liabilities_new(
			fund_name,
			full_name_list,
			street_address_list,
			starting_check_number,
			dialog_box_payment_amount,
			/* --------------------------------------------- */
			/* Returns memo, program memory, or heap memory. */
			/* --------------------------------------------- */
			pay_liabilities_transaction_memo(
				fund_name,
				memo,
				starting_check_number ) ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: pay_liabilities_new() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( pay_liabilities->output.transaction_list ) )
		return;

	do {
		transaction =
			list_get(
				pay_liabilities->output.transaction_list );

		sprintf(transaction_memo,
			"%s/%s: %s",
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time );

		if ( transaction->check_number )
		{
			sprintf( transaction_memo +
				 strlen( transaction_memo ),
				 " (%d)", transaction->check_number );
		}

		format_initial_capital(
			transaction_memo,
			transaction_memo );

		journal_list_html_display(
			transaction->journal_list,
			transaction_memo );

	} while( list_next( pay_liabilities->output.transaction_list ) );

	if ( list_length( pay_liabilities->output.vendor_payment_list ) )
	{
		output_vendor_payment(
			pay_liabilities->output.vendor_payment_list );
	}
}

void output_vendor_payment(
			LIST *vendor_payment_list )
{
	VENDOR_PAYMENT *vendor_payment;
	char sys_string[ 1024 ];
	FILE *output_pipe;

	if ( !list_rewind( vendor_payment_list ) ) return;

	strcpy( sys_string,
"html_table.e '^^Vendor Payment' 'full_name,street_address,purchase_order,payment' '^' 'left,left,left,right'" );

	output_pipe = popen( sys_string, "w" );

	do {
		vendor_payment = list_get_pointer( vendor_payment_list );

		fprintf( output_pipe,
			 "%s^%s^%s^%.2lf\n",
			 vendor_payment->vendor_entity->full_name,
			 vendor_payment->vendor_entity->street_address,
			 vendor_payment->purchase_date_time,
			 vendor_payment->payment_amount );

	} while( list_next( vendor_payment_list ) );

	pclose( output_pipe );
}

void print_checks_vendor_payment_insert(
			LIST *vendor_payment_list )
{
	PURCHASE *purchase;
	VENDOR_PAYMENT *vendor_payment;

	if ( !list_rewind( vendor_payment_list ) ) return;

	do {
		vendor_payment = list_get( vendor_payment_list );

		if ( !vendor_payment->vendor_payment_transaction )
		{
			fprintf( stderr,
				 "Warning in %s/%s()/%d: empty transaction.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			continue;
		}

		if ( !vendor_payment->purchase )
		{
			fprintf( stderr,
				 "Warning in %s/%s()/%d: empty purchase.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			continue;
		}

		vendor_payment_insert(
			vendor_payment->
				vendor_entity->
				full_name,
			vendor_payment->
				vendor_entity->
				street_address,
			vendor_payment->purchase_date_time,
			vendor_payment->payment_date_time,
			vendor_payment->payment_amount,
			vendor_payment->check_number,
			vendor_payment->
				vendor_payment_transaction->
				transaction_date_time );

		purchase = vendor_payment->purchase;

		/* Update amount_due */
		/* ----------------- */
		purchase->purchase_amount_due =
			Purchase_amount_due(
				purchase->purchase_invoice_amount,
				( purchase->purchase_vendor_payment_total =
					vendor_payment_total(
					   purchase->
					     purchase_vendor_payment_list ) ) );

		purchase_update(
			purchase->purchase_equipment_total,
			purchase->purchase_invoice_amount,
			purchase->purchase_vendor_payment_total,
			purchase->purchase_amount_due,
			purchase->purchase_transaction->transaction_date_time,
			purchase->vendor_entity->full_name,
			purchase->vendor_entity->street_address,
			purchase->purchase_date_time );


	} while( list_next( vendor_payment_list ) );
}

