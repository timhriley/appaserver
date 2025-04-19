/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/feeder_init_execute.c		*/
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
#include "security.h"
#include "exchange.h"
#include "import_predict.h"
#include "feeder_init.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_name;
	char *full_name;
	char *account_type;
	char *exchange_format_filename;
	boolean execute_boolean;
	boolean checking_boolean;
	boolean okay_continue = 1;
	EXCHANGE *exchange = {0};
	FEEDER_INIT *feeder_init = {0};
	char *error_string;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 9 )
	{
		fprintf(stderr,
"Usage: %s session login_name role process full_name account_type exchange_format_filename execute_yn\n",
			argv[ 0 ] );

		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	process_name = argv[ 4 ];
	full_name = argv[ 5 ];
	account_type = argv[ 6 ];
	exchange_format_filename = argv[ 7 ];
	execute_boolean = (*argv[ 8 ] == 'y');

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name /* title */ );

	if ( !*full_name
	||   strcmp(
		full_name,
		"full_name" ) == 0 )
	{
		printf(	"%s\n",
			FEEDER_INIT_INSTITUTION_MISSING_MESSAGE );

		okay_continue = 0;
	}

	if ( !*account_type
	||   strcmp(
		account_type,
		"account_type" ) == 0 )
	{
		printf(
		"<h3>Please select an account type.</h3>\n" );

		okay_continue = 0;
	}

	if ( !*exchange_format_filename
	||   strcmp(
		exchange_format_filename,
		"exchange_format_filename" ) == 0 )
	{
		printf(
		"<h3>Please transmit an exchange formatted file.</h3>\n" );

		okay_continue = 0;
	}

	if ( okay_continue )
	{
		exchange =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			exchange_parse(
				application_name,
				exchange_format_filename,
				appaserver_parameter_upload_directory() );
	}

	if ( exchange && !exchange->open_tag_boolean )
	{
		printf(
	"<h3>Sorry, but this file is not in open exchange format.</h3>\n" );

		okay_continue = 0;
	}

	if ( exchange && !list_length( exchange->exchange_journal_list ) )
	{
		printf(
"<h3>Sorry, but this exchange formatted file doesn't have any transactions.</h3>\n" );
		okay_continue = 0;
	}

	checking_boolean = ( strcmp( account_type, "checking" ) == 0 );

	if ( okay_continue )
	{
		feeder_init =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			feeder_init_new(
				application_name,
				session_key,
				login_name,
				role_name,
				full_name /* financial_institution_full_name */,
				checking_boolean,
				exchange->exchange_journal_begin_amount,
				exchange->minimum_date_string );
	}

	if ( feeder_init
	&&   feeder_init->
		feeder_init_input->
		institution_missing_boolean )
	{
		printf( "%s\n",
			FEEDER_INIT_INSTITUTION_MISSING_MESSAGE );

		feeder_init = NULL;
	}

	if ( feeder_init
	&&   feeder_init->
		feeder_init_input->
		account_exist_boolean )
	{
		printf( "%s\n",
			FEEDER_INIT_ACCOUNT_EXIST_MESSAGE );

		feeder_init = NULL;
	}

	if ( feeder_init
	&&   !feeder_init->
		feeder_init_input->
		entity_self )
	{
		printf( "%s\n",
			FEEDER_INIT_ENTITY_SELF_MESSAGE );

		feeder_init = NULL;
	}

	if ( feeder_init
	&&   feeder_init->
		feeder_init_passthru->
		exist_boolean )
	{
		printf( "%s\n",
			FEEDER_INIT_PASSTHRU_EXIST_MESSAGE );
	}

	if ( !execute_boolean )
	{
		if ( feeder_init
		&&   feeder_init->
			feeder_init_input->
			date_recent_boolean )
		{
			printf( "%s\n",
				FEEDER_INIT_RECENT_MESSAGE );
		}

		printf( "%s\n",
			FEEDER_INIT_OPENING_MESSAGE );

		if ( feeder_init )
		{
			feeder_init_transaction_html_display(
				feeder_init->feeder_init_checking,
				feeder_init->feeder_init_credit );
		}

		printf( "%s\n",
			IMPORT_PREDICT_SHORTCUT_MESSAGE );
	}

	if ( execute_boolean )
	{
		if ( !feeder_init ) goto feeder_init_display_continue;

		error_string =
			sql_execute(
				SQL_EXECUTABLE,
				feeder_init->
					feeder_init_input->
					appaserver_error_filespecification,
				feeder_init->insert_sql_list,
				(char *)0 /* sql_statement */ );

		if ( error_string )
		{
			appaserver_error_message_file(
				application_name,
				login_name,
				error_string );
		}

		if ( checking_boolean )
		{
			feeder_init->feeder_init_checking =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				feeder_init_checking_new(
					1 /* execute_boolean */,
					exchange->exchange_journal_begin_amount,
					exchange->minimum_date_string,
					feeder_init->
						feeder_init_input->
							account_name,
					feeder_init->
						feeder_init_input->
						entity_self->
						full_name,
					feeder_init->
						feeder_init_input->
						entity_self->
						street_address );
		}
		else
		{
			feeder_init->feeder_init_credit =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				feeder_init_credit_new(
					1 /* execute_boolean */,
					-exchange->exchange_journal_begin_amount
					/* negate_exchange_journal_begin */,
					exchange->minimum_date_string,
					feeder_init->
						feeder_init_input->
						account_name,
					feeder_init->
						feeder_init_input->
						entity_self->
						full_name,
					feeder_init->
						feeder_init_input->
						entity_self->
						street_address );
		}

		if ( feeder_init->feeder_init_checking )
		{
			feeder_init_transaction_insert(
				feeder_init->
					feeder_init_checking->
					feeder_init_transaction );
		}
		else
		{
			feeder_init_transaction_insert(
				feeder_init->
					feeder_init_credit->
					feeder_init_transaction );
		}

feeder_init_display_continue:

		printf( "%s\n",
			FEEDER_INIT_OPENING_MESSAGE );

		if ( feeder_init )
		{
			feeder_init_transaction_html_display(
				feeder_init->feeder_init_checking,
				feeder_init->feeder_init_credit );
		}

		printf( "%s\n",
			FEEDER_INIT_TRIAL_BALANCE_MESSAGE );

		if ( feeder_init )
		{
			security_system(
				SECURITY_FORK_CHARACTER,
				SECURITY_FORK_STRING,
				feeder_init->trial_balance_system_string );
		}

		printf( "%s\n",
			FEEDER_INIT_ACTIVITY_MESSAGE );

		if ( feeder_init )
		{
			security_system(
				SECURITY_FORK_CHARACTER,
				SECURITY_FORK_STRING,
				feeder_init->activity_system_string );
		}

		printf( "%s\n",
			FEEDER_INIT_POSITION_MESSAGE );

		if ( feeder_init )
		{
			security_system(
				SECURITY_FORK_CHARACTER,
				SECURITY_FORK_STRING,
				feeder_init->position_system_string );
		}

		printf( "%s\n",
			FEEDER_INIT_DEBIT_CREDIT_MESSAGE );

		printf( "%s\n",
			FEEDER_INIT_UPLOAD_MESSAGE );
	}

	printf( "%s\n",
		FEEDER_INIT_MESSAGES_AVAILABLE_MESSAGE );

	document_close();

	return 0;
}

