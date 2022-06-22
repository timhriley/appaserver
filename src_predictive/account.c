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

char *account_revenue( void )
{
	return
	account_hard_coded_account_name(
		ACCOUNT_REVENUE_KEY,
		0 /* not  warning_only */,
		__FUNCTION__ );
}

char *account_receivable( void )
{
	return
	account_hard_coded_account_name(
		ACCOUNT_RECEIVABLE_KEY,
		0 /* not  warning_only */,
		__FUNCTION__ );
}

char *account_payable( void )
{
	return
	account_hard_coded_account_name(
		ACCOUNT_PAYABLE_KEY,
		0 /* not warning_only */,
		__FUNCTION__ );
}

char *account_uncleared_checks( void )
{
	return
	account_hard_coded_account_name(
		ACCOUNT_UNCLEARED_CHECKS_KEY,
		0 /* not  warning_only */,
		__FUNCTION__ );
}

char *account_gain( void )
{
	return
	account_hard_coded_account_name(
		ACCOUNT_GAIN_KEY,
		0 /* not  warning_only */,
		__FUNCTION__ );
}

char *account_loss( void )
{
	return
	account_hard_coded_account_name(
		ACCOUNT_LOSS_KEY,
		0 /* not  warning_only */,
		__FUNCTION__ );
}

char *account_sales_tax_payable( void )
{
	return
	account_hard_coded_account_name(
		ACCOUNT_SALES_TAX_PAYABLE_KEY,
		0 /* not  warning_only */,
		__FUNCTION__ );
}

char *account_depreciation_expense( void )
{
	return
	account_hard_coded_account_name(
		ACCOUNT_DEPRECIATION_KEY,
		0 /* not  warning_only */,
		__FUNCTION__ );
}

char *account_accumulated_depreciation( void )
{
	return
	account_hard_coded_account_name(
		ACCOUNT_ACCUMULATED_KEY,
		0 /* not  warning_only */,
		__FUNCTION__ );
}


char *account_shipping_revenue( void )
{
	return
	account_hard_coded_account_name(
		ACCOUNT_SHIPPING_REVENUE_KEY,
		0 /* not  warning_only */,
		__FUNCTION__ );
}

char *account_cash( void )
{
	return
	account_hard_coded_account_name(
		ACCOUNT_CASH_KEY,
		0 /* not  warning_only */,
		__FUNCTION__ );
}

char *account_fees_expense( void )
{
	return
	account_hard_coded_account_name(
		ACCOUNT_FEES_EXPENSE_KEY,
		0 /* not  warning_only */,
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

LIST *account_subclassification_account_name_list(
			char *where,
			char *account_table )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh account %s \"%s\"",
		account_table,
		where );

	return pipe2list( system_string );
}

double account_balance( LIST *journal_account_journal_list )
{
	JOURNAL *latest_journal;

	if ( ! ( latest_journal =
			journal_list_latest(
				journal_account_journal_list ) )
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

	if ( !revenue_total ) return;
	if ( !list_rewind( account_list ) ) return;

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

boolean account_accumulate_debit( char *account_name )
{
	ACCOUNT *account;
	static LIST *list = {0};

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

	return account->subclassification->element->accumulate_debit;
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

	sprintf( where,
		 "account = '%s'",
		 account_escape_name( account_name ) );

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
			account->account_key,
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
			char *session,
			char *login_name,
			char *role_name,
			char *beginning_date,
			char *as_of_date,
			char *account_name )
{
	char action_string[ 4096 ];

	sprintf( action_string,
"/cgi-bin/post_prompt_edit_form?%s^%s^%s^journal_ledger^%s^lookup^prompt^edit_frame^0^lookup_option_radio_button~lookup@llookup_before_drop_down_state~skipped@relation_operator_account_0~equals@account_1~%s@llookup_before_drop_down_base_folder~journal_ledger@relation_operator_transaction_date_time_0~between@from_transaction_date_time_0~%s 00:00:00@to_transaction_date_time_0~%s 23:59:59",
		 login_name,
		 application_name,
		 session,
		 role_name,
		 account_name,
		 beginning_date,
		 as_of_date );

	return strdup( action_string );
}

void account_list_action_string_set(
			LIST *account_list,
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *beginning_date,
			char *as_of_date )
{
	ACCOUNT *account;

	if ( !list_rewind( account_list ) ) return;

	do {
		account = list_get( account_list );

		account->account_action_string =
			account_action_string(
				application_name,
				session,
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

	if ( !list_rewind( account_list ) ) return;

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

		if ( !account_accumulate_debit( account->account_name )
		&&   latest_journal->balance < 0.0 )
		{
			debit_total += -latest_journal->balance;
		}
		else
		if ( account_accumulate_debit( account->account_name )
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

		if ( account_accumulate_debit( account->account_name )
		&&   latest_journal->balance < 0.0 )
		{
			credit_total += -latest_journal->balance;
		}
		else
		if ( !account_accumulate_debit( account->account_name )
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
					     journal_account_journal_list ) )
		{
			continue;
		}

		balance_total += account->latest_journal->balance;

	} while ( list_next( account_list ) );

	return balance_total;
}

#ifdef NOT_DEFINED
char *account_escape_name(
			char *account_name )
{
	static char escape_name[ 256 ];

	string_escape_quote( escape_name, account_name );
	return escape_name;
}

char *account_name_escape(
			char *account_name )
{
	return account_escape_name( account_name );
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

char *account_non_cash_account_name(
			LIST *journal_list )
{
	static char *checking_account = {0};
	JOURNAL *journal;

	if ( !checking_account )
	{
		checking_account =
			account_hard_coded_account_name(
				ACCOUNT_CASH_KEY,
				0 /* not warning_only */,
				__FUNCTION__ );
	}

	if ( !list_rewind( journal_list ) )
		return ACCOUNT_NOT_SET;

	do {
		journal = list_get( journal_list );

		if ( string_strcmp(	journal->account_name,
					checking_account ) != 0 )
		{
			return account_name_display(
					journal->account_name );
		}
	} while( list_next( journal_list ) );

	return ACCOUNT_NOT_SET;
}

LIST *account_omit_latex_row_list(
			double *total_element,
			LIST *account_list,
			char *element_name,
			boolean element_accumulate_debit,
			double percent_denominator )
{
	LIST *row_list;
	ACCOUNT *account;
	char total_element_label[ 128 ];
	char format_buffer[ 128 ];
	double latest_journal_balance;
	LATEX_ROW *latex_row;
	double percent_of_total;

	*total_element = 0.0;

	if ( !list_rewind( account_list ) ) return (LIST *)0;

	row_list = list_new();

	latex_row = latex_new_latex_row();
	list_append_pointer( row_list, latex_row );

	sprintf( format_buffer,
		 "\\large \\bf %s",
		 element_name );

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( format_initial_capital(
				format_buffer,
				format_buffer ) ),
		0 /* not large_bold */ );

	sprintf(format_buffer,
	 	"\\large \\bf Total %s",
	 	element_name );

	format_initial_capital( total_element_label, format_buffer );

	do {
		account =
			list_get_pointer( account_list ); 

		if ( !account->latest_journal
		||   !account->latest_journal->balance )
			continue;

		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		if (	element_accumulate_debit ==
				account->accumulate_debit )
		{
			latest_journal_balance =
				account->latest_journal->balance;
		}
		else
		{
			latest_journal_balance =
				0.0 - account->latest_journal->balance;
		}

		latex_column_data_set(
			latex_row->column_data_list,
			strdup( format_initial_capital(
					format_buffer,
					account->
					    account_name ) ),
			0 /* not large_bold */ );

		latex_column_data_set(
			latex_row->column_data_list,
			strdup( place_commas_in_money(
			   	     latest_journal_balance ) ),
			0 /* not large_bold */ );

		/* Blank space for the element column. */
		/* ----------------------------------- */
		latex_column_data_set(
			latex_row->column_data_list,
			strdup( "" ),
			0 /* not large_bold */ );

		if ( percent_denominator )
		{
			char buffer[ 128 ];

			percent_of_total =
				( latest_journal_balance /
		  		  percent_denominator ) * 100.0;

			sprintf( buffer,
		 		"%.1lf%c",
		 		percent_of_total,
		 		'%' );

			latex_column_data_set(
				latex_row->column_data_list,
				strdup( buffer ),
				0 /* not large_bold */ );
		}
	
		*total_element += latest_journal_balance;

	} while( list_next( account_list ) );

	if ( *total_element )
	{
		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		latex_column_data_set(
			latex_row->column_data_list,
			strdup( total_element_label ),
			0 /* not large_bold */ );

		latex_column_data_set(
			latex_row->column_data_list,
			strdup( "" ),
			0 /* not large_bold */ );

		latex_column_data_set(
			latex_row->column_data_list,
			strdup( place_commas_in_money(
				   *total_element ) ),
			0 /* not large_bold */ );

		if ( percent_denominator )
		{
			char buffer[ 128 ];

			percent_of_total =
				( *total_element /
		  		percent_denominator ) * 100.0;

			sprintf( buffer,
		 		"%.1lf%c",
		 		percent_of_total,
		 		'%' );

			latex_column_data_set(
				latex_row->column_data_list,
				strdup( buffer ),
				0 /* not large_bold */ );
		}

		/* Blank line */
		/* ---------- */
		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		latex_column_data_set(
			latex_row->column_data_list,
			strdup( "" ),
			0 /* not large_bold */ );

	} /* if total_element */

	return row_list;
}

void account_propagate( char *account_name,
			char *transaction_date_time )
{
	/* Executes journal_list_set_balances() */
	/* ------------------------------------ */
	journal_propagate(
		transaction_date_time,
		account_name );
}

boolean account_accumulate_debit(
			char *subclassification_name )
{
	SUBCLASSIFICATION *subclassification;
	ELEMENT *element;

	if ( ! ( subclassification =
			subclassification_fetch(
				subclassification_name ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: subclassification_fetch(%s) returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 subclassification_name );
	}

	if ( ! ( element =
			element_fetch(
				subclassification->element_name ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: element_fetch(%s) returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 subclassification->element_name );
	}

	return element->accumulate_debit;
}

void account_transaction_propagate(
			char *propagate_transaction_date_time )
{
	char sys_string[ 1024 ];
	char account[ 256 ];
	FILE *input_pipe;

	sprintf( sys_string,
		 "echo \"select %s from %s;\" | sql",
		 "account",
		 ACCOUNT_TABLE_NAME );

	input_pipe = popen( sys_string, "r" );

	while( string_input( account, input_pipe, 256 ) )
	{
		journal_propagate(
			propagate_transaction_date_time,
			account );
	}
	pclose( input_pipe );
}

char *account_hard_coded_key_where(
			char *hard_coded_account_key )
{
	static char where[ 128 ];

	if ( !hard_coded_account_key )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: hard_coded_account_key is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"hard_coded_account_key = '%s'",
		hard_coded_account_key );

	return where;
}

ACCOUNT *account_key_fetch(
			char *hard_coded_account_key,
			boolean fetch_subclassification,
			boolean fetch_entity )
{
	char where[ 128 ];

	if ( !hard_coded_account_key )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: hard_coded_account_key is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	account_parse(
		string_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			account_system_string(
				ACCOUNT_SELECT,
				ACCOUNT_TABLE,		
				account_hard_coded_key_where(
					hard_coded_account_key )
						/* where */ ) ),
		fetch_subclassification,
		fetch_entity );
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

char *account_system_string(
			char *account_select,
			char *account_table,
			char *where )
{
	char system_string[ 1024 ];

	if ( !account_select
	||   !account_table )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !where ) where = "1 = 1";

	sprintf( system_string,
		 "select.sh \"%s\" %s \"%s\" select",
		 account_select,
		 account_table,
		 where );

	return strdup( system_string );
}

LIST *account_system_list( char *sys_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *account_list;

	input_pipe = popen( sys_string, "r" );
	account_list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set( account_list, account_parse( input ) );
	}
	pclose( input_pipe );
	return account_list;
}

char *account_element_name(
			char *subclassification_name )
{
	SUBCLASSIFICATION *subclassification;

	if ( ! ( subclassification =
			subclassification_fetch(
				subclassification_name ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: subclassification_fetch(%s) returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 subclassification_name );
	}

	return subclassification->element_name;
}

boolean account_name_accumulate_debit(
			char *account_name )
{
	static LIST *account_list = {0};
	ACCOUNT *account;

	if ( !account_list )
	{
		account_list = account_list_fetch( "1 = 1" );
	}

	if ( ! ( account =
			account_seek(
				account_name,
				account_list ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: account_seek(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			account_name );
		exit( 1 );
	}

	return account->accumulate_debit;
}

char *account_list_display(
			LIST *account_list )
{
	char display[ 65536 ];
	char *ptr = display;
	ACCOUNT *account;

	*ptr = '\0';

	if ( list_rewind( account_list ) )
	{
		do {
			account = list_get( account_list );

			if ( !account->latest_journal ) continue;

			ptr += sprintf(
				ptr,
				"Account: %s, account_balance = %.2lf\n",
				account->account_name,
				account->latest_journal->balance );

		} while ( list_next( account_list ) );
	}

	return strdup( display );
}

double account_liability_due( LIST *liability_journal_list )
{
	JOURNAL *journal;
	double amount_due;
	double difference;

	if ( !list_rewind( liability_journal_list ) ) return 0.0;

	amount_due = 0.0;

	do {
		journal = list_get( liability_journal_list );

		difference =	journal->credit_amount -
				journal->debit_amount;

		amount_due += difference;

	} while ( list_next( liability_journal_list ) );

	return amount_due;
}

#endif
