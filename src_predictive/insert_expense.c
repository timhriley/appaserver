/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/insert_expense.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_error.h"
#include "String.h"
#include "piece.h"
#include "appaserver.h"
#include "sql.h"
#include "float.h"
#include "date_convert.h"
#include "entity.h"
#include "predictive.h"
#include "journal.h"
#include "account.h"
#include "insert_expense.h"

INSERT_EXPENSE_INPUT *
	insert_expense_input_new(
		char *fund_name,
		char *feeder_account,
		char *full_name,
		char *contact_key,
		char *new_full_name,
		char *transaction_date,
		char *debit_account,
		double transaction_amount,
		int check_number )
{
	INSERT_EXPENSE_INPUT *
		insert_expense_input;

	insert_expense_input =
		insert_expense_input_calloc();

	insert_expense_input->transaction_date =
		/* -------------------------------- */
		/* Returns parameter or heap memory */
		/* -------------------------------- */
		insert_expense_input_transaction_date(
			transaction_date );

	insert_expense_input->
		predictive_fund_boolean =
			predictive_fund_boolean(
				PREDICTIVE_FUND_TABLE,
				PREDICTIVE_FUND_COLUMN );

	insert_expense_input->fund_empty_boolean =
		insert_expense_input_fund_empty_boolean(
			fund_name,
			insert_expense_input->
				predictive_fund_boolean );

	if ( insert_expense_input->fund_empty_boolean )
		return insert_expense_input;

	insert_expense_input->feeder_empty_boolean =
		insert_expense_input_feeder_empty_boolean(
			feeder_account );

	if ( insert_expense_input->feeder_empty_boolean )
		return insert_expense_input;

	insert_expense_input->new_name_boolean =
		insert_expense_input_new_name_boolean(
			new_full_name );

	insert_expense_input->name_empty_boolean =
		insert_expense_input_name_empty_boolean(
			full_name,
			insert_expense_input->new_name_boolean );

	if ( insert_expense_input->name_empty_boolean )
		return insert_expense_input;

	insert_expense_input->full_name =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		insert_expense_input_full_name(
			full_name,
			new_full_name,
			insert_expense_input->new_name_boolean );

	insert_expense_input->debit_empty_boolean =
		insert_expense_input_debit_empty_boolean(
			debit_account );

	if ( insert_expense_input->debit_empty_boolean )
		return insert_expense_input;

	insert_expense_input->amount_empty_boolean =
		insert_expense_input_amount_empty_boolean(
			transaction_amount );

	if ( insert_expense_input->amount_empty_boolean )
		return insert_expense_input;

	insert_expense_input->check_duplicate_boolean =
		insert_expense_input_check_duplicate_boolean(
			TRANSACTION_TABLE,
			transaction_amount,
			check_number );

	if ( insert_expense_input->check_duplicate_boolean )
		return insert_expense_input;

	insert_expense_input->
		entity_contact_key_boolean =
			entity_contact_key_boolean(
				ENTITY_TABLE,
				ENTITY_CONTACT_KEY_COLUMN );

	insert_expense_input->journal_duplicate_boolean =
		insert_expense_input_journal_duplicate_boolean(
			JOURNAL_TABLE,
			fund_name,
			insert_expense_input->full_name,
			contact_key,
			insert_expense_input->transaction_date,
			debit_account,
			transaction_amount,
			insert_expense_input->
				predictive_fund_boolean,
			insert_expense_input->
				entity_contact_key_boolean );

	if ( insert_expense_input->journal_duplicate_boolean )
		return insert_expense_input;

	insert_expense_input->credit_account =
		/* -------------------------------- */
		/* Returns parameter or heap memory */
		/* -------------------------------- */
		insert_expense_input_credit_account(
			ACCOUNT_UNCLEARED_CHECKS_KEY,
			feeder_account,
			check_number );

	insert_expense_input->transaction_date_time =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		insert_expense_input_transaction_date_time(
			insert_expense_input->transaction_date );

	insert_expense_input->
		entity_contact_key_boolean =
			entity_contact_key_boolean(
				ENTITY_TABLE,
				ENTITY_CONTACT_KEY_COLUMN );

	insert_expense_input->
		entity_contact_key =
			/* ------------------------------------- */
			/* Returns parameter heap memory or null */
			/* ------------------------------------- */
			entity_contact_key(
				ENTITY_TABLE,
				ENTITY_CONTACT_KEY_COLUMN,
				ENTITY_CONTACT_KEY_DEFAULT,
				full_name,
				contact_key,
				insert_expense_input->
					entity_contact_key_boolean );

	return insert_expense_input;
}

INSERT_EXPENSE_INPUT *
	insert_expense_input_calloc(
		void )
{
	INSERT_EXPENSE_INPUT *
		insert_expense_input;

	if ( ! ( insert_expense_input =
		    calloc( 1,
			    sizeof ( INSERT_EXPENSE_INPUT ) ) ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return insert_expense_input;
}

boolean insert_expense_input_feeder_empty_boolean(
		char *feeder_account )
{
	boolean empty_boolean = 0;

	if ( !feeder_account
	||   !*feeder_account
	||   strcmp( feeder_account, "feeder_account" ) == 0 )
	{
		empty_boolean = 1;
	}

	return empty_boolean;
}

boolean insert_expense_input_name_empty_boolean(
		char *full_name,
		boolean new_name_boolean )
{
	boolean empty_boolean = 0;

	if ( !full_name
	||   !*full_name
	||   strcmp( full_name, "full_name" ) == 0 )
	{
		empty_boolean = !new_name_boolean;
	}

	return empty_boolean;
}

boolean insert_expense_input_debit_empty_boolean(
		char *debit_account )
{
	boolean empty_boolean = 0;

	if ( !debit_account
	||   !*debit_account
	||   strcmp( debit_account, "account" ) == 0 )
	{
		empty_boolean = 1;
	}

	return empty_boolean;
}

boolean insert_expense_input_amount_empty_boolean(
		double transaction_amount )
{
	boolean empty_boolean = 0;

	if ( float_virtually_same(
		transaction_amount,
		0.0 )
	||   transaction_amount < 0.0 )
	{
		empty_boolean = 1;
	}

	return empty_boolean;
}

boolean insert_expense_input_check_duplicate_boolean(
		const char *transaction_table,
		double transaction_amount,
		int check_number )
{
	char where[ 128 ];
	char system_string[ 256 ];
	boolean duplicate_boolean = 0;

	if ( check_number )
	{
		snprintf(
			where,
			sizeof ( where ),
			"transaction_amount = %.2lf and "
			"check_number = %d",
			transaction_amount,
			check_number );

		snprintf(
			system_string,
			sizeof ( system_string ),
			"select.sh 'count(1)' %s \"%s\"",
			transaction_table,
			where );

		duplicate_boolean = atoi( string_pipe( system_string ) );
	}

	return duplicate_boolean;
}

char *insert_expense_input_transaction_date(
		char *transaction_date )
{
	DATE_CONVERT *date_convert;
	char *date;

	date_convert =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		date_convert_new(
			date_convert_unknown /* source_enum */,
			date_convert_international /* destination_enum */,
			transaction_date );

	if ( date_convert->source_enum ==
	     date_convert_unknown )
	{
		date = string_pipe( "now.sh ymd" );
	}
	else
	{
		date = date_convert->return_date_string;
	}

	return date;
}

boolean insert_expense_input_fund_empty_boolean(
		char *fund_name,
		boolean fund_boolean )
{
	boolean fund_empty_boolean = 0;

	if ( fund_boolean )
	{
		if ( !predictive_fund_name(
			fund_name,
			1 /* predictive_fund_boolean */ ) )
		{
			fund_empty_boolean = 1;
		}
	}

	return fund_empty_boolean;
}

boolean insert_expense_input_journal_duplicate_boolean(
		const char *journal_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *transaction_date,
		char *debit_account,
		double transaction_amount,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *journal_where;
	char system_string[ 1024 ];

	journal_where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		insert_expense_input_journal_where(
			fund_name,
			full_name,
			contact_key,
			transaction_date,
			debit_account,
			transaction_amount,
			fund_boolean,
			contact_key_boolean );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh 'count(1)' %s \"%s\"",
		journal_table,
		journal_where );

	free( journal_where );

	return atoi( string_pipe( system_string ) );
}

char *insert_expense_input_journal_where(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *transaction_date,
		char *debit_account,
		double transaction_amount,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	char *fund_where;
	char *primary_where;
	char where[ 1024 ];

	fund_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		predictive_fund_where(
			PREDICTIVE_FUND_COLUMN,
			fund_name,
			fund_boolean );

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		entity_primary_where(
			ENTITY_FULL_NAME_COLUMN,
			ENTITY_CONTACT_KEY_COLUMN,
			full_name,
			contact_key,
			contact_key_boolean );

	snprintf(
		where,
		sizeof ( where ),
		"account = '%s' and "
		"transaction_date_time like '%s %c' and "
		"debit_amount = %.2lf and "
		"%s and %s",
		debit_account,
		transaction_date,
		'%',
		transaction_amount,
		fund_where,
		primary_where );

	return strdup( where );
}

char *insert_expense_input_credit_account(
		const char *account_uncleared_checks_key,
		char *feeder_account,
		int check_number )
{
	char *credit_account;

	if ( check_number )
	{
		credit_account =
			/* ------------------------------------ */
			/* Returns heap memory from static list */
			/* ------------------------------------ */
			account_uncleared_checks_string(
				account_uncleared_checks_key,
				__FUNCTION__ );
	}
	else
	{
		credit_account = feeder_account;
	}

	return credit_account;
}

char *insert_expense_input_transaction_date_time(
		char *transaction_date )
{
	char *transaction_time;
	char transaction_date_time[ 128 ];

	if ( !transaction_date )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"transaction_date is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* Returns heap memory or null */
	/* ----------------------------*/
	transaction_time = string_pipe( "now.sh seconds" );

	snprintf(
		transaction_date_time,
		sizeof ( transaction_date_time ),
		"%s %s",
		transaction_date,
		transaction_time );

	free( transaction_time );

	return strdup( transaction_date_time );
}

INSERT_EXPENSE *
	insert_expense_new(
		char *fund_name,
		char *feeder_account,
		char *full_name,
		char *contact_key,
		char *new_full_name,
		char *transaction_date,
		char *debit_account,
		double transaction_amount,
		int check_number,
		char *memo )
{
	INSERT_EXPENSE *insert_expense;

	insert_expense = insert_expense_calloc();

	insert_expense->insert_expense_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		insert_expense_input_new(
			fund_name,
			feeder_account,
			full_name,
			contact_key,
			new_full_name,
			transaction_date,
			debit_account,
			transaction_amount,
			check_number );

	insert_expense->error_message =
		/* -------------------------------------------- */
		/* Returns program memory, heap memory, or null */
		/* -------------------------------------------- */
		insert_expense_error_message(
			check_number,
			insert_expense->
				insert_expense_input->
				fund_empty_boolean,
			insert_expense->
				insert_expense_input->
				feeder_empty_boolean,
			insert_expense->
				insert_expense_input->
				name_empty_boolean,
			insert_expense->
				insert_expense_input->
				debit_empty_boolean,
			insert_expense->
				insert_expense_input->
				amount_empty_boolean,
			insert_expense->
				insert_expense_input->
				check_duplicate_boolean,
			insert_expense->
				insert_expense_input->
				journal_duplicate_boolean );

	if ( !insert_expense->error_message )
	{
		insert_expense->transaction_binary =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			transaction_binary(
				fund_name,
				insert_expense->
					insert_expense_input->
					full_name,
				insert_expense->
					insert_expense_input->
					entity_contact_key,
				insert_expense->
					insert_expense_input->
					transaction_date_time,
				transaction_amount,
				memo,
				debit_account,
				feeder_account /* credit_account_name */ );
	}

	return insert_expense;
}

INSERT_EXPENSE *insert_expense_calloc( void )
{
	INSERT_EXPENSE *insert_expense;

	if ( ! ( insert_expense =
			calloc( 1,
				sizeof ( INSERT_EXPENSE ) ) ) )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return insert_expense;
}

char *insert_expense_error_message(
		int check_number,
		boolean fund_empty_boolean,
		boolean feeder_empty_boolean,
		boolean name_empty_boolean,
		boolean debit_empty_boolean,
		boolean amount_empty_boolean,
		boolean check_duplicate_boolean,
		boolean journal_duplicate_boolean )
{
	char *error_message = {0};

	if ( fund_empty_boolean )
		error_message =
			"Please choose a fund.";
	else
	if ( feeder_empty_boolean )
		error_message =
			"Please choose a feeder account.";
	else
	if ( name_empty_boolean )
		error_message =
			"Please choose an entity or enter in a new full name.";
	else
	if ( debit_empty_boolean )
		error_message =
			"Please choose a nominal account.";
	else
	if ( amount_empty_boolean )
		error_message =
			"Please enter in a positive transaction amount.";
	else
	if ( check_duplicate_boolean )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"Check number %d already exists.",
			check_number );

		error_message = strdup( message );
	}
	else
	if ( journal_duplicate_boolean )
		error_message =
			INSERT_EXPENSE_JOURNAL_DUPLICATE_MESSAGE;

	return error_message;
}

boolean insert_expense_input_new_name_boolean(
		char *new_full_name )
{
	boolean new_name_boolean = 0;

	if ( new_full_name
	&&   *new_full_name
	&&   strcmp( new_full_name, "new_full_name" ) != 0 )
	{
		new_name_boolean = 1;
	}

	return new_name_boolean;
}

char *insert_expense_input_full_name(
		char *full_name,
		char *new_full_name,
		boolean new_name_boolean )
{
	char *input_full_name;

	if ( new_name_boolean )
		input_full_name = new_full_name;
	else
		input_full_name = full_name;

	return input_full_name;
}

