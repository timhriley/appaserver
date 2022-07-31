/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/account.c			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "list.h"
#include "piece.h"
#include "sql.h"
#include "boolean.h"
#include "predictive.h"
#include "float.h"
#include "subclassification.h"
#include "journal.h"
#include "account.h"

LIST *account_statement_list(
			char *subclassification_primary_where,
			char *transaction_date_time_closing,
			boolean fetch_subclassification,
			boolean fetch_element,
			boolean fetch_journal_latest,
			boolean fetch_transaction )
{
	FILE *pipe;
	char input[ 256 ];
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

	pipe =
		account_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			account_system_string(
				ACCOUNT_SELECT,
				ACCOUNT_TABLE,
				subclassification_primary_where ) );

	while ( string_input( input, pipe, 256 ) )
	{
		account =
		/* ---------------------------------------------------------- */
		/* Returns null if fetch_journal_latest is set but not exists */
		/* ---------------------------------------------------------- */
			account_statement_parse(
				input,
				transaction_date_time_closing,
				fetch_subclassification,
				fetch_element,
				fetch_journal_latest,
				fetch_transaction );

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
			boolean fetch_subclassification,
			boolean fetch_element,
			boolean fetch_journal_latest,
			boolean fetch_transaction )
{
	ACCOUNT *account;

	if ( !input
	||   !*input
	||   !transaction_date_time_closing )
	{
		return (ACCOUNT *)0;
	}

	if ( ! ( account =
			account_parse(
				input,
				fetch_subclassification,
				fetch_element ) ) )
	{
		return (ACCOUNT *)0;
	}

	if ( fetch_journal_latest )
	{
		if ( ! ( account->account_journal_latest =
				account_journal_latest(
					account->account_name,
					transaction_date_time_closing,
					fetch_transaction ) ) )
		{
			account = (ACCOUNT *)0;
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

ACCOUNT *account_new( char *account_name )
{
	ACCOUNT *account;

	if ( ! ( account = calloc( 1, sizeof( ACCOUNT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	account->account_name = account_name;
	return account;
}

char *account_name_format(
			char *account_name )
{
	static char name_format[ 256 ];

	return format_initial_capital( name_format, account_name );
}

char *account_revenue( char *account_revenue_key )
{
	return
	account_hard_coded_account_name(
		account_revenue_key,
		0 /* not warning_only */,
		__FUNCTION__ );
}

char *account_receivable( char *account_receivable_key )
{
	return
	account_hard_coded_account_name(
		account_receivable_key,
		0 /* not warning_only */,
		__FUNCTION__ );
}

char *account_payable( char *account_payable_key )
{
	return
	account_hard_coded_account_name(
		account_payable_key,
		0 /* not warning_only */,
		__FUNCTION__ );
}

char *account_uncleared_checks( char *account_uncleared_checks_key )
{
	return
	account_hard_coded_account_name(
		account_uncleared_checks_key,
		0 /* not warning_only */,
		__FUNCTION__ );
}

char *account_gain( char *account_gain_key )
{
	return
	account_hard_coded_account_name(
		account_gain_key,
		0 /* not warning_only */,
		__FUNCTION__ );
}

char *account_loss( char *account_loss_key )
{
	return
	account_hard_coded_account_name(
		account_loss_key,
		0 /* not warning_only */,
		__FUNCTION__ );
}

char *account_sales_tax_payable( char *account_sales_tax_payable_key )
{
	return
	account_hard_coded_account_name(
		account_sales_tax_payable_key,
		0 /* not warning_only */,
		__FUNCTION__ );
}

char *account_depreciation_expense( char *account_depreciation_key )
{
	return
	account_hard_coded_account_name(
		account_depreciation_key,
		0 /* not warning_only */,
		__FUNCTION__ );
}

char *account_accumulated_depreciation( char *account_accumulated_key )
{
	return
	account_hard_coded_account_name(
		account_accumulated_key,
		0 /* not warning_only */,
		__FUNCTION__ );
}


char *account_shipping_revenue( char *account_shipping_revenue_key )
{
	return
	account_hard_coded_account_name(
		account_shipping_revenue_key,
		0 /* not warning_only */,
		__FUNCTION__ );
}

char *account_cash( char *account_cash_key )
{
	return
	account_hard_coded_account_name(
		account_cash_key,
		0 /* not warning_only */,
		__FUNCTION__ );
}

char *account_closing_entry( char *account_closing_key )
{
	return
	account_hard_coded_account_name(
		account_closing_key,
		0 /* not warning_only */,
		__FUNCTION__ );
}

char *account_drawing( char *account_drawing_key )
{
	return
	account_hard_coded_account_name(
		account_drawing_key,
		0 /* not warning_only */,
		__FUNCTION__ );
}

char *account_fees_expense( char *account_fees_expense_key )
{
	return
	account_hard_coded_account_name(
		account_fees_expense_key,
		0 /* not warning_only */,
		__FUNCTION__ );
}

char *account_sales_tax_expense( char *account_sales_tax_expense_key )
{
	return
	account_hard_coded_account_name(
		account_sales_tax_expense_key,
		0 /* not warning_only */,
		__FUNCTION__ );
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

	if ( !list_rewind( account_list ) )
	{
		return (LIST *)0;
	}

	balance_sort_list = list_new();

	do {
		account = list_get( account_list );

		list_add_pointer_in_order(
			balance_sort_list,
			account,
			account_balance_match_function );

	} while ( list_next( account_list ) );

	return balance_sort_list;
}

int account_balance_match_function(
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

void account_list_percent_of_asset_set(
			LIST *account_list,
			double asset_sum )
{
	ACCOUNT *account;

	if ( !asset_sum
	||   !list_rewind( account_list ) )
	{
		return;
	}

	do {
		account = list_get( account_list );

		account->percent_of_asset =
			float_percent_of_total(
				account->account_journal_latest->balance,
				asset_sum );

	} while ( list_next( account_list ) );
}

void account_list_percent_of_revenue_set(
			LIST *account_list,
			double revenue_sum )
{
	ACCOUNT *account;

	if ( !revenue_sum
	||   !list_rewind( account_list ) )
	{
		return;
	}

	do {
		account = list_get( account_list );

		account->percent_of_revenue =
			float_percent_of_total(
				account->account_journal_latest->balance,
				revenue_sum );

	} while ( list_next( account_list ) );
}

ACCOUNT *account_seek(	char *account_name,
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

		if ( strcmp( account->account_name, account_name ) == 0 )
			return account;

	} while ( list_next( account_list ) );

	return (ACCOUNT *)0;
}

ACCOUNT *account_parse(	char *input,
			boolean fetch_subclassification,
			boolean fetch_element )
{
	char account_name[ 128 ];
	char piece_buffer[ 1024 ];
	ACCOUNT *account;

	if ( !input || !*input ) return (ACCOUNT *)0;

	/* See ACCOUNT_SELECT */
	/* ------------------ */
	piece( account_name, SQL_DELIMITER, input, 0 );

	account =
		account_new(
			strdup( account_name ) );

	if ( piece( piece_buffer, SQL_DELIMITER, input, 1 ) )
	{
		account->subclassification_name = strdup( piece_buffer );
	}

	if ( piece( piece_buffer, SQL_DELIMITER, input, 2 ) )
	{
		account->hard_coded_account_key = strdup( piece_buffer );
	}

	if ( piece( piece_buffer, SQL_DELIMITER, input, 3 ) )
	{
		account->chart_account_number = atoi( piece_buffer );
	}

	if ( piece( piece_buffer, SQL_DELIMITER, input, 4 ) )
	{
		account->annual_budget = atoi( piece_buffer );
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

ACCOUNT *account_fetch(	char *account_name,
			boolean fetch_subclassification,
			boolean fetch_element )
{
	if ( !account_name ) return (ACCOUNT *)0;

	return
	account_parse(
		string_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			account_system_string(
				ACCOUNT_SELECT,
				ACCOUNT_TABLE,
	 			/* --------------------- */
	 			/* Returns static memory */
	 			/* --------------------- */
	 			account_primary_where( account_name )
					/* where */ ) ),
		fetch_subclassification,
		fetch_element );
}

char *account_primary_where(
			char *account_name )
{
	static char where[ 128 ];
	char escape_account[ 64 ];

	sprintf(where,
		"account = '%s'",
		string_escape_quote(
			escape_account,
			account_name ) );

	return where;
}

char *account_hard_coded_account_name(
			char *account_key,
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
			return "";
		}
	}
	return account->account_name;
}

ACCOUNT *account_key_seek(
			char *account_key,
			LIST *account_list )
{
	ACCOUNT *account;

	if ( !list_rewind( account_list ) ) return (ACCOUNT *)0;

	do {
		account = list_get( account_list );

		if ( string_exists_substr(
			account->hard_coded_account_key,
			account_key ) )
		{
			return account;
		}

	} while( list_next( account_list ) );

	return (ACCOUNT *)0;
}

boolean account_name_changed(
			char *preupdate_account_name )
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
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *transaction_begin_date_string,
			char *transaction_date_time_closing,
			char *account_name )
{
	char action_string[ 1024 ];

	sprintf(action_string,
"/cgi-bin/post_prompt_edit_form?%s^%s^%s^journal_ledger^%s^lookup^prompt^edit_frame^0^lookup_option_radio_button~lookup@llookup_before_drop_down_state~skipped@relation_operator_account_0~equals@account_1~%s@llookup_before_drop_down_base_folder~journal_ledger@relation_operator_transaction_date_time_0~between@from_transaction_date_time_0~%s 00:00:00@to_transaction_date_time_0~%s",
		login_name,
		application_name,
		session_key,
		role_name,
		account_name,
		transaction_begin_date_string,
		transaction_date_time_closing );

	return strdup( action_string );
}

void account_list_action_string_set(
			LIST *account_statement_list,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *transaction_begin_date_string,
			char *transaction_date_time_closing )
{
	ACCOUNT *account;

	if ( !list_rewind( account_statement_list ) ) return;

	do {
		account = list_get( account_statement_list );

		if ( account->transaction_count )
		{
			account->action_string =
				account_action_string(
					application_name,
					session_key,
					login_name,
					role_name,
					transaction_begin_date_string,
					transaction_date_time_closing,
					account->account_name );
		}

	} while ( list_next( account_statement_list ) );
}

void account_prior_year_set(
			ACCOUNT *prior_account /* in/out */,
			ACCOUNT *current_account )
{
	if ( !prior_account
	||   !prior_account->account_journal_latest
	||   !current_account
	||   !current_account->account_journal_latest )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	prior_account->delta_prior =
		float_delta_prior(
			prior_account->account_journal_latest->balance,
			current_account->account_journal_latest->balance );
}

void account_list_prior_year_set(
			LIST *prior_account_list /* in/out */,
			LIST *current_account_list )
{
	ACCOUNT *current_account;
	ACCOUNT *prior_account;

	if ( list_rewind( current_account_list ) ) return;

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
			current_account->delta_prior = 100;

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

		account_prior_year_set(
			prior_account /* in/out */,
			current_account );

	} while ( list_next( current_account_list ) );
}

double account_list_debit_sum(
			LIST *account_list,
			boolean element_accumulate_debit )
{
	ACCOUNT *account;
	double debit_sum;

	if ( !list_rewind( account_list ) ) return 0.0;

	debit_sum = 0.0;

	do {
		account = list_get( account_list );

		if ( !account->account_journal_latest )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: account_journal_latest is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
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
	double credit_sum;

	if ( !list_rewind( account_list ) ) return 0.0;

	credit_sum = 0.0;

	do {
		account = list_get( account_list );

		if ( !account->account_journal_latest )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: account_journal_latest is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
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
	double balance_total;

	if ( !list_rewind( account_list ) ) return 0.0;

	balance_total = 0.0;

	do {
		account = list_get( account_list );

		if ( !account->account_journal_latest )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: account_journal_latest is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		balance_total += account->account_journal_latest->balance;

	} while ( list_next( account_list ) );

	return balance_total;
}

ACCOUNT *account_getset(
			LIST *account_list,
			char *account_name )
{
	ACCOUNT *account;

	if ( !account_list )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty account_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ( account = account_seek( account_name, account_list ) ) )
	{
		return account;
	}

	account = account_new( strdup( account_name ) );
	list_set( account_list, account );
	return account;
}

LIST *account_cash_name_list(
			char *account_table,
			char *subclassification_cash )
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

	return pipe2list( system_string );
}

LIST *account_current_liability_name_list(
			char *account_table,
			char *subclassification_current_liability,
			char *account_uncleared_checks )
{
	char system_string[ 256 ];
	char where[ 128 ];

	sprintf(where,
		"subclassification = '%s' and account <> '%s'",
		subclassification_current_liability,
		account_uncleared_checks );

	sprintf(system_string,
		"select.sh account %s \"%s\"",
		account_table,
		where );

	return pipe2list( system_string );
}

LIST *account_receivable_name_list(
			char *account_table,
			char *subclassification_receivable )
{
	char system_string[ 256 ];
	char where[ 128 ];

	sprintf(where,
		"subclassification = '%s'",
		subclassification_receivable );

	sprintf(system_string,
		"select.sh account %s \"%s\"",
		account_table,
		where );

	return pipe2list( system_string );
}

LIST *account_list(	char *where,
			boolean fetch_subclassification,
			boolean fetch_element )
{
	if ( !where ) where = "1 = 1";

	return
	account_system_list(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		account_system_string(
			ACCOUNT_SELECT,
			ACCOUNT_TABLE,
			where ),
		fetch_subclassification,
		fetch_element );
}

LIST *account_system_list(
			char *system_string,
			boolean fetch_subclassification,
			boolean fetch_element )
{
	LIST *system_list;
	char input[ 1024 ];
	FILE *pipe;

	system_list = list_new();
	pipe = account_pipe( system_string );

	while( string_input( input, pipe, 1024 ) )
	{
		list_set(
			system_list,
			account_parse(
				input,
				fetch_subclassification,
				fetch_element ) );
	}

	pclose( pipe );

	return system_list;
}

char *account_system_string(
			char *select,
			char *table,
			char *where )
{
	char system_string[ 1024 ];

	if ( !select
	||   !table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !where ) where = "1 = 1";

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" account",
		select,
		table,
		where );

	return strdup( system_string );
}

double account_list_sum( LIST *account_statement_list )
{
	ACCOUNT *account;
	double list_sum;

	if ( !list_rewind( account_statement_list ) ) return 0.0;

	list_sum = 0.0;

	do {
		account = list_get( account_statement_list );

		if ( !account->account_journal_latest )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: account_journal_latest is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
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

	if ( !list_rewind( account_statement_list ) ) return;

	do {
		account = list_get( account_statement_list );

		if ( !account->account_journal_latest )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: account_journal_latest is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
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

ACCOUNT *account_element_account_seek(
			char *account_name,
			LIST *element_statement_list )
{
	ELEMENT *element;

	if ( !list_rewind( element_statement_list ) )
		return (ACCOUNT *)0;

	do {
		element = list_get( element_statement_list );

		if ( list_length( element->subclassification_statement_list ) )
		{
			return
			account_subclassification_account_seek(
				account_name,
				element->subclassification_statement_list );
		}

	} while ( list_next( element_statement_list ) );

	return (ACCOUNT *)0;
}

ACCOUNT *account_subclassification_account_seek(
			char *account_name,
			LIST *subclassification_statement_list )
{
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;

	if ( !list_rewind( subclassification_statement_list ) )
		return (ACCOUNT *)0;

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

	return (ACCOUNT *)0;
}

ACCOUNT_JOURNAL *account_journal_latest(
			char *account_name,
			char *transaction_date_time_closing,
			boolean fetch_transaction )
{
	ACCOUNT_JOURNAL *account_journal;
	JOURNAL *journal;

	if ( !account_name
	||   !transaction_date_time_closing )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( journal =
			journal_latest(
				account_name,
				transaction_date_time_closing,
				fetch_transaction ) ) )
	{
		return (ACCOUNT_JOURNAL *)0;
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

	if ( ! ( account_journal = calloc( 1, sizeof( ACCOUNT_JOURNAL ) ) ) )
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

