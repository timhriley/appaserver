/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_matched_journal.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "String.h"
#include "piece.h"
#include "appaserver.h"
#include "column.h"
#include "float.h"
#include "sql.h"
#include "sed.h"
#include "environ.h"
#include "process.h"
#include "application_constant.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "date_convert.h"
#include "entity.h"
#include "predictive.h"
#include "account.h"
#include "exchange.h"
#include "transaction.h"
#include "transaction_date.h"
#include "journal_propagate.h"
#include "feeder_load_event.h"
#include "feeder_audit.h"
#include "feeder.h"

LIST *feeder_matched_journal_list(
		const char *feeder_row_table,
		char *feeder_account_name,
		char *feeder_match_minimum_date,
		char *account_uncleared_checks_string )
{
	LIST *list = list_new();
	char *subquery;
	char *where;
	LIST *system_list;
	JOURNAL *journal;

	if ( !feeder_account_name
	||   !feeder_match_minimum_date
	||   !account_uncleared_checks_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	subquery =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_matched_journal_subquery(
			JOURNAL_TABLE,
			feeder_row_table,
			feeder_account_name,
			account_uncleared_checks_string );

	where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		feeder_matched_journal_where(
			feeder_account_name,
			account_uncleared_checks_string,
			subquery,
			feeder_match_minimum_date );

	system_list =
		journal_system_list(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			journal_system_string(
				JOURNAL_SELECT,
				JOURNAL_TABLE,
				PREDICTIVE_FUND_COLUMN,
				ENTITY_CONTACT_KEY_COLUMN,
				where ),
			0 /* not fetch_account */,
			0 /* not fetch_subclassification */,
			0 /* not fetch_element */,
			1 /* fetch_transaction */ );

	if ( list_rewind( system_list ) )
	do {
		journal = list_get( system_list );

		list_set(
			list,
			feeder_matched_journal_new(
				journal ) );

	} while ( list_next( system_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

FEEDER_MATCHED_JOURNAL *feeder_matched_journal_calloc( void )
{
	FEEDER_MATCHED_JOURNAL *feeder_matched_journal;

	if ( ! ( feeder_matched_journal =
			calloc(	1,
				sizeof ( FEEDER_MATCHED_JOURNAL ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder_matched_journal;
}

char *feeder_matched_journal_subquery(
		const char *journal_table,
		const char *feeder_row_table,
		char *feeder_account_name,
		char *account_uncleared_checks_string )
{
	char subquery[ 1024 ];

	snprintf(
		subquery,
		sizeof ( subquery ),
		"not exists						"
		"(select 1 from %s					"
		"	where %s.full_name =				"
		"	      %s.full_name and				"
		"	      %s.street_address =			"
		"	      %s.street_address and			"
		"	      %s.transaction_date_time =		"
		"	      %s.transaction_date_time and		"
		"	      %s.feeder_account in ('%s','%s'))		",
		feeder_row_table,
		feeder_row_table,
		journal_table,
		feeder_row_table,
		journal_table,
		feeder_row_table,
		journal_table,
		feeder_row_table,
		feeder_account_name,
		account_uncleared_checks_string );

	return strdup( subquery );
}

char *feeder_matched_journal_where(
		char *feeder_account_name,
		char *account_uncleared_checks,
		char *subquery,
		char *feeder_match_minimum_date )
{
	char where[ 2048 ];

	if ( !feeder_account_name
	||   !account_uncleared_checks
	||   !subquery
	||   !feeder_match_minimum_date )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(where,
		"account in ('%s','%s') and		"
		"transaction_date_time >= '%s' and	"
		"%s					",
		feeder_account_name,
		account_uncleared_checks,
		feeder_match_minimum_date,
		subquery );

	return strdup( where );
}

FEEDER_MATCHED_JOURNAL *
	feeder_matched_journal_check_seek(
		char *feeder_account_name,
		char *account_uncleared_checks_string,
		int check_number,
		double exchange_journal_amount,
		LIST *feeder_matched_journal_list )
{
	FEEDER_MATCHED_JOURNAL *feeder_matched_journal;

	if ( !feeder_account_name
	||   !account_uncleared_checks_string
	||   !check_number
	||   !exchange_journal_amount )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( feeder_matched_journal_list ) )
	do {
		feeder_matched_journal =
			list_get(
				feeder_matched_journal_list );

		if ( feeder_matched_journal->taken ) continue;

		if ( feeder_matched_journal->check_number == check_number
		&&   float_money_virtually_same(
			feeder_matched_journal->amount,
			exchange_journal_amount ) )
		{
			feeder_matched_journal->check_update_statement =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				feeder_matched_journal_check_update_statement(
					JOURNAL_TABLE,
					feeder_account_name,
					account_uncleared_checks_string,
					feeder_matched_journal->
						full_name,
					feeder_matched_journal->
						contact_key,
					feeder_matched_journal->
						transaction_date_time );

			return feeder_matched_journal;
		}

	} while ( list_next( feeder_matched_journal_list ) );

	return NULL;
}

FEEDER_MATCHED_JOURNAL *
	feeder_matched_journal_amount_seek(
		double amount,
		LIST *feeder_matched_journal_list )
{
	FEEDER_MATCHED_JOURNAL *feeder_matched_journal;

	if ( !amount )
	{
		char message[ 128 ];

		sprintf(message, "amount is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( feeder_matched_journal_list ) )
	do {
		feeder_matched_journal =
			list_get(
				feeder_matched_journal_list );

		if ( feeder_matched_journal->taken ) continue;
		if ( feeder_matched_journal->check_number ) continue;

		if ( float_money_virtually_same(
			feeder_matched_journal->amount,
			amount ) )
		{
			return feeder_matched_journal;
		}

	} while ( list_next( feeder_matched_journal_list ) );

	return NULL;
}

FEEDER_MATCHED_JOURNAL *feeder_matched_journal_new( JOURNAL *journal )
{
	FEEDER_MATCHED_JOURNAL *feeder_matched_journal;

	if ( !journal )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: journal is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !journal->transaction )
	{
		char message[ 256 ];

		snprintf(
			message,
			sizeof ( message ),
			"journal->transaction is empty for (%s/%s).",
			journal->full_name,
			journal->transaction_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_matched_journal = feeder_matched_journal_calloc();

	feeder_matched_journal->amount =
		feeder_matched_journal_amount(
			journal->debit_amount,
			journal->credit_amount );

	feeder_matched_journal->full_name = journal->full_name;
	feeder_matched_journal->contact_key = journal->contact_key;

	feeder_matched_journal->transaction_date_time =
		journal->transaction_date_time;

	feeder_matched_journal->account_name = journal->account_name;
	feeder_matched_journal->debit_amount = journal->debit_amount;
	feeder_matched_journal->credit_amount = journal->credit_amount;

	feeder_matched_journal->check_number =
		journal->transaction->check_number;

	return feeder_matched_journal;
}

double feeder_matched_journal_amount(
		double debit_amount,
		double credit_amount )
{
	if ( debit_amount )
		return debit_amount;
	else
		return 0.0 - credit_amount;
}

char *feeder_matched_journal_check_update_statement(
		const char *journal_table,
		char *feeder_account,
		char *account_uncleared_checks_string,
		char *full_name,
		char *contact_key,
		char *transaction_date_time )
{
	char update_statement[ 1024 ];

	if ( !feeder_account
	||   !account_uncleared_checks_string
	||   !full_name
	||   !transaction_date_time )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		update_statement,
		sizeof ( update_statement ),
		"update %s set account = '%s' where %s;",
		journal_table,
		feeder_account,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		journal_primary_where(
			(char *)0 /* fund_name */,
			full_name,
			contact_key,
			transaction_date_time,
			account_uncleared_checks_string ) );

	return strdup( update_statement );
}

