/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/element.c			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "list.h"
#include "boolean.h"
#include "String.h"
#include "piece.h"
#include "subclassification.h"
#include "element.h"

LIST *element_subclassification_list(
			char *element_name,
			char *begin_transaction_date_time,
			char *end_transaction_date_time )
{
	LIST *subclassification_name_list;
	char *subclassification_name;
	LIST *list;

	subclassification_name_list =
		subclassification_element_name_list(
			element_primary_where(
				element_name ),
			SUBCLASSIFICATION_TABLE,
			"display_order" /* order_column */ );

	if ( !list_rewind( name_list ) ) return (LIST *)0;

	list = list_new();

	do {
		subclassification_name =
			list_get(
				subclassification_name_list );

		list_set(
			list,
			subclassification_element_fetch(
				subclassification_name,
				begin_transaction_date_time,
				end_transaction_date_time ) );

	} while ( list_next( subclassification_name_list ) );

	return list;
}

char *element_primary_where( char *element_name )
{
	static char where[ 128 ];

	sprintf(where,
		"element = '%s'",
		element_name );

	return where;
}

ELEMENT *element_calloc( void )
{
	ELEMENT *element;

	if ( ! ( element = calloc( 1, sizeof( ELEMENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return element;
}

ELEMENT *element_new( char *element_name )
{
	ELEMENT *element = element_calloc();

	element->element_name = element_name;

	return element;
}

ELEMENT *element_parse( char *input )
{
	char element_name[ 128 ];
	char piece_buffer[ 16 ];
	ELEMENT *element;

	if ( !input ) return (ELEMENT *)0;

	/* See ELEMENT_SELECT */
	/* ------------------ */
	piece( element_name, SQL_DELIMITER, input, 0 );
	element = element_new( strdup( element_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	element->accumulate_debit = ( *piece_buffer == 'y' );

	return element;
}

#ifdef NOT_DEFINED
ELEMENT *element_parse(
			char *input,
			char *fund_name,
			char *transaction_date_time_nominal,
			char *transaction_date_time_fixed,
			boolean fetch_subclassification_list,
			boolean fetch_account_list )
{
	char element_name[ 128 ];
	char piece_buffer[ 16 ];
	ELEMENT *element;
	char *transaction_date_time;

	if ( !input ) return (ELEMENT *)0;

	piece( element_name, SQL_DELIMITER, input, 0 );
	element = element_new( strdup( element_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	element->accumulate_debit = ( *piece_buffer == 'y' );

	if ( element_is_nominal( element->element_name ) )
	{
		transaction_date_time =
			transaction_date_time_nominal;
	}
	else
	{
		transaction_date_time =
			transaction_date_time_fixed;
	}

	if ( fetch_account_list )
	{
		element->account_list =
			element_account_list(
				&element->element_current_balance,
				element->element_name,
				fund_name,
				transaction_date_time );
	}

	if ( fetch_subclassification_list )
	{
		element->subclassification_list =
			element_subclassification_list(
				&element->element_current_balance,
				element->element_name,
				fund_name,
				transaction_date_time );
	}

	return element;
}

char *element_sys_string( char *where )
{
	char sys_string[ 1024 ];

	if ( !where || !*where ) return (char *)0;

	sprintf(sys_string,
		"echo \"select %s from %s where %s;\" | sql",
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		element_select(),
		"element",
		where );

	return strdup( sys_string );
}

ELEMENT *element_fetch( char *element_name )
{
	static LIST *list = {0};
	ELEMENT *element;

	if ( !list )
	{
		list =
			element_fetch_list(
				element_sys_string(
					"1 = 1" ) );
	}

	if ( ! ( element = element_seek( element_name, list ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: element_seek(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__, __LINE__,
			element_name );
		exit( 1 );
	}

	return element;
}

boolean element_accumulate_debit( char *element_name )
{
	char sys_string[ 1024 ];
	char where [128 ];
	char *results;

	sprintf( where, "element = '%s'", element_name );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 "accumulate_debit_yn",
		 "element",
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		return 0;
	}

	if ( *results == 'y' )
		return 1;
	else
		return 0;
}

LIST *element_fetch_list( char *sys_string )
{
	LIST *element_list;
	ELEMENT *element;
	char input_buffer[ 256 ];
	char element_name[ 128 ];
	char accumulate_debit_yn[ 2 ];
	FILE *input_pipe;

	element_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while( string_input( input_buffer, input_pipe, 256 ) )
	{
		piece( element_name, SQL_DELIMITER, input_buffer, 0 );

		element =
			element_new(
				strdup( element_name ) );

		piece(	accumulate_debit_yn,
			SQL_DELIMITER,
			input_buffer,
			1 );

		element->accumulate_debit = ( *accumulate_debit_yn == 'y' );

		list_set( element_list, element );
	}

	pclose( input_pipe );
	return element_list;
}

LIST *element_system_list(
			char *system_string,
			char *fund_name,
			char *transaction_date_time_nominal,
			char *transaction_date_time_fixed,
			boolean fetch_subclassification_list,
			boolean fetch_account_list )
{
	LIST *element_list;
	char input[ 256 ];
	FILE *input_pipe;

	element_list = list_new();
	input_pipe = popen( system_string, "r" );

	while( string_input( input, input_pipe, 256 ) )
	{
		list_set(
			element_list,
			element_parse(
				input,
				fund_name,
				transaction_date_time_nominal,
				transaction_date_time_fixed,
				fetch_subclassification_list,
				fetch_account_list ) );
	}

	pclose( input_pipe );
	return element_list_sort( element_list );
}

char *element_system_string( char *where )
{
	char system_string[ 1024 ];

	if ( !where || !*where ) where = "1 = 1";

	sprintf(system_string,
		"select.sh \"%s\" element \"%s\"",
		element_select(),
		where );

	return strdup( system_string );
}

char *element_filter_where(
			LIST *filter_element_name_list )
{
	char where[ 1024 ];

	if ( !list_length( filter_element_name_list ) )
	{
		strcpy( where, "1 = 1" );
	}
	else
	{
		char *ptr = timlib_in_clause( filter_element_name_list );

		sprintf(where,
			"element in (%s)",
			ptr );
		free( ptr );
	}

	return strdup( where );
}

LIST *element_list(	LIST *filter_element_name_list,
			char *fund_name,
			char *transaction_date_time_nominal,
			char *transaction_date_time_fixed,
			boolean fetch_subclassification_list,
			boolean fetch_account_list )
{
	LIST *list;

	list = element_system_list(
			element_system_string(
				element_filter_where(
					filter_element_name_list ) ),
			fund_name,
			transaction_date_time_nominal,
			transaction_date_time_fixed,
			fetch_subclassification_list,
			fetch_account_list );

	return list;
}

LIST *element_subclassification_list(
			double *element_current_balance,
			char *element_name,
			char *fund_name,
			char *transaction_date_time_closing )
{
	LIST *subclassification_list;
	SUBCLASSIFICATION *subclassification;
	char sys_string[ 1024 ];
	char where[ 256 ];
	char subclassification_name[ 128 ];
	FILE *input_pipe;

	*element_current_balance = 0.0;

	sprintf( where, "element = '%s'", element_name );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s order by %s;\" | sql",
		 "subclassification",
		 "subclassification",
		 where,
		 "display_order" );

	subclassification_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while( string_input( subclassification_name, input_pipe, 128 ) )
	{
		subclassification =
			subclassification_new(
				strdup( subclassification_name ) );

		subclassification->account_list =
			subclassification_total_account_list(
				&subclassification->subclassification_balance,
				subclassification->subclassification_name,
				fund_name,
				transaction_date_time_closing );

		*element_current_balance +=
			subclassification->
				subclassification_balance;

		list_set(	subclassification_list,
				subclassification );
	}

	pclose( input_pipe );
	return subclassification_list;
}

LIST *element_list_sort( LIST *element_list )
{
	ELEMENT *element;
	LIST *return_element_list;

	return_element_list = list_new();

	if ( ( element =
		element_seek(
			ELEMENT_ASSET,
			element_list ) ) )
	{
		list_set( return_element_list, element );
	}

	if ( ( element =
		element_seek(
			ELEMENT_LIABILITY,
			element_list ) ) )
	{
		list_set( return_element_list, element );
	}

	if ( ( element =
		element_seek(
			ELEMENT_REVENUE,
			element_list ) ) )
	{
		list_set( return_element_list, element );
	}

	if ( ( element =
		element_seek(
			ELEMENT_EXPENSE,
			element_list ) ) )
	{
		list_set( return_element_list, element );
	}

	if ( ( element =
		element_seek(
			ELEMENT_GAIN,
			element_list ) ) )
	{
		list_set( return_element_list, element );
	}

	if ( ( element =
		element_seek(
			ELEMENT_LOSS,
			element_list ) ) )
	{
		list_set( return_element_list, element );
	}

	if ( ( element =
		element_seek(
			ELEMENT_EQUITY,
			element_list ) ) )
	{
		list_set( return_element_list, element );
	}

	return return_element_list;
}

ELEMENT *element_seek(	
			char *element_name,
			LIST *element_list )
{
	ELEMENT *element;

	if ( !list_rewind( element_list ) ) return (ELEMENT *)0;

	do {
		element = list_get( element_list );

		if ( strcmp( element->element_name, element_name ) == 0 )
		{
			return element;
		}

	} while( list_next( element_list ) );

	return (ELEMENT *)0;
}

LIST *element_account_list(
			double *element_total,
			char *element_name,
			char *fund_name,
			char *date_time_string )
{
	ACCOUNT *account;
	char sys_string[ 1024 ];
	char where[ 256 ];
	char account_name[ 128 ];
	FILE *input_pipe;
	char *folder;
	LIST *account_list;
	char *fund_where;
	JOURNAL *latest_journal;
	char local_date_time_string[ 32 ];

	if ( character_exists( date_time_string, ' ' ) )
	{
		strcpy( local_date_time_string, date_time_string );
	}
	else
	{
		sprintf(local_date_time_string,
			"%s %s",
			date_time_string,
			TRANSACTION_CLOSING_TRANSACTION_TIME );
	}

	fund_where =
		predictive_fund_where(
			fund_name );

	*element_total = 0.0;

	folder = "account,subclassification";

	sprintf(	where,
			"%s and					"
			"element = '%s' and			"
			"account.subclassification =		"
			"subclassification.subclassification	",
			fund_where,
			element_name );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\" | sql",
		 "account",
		 folder,
		 where );

	account_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while( get_line( account_name, input_pipe ) )
	{
		latest_journal =
			journal_latest(
				account_name,
				local_date_time_string );

		if ( !latest_journal
		||   timlib_double_virtually_same(
			latest_journal->balance,
			0.0 ) )
		{
			continue;
		}

		account = account_fetch( strdup( account_name ) );

		/* Change account name from stack memory to heap. */
		/* ---------------------------------------------- */
		latest_journal->account_name = account->account_name;

		account->latest_journal = latest_journal;

		*element_total += account->latest_journal->balance;

		list_add_pointer_in_order(
			account_list,
			account,
			account_balance_match_function );
	}

	pclose( input_pipe );
	return account_list;
}

boolean element_is_period( char *element_name )
{
	return element_is_nominal( element_name );
}

boolean element_is_nominal( char *element_name )
{
	if ( !element_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty element_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( strcmp(	element_name,
			ELEMENT_REVENUE ) == 0 )
		return 1;
	else
	if ( strcmp(	element_name,
			ELEMENT_EXPENSE ) == 0 )
		return 1;
	else
	if ( strcmp(	element_name,
			ELEMENT_GAIN ) == 0 )
		return 1;
	else
	if ( strcmp(	element_name,
			ELEMENT_LOSS ) == 0 )
		return 1;
	else
		return 0;
}

double element_value(	LIST *account_list,
			boolean element_accumulate_debit )
{
	double value = 0.0;
	ACCOUNT *account;
	double latest_journal_balance;

	if ( !list_rewind( account_list ) ) return 0.0;
	
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

		value += latest_journal_balance;

	} while( list_next( account_list ) );

	return value;
}

LATEX_ROW *element_latex_net_income_row(
			double net_income,
			boolean is_statement_of_activities,
			double percent_denominator,
			boolean omit_subclassification )
{
	LATEX_ROW *latex_row;

	latex_row = latex_new_latex_row();

	if ( is_statement_of_activities )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"Change in Net Assets",
			1 /* not large_bold */ );
	}
	else
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"Net Income",
			1 /* not large_bold */ );
	}

	if ( !omit_subclassification )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"",
			0 /* not large_bold */ );
	}

	latex_column_data_set(
		latex_row->column_data_list,
		"",
		0 /* not large_bold */ );

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( place_commas_in_money(
			   net_income ) ),
		0 /* not large_bold */ );

	if ( percent_denominator )
	{
		char buffer[ 128 ];
		double percent_of_total;

		percent_of_total =
			( net_income /
	  		percent_denominator ) * 100.0;

		sprintf(buffer,
	 		"%.1lf%c",
	 		percent_of_total,
	 		'%' );

		latex_column_data_set(
			latex_row->column_data_list,
			strdup( buffer ),
			0 /* not large_bold */ );
	}

	return latex_row;
}

LATEX_ROW *element_latex_subclassification_aggregate_net_income_row(
			double net_income,
			boolean is_statement_of_activities,
			double percent_denominator )
{
	LATEX_ROW *latex_row;

	latex_row = latex_new_latex_row();

	if ( is_statement_of_activities )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"Change in Net Assets",
			1 /* not large_bold */ );
	}
	else
	{
		latex_column_data_set(
			latex_row->column_data_list,
			"Net Income",
			1 /* not large_bold */ );
	}

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( place_commas_in_money(
			   net_income ) ),
		0 /* not large_bold */ );

	if ( percent_denominator )
	{
		char buffer[ 128 ];
		double percent_of_total;

		percent_of_total =
			( net_income /
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
	return latex_row;
}

LATEX_ROW *element_latex_liabilities_plus_equity_row(
			double liabilities_plus_equity,
			int skip_columns )
{
	LATEX_ROW *latex_row;

	latex_row = latex_new_latex_row();

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( "Liabilities Plus Equity" ),
		1 /* large_bold */ );

	while ( skip_columns-- )
	{
		latex_column_data_set(
			latex_row->column_data_list,
			strdup( "" ),
			0 /* not large_bold */ );
	}

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( place_commas_in_money(
			   liabilities_plus_equity ) ),
		0 /* not large_bold */ );

	return latex_row;
}

void element_list_propagate(
			LIST *element_list,
			char *transaction_date_time_string )
{
	ELEMENT *element;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;

	if ( !list_rewind( element_list ) ) return;

	do {
		element = list_get( element_list );

		if ( !list_rewind( element->subclassification_list ) ) continue;

		do {
			subclassification =
				list_get(
					element->
						subclassification_list );

			if ( !list_rewind( subclassification->account_list ) )
				continue;

			do {
				account =
					list_get( 
						subclassification->
							account_list );

				account_propagate(
					account->account_name,
					transaction_date_time_string );

			} while( list_next( subclassification->account_list ) );
		} while( list_next( element->subclassification_list ) );
	} while( list_next( element_list ) );
}

ACCOUNT *element_account_seek(
			char *account_name,
			LIST *element_list )
{
	ELEMENT *element;
	SUBCLASSIFICATION *subclassification;
	ACCOUNT *account;

	if ( !list_rewind( element_list ) ) return (ACCOUNT *)0;

	do {
		element = list_get( element_list );

		if ( list_length( element->account_list ) )
		{
			if ( ( account =
				account_seek(
					account_name,
					element->account_list ) ) )
			{
				return account;
			}
		}

		if ( !list_rewind( element->subclassification_list ) )
			continue;

		do {
			subclassification =
				list_get(
					element->
						subclassification_list );

			if ( !list_rewind( subclassification->account_list ) )
				continue;

			if ( ( account =
				account_seek(
					account_name,
					subclassification->account_list ) ) )
			{
				return account;
			}

		} while( list_next( element->subclassification_list ) );
	} while( list_next( element_list ) );

	return (ACCOUNT *)0;
}

SUBCLASSIFICATION *element_subclassification_seek(
			char *subclassification_name,
			LIST *element_list )
{
	ELEMENT *element;
	SUBCLASSIFICATION *subclassification;

	if ( !list_rewind( element_list ) ) return (SUBCLASSIFICATION *)0;

	do {
		element = list_get( element_list );

		if ( !list_rewind( element->subclassification_list ) )
			continue;

		do {
			subclassification =
				list_get(
					element->
						subclassification_list );

			if ( strcmp(
				subclassification->subclassification_name,
				subclassification_name ) == 0 )
			{
				return subclassification;
			}

		} while( list_next( element->subclassification_list ) );
	} while ( list_next( element_list ) );

	return (SUBCLASSIFICATION *)0;
}

void element_account_action_string_set(
			ELEMENT *element,
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *beginning_date,
			char *as_of_date )
{
	if ( list_length( element->subclassification_list ) )
	{
		subclassification_list_account_action_string_set(
			element->subclassification_list,
			application_name,
			session,
			login_name,
			role_name,
			beginning_date,
			as_of_date );
	}
	else
	if ( list_length( element->account_list ) )
	{
		account_list_action_string_set(
			element->account_list,
			application_name,
			session,
			login_name,
			role_name,
			beginning_date,
			as_of_date );
	}
}

void element_list_account_action_string_set(
			LIST *element_list,
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *beginning_date,
			char *as_of_date )
{
	ELEMENT *element;

	if ( !list_rewind( element_list ) ) return;

	do {
		element = list_get( element_list );

		element_account_action_string_set(
			element,
			application_name,
			session,
			login_name,
			role_name,
			beginning_date,
			as_of_date );

	} while ( list_next( element_list ) );
}

void element_prior_year_element_list_delta_prior_set(
			LIST *prior_year_element_list,
			ELEMENT *preclose_element )
{
	ELEMENT *prior_year_element;

	if ( ! ( prior_year_element =
			element_seek(
				preclose_element->element_name,
				prior_year_element_list ) ) )
	{
		return;
	}

	prior_year_element->element_delta_prior =
		statement_delta_prior(
			prior_year_element->element_current_balance
				/* prior_total */,
			preclose_element->element_current_balance
				/* total */ );

	if ( list_length( prior_year_element->subclassification_list ) )
	{
		subclassification_list_delta_prior_set(
			prior_year_element->subclassification_list,
			preclose_element->subclassification_list );
	}
	else
	if ( list_length( prior_year_element->account_list ) )
	{
		account_list_delta_prior_set(
			prior_year_element->account_list,
			preclose_element->account_list );
	}
}

void element_list_delta_prior_set(
			LIST *prior_year_element_list,
			LIST *preclose_element_list )
{
	ELEMENT *preclose_element;

	if ( !list_rewind( preclose_element_list ) ) return;

	do {
		preclose_element = list_get( preclose_element_list );

		element_prior_year_element_list_delta_prior_set(
			prior_year_element_list,
			preclose_element );

	} while ( list_next( preclose_element_list ) );
}

double element_debit_total(
			LIST *element_list )
{
	ELEMENT *element;
	double total;

	if ( !list_rewind( element_list ) ) return 0.0;

	total = 0.0;

	do {
		element = list_get( element_list );

		if ( list_length( element->subclassification_list ) )
		{
			total +=
				subclassification_debit_total(
					element->subclassification_list );
		}
		else
		if ( list_length( element->account_list ) )
		{
			total +=
				account_debit_total(
					element->account_list );
		}

	} while ( list_next( element_list ) );

	return total;
}

double element_credit_total(
			LIST *element_list )
{
	ELEMENT *element;
	double total;

	if ( !list_rewind( element_list ) ) return 0.0;

	total = 0.0;

	do {
		element = list_get( element_list );

		if ( list_length( element->subclassification_list ) )
		{
			total +=
				subclassification_credit_total(
					element->subclassification_list );
		}
		else
		if ( list_length( element->account_list ) )
		{
			total +=
				account_credit_total(
					element->account_list );
		}

	} while ( list_next( element_list ) );

	return total;
}

void element_list_current_balance(
			LIST *element_list,
			double equity_net_income )
{
	ELEMENT *element;

	if ( !list_rewind( element_list ) ) return;

	do {
		element = list_get( element_list );

		element->element_current_balance =
			element_current_balance(
				element->subclassification_list,
				element->account_list );

		if ( strcmp(	element->element_name,
				ELEMENT_EQUITY ) == 0
		&&   equity_net_income )
		{
			element->element_current_balance +=
				equity_net_income;
		}

	} while ( list_next( element_list ) );
}

double element_positive_balance_total(
			LIST *subclassification_list,
			LIST *account_list )
{
	if ( list_length( subclassification_list ) )
	{
		return
			subclassification_positive_balance_total(
				subclassification_list );
	}
	else
	if ( list_length( account_list ) )
	{
		return
			account_positive_balance_total(
				account_list );
	}
	else
	{
		return 0.0;
	}
}

double element_current_balance(
			LIST *subclassification_list,
			LIST *account_list )
{
	if ( list_length( subclassification_list ) )
	{
		return
			subclassification_list_balance(
				subclassification_list );
	}
	else
	if ( list_length( account_list ) )
	{
		return
			account_list_balance(
				account_list );
	}
	else
	{
		return 0.0;
	}
}

void element_list_percent_of_asset_set(
			LIST *element_list )
{
	ELEMENT *denominator_element;
	double asset_total = {0};

	if ( ( denominator_element =
			element_seek(
				ELEMENT_ASSET,
				element_list ) ) )
	{
		asset_total =
			element_positive_balance_total(
				denominator_element->
					subclassification_list,
				denominator_element->
					account_list );
	}

	if ( !asset_total ) return;

	element_denominator_percent_of_asset_set(
		element_list,
		asset_total );
}

void element_list_percent_of_revenue_set(
			LIST *element_list )
{
	ELEMENT *denominator_element;
	double revenue_total = {0};

	if ( ( denominator_element =
			element_seek(
				ELEMENT_REVENUE,
				element_list ) ) )
	{
		revenue_total =
			element_positive_balance_total(
				denominator_element->
					subclassification_list,
				denominator_element->
					account_list );
	}

	if ( !revenue_total ) return;

	element_denominator_percent_of_revenue_set(
		element_list,
		revenue_total );
}

void element_denominator_percent_of_asset_set(
			LIST *element_list,
			double asset_total )
{
	ELEMENT *element;

	if ( !asset_total ) return;
	if ( !list_rewind( element_list ) ) return;

	do {
		element = list_get( element_list );

		element->percent_of_asset =
			element_percent_of_total(
				element->element_current_balance,
				asset_total );

		if ( list_length( element->subclassification_list ) )
		{
			subclassification_list_percent_of_asset_set(
				element->subclassification_list,
				asset_total );
		}
		else
		if ( list_length( element->account_list ) )
		{
			account_list_percent_of_asset_set(
				element->account_list,
				asset_total );
		}

	} while ( list_next( element_list ) );
}

void element_denominator_percent_of_revenue_set(
			LIST *element_list,
			double revenue_total )
{
	ELEMENT *element;

	if ( !revenue_total ) return;
	if ( !list_rewind( element_list ) ) return;

	do {
		element = list_get( element_list );

		element->percent_of_revenue =
			element_percent_of_total(
				element->element_current_balance,
				revenue_total );

		if ( list_length( element->subclassification_list ) )
		{
			subclassification_list_percent_of_revenue_set(
				element->subclassification_list,
				revenue_total );
		}
		else
		if ( list_length( element->account_list ) )
		{
			account_list_percent_of_revenue_set(
				element->account_list,
				revenue_total );
		}

	} while ( list_next( element_list ) );
}

int element_percent_of_total(
			double total,
			double denominator )
{
	double percent;

	if ( !denominator ) return 0;

	percent =
		(total /
		 denominator) * 100.0;

	return float_round_int( percent );
}

EQUITY_ELEMENT *equity_element_new(
			char *element_name,
			double current_balance,
			char *fund_name,
			char *begin_date_string )
{
	EQUITY_ELEMENT *equity_element;

	if ( ! ( equity_element =
			calloc( 1, sizeof( EQUITY_ELEMENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
	}

	equity_element->element_name = element_name;
	equity_element->current_balance = current_balance;
	equity_element->fund_name = fund_name;
	equity_element->begin_date_string = begin_date_string;

	return equity_element;
}

EQUITY_ELEMENT *equity_element_fetch(
			char *element_name,
			double current_balance,
			char *fund_name,
			char *begin_date_string )
{
	EQUITY_ELEMENT *equity_element;
	DATE *prior;

	equity_element =
		equity_element_new(
			element_name,
			current_balance,
			fund_name,
			begin_date_string );

	prior =
		/* ------------------- */
		/* Trims trailing time */
		/* ------------------- */
		date_yyyy_mm_dd_new( begin_date_string );

	if ( !prior )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: date_yyyy_mm_dd_new(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			begin_date_string );

		exit( 1 );
	}

	date_decrement_second( prior, 1 );

	equity_element->prior_transaction_date_time =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		date_display19( prior );

	equity_element->prior_element_account_list =
		element_account_list(
			&equity_element->prior_balance,
			equity_element->element_name,
			equity_element->fund_name,
			equity_element->prior_transaction_date_time );

	return equity_element;
}

double equity_element_balance_change(
			double prior_balance,
			double current_balance )
{
	return	current_balance - prior_balance;
}

char *element_list_display(
			LIST *element_list )
{
	char display[ 65536 ];
	char *ptr = display;
	ELEMENT *element;

	*ptr = '\0';

	if ( list_rewind( element_list ) )
	{
		do {
			element = list_get( element_list );

			if ( !element->element_current_balance )
				continue;

			ptr += sprintf(
				ptr,
				"Element: %s, balance = %.2lf\n",
				element->element_name,
				element->element_current_balance );

		} while ( list_next( element_list ) );
	}

	return strdup( display );
}
#endif
