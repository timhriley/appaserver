/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/subclassification.c		*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "list.h"
#include "sql.h"
#include "piece.h"
#include "boolean.h"
#include "latex.h"
#include "html_table.h"
#include "account.h"
#include "predictive.h"
#include "element.h"
#include "statement.h"
#include "subclassification.h"

char *subclassification_primary_where(
			char *subclassification_name )
{
	char where[ 256 ];

	sprintf( where,
		 "subclassification = '%s'",
		 subclassification_name );

	return strdup( where );
}

SUBCLASSIFICATION *subclassification_parse(
			char *input )
{
	char subclassification_name[ 128 ];
	char piece_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;

	if ( !input || !*input ) return (SUBCLASSIFICATION *)0;

	/* See: attribute_list subclassification */
	/* ------------------------------------- */
	piece( subclassification_name, SQL_DELIMITER, input, 0 );

	subclassification =
		subclassification_new(
			strdup( subclassification_name ) );

	piece( piece_buffer, SQL_DELIMITER, input, 1 );
	subclassification->element_name = strdup( piece_buffer );

	piece( piece_buffer, SQL_DELIMITER, input, 2 );
	subclassification->display_order = atoi( piece_buffer );

	return subclassification;
}

SUBCLASSIFICATION *subclassification_fetch(
			char *subclassification_name )
{
	static LIST *list = {0};
	SUBCLASSIFICATION *subclassification;

	if ( !list )
	{
		list = subclassification_list();
	}

	if ( ! ( subclassification =
			subclassification_seek(
				subclassification_name,
				list ) ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: subclassification_seek(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			subclassification_name );
		exit( 1 );
	}

	return subclassification;
}

SUBCLASSIFICATION *subclassification_total_fetch(
			double *subclassification_total,
			char *subclassification_name,
			char *fund_name,
			char *transaction_date_time_closing )
{
	SUBCLASSIFICATION *subclassification;
	char sys_string[ 1024 ];

	if ( !subclassification_name ) return (SUBCLASSIFICATION *)0;

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" none",
		 "subclassification",
		 /* -------------------------- */
		 /* Safely returns heap memory */
		 /* -------------------------- */
		 subclassification_primary_where(
			subclassification_name ) );

	subclassification =
		subclassification_parse(
			pipe2string( sys_string ) );

	subclassification->account_list =
		subclassification_total_account_list(
			subclassification_total,
			subclassification->subclassification_name,
			fund_name,
			transaction_date_time_closing );

	return subclassification;
}

SUBCLASSIFICATION *subclassification_new(
			char *subclassification_name )
{
	SUBCLASSIFICATION *subclassification;

	if ( ! ( subclassification =
			calloc( 1, sizeof( SUBCLASSIFICATION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	subclassification->subclassification_name = subclassification_name;
	return subclassification;
}

double subclassification_html_display(
					HTML_TABLE *html_table,
					LIST *subclassification_list,
					char *element_name,
					boolean element_accumulate_debit,
					double percent_denominator )
{
	double total_element = 0.0;
	double subclassification_amount;
	ACCOUNT *account;
	char buffer[ 128 ];
	char format_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;
	char element_title[ 128 ];
	double latest_journal_balance;
	boolean first_time = 1;
	double percent_of_total;

	if ( !html_table )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty html_table.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

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

		/* Maybe financial_position, so display beginning balance. */
		/* ------------------------------------------------------- */
		total_element =
			subclassification_net_assets_html_output(
				html_table,
				subclassification_list,
				element_accumulate_debit );

		first_time = 0;
	}

	if ( !list_rewind( subclassification_list ) ) return 0.0;

	do {
		subclassification = list_get_pointer( subclassification_list );

		/* Skip net assets. */
		/* ---------------- */
		if ( strcmp(
			subclassification->
				subclassification_name,
			SUBCLASSIFICATION_NET_ASSETS ) == 0 )
		{
			continue;
		}

		if ( !list_rewind( subclassification->account_list ) )
			continue;

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

		/* -------------------------------------------- */
		/* If more than one account,			*/
		/* then display the subclassification name.	*/
		/* -------------------------------------------- */
		if ( list_length( subclassification->account_list ) > 1 )
		{
			html_table->data_list = list_new();

			sprintf(element_title,
				"<h3>%s</h3>",
				format_initial_capital(
					format_buffer,
					subclassification->
						subclassification_name ) );

			html_table_set_data(	html_table->data_list,
						element_title );
	
			html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );
			html_table->data_list = list_new();
		}

		subclassification_amount = 0.0;

		do {
			account =
				list_get_pointer(
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

			/* ---------------------------------------------- */
			/* Don't display single Change in net assets row. */
			/* Display the total row below.			  */
			/* ---------------------------------------------- */
			if ( strcmp(	account->account_name,
					ACCOUNT_CHANGE_IN_NET_ASSETS )
					!= 0 )
			{
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
			}

			total_element += latest_journal_balance;

			subclassification_amount += latest_journal_balance;

		} while( list_next( subclassification->account_list ) );

		if ( strcmp(
			subclassification->
				subclassification_name,
			ACCOUNT_CHANGE_IN_NET_ASSETS ) == 0 )
		{
			sprintf( buffer,
				 "<h3>%s</h3>",
				 "Change in Net Assets" );
		}
		else
		{
			sprintf( buffer,
			 	"<h3>Total %s</h3>",
			 	format_initial_capital(
					format_buffer,
			 		subclassification->
					    subclassification_name ) );
		}

		html_table_set_data(	html_table->data_list,
					strdup( buffer ) );

		html_table_set_data( html_table->data_list, strdup( "" ) );

		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money(
				subclassification_amount ) ) );
	
		if ( percent_denominator )
		{
			char buffer[ 128 ];

			percent_of_total =
				( subclassification_amount /
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

		html_table->data_list = list_new();

	} while( list_next( subclassification_list ) );

	if ( !timlib_double_virtually_same( total_element, 0.0 ) )
	{
		if ( strcmp(
			subclassification->
				subclassification_name,
			ACCOUNT_CHANGE_IN_NET_ASSETS ) == 0 )
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

double subclassification_net_assets_html_output(
			HTML_TABLE *html_table,
			LIST *subclassification_list,
			boolean element_accumulate_debit )
{
	double subclassification_amount;
	ACCOUNT *account;
	char buffer[ 128 ];
	char format_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;
	char element_title[ 128 ];
	double latest_journal_balance;

	if ( !html_table )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty html_table.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( subclassification_list ) ) return 0.0;

	do {
		subclassification = list_get( subclassification_list );

		/* Only display net assets. */
		/* ------------------------ */
		if ( strcmp(
			subclassification->
				subclassification_name,
			SUBCLASSIFICATION_NET_ASSETS ) != 0 )
		{
			continue;
		}

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		if ( list_length( subclassification->account_list ) > 1 )
		{
			html_table->data_list = list_new();

			sprintf(element_title,
				"<h3>%s</h3>",
				format_initial_capital(
					format_buffer,
					subclassification->
						subclassification_name ) );

			html_table_set_data(	html_table->data_list,
						element_title );
	
			html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );
			html_table->data_list = list_new();
		}

		subclassification_amount = 0.0;

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

			html_table_set_data(
				html_table->data_list,
				strdup( format_initial_capital(
					buffer,
					account->account_name ) ) );
	
			html_table_set_data(
				html_table->data_list,
				strdup( place_commas_in_money(
					   latest_journal_balance ) ) );
	
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

			subclassification_amount +=
				latest_journal_balance;

		} while( list_next( subclassification->account_list ) );

		sprintf( buffer,
			 "<h3>%s</h3>",
			 SUBCLASSIFICATION_BEGINNING_BALANCE_LABEL );

		html_table_set_data(	html_table->data_list,
					strdup( buffer ) );
		html_table_set_data( html_table->data_list, strdup( "" ) );

		html_table_set_data(
			html_table->data_list,
			strdup( place_commas_in_money(
				subclassification_amount ) ) );
	
		html_table_output_data(
			html_table->data_list,
			html_table->
				number_left_justified_columns,
			html_table->
				number_right_justified_columns,
			html_table->background_shaded,
			html_table->justify_list );

		html_table->data_list = list_new();

		return subclassification_amount;

	} while( list_next( subclassification_list ) );

	return 0.0;
}

double subclassification_aggregate_html(
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

		if ( !subclassification_net_assets_equity_exists(
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
		subclassification = list_get_pointer( subclassification_list );

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
			ACCOUNT_CHANGE_IN_NET_ASSETS ) == 0 )
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

boolean subclassification_net_assets_equity_exists(
			LIST *subclassification_list )
{
	SUBCLASSIFICATION *subclassification;

	if ( !list_rewind( subclassification_list ) ) return 0;

	do {
		subclassification = list_get_pointer( subclassification_list );

		if ( strcmp(
			subclassification->
				subclassification_name,
			SUBCLASSIFICATION_NET_ASSETS ) == 0 )
		{
			return 1;
		}
	} while( list_next( subclassification_list ) );

	return 0;
}

LATEX_ROW *subclassification_latex_liabilities_plus_equity_row(
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

LIST *subclassification_aggregate_beginning_row_list(
			double *total_element,
			LIST *subclassification_list,
			double percent_denominator )
{
	LIST *row_list;
	SUBCLASSIFICATION *subclassification;
	LATEX_ROW *latex_row;

	if ( !list_rewind( subclassification_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		subclassification = list_get_pointer( subclassification_list );

		/* Only do net assets. */
		/* ------------------- */
		if ( strcmp(
			subclassification->
				subclassification_name,
			SUBCLASSIFICATION_NET_ASSETS ) != 0 )
		{
			continue;
		}

		if ( timlib_dollar_virtually_same(
			subclassification->subclassification_total,
			0.0 ) )
		{
			continue;
		}

		*total_element += subclassification->subclassification_total;

		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		latex_column_data_set(
			latex_row->column_data_list,
			SUBCLASSIFICATION_BEGINNING_BALANCE_LABEL,
			0 /* not large_bold */ );

		latex_column_data_set(
			latex_row->column_data_list,
			strdup( place_commas_in_money(
				   subclassification->
					subclassification_total ) ),
			0 /* not large_bold */ );

		if ( percent_denominator )
		{
			char buffer[ 128 ];
			double percent_of_total;

			percent_of_total =
				( subclassification->
					subclassification_total /
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

	} while( list_next( subclassification_list ) );

	return row_list;
}

LIST *subclassification_aggregate_latex_row_list(
			double *total_element,
			LIST *subclassification_list,
			char *element_name,
			double percent_denominator )
{
	LIST *row_list;
	char format_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;
	boolean first_time = 1;
	LATEX_ROW *latex_row;
	double percent_of_total;

	*total_element = 0.0;

	if ( !list_length( subclassification_list ) ) return (LIST *)0;

	row_list = list_new();

	/* For equity, always display the element title */
	/* -------------------------------------------- */
	if ( strcmp( element_name, ELEMENT_EQUITY ) ==  0 )
	{
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

		first_time = 0;

		if ( !subclassification_net_assets_equity_exists(
					subclassification_list  ) )
		{
			goto equity_all_done;
		}

		list_append_list(
		      row_list,
		      subclassification_aggregate_beginning_row_list(
				total_element,
				subclassification_list,
				percent_denominator ) );
	}

equity_all_done:

	list_rewind( subclassification_list );

	do {
		subclassification = list_get_pointer( subclassification_list );

		if ( timlib_dollar_virtually_same(
			subclassification->subclassification_total,
			0.0 ) )
		{
			continue;
		}

		/* Don't do net assets. */
		/* -------------------- */
		if ( strcmp(
			subclassification->
				subclassification_name,
			SUBCLASSIFICATION_NET_ASSETS ) == 0 )
		{
			continue;
		}

		if ( first_time )
		{
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

			first_time = 0;
		}

		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		latex_column_data_set(
			latex_row->column_data_list,
			strdup( format_initial_capital(
					format_buffer,
					subclassification->
					    subclassification_name ) ),
			0 /* not large_bold */ );

		latex_column_data_set(
			latex_row->column_data_list,
			strdup( place_commas_in_money(
			   	     subclassification->
					subclassification_total ) ),
			0 /* not large_bold */ );

		if ( percent_denominator )
		{
			char buffer[ 128 ];

			percent_of_total =
				( subclassification->subclassification_total /
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
		else
		{
			latex_column_data_set(
				latex_row->column_data_list,
				strdup( SUBCLASSIFICATION_NOTANUMBER ),
				0 /* not large_bold */ );
		}
	
		*total_element += subclassification->subclassification_total;

	} while( list_next( subclassification_list ) );

	latex_row = latex_new_latex_row();
	list_append_pointer( row_list, latex_row );

	if ( subclassification_net_assets_equity_exists(
				subclassification_list  ) )
	{
		sprintf(format_buffer,
			"\\large \\bf %s",
			"Equity Ending Balance" );
	}
	else
	{
		sprintf(format_buffer,
		 	"\\large \\bf Total %s",
		 	element_name );
	}

	format_initial_capital( format_buffer, format_buffer );

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( format_buffer ),
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
	else
	{
		latex_column_data_set(
			latex_row->column_data_list,
			strdup( SUBCLASSIFICATION_NOTANUMBER ),
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

	return row_list;
}

LIST *subclassification_display_latex_row_list(
			double *total_element,
			LIST *subclassification_list,
			char *element_name,
			boolean element_accumulate_debit,
			double percent_denominator )
{
	LIST *row_list;
	double subclassification_amount;
	ACCOUNT *account;
	char format_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;
	double latest_journal_balance;
	int first_time = 1;
	LATEX_ROW *latex_row;
	double percent_of_total;

	*total_element = 0.0;

	if ( !list_length( subclassification_list ) ) return (LIST *)0;

	row_list = list_new();

	/* For equity, always display the element title */
	/* -------------------------------------------- */
	if ( strcmp( element_name, ELEMENT_EQUITY ) ==  0 )
	{
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

		first_time = 0;

		if ( subclassification_net_assets_equity_exists(
					subclassification_list  ) )
		{
			list_append_list(
				row_list,
				subclassification_beginning_latex_row_list(
					total_element,
					subclassification_list,
					element_accumulate_debit ) );
		}
	}

	list_rewind( subclassification_list );

	do {
		subclassification = list_get_pointer( subclassification_list );

		/* Don't do net assets. */
		/* -------------------- */
		if ( strcmp(
			subclassification->
				subclassification_name,
			SUBCLASSIFICATION_NET_ASSETS ) == 0 )
		{
			continue;
		}

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		if ( first_time )
		{
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

			first_time = 0;
		}

		if ( list_length( subclassification->account_list ) > 1 )
		{
			latex_row = latex_new_latex_row();
			list_append_pointer( row_list, latex_row );

			sprintf( format_buffer,
				 "\\bf %s",
				 subclassification->
					subclassification_name );

			latex_column_data_set(
				latex_row->column_data_list,
				strdup( format_initial_capital(
						format_buffer,
						format_buffer ) ),
				0 /* not large_bold */ );
		}

		subclassification_amount = 0.0;

		do {
			account =
				list_get_pointer(
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

			/* ---------------------------------------------- */
			/* Don't display single Change in net assets row. */
			/* Display the total row below.			  */
			/* ---------------------------------------------- */
			if ( strcmp(	account->account_name,
					ACCOUNT_CHANGE_IN_NET_ASSETS )
					!= 0 )
			{
				latex_row = latex_new_latex_row();
				list_append_pointer( row_list, latex_row );

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

				if ( percent_denominator )
				{
					char buffer[ 128 ];

					latex_column_data_set(
						latex_row->column_data_list,
						strdup( "" ),
						0 /* not large_bold */ );

					latex_column_data_set(
						latex_row->column_data_list,
						strdup( "" ),
						0 /* not large_bold */ );

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
			}
	
			*total_element += latest_journal_balance;

			subclassification_amount +=
					latest_journal_balance;

		} while( list_next( subclassification->account_list ) );

		if ( subclassification_amount
		&&   list_length( subclassification_list ) > 1 )
		{
			latex_row = latex_new_latex_row();
			list_append_pointer( row_list, latex_row );

			if ( strcmp(
				subclassification->
					subclassification_name,
				SUBCLASSIFICATION_NET_ASSETS ) == 0 )
			{
				sprintf(
				    format_buffer,
				    "\\bf %s",
				    SUBCLASSIFICATION_BEGINNING_BALANCE_LABEL );
			}
			else
			if ( strcmp(
				subclassification->
					subclassification_name,
				ACCOUNT_CHANGE_IN_NET_ASSETS ) == 0 )
			{
				sprintf( format_buffer,
					 "\\bf %s",
					 "Change in Net Assets" );
			}
			else
			{
				sprintf( format_buffer,
				 	"\\bf Total %s",
				 	subclassification->
						subclassification_name );
			}

			latex_column_data_set(
				latex_row->column_data_list,
				strdup( format_initial_capital(
						format_buffer,
						format_buffer ) ),
				0 /* not large_bold */ );

			latex_column_data_set(
				latex_row->column_data_list,
				strdup( "" ),
				0 /* not large_bold */ );

			latex_column_data_set(
				latex_row->column_data_list,
				strdup( place_commas_in_money(
					   subclassification_amount ) ),
				0 /* not large_bold */ );

			if ( percent_denominator )
			{
				char buffer[ 128 ];

				latex_column_data_set(
					latex_row->column_data_list,
					strdup( "" ),
					0 /* not large_bold */ );

				percent_of_total =
					( subclassification_amount /
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
		}

	} while( list_next( subclassification_list ) );

	if ( *total_element )
	{
		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		if ( strcmp(
			subclassification->
				subclassification_name,
			ACCOUNT_CHANGE_IN_NET_ASSETS ) == 0 )
		{
			sprintf(format_buffer,
				"\\large \\bf %s",
				"Equity Ending Balance" );
		}
		else
		{
			sprintf(format_buffer,
			 	"\\large \\bf Total %s",
			 	element_name );
		}

		 format_initial_capital( format_buffer, format_buffer );

		latex_column_data_set(
			latex_row->column_data_list,
			strdup( format_buffer ),
			0 /* not large_bold */ );

		latex_column_data_set(
			latex_row->column_data_list,
			strdup( "" ),
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
	}

	return row_list;
}

LIST *subclassification_beginning_latex_row_list(
			double *total_element,
			LIST *subclassification_list,
			boolean element_accumulate_debit )
{
	LIST *row_list;
	double subclassification_amount;
	ACCOUNT *account;
	char format_buffer[ 128 ];
	SUBCLASSIFICATION *subclassification;
	double latest_journal_balance;
	LATEX_ROW *latex_row;

	if ( !list_rewind( subclassification_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		subclassification = list_get_pointer( subclassification_list );

		/* Only do net assets. */
		/* ------------------- */
		if ( strcmp(
			subclassification->
				subclassification_name,
			SUBCLASSIFICATION_NET_ASSETS ) != 0 )
		{
			continue;
		}

		if ( !list_rewind( subclassification->account_list ) )
			continue;

		if ( list_length( subclassification->account_list ) > 1 )
		{
			latex_row = latex_new_latex_row();
			list_append_pointer( row_list, latex_row );

			sprintf( format_buffer,
				 "\\bf %s",
				 subclassification->
					subclassification_name );

			latex_column_data_set(
				latex_row->column_data_list,
				strdup( format_initial_capital(
						format_buffer,
						format_buffer ) ),
				0 /* not large_bold */ );
		}

		subclassification_amount = 0.0;

		do {
			account =
				list_get_pointer(
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

			latex_row = latex_new_latex_row();
			list_append_pointer( row_list, latex_row );

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
	
			*total_element += latest_journal_balance;

			subclassification_amount +=
					latest_journal_balance;

		} while( list_next( subclassification->account_list ) );

		latex_row = latex_new_latex_row();
		list_append_pointer( row_list, latex_row );

		sprintf( format_buffer,
			 "\\bf %s",
			 SUBCLASSIFICATION_BEGINNING_BALANCE_LABEL );

		latex_column_data_set(
			latex_row->column_data_list,
			strdup( format_initial_capital(
					format_buffer,
					format_buffer ) ),
			0 /* not large_bold */ );

		latex_column_data_set(
			latex_row->column_data_list,
			strdup( "" ),
			0 /* not large_bold */ );

		latex_column_data_set(
			latex_row->column_data_list,
			strdup( place_commas_in_money(
				   subclassification_amount ) ),
			0 /* not large_bold */ );

		return row_list;

	} while( list_next( subclassification_list ) );

	return (LIST *)0;
}

void subclassification_aggregate_net_income_output(
			HTML_TABLE *html_table,
			double net_income,
			boolean is_statement_of_activities,
			double percent_denominator )
{
	double percent_of_total;

	if ( !html_table )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty html_table.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	html_table->data_list = list_new();

	if ( is_statement_of_activities )
	{
		html_table_set_data(
			html_table->data_list,
			"<h2>Change in Net Assets</h2>" );
	}
	else
	{
		html_table_set_data(
			html_table->data_list,
			"<h2>Net Income</h2>" );
	}
	html_table_set_data( html_table->data_list, strdup( "" ) );

	html_table_set_data(	html_table->data_list,
				strdup( place_commas_in_money( net_income ) ) );

	if ( percent_denominator )
	{
		char buffer[ 128 ];

		percent_of_total =
			( net_income /
			  percent_denominator ) * 100.0;

		sprintf( buffer,
			 "%.1lf%c",
			 percent_of_total,
			 '%' );

		html_table_set_data(
			html_table->data_list,
			strdup( buffer ) );
	}

	html_table_output_data( html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

	html_table->data_list = list_new();
}

boolean subclassification_net_assets_exists(
			LIST *subclassification_list )
{
	SUBCLASSIFICATION *subclassification;

	if ( !list_rewind( subclassification_list ) ) return 0;

	do {
		subclassification = list_get_pointer( subclassification_list );

		if ( strcmp(
			subclassification->
				subclassification_name,
			SUBCLASSIFICATION_NET_ASSETS ) == 0 )
		{
			return 1;
		}
	} while( list_next( subclassification_list ) );

	return 0;
}

LIST *subclassification_total_account_list(
			double *subclassification_total,
			char *subclassification_name,
			char *fund_name,
			char *transaction_date_time_closing )
{
	LIST *account_list;
	ACCOUNT *account;
	char sys_string[ 1024 ];
	char where[ 256 ];
	char account_name[ 128 ];
	FILE *input_pipe;
	JOURNAL *latest_journal;

	if ( fund_name
	&&   *fund_name
	&&   strcmp( fund_name, "fund" ) != 0 )
	{
		sprintf(where,
			"fund = '%s' and subclassification = '%s'",
			fund_name,
			subclassification_name );
	}
	else
	{
		sprintf(where,
			"subclassification = '%s'",
			subclassification_name );
	}

	sprintf(sys_string,
		"select.sh \"%s\" %s \"%s\" select",
		"account",
		"account",
		 where );

	account_list = list_new();
	input_pipe = popen( sys_string, "r" );

	while( string_input( account_name, input_pipe, 128 ) )
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

		account =
			account_fetch(
				strdup( account_name ) );

		/* Change account name from stack memory to heap. */
		/* ---------------------------------------------- */
		latest_journal->account_name = account->account_name;

		account->latest_journal = latest_journal;

		list_add_pointer_in_order(
			account_list,
			account,
			account_balance_match_function );

		*subclassification_total += account->latest_journal->balance;
	}

	pclose( input_pipe );
	return account_list;
}

LIST *subclassification_list( void )
{
	return subclassification_system_list(
			subclassification_sys_string(
				"1 = 1" ) );
}

LIST *subclassification_system_list(
			char *sys_string )
{
	FILE *input_pipe;
	char input[ 1024 ];
	LIST *subclassification_list;

	input_pipe = popen( sys_string, "r" );
	subclassification_list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			subclassification_list,
			subclassification_parse( input ) );
	}
	pclose( input_pipe );
	return subclassification_list;
}

char *subclassification_sys_string( char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" select",
		 SUBCLASSIFICATION_TABLE_NAME,
		 where );

	return strdup( sys_string );
}

SUBCLASSIFICATION *subclassification_seek(
			char *subclassification_name,
			LIST *subclassification_list )
{
	SUBCLASSIFICATION *subclassification;

	if ( !list_rewind( subclassification_list ) )
		return (SUBCLASSIFICATION *)0;

	do {
		subclassification =
			list_get(
				subclassification_list );

		if ( string_strcmp(
			subclassification->subclassification_name,
			subclassification_name ) == 0 )
		{
			return subclassification;
		}

	} while ( list_next( subclassification_list ) );

	return (SUBCLASSIFICATION *)0;
}

boolean subclassification_current_liability(
			char *subclassification_name )
{
	return ( strcmp(
			subclassification_name,
			SUBCLASSIFICATION_CURRENT_LIABILITY ) == 0 );
}

boolean subclassification_receivable(
			char *subclassification_name )
{
	return ( strcmp(
			subclassification_name,
			SUBCLASSIFICATION_RECEIVABLE ) == 0 );
}

void subclassification_list_account_action_string_set(
			LIST *subclassification_list,
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *beginning_date,
			char *as_of_date )
{
	SUBCLASSIFICATION *subclassification;

	if ( !list_rewind( subclassification_list ) ) return;

	do {
		subclassification = list_get( subclassification_list );

		if ( list_length( subclassification->account_list ) )
		{
			account_list_action_string_set(
				subclassification->account_list,
				application_name,
				session,
				login_name,
				role_name,
				beginning_date,
				as_of_date );
		}

	} while ( list_next( subclassification_list ) );
}

void subclassification_delta_prior_set(
			LIST *prior_subclassification_list,
			SUBCLASSIFICATION *subclassification )
{
	SUBCLASSIFICATION *prior_subclassification;

	if ( ! ( prior_subclassification =
			subclassification_seek(
				subclassification->subclassification_name,
				prior_subclassification_list ) ) )
	{
		return;
	}

	prior_subclassification->delta_prior =
		statement_delta_prior(
			prior_subclassification->subclassification_total,
			subclassification->subclassification_total );

	account_list_delta_prior_set(
		prior_subclassification->account_list,
		subclassification->account_list );
}

void subclassification_list_delta_prior_set(
			LIST *prior_subclassification_list,
			LIST *subclassification_list )
{
	SUBCLASSIFICATION *subclassification;

	if ( !list_rewind( subclassification_list ) ) return;

	do {
		subclassification = list_get( subclassification_list );

		subclassification_delta_prior_set(
			prior_subclassification_list,
			subclassification );

	} while ( list_next( subclassification_list ) );
}

double subclassification_debit_total(
			LIST *subclassification_list )
{
	SUBCLASSIFICATION *subclassification;
	double total;

	if ( !list_rewind( subclassification_list ) ) return 0.0;

	total = 0.0;

	do {
		subclassification = list_get( subclassification_list );

		if ( list_length( subclassification->account_list ) )
		{
			total +=
				account_debit_total(
					subclassification->
						account_list );
		}

	} while ( list_next( subclassification_list ) );

	return total;
}

double subclassification_credit_total(
			LIST *subclassification_list )
{
	SUBCLASSIFICATION *subclassification;
	double total;

	if ( !list_rewind( subclassification_list ) ) return 0.0;

	total = 0.0;

	do {
		subclassification = list_get( subclassification_list );

		if ( list_length( subclassification->account_list ) )
		{
			total +=
				account_credit_total(
					subclassification->
						account_list );
		}

	} while ( list_next( subclassification_list ) );

	return total;
}

double subclassification_balance_total(
			LIST *subclassification_list )
{
	SUBCLASSIFICATION *subclassification;
	double total;

	if ( !list_rewind( subclassification_list ) ) return 0.0;

	total = 0.0;

	do {
		subclassification = list_get( subclassification_list );

		subclassification->subclassification_total =
			account_balance_total(
				subclassification->account_list );

		total += subclassification->subclassification_total;

	} while ( list_next( subclassification_list ) );

	return total;
}

void subclassification_list_percent_of_asset_set(
			LIST *subclassification_list,
			double asset_total )
{
	SUBCLASSIFICATION *subclassification;
	double percent_of_asset;

	if ( !asset_total ) return;

	if ( !list_rewind( subclassification_list ) ) return;

	do {
		subclassification = list_get( subclassification_list );

		percent_of_asset =
			(subclassification->subclassification_total /
			 asset_total) * 100.0;

		subclassification->percent_of_asset =
			float_round_int( percent_of_asset );

		if ( list_length( subclassification->account_list ) )
		{
			account_list_percent_of_asset_set(
				subclassification->account_list,
				asset_total );
		}

	} while ( list_next( subclassification_list ) );
}

void subclassification_list_percent_of_revenue_set(
			LIST *subclassification_list,
			double revenue_total )
{
	SUBCLASSIFICATION *subclassification;
	double percent_of_revenue;

	if ( !revenue_total ) return;

	if ( !list_rewind( subclassification_list ) ) return;

	do {
		subclassification = list_get( subclassification_list );

		percent_of_revenue =
			(subclassification->subclassification_total /
			 revenue_total) * 100.0;

		subclassification->percent_of_revenue =
			float_round_int( percent_of_revenue );

		if ( list_length( subclassification->account_list ) )
		{
			account_list_percent_of_revenue_set(
				subclassification->account_list,
				revenue_total );
		}

	} while ( list_next( subclassification_list ) );
}

LIST *subclassification_account_list(
			LIST *subclassification_list )
{
	SUBCLASSIFICATION *subclassification;
	LIST *account_list;

	if ( !list_rewind( subclassification_list ) ) return (LIST *)0;

	account_list = list_new();

	do {
		subclassification = list_get( subclassification_list );

		list_append_list(
			account_list,
			subclassification->account_list );

	} while ( list_next( subclassification_list ) );

	return account_list;
}

char *subclassification_total_label(
			char *subclassification_name,
			char *alternate_display_name )
{
	char label[ 256 ];
	char format_buffer[ 128 ];

	if ( alternate_display_name && *alternate_display_name )
	{
		strcpy( label, alternate_display_name );
	}
	else
	{
		sprintf(label,
			"Total %s",
			format_initial_capital(
				format_buffer,
				subclassification_name ) );
	}

	return strdup( label );
}

