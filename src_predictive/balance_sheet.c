/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/balance_sheet.c			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "float.h"
#include "timlib.h"
#include "transaction.h"
#include "statement.h"
#include "element.h"
#include "latex.h"
#include "html_table.h"
#include "balance_sheet.h"

BALANCE_SHEET_SUBCLASS_DISPLAY_HTML *
	balance_sheet_subclass_display_html_new(
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			double income_statement_fetch_net_income,
			char *net_income_label )
{
}

BALANCE_SHEET_SUBCLASS_DISPLAY_HTML *
	balance_sheet_subclass_display_html_calloc(
			void )
{
}

LIST *balance_sheet_subclass_display_html_equity_row_list(
			STATEMENT *statement,
			int statement_prior_year_list_length,
			ELEMENT *element_equity_begin,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
}

HTML_ROW *balance_sheet_subclass_display_html_equity_begin_row(
			double element_equity_begin_sum )
{
}

HTML_ROW *balance_sheet_subclass_display_html_transaction_row(
			double balance_sheet_equity_transaction_amount )
{
}

HTML_ROW *balance_sheet_subclass_display_html_net_income_row(
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
}

HTML_ROW *balance_sheet_subclass_display_html_ending_balance(
			double balance_sheet_equity_ending_balance )
{
}

HTML_ROW *balance_sheet_subclass_display_html_liability_plus_equity(
			double balance_sheet_liability_plus_equity )
{
}

BALANCE_SHEET_SUBCLASS_OMIT_HTML *
	balance_sheet_subclass_omit_html_new(
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
}

BALANCE_SHEET_SUBCLASS_OMIT_HTML *
	balance_sheet_subclass_omit_html_calloc(
			void )
{
}

LIST *balance_sheet_subclass_omit_html_equity_row_list(
			STATEMENT *statement,
			int statement_prior_year_list_length,
			ELEMENT *element_equity_begin,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
}

HTML_ROW *balance_sheet_subclass_omit_html_equity_begin_row(
			double element_equity_begin_sum )
{
}

HTML_ROW *balance_sheet_subclass_omit_html_transaction_row(
			double balance_sheet_equity_transaction_amount )
{
}

HTML_ROW *balance_sheet_subclass_omit_html_net_income_row(
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
}

HTML_ROW *balance_sheet_subclass_omit_html_ending_balance(
			double balance_sheet_equity_ending_balance )
{
}

HTML_ROW *balance_sheet_subclass_omit_html_liability_plus_equity(
			double balance_sheet_liability_plus_equity )
{
}

BALANCE_SHEET_SUBCLASS_AGGR_HTML *
	balance_sheet_subclass_aggr_html_new(
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
}

BALANCE_SHEET_SUBCLASS_AGGR_HTML *
	balance_sheet_subclass_aggr_html_calloc(
			void )
{
}

LIST *balance_sheet_subclass_aggr_html_equity_row_list(
			STATEMENT *statement,
			int statement_prior_year_list_length,
			ELEMENT *element_equity_begin,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
}

HTML_ROW *balance_sheet_subclass_aggr_html_equity_begin_row(
			double element_equity_begin_sum )
{
}

HTML_ROW *balance_sheet_subclass_aggr_html_transaction_row(
			double balance_sheet_equity_transaction_amount )
{
}

HTML_ROW *balance_sheet_subclass_aggr_html_net_income_row(
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
}

HTML_ROW *balance_sheet_subclass_aggr_html_ending_balance(
			double balance_sheet_equity_ending_balance )
{
}

HTML_ROW *balance_sheet_subclass_aggr_html_liability_plus_equity(
			double balance_sheet_liability_plus_equity )
{
}

BALANCE_SHEET_HTML *balance_sheet_html_new(
			enum statement_subclassification_option
				statement_subclassification_option,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
}

BALANCE_SHEET_HTML *balance_sheet_html_calloc( void )
{
}

HTML_TABLE *balance_sheet_html_table(
			char *statement_caption_subtitle,
			LIST *heading_list,
			LIST *row_list )
{
}

BALANCE_SHEET_SUBCLASS_DISPLAY_LATEX *
	balance_sheet_subclass_display_latex_new(
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
}

BALANCE_SHEET_SUBCLASS_DISPLAY_LATEX *
	balance_sheet_subclass_display_latex_calloc(
			void )
{
}

LIST *balance_sheet_subclass_display_latex_equity_row_list(
			STATEMENT *statement,
			int statement_prior_year_list_length,
			ELEMENT *element_equity_begin,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
}

LATEX_ROW *balance_sheet_subclass_display_latex_equity_begin_row(
			double element_equity_begin_sum )
{
}

LATEX_ROW *balance_sheet_subclass_display_latex_transaction_row(
			double balance_sheet_equity_transaction_amount )
{
}

LATEX_ROW *balance_sheet_subclass_display_latex_net_income_row(
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
}

LATEX_ROW *balance_sheet_subclass_display_latex_ending_balance(
			double balance_sheet_equity_ending_balance )
{
}

LATEX_ROW *balance_sheet_subclass_display_latex_liability_plus_equity(
			double balance_sheet_liability_plus_equity )
{
}

BALANCE_SHEET_SUBCLASS_OMIT_LATEX *
	balance_sheet_subclass_omit_latex_new(
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
}

BALANCE_SHEET_SUBCLASS_OMIT_LATEX *
	balance_sheet_subclass_omit_latex_calloc(
			void )
{
}

LIST *balance_sheet_subclass_omit_latex_equity_row_list(
			STATEMENT *statement,
			int statement_prior_year_list_length,
			ELEMENT *element_equity_begin,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
}

LATEX_ROW *balance_sheet_subclass_omit_latex_equity_begin_row(
			double element_equity_begin_sum )
{
}

LATEX_ROW *balance_sheet_subclass_omit_latex_transaction_row(
			double balance_sheet_equity_transaction_amount )
{
}

LATEX_ROW *balance_sheet_subclass_omit_latex_net_income_row(
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
}

LATEX_ROW *balance_sheet_subclass_omit_latex_ending_balance(
			double balance_sheet_equity_ending_balance )
{
}

LATEX_ROW *balance_sheet_subclass_omit_latex_liability_plus_equity(
			double balance_sheet_liability_plus_equity )
{
}

BALANCE_SHEET_SUBCLASS_AGGR_LATEX *
	balance_sheet_subclass_aggr_latex_new(
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
}

BALANCE_SHEET_SUBCLASS_AGGR_LATEX *
	balance_sheet_subclass_aggr_latex_calloc(
			void )
{
}

LIST *balance_sheet_subclass_aggr_latex_equity_row_list(
			STATEMENT *statement,
			int statement_prior_year_list_length,
			ELEMENT *element_equity_begin,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
}

LATEX_ROW *balance_sheet_subclass_aggr_latex_equity_begin_row(
			double element_equity_begin_sum )
{
}

LATEX_ROW *balance_sheet_subclass_aggr_latex_transaction_row(
			double balance_sheet_equity_transaction_amount )
{
}

LATEX_ROW *balance_sheet_subclass_aggr_latex_net_income_row(
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
}

LATEX_ROW *balance_sheet_subclass_aggr_latex_ending_balance(
			double balance_sheet_equity_ending_balance )
{
}

LATEX_ROW *balance_sheet_subclass_aggr_latex_liability_plus_equity(
			double balance_sheet_liability_plus_equity )
{
}

BALANCE_SHEET_LATEX *
	balance_sheet_latex_new(
			enum statement_subclassification_option
				statement_subclassification_option,
			char *tex_filename,
			char *dvi_filename,
			char *working_directory,
			boolean statement_pdf_landscape_boolean,
			char *statement_logo_filename,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label )
{
}

BALANCE_SHEET_LATEX *balance_sheet_latex_calloc( void )
{
}

LATEX_TABLE *balance_sheet_latex_table(
			char *statement_caption,
			LIST *heading_list,
			LIST *row_list )
{
}

BALANCE_SHEET_PDF *balance_sheet_pdf_new(
			char *application_name,
			char *process_name,
			char *document_root_directory,
			enum statement_subclassification_option
				statement_subclassification_option,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label,
			pid_t process_id )
{
	BALANCE_SHEET_PDF *balance_sheet_pdf;

	if ( !application_name
	||   !process_name
	||   !document_root_directory
	||   !statement
	||   !income_statement_net_income_label
	||   !process_id )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	balance_sheet_pdf = balance_sheet_pdf_calloc();

	balance_sheet_pdf->statement_pdf_landacape_boolean =
		statement_pdf_landacape_boolean(
		list_length( statement_prior_year_list )
			/* statement_prior_year_list_length */ );

	balance_sheet_pdf->statement_link =
		statement_link_new(
			application_name,
			process_name,
			document_root_directory,
			statement->transaction_begin_date_string,
			statement->transaction_as_of_date,
			(char *)0 /* statement_pdf_preclose_key */,
			process_id );

	balance_sheet->pdf->balance_sheet_latex =
		balance_sheet_latex_new(
			statement_subclassification_option,
			balance_sheet_pdf->statement_link->tex_filename,
			balance_sheet_pdf->statement_link->dvi_filename,
			balance_sheet_pdf->statement_link->working_directory,
			balance_sheet_pdf->statement_pdf_landscape_boolean,
			statement->statement_logo_filename,
			statement,
			statement_prior_year_list,
			element_equity_begin,
			income_statement_fetch_net_income,
			income_statement_net_income_label );

	return balance_sheet_pdf;
}

BALANCE_SHEET_PDF *balance_sheet_pdf_calloc( void )
{
	BALANCE_SHEET_PDF *balance_sheet_pdf;

	if ( ! ( balance_sheet_pdf =
			calloc( 1, sizeof( BALANCE_SHEET_PDF ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return balance_sheet_pdf;
}

BALANCE_SHEET *balance_sheet_fetch(
			char *argv_0,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_name,
			char *document_root_directory,
			char *as_of_date,
			int prior_year_count,
			char *subclassifiction_option_string,
			char *output_medium_string )
{
	BALANCE_SHEET *balance_sheet;

	if ( !argv_0
	||   !application_name
	||   !session_key
	||   !role_name
	||   !process_name
	||   !document_root_directory
	||   !as_of_date
	||   !subclassification_option_string
	||   !output_medium_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	balance_sheet = balance_sheet_calloc();

	balance_sheet->statement_subclassification_option =
		statement_resolve_subclassification_option(
			subclassifiction_option_string );

	balance_sheet->statement_output_medium =
		statement_resolve_output_medium(
			output_medium_string );

	balance_sheet->transaction_as_of_date =
		transaction_as_of_date(
			TRANSACTION_TABLE,
			as_of_date );

	if ( !balance_sheet->transaction_as_of_date )
	{
		free( balance_sheet );
		return (BALANCE_SHEET *)0;
	}

	balance_sheet->transaction_begin_date_string =
		transaction_begin_date_string(
			TRANSACTION_TABLE,
			balance_sheet->transaction_as_of_date );

	if ( !balance_sheet->transaction_begin_date_string )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: transaction_begin_date_string(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			balance_sheet->transaction_as_of_date );
		exit( 1 );
	}

	balance_sheet->element_name_list =
		balance_sheet_element_name_list(
			ELEMENT_ASSET,
			ELEMENT_LIABILITY,
			ELEMENT_EQUITY );

	balance_sheet->transaction_closing_entry_exists =
		transaction_closing_entry_exists(
			TRANSACTION_TABLE,
			TRANSACTION_CLOSE_TIME,
			balance_sheet->transaction_as_of_date );

	balance_sheet->transaction_date_time_closing =
		transaction_date_time_closing(
			TRANSACTION_PRECLOSE_TIME,
			TRANSACTION_CLOSE_TIME,
			balance_sheet->transaction_as_of_date,
			balance_sheet->transaction_closing_entry_exists
				/* preclose_time_boolean */ );

	balance_sheet->statement =
		statement_fetch(
			application_name,
			process_name,
			balance_sheet->element_name_list,
			balance_sheet->transaction_begin_date_string,
			balance_sheet->transaction_as_of_date,
			balance_sheet->transaction_date_time_closing,
			0 /* not fetch_transaction */ );

	if ( balance_sheet->statement_output_medium == output_table )
	{
		element_account_transaction_count_set(
			balance_sheet->statement->element_statement_list,
			balance_sheet->transaction_begin_date_string,
			balance_sheet->transaction_date_time_closing );

		element_account_action_string_set(
			balance_sheet->statement->element_statement_list,
			application_name,
			session_key,
			login_name,
			role_name,
			balance_sheet->transaction_begin_date_string,
			balance_sheet->transaction_date_time_closing );
	}

	if ( prior_year_count )
	{
		balance_sheet->statement_prior_year_list =
			statement_prior_year_list(
				balance_sheet->element_name_list,
				balance_sheet->transaction_date_time_closing,
				prior_year_count,
				balance_sheet->statement );
	}

	balance_sheet->prior_date_time_closing =
		balance_sheet_prior_date_time_closing(
			balance_sheet->transaction_begin_date_string );

	if ( balance_sheet->prior_date_time_closing )
	{
		balance_sheet->element_equity_begin =
			element_statement_fetch(
				ELEMENT_EQUITY,
				balance_sheet->prior_date_time_closing,
				1 /* fetch_subclassification_list */,
				1 /* fetch_account_list */,
				1 /* fetch_journal_latest */,
				0 /* not fetch_transaction */ );

		balance_sheet->element_equity_begin->sum =
			element_sum(
				element_equity_begin );
	}

	balance_sheet->income_statement_fetch_net_income =
		income_statement_fetch_net_income(
			balance_sheet->transaction_date_time_closing );

	balance_sheet->income_statement_net_income_label =
		income_statement_net_income_label(
			argv_0 );

	if (	balance_sheet->statement_subclassification_option ==
		subclassification_omit )
	{
		element_list_account_statement_list_set(
			balance_sheet->statement->element_statement_list );
	}

	if ( balance_sheet->statement_output_medium == output_PDF )
	{
		balance_sheet->balance_sheet_pdf =
			balance_sheet_pdf_new(
				application_name,
				process_name,
				document_root_directory,
				balance_sheet->
					statement_subclassification_option,
				balance_sheet->statement,
				balance_sheet->statement_prior_year_list,
				balance_sheet->element_equity_begin,
				balance_sheet->
					income_statement_fetch_net_income,
				balance_sheet->
					income_statement_net_income_label,
				getpid() /* process_id */ );
	}
	else
	if ( balance_sheet->statement_output_medium == output_table )
	{
		balance_sheet->balance_sheet_html =
			balance_sheet_html_new(
				balance_sheet->
					statement_subclassification_option,
				balance_sheet->statement,
				balance_sheet->statement_prior_year_list,
				balance_sheet->element_equity_begin,
				balance_sheet->
					income_statement_fetch_net_income,
				balance_sheet->
					income_statement_net_income_label );
	}

	return balance_sheet;
}

BALANCE_SHEET *balance_sheet_calloc( void )
{
	BALANCE_SHEET *balance_sheet;

	if ( ! ( balance_sheet = calloc( 1, sizeof( BALANCE_SHEET ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return balance_sheet;
}

LIST *balance_sheet_element_name_list(
			char *element_asset,
			char *element_liability,
			char *element_equity )
{
	LIST *element_name_list = list_new();

	list_set( element_name_list, element_asset );
	list_set( element_name_list, element_liability );
	list_set( element_name_list, element_equity );

	return element_name_list;
}

char *balance_sheet_prior_date_time_closing(
			char *transaction_begin_date_string )
{
	DATE *prior =
		/* ------------------- */
		/* Trims trailing time */
		/* ------------------- */
		date_yyyy_mm_dd_new(
			transaction_begin_date_string );

	date_decrement_second( prior, 1 );

	return date_display19( prior );
}

double balance_sheet_equity_transaction_amount(
			double element_equity_current_sum,
			double element_equity_begin_sum )
{
	return
	element_equity_current_sum -
	element_equity_begin_sum;
}

double balance_sheet_equity_ending_balance(
			double element_equity_current_sum,
			double income_statement_fetch_net_income )
{
	return
	element_equity_current_sum +
	income_statement_fetch_net_income;
}

double balance_sheet_liability_plus_equity(
			double element_liability_sum,
			double balance_sheet_equity_ending_balance )
{
	return
	element_liability_sum +
	balance_sheet_equity_ending_balance;
}
