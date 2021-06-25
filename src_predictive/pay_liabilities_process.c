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
#include "liability.h"
#include "account.h"
#include "journal.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define PROMPT				"Press here to view check."

/* Prototypes */
/* ---------- */

void print_checks_transaction_display(
			LIST *liability_transaction_list );

void print_checks_post(
			LIST *liability_transaction_list );

char *print_checks_create(
			char *application_name,
			LIST *liability_entity_list,
			char *memo,
			int starting_check_number,
			char *document_root_directory,
			char *process_name,
			char *session,
			char personal_size_yn );

char *pay_liabilities_process(
			char **error_message,
			char *application_name,
			LIST *full_name_list,
			LIST *street_address_list,
			int starting_check_number,
			char *memo,
			double dialog_box_payment_amount,
			boolean execute,
			char *document_root_directory,
			char *process_name,
			char *session,
			char personal_size_yn );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *session;
	char *full_name_list_string;
	char *street_address_list_string;
	int starting_check_number;
	char *memo;
	double dialog_box_payment_amount;
	char personal_size_yn;
	boolean execute;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char title[ 128 ];
	LIST *full_name_list;
	LIST *street_address_list;
	char *pdf_filename;
	char *error_message;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 10 )
	{
		fprintf( stderr,
"Usage: %s process session full_name[,full_name] street_address[,street_address] starting_check_number memo payment_amount personal_size_yn execute_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	session = argv[ 2 ];
	full_name_list_string = argv[ 3 ];
	street_address_list_string = argv[ 4 ];
	starting_check_number = atoi( argv[ 5 ] );
	memo = argv[ 6 ];
	dialog_box_payment_amount = atof( argv[ 7 ] );
	personal_size_yn = *argv[ 8 ];
	execute = ( *argv[ 9 ] == 'y' );

	appaserver_parameter_file = appaserver_parameter_file_new();

	format_initial_capital( title, process_name ),

	document_quick_output_body(
		application_name,
		appaserver_parameter_file->
			appaserver_mount_point );

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

	error_message = (char *)0;

	pdf_filename =
		pay_liabilities_process(
			&error_message,
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
			personal_size_yn );

	if ( !pdf_filename )
	{
		if ( error_message )
		{
			printf( "<h3>%s</h3>\n", error_message );
		}
		else
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

char *pay_liabilities_process(
			char **error_message,
			char *application_name,
			LIST *full_name_list,
			LIST *street_address_list,
			int starting_check_number,
			char *memo,
			double dialog_box_payment_amount,
			boolean execute,
			char *document_root_directory,
			char *process_name,
			char *session,
			char personal_size_yn )
{
	char *pdf_filename = {0};
	LIABILITY *liability;

	if ( list_length( full_name_list ) > 1
	&&   dialog_box_payment_amount )
	{
		*error_message = "Set payment amount only for one entity.";
		return (char *)0;
	}

	liability =
		liability_new(
			dialog_box_payment_amount,
			starting_check_number,
			/* -------------------------------------- */
			/* Sets account_balance_zero_journal_list */
			/* -------------------------------------- */
			liability_balance_zero_account_list(),
			liability_account_entity_list(),
			entity_full_street_list(
				full_name_list,
				street_address_list )
				/* input_entity_list */ );

	liability->liability_tax_redirect_account_list =
		liability_tax_redirect_account_list(
			liability->liability_balance_zero_account_list,
			liability->liability_account_entity_list );

	liability->liability_entity_list =
		liability_entity_list(
			liability->
				liability_tax_redirect_account_list
				/* liability_account_list */,
			liability->input_entity_list,
			liability->dialog_box_payment_amount );

	liability->liability_balance_zero_entity_list =
		/* ------------------------------------- */
		/* Sets entity_balance_zero_account_list */
		/* ------------------------------------- */
		liability_balance_zero_entity_list(
			liability->liability_entity_list,
			liability->
				liability_tax_redirect_account_list
				/* liability_account_list */ );

	liability->liability_steady_state_entity_list =
		liability_steady_state_entity_list(
			liability->
				liability_balance_zero_entity_list );

	if ( starting_check_number )
	{
		if ( ! ( pdf_filename =
			   print_checks_create(
				application_name,
				liability->liability_steady_state_entity_list,
				memo,
				starting_check_number,
				document_root_directory,
				process_name,
				session,
				personal_size_yn ) ) )
		{
			return (char *)0;
		}
	}
	else
	{
		pdf_filename = "";
	}

	if ( execute )
	{
		print_checks_post(
			liability_transaction_list(
				liability->liability_steady_state_entity_list,
				liability_credit_account_name(
					starting_check_number ),
				starting_check_number ) );
	}
	else
	{
		print_checks_transaction_display(
			liability_transaction_list(
				liability->liability_steady_state_entity_list,
				liability_credit_account_name(
					starting_check_number ),
				starting_check_number ) );
	}

	return pdf_filename;
}

char *print_checks_create(
			char *application_name,
			LIST *liability_entity_list,
			char *memo,
			int starting_check_number,
			char *document_root_directory,
			char *process_name,
			char *session,
			char personal_size_yn )
{
	ENTITY *entity;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *output_filename;
	char *pdf_filename;
	char *document_root_filename;
	char *ftp_filename;
	APPASERVER_LINK_FILE *appaserver_link_file;

	if ( !list_length( liability_entity_list ) )
	{
		return (char *)0;
	}

	appaserver_link_file =
		appaserver_link_file_new(
			application_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_prepend_http_protocol_yn(
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

	list_rewind( liability_entity_list );

	do {
		entity = list_get( liability_entity_list );

		if ( !entity->entity_liability_payment_amount ) continue;

		fprintf( output_pipe,
			 "%s^%.2lf^%s^%d\n",
			 entity->full_name,
			 entity->entity_liability_payment_amount,
			 (*memo) ? memo : "",
			 starting_check_number++ );

	} while( list_next( liability_entity_list ) );

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
			"check_page" /* target */,
			(char *)0 /* application_type */ );

	printf( "<p>\n" );

	return pdf_filename;
}

void print_checks_post( LIST *liability_transaction_list )
{
	transaction_list_insert(
		liability_transaction_list,
		0 /* not lock_transaction */ );

	printf( "<h3>Post to transaction complete.</h3>\n" );
}

void print_checks_transaction_display(
			LIST *liability_transaction_list )
{
	TRANSACTION *transaction;

	if ( !list_rewind( liability_transaction_list ) )
		return;

	do {
		transaction =
			list_get(
				liability_transaction_list );

		journal_list_html_display(
			transaction->journal_list,
			(char *)0 /* transaction_memo */ );

	} while( list_next( liability_transaction_list ) );
}

