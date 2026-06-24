/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/insert_expense_transaction_execute.c	*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "document.h"
#include "entity.h"
#include "insert_expense.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *fund_name;
	char *feeder_account;
	char *full_name;
	char *contact_key;
	char *new_full_name;
	char *account;
	char *transaction_date;
	double transaction_amount;
	int check_number;
	char *memo;
	INSERT_EXPENSE *insert_expense;
	char *error_message = {0};

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 12 )
	{
		fprintf(stderr,
"Usage: %s process fund feeder_account full_name contact_key new_full_name account transaction_date transaction_amount check_number memo\n",
			argv[ 0 ] );

		exit ( 1 );
	}

	process_name = argv[ 1 ];
	fund_name = argv[ 2 ];
	feeder_account = argv[ 3 ];
	full_name = argv[ 4 ];
	contact_key = argv[ 5 ];
	new_full_name = argv[ 6 ];
	account = argv[ 7 ];
	transaction_date = argv[ 8 ];
	transaction_amount = atof( argv[ 9 ] );
	check_number = atoi( argv[ 10 ] );
	memo = argv[ 11 ];

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name /* title */ );


	insert_expense =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		insert_expense_new(
			fund_name,
			feeder_account,
			full_name,
			contact_key,
			new_full_name,
			transaction_date,
			account /* debit_account */,
			transaction_amount,
			check_number,
			memo );

	if ( insert_expense->error_message )
	{
		printf( "<h3>%s</h3>\n",
			insert_expense->error_message );
	}
	else
	{
		insert_expense->
			transaction_binary->
			transaction_date_time =
				/* -------------------------------------- */
				/* Returns inserted transaction_date_time */
				/* -------------------------------------- */
				transaction_insert(
					insert_expense->
						transaction_binary->
							fund_name,
					insert_expense->
						transaction_binary->
							full_name,
					insert_expense->
						transaction_binary->
							contact_key,
					insert_expense->
						transaction_binary->
							transaction_date_time,
					insert_expense->
						transaction_binary->
							transaction_amount,
					insert_expense->
						transaction_binary->
							check_number,
					insert_expense->
						transaction_binary->
							memo,
					insert_expense->
							transaction_binary->
							journal_list,
					1 /* insert_journal_list_boolean */ );
	
		if ( insert_expense->
			insert_expense_input->
			new_name_boolean )
		{
			error_message =
				/* ----------------------------- */
				/* Returns error message or null */
				/* ----------------------------- */
				entity_insert(
					ENTITY_TABLE,
					ENTITY_FULL_NAME_COLUMN,
					ENTITY_CONTACT_KEY_COLUMN,
					insert_expense->
					    insert_expense_input->
					    entity_contact_key_boolean,
					insert_expense->
					    insert_expense_input->
						full_name,
					insert_expense->
					    insert_expense_input->
					    entity_contact_key,
					1 /* ignore_duplicate_boolean */ );
		}
	
		if ( error_message ) printf( "%s\n", error_message );

		transaction_html_display( insert_expense->transaction_binary );
	}

	document_close();

	return 0;
}

