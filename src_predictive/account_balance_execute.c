/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/account_balance_execute.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "document.h"
#include "sql.h"
#include "entity.h"
#include "investment_account.h"
#include "account_balance.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *fund_name;
	char *as_of_date;
	char *financial_institution_full_name;
	char *financial_institution_contact_key;
	char *investment_purpose;
	boolean execute_boolean;
	ACCOUNT_BALANCE_PROCESS *account_balance_process;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 8 )
	{
		fprintf(stderr,
"Usage: %s process fund as_of_date full_name contact_key investment_purpose execute_yn\n",
			argv[ 0 ] );

		exit ( 1 );
	}

	process_name = argv[ 1 ];
	fund_name = argv[ 2 ];
	as_of_date = argv[ 3 ];
	financial_institution_full_name = argv[ 4 ];
	financial_institution_contact_key = argv[ 5 ];
	investment_purpose = argv[ 6 ];
	execute_boolean = (*argv[ 7 ] == 'y');

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name /* title */ );

	account_balance_process =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		account_balance_process_new(
			fund_name,
			financial_institution_full_name,
			financial_institution_contact_key,
			as_of_date,
			investment_purpose );


	investment_account_list_html_output(
		SQL_DELIMITER,
		INVESTMENT_ACCOUNT_HEADING,
		INVESTMENT_ACCOUNT_JUSTIFICATION,
		account_balance_process->
			investment_account_list,
		account_balance_process->
			investment_account_asset_sum );

	if ( account_balance_process->investment_transaction_boolean )
	{
		investment_transaction_output(
			INVESTMENT_TRANSACTION_ASSET_MEMO,
			execute_boolean,
			account_balance_process->
				investment_transaction_asset );

		investment_transaction_output(
			INVESTMENT_TRANSACTION_LIABILITY_MEMO,
			execute_boolean,
			account_balance_process->
				investment_transaction_liability );
	}

	document_close();

	return 0;
}

