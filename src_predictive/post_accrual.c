/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/post_accrual.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "boolean.h"
#include "list.h"
#include "timlib.h"
#include "environ.h"
#include "appaserver_error.h"
#include "document.h"
#include "transaction.h"
#include "journal.h"
#include "application.h"
#include "accrual.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *process_name;
	char *full_name;
	char *street_address;
	char *accrual_description;
	boolean execute;
	boolean with_html;
	ACCRUAL *accrual = {0};
	LIST *accrual_list = {0};

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s session process full_name street_address accrual_description execute_yn with_html_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv[ 1 ];
	process_name = argv[ 2 ];
	full_name = argv[ 3 ];
	street_address = argv[ 4 ];
	accrual_description = argv[ 5 ];
	execute = (*argv[ 6 ] == 'y');
	with_html = (*argv[ 7 ] == 'y');

	if ( with_html )
	{
		document_process_output(
			application_name,
			(LIST *)0 /* javascript_filename_list */,
			process_name /* title */ );
	}

	if ( !isdigit( *session_key )
	||   !*full_name
	||   strcmp( full_name, "full_name" ) == 0 )
	{
		accrual_list = accrual_list_fetch();
	}
	else
	{
		accrual =
			accrual_fetch(
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
					accrual->transaction /* in/out */,
					1 /* insert_journal_list_boolean */ );
		}

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
