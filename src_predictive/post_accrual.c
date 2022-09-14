/* ----------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/post_accrual.c	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "boolean.h"
#include "list.h"
#include "timlib.h"
#include "environ.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "transaction.h"
#include "journal.h"
#include "application.h"
#include "folder_menu.h"
#include "appaserver_parameter_file.h"
#include "accrual.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *role_name;
	char *process_name;
	char *full_name;
	char *street_address;
	char *accrual_description;
	boolean execute;
	boolean with_html;
	char title[ 128 ];
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	ACCRUAL *accrual = {0};
	LIST *accrual_list = {0};

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 9 )
	{
		fprintf( stderr,
"Usage: %s session role process full_name street_address accrual_description execute_yn with_html_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv[ 1 ];
	role_name = argv[ 2 ];
	process_name = argv[ 3 ];
	full_name = argv[ 4 ];
	street_address = argv[ 5 ];
	accrual_description = argv[ 6 ];
	execute = (*argv[ 7 ] == 'y');
	with_html = (*argv[ 8 ] == 'y');

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( with_html )
	{
		document_quick_output_body(
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point );

		printf( "<h1>%s</h1>\n", 
			format_initial_capital(
				title,
				process_name ) );
		fflush( stdout );
	}

	if ( !isdigit( *session_key )
	||   !*full_name
	||   strcmp( full_name, "full_name" ) == 0 )
	{
		accrual_list =
			accrual_list_fetch(
				application_name );
	}
	else
	{
		accrual =
			accrual_fetch(
				application_name,
				full_name,
				street_address,
				accrual_description );
	}

	if ( !list_length( accrual_list )
	&&   !accrual
	&&   with_html )
	{
		printf(
		"<h3>No accrual transactions to process.</h3>\n" );
	}
	else
	if ( !execute && with_html )
	{
		if ( list_length( accrual_list ) )
		{
			transaction_list_html_display(
				accrual_transaction_list_extract(
					accrual_list ) );
		}
		else
		if ( accrual )
		{
			transaction_html_display(
				accrual->transaction );
		}
	}
	else
	if ( execute )
	{
		char *transaction_date_time;

		if ( list_length( accrual_list ) )
		{
			transaction_list_insert(
				accrual_transaction_list_extract(
					accrual_list ),
				1 /* insert_journal_list_boolean */ );
		}
		else
		if ( accrual )
		{
			transaction_date_time =
				transaction_stamp_insert(
					accrual->transaction,
					1 /* insert_journal_list_boolean */ );
		}

		folder_menu_refresh_row_count(
			application_name,
			TRANSACTION_TABLE,
			session_key,
			appaserver_parameter_file->
				appaserver_data_directory,
			role_name );

		folder_menu_refresh_row_count(
			application_name,
			JOURNAL_TABLE,
			session_key,
			appaserver_parameter_file->
				appaserver_data_directory,
			role_name );

		if ( with_html )
		{
			printf( "<h3>Process complete:</h3>\n" );

			if ( transaction_date_time )
			{
				printf( "<h3>%s</h3>\n",
					transaction_date_time );
			}
		}
	}

	return 0;
}
