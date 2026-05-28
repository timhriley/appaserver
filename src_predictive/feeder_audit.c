/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_audit.c			*/
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
#include "float.h"
#include "sql.h"
#include "appaserver_error.h"
#include "date_convert.h"
#include "transaction_date.h"
#include "feeder_audit.h"

FEEDER_AUDIT *feeder_audit_fetch(
		char *application_name,
		char *login_name,
		char *feeder_account_name )
{
	FEEDER_AUDIT *feeder_audit;

	if ( !application_name
	||   !login_name
	||   !feeder_account_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_audit = feeder_audit_calloc();

	if ( ! ( feeder_audit->feeder_load_event =
			feeder_load_event_latest_fetch(
				FEEDER_LOAD_EVENT_TABLE,
				feeder_account_name ) ) )
	{
		return feeder_audit;
	}

	feeder_audit->feeder_audit_journal =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		feeder_audit_journal_fetch(
			feeder_account_name,
			feeder_audit->
				feeder_load_event->
				feeder_load_date_time );

	feeder_audit->account_fetch =
		account_fetch(
			feeder_account_name,
			1 /* fetch_subclassification */,
			1 /* fetch_element */ );

	if ( !feeder_audit->account_fetch )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"account_fetch(%s) returned empty.",
			feeder_account_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_audit->journal_balance =
		feeder_audit_accumulate_debit_journal_balance(
			feeder_audit->
				feeder_audit_journal->
				balance
				/* journal_fetch_balance */,
			feeder_audit->
				account_fetch->
				subclassification->
				element->
				accumulate_debit );

	feeder_audit->balance_difference =
		feeder_audit_balance_difference(
			feeder_audit->
				feeder_load_event->
				feeder_row_account_end_balance,
		feeder_audit->journal_balance );

	feeder_audit->difference_zero_boolean =
		feeder_audit_difference_zero_boolean(
			feeder_audit->balance_difference );

	feeder_audit->html_table =
		html_table_new(
			FEEDER_AUDIT_HTML_TITLE,
			(char *)0 /* sub_title */,
			(char *)0 /* sub_sub_title */ );

	feeder_audit->html_table->column_list =
		feeder_audit_html_column_list();

	feeder_audit->html_table->row_list = list_new();

	list_set(
		feeder_audit->html_table->row_list,
		feeder_audit_html_row(
			application_name,
			login_name,
			feeder_audit->
				feeder_load_event->
				feeder_row_account_end_balance,
			feeder_audit->
				feeder_load_event->
				feeder_row_account_end_date,
			feeder_audit->journal_balance,
			feeder_audit->balance_difference,
			feeder_audit->difference_zero_boolean ) );

	return feeder_audit;
}

double feeder_audit_accumulate_debit_journal_balance(
		double journal_fetch_balance,
		boolean element_accumulate_debit )
{
	if ( element_accumulate_debit )
		return journal_fetch_balance;
	else
		return -journal_fetch_balance;
}

double feeder_audit_balance_different(
		double file_row_balance,
		double journal_balance )
{
	return file_row_balance - journal_balance;
}

boolean feeder_audit_difference_zero_boolean( double balance_difference )
{
	return
	float_money_virtually_same(
		balance_difference,
		0.0 );
}

FEEDER_AUDIT *feeder_audit_calloc( void )
{
	FEEDER_AUDIT *feeder_audit;

	if ( ! ( feeder_audit = calloc( 1, sizeof ( FEEDER_AUDIT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return feeder_audit;
}

LIST *feeder_audit_html_column_list( void )
{
	LIST *list = list_new();

	list_set(
		list,
		html_column_new(
			"final_feeder_date",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"feeder_load_event_balance",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"journal_balance",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"difference",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"status",
			0 /* not right_justify_boolean */ ) );

	return list;
}

HTML_ROW *feeder_audit_html_row(
		char *application_name,
		char *login_name,
		double account_end_balance,
		char *account_end_date,
		double journal_balance,
		double balance_difference,
		boolean difference_zero_boolean )
{
	LIST *cell_list;
	char *date_string;

	if ( !application_name
	||   !login_name
	||   !account_end_date )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	date_string =
		/* ----------------------------------------------- */
		/* Returns heap memory, source_date_string or null */
		/* ----------------------------------------------- */
		date_convert_return_date_string(
			date_convert_international
				/* source_enum */,
			date_convert_login_name_enum(
				application_name,
				login_name )
				/* destination_enum */,
			account_end_date
				/* source_date_string */ );

	cell_list =
		feeder_audit_html_cell_list(
			date_string /* feeder_date */,
			account_end_balance,
			journal_balance,
			balance_difference,
			difference_zero_boolean );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	html_row_new( cell_list );
}

double feeder_audit_balance_difference(
		double file_row_balance,
		double journal_balance )
{
	return file_row_balance - journal_balance;
}

LIST *feeder_audit_html_cell_list(
		char *feeder_date,
		double event_account_end_balance,
		double journal_balance,
		double balance_difference,
		boolean difference_zero_boolean )
{
	LIST *list = list_new();

	if ( !feeder_date )
	{
		char message[ 128 ];

		sprintf(message, "feeder_date is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list_set(
		list,
		html_cell_new(
			feeder_date,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			strdup(
				/* ---------------------- */
				/* Returns static memory. */
				/* Doesn't trim pennies.  */
				/* ---------------------- */
				string_commas_money(
					event_account_end_balance ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			strdup(
				/* ---------------------- */
				/* Returns static memory. */
				/* Doesn't trim pennies.  */
				/* ---------------------- */
				string_commas_money(
					journal_balance ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			strdup(
				/* ---------------------- */
				/* Returns static memory. */
				/* Doesn't trim pennies.  */
				/* ---------------------- */
				string_commas_money(
					balance_difference ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	if ( difference_zero_boolean )
	{
		list_set(
			list,
			html_cell_new(
				"Okay",
				0 /* not large_boolean */,
				0 /* not bold_boolean */ ) );
	}
	else
	{
		list_set(
			list,
			html_cell_new(
				"Different",
				1 /* large_boolean */,
				1 /* bold_boolean */ ) );
	}

	return list;
}
