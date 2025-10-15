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
#include "appaserver.h"
#include "appaserver_error.h"
#include "account.h"
#include "transaction.h"
#include "preupdate_change.h"
#include "journal.h"
#include "journal_propagate.h"

void journal_trigger_insert(
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *account_name );

void journal_trigger_delete(
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *account_name );

void journal_trigger_update(
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *account_name,
		char *preupdate_transaction_date_time,
		char *preupdate_account,
		boolean transaction_no_change_boolean,
		boolean account_no_change_boolean );

/* Returns either parameter */
/* ------------------------ */
char *journal_trigger_earlier_date_time(
		char *transaction_date_time,
		char *preupdate_transaction_date_time,
		boolean transaction_no_change_boolean );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *transaction_date_time;
	char *account_name;
	char *debit_amount;
	char *credit_amount;
	char *state;
	char *preupdate_transaction_date_time;
	char *preupdate_account;
	char *preupdate_debit_amount;
	char *preupdate_credit_amount;
	PREUPDATE_CHANGE *transaction_preupdate_change;
	PREUPDATE_CHANGE *account_preupdate_change;
	PREUPDATE_CHANGE *debit_preupdate_change;
	PREUPDATE_CHANGE *credit_preupdate_change;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 12 )
	{
		fprintf( stderr,
"Usage: %s full_name street_address transaction_date_time account debit_amount credit_amount state preupdate_transaction_date_time preupdate_account preupdate_debit_amount preupdate_credit_amount\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	full_name = argv[ 1 ];
	street_address = argv[ 2 ];
	transaction_date_time = argv[ 3 ];
	account_name = argv[ 4 ];
	debit_amount = argv[ 5 ];
	credit_amount = argv[ 6 ];
	state = argv[ 7 ];
	preupdate_transaction_date_time = argv[ 8 ];
	preupdate_account = argv[ 9 ];
	preupdate_debit_amount = argv[ 10 ];
	preupdate_credit_amount = argv[ 11 ];

	if ( strcmp( state, APPASERVER_PREDELETE_STATE ) == 0 ) exit( 0 );

	if ( strcmp( state, APPASERVER_DELETE_STATE ) == 0 )
	{
		journal_trigger_delete(
			full_name,
			street_address,
			transaction_date_time,
			account_name );

		exit( 0 );
	}

	if ( strcmp( state, "insert" ) == 0 )
	{
		journal_trigger_insert(
			full_name,
			street_address,
			transaction_date_time,
			account_name );

		exit( 0 );
	}

	/* Must be APPASERVER_UPDATE_STATE */
	/* ------------------------------- */
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

	if ( transaction_preupdate_change->no_change_boolean
	&&   account_preupdate_change->no_change_boolean
	&&   debit_preupdate_change->no_change_boolean
	&&   credit_preupdate_change->no_change_boolean )
	{
		exit( 0 );
	}

	journal_trigger_update(
		full_name,
		street_address,
		transaction_date_time,
		account_name,
		preupdate_transaction_date_time,
		preupdate_account,
		transaction_preupdate_change->no_change_boolean,
		account_preupdate_change->no_change_boolean );

	return 0;
}

void journal_trigger_update(
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *account_name,
		char *preupdate_transaction_date_time,
		char *preupdate_account,
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

	if ( !account_no_change_boolean )
	{
		journal_propagate =
			journal_propagate_new(
				earlier_date_time,
				preupdate_account /* account_name */ );

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

	journal_propagate =
		journal_propagate_new(
			earlier_date_time,
			account_name );

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
		full_name,
		street_address,
		transaction_date_time );
}

void journal_trigger_insert(
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *account_name )
{
	JOURNAL_PROPAGATE *journal_propagate;

	if ( !full_name
	||   !street_address
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
			transaction_date_time,
			account_name );

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
		full_name,
		street_address,
		transaction_date_time );
}

void journal_trigger_delete(
		char *full_name,
		char *street_address,
		char *transaction_date_time,
		char *account_name )
{
	JOURNAL_PROPAGATE *journal_propagate;

	if ( !full_name
	||   !street_address
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
			transaction_date_time,
			account_name );

	/* If still rows for this account */
	/* ------------------------------ */
	if ( journal_propagate )
	{
		journal_list_update(
			journal_propagate->
				update_statement_list );
	}

	transaction_fetch_update(
		full_name,
		street_address,
		transaction_date_time );
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

