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
#include "entity.h"
#include "predictive.h"
#include "element.h"
#include "journal.h"
#include "subclassification.h"
#include "element.h"
#include "statement.h"
#include "account.h"

LIST *account_list( void )
{
	return account_list_fetch( "1 = 1" );
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

char *account_name_escape(
			char *account_name )
{
	return account_escape_name( account_name );
}

char *account_name_format(
			char *account_name )
{
	static char name_format[ 256 ];

	return format_initial_capital( name_format, account_name );
}

char *account_escape_name(
			char *account_name )
{
	static char escape_name[ 256 ];

	string_escape_quote( escape_name, account_name );
	return escape_name;
}

char *account_revenue( char *fund_name )
{
	return account_hard_coded_account_name(
			fund_name,
			ACCOUNT_REVENUE_KEY,
			0 /* not  warning_only */,
			__FUNCTION__ );
}

char *account_receivable( char *fund_name )
{
	char *account_name;

	account_name =
		account_hard_coded_account_name(
			fund_name,
			ACCOUNT_RECEIVABLE_KEY,
			0 /* not  warning_only */,
			__FUNCTION__ );

	return account_name;
}

char *account_payable( char *fund_name )
{
	return account_hard_coded_account_name(
			fund_name,
			ACCOUNT_PAYABLE_KEY,
			0 /* not warning_only */,
			__FUNCTION__ );
}

char *account_uncleared_checks( char *fund_name )
{
	return account_hard_coded_account_name(
			fund_name,
			ACCOUNT_UNCLEARED_CHECKS_KEY,
			0 /* not  warning_only */,
			__FUNCTION__ );
}

char *account_gain( char *fund_name )
{
	return account_hard_coded_account_name(
			fund_name,
			ACCOUNT_GAIN_KEY,
			0 /* not  warning_only */,
			__FUNCTION__ );
}

char *account_loss( char *fund_name )
{
	return account_hard_coded_account_name(
			fund_name,
			ACCOUNT_LOSS_KEY,
			0 /* not  warning_only */,
			__FUNCTION__ );
}

char *account_sales_tax_payable( char *fund_name )
{
	return account_hard_coded_account_name(
			fund_name,
			ACCOUNT_SALES_TAX_PAYABLE_KEY,
			0 /* not  warning_only */,
			__FUNCTION__ );
}

char *account_depreciation_expense( char *fund_name )
{
	return account_hard_coded_account_name(
			fund_name,
			ACCOUNT_DEPRECIATION_KEY,
			0 /* not  warning_only */,
			__FUNCTION__ );
}

char *account_accumulated_depreciation( char *fund_name )
{
	return account_hard_coded_account_name(
			fund_name,
			ACCOUNT_ACCUMULATED_KEY,
			0 /* not  warning_only */,
			__FUNCTION__ );
}


char *account_shipping_revenue( char *fund_name )
{
	return account_hard_coded_account_name(
			fund_name,
			ACCOUNT_SHIPPING_REVENUE_KEY,
			0 /* not  warning_only */,
			__FUNCTION__ );
}

char *account_cash( char *fund_name )
{
	return account_hard_coded_account_name(
			fund_name,
			ACCOUNT_CASH_KEY,
			0 /* not  warning_only */,
			__FUNCTION__ );
}

char *account_fees_expense( char *fund_name )
{
	return account_hard_coded_account_name(
			fund_name,
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

int account_balance_match_function(
			ACCOUNT *account_from_list,
			ACCOUNT *account_compare )
{
	if ( !account_from_list->latest_journal
	||   !account_from_list->latest_journal->balance )
	{
		return 1;
	}

	if ( !account_compare->latest_journal
	||   !account_compare->latest_journal->balance )
	{
		return 1;
	}

	/* Sort descending */
	/* --------------- */
	if (	account_from_list->latest_journal->balance <=
		account_compare->latest_journal->balance )
	{
		return 1;
	}
	else
	{
		return -1;
	}
}

ACCOUNT *account_getset(
			LIST *account_list,
			char *account_name )
{
	ACCOUNT *account;

	if ( ( account = account_seek( account_name, account_list ) ) )
	{
		return account;
	}

	account = account_new( strdup( account_name ) );
	list_set( account_list, account );
	return account;
}

ACCOUNT *account_seek(	
			char *account_name,
			LIST *account_list )
{
	ACCOUNT *account;

	if ( !list_rewind( account_list ) ) return (ACCOUNT *)0;

	do {
		account = list_get( account_list );

		if ( strcmp( account->account_name, account_name ) == 0 )
			return account;
	} while ( list_next( account_list ) );

	return (ACCOUNT *)0;
}

char *account_select( void )
{
	if ( predictive_fund_attribute_exists() )
	{
		return
			"account,"
			"subclassification,"
			"hard_coded_account_key,"
			"chart_account_number,"
			"annual_budget,"
			"fund";
	}
	else
	{
		return
			"account,"
			"subclassification,"
			"hard_coded_account_key,"
			"chart_account_number,"
			"annual_budget";
	}
}

ACCOUNT *account_parse( char *input )
{
	char account_name[ 128 ];
	char piece_buffer[ 1024 ];
	ACCOUNT *account;

	if ( !input || !*input ) return (ACCOUNT *)0;

	piece( account_name, SQL_DELIMITER, input, 0 );

	account =
		account_new(
			strdup( account_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	account->subclassification_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	account->account_key = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 3 );
	account->chart_account_number = atoi( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 4 );
	account->annual_budget = atof( piece_buffer );

	if ( predictive_fund_attribute_exists() )
	{
		piece( piece_buffer, SQL_DELIMITER, input, 5 );
		account->fund_name = strdup( piece_buffer );
	}

	account->element_name =
		account_element_name(
			account->subclassification_name );

	account->accumulate_debit =
		account_accumulate_debit(
			account->subclassification_name );

	return account;
}

ACCOUNT *account_fetch(	char *account_name )
{
	if ( !account_name ) return (ACCOUNT *)0;

	return	account_parse(
			pipe2string(
				account_system_string(
		 			/* -------------------------- */
		 			/* Safely returns heap memory */
		 			/* -------------------------- */
		 			account_primary_where(
						account_name ) ) ) );
}

char *account_primary_where(
			char *account_name )
{
	char where[ 128 ];

	sprintf( where,
		 "account = '%s'",
		 account_escape_name( account_name ) );

	return strdup( where );
}

char *account_hard_coded_account_name(
			char *fund_name,
			char *account_key,
			boolean warning_only,
			const char *calling_function_name )
{
	static LIST *list = {0};
	ACCOUNT *account;

	if ( !list )
	{
		list = account_list();
	}

	if ( ! ( account =
			account_key_seek(
				list,
				fund_name,
				account_key ) ) )
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
			LIST *account_list,
			char *fund_name,
			char *account_key )
{
	ACCOUNT *account;

	if ( !list_rewind( account_list ) ) return (ACCOUNT *)0;

	do {
		account = list_get( account_list );

		if ( fund_name
		&&   *fund_name
		&&   strcmp( fund_name, "fund" ) != 0 )
		{
			if ( timlib_strcmp(
				account->fund_name,
				fund_name ) == 0
			&&   string_exists_substr(
				account->account_key,
				account_key ) == 0 )
			{
				return account;
			}
		}
		else
		{
			if ( string_exists_substr(
				account->account_key,
				account_key ) )
			{
				return account;
			}
		}
	} while( list_next( account_list ) );

	return (ACCOUNT *)0;
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
				(char *)0 /* fund_name */,
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

ACCOUNT *account_key_fetch( char *account_key )
{
	char where[ 128 ];

	sprintf(where,
		"hard_coded_account_key = '%s'",
		account_key );

	return account_parse(
			pipe2string(
				account_system_string(
					where ) ) );
}

LIST *account_list_fetch( char *where )
{
	char *sys_string;

	sys_string = account_system_string( where );
	return account_system_list( sys_string );
}

char *account_system_string( char *where )
{
	char system_string[ 1024 ];

	sprintf( system_string,
		 "select.sh '%s' %s \"%s\" select",
		 /* ---------------------- */
		 /* Returns program memory */
		 /* ---------------------- */
		 account_select(),
		 ACCOUNT_TABLE_NAME,
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
"/cgi-bin/post_prompt_edit_form?%s^%s^%s^journal_ledger^%s^lookup^prompt^edit_frame^0^lookup_option_radio_button~lookup@llookup_before_drop_down_state~skipped@relation_operator_account_0~equals@account_1~%s@llookup_before_drop_down_base_folder~journal_ledger@relation_operator_transaction_date_time_0~between@from_transaction_date_time_0~%s 00:00:00@to_transaction_date_time_0~%s",
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

void account_delta_prior_set(
			LIST *prior_account_list,
			ACCOUNT *account )
{
	ACCOUNT *prior_account;

	if ( !account->latest_journal ) return;

	if ( ! ( prior_account =
			account_seek(
				account->account_name,
				prior_account_list ) ) )
	{
		return;
	}

	if ( !prior_account->latest_journal ) return;

	prior_account->delta_prior =
		statement_delta_prior(
			prior_account->latest_journal->balance,
			account->latest_journal->balance );
}

void account_list_delta_prior_set(
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
}

double account_debit_total(
			LIST *account_list )
{
	ACCOUNT *account;
	double total;

	if ( !list_rewind( account_list ) ) return 0.0;

	total = 0.0;

	do {
		account = list_get( account_list );

		if ( !account->latest_journal ) continue;
		if ( !account->latest_journal->balance ) continue;

		if ( !account->accumulate_debit
		&&   account->latest_journal->balance < 0.0 )
		{
			total += -account->latest_journal->balance;
		}
		else
		if ( account->accumulate_debit
		&&   account->latest_journal->balance > 0.0 )
		{
			total += account->latest_journal->balance;
		}

	} while ( list_next( account_list ) );

	return total;
}

double account_credit_total(
			LIST *account_list )
{
	ACCOUNT *account;
	double total;

	if ( !list_rewind( account_list ) ) return 0.0;

	total = 0.0;

	do {
		account = list_get( account_list );

		if ( !account->latest_journal ) continue;
		if ( !account->latest_journal->balance ) continue;

		if ( account->accumulate_debit
		&&   account->latest_journal->balance < 0.0 )
		{
			total += -account->latest_journal->balance;
		}
		else
		if ( !account->accumulate_debit
		&&   account->latest_journal->balance > 0.0 )
		{
			total += account->latest_journal->balance;
		}

	} while ( list_next( account_list ) );

	return total;
}

double account_list_balance(
			LIST *account_list )
{
	ACCOUNT *account;
	double balance;

	if ( !list_rewind( account_list ) ) return 0.0;

	balance = 0.0;

	do {
		account = list_get( account_list );

		if ( !account->latest_journal ) continue;
		if ( !account->latest_journal->balance ) continue;

		account->account_balance =
			account->latest_journal->balance;

		balance += account->account_balance;

	} while ( list_next( account_list ) );

	return balance;
}

void account_list_percent_of_asset_set(
			LIST *account_list,
			double asset_total )
{
	ACCOUNT *account;

	if ( !asset_total ) return;
	if ( !list_rewind( account_list ) ) return;

	do {
		account = list_get( account_list );

		account->percent_of_asset =
			element_percent_of_total(
				account->account_balance,
				asset_total );

	} while ( list_next( account_list ) );
}

void account_list_percent_of_revenue_set(
			LIST *account_list,
			double revenue_total )
{
	ACCOUNT *account;

	if ( !revenue_total ) return;
	if ( !list_rewind( account_list ) ) return;

	do {
		account = list_get( account_list );

		account->percent_of_revenue =
			element_percent_of_total(
				account->account_balance,
				revenue_total );

	} while ( list_next( account_list ) );
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
	ACCOUNT *account;

	if ( ! ( account =
			account_fetch(
				account_name ) ) )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: account_fetch(%s) returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 account_name );
		return 0;
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


