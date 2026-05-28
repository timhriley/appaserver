/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_audit_journal.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "appaserver_error.h"
#include "String.h"
#include "predictive.h"
#include "journal.h"
#include "feeder_row.h"
#include "feeder_audit_journal.h"

FEEDER_AUDIT_JOURNAL *feeder_audit_journal_fetch(
		char *feeder_account_name,
		char *feeder_load_date_time )
{
	FEEDER_AUDIT_JOURNAL *feeder_audit_journal;

	if ( !feeder_account_name
	||   !feeder_load_date_time )
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

	feeder_audit_journal = feeder_audit_journal_calloc();

	feeder_audit_journal->row_list =
		feeder_audit_journal_row_list(
			feeder_account_name,
			feeder_load_date_time );

	feeder_audit_journal->balance =
		feeder_audit_journal_balance(
			feeder_audit_journal->row_list );

	return feeder_audit_journal;
}

FEEDER_AUDIT_JOURNAL *feeder_audit_journal_calloc( void )
{
	FEEDER_AUDIT_JOURNAL *feeder_audit_journal;

	if ( ! ( feeder_audit_journal =
			calloc( 1,
				sizeof ( FEEDER_AUDIT_JOURNAL ) ) ) )
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

	return feeder_audit_journal;
}

LIST *feeder_audit_journal_row_list(
		char *feeder_account_name,
		char *feeder_load_date_time )
{
	LIST *row_list;

	if ( !predictive_fund_boolean(
		PREDICTIVE_FUND_TABLE_NAME,
		PREDICTIVE_FUND_COLUMN_NAME ) )
	{
		row_list =
			feeder_audit_journal_non_fund_row_list(
				feeder_account_name,
				feeder_load_date_time );
	}
	else
	{
		row_list =
			feeder_audit_journal_fund_row_list(
				feeder_account_name,
				feeder_load_date_time );
	}

	return row_list;
}

LIST *feeder_audit_journal_non_fund_row_list(
		char *feeder_account_name,
		char *feeder_load_date_time )
{
	LIST *row_list = list_new();
	FEEDER_ROW *feeder_row;

	feeder_row =
		feeder_audit_feeder_row_fetch(
			(char *)0 /* fund_name */,
			feeder_account_name,
			feeder_load_date_time );

	if ( !feeder_row )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"feeder_audit_feeder_row_fetch(%s,%s) returned empty.",
			feeder_account_name,
			feeder_load_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list_set( row_list, feeder_row );

	return row_list;
}

LIST *feeder_audit_journal_fund_row_list(
		char *feeder_account_name,
		char *feeder_load_date_time )
{
	LIST *row_list = list_new();
	LIST *fund_name_list;
	char *fund_name;
	FEEDER_ROW *feeder_row;

	fund_name_list =
		predictive_fund_name_list(
			PREDICTIVE_FUND_TABLE_NAME,
			PREDICTIVE_FUND_COLUMN_NAME );

	if ( list_rewind( fund_name_list ) )
	do {
		fund_name = list_get( fund_name_list );

		feeder_row =
			feeder_audit_feeder_row_fetch(
				fund_name,
				feeder_account_name,
				feeder_load_date_time );

		list_set( row_list, feeder_row );

	} while ( list_next( fund_name_list ) );

	if ( !list_length( row_list ) )
	{
		list_free( row_list );
		row_list = NULL;
	}

	return row_list;
}

FEEDER_ROW *feeder_audit_feeder_row_fetch(
		char *fund_name,
		char *feeder_account_name,
		char *feeder_load_date_time )
{
	int final_number;
	FEEDER_ROW *feeder_row;
	JOURNAL *journal;

	final_number =
		feeder_row_final_number(
			FEEDER_ROW_TABLE,
			fund_name,
			feeder_account_name,
			feeder_load_date_time );

	if ( !final_number )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"feeder_row_final_number(%s,%s,%s) returned empty.",
			(fund_name) ? fund_name : "fund",
			feeder_account_name,
			feeder_load_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	feeder_row =
		feeder_row_fetch(
			feeder_account_name,
			feeder_load_date_time,
			final_number /* feeder_row_number */ );

	if ( !feeder_row )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"feeder_row_fetch(%s,%s,%d) returned empty.",
			feeder_account_name,
			feeder_load_date_time,
			final_number );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	journal =
		journal_account_fetch(
			fund_name,
			feeder_row->transaction_date_time,
			feeder_account_name,
			0 /* not fetch_account */,
			0 /* not fetch_subclassification */,
			0 /* not fetch_entity */,
			0 /* not fetch_transaction */ );

	if ( !journal )
	{
		char message[ 1024 ];

		snprintf(
			message,
			sizeof ( message ),
			"journal_account_fetch(%s,%s,%s) returned empty.",
			(fund_name) ? fund_name : "fund",
			feeder_row->transaction_date_time,
			feeder_account_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_row->journal = journal;

	return feeder_row;
}

double feeder_audit_journal_balance( LIST *feeder_row_list )
{
	FEEDER_ROW *feeder_row;
	double balance = {0};

	if ( list_rewind( feeder_row_list ) )
	do {
		feeder_row  = list_get( feeder_row_list );

		balance += feeder_row->journal->balance;

	} while ( list_next( feeder_row_list ) );

	return balance;
}
