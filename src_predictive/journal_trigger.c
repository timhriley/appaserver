/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/journal_trigger.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "boolean.h"
#include "list.h"
#include "piece.h"
#include "environ.h"
#include "entity.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "account.h"
#include "transaction.h"
#include "predictive.h"
#include "preupdate_change.h"
#include "journal.h"
#include "journal_propagate.h"

void journal_trigger_insert(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *transaction_date_time,
		char *account_name,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

void journal_trigger_delete(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *transaction_date_time,
		char *account_name,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

void journal_trigger_update(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *transaction_date_time,
		char *account_name,
		char *preupdate_fund,
		char *preupdate_transaction_date_time,
		char *preupdate_account,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		boolean fund_no_change_boolean,
		boolean transaction_no_change_boolean,
		boolean account_no_change_boolean );

/* Returns either parameter */
/* ------------------------ */
char *journal_trigger_earlier_date_time(
		char *transaction_date_time,
		char *preupdate_transaction_date_time,
		boolean transaction_no_change_boolean );

/* Returns either parameter */
/* ------------------------ */
char *journal_trigger_preupdate_fund(
		char *fund_name,
		char *preupdate_fund,
		boolean fund_no_change_boolean );

/* Returns either parameter */
/* ------------------------ */
char *journal_trigger_preupdate_account(
		char *account_name,
		char *preupdate_account,
		boolean account_no_change_boolean );

int main( int argc, char **argv )
{
	char *application_name;
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *transaction_date_time;
	char *account_name;
	char *debit_amount;
	char *credit_amount;
	char *state;
	char *preupdate_fund;
	char *preupdate_transaction_date_time;
	char *preupdate_account;
	char *preupdate_debit_amount;
	char *preupdate_credit_amount;
	PREUPDATE_CHANGE *fund_preupdate_change;
	PREUPDATE_CHANGE *transaction_preupdate_change;
	PREUPDATE_CHANGE *account_preupdate_change;
	PREUPDATE_CHANGE *debit_preupdate_change;
	PREUPDATE_CHANGE *credit_preupdate_change;
	boolean fund_boolean;
	boolean contact_key_boolean;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 14 )
	{
		fprintf( stderr,
"Usage: %s fund full_name contact_key transaction_date_time account debit_amount credit_amount state preupdate_fund preupdate_transaction_date_time preupdate_account preupdate_debit_amount preupdate_credit_amount\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	fund_name = argv[ 1 ];
	full_name = argv[ 2 ];
	contact_key = argv[ 3 ];
	transaction_date_time = argv[ 4 ];
	account_name = argv[ 5 ];
	debit_amount = argv[ 6 ];
	credit_amount = argv[ 7 ];
	state = argv[ 8 ];
	preupdate_fund = argv[ 9 ];
	preupdate_transaction_date_time = argv[ 10 ];
	preupdate_account = argv[ 11 ];
	preupdate_debit_amount = argv[ 12 ];
	preupdate_credit_amount = argv[ 13 ];

	if ( strcmp( state, APPASERVER_PREDELETE_STATE ) == 0 ) exit( 0 );

	fund_boolean =
		predictive_fund_boolean(
			PREDICTIVE_FUND_TABLE,
			PREDICTIVE_FUND_COLUMN );

	contact_key_boolean =
		entity_contact_key_boolean(
			ENTITY_TABLE,
			ENTITY_CONTACT_KEY_COLUMN );

	if ( strcmp( state, APPASERVER_DELETE_STATE ) == 0 )
	{
		journal_trigger_delete(
			fund_name,
			full_name,
			contact_key,
			transaction_date_time,
			account_name,
			fund_boolean,
			contact_key_boolean );

		exit( 0 );
	}

	if ( strcmp( state, "insert" ) == 0 )
	{
		journal_trigger_insert(
			fund_name,
			full_name,
			contact_key,
			transaction_date_time,
			account_name,
			fund_boolean,
			contact_key_boolean );

		exit( 0 );
	}

	/* Must be APPASERVER_UPDATE_STATE */
	/* ------------------------------- */
	fund_preupdate_change =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		preupdate_change_new(
			APPASERVER_INSERT_STATE,
			APPASERVER_PREDELETE_STATE,
			state,
			preupdate_fund,
			fund_name /* postupdate_datum */,
			"preupdate_fund_name"
				/* preupdate_placeholder_name */ );

	transaction_preupdate_change =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		preupdate_change_new(
			APPASERVER_INSERT_STATE,
			APPASERVER_PREDELETE_STATE,
			state,
			preupdate_transaction_date_time,
			transaction_date_time /* postupdate_datum */,
			"preupdate_transaction_date_time"
				/* preupdate_placeholder_name */ );

	account_preupdate_change =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		preupdate_change_new(
			APPASERVER_INSERT_STATE,
			APPASERVER_PREDELETE_STATE,
			state,
			preupdate_account,
			account_name /* postupdate_datum */,
			"preupdate_account"
				/* preupdate_placeholder_name */ );

	debit_preupdate_change =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		preupdate_change_new(
			APPASERVER_INSERT_STATE,
			APPASERVER_PREDELETE_STATE,
			state,
			preupdate_debit_amount,
			debit_amount /* postupdate_datum */,
			"preupdate_debit_amount"
				/* preupdate_placeholder_name */ );

	credit_preupdate_change =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		preupdate_change_new(
			APPASERVER_INSERT_STATE,
			APPASERVER_PREDELETE_STATE,
			state,
			preupdate_credit_amount,
			credit_amount /* postupdate_datum */,
			"preupdate_credit_amount"
				/* preupdate_placeholder_name */ );

	if ( fund_preupdate_change->no_change_boolean
	&&   transaction_preupdate_change->no_change_boolean
	&&   account_preupdate_change->no_change_boolean
	&&   debit_preupdate_change->no_change_boolean
	&&   credit_preupdate_change->no_change_boolean )
	{
		exit( 0 );
	}

	journal_trigger_update(
		fund_name,
		full_name,
		contact_key,
		transaction_date_time,
		account_name,
		preupdate_fund,
		preupdate_transaction_date_time,
		preupdate_account,
		fund_boolean,
		contact_key_boolean,
		fund_preupdate_change->no_change_boolean,
		transaction_preupdate_change->no_change_boolean,
		account_preupdate_change->no_change_boolean );

	return 0;
}

void journal_trigger_update(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *transaction_date_time,
		char *account_name,
		char *preupdate_fund,
		char *preupdate_transaction_date_time,
		char *preupdate_account,
		boolean fund_boolean,
		boolean contact_key_boolean,
		boolean fund_no_change_boolean,
		boolean transaction_no_change_boolean,
		boolean account_no_change_boolean )
{
	char *earlier_date_time;
	JOURNAL_PROPAGATE *journal_propagate;

	earlier_date_time =
		/* ------------------------ */
		/* Returns either parameter */
		/* ------------------------ */
		journal_trigger_earlier_date_time(
			transaction_date_time,
			preupdate_transaction_date_time,
			transaction_no_change_boolean );

	/* If the fund changed or the account changed */
	/* ------------------------------------------ */
	if ( !fund_no_change_boolean
	||   !account_no_change_boolean )
	{
		/* Propagate the old value(s) */
		/* -------------------------- */
		journal_propagate =
			journal_propagate_new(
				/* ------------------------ */
				/* Returns either parameter */
				/* ------------------------ */
				journal_trigger_preupdate_fund(
					fund_name,
					preupdate_fund,
					fund_no_change_boolean ),
				earlier_date_time,
				/* ------------------------ */
				/* Returns either parameter */
				/* ------------------------ */
				journal_trigger_preupdate_account(
					account_name,
					preupdate_account,
					account_no_change_boolean ),
				fund_boolean,
				contact_key_boolean );

		if ( !journal_propagate )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"journal_propagate_new(%s,%s) returned empty.",
				earlier_date_time,
				preupdate_account );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		journal_list_update(
			journal_propagate->
				update_statement_list );
	}

	/* Propagate the new value(s) */
	/* -------------------------- */
	journal_propagate =
		journal_propagate_new(
			fund_name,
			earlier_date_time,
			account_name,
			fund_boolean,
			contact_key_boolean );

	if ( !journal_propagate )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"journal_propagate_new(%s,%s) returned empty.",
			earlier_date_time,
			account_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	journal_list_update(
		journal_propagate->
			update_statement_list );

	transaction_fetch_update(
		fund_name,
		full_name,
		contact_key,
		transaction_date_time,
		fund_boolean,
		contact_key_boolean );
}

void journal_trigger_insert(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *transaction_date_time,
		char *account_name,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	JOURNAL_PROPAGATE *journal_propagate;

	if ( !full_name
	||   !transaction_date_time
	||   !account_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	journal_propagate =
		journal_propagate_new(
			fund_name,
			transaction_date_time,
			account_name,
			fund_boolean,
			contact_key_boolean );

	if ( !journal_propagate )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"journal_propagate_new(%s,%s) returned empty.",
			transaction_date_time,
			account_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	journal_list_update(
		journal_propagate->
			update_statement_list );

	transaction_fetch_update(
		fund_name,
		full_name,
		contact_key,
		transaction_date_time,
		fund_boolean,
		contact_key_boolean );
}

void journal_trigger_delete(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *transaction_date_time,
		char *account_name,
		boolean fund_boolean,
		boolean contact_key_boolean )
{
	JOURNAL_PROPAGATE *journal_propagate;

	if ( !full_name
	||   !transaction_date_time
	||   !account_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	journal_propagate =
		journal_propagate_new(
			fund_name,
			transaction_date_time,
			account_name,
			fund_boolean,
			contact_key_boolean );


	/* If still rows for this account */
	/* ------------------------------ */
	if ( journal_propagate )
	{
		journal_list_update(
			journal_propagate->
				update_statement_list );
	}

	transaction_fetch_update(
		fund_name,
		full_name,
		contact_key,
		transaction_date_time,
		fund_boolean,
		contact_key_boolean );
}

char *journal_trigger_earlier_date_time(
		char *transaction_date_time,
		char *preupdate_transaction_date_time,
		boolean transaction_no_change_boolean )
{
	char *earlier_date_time;

	if ( !transaction_no_change_boolean )
	{
		if ( string_strcmp(
			transaction_date_time,
			preupdate_transaction_date_time ) <= 0 )
		{
			earlier_date_time = transaction_date_time;
		}
		else
		{
			earlier_date_time = preupdate_transaction_date_time;
		}
	}
	else
	{
		earlier_date_time = transaction_date_time;
	}

	return earlier_date_time;
}

char *journal_trigger_preupdate_fund(
		char *fund_name,
		char *preupdate_fund,
		boolean fund_no_change_boolean )
{
	char *trigger_preupdate_fund;

	if ( fund_no_change_boolean )
	{
		trigger_preupdate_fund = fund_name;
	}
	else
	{
		trigger_preupdate_fund = preupdate_fund;
	}

	return trigger_preupdate_fund;
}

char *journal_trigger_preupdate_account(
		char *account_name,
		char *preupdate_account,
		boolean account_no_change_boolean )
{
	char *trigger_preupdate_account;

	if ( account_no_change_boolean )
	{
		trigger_preupdate_account = account_name;
	}
	else
	{
		trigger_preupdate_account = preupdate_account;
	}

	return trigger_preupdate_account;
}

