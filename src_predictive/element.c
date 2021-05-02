/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/element.c			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "list.h"
#include "float.h"
#include "sql.h"
#include "piece.h"
#include "environ.h"
#include "html_table.h"
#include "boolean.h"
#include "account.h"
#include "subclassification.h"
#include "predictive.h"
#include "statement.h"
#include "element.h"

char *element_select( void )
{
	return "element.element,accumulate_debit_yn";
}

ELEMENT *element_parse(	char *input )
{
	char element_name[ 128 ];
	char piece_buffer[ 16 ];
	ELEMENT *element;

	if ( !input ) return (ELEMENT *)0;

	piece( element_name, SQL_DELIMITER, input, 0 );
	element = element_new( strdup( element_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	element->accumulate_debit = ( *piece_buffer == 'y' );

	return element;
}

char *element_primary_where( char *element_name )
{
	char where[ 256 ];

	sprintf( where,
		 "element = '%s'",
		 element_name );

	return strdup( where );
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

ELEMENT *element_account_name_fetch(
			char *account_name )
{
	char sys_string[ 1024 ];
	char *from;
	char join[ 256 ];
	char where[ 256 ];

	from = "account,subclassification,element";

	sprintf(join,
	"account.subclassification = subclassification.subclassification and "
	"subclassification.element = element.element " );

	sprintf(where,
		"account = '%s' and			"
		"%s					",
		account_escape_name( account_name ),
		join );

	sprintf(sys_string,
		"echo \"select %s from %s where %s;\" | sql.e",
		/* -------------------------- */
		/* Safely returns heap memory */
		/* -------------------------- */
		element_select(),
		from,
		where );

	return element_parse( pipe2string( sys_string ) );
}

ELEMENT *element_new( char *element_name )
{
	ELEMENT *element;

	if ( ! ( element = calloc( 1, sizeof( ELEMENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	element->element_name = element_name;

	element->accumulate_debit =
		element_accumulate_debit(
			element->element_name );

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
		fprintf( stderr,
		"ERROR in %s/%s()/%d: pipe2string(%s) returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 element_name );
		exit( 1 );
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
			char *sys_string,
			LIST *filter_element_name_list,
			char *fund_name,
			char *transaction_date_time_closing,
			boolean fetch_subclassification_list,
			boolean fetch_account_list )
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

		if ( list_length( filter_element_name_list )
		&&   !list_exists_string(
			element_name,
			filter_element_name_list ) )
		{
			continue;
		}

		element =
			element_new(
				strdup( element_name ) );

		piece(	accumulate_debit_yn,
			SQL_DELIMITER,
			input_buffer,
			1 );

		element->accumulate_debit = ( *accumulate_debit_yn == 'y' );

		if ( fetch_account_list )
		{
			element->account_list =
				element_account_list(
					&element->element_balance_total,
					element->element_name,
					fund_name,
					transaction_date_time_closing );
		}

		if ( fetch_subclassification_list )
		{
			element->subclassification_list =
				element_subclassification_list(
					&element->element_balance_total,
					element->element_name,
					fund_name,
					transaction_date_time_closing );
		}

		list_set( element_list, element );
	}

	pclose( input_pipe );

	return element_list_sort( element_list );
}

LIST *element_list(	LIST *filter_element_name_list,
			char *fund_name,
			char *transaction_date_time_closing,
			boolean fetch_subclassification_list,
			boolean fetch_account_list )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh \"%s\" element",
		 element_select() );

	return element_system_list(
			sys_string,
			filter_element_name_list,
			fund_name,
			transaction_date_time_closing,
			fetch_subclassification_list,
			fetch_account_list );
}

LIST *element_subclassification_list(
			double *element_total,
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

	*element_total = 0.0;

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
				&subclassification->subclassification_total,
				subclassification->subclassification_name,
				fund_name,
				transaction_date_time_closing );

		*element_total += subclassification->subclassification_total;

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

	if ( ( element = element_seek(
				ELEMENT_ASSET,
				element_list ) ) )
	{
		list_set( return_element_list, element );
	}

	if ( ( element = element_seek(
				ELEMENT_LIABILITY,
				element_list ) ) )
	{
		list_set( return_element_list, element );
	}

	if ( ( element = element_seek(
				ELEMENT_REVENUE,
				element_list ) ) )
	{
		list_set( return_element_list, element );
	}

	if ( ( element = element_seek(
				ELEMENT_EXPENSE,
				element_list ) ) )
	{
		list_set( return_element_list, element );
	}

	if ( ( element = element_seek(
				ELEMENT_GAIN,
				element_list ) ) )
	{
		list_set( return_element_list, element );
	}

	if ( ( element = element_seek(
				ELEMENT_LOSS,
				element_list ) ) )
	{
		list_set( return_element_list, element );
	}

	if ( ( element = element_seek(
				ELEMENT_EQUITY,
				element_list ) ) )
	{
		list_set( return_element_list, element );
	}
	return return_element_list;
}

ELEMENT *element_list_seek(
			char *element_name,
			LIST *element_list )
{
	return element_seek( element_name, element_list );
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
			char *transaction_date_time_closing )
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
				transaction_date_time_closing );

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

boolean element_account_accumulate_debit(
			char *account_name )
{
	ELEMENT *element;

	if ( ! ( element =
			element_account_name_fetch(
				account_name ) ) )
	{
		fprintf( stderr,
	"Warning in %s/%s()/%d: cannot fetch element for account = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 account_name );
		return 0;
	}

	return element->accumulate_debit;
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
			prior_year_element->element_balance_total
				/* prior_total */,
			preclose_element->element_balance_total
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

void element_list_balance_total(
			LIST *element_list )
{
	ELEMENT *element;

	if ( !list_rewind( element_list ) ) return;

	do {
		element = list_get( element_list );

		element->element_balance_total =
			element_balance_total(
				element->subclassification_list,
				element->account_list );

	} while ( list_next( element_list ) );
}

double element_balance_total(
			LIST *subclassification_list,
			LIST *account_list )
{
	if ( list_length( subclassification_list ) )
	{
		return
			subclassification_balance_total(
				subclassification_list );
	}
	else
	if ( list_length( account_list ) )
	{
		return
			account_balance_total(
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
		asset_total = denominator_element->element_balance_total;
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
		revenue_total = denominator_element->element_balance_total;
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
	double percent_of_asset;

	if ( !asset_total ) return;
	if ( !list_rewind( element_list ) ) return;

	do {
		element = list_get( element_list );

		percent_of_asset =
			(element->element_balance_total /
			 asset_total) * 100.0;

		element->percent_of_asset =
			float_round_int(
				percent_of_asset );

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
	double percent_of_revenue;

	if ( !revenue_total ) return;
	if ( !list_rewind( element_list ) ) return;

	do {
		element = list_get( element_list );

		percent_of_revenue =
			(element->element_balance_total /
			 revenue_total) * 100.0;

		element->percent_of_revenue =
			float_round_int(
				percent_of_revenue );

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

