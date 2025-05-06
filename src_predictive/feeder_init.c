/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_init.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include "environ.h"
#include "String.h"
#include "appaserver_error.h"
#include "float.h"
#include "insert.h"
#include "date.h"
#include "security.h"
#include "feeder.h"
#include "feeder_init.h"

FEEDER_INIT_PASSTHRU *feeder_init_passthru_new(
		boolean checking_boolean,
		char *entity_self_full_name,
		char *entity_self_street_address )
{
	FEEDER_INIT_PASSTHRU *feeder_init_passthru;

	if ( !entity_self_full_name
	||   !entity_self_street_address )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_init_passthru = feeder_init_passthru_calloc();

	feeder_init_passthru->account_name =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_init_passthru_account_name(
			ACCOUNT_PASSTHRU_KEY );

	feeder_init_passthru->feeder_phrase =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		(char *)feeder_init_passthru_feeder_phrase(
			FEEDER_INIT_CASH_FEEDER_PHRASE,
			FEEDER_INIT_CARD_FEEDER_PHRASE,
			checking_boolean );

	feeder_init_passthru->exist_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_init_passthru_exist_system_string(
			FEEDER_PHRASE_TABLE,
			feeder_init_passthru->feeder_phrase );

	feeder_init_passthru->exist_boolean =
		feeder_init_passthru_exist_boolean(
			feeder_init_passthru->exist_system_string );

	if ( feeder_init_passthru->exist_boolean )
		return feeder_init_passthru;

	feeder_init_passthru->insert_sql =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_init_passthru_insert_sql(
			FEEDER_PHRASE_TABLE,
			feeder_init_passthru->account_name,
			entity_self_full_name,
			entity_self_street_address,
			feeder_init_passthru->feeder_phrase );

	return feeder_init_passthru;
}

FEEDER_INIT_PASSTHRU *feeder_init_passthru_calloc( void )
{
	FEEDER_INIT_PASSTHRU *feeder_init_passthru;

	if ( ! ( feeder_init_passthru =
			calloc( 1,
				sizeof ( FEEDER_INIT_PASSTHRU ) ) ) )
	{
		char message[ 128 ];

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

	return feeder_init_passthru;
}

const char *feeder_init_passthru_feeder_phrase(
		const char *feeder_init_cash_feeder_phrase,
		const char *feeder_init_card_feeder_phrase,
		boolean checking_boolean )
{
	if ( checking_boolean )
		return feeder_init_cash_feeder_phrase;
	else
		return feeder_init_card_feeder_phrase;
}

boolean feeder_init_passthru_exist_boolean( char *system_string )
{
	char *fetch;

	if ( !system_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	fetch =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_pipe_fetch(
			system_string );

	if ( fetch && atoi( fetch ) )
		return 1;
	else
		return 0;
}

char *feeder_init_passthru_insert_sql(
		const char *feeder_phrase_table,
		char *account_name,
		char *entity_self_full_name,
		char *entity_self_street_address,
		char *passthru_feeder_phrase )
{
	char *attribute_name_list_string;
	char *value_list_string;
	char *insert_sql;
	LIST *insert_datum_list;

	if ( !account_name
	||   !entity_self_full_name
	||   !entity_self_street_address
	||   !passthru_feeder_phrase )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	insert_datum_list = list_new();

	list_set(
		insert_datum_list,
		insert_datum_new(
			"feeder_phrase" /* attribute_name */,
			passthru_feeder_phrase /* datum */,
			1 /* primary_key_index */,
			0 /* not attribute_is_number */ ) );

	list_set(
		insert_datum_list,
		insert_datum_new(
			"nominal_account" /* attribute_name */,
			account_name /* datum */,
			0 /* primary_key_index */,
			0 /* not attribute_is_number */ ) );

	list_set(
		insert_datum_list,
		insert_datum_new(
			"full_name" /* attribute_name */,
			entity_self_full_name /* datum */,
			0 /* primary_key_index */,
			0 /* not attribute_is_number */ ) );

	list_set(
		insert_datum_list,
		insert_datum_new(
			"street_address" /* attribute_name */,
			entity_self_street_address /* datum */,
			0 /* primary_key_index */,
			0 /* not attribute_is_number */ ) );

	attribute_name_list_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		insert_datum_attribute_name_list_string(
			insert_datum_list );

	value_list_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		insert_datum_value_list_string(
			insert_datum_list );

	insert_sql =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		insert_folder_sql_statement_string(
			(char *)feeder_phrase_table,
			attribute_name_list_string,
			value_list_string );

	return insert_sql;
}

char *feeder_init_passthru_exist_system_string(
		const char *feeder_phrase_table,
		char *passthru_feeder_phrase )
{
	char system_string[ 1024 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"count(1)\" %s \"%s\"",
		feeder_phrase_table,
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		feeder_phrase_primary_where(
	       		passthru_feeder_phrase ) );

	return strdup( system_string );
}

FEEDER_INIT_TRANSACTION *feeder_init_transaction_new(
		const char *transaction_begin_time,
		double exchange_journal_begin_amount,
		char *exchange_minimum_date_string,
		char *entity_self_full_name,
		char *entity_self_street_address,
		JOURNAL *debit_journal,
		JOURNAL *credit_journal )
{
	FEEDER_INIT_TRANSACTION *feeder_init_transaction;

	if ( !exchange_minimum_date_string
	||   !entity_self_full_name
	||   !entity_self_street_address
	||   !debit_journal
	||   !credit_journal )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_init_transaction = feeder_init_transaction_calloc();

	feeder_init_transaction->date_time =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		feeder_init_transaction_date_time(
			transaction_begin_time,
			exchange_minimum_date_string );

	feeder_init_transaction->transaction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		transaction_new(
			entity_self_full_name,
			entity_self_street_address,
			feeder_init_transaction->date_time );

	feeder_init_transaction->transaction->transaction_amount =
		float_abs( exchange_journal_begin_amount );

	feeder_init_transaction->transaction->memo =
		TRANSACTION_OPEN_MEMO;

	feeder_init_transaction->transaction->journal_list = list_new();

	list_set(
		feeder_init_transaction->transaction->journal_list,
		debit_journal );

	list_set(
		feeder_init_transaction->transaction->journal_list,
		credit_journal );

	return feeder_init_transaction;
}

FEEDER_INIT_TRANSACTION *feeder_init_transaction_calloc( void )
{
	FEEDER_INIT_TRANSACTION *feeder_init_transaction;

	if ( ! ( feeder_init_transaction =
			calloc( 1,
				sizeof ( FEEDER_INIT_TRANSACTION ) ) ) )
	{
		char message[ 128 ];

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

	return feeder_init_transaction;
}

char *feeder_init_transaction_date_time(
		const char *transaction_begin_time,
		char *exchange_minimum_date_string )
{
	static char transaction_date_time[ 32 ];

	if ( !exchange_minimum_date_string
	||   !*exchange_minimum_date_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"checking_begin_date is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		transaction_date_time,
		sizeof ( transaction_date_time ),
		"%s %s",
		exchange_minimum_date_string,
		transaction_begin_time );

	return transaction_date_time;
}

char *feeder_init_transaction_equity_account_name(
		const char *account_equity_key )
{
	return
	/* ---------------------------------------------------- */
	/* Returns heap memory from static list or null (maybe) */
	/* ---------------------------------------------------- */
	account_hard_coded_account_name(
		(char *)account_equity_key,
		0 /* not warning_only */,
		__FUNCTION__ /* calling_function_name */ );
}

JOURNAL *feeder_init_transaction_journal(
		double exchange_journal_begin_amount,
		char *account_name,
		boolean debit_boolean,
		boolean fetch_account_boolean )
{
	JOURNAL *journal;

	if ( !account_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	journal =
		journal_new(
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			(char *)0 /* transaction_date_time */,
			account_name );

	if ( fetch_account_boolean )
	{
		if ( ! ( journal->account =
				account_fetch(
					journal->account_name,
					1 /* fetch_subclassification */,
					1 /* fetch_element */ ) ) )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"account_fetch(%s) returned empty.",
				journal->account_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}
	}

	if ( debit_boolean )
	{
		journal->debit_amount =
			exchange_journal_begin_amount;
	}
	else
	{
		journal->credit_amount =
			exchange_journal_begin_amount;
	}

	return journal;
}

void feeder_init_transaction_insert(
		FEEDER_INIT_TRANSACTION *feeder_init_transaction )
{
	if ( !feeder_init_transaction
	||   !feeder_init_transaction->transaction )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"feeder_init_transaction is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	transaction_insert(
		feeder_init_transaction->
			transaction->
			full_name,
		feeder_init_transaction->
			transaction->
			street_address,
		feeder_init_transaction->
			transaction->
			transaction_date_time,
		feeder_init_transaction->
			transaction->
			transaction_amount,
		0 /* check_number */,
		feeder_init_transaction->
			transaction->
			memo,
		'n' /* lock_transaction_yn */,
		feeder_init_transaction->
			transaction->
			journal_list,
		1 /* insert_journal_list_boolean */ );
}

FEEDER_INIT_CREDIT *feeder_init_credit_new(
		boolean execute_boolean,
		double negate_exchange_journal_begin_amount,
		char *exchange_minimum_date_string,
		char *account_name,
		char *entity_self_full_name,
		char *entity_self_street_address )
{
	FEEDER_INIT_CREDIT *feeder_init_credit;

	if ( !exchange_minimum_date_string
	||   !account_name
	||   !entity_self_full_name
	||   !entity_self_street_address )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_init_credit = feeder_init_credit_calloc();

	feeder_init_credit->feeder_init_transaction_equity_account_name =
		/* ------------------------------------ */
		/* Returns heap memory from static list */
		/* ------------------------------------ */
		feeder_init_transaction_equity_account_name(
			ACCOUNT_EQUITY_KEY );

	feeder_init_credit->debit_journal =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		feeder_init_transaction_journal(
			negate_exchange_journal_begin_amount,
			feeder_init_credit->
				feeder_init_transaction_equity_account_name,
			1 /* debit_boolean */,
			execute_boolean /* fetch_account_boolean */ );

	feeder_init_credit->credit_journal =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		feeder_init_transaction_journal(
			negate_exchange_journal_begin_amount,
			account_name,
			0 /* not debit_boolean */,
			execute_boolean /* fetch_account_boolean */ );

	feeder_init_credit->feeder_init_transaction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		feeder_init_transaction_new(
			TRANSACTION_BEGIN_TIME,
			negate_exchange_journal_begin_amount,
			exchange_minimum_date_string,
			entity_self_full_name,
			entity_self_street_address,
			feeder_init_credit->debit_journal,
			feeder_init_credit->credit_journal );

	return feeder_init_credit;
}

FEEDER_INIT_CREDIT *feeder_init_credit_calloc( void )
{
	FEEDER_INIT_CREDIT *feeder_init_credit;

	if ( ! ( feeder_init_credit =
			calloc( 1,
				sizeof ( FEEDER_INIT_CREDIT ) ) ) )
	{
		char message[ 128 ];

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

	return feeder_init_credit;
}

FEEDER_INIT_CHECKING *feeder_init_checking_new(
		boolean execute_boolean,
		double exchange_journal_begin_amount,
		char *exchange_minimum_date_string,
		char *account_name,
		char *entity_self_full_name,
		char *entity_self_street_address )
{
	FEEDER_INIT_CHECKING *feeder_init_checking;

	if ( !exchange_minimum_date_string
	||   !account_name
	||   !entity_self_full_name
	||   !entity_self_street_address )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_init_checking = feeder_init_checking_calloc();

	feeder_init_checking->feeder_init_transaction_equity_account_name =
		/* ------------------------------------ */
		/* Returns heap memory from static list */
		/* ------------------------------------ */
		feeder_init_transaction_equity_account_name(
			ACCOUNT_EQUITY_KEY );

	feeder_init_checking->debit_journal =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		feeder_init_transaction_journal(
			exchange_journal_begin_amount,
			account_name,
			1 /* debit_boolean */,
			execute_boolean /* fetch_account_boolean */ );

	feeder_init_checking->credit_journal =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		feeder_init_transaction_journal(
			exchange_journal_begin_amount,
			feeder_init_checking->
				feeder_init_transaction_equity_account_name,
			0 /* not debit_boolean */,
			execute_boolean /* fetch_account_boolean */ );

	feeder_init_checking->feeder_init_transaction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		feeder_init_transaction_new(
			TRANSACTION_BEGIN_TIME,
			exchange_journal_begin_amount,
			exchange_minimum_date_string,
			entity_self_full_name,
			entity_self_street_address,
			feeder_init_checking->debit_journal,
			feeder_init_checking->credit_journal );

	return feeder_init_checking;
}

FEEDER_INIT_CHECKING *feeder_init_checking_calloc( void )
{
	FEEDER_INIT_CHECKING *feeder_init_checking;

	if ( ! ( feeder_init_checking =
			calloc( 1,
				sizeof ( FEEDER_INIT_CHECKING ) ) ) )
	{
		char message[ 128 ];

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

	return feeder_init_checking;
}

FEEDER_INIT_INPUT *feeder_init_input_new(
		char *application_name,
		char *financial_institution_full_name,
		boolean checking_boolean,
		char *exchange_minimum_date_string )
{
	FEEDER_INIT_INPUT *feeder_init_input;

	if ( !application_name
	||   !financial_institution_full_name
	||   !exchange_minimum_date_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_init_input = feeder_init_input_calloc();

	feeder_init_input->institution_missing_boolean =
		feeder_init_input_institution_missing_boolean(
			SECURITY_FORBID_CHARACTER_STRING,
			financial_institution_full_name );

	if ( feeder_init_input->institution_missing_boolean )
		return feeder_init_input;

	feeder_init_input->account_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		feeder_init_input_account_name(
			financial_institution_full_name,
			checking_boolean );

	feeder_init_input->account_exist_boolean =
		feeder_init_input_account_exist_boolean(
		ACCOUNT_TABLE,
		feeder_init_input->account_name );

	if ( feeder_init_input->account_exist_boolean )
		return feeder_init_input;

	feeder_init_input->date_now_yyyy_mm_dd =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_now_yyyy_mm_dd(
			date_utc_offset() );

	feeder_init_input->date_recent_boolean =
		feeder_init_input_date_recent_boolean(
			FEEDER_INIT_INPUT_DAYS_AGO,
			exchange_minimum_date_string,
			feeder_init_input->date_now_yyyy_mm_dd );

	feeder_init_input->entity_self =
		entity_self_fetch(
			0 /* not fetch_entity_boolean */ );

	feeder_init_input->appaserver_error_filespecification =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_error_filespecification(
			application_name );

	return feeder_init_input;
}

FEEDER_INIT_INPUT *feeder_init_input_calloc( void )
{
	FEEDER_INIT_INPUT *feeder_init_input;

	if ( ! ( feeder_init_input =
			calloc( 1,
				sizeof ( FEEDER_INIT_INPUT ) ) ) )
	{
		char message[ 128 ];

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

	return feeder_init_input;
}

boolean feeder_init_input_account_exist_boolean(
		const char *account_table,
		char *account_name )
{
	char system_string[ 1024 ];
	char *primary_where;
	char *fetch;

	if ( !account_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	primary_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		account_primary_where(
			account_name );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"count(1)\" %s \"%s\"",
		account_table,
		primary_where );

	/* Returns heap memory or null */
	/* --------------------------- */
	fetch = string_pipe_fetch( system_string );

	if ( fetch && *fetch )
		return atoi( fetch );
	else
		return 0;
}

boolean feeder_init_input_date_recent_boolean(
		const int feeder_init_input_days_ago,
		char *exchange_minimum_date_string,
		char *date_now_yyyy_mm_dd )
{
	int days_between;

	if ( !exchange_minimum_date_string
	||   !date_now_yyyy_mm_dd )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	days_between =
		date_days_between(
			exchange_minimum_date_string /* early_date_string */,
			date_now_yyyy_mm_dd /* late_date_string */ );

	if ( days_between < 0 )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"begin date is in the future: %s.",
			exchange_minimum_date_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( days_between < feeder_init_input_days_ago )
		return 1;
	else
		return 0;
}

char *feeder_init_input_account_name(
		char *financial_institution_full_name,
		boolean checking_boolean )
{
	char *mnemonic;
	static char account_name[ 128 ];

	if ( !financial_institution_full_name
	|| !*financial_institution_full_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"financial_institution is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	mnemonic =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		string_mnemonic(
			financial_institution_full_name );

	if ( checking_boolean )
	{
		snprintf(
			account_name,
			sizeof ( account_name ),
			"%s_checking",
			mnemonic );
	}
	else
	{
		snprintf(
			account_name,
			sizeof ( account_name ),
			"%s_creditcard",
			mnemonic );
	}

	return account_name;
}

boolean feeder_init_input_institution_missing_boolean(
		const char *security_forbid_character_string,
		char *financial_institution_full_name )
{
	if ( !financial_institution_full_name
	||   !*financial_institution_full_name
	||   strcmp( financial_institution_full_name, "full_name" ) == 0 )
	{
		return 1;
	}

	if ( security_forbid_boolean(
		security_forbid_character_string,
		financial_institution_full_name /* string */ ) )
	{
		return 1;
	}

	return 0;
}

FEEDER_INIT *feeder_init_new(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *financial_institution_full_name,
		char *financial_institution_street_address,
		boolean checking_boolean,
		double exchange_journal_begin_amount,
		char *exchange_minimum_date_string )
{
	FEEDER_INIT *feeder_init;

	if ( !session_key
	||   !login_name
	||   !role_name
	||   !financial_institution_full_name
	||   !financial_institution_street_address
	||   !exchange_minimum_date_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_init = feeder_init_calloc();

	feeder_init->feeder_init_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		feeder_init_input_new(
			application_name,
			financial_institution_full_name,
			checking_boolean,
			exchange_minimum_date_string );

	if ( feeder_init->feeder_init_input->institution_missing_boolean )
		return feeder_init;

	if ( feeder_init->feeder_init_input->account_exist_boolean )
		return feeder_init;

	if ( !feeder_init->feeder_init_input->entity_self )
		return feeder_init;

	if ( checking_boolean )
	{
		feeder_init->feeder_init_checking =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			feeder_init_checking_new(
				0 /* not execute_boolean */,
				exchange_journal_begin_amount,
				exchange_minimum_date_string,
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
				0 /* not execute_boolean */,
				-exchange_journal_begin_amount
				/* negate_exchange_journal_begin_amount */,
				exchange_minimum_date_string,
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

	feeder_init->feeder_init_passthru =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		feeder_init_passthru_new(
			checking_boolean,
			feeder_init->
				feeder_init_input->
				entity_self->
				full_name,
			feeder_init->
				feeder_init_input->
				entity_self->
				street_address );

	feeder_init->account_insert_sql =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_init_account_insert_sql(
			ACCOUNT_TABLE,
			ACCOUNT_PRIMARY_KEY,
			SUBCLASSIFICATION_CASH,
			SUBCLASSIFICATION_CURRENT_LIABILITY,
			ACCOUNT_CREDIT_CARD_KEY,
			checking_boolean,
			feeder_init->feeder_init_input->account_name );

	feeder_init->feeder_account_insert_sql =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_init_feeder_account_insert_sql(
			FEEDER_ACCOUNT_TABLE,
			FEEDER_ACCOUNT_PRIMARY_KEY,
			feeder_init->feeder_init_input->account_name,
			financial_institution_full_name,
			financial_institution_street_address );

	feeder_init->insert_sql_list =
		feeder_init_insert_sql_list(
			feeder_init->feeder_init_passthru->insert_sql,
			feeder_init->account_insert_sql,
			feeder_init->feeder_account_insert_sql );

	feeder_init->trial_balance_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		feeder_init_trial_balance_system_string(
			FEEDER_INIT_TRIAL_EXECUTABLE,
			session_key,
			login_name,
			role_name );

	feeder_init->activity_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		feeder_init_activity_system_string(
			FEEDER_INIT_ACTIVITY_EXECUTABLE,
			session_key,
			login_name,
			role_name );

	feeder_init->position_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		feeder_init_position_system_string(
			FEEDER_INIT_POSITION_EXECUTABLE,
			session_key,
			login_name,
			role_name );

	return feeder_init;
}

FEEDER_INIT *feeder_init_calloc( void )
{
	FEEDER_INIT *feeder_init;

	if ( ! ( feeder_init = calloc( 1, sizeof ( FEEDER_INIT ) ) ) )
	{
		char message[ 128 ];

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

	return feeder_init;
}

char *feeder_init_feeder_account_insert_sql(
		const char *feeder_account_table,
		const char *feeder_account_primary_key,
		char *account_name,
		char *financial_institution_full_name,
		char *financial_institution_street_address )
{
	char *attribute_name_list_string;
	char *value_list_string;
	LIST *insert_datum_list;

	if ( !account_name
	||   !financial_institution_full_name
	||   !financial_institution_street_address )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	insert_datum_list = list_new();

	list_set(
		insert_datum_list,
		insert_datum_new(
			(char *)feeder_account_primary_key /* attribute_name */,
			account_name /* datum */,
			1 /* primary_key_index */,
			0 /* not attribute_is_number */ ) );

	list_set(
		insert_datum_list,
		insert_datum_new(
			"full_name" /* attribute_name */,
			financial_institution_full_name /* datum */,
			0 /* primary_key_index */,
			0 /* not attribute_is_number */ ) );

	list_set(
		insert_datum_list,
		insert_datum_new(
			"street_address" /* attribute_name */,
			financial_institution_street_address /* datum */,
			0 /* primary_key_index */,
			0 /* not attribute_is_number */ ) );

	attribute_name_list_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		insert_datum_attribute_name_list_string(
			insert_datum_list );

	value_list_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		insert_datum_value_list_string(
			insert_datum_list );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	insert_folder_sql_statement_string(
		(char *)feeder_account_table,
		attribute_name_list_string,
		value_list_string );
}

LIST *feeder_init_insert_sql_list(
		char *passthru_insert_sql,
		char *account_insert_sql,
		char *feeder_account_insert_sql )
{
	LIST *sql_list = list_new();

	list_set( sql_list, passthru_insert_sql );
	list_set( sql_list, account_insert_sql );
	list_set( sql_list, feeder_account_insert_sql );

	return sql_list;
}

char *feeder_init_trial_balance_system_string(
		const char *feeder_init_trial_executable,
		char *session_key,
		char *login_name,
		char *role_name )
{
	static char system_string[ 256 ];

	if ( !session_key
	||   !login_name
	||   !role_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"%s %s %s %s trial_balance as_of_date 0 omit table",
		feeder_init_trial_executable,
		session_key,
		login_name,
		role_name );

	return system_string;
}

char *feeder_init_activity_system_string(
		const char *feeder_init_activity_executable,
		char *session_key,
		char *login_name,
		char *role_name )
{
	static char system_string[ 256 ];

	if ( !session_key
	||   !login_name
	||   !role_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"%s %s %s %s statement_of_activitites as_of_date 0 omit table",
		feeder_init_activity_executable,
		session_key,
		login_name,
		role_name );

	return system_string;
}

char *feeder_init_position_system_string(
		const char *feeder_init_position_executable,
		char *session_key,
		char *login_name,
		char *role_name )
{
	static char system_string[ 256 ];

	if ( !session_key
	||   !login_name
	||   !role_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"%s %s %s %s financial_position as_of_date 0 omit table",
		feeder_init_position_executable,
		session_key,
		login_name,
		role_name );

	return system_string;
}

void feeder_init_transaction_html_display(
		FEEDER_INIT_CHECKING *feeder_init_checking,
		FEEDER_INIT_CREDIT *feeder_init_credit )
{
	if ( feeder_init_checking )
	{
		transaction_html_display(
			feeder_init_checking->
				feeder_init_transaction->
				transaction );
	}
	else
	if ( feeder_init_credit )
	{
		transaction_html_display(
			feeder_init_credit->
				feeder_init_transaction->
				transaction );
	}
}

char *feeder_init_account_insert_sql(
		const char *account_table,
		const char *account_primary_key,
		const char *subclassification_cash,
		const char *subclassification_current_liability,
		const char *account_credit_card_key,
		boolean checking_boolean,
		char *account_name )
{
	char *attribute_name_list_string;
	char *value_list_string;
	LIST *insert_datum_list;
	char *subclassification;

	if ( !account_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	insert_datum_list = list_new();

	list_set(
		insert_datum_list,
		insert_datum_new(
			(char *)account_primary_key /* attribute_name */,
			account_name /* datum */,
			1 /* primary_key_index */,
			0 /* not attribute_is_number */ ) );

	subclassification =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		(char *)feeder_init_subclassification(
			subclassification_cash,
			subclassification_current_liability,
			checking_boolean );

	list_set(
		insert_datum_list,
		insert_datum_new(
			"subclassification" /* attribute_name */,
			subclassification /* datum */,
			0 /* primary_key_index */,
			0 /* not attribute_is_number */ ) );

	if ( !checking_boolean )
	{
		list_set(
			insert_datum_list,
			insert_datum_new(
				"hard_coded_account_key",
				(char *)account_credit_card_key,
				0 /* primary_key_index */,
				0 /* not attribute_is_number */ ) );
	}

	attribute_name_list_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		insert_datum_attribute_name_list_string(
			insert_datum_list );

	value_list_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		insert_datum_value_list_string(
			insert_datum_list );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	insert_folder_sql_statement_string(
		(char *)account_table,
		attribute_name_list_string,
		value_list_string );
}

const char *feeder_init_subclassification(
		const char *subclassification_cash,
		const char *subclassification_current_liability,
		boolean checking_boolean )
{
	if ( checking_boolean )
		return subclassification_cash;
	else
		return subclassification_current_liability;
}

char *feeder_init_passthru_account_name( const char *account_passthru_key )
{
	return
	/* ---------------------------------------------------- */
	/* Returns heap memory from static list or null (maybe) */
	/* ---------------------------------------------------- */
	account_hard_coded_account_name(
		(char *)account_passthru_key,
		0 /* not warning_only */,
		__FUNCTION__ /* calling_function_name */ );
}

