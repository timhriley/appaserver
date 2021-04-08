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
#include "html_table.h"
#include "sql.h"
#include "boolean.h"
#include "entity.h"
#include "predictive.h"
#include "element.h"
#include "journal.h"
#include "subclassification.h"
#include "element.h"
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

	if ( ( account = account_seek( account_list, account_name ) ) )
	{
		return account;
	}

	account = account_new( strdup( account_name ) );
	list_set( account_list, account );
	return account;
}

ACCOUNT *account_seek(	LIST *account_list,
			char *account_name )
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

double account_list_html_output(
			HTML_TABLE *html_table,
			LIST *account_list,
			char *element_name,
			boolean element_accumulate_debit,
			double percent_denominator )
{
	double total_element = 0.0;
	ACCOUNT *account;
	char buffer[ 128 ];
	char format_buffer[ 128 ];
	char element_title[ 128 ];
	double latest_journal_balance;
	double percent_of_total;

	if ( !list_length( account_list ) ) return 0.0;

	if ( !html_table )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty html_table.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf(	element_title,
			"<h2>%s</h2>",
			format_initial_capital(
				format_buffer,
				element_name ) );

	html_table_set_data(
			html_table->data_list,
			element_title );

	html_table_output_data(
		html_table->data_list,
		html_table->
			number_left_justified_columns,
		html_table->
			number_right_justified_columns,
		html_table->background_shaded,
		html_table->justify_list );

	html_table->data_list = list_new();

	list_rewind( account_list );
	do {
		account = list_get( account_list );

		if ( !account->latest_journal
		||   !account->latest_journal->balance )
			continue;

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

		html_table_set_data(
			html_table->data_list,
			strdup( format_initial_capital(
				buffer,
				account->account_name ) ) );
	
		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money(
				   latest_journal_balance ) ) );

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

			html_table_set_data(
				html_table->data_list,
				strdup( "" ) );

			html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );
		}

		html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new();

		total_element += latest_journal_balance;

	} while( list_next( account_list ) );

	if ( !timlib_double_virtually_same( total_element, 0.0 ) )
	{
		sprintf(element_title,
			"<h2>Total %s</h2>",
			element_name );

		html_table_set_data(	html_table->data_list,
					element_title );
	
		html_table_set_data( html_table->data_list, strdup( "" ) );

		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money(
				total_element ) ) );

		if ( percent_denominator )
		{
			char buffer[ 128 ];

			percent_of_total =
				( total_element /
				  percent_denominator ) * 100.0;

			sprintf( buffer,
				 "%.1lf%c",
				 percent_of_total,
				 '%' );

			html_table_set_data(
				html_table->data_list,
				strdup( buffer ) );
		}

		html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

		html_table->data_list = list_new();
	}

	return total_element;
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

	latex_append_column_data_list(
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

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( format_initial_capital(
					format_buffer,
					account->
					    account_name ) ),
			0 /* not large_bold */ );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( place_commas_in_money(
			   	     latest_journal_balance ) ),
			0 /* not large_bold */ );

		/* Blank space for the element column. */
		/* ----------------------------------- */
		latex_append_column_data_list(
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

			latex_append_column_data_list(
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

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( total_element_label ),
			0 /* not large_bold */ );

		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( "" ),
			0 /* not large_bold */ );

		latex_append_column_data_list(
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

			latex_append_column_data_list(
				latex_row->column_data_list,
				strdup( buffer ),
				0 /* not large_bold */ );
		}

		/* Blank line */
		/* ---------- */
		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		latex_append_column_data_list(
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

/*
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 account_select(),
		 ACCOUNT_TABLE_NAME,
		 where,
		 "account" );
*/

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

