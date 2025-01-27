/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/import_predictbooks.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "document.h"
#include "import_predict.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_name;
	char *full_name;
	char *name_of_bank;
	char *checking_begin_date;
	double checking_begin_balance;
	boolean execute_boolean;
	IMPORT_PREDICT *import_predict;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 10 )
	{
		fprintf(stderr,
"Usage: %s session login_name role process full_name name_of_bank checking_begin_date checking_begin_balance execute_yn\n",
			argv[ 0 ] );

		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	process_name = argv[ 4 ];
	full_name = argv[ 5 ];
	name_of_bank = argv[ 6 ];
	checking_begin_date = argv[ 7 ];
	checking_begin_balance = atof( argv[ 8 ] );
	execute_boolean = (*argv[ 9 ] == 'y');

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name /* title */ );

	import_predict =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		import_predict_new(
			IMPORT_PREDICT_SQLGZ,
			application_name,
			session_key,
			login_name,
			role_name,
			full_name,
			name_of_bank,
			checking_begin_date,
			checking_begin_balance,
			appaserver_parameter_mount_point() );

	if ( import_predict->template_boolean )
	{
		printf(	"%s\n",
			IMPORT_PREDICT_TEMPLATE_MESSAGE );
	}

	if ( import_predict->exists_boolean
	&&   !execute_boolean )
	{
		printf( "%s\n",
			IMPORT_PREDICT_EXISTS_MESSAGE );
	}

	if ( import_predict->begin_date_missing_boolean )
	{
		printf( "%s\n",
			IMPORT_PREDICT_DATE_MISSING_MESSAGE );
		document_close();
		exit( 0 );
	}

	if ( import_predict->balance_zero_boolean )
	{
		printf( "%s\n",
			IMPORT_PREDICT_BALANCE_ZERO_MESSAGE );
		document_close();
		exit( 0 );
	}

	if ( import_predict->bank_missing_boolean )
	{
		printf( "%s\n",
			IMPORT_PREDICT_BANK_MISSING_MESSAGE );
		document_close();
		exit( 0 );
	}

	if ( !import_predict->entity_self )
	{
		printf( "%s\n",
			IMPORT_PREDICT_ENTITY_SELF_MESSAGE );
		document_close();
		exit( 0 );
	}

	if ( execute_boolean )
	{
		if ( system( import_predict->system_string ) ){}

		if ( system(
			import_predict->
				delete_role_system_string ) ){}

		if ( system(
			import_predict->
				import_predict_process->
				insert_system_string ) ){}

		if ( import_predict->
			entity_system_string )
		{
			if ( system(
				import_predict->
					entity_system_string ) ){}
		}

		import_predict->
			import_predict_transaction->
			transaction->
			journal_list =
			import_predict_transaction_journal_list(
				IMPORT_PREDICT_CASH_ACCOUNT,
				IMPORT_PREDICT_EQUITY_ACCOUNT,
				checking_begin_balance,
				1 /* fetch_account_boolean */ );

		(void)transaction_insert(
			import_predict->
				import_predict_transaction->
				transaction->
				full_name,
			import_predict->
				import_predict_transaction->
				transaction->
				street_address,
			import_predict->
				import_predict_transaction->
				transaction->
				transaction_date_time,
			import_predict->
				import_predict_transaction->
				transaction->
				transaction_amount,
			0 /* check_number */,
			import_predict->
				import_predict_transaction->
				transaction->
				memo,
			'n' /* lock_transaction_yn */,
			import_predict->
				import_predict_transaction->
				transaction->
				journal_list,
			1 /* insert_journal_list_boolean */ );

		transaction_html_display(
			import_predict->
				import_predict_transaction->
				transaction );

		printf( "%s\n",
			IMPORT_PREDICT_WARNING_MESSAGE );

		if ( !import_predict->exists_boolean )
		{
			if ( import_predict->menu_anchor_tag )
			{
				printf(	"%s\n",
					IMPORT_PREDICT_REFRESH_MESSAGE );

				printf(	"%s\n",
					import_predict->menu_anchor_tag );
			}
			else
			{
				printf( "%s\n",
					IMPORT_PREDICT_LOGIN_AGAIN_MESSAGE );
			}
		}

		printf( "<h3>Reminder:</h3>\n" );

		printf( "%s\n",
			IMPORT_PREDICT_SHORTCUT_MESSAGE );

		printf( "%s\n",
			IMPORT_PREDICT_TRIAL_BALANCE_MESSAGE );

		fflush( stdout );
		if ( system( import_predict->trial_balance_system_string ) ){}
		fflush( stdout );

		printf( "%s\n",
			IMPORT_PREDICT_FINANCIAL_POSITION_MESSAGE );

		fflush( stdout );
		if ( system(
			import_predict->
				financial_position_system_string ) ){}
		fflush( stdout );

		printf( "%s\n",
			IMPORT_PREDICT_NEXT_STEP_MESSAGE );

		printf( "<h3>Process complete</h3>\n" );
	}
	else
	{
		printf( "%s\n",
			IMPORT_PREDICT_OPENING_MESSAGE );

		transaction_html_display(
			import_predict->
				import_predict_transaction->
				transaction );

		printf( "%s\n",
			IMPORT_PREDICT_SHORTCUT_MESSAGE );
	}

	document_close();

	return 0;
}

