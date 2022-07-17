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
	JOURNAL *from_list_latest_journal;
	JOURNAL *compare_latest_journal;

	from_list_latest_journal =
		list_last(
			account_from_list->
				journal_account_journal_list );

	compare_latest_journal =
		list_last(
			account_compare->
				journal_account_journal_list );

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

double account_balance( LIST *journal_account_journal_list )
{
	JOURNAL *latest_journal;

	if ( ! ( latest_journal =
			journal_list_latest(
				journal_account_journal_list ) ) )
	{
		return 0.0;
	}

	return latest_journal->balance;
}


LIST *account_list_percent_of_asset_set(
			LIST *account_list,
			double asset_total )
{
	ACCOUNT *account;

	if ( !asset_total ) return account_list;

	if ( !list_rewind( account_list ) ) return account_list;

	do {
		account = list_get( account_list );

		account->percent_of_asset =
			float_percent_of_total(
				account_balance(
					account->journal_account_journal_list ),
				asset_total );

	} while ( list_next( account_list ) );

	return account_list;
}

LIST *account_list_percent_of_revenue_set(
			LIST *account_list,
			double revenue_total )
{
	ACCOUNT *account;

	if ( !revenue_total ) return account_list;

	if ( !list_rewind( account_list ) ) return account_list;

	do {
		account = list_get( account_list );

		account->percent_of_revenue =
			float_percent_of_total(
				account_balance(
					account->journal_account_journal_list ),
				revenue_total );

	} while ( list_next( account_list ) );

	return account_list;
}

ACCOUNT *account_subclassification_fetch(
			char *account_name,
			char *begin_transaction_date_time,
			char *end_transaction_date_time )
{
	ACCOUNT *account;

	if ( !account_name
	||   !begin_transaction_date_time
	||   !end_transaction_date_time )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	account = account_new( account_name );

	account->journal_account_journal_list =
		journal_account_journal_list(
			account_name,
			begin_transaction_date_time,
			end_transaction_date_time );

	if ( !list_length( account->journal_account_journal_list ) )
	{
		free( account );
		return (ACCOUNT *)0;
	}

	return account;
}

boolean account_accumulate_debit(
			char *account_name,
			boolean expect_lots )
{
	if ( expect_lots )
	{
		static LIST *list = {0};
		ACCOUNT *account;

		if ( !list )
		{
			list =
				account_list(
					"1 = 1" /* where */,
					1 /* fetch_subclassification */,
					1 /* fetch_entity */ );
		}

		if ( ! ( account =
				account_seek(
					account_name,
					list ) ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: account_seek(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				account_name );
			exit( 1 );
		}

		if ( !account->subclassification
		||   !account->subclassification->element )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: element is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		return
		account->
			subclassification->
			element->
			accumulate_debit;
	}
	else
	{
		ACCOUNT *account;

		account =
			account_fetch(
				account_name,
				1 /* fetch_subclassification */,
				1 /* fetch_element */ );

		return
		account->
			subclassification->
			element->
			accumulate_debit;
	}
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
			boolean fetch_entity )
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

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	account->subclassification_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	account->hard_coded_account_key = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	account->chart_account_number = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	account->annual_budget = atoi( piece_buffer );

	if ( fetch_subclassification )
	{
		account->subclassification =
			subclassification_fetch(
				account->subclassification_name,
				fetch_entity );
	}

	return account;
}

ACCOUNT *account_fetch(	char *account_name,
			boolean fetch_subclassification,
			boolean fetch_entity )
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
		fetch_entity );
}

char *account_primary_where(
			char *account_name )
{
	static char where[ 128 ];

	sprintf(where,
		"account = '%s'",
		account_name );

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
			char *beginning_date,
			char *as_of_date,
			char *account_name )
{
	char action_string[ 2048 ];

	sprintf( action_string,
"/cgi-bin/post_prompt_edit_form?%s^%s^%s^journal_ledger^%s^lookup^prompt^edit_frame^0^lookup_option_radio_button~lookup@llookup_before_drop_down_state~skipped@relation_operator_account_0~equals@account_1~%s@llookup_before_drop_down_base_folder~journal_ledger@relation_operator_transaction_date_time_0~between@from_transaction_date_time_0~%s 00:00:00@to_transaction_date_time_0~%s 23:59:59",
		 login_name,
		 application_name,
		 session_key,
		 role_name,
		 account_name,
		 beginning_date,
		 as_of_date );

	return strdup( action_string );
}

void account_list_action_string_set(
			LIST *account_list,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *beginning_date,
			char *as_of_date )
{
	ACCOUNT *account;

	if ( !list_rewind( account_list ) ) return;

	do {
		account = list_get( account_list );

		account->action_string =
			account_action_string(
				application_name,
				session_key,
				login_name,
				role_name,
				beginning_date,
				as_of_date,
				account->account_name );

	} while ( list_next( account_list ) );
}

int account_delta_prior(
			double prior_account_latest_journal_balance,
			double account_latest_journal_balance )
{
	return
	float_delta_prior(
		prior_account_latest_journal_balance,
		account_latest_journal_balance );
}

void account_delta_prior_set(
			LIST *prior_account_list,
			ACCOUNT *account )
{
	ACCOUNT *prior_account;
	JOURNAL *prior_latest_journal;
	JOURNAL *latest_journal;

	if ( ! ( prior_account =
			account_seek(
				account->account_name,
				prior_account_list ) ) )
	{
		return;
	}

	if ( ! ( prior_latest_journal =
			journal_list_latest(
				prior_account->
					journal_account_journal_list ) ) )
	{
		return;
	}

	if ( ! ( latest_journal =
			journal_list_latest(
				account->
					journal_account_journal_list ) ) )
	{
		return;
	}

	prior_account->delta_prior =
		account_delta_prior(
			prior_latest_journal->balance,
			latest_journal->balance );
}

LIST *account_list_delta_prior_set(
			LIST *prior_account_list,
			LIST *account_list )
{
	ACCOUNT *account;

	if ( !list_rewind( account_list ) ) return prior_account_list;

	do {
		account = list_get( account_list );

		account_delta_prior_set(
			prior_account_list,
			account );

	} while ( list_next( account_list ) );

	return prior_account_list;
}

double account_debit_total( LIST *account_list )
{
	ACCOUNT *account;
	JOURNAL *latest_journal;
	double debit_total;

	if ( !list_rewind( account_list ) ) return 0.0;

	debit_total = 0.0;

	do {
		account = list_get( account_list );

		if ( ! ( latest_journal =
				journal_list_latest(
					account->
					     journal_account_journal_list ) ) )
		{
			continue;
		}

		if ( !account_accumulate_debit(
			account->account_name,
			1 /* expect_lots */ )
		&&   latest_journal->balance < 0.0 )
		{
			debit_total += -latest_journal->balance;
		}
		else
		if ( account_accumulate_debit(
			account->account_name,
			1 /* expect_lots */ )
		&&   latest_journal->balance > 0.0 )
		{
			debit_total += latest_journal->balance;
		}

	} while ( list_next( account_list ) );

	return debit_total;
}

double account_credit_total( LIST *account_list )
{
	ACCOUNT *account;
	JOURNAL *latest_journal;
	double credit_total;

	if ( !list_rewind( account_list ) ) return 0.0;

	credit_total = 0.0;

	do {
		account = list_get( account_list );

		if ( ! ( latest_journal =
				journal_list_latest(
					account->
					     journal_account_journal_list ) ) )
		{
			continue;
		}

		if ( account_accumulate_debit(
			account->account_name,
			1 /* expect_lots */ )
		&&   latest_journal->balance < 0.0 )
		{
			credit_total += -latest_journal->balance;
		}
		else
		if ( !account_accumulate_debit(
			account->account_name,
			1 /* expect_lots */ )
		&&   latest_journal->balance > 0.0 )
		{
			credit_total += latest_journal->balance;
		}

	} while ( list_next( account_list ) );

	return credit_total;
}

double account_balance_total( LIST *account_list )
{
	ACCOUNT *account;
	JOURNAL *latest_journal;
	double balance_total;

	if ( !list_rewind( account_list ) ) return 0.0;

	balance_total = 0.0;

	do {
		account = list_get( account_list );

		if ( ! ( latest_journal =
				journal_list_latest(
					account->
					     journal_account_journal_list ) ) )
		{
			continue;
		}

		balance_total += latest_journal->balance;

	} while ( list_next( account_list ) );

	return balance_total;
}

LIST *account_following_balance_zero_journal_list( char *account_name )
{
	return journal_following_balance_zero_list( account_name );
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

LIST *account_subclassification_account_name_list(
			char *account_table,
			char *subclassification_primary_where )
{
	if ( !account_table
	||   !subclassification_primary_where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	pipe2list(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		account_subclassification_account_system_string(
			account_table,
			subclassification_primary_where ) );
}

char *account_subclassification_account_system_string(
			char *account_table,
			char *subclassification_primary_where )
{
	char system_string[ 1024 ];

	if ( !account_table
	||   !subclassification_primary_where )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"select.sh account %s \"%s\" none",
		account_table,
		subclassification_primary_where );

	return strdup( system_string );
}

LIST *account_list(	char *where,
			boolean fetch_subclassification,
			boolean fetch_entity )
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
		fetch_entity );
}

LIST *account_system_list(
			char *system_string,
			boolean fetch_subclassification,
			boolean fetch_element )
{
	LIST *system_list;
	char input[ 1024 ];
	FILE *input_pipe;

	system_list = list_new();
	input_pipe = popen( system_string, "r" );

	while( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			system_list,
			account_parse(
				input,
				fetch_subclassification,
				fetch_element ) );
	}

	pclose( input_pipe );

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

