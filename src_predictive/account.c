/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/account.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "list.h"
#include "boolean.h"
#include "float.h"
#include "piece.h"
#include "sql.h"
#include "appaserver_error.h"
#include "folder_attribute.h"
#include "dictionary_separate.h"
#include "drillthru.h"
#include "prompt_lookup.h"
#include "predictive.h"
#include "dictionary.h"
#include "subclassification.h"
#include "journal.h"
#include "account.h"

LIST *account_statement_list(
		char *subclassification_primary_where,
		char *transaction_date_time_closing,
		boolean fetch_subclassification,
		boolean fetch_element,
		boolean fetch_journal_latest,
		boolean fetch_transaction,
		boolean latest_zero_balance_boolean )
{
	boolean chart_account_boolean;
	char *select_string;
	FILE *pipe;
	char input[ 1024 ];
	LIST *statement_list;
	ACCOUNT *account;

	if ( !subclassification_primary_where
	||   !transaction_date_time_closing )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	statement_list = list_new();

	chart_account_boolean =
		account_chart_account_boolean(
			ACCOUNT_TABLE,
			ACCOUNT_CHART_ACCOUNT_NUMBER );

	select_string =
		/* Returns heap memory */
		/* ------------------- */
		account_select_string(
			ACCOUNT_SELECT,
			ACCOUNT_CHART_ACCOUNT_NUMBER,
			chart_account_boolean );

	pipe =
		account_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			account_system_string(
				select_string,
				ACCOUNT_TABLE,
				subclassification_primary_where ) );

	while ( string_input( input, pipe, sizeof ( input ) ) )
	{
		account =
			account_statement_parse(
				input,
				transaction_date_time_closing,
				chart_account_boolean,
				fetch_subclassification,
				fetch_element,
				fetch_journal_latest,
				fetch_transaction,
				latest_zero_balance_boolean );

		if ( account )
		{
			list_set(
				statement_list,
				account );
		}
	}

	pclose( pipe );

	return statement_list;
}

ACCOUNT *account_statement_parse(
		char *input,
		char *transaction_date_time_closing,
		boolean account_chart_account_boolean,
		boolean fetch_subclassification,
		boolean fetch_element,
		boolean fetch_journal_latest,
		boolean fetch_transaction,
		boolean latest_zero_balance_boolean )
{
	ACCOUNT *account;

	if ( !input
	||   !*input
	||   !transaction_date_time_closing )
	{
		return NULL;
	}

	if ( ! ( account =
			account_parse(
				input,
				account_chart_account_boolean,
				fetch_subclassification,
				fetch_element ) ) )
	{
		return NULL;
	}

	if ( fetch_journal_latest )
	{
		if ( ! ( account->account_journal_latest =
				account_journal_latest(
					JOURNAL_TABLE,
					account->account_name,
					transaction_date_time_closing,
					fetch_transaction,
					latest_zero_balance_boolean ) ) )
		{
			free( account );
			account = NULL;
		}
	}

	return account;
}

FILE *account_pipe( char *account_system_string )
{
	if ( !account_system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	popen( account_system_string, "r" );
}

ACCOUNT *account_calloc( void )
{
	ACCOUNT *account;

	if ( ! ( account = calloc( 1, sizeof ( ACCOUNT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return account;
}

ACCOUNT *account_new( char *account_name )
{
	ACCOUNT *account;

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

	account = account_calloc();
	account->account_name = account_name;

	return account;
}

char *account_name_format( char *account_name )
{
	static char name_format[ 256 ];

	return string_initial_capital( name_format, account_name );
}

char *account_key_account_name(
		const char *account_key,
		const char *calling_function_name )
{
	return
	/* ------------------------------------ */
	/* Returns heap memory from static list */
	/* ------------------------------------ */
	account_hard_coded_account_name(
		account_key,
		0 /* not warning_only */,
		calling_function_name );
}

ACCOUNT *account_revenue(
		const char *account_revenue_key,
		const char *calling_function_name )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	account_key_fetch(
		account_revenue_key,
		calling_function_name );
}

ACCOUNT *account_receivable(
		const char *account_receivable_key,
		const char *calling_function_name )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	account_key_fetch(
		account_receivable_key,
		calling_function_name );
}

ACCOUNT *account_payable(
		const char *account_payable_key,
		const char *calling_function_name )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	account_key_fetch(
		account_payable_key,
		calling_function_name );
}

ACCOUNT *account_uncleared_checks(
		const char *account_uncleared_checks_key,
		const char *calling_function_name )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	account_key_fetch(
		account_uncleared_checks_key,
		calling_function_name );
}

ACCOUNT *account_gain(
		const char *account_gain_key,
		const char *calling_function_name )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	account_key_fetch(
		account_gain_key,
		calling_function_name );
}

ACCOUNT *account_equity(
		const char *account_equity_key,
		const char *calling_function_name )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	account_key_fetch(
		account_equity_key,
		calling_function_name );
}

ACCOUNT *account_cost_of_goods_sold(
		const char *account_CGS_key,
		const char *calling_function_name )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	account_key_fetch(
		account_CGS_key,
		calling_function_name );
}

ACCOUNT *account_inventory(
		const char *account_inventory_key,
		const char *calling_function_name )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	account_key_fetch(
		account_inventory_key,
		calling_function_name );
}

char *account_equity_string(
		const char *account_equity_key,
		const char *calling_function_name )
{
	return
	/* ------------------------------------ */
	/* Returns heap memory from static list */
	/* ------------------------------------ */
	account_hard_coded_account_name(
		account_equity_key,
		0 /* not warning_only */,
		calling_function_name );
}

char *account_uncleared_checks_string(
		const char *account_uncleared_checks_key,
		const char *calling_function_name )
{
	return
	/* ------------------------------------ */
	/* Returns heap memory from static list */
	/* ------------------------------------ */
	account_hard_coded_account_name(
		account_uncleared_checks_key,
		0 /* not warning_only */,
		calling_function_name );
}

char *account_depreciation_string(
		const char *account_depreciation_key,
		const char *calling_function_name )
{
	return
	/* ------------------------------------ */
	/* Returns heap memory from static list */
	/* ------------------------------------ */
	account_hard_coded_account_name(
		account_depreciation_key,
		0 /* not warning_only */,
		calling_function_name );
}

char *account_payable_string(
		const char *account_payable_key,
		const char *calling_function_name )
{
	return
	/* ------------------------------------ */
	/* Returns heap memory from static list */
	/* ------------------------------------ */
	account_hard_coded_account_name(
		account_payable_key,
		0 /* not warning_only */,
		calling_function_name );
}

char *account_receivable_string(
		const char *account_receivable_key,
		const char *calling_function_name )
{
	return
	/* ------------------------------------ */
	/* Returns heap memory from static list */
	/* ------------------------------------ */
	account_hard_coded_account_name(
		account_receivable_key,
		0 /* not warning_only */,
		calling_function_name );
}

char *account_loss_string(
		const char *account_loss_key,
		const char *calling_function_name )
{
	return
	/* ------------------------------------ */
	/* Returns heap memory from static list */
	/* ------------------------------------ */
	account_hard_coded_account_name(
		account_loss_key,
		0 /* not warning_only */,
		calling_function_name );
}

char *account_accumulated_depreciation_string(
		const char *account_accumulated_depreciation_key,
		const char *calling_function_name )
{
	return
	/* ------------------------------------ */
	/* Returns heap memory from static list */
	/* ------------------------------------ */
	account_hard_coded_account_name(
		account_accumulated_depreciation_key,
		0 /* not warning_only */,
		calling_function_name );
}

ACCOUNT *account_loss(
		const char *account_loss_key,
		const char *calling_function_name )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	account_key_fetch(
		account_loss_key,
		calling_function_name );
}

ACCOUNT *account_sales_tax_payable(
		const char *account_sales_tax_payable_key,
		const char *calling_function_name )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	account_key_fetch(
		account_sales_tax_payable_key,
		calling_function_name );
}

ACCOUNT *account_depreciation(
		const char *account_depreciation_key,
		const char *calling_function_name )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	account_key_fetch(
		account_depreciation_key,
		calling_function_name );
}

ACCOUNT *account_accumulated_depreciation(
		const char *account_accumulated_key,
		const char *calling_function_name )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	account_key_fetch(
		account_accumulated_key,
		calling_function_name );
}

ACCOUNT *account_shipping_revenue(
		const char *account_shipping_revenue_key,
		const char *calling_function_name )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	account_key_fetch(
		account_shipping_revenue_key,
		calling_function_name );
}

ACCOUNT *account_credit_card_passthru(
		const char *account_passthru_key,
		const char *calling_function_name )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	account_key_fetch(
		account_passthru_key,
		calling_function_name );
}

char *account_closing_entry_string(
		const char *account_closing_key,
		const char *account_equity_key,
		const char *calling_function_name )
{
	char *closing_entry_string;

	closing_entry_string =
		/* -------------------------------------------- */
		/* Returns heap memory from static list or null */
		/* -------------------------------------------- */
		account_hard_coded_account_name(
			account_closing_key,
			1 /* warning_only */,
			calling_function_name );

	if ( closing_entry_string ) return closing_entry_string;

	closing_entry_string =
		/* ------------------------------------ */
		/* Returns heap memory from static list */
		/* ------------------------------------ */
		account_hard_coded_account_name(
			account_equity_key,
			0 /* not warning_only */,
			calling_function_name );

	return closing_entry_string;
}

ACCOUNT *account_drawing(
		const char *account_drawing_key,
		const char *calling_function_name )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	account_key_fetch(
		account_drawing_key,
		calling_function_name );
}

ACCOUNT *account_fees_expense(
		const char *account_fees_expense_key,
		const char *calling_function_name )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	account_key_fetch(
		account_fees_expense_key,
		calling_function_name );
}

ACCOUNT *account_sales_tax_expense(
		const char *account_sales_tax_expense_key,
		const char *calling_function_name )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	account_key_fetch(
		account_sales_tax_expense_key,
		calling_function_name );
}

char *account_name_display( char *account_name )
{
	if ( !account_name )
		return ACCOUNT_NOT_SET;
	else
		return account_name;
}

LIST *account_balance_sort_list( LIST *account_list )
{
	LIST *balance_sort_list;
	ACCOUNT *account;

	if ( !list_rewind( account_list ) ) return NULL;

	balance_sort_list = list_new();

	do {
		account = list_get( account_list );

		list_add_pointer_in_order(
			balance_sort_list,
			account,
			account_balance_compare_function );

	} while ( list_next( account_list ) );

	return balance_sort_list;
}

int account_balance_compare_function(
		ACCOUNT *account_from_list,
		ACCOUNT *account_compare )
{
	ACCOUNT_JOURNAL *from_list_latest_journal;
	ACCOUNT_JOURNAL *compare_latest_journal;

	from_list_latest_journal =
		account_from_list->account_journal_latest;

	compare_latest_journal =
		account_compare->account_journal_latest;

	if ( !from_list_latest_journal
	||   !from_list_latest_journal->balance )
	{
		return 1;
	}

	if ( !compare_latest_journal
	||   !compare_latest_journal->balance )
	{
		return 1;
	}

	if (	from_list_latest_journal->balance <=
		compare_latest_journal->balance )
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

void account_percent_of_asset_set(
		LIST *subclassification_account_statement_list,
		double asset_sum )
{
	ACCOUNT *account;

	if ( !asset_sum
	||   !list_rewind( subclassification_account_statement_list ) )
	{
		return;
	}

	do {
		account = list_get( subclassification_account_statement_list );

		account->percent_of_asset =
			float_percent_of_total(
				account->account_journal_latest->balance,
				asset_sum );

	} while ( list_next( subclassification_account_statement_list ) );
}

void account_percent_of_income_set(
		LIST *subclassification_account_statement_list,
		double element_income )
{
	ACCOUNT *account;

	if ( float_virtually_same( element_income, 0.0 )
	||   !list_rewind( subclassification_account_statement_list ) )
	{
		return;
	}

	do {
		account = list_get( subclassification_account_statement_list );

		account->percent_of_income =
			float_percent_of_total(
				account->account_journal_latest->balance,
				element_income );

	} while ( list_next( subclassification_account_statement_list ) );
}

ACCOUNT *account_seek(
		char *account_name,
		LIST *account_list )
{
	ACCOUNT *account;

	if ( !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: account_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	if ( !list_rewind( account_list ) ) return (ACCOUNT *)0;

	do {
		account = list_get( account_list );

		if ( string_strcmp( account->account_name, account_name ) == 0 )
			return account;

	} while ( list_next( account_list ) );

	return NULL;
}

ACCOUNT *account_parse(
		char *input,
		boolean account_chart_account_boolean,
		boolean fetch_subclassification,
		boolean fetch_element )
{
	char account_name[ 128 ];
	char piece_buffer[ 1024 ];
	ACCOUNT *account;

	if ( !input || !*input ) return NULL;

	/* See account_select_string() */
	/* --------------------------- */
	piece( account_name, SQL_DELIMITER, input, 0 );

	account =
		account_new(
			strdup( account_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	if ( *piece_buffer )
		account->subclassification_name =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	if ( *piece_buffer )
		account->hard_coded_account_key =
			strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 ); 
	if ( *piece_buffer )
		account->annual_budget =
			atoi( piece_buffer );

	if ( account_chart_account_boolean )
	{
		piece( piece_buffer, SQL_DELIMITER, input, 4 );
		if ( *piece_buffer )
			account->chart_account_number =
				strdup( piece_buffer );
	}

	if ( fetch_subclassification )
	{
		account->subclassification =
			subclassification_fetch(
				account->subclassification_name,
				fetch_element );
	}

	return account;
}

ACCOUNT *account_fetch(
		char *account_name,
		boolean fetch_subclassification,
		boolean fetch_element )
{
	ACCOUNT *account = {0};
	static LIST *list = {0};

	if ( !account_name ) return NULL;

	if ( fetch_subclassification && fetch_element )
	{
		if ( !list )
		{
			list =
				account_list(
					"1 = 1" /* where */,
					1 /* fetch_subclassification */,
					1 /* fetch_element */ );
		}

		account = account_seek( account_name, list );
	}
	else
	{
		boolean chart_account_boolean;
		char *select_string;

		chart_account_boolean =
			account_chart_account_boolean(
				ACCOUNT_TABLE,
				ACCOUNT_CHART_ACCOUNT_NUMBER );

		select_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			account_select_string(
				ACCOUNT_SELECT,
				ACCOUNT_CHART_ACCOUNT_NUMBER,
				chart_account_boolean );

		account =
			account_parse(
				string_pipe(
					/* ------------------- */
					/* Returns heap memory */
					/* ------------------- */
					account_system_string(
						select_string,
						ACCOUNT_TABLE,
	 					/* --------------------- */
	 					/* Returns static memory */
	 					/* --------------------- */
	 					account_primary_where(
							account_name ) ) ),
				chart_account_boolean,
				fetch_subclassification,
				fetch_element );
	}

	return account;
}

char *account_primary_where( char *account_name )
{
	static char where[ 128 ];
	char escape_account[ 64 ];

	snprintf(
		where,
		sizeof ( where ),
		"account = '%s'",
		string_escape_quote(
			escape_account,
			account_name ) );

	return where;
}

char *account_hard_coded_account_name(
		const char *account_key,
		boolean warning_only,
		const char *calling_function_name )
{
	static LIST *list = {0};
	ACCOUNT *account;

	if ( !list )
	{
		list = account_list( (char *)0, 0, 0 );
	}

	if ( ! ( account =
			account_key_seek(
				account_key,
				list ) ) )
	{
		if ( !warning_only )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d; called from %s(): cannot fetch key=%s.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 calling_function_name,
				 account_key );

			exit( 1 );
		}
		else
		{
			return NULL;
		}
	}

	return account->account_name;
}

ACCOUNT *account_key_seek(
		const char *account_key,
		LIST *account_list )
{
	ACCOUNT *account;

	if ( list_rewind( account_list ) )
	do {
		account = list_get( account_list );

		if ( piece_boolean(
			(char *)account_key /* search_string */,
			account->hard_coded_account_key /* delimited_string */,
			'|' /* delimiter */ ) )
		{
			return account;
		}

	} while( list_next( account_list ) );

	return NULL;
}

boolean account_name_changed( char *preupdate_account_name )
{
	if ( !preupdate_account_name
	||   !*preupdate_account_name
	||   strcmp(	preupdate_account_name,
			"preupdate_account" ) == 0 )
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

char *account_action_string(
		const char *dictionary_attribute_datum_delimiter,
		const char *dictionary_element_delimiter,
		const char *journal_table,
		const char *dictionary_separate_drillthru_prefix,
		const char *drillthru_skipped_key,
		const char *prompt_lookup_from_prefix,
		const char *prompt_lookup_to_prefix,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *transaction_begin_date_string,
		char *end_date_time_string,
		char *account_name )
{
	char action_string[ 1024 ];
	char *ptr = action_string;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !transaction_begin_date_string
	||   !end_date_time_string
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

	ptr += sprintf(
		ptr,
		"/cgi-bin/post_prompt_lookup?%s+%s+%s+%s+%s+",
		application_name,
		session_key,
		login_name,
		role_name,
		journal_table );

	ptr += sprintf(
		ptr,
		"form_radio_list%sradio_lookup%s",
		dictionary_attribute_datum_delimiter,
		dictionary_element_delimiter );

	ptr += sprintf(
		ptr,
		"account%s%s%s",
		dictionary_attribute_datum_delimiter,
		account_name,
		dictionary_element_delimiter );

	ptr += sprintf(
		ptr,
		"relation_operator_transaction_date_time%sbetween%s",
		dictionary_attribute_datum_delimiter,
		dictionary_element_delimiter );

	ptr += sprintf(
		ptr,
		"%stransaction_date_time%s%s 00:00:00%s",
		prompt_lookup_from_prefix,
		dictionary_attribute_datum_delimiter,
		transaction_begin_date_string,
		dictionary_element_delimiter );

	ptr += sprintf(
		ptr,
		"%stransaction_date_time%s%s%s",
		prompt_lookup_to_prefix,
		dictionary_attribute_datum_delimiter,
		end_date_time_string,
		dictionary_element_delimiter );

	ptr += sprintf(
		ptr,
		"%s%s%syes",
		dictionary_separate_drillthru_prefix,
		drillthru_skipped_key,
		dictionary_attribute_datum_delimiter );

	return strdup( action_string );
}

void account_list_action_string_set(
		LIST *account_statement_list,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *transaction_begin_date_string,
		char *end_date_time_string )
{
	ACCOUNT *account;

	if ( list_rewind( account_statement_list ) )
	do {
		account = list_get( account_statement_list );

		if ( account->transaction_count )
		{
			account->action_string =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				account_action_string(
					DICTIONARY_ATTRIBUTE_DATUM_DELIMITER,
					DICTIONARY_ELEMENT_DELIMITER,
					JOURNAL_TABLE,
					DICTIONARY_SEPARATE_DRILLTHRU_PREFIX,
					DRILLTHRU_SKIPPED_KEY,
					PROMPT_LOOKUP_FROM_PREFIX,
					PROMPT_LOOKUP_TO_PREFIX,
					application_name,
					session_key,
					login_name,
					role_name,
					transaction_begin_date_string,
					end_date_time_string,
					account->account_name );
		}

	} while ( list_next( account_statement_list ) );
}

void account_list_delta_prior_percent_set(
		LIST *prior_account_list /* in/out */,
		LIST *current_account_list )
{
	ACCOUNT *current_account;
	ACCOUNT *prior_account;

	if ( list_rewind( current_account_list ) )
	do {
		current_account =
			list_get(
				current_account_list );

		if ( !current_account->account_journal_latest )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: current_account->account_journal_latest is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( ! ( prior_account =
				account_seek(
					current_account->account_name,
					prior_account_list ) ) )
		{
			continue;
		}

		if ( !prior_account->account_journal_latest )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: prior_account->account_journal_latest is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( prior_account->account_journal_latest->balance )
		{
			prior_account->delta_prior_percent =
				float_delta_prior_percent(
					prior_account->
						account_journal_latest->
						balance,
					current_account->
						account_journal_latest->
						balance );
		}

	} while ( list_next( current_account_list ) );
}

double account_list_debit_sum(
		LIST *account_list,
		boolean element_accumulate_debit )
{
	ACCOUNT *account;
	double debit_sum = 0.0;

	if ( list_rewind( account_list ) )
	do {
		account = list_get( account_list );

		if ( !account->account_journal_latest )
		{
			char message[ 128 ];

			sprintf(message,
				"account->account_journal_latest is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !element_accumulate_debit
		&&   account->account_journal_latest->balance < 0.0 )
		{
			debit_sum -= account->account_journal_latest->balance;
		}
		else
		if ( element_accumulate_debit
		&&   account->account_journal_latest->balance > 0.0 )
		{
			debit_sum += account->account_journal_latest->balance;
		}

	} while ( list_next( account_list ) );

	return debit_sum;
}

double account_list_credit_sum(
		LIST *account_list,
		boolean element_accumulate_debit )
{
	ACCOUNT *account;
	double credit_sum = 0.0;

	if ( list_rewind( account_list ) )
	do {
		account = list_get( account_list );

		if ( !account->account_journal_latest )
		{
			char message[ 128 ];

			sprintf(message,
				"account->account_journal_latest is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( element_accumulate_debit
		&&   account->account_journal_latest->balance < 0.0 )
		{
			credit_sum -= account->account_journal_latest->balance;
		}
		else
		if ( !element_accumulate_debit
		&&   account->account_journal_latest->balance > 0.0 )
		{
			credit_sum += account->account_journal_latest->balance;
		}

	} while ( list_next( account_list ) );

	return credit_sum;
}

double account_balance_total( LIST *account_list )
{
	ACCOUNT *account;
	double balance_total = 0.0;

	if ( list_rewind( account_list ) )
	do {
		account = list_get( account_list );

		if ( !account->account_journal_latest )
		{
			char message[ 128 ];

			sprintf(message,
				"account->account_journal_latest is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		balance_total += account->account_journal_latest->balance;

	} while ( list_next( account_list ) );

	return balance_total;
}

ACCOUNT *account_getset(
		LIST *account_list,
		ACCOUNT *account )
{
	ACCOUNT *local_account;

	if ( !account_list )
	{
		char message[ 128 ];

		sprintf(message, "account_list is null." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ( local_account =
		account_seek(
			account->account_name,
			account_list ) ) )
	{
		return local_account;
	}

	list_set( account_list, account );

	return account;
}

LIST *account_cash_name_list(
		const char *account_table,
		const char *subclassification_cash )
{
	char system_string[ 256 ];
	char where[ 128 ];

	sprintf(where,
		"subclassification = '%s'",
		subclassification_cash );

	sprintf(system_string,
		"select.sh account %s \"%s\"",
		account_table,
		where );

	return list_pipe( system_string );
}

LIST *account_current_liability_name_list(
		const char *account_table,
		const char *subclassification_current_liability,
		const char *account_credit_card_key,
		LIST *exclude_account_name_list )
{
	char system_string[ 2048 ];
	char where[ 1024 ];
	char in_clause[ 512 ];

	if ( list_length( exclude_account_name_list ) )
	{
		snprintf(
			in_clause,
			sizeof ( in_clause ),
			"account not in (%s)",
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			string_in_clause( exclude_account_name_list ) );
	}
	else
	{
		strcpy( in_clause, "1 = 1" );
	}

	snprintf(
		where,
		sizeof ( where ),
		"subclassification = '%s' and			"
		"ifnull(hard_coded_account_key,'') <> '%s' and	"
		"%s						",
		subclassification_current_liability,
		account_credit_card_key,
		in_clause );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh account %s \"%s\"",
		account_table,
		where );

	return
	list_pipe_fetch( system_string );
}

LIST *account_receivable_name_list(
		const char *account_table,
		const char *subclassification_receivable )
{
	char system_string[ 256 ];
	char where[ 128 ];

	snprintf(
		where,
		sizeof ( where ),
		"subclassification = '%s'",
		subclassification_receivable );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh account %s \"%s\"",
		account_table,
		where );

	return
	list_pipe( system_string );
}

LIST *account_list(
		char *where,
		boolean fetch_subclassification,
		boolean fetch_element )
{
	boolean chart_account_boolean;
	char *select_string;

	if ( !where ) where = "1 = 1";

	chart_account_boolean =
		account_chart_account_boolean(
			ACCOUNT_TABLE,
			ACCOUNT_CHART_ACCOUNT_NUMBER );

	select_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		account_select_string(
			ACCOUNT_SELECT,
			ACCOUNT_CHART_ACCOUNT_NUMBER,
			chart_account_boolean );

	return
	account_system_list(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		account_system_string(
			select_string,
			ACCOUNT_TABLE,
			where ),
		chart_account_boolean,
		fetch_subclassification,
		fetch_element );
}

LIST *account_system_list(
		char *system_string,
		boolean account_chart_account_boolean,
		boolean fetch_subclassification,
		boolean fetch_element )
{
	LIST *system_list;
	char input[ 1024 ];
	FILE *pipe;

	system_list = list_new();
	pipe = account_pipe( system_string );

	while( string_input( input, pipe, sizeof ( input ) ) )
	{
		list_set(
			system_list,
			account_parse(
				input,
				account_chart_account_boolean,
				fetch_subclassification,
				fetch_element ) );
	}

	pclose( pipe );

	return system_list;
}

char *account_system_string(
		char *select,
		const char *table,
		char *where )
{
	char system_string[ 1024 ];

	if ( !select )
	{
		char message[ 128 ];

		sprintf(message, "select is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !where ) where = "1 = 1";

	snprintf(
		system_string,
		sizeof ( system_string ),
		"select.sh \"%s\" %s \"%s\" account",
		select,
		table,
		where );

	return strdup( system_string );
}

double account_list_sum( LIST *account_statement_list )
{
	ACCOUNT *account;
	double list_sum = 0.0;

	if ( list_rewind( account_statement_list ) )
	do {
		account = list_get( account_statement_list );

		if ( !account->account_journal_latest )
		{
			char message[ 128 ];

			sprintf(message,
				"account->account_journal_latest is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_sum += account->account_journal_latest->balance;

	} while ( list_next( account_statement_list ) );

	return list_sum;
}

void account_transaction_count_set(
		LIST *account_statement_list,
		char *transaction_begin_date_string,
		char *transaction_date_time_closing )
{
	ACCOUNT *account;

	if ( list_rewind( account_statement_list ) )
	do {
		account = list_get( account_statement_list );

		if ( !account->account_journal_latest )
		{
			char message[ 128 ];

			sprintf(message,
				"account->account_journal_latest is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( account->account_journal_latest->balance )
		{
			account->transaction_count =
				journal_transaction_count(
					JOURNAL_TABLE,
					account->account_name,
					transaction_begin_date_string,
					transaction_date_time_closing );
		}

	} while ( list_next( account_statement_list ) );
}

ACCOUNT *account_element_list_seek(
		char *account_name,
		LIST *element_statement_list )
{
	ELEMENT *element;
	ACCOUNT *account;

	if ( list_rewind( element_statement_list ) )
	do {
		element = list_get( element_statement_list );

		if ( list_length( element->subclassification_statement_list ) )
		{
			account =
				account_subclassification_list_seek(
					account_name,
					element->
					     subclassification_statement_list );

			if ( account ) return account;
		}

	} while ( list_next( element_statement_list ) );

	return NULL;
}

ACCOUNT *account_subclassification_list_seek(
		char *account_name,
		LIST *subclassification_statement_list )
{
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;

	if ( list_rewind( subclassification_statement_list ) )
	do {
		subclassification =
			list_get(
				subclassification_statement_list );

		if ( list_length( subclassification->account_statement_list ) )
		{
			if ( ( account =
				account_seek(
					account_name,
					subclassification->
						account_statement_list ) ) )
			{
				return account;
			}
		}

	} while ( list_next( subclassification_statement_list ) );

	return NULL;
}

ACCOUNT_JOURNAL *account_journal_latest(
		const char *journal_table,
		char *account_name,
		char *end_date_time_string,
		boolean fetch_transaction,
		boolean latest_zero_balance_boolean )
{
	ACCOUNT_JOURNAL *account_journal;
	JOURNAL *journal;

	if ( !account_name
	||   !end_date_time_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( journal =
			journal_latest(
				journal_table,
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

boolean account_accumulate_debit( char *account_name )
{
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

	return
	account_accumulate_debit_boolean(
		account_name );
}

boolean account_accumulate_debit_boolean( char *account_name )
{
	ACCOUNT *account;

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

	if ( ! ( account =
			account_fetch(
				account_name,
				1 /* fetch_subclassification */,
				1 /* fetch_element */ ) ) )
	{
		char message[ 128 ];

		sprintf(message,
			"account_fetch(%s) returned empty.",
			account_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	account->
		subclassification->
		element->
		accumulate_debit;
}

ACCOUNT *account_key_fetch(
		const char *hard_coded_account_key,
		const char *calling_function_name )
{
	ACCOUNT *account;

	account =
		account_fetch(
			/* -------------------------------------------- */
			/* Returns heap memory from static list or null */
			/* -------------------------------------------- */
			account_hard_coded_account_name(
				(char *)hard_coded_account_key,
				0 /* not warning_only */,
				calling_function_name ),
			1 /* fetch_subclassification */,
			1 /* fetch_element */ );

	if ( !account )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"account_fetch(key=%s,function=%s) returned empty.",
			hard_coded_account_key,
			calling_function_name );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	return account;
}

boolean account_chart_account_boolean(
		const char *account_table,
		const char *account_chart_account_number )
{
	return
	folder_attribute_exists(
		(char *)account_table,
		(char *)account_chart_account_number );
}

char *account_select_string(
		const char *account_select,
		const char *account_chart_account_number,
		boolean account_chart_account_boolean )
{
	char select_string[ 1024 ];

	if ( account_chart_account_boolean )
		snprintf(
			select_string,
			sizeof ( select_string ),
			"%s,%s",
			account_select,
			account_chart_account_number );
	else
		strcpy( select_string, account_select );

	return strdup( select_string );
}

char *account_drawing_string( const char *account_drawing_key )
{
	return
	account_hard_coded_account_name(
		account_drawing_key,
		1 /* warning_only */,
		__FUNCTION__ );
}

