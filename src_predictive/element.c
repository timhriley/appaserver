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
#include "sql.h"
#include "String.h"
#include "piece.h"
#include "element.h"

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

	if ( piece( piece_buffer, SQL_DELIMITER, input, 1 ) )
	{
		element->accumulate_debit = ( *piece_buffer == 'y' );
	}

	return element;
}

ELEMENT *element_fetch( char *element_name )
{
	char input[ 128 ];
	ELEMENT *element;
	FILE *input_pipe;

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	input_pipe =
		element_input_pipe(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			element_system_string(
				ELEMENT_SELECT,
				ELEMENT_TABLE,
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				element_primary_where(
					element_name ) ) );

	element =
		element_parse(
			string_input(
				input,
				input_pipe,
				128 ) );

	pclose( input_pipe );

	return element;
}

char *element_primary_where( char *element_name )
{
	static char where[ 128 ];

	if ( !element_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: element_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf( where, "element = '%s'", element_name );

	return where;
}

char *element_system_string(
			char *element_select,
			char *element_table,
			char *where )
{
	char system_string[ 1024 ];

	if ( !element_select
	||   !element_table )
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
		"select.sh \"%s\" %s \"%s\"",
		element_select,
		element_table,
		where );

	return strdup( system_string );
}

ELEMENT *element_statement_parse(
			char *input,
			char *transaction_date_time_closing,
			boolean fetch_subclassification_list,
			boolean fetch_account_list,
			boolean fetch_latest_journal )
{
	char element_name[ 128 ];
	char piece_buffer[ 16 ];
	ELEMENT *element;

	if ( !input ) return (ELEMENT *)0;

	element = element_parse( input );

	if ( fetch_subclassification_list )
	{
		element->subclassification_list =
			subclassification_statement_list(
				element_primary_where(
					element->element_name ),
				transaction_date_time_closing,
				fetch_account_list,
				fetch_latest_journal );
	}

	return element;
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

ELEMENT *element_seek(	char *element_name,
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

LIST *element_statement_list(
			LIST *filter_element_name_list,
			char *transaction_date_time_closing,
			boolean fetch_subclassification_list,
			boolean fetch_account_list,
			boolean fetch_latest_journal )
{
	LIST *element_list;
	char *element_name;

	if ( !list_rewind( filter_element_name_list ) ) return (LIST *)0;

	do {
		element_name = list_get( filter_element_name_list );

		list_set(
			element_list,
			element_statement_fetch(
				element_name,
				transaction_date_time_closing,
				fetch_subclassification_list,
				fetch_account_list,
				fetch_latest_journal ) );

	} while ( list_next( filter_element_name_list ) );

	return element_list;
}

double element_value(	ELEMENT *element;
			boolean accumulate_debit )
{
	if ( !list_length( element->subclassification_statement_list ) )
	{
		return 0.0;
	}

	return
	subclassification_statement_list_value(
		element->subclassification_statement_list );
}

#ifdef NOT_DEFINED
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
			account_balance_total( account_list );
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

#endif

FILE *element_input_pipe( char *element_system_string )
{
	return
	popen( element_system_string, "r" );
}

