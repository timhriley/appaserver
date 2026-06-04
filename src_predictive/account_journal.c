/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/account_journal.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "list.h"
#include "boolean.h"
#include "appaserver_error.h"
#include "journal.h"
#include "account_journal.h"

ACCOUNT_JOURNAL *account_journal_latest(
		const char *journal_table,
		char *fund_name,
		char *account_name,
		char *end_date_time_string,
		boolean fetch_transaction,
		boolean latest_zero_balance_boolean )
{
	ACCOUNT_JOURNAL *account_journal;
	JOURNAL *journal;

	if ( !account_name )
	{
		char message[ 128 ];

		sprintf(message, "account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( journal =
			journal_latest(
				journal_table,
				fund_name,
				account_name,
				end_date_time_string,
				fetch_transaction,
				latest_zero_balance_boolean ) ) )
	{
		return NULL;
	}

	account_journal = account_journal_calloc();

	account_journal->full_name = journal->full_name;
	account_journal->street_address = journal->street_address;
	account_journal->transaction_date_time = journal->transaction_date_time;
	account_journal->account_name = journal->account_name;
	account_journal->previous_balance = journal->previous_balance;
	account_journal->debit_amount = journal->debit_amount;
	account_journal->credit_amount = journal->credit_amount;
	account_journal->balance = journal->balance;
	account_journal->transaction = journal->transaction;

	return account_journal;
}

ACCOUNT_JOURNAL *account_journal_calloc( void )
{
	ACCOUNT_JOURNAL *account_journal;

	if ( ! ( account_journal = calloc( 1, sizeof ( ACCOUNT_JOURNAL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return account_journal;
}

