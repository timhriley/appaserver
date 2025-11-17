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

	feeder_audit->feeder_row_final_number =
		feeder_row_final_number(
			FEEDER_ROW_TABLE,
			feeder_account_name,
			feeder_audit->
				feeder_load_event->
				feeder_load_date_time );

	if ( !feeder_audit->feeder_row_final_number )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"feeder_row_final_number(%s,%s) returned empty.",
			feeder_account_name,
			feeder_audit->
				feeder_load_event->
				feeder_load_date_time );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( feeder_audit->feeder_row_fetch =
			feeder_row_fetch(
				feeder_account_name,
				feeder_audit->
					feeder_load_event->
					feeder_load_date_time,
				feeder_audit->
					feeder_row_final_number
					/* feeder_row_number */ ) ) )
	{
		char message[ 256 ];

		snprintf(
			message,
			sizeof ( message ),
			"feeder_row_fetch(%s,%s,%d) returned empty.",
				feeder_account_name,
				feeder_audit->
					feeder_load_event->
					feeder_load_date_time,
				feeder_audit->
					feeder_row_final_number );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	feeder_audit->journal_latest =
		journal_latest(
			JOURNAL_TABLE,
			(char *)0 /* fund_name */,
			feeder_account_name,
			(char *)0 /* transaction_date_time */,
			0 /* not fetch_transaction_boolean */,
			1 /* latest_zero_balance_boolean */ );

	if ( !feeder_audit->journal_latest )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"journal_latest(%s) returned empty.",
			feeder_account_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !feeder_audit->journal_latest->full_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"journal_latest(%s)->full_name is empty.",
			feeder_account_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

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


	feeder_audit->credit_balance_negate =
		feeder_audit_credit_balance_negate(
			feeder_audit->
				journal_latest->
				balance
				/* journal_balance */,
			feeder_audit->
				account_fetch->
				subclassification->
				element->
				accumulate_debit );

	feeder_audit->balance_difference =
		feeder_audit_balance_difference(
			feeder_audit->
				feeder_row_fetch->
				calculate_balance,
		feeder_audit->credit_balance_negate
			/* journal_balance */ );

	feeder_audit->difference_zero =
		feeder_audit_difference_zero(
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
			feeder_audit->feeder_row_fetch
				/* feeder_row */,
			feeder_audit->journal_latest
				/* journal */,
			feeder_audit->credit_balance_negate
				/* journal_balance */,
			feeder_audit->balance_difference,
			feeder_audit->difference_zero ) );

	return feeder_audit;
}

double feeder_audit_credit_balance_negate(
		double journal_balance,
		boolean element_accumulate_debit )
{
	if ( element_accumulate_debit )
		return journal_balance;
	else
		return -journal_balance;
}

double feeder_audit_balance_different(
		double calculate_balance,
		double journal_balance )
{
	return calculate_balance - journal_balance;
}

boolean feeder_audit_difference_zero( double balance_difference )
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
			"final_row_number",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"final_feeder_date",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"feeder_row_full_name",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"journal_full_name",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"feeder_description",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"feeder_row_transaction_date_time",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"journal_transaction_date_time",
			0 /* not right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"feeder_row_amount",
			1 /* right_justify_boolean */ ) );

	list_set(
		list,
		html_column_new(
			"calculate_balance",
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
		FEEDER_ROW *feeder_row,
		JOURNAL *journal,
		double journal_balance,
		double feeder_audit_balance_difference,
		boolean feeder_audit_difference_zero )
{
	LIST *cell_list;

	if ( !application_name
	||   !login_name
	||   !feeder_row
	||   !journal )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list =
		feeder_audit_html_cell_list(
			feeder_row->feeder_row_number,
			feeder_row->full_name,
			feeder_row->file_row_description,
			feeder_row->transaction_date_time,
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
				feeder_row->feeder_date
					/* source_date_string */)
				/* feeder_date */,
			feeder_row->file_row_amount,
			feeder_row->calculate_balance,
			journal->full_name,
			journal->transaction_date_time,
			journal_balance,
			feeder_audit_balance_difference,
			feeder_audit_difference_zero );

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	html_row_new( cell_list );
}

double feeder_audit_balance_difference(
		double calculate_balance,
		double journal_balance )
{
	return calculate_balance - journal_balance;
}

LIST *feeder_audit_html_cell_list(
		int feeder_row_number,
		char *feeder_row_full_name,
		char *file_row_description,
		char *feeder_row_transaction_date_time,
		char *feeder_date,
		double feeder_row_file_row_amount,
		double feeder_row_calculate_balance,
		char *journal_full_name,
		char *journal_transaction_date_time,
		double journal_balance,
		double balance_difference,
		boolean difference_zero )
{
	LIST *list;
	char cell_string[ 128 ];

	if ( !feeder_row_full_name
	||   !file_row_description
	||   !feeder_row_transaction_date_time
	||   !feeder_date
	||   !feeder_row_file_row_amount )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !journal_full_name )
	{
		char message[ 128 ];

		sprintf(message, "journal_full_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !feeder_row_number )
	{
		char message[ 128 ];

		sprintf(message, "feeder_row_number is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !journal_transaction_date_time )
	{
		char message[ 128 ];

		sprintf(message, "journal_transaction_date_time is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list = list_new();

	sprintf(cell_string,
		"%d",
		feeder_row_number );

	list_set(
		list,
		html_cell_new(
			strdup( cell_string ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			feeder_date,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			feeder_row_full_name,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			journal_full_name,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			file_row_description,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			feeder_row_transaction_date_time,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		list,
		html_cell_new(
			journal_transaction_date_time,
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
					feeder_row_file_row_amount ) ),
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
					feeder_row_calculate_balance ) ),
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

	if ( difference_zero )
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
