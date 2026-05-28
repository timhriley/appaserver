/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder.c				*/
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
#include "predictive.h"
#include "account.h"
#include "feeder_row.h"
#include "feeder_matched_journal.h"
#include "feeder_load_event.h"
#include "feeder.h"

FEEDER *feeder_fetch(
		char *application_name,
		char *login_name,
		char *fund_name,
		char *feeder_account_name,
		char *exchange_format_filename,
		LIST *exchange_journal_list,
		double exchange_journal_begin_amount,
		double exchange_balance_amount,
		char *exchange_minimum_date_string )
{
	FEEDER *feeder;

	if ( !application_name
	||   !login_name
	||   !feeder_account_name
	||   !exchange_format_filename
	||   !exchange_journal_list
	||   !exchange_minimum_date_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder = feeder_calloc();

	feeder->feeder_account_name = feeder_account_name;

	feeder->feeder_account =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		feeder_account_fetch(
			FEEDER_ACCOUNT_TABLE,
			feeder_account_name );

	feeder->feeder_load_row_list =
		feeder_load_row_list(
			exchange_journal_list );

/*
feeder_load_row_list_raw_display(
	stderr,
	feeder->feeder_load_row_list );
*/

	feeder->predictive_fund_boolean =
		predictive_fund_boolean(
			PREDICTIVE_FUND_TABLE_NAME,
			PREDICTIVE_FUND_COLUMN_NAME );

	feeder_load_row_file_row_balance_set(
		exchange_balance_amount,
		feeder->feeder_load_row_list
			/* reads exchange_journal_amount */
			/* sets file_row_balance */ );

	feeder->feeder_phrase_list =
		feeder_phrase_list(
			FEEDER_PHRASE_SELECT,
			FEEDER_PHRASE_TABLE );

	feeder->match_days_ago =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		feeder_match_days_ago(
			FEEDER_LOAD_TRANSACTION_DAYS_AGO,
			FEEDER_MATCH_DEFAULT_DAYS_AGO );

	feeder->match_minimum_date =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		feeder_match_minimum_date(
			exchange_minimum_date_string,
			feeder->match_days_ago );

	feeder->feeder_row_exist_list =
		feeder_row_exist_list(
			FEEDER_ROW_TABLE,
			feeder_account_name,
			feeder->match_minimum_date );

	feeder->account_uncleared_checks_string =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		account_uncleared_checks_string(
			ACCOUNT_UNCLEARED_CHECKS_KEY,
			__FUNCTION__ );

	feeder->feeder_matched_journal_list =
		feeder_matched_journal_list(
			FEEDER_ROW_TABLE,
			fund_name,
			feeder_account_name,
			feeder->match_minimum_date,
			feeder->account_uncleared_checks_string );

	feeder->feeder_row_list =
		feeder_row_list(
			fund_name,
			feeder_account_name,
			feeder->
				feeder_account->
				financial_institution_full_name,
			feeder->
				feeder_account->
				financial_institution_street_address,
			feeder->account_uncleared_checks_string,
			feeder->feeder_phrase_list,
			feeder->feeder_row_exist_list,
			feeder->feeder_matched_journal_list,
			feeder->feeder_load_row_list );

	feeder->feeder_row_count =
		feeder_row_count(
			feeder->feeder_row_list );

	if ( !feeder->feeder_row_count ) return feeder;

	feeder->feeder_row_insert_count =
		feeder_row_insert_count(
			feeder->feeder_row_list );

	feeder->feeder_load_event_prior_account_end_balance =
		feeder_load_event_prior_account_end_balance(
			FEEDER_LOAD_EVENT_TABLE,
			feeder_account_name );

	feeder_row_calculate_balance_set(
		feeder->feeder_row_list /* sets calculate_balance */,
		feeder->feeder_load_event_prior_account_end_balance );

	feeder->feeder_row_list_status_out_of_balance_boolean =
		feeder_row_list_status_out_of_balance_boolean(
			feeder->feeder_row_list );

	feeder_row_list_status_set(
		feeder->feeder_row_list /* sets feeder_row_status */ );

	feeder->feeder_load_event_latest_fetch =
		feeder_load_event_latest_fetch(
			FEEDER_LOAD_EVENT_TABLE,
			feeder_account_name );

	feeder->feeder_load_event_error_double =
		feeder_load_event_error_double(
			exchange_journal_begin_amount,
			feeder->feeder_row_list,
			feeder->feeder_load_event_latest_fetch );

	feeder->feeder_row_account_end_date =
		/* ------------------------------------------------------- */
		/* Returns feeder_row->feeder_load_row->international_date */
		/* ------------------------------------------------------- */
		feeder_row_account_end_date(
			feeder->feeder_row_list );

	feeder->feeder_row_account_end_balance =
		/* ------------------------------------------------------ */
		/* Returns feeder_row->feeder_load_row->calculate_balance */
		/* ------------------------------------------------------ */
		feeder_row_account_end_balance(
			feeder->feeder_row_list );

	feeder->feeder_row_list_non_match_boolean =
		feeder_row_list_non_match_boolean(
			feeder->feeder_row_list );

	if (	!feeder->feeder_row_list_status_out_of_balance_boolean
	&&	!feeder->feeder_row_list_non_match_boolean
	&&	!feeder->feeder_load_event_error_double )
	{
		feeder->feeder_load_event =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			feeder_load_event_new(
				login_name,
				feeder_account_name,
				exchange_format_filename,
				feeder->feeder_row_account_end_date,
				feeder->feeder_row_account_end_balance,
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				date_now19( date_utc_offset() )
					/* feeder_load_date_time */ );
	}

	return feeder;
}

FEEDER *feeder_calloc( void )
{
	FEEDER *feeder;

	if ( ! ( feeder = calloc( 1, sizeof ( FEEDER ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder;
}

void feeder_execute(
		char *process_name,
		char *fund_name,
		FEEDER *feeder )
{
	if ( !process_name
	||   !feeder )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* ---------------------------- */
	/* May reset			*/
	/* feeder_row->			*/
	/*	feeder_transaction->	*/
	/*	transaction_date_time	*/
	/* ---------------------------- */
	feeder_row_transaction_insert(
		fund_name,
		feeder->feeder_row_list );

	feeder_row_check_journal_update(
		feeder->feeder_row_list );

	feeder_row_journal_propagate(
		fund_name,
		feeder->feeder_account_name,
		feeder->
			feeder_load_event->
			feeder_load_date_time,
		feeder->account_uncleared_checks_string );

	feeder_load_event_insert(
		FEEDER_LOAD_EVENT_TABLE,
		FEEDER_LOAD_EVENT_INSERT,
		feeder->
			feeder_load_event->
			feeder_account_name,
		feeder->
			feeder_load_event->
			exchange_format_filename
				/* feeder_load_filename */,
		feeder->
			feeder_load_event->
			feeder_row_account_end_date,
		feeder->
			feeder_load_event->
			feeder_row_account_end_balance,
		feeder->
			feeder_load_event->
			feeder_load_date_time,
		feeder->
			feeder_load_event->
			appaserver_user->
			full_name,
		feeder->
			feeder_load_event->
			appaserver_user->
			street_address );

	feeder_row_list_insert(
		fund_name,
		feeder->feeder_account_name,
		feeder->
			feeder_load_event->
			feeder_load_date_time,
		feeder->feeder_row_list,
		feeder->predictive_fund_boolean );

	printf( "<h3>Execute feeder row count: %d</h3>\n",
		feeder->feeder_row_count );

	process_execution_count_increment(
		process_name );
}

void feeder_display( FEEDER *feeder )
{
	if ( !feeder )
	{
		char message[ 128 ];

		sprintf(message, "feeder is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( feeder->feeder_row_list_non_match_boolean )
	{
		printf( "<h1>Feeder Error Table</h1>\n" );

		feeder_row_error_display(
			feeder->feeder_row_list );
	}

	printf( "<h1>Transaction Table</h1>\n" );

	feeder_row_list_display(
		feeder->feeder_row_list );

	printf( "<h3>Non-execute feeder row count: %d</h3>\n",
		feeder->feeder_row_count );
}

boolean feeder_execute_boolean(
		boolean execute_boolean,
		boolean non_match_boolean,
		boolean out_of_balance_boolean,
		double feeder_load_event_error_double )
{
	if ( non_match_boolean
	||   out_of_balance_boolean
	||   feeder_load_event_error_double )
	{
		return 0;
	}

	return execute_boolean;
}

void feeder_process(
		char *application_name,
		char *process_name,
		char *login_name,
		char *fund_name,
		char *feeder_account_name,
		boolean execute_boolean,
		FEEDER *feeder )
{
	if ( feeder )
	{
		execute_boolean =
			feeder_execute_boolean(
			    execute_boolean,
			    feeder->feeder_row_list_non_match_boolean,
			    feeder->
				feeder_row_list_status_out_of_balance_boolean,
			    feeder->feeder_load_event_error_double );

		if ( !feeder->feeder_row_count )
		{
			printf( "<h3>No new feeder rows to process.</h3>\n" );
		}
		else
		if ( feeder->feeder_load_event_error_double )
		{
			char message[ 2048 ];

			feeder_display( feeder );

			snprintf(
				message,
				sizeof ( message ),
				FEEDER_MATCH_ERROR_TEMPLATE,
				feeder->feeder_load_event_error_double );

			printf( "%s\n", message );
		}
		else
		if ( execute_boolean
		&&   feeder->feeder_row_insert_count )
		{
			feeder_execute(
				process_name,
				fund_name,
				feeder );
		}
		else
		{
			feeder_display( feeder );
		}
	}

	if ( !feeder
	||   !feeder->feeder_row_insert_count
	||   execute_boolean )
	{
		FEEDER_AUDIT *feeder_audit;

		feeder_audit =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			feeder_audit_fetch(
				application_name,
				login_name,
				feeder_account_name );

		if ( !feeder_audit->feeder_load_event )
		{
			printf(
			"<h3>Warning: no feeder load events.</h3>\n" );
		}
		else
		if ( !feeder_audit->html_table )
		{
			printf(
			"<h3>ERROR: html_table is empty.</h3>\n" );
		}
		else
		{
			html_table_output(
				feeder_audit->html_table,
				HTML_TABLE_ROWS_BETWEEN_HEADING );

			if ( !feeder_audit->difference_zero_boolean )
			{
				printf( "<h3>%s</h3>\n",
					FEEDER_AUDIT_DIFFERENCE_MESSAGE );
			}
		}
	}
}

char *feeder_match_minimum_date(
		char *exchange_minimum_date_string,
		int feeder_match_days_ago )
{
	DATE *date;

	if ( !exchange_minimum_date_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"exchange_minimum_date_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( date =
			date_yyyy_mm_dd_new(
				exchange_minimum_date_string ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"Cannot determine minimum date using [%s].",
			exchange_minimum_date_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	date =
		date_decrement_days(
			date,
			(double)feeder_match_days_ago );

	return
	/* ------------------------- */
	/* Returns heap memory or "" */
	/* ------------------------- */
	date_display_yyyy_mm_dd( date );
}

int feeder_match_days_ago(
		const char *feeder_load_transaction_days_ago,
		const int feeder_match_default_days_ago )
{
	char *days_ago_string;

	days_ago_string =
	/* ------------------------------------------------------------------ */
	/* Returns component of application_constant->dictionary->get() or "" */
	/* ------------------------------------------------------------------ */
	     application_constant_fetch(
			(char *)feeder_load_transaction_days_ago );

	if ( *days_ago_string )
		return atoi( days_ago_string );
	else
		return feeder_match_default_days_ago;
}

