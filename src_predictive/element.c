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
					&element->element_total,
					element->element_name,
					fund_name,
					transaction_date_time_closing );
		}

		if ( fetch_subclassification_list )
		{
			element->subclassification_list =
				element_subclassification_list(
					&element->element_total,
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
			LIST *element_list,
			char *element_name )
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

double element_value(	LIST *subclassification_list,
			boolean element_accumulate_debit )
{
	double total_element = 0.0;
	ACCOUNT *account;
	SUBCLASSIFICATION *subclassification;
	double latest_journal_balance;

	if ( !list_rewind( subclassification_list ) ) return 0.0;
	
	do {
		subclassification = list_get( subclassification_list );

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		do {
			account =
				list_get(
					subclassification->account_list );

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

			total_element += latest_journal_balance;

		} while( list_next( subclassification->account_list ) );

	} while( list_next( subclassification_list ) );

	return total_element;
}

double element_subclassification_aggregate_html_output(
			HTML_TABLE *html_table,
			LIST *subclassification_list,
			char *element_name,
			double percent_denominator )
{
	double total_element = 0.0;
	char buffer[ 128 ];
	char format_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;
	char element_title[ 128 ];
	boolean first_time = 1;
	double percent_of_total;

	/* For equity, always display the element title */
	/* -------------------------------------------- */
	if ( strcmp(	element_name,
			ELEMENT_EQUITY ) ==  0 )
	{
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

		first_time = 0;

		if ( !subclassification_net_assets_exists(
				subclassification_list ) )
		{
			goto equity_all_done;
		}

		total_element =
			subclassification_net_assets_html_output(
				html_table,
				subclassification_list,
				0 /* not element_accumulate_debit */ );

		first_time = 0;
	}

equity_all_done:

	if ( !list_rewind( subclassification_list ) ) return 0.0;

	do {
		subclassification = list_get( subclassification_list );

		if ( timlib_dollar_virtually_same(
			subclassification->subclassification_total,
			0.0 ) )
		{
			continue;
		}

		/* Don't display net assets. */
		/* ------------------------- */
		if ( strcmp(
			subclassification->
				subclassification_name,
			SUBCLASSIFICATION_NET_ASSETS ) == 0 )
		{
			continue;
		}

		if ( first_time )
		{
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

			first_time = 0;
		}

		total_element += subclassification->subclassification_total;

		sprintf(buffer,
		 	"%s",
		 	format_initial_capital(
				format_buffer,
		 		subclassification->
				    subclassification_name ) );

		html_table_set_data(	html_table->data_list,
					strdup( buffer ) );

		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money(
				subclassification->
					subclassification_total ) ) );
	
		if ( percent_denominator )
		{
			char buffer[ 128 ];

			html_table_set_data(
				html_table->data_list,
				strdup( "" ) );

			percent_of_total =
				( subclassification->
					subclassification_total /
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

	} while( list_next( subclassification_list ) );

	if ( !timlib_double_virtually_same( total_element, 0.0 ) )
	{
		if ( strcmp(
			subclassification->
				subclassification_name,
			SUBCLASSIFICATION_CHANGE_IN_NET_ASSETS ) == 0 )
		{
			sprintf(element_title,
				"<h2>%s</h2>",
				"Equity Ending Balance" );
		}
		else
		{
			sprintf(element_title,
				"<h2>Total %s</h2>",
				element_name );
		}

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
		latex_append_column_data_list(
			latex_row->column_data_list,
			"Change in Net Assets",
			1 /* not large_bold */ );
	}
	else
	{
		latex_append_column_data_list(
			latex_row->column_data_list,
			"Net Income",
			1 /* not large_bold */ );
	}

	if ( !omit_subclassification )
	{
		latex_append_column_data_list(
			latex_row->column_data_list,
			"",
			0 /* not large_bold */ );
	}

	latex_append_column_data_list(
		latex_row->column_data_list,
		"",
		0 /* not large_bold */ );

	latex_append_column_data_list(
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

		latex_append_column_data_list(
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
		latex_append_column_data_list(
			latex_row->column_data_list,
			"Change in Net Assets",
			1 /* not large_bold */ );
	}
	else
	{
		latex_append_column_data_list(
			latex_row->column_data_list,
			"Net Income",
			1 /* not large_bold */ );
	}

	latex_append_column_data_list(
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

		latex_append_column_data_list(
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

	latex_append_column_data_list(
		latex_row->column_data_list,
		strdup( "Liabilities Plus Equity" ),
		1 /* large_bold */ );

	while ( skip_columns-- )
	{
		latex_append_column_data_list(
			latex_row->column_data_list,
			strdup( "" ),
			0 /* not large_bold */ );
	}

	latex_append_column_data_list(
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

ACCOUNT *element_list_account_seek(
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

void element_set_account_action_string(
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
		subclassification_list_set_account_action_string(
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
		account_list_set_action_string(
			element->account_list,
			application_name,
			session,
			login_name,
			role_name,
			beginning_date,
			as_of_date );
	}
}

void element_list_set_account_action_string(
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

		element_set_account_action_string(
			element,
			application_name,
			session,
			login_name,
			role_name,
			beginning_date,
			as_of_date );

	} while ( list_next( element_list ) );
}

int element_delta_prior(
			double prior_total,
			double total )
{
	double difference;
	double delta;

	if ( !prior_total ) return 0;

	difference = total - prior_total;

	delta = (difference / prior_total) * 100.0;

	return float_round_int( delta );
}

void element_prior_year_element_list_set_delta_prior(
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
		element_delta_prior(
			prior_year_element->element_total
				/* prior_total */,
			preclose_element->element_total
				/* total */ );

	if ( list_length( prior_year_element->subclassification_list ) )
	{
		subclassification_list_set_delta_prior(
			prior_year_element->subclassification_list,
			preclose_element->subclassification_list );
	}
	else
	if ( list_length( prior_year_element->account_list ) )
	{
		account_list_set_delta_prior(
			prior_year_element->account_list,
			preclose_element->account_list );
	}
}

void element_list_set_delta_prior(
			LIST *prior_year_element_list,
			LIST *preclose_element_list )
{
	ELEMENT *preclose_element;

	if ( !list_rewind( preclose_element_list ) ) return;

	do {
		preclose_element = list_get( preclose_element_list );

		element_prior_year_element_list_set_delta_prior(
			prior_year_element_list,
			preclose_element );

	} while ( list_next( preclose_element_list ) );
}

double element_list_debit_total(
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
				subclassification_list_debit_total(
					element->subclassification_list );
		}
		else
		if ( list_length( element->account_list ) )
		{
			total +=
				account_list_debit_total(
					element->account_list );
		}

	} while ( list_next( element_list ) );

	return total;
}

double element_list_credit_total(
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
				subclassification_list_credit_total(
					element->subclassification_list );
		}
		else
		if ( list_length( element->account_list ) )
		{
			total +=
				account_list_credit_total(
					element->account_list );
		}

	} while ( list_next( element_list ) );

	return total;
}

void element_list_set_total(
			LIST *element_list )
{
	ELEMENT *element;

	if ( !list_rewind( element_list ) ) return;

	do {
		element = list_get( element_list );

		element->element_total =
			element_total(
				element->subclassification_list,
				element->account_list );

	} while ( list_next( element_list ) );
}

double element_total(	LIST *subclassification_list,
			LIST *account_list )
{
	if ( list_length( subclassification_list ) )
	{
		return
			subclassification_list_total(
				subclassification_list );
	}
	else
	if ( list_length( account_list ) )
	{
		return
			account_list_total(
				account_list );
	}
	else
	{
		return 0.0;
	}
}

void element_list_set_percent_of_assets(
			LIST *element_list )
{
	ELEMENT *denominator_element;
	double assets_total = {0};

	if ( ( denominator_element =
			element_seek(
				ELEMENT_ASSET,
				element_list ) ) )
	{
		assets_total = denominator_element->element_total;
	}

	if ( !assets_total ) return;

	element_denominator_set_percent_of_assets(
		element_list,
		assets_total );
}

void element_list_set_percent_of_revenues(
			LIST *element_list )
{
	ELEMENT *denominator_element;
	double revenues_total = {0};

	if ( ( denominator_element =
			element_seek(
				ELEMENT_REVENUE,
				element_list ) ) )
	{
		revenues_total = denominator_element->element_total;
	}

	if ( !revenues_total ) return;

	element_denominator_set_percent_of_revenues(
		element_list,
		revenues_total );
}

void element_denominator_set_percent_of_assets(
			LIST *element_list,
			double assets_total )
{
	ELEMENT *element;
	double percent_of_assets;

	if ( !assets_total ) return;
	if ( !list_rewind( element_list ) ) return;

	do {
		element = list_get( element_list );

		percent_of_assets =
			(element->element_total /
			 assets_total) * 100.0;

		element->percent_of_assets =
			float_round_int(
				percent_of_assets );

		if ( list_length( element->subclassification_list ) )
		{
			subclassification_list_set_percent_of_assets(
				element->subclassification_list,
				assets_total );
		}
		else
		if ( list_length( element->account_list ) )
		{
			account_list_set_percent_of_assets(
				element->account_list,
				assets_total );
		}

	} while ( list_next( element_list ) );
}

void element_denominator_set_percent_of_revenues(
			LIST *element_list,
			double revenues_total )
{
	ELEMENT *element;
	double percent_of_revenues;

	if ( !revenues_total ) return;
	if ( !list_rewind( element_list ) ) return;

	do {
		element = list_get( element_list );

		percent_of_revenues =
			(element->element_total /
			 revenues_total) * 100.0;

		element->percent_of_revenues =
			float_round_int(
				percent_of_revenues );

		if ( list_length( element->subclassification_list ) )
		{
			subclassification_list_set_percent_of_revenues(
				element->subclassification_list,
				revenues_total );
		}
		else
		if ( list_length( element->account_list ) )
		{
			account_list_set_percent_of_revenues(
				element->account_list,
				revenues_total );
		}

	} while ( list_next( element_list ) );
}

