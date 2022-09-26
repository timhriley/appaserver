/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/budget.c			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */


#include <stdio.h>
#include <unistd.h>
#include "String.h"
#include "timlib.h"
#include "date_convert.h"
#include "date.h"
#include "float.h"
#include "appaserver_library.h"
#include "html_table.h"
#include "latex.h"
#include "element.h"
#include "transaction.h"
#include "budget.h"

LIST *budget_annualized_list(
			DATE *budget_begin_date,
			DATE *budget_as_of_date,
			LIST *element_statement_list,
			STATEMENT_PRIOR_YEAR *statement_prior_year )
{
	LIST *list;
	ELEMENT *element;
	ACCOUNT *account;

	if ( !budget_begin_date
	||   !budget_as_of_date
	||   !list_rewind( element_statement_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list = list_new();

	do {
		element = list_get( element_statement_list );

		if ( !list_rewind( element->account_statement_list ) )
			continue;

		do {
			account =
				list_get(
					element->account_statement_list );


			if ( !account->account_journal_latest )
			{
				fprintf(stderr,
		"ERROR in %s/%s()/%d: account_journal_latest is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}


			if ( double_virtually_same(
				account->account_journal_latest->balance,
				0.0 ) )
			{
				continue;
			}

			list_set(
				list,
				budget_annualized_new(
					budget_begin_date,
					budget_as_of_date,
					account,
					statement_prior_year ) );

		} while ( list_next( element->account_statement_list ) );

	} while ( list_next( element_statement_list ) );

	return list;
}

BUDGET_ANNUALIZED *budget_annualized_new(
			DATE *budget_begin_date,
			DATE *budget_as_of_date,
			ACCOUNT *account,
			STATEMENT_PRIOR_YEAR *statement_prior_year )
{
	BUDGET_ANNUALIZED *budget_annualized;

	if ( !budget_begin_date
	||   !budget_as_of_date
	||   !account
	||   !account->account_journal_latest )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !account->account_journal_latest->balance )
		return (BUDGET_ANNUALIZED *)0;

	budget_annualized = budget_annualized_calloc();

	budget_annualized->account = account;

	budget_annualized->days_so_far =
		budget_annualized_days_so_far(
			budget_begin_date,
			budget_as_of_date );

	budget_annualized->days_in_year =
		budget_annualized_days_in_year(
			budget_begin_date );

	budget_annualized->year_ratio =
		budget_annualized_year_ratio(
			budget_annualized->days_so_far,
			budget_annualized->days_in_year );

	budget_annualized->account_amount =
		budget_annualized_account_amount(
			account->account_journal_latest->balance
				/* account_latest_balance */,
			account->annual_budget );

	budget_annualized->amount =
		budget_annualized_amount(
			budget_annualized->account_amount,
			budget_annualized->year_ratio );

	if ( statement_prior_year )
	{
		budget_annualized->prior_account =
			account_element_list_seek(
				account->account_name,
				statement_prior_year->
					element_statement_list );

		if ( budget_annualized->prior_account )
		{
			if ( !budget_annualized->
				prior_account->
				account_journal_latest )
			{
				fprintf(stderr,
"ERROR in %s/%s()/%d: prior_account->account_journal_latest is empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			if ( budget_annualized->
				prior_account->
				account_journal_latest->
				balance )
			{
				budget_annualized->difference =
				   budget_annualized_difference(
					budget_annualized->amount,
					budget_annualized->
						prior_account->
						account_journal_latest->
						balance
						/* prior_account_balance */ );
			}
		}
	}

	return budget_annualized;
}

BUDGET_ANNUALIZED *budget_annualized_calloc( void )
{
	BUDGET_ANNUALIZED *budget_annualized;

	if ( ! ( budget_annualized =
			calloc( 1, sizeof( BUDGET_ANNUALIZED ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return budget_annualized;
}

int budget_annualized_days_so_far(
			DATE *begin_date,
			DATE *as_of_date )
{
	if ( !begin_date
	||   !as_of_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	date_subtract_days(
		as_of_date /* later_date */,
		begin_date /* earlier_date */ );
}

int budget_annualized_days_in_year(
			DATE *begin_date )
{
	if ( !begin_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: begin_date is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return
	date_days_in_year(
		date_year( 
			begin_date ) );
}

double budget_annualized_year_ratio(
			int days_so_far,
			int days_in_year )
{
	if ( !days_in_year )
		return 0.0;
	else
		return (double)days_so_far / (double)days_in_year;
}

double budget_annualized_account_amount(
			double account_latest_balance,
			int account_annual_amount )
{
	if ( account_annual_amount )
		return (double)account_annual_amount;
	else
		return account_latest_balance;
}

double budget_annualized_amount(
			double account_amount,
			double year_ratio )
{
	if ( double_virtually_same( year_ratio, 0.0 ) )
	{
		return 0.0;
	}
	else
	{
		return
		account_amount /
		year_ratio;
	}
}

double budget_annualized_difference(
			double budget_annualized_amount,
			double prior_account_balance )
{
	return
	prior_account_balance -
	budget_annualized_amount;
}

BUDGET *budget_fetch(	char *application_name,
			char *process_name,
			char *document_root_directory,
			char *as_of_date_string,
			char *output_medium_string )
{
	BUDGET *budget;

	if ( !application_name
	||   !process_name
	||   !document_root_directory
	||   !as_of_date_string
	||   !output_medium_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	budget = budget_calloc();

	budget->statement_output_medium =
		statement_resolve_output_medium(
			output_medium_string );

	budget->transaction_as_of_date =
		/* ----------------------------------------------- */
		/* Returns as_of_date_string, heap memory, or null */
		/* ----------------------------------------------- */
		transaction_as_of_date(
			TRANSACTION_TABLE,
			as_of_date_string );

	if ( !budget->transaction_as_of_date )
	{
		return (BUDGET *)0;
	}

	budget->as_of_date =
		budget_as_of_date(
			budget->transaction_as_of_date );

	budget->transaction_begin_date_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		transaction_begin_date_string(
			TRANSACTION_TABLE,
			budget->transaction_as_of_date );

	if ( !budget->transaction_begin_date_string )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: transaction_begin_date_string(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			budget->transaction_as_of_date );
		exit( 1 );
	}

	budget->begin_date =
		budget_begin_date(
			budget->transaction_begin_date_string );

	budget->element_name_list =
		budget_element_name_list(
			ELEMENT_REVENUE,
			ELEMENT_EXPENSE );

	budget->transaction_closing_entry_exists =
		transaction_closing_entry_exists(
			TRANSACTION_TABLE,
			TRANSACTION_CLOSE_TIME,
			budget->transaction_as_of_date );

	budget->transaction_date_time_closing =
		transaction_date_time_closing(
			TRANSACTION_PRECLOSE_TIME,
			TRANSACTION_CLOSE_TIME,
			budget->transaction_as_of_date,
			budget->transaction_closing_entry_exists
				/* preclose_time_boolean */ );

	budget->statement =
		statement_fetch(
			application_name,
			process_name,
			budget->element_name_list,
			budget->transaction_begin_date_string,
			budget->transaction_as_of_date,
			budget->transaction_date_time_closing,
			0 /* not fetch_transaction */ );

	element_list_account_statement_list_set(
		budget->statement->element_statement_list );

	budget->end_date_time_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		budget_end_date_time_string(
			budget->transaction_begin_date_string );

	budget->statement_prior_year_list =
		statement_prior_year_list(
				budget->element_name_list,
				budget->end_date_time_string,
				1 /* prior_year_count */,
				budget->statement );

	budget->annualized_list =
		budget_annualized_list(
			budget->begin_date,
			budget->as_of_date,
			budget->statement->element_statement_list,
			list_first( budget->statement_prior_year_list )
				/* statement_prior_year */ );

	if ( budget->statement_output_medium == statement_output_PDF )
	{
		budget->budget_pdf =
			budget_pdf_new(
				application_name,
				process_name,
				document_root_directory,
				budget->
					statement->
					transaction_begin_date_string,
				budget->
					statement->
					transaction_as_of_date,
				budget->
					statement->
					statement_caption->
					logo_filename,
				budget->
					statement->
					statement_caption->
					combined,
				budget->annualized_list,
				getpid() /* process_id */ );
	}
	else
	if ( budget->statement_output_medium == statement_output_table )
	{
/*
		budget->budget_html =
			budget_html_new(
				budget->budget_annualized_list );
*/
	}

	return budget;
}

BUDGET *budget_calloc( void )
{
	BUDGET *budget;

	if ( ! ( budget = calloc( 1, sizeof( BUDGET ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return budget;
}

DATE *budget_as_of_date( char *transaction_as_of_date )
{
	if ( !transaction_as_of_date )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: transaction_as_of_date is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return date_yyyy_mm_dd_new( transaction_as_of_date );
}

DATE *budget_begin_date( char *transaction_begin_date_string )
{
	if ( !transaction_begin_date_string )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: transaction_begin_date_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return date_yyyy_mm_dd_new( transaction_begin_date_string );
}

LIST *budget_element_name_list(
			char *element_revenue,
			char *element_expense )
{
	LIST *name_list = list_new();

	list_set( name_list, element_revenue );
	list_set( name_list, element_expense );

	return name_list;
}

char *budget_end_date_time_string(
			char *transaction_begin_date_string )
{
	DATE *future_date;

	if ( !transaction_begin_date_string )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: transaction_begin_date_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	future_date = date_yyyy_mm_dd_new( transaction_begin_date_string );

	if ( !date_set_time_hhmmss(
		future_date,
		TRANSACTION_PRECLOSE_TIME ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: date_set_time_hhmmss() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	date_increment_years( future_date, 1 );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	date_display19( future_date );
}

char *budget_difference_display( double difference )
{
	if ( difference )
	{
		return
		/* --------------------- */
		/* Returns heap memory   */
		/* Doesn't trim pennies  */
		/* --------------------- */
		timlib_place_commas_in_money(
			difference );
	}
	else
	{
		return (char *)0;
	}
}

char *budget_amount_display( ACCOUNT *prior_account )
{
	if ( prior_account && prior_account->account_journal_latest )
	{
		return
		/* --------------------- */
		/* Returns heap memory   */
		/* Doesn't trim pennies  */
		/* --------------------- */
		timlib_place_commas_in_money(
			prior_account->
				account_journal_latest->
				balance );
	}
	else
	{
		return (char *)0;
	}
}

LATEX_ROW *budget_latex_row(
			char *account_name,
			double account_amount,
			ACCOUNT *prior_account,
			double difference )
{
	LATEX_ROW *latex_row;

	if ( !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: account_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label( account_name /* name */ ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		/* --------------------- */
		/* Returns heap memory   */
		/* Doesn't trim pennies  */
		/* --------------------- */
		timlib_place_commas_in_money( account_amount ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		budget_amount_display( prior_account ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		budget_difference_display( difference ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	return latex_row;
}

LIST *budget_latex_row_list( LIST *budget_annualized_list )
{
	LIST *row_list;
	BUDGET_ANNUALIZED *budget_annualized;

	if ( !list_rewind( budget_annualized_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		budget_annualized = list_get( budget_annualized_list );

		if ( !budget_annualized->account
		||   !budget_annualized->account->account_journal_latest )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: account_journal_latest is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			row_list,
			budget_latex_row(
				budget_annualized->account->account_name,
				budget_annualized->account_amount,
				budget_annualized->prior_account,
				budget_annualized->difference ) );

	} while ( list_next( budget_annualized_list ) );

	return row_list;
}

LIST *budget_latex_heading_list( void )
{
	LIST *heading_list = list_new();

	list_set(
		heading_list,
		latex_table_heading_new(
			"Account",
			0 /* right_Justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Balance",
			1 /* right_Justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Annualized",
			1 /* right_Justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Budget",
			1 /* right_Justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Difference",
			1 /* right_Justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	return heading_list;
}

LATEX_TABLE *budget_latex_table(
			char *statement_caption_combined,
			LIST *budget_annualized_list )
{
	LATEX_TABLE *latex_table;

	if ( !statement_caption_combined
	||   !list_length( budget_annualized_list ) )
	{
		return (LATEX_TABLE *)0;
	}

	latex_table =
		latex_table_new(
			statement_caption_combined );

	latex_table->heading_list =
		budget_latex_heading_list();

	latex_table->row_list =
		budget_latex_row_list(
			budget_annualized_list );

	return latex_table;
}

BUDGET_LATEX *budget_latex_new(
			char *tex_filename,
			char *dvi_filename,
			char *working_directory,
			char *statement_logo_filename,
			char *statement_caption_combined,
			LIST *budget_annualized_list )
{
	BUDGET_LATEX *budget_latex;

	if ( !tex_filename
	||   !dvi_filename
	||   !working_directory
	||   !statement_caption_combined )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( budget_annualized_list ) ) return (BUDGET_LATEX *)0;

	budget_latex = budget_latex_calloc();

	budget_latex->latex =
		latex_new(
			tex_filename,
			dvi_filename,
			working_directory,
			0 /* not landscape_boolean */,
			statement_logo_filename );

	list_set(
		budget_latex->latex->table_list,
		budget_latex_table(
			statement_caption_combined,
			budget_annualized_list ) );

	return budget_latex;
}

BUDGET_LATEX *budget_latex_calloc( void )
{
	BUDGET_LATEX *budget_latex;

	if ( ! ( budget_latex = calloc( 1, sizeof( BUDGET_LATEX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return budget_latex;
}

BUDGET_PDF *budget_pdf_new(
			char *application_name,
			char *process_name,
			char *document_root_directory,
			char *transaction_begin_date_string,
			char *transaction_as_of_date,
			char *statement_logo_filename,
			char *statement_caption_combined,
			LIST *budget_annualized_list,
			pid_t process_id )
{
	BUDGET_PDF *budget_pdf;

	if ( !application_name
	||   !process_name
	||   !document_root_directory
	||   !transaction_begin_date_string
	||   !transaction_as_of_date
	||   !statement_caption_combined
	||   !process_id )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( budget_annualized_list ) ) return (BUDGET_PDF *)0;

	budget_pdf = budget_pdf_calloc();

	budget_pdf->statement_link =
		statement_link_new(
			application_name,
			process_name,
			document_root_directory,
			transaction_begin_date_string,
			transaction_as_of_date,
			(char *)0 /* statement_pdf_preclose_key */,
			process_id );

	budget_pdf->budget_latex =
		budget_latex_new(
			budget_pdf->statement_link->tex_filename,
			budget_pdf->statement_link->dvi_filename,
			budget_pdf->statement_link->working_directory,
			statement_logo_filename,
			statement_caption_combined,
			budget_annualized_list );

	return budget_pdf;
}

BUDGET_PDF *budget_pdf_calloc( void )
{
	BUDGET_PDF *budget_pdf;

	if ( ! ( budget_pdf = calloc( 1, sizeof( BUDGET_PDF ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return budget_pdf;
}

BUDGET_HTML *budget_html_new(
			char *statement_caption_subtitle,
			LIST *budget_annualized_list )
{
	BUDGET_HTML *budget_html;

	if ( !statement_caption_subtitle )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: statement_caption_subtitle is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( budget_annualized_list ) ) return (BUDGET_HTML *)0;

	budget_html = budget_html_calloc();

	budget_html->table =
		budget_html_table(
			statement_caption_subtitle,
			budget_annualized_list );

	return budget_html;
}

BUDGET_HTML *budget_html_calloc( void )
{
	BUDGET_HTML *budget_html;

	if ( ! ( budget_html = calloc( 1, sizeof( BUDGET_HTML ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return budget_html;
}

LIST *budget_html_heading_list( void )
{
	LIST *heading_list = list_new();

	list_set(
		heading_list,
		html_heading_new(
			"Account",
			0 /* not right_Justified_boolean */ ) );

	list_set(
		heading_list,
		html_heading_new(
			"Balance",
			1 /* right_Justified_boolean */ ) );

	list_set(
		heading_list,
		html_heading_new(
			"Annualized",
			1 /* right_Justified_boolean */ ) );

	list_set(
		heading_list,
		html_heading_new(
			"Budget",
			1 /* right_Justified_boolean */ ) );

	list_set(
		heading_list,
		html_heading_new(
			"Difference",
			1 /* right_Justified_boolean */ ) );

	return heading_list;
}

LIST *budget_html_row_list( LIST *budget_annualized_list )
{
	BUDGET_ANNUALIZED *budget_annualized;
	LIST *row_list;

	if ( !list_rewind( budget_annualized_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		budget_annualized = list_get( budget_annualized_list );

		if ( !budget_annualized->account
		||   !budget_annualized->account->account_journal_latest )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: account_journal_latest is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			row_list,
			budget_html_row(
				budget_annualized->account->account_name,
				budget_annualized->account_amount,
				budget_annualized->prior_account,
				budget_annualized->difference ) );

	} while ( list_next( budget_annualized_list ) );

	return row_list;
}

HTML_ROW *budget_html_row(
			char *account_name,
			double account_amount,
			ACCOUNT *prior_account,
			double difference )
{
	HTML_ROW *html_row;

	if ( !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: account_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_data_label( account_name /* name */ ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		/* --------------------- */
		/* Returns heap memory   */
		/* Doesn't trim pennies  */
		/* --------------------- */
		timlib_place_commas_in_money( account_amount ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		budget_amount_display( prior_account ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		budget_difference_display( difference ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	return html_row;
}

HTML_TABLE *budget_html_table(
			char *statement_caption_subtitle,
			LIST *budget_annualized_list )
{
	HTML_TABLE *html_table;

	if ( !statement_caption_subtitle )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: statement_caption_subtitle is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( budget_annualized_list ) ) return (HTML_TABLE *)0;

	html_table =
		html_table_new(
			(char *)0 /* title */,
			statement_caption_subtitle,
			(char *)0 /* sub_sub_title */ );

	html_table->heading_list = budget_html_heading_list();

	html_table->row_list =
		budget_html_row_list(
			budget_annualized_list );

	return html_table;
}

