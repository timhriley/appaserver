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
			double budget_year_ratio,
			LIST *element_statement_list,
			STATEMENT_PRIOR_YEAR *statement_prior_year )
{
	LIST *list;
	ELEMENT *element;
	ACCOUNT *account;

	if ( !list_rewind( element_statement_list ) ) return (LIST *)0;

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

			list_set(
				list,
				budget_annualized_new(
					budget_year_ratio,
					element->element_name,
					account,
					statement_prior_year ) );

		} while ( list_next( element->account_statement_list ) );

	} while ( list_next( element_statement_list ) );

	return list;
}

BUDGET_ANNUALIZED *budget_annualized_new(
			double budget_year_ratio,
			char *element_name,
			ACCOUNT *account,
			STATEMENT_PRIOR_YEAR *statement_prior_year )
{
	BUDGET_ANNUALIZED *budget_annualized;

	if ( !account
	||   !account->account_journal_latest )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	budget_annualized = budget_annualized_calloc();

	budget_annualized->element_name = element_name;
	budget_annualized->account = account;

	budget_annualized->account_amount =
		budget_annualized_account_amount(
			account->account_journal_latest->balance
				/* account_latest_balance */,
			account->annual_budget );

	budget_annualized->amount =
		budget_annualized_amount(
			budget_annualized->account_amount,
			budget_year_ratio );

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

			budget_annualized->budget =
				budget_annualized_budget(
					budget_annualized->
						prior_account->
						account_journal_latest->
						balance
						  /* prior_account_balance */ );

			budget_annualized->difference =
			   budget_annualized_difference(
				budget_annualized->amount,
				budget_annualized->budget );

			budget_annualized->difference_percent =
			   budget_annualized_difference_percent(
				budget_annualized->budget,
				budget_annualized->difference );

			budget_annualized->difference_cell =
			   budget_annualized_difference_cell(
				budget_annualized->difference,
				budget_annualized->difference_percent );
		}
	}

	return budget_annualized;
}

double budget_annualized_budget( double prior_account_balance )
{
	return prior_account_balance;
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

int budget_days_so_far(
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
		begin_date /* earlier_date */ ) + 1;
}

int budget_days_in_year( DATE *begin_date )
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

double budget_year_ratio(
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
			double amount,
			double budget )
{
	return amount - budget;
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

	budget->as_of_date =
		budget_as_of_date(
			as_of_date_string,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			date_now_yyyy_mm_dd( date_utc_offset() ) );

	budget->transaction_begin_date_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		transaction_begin_date_string(
			TRANSACTION_TABLE,
			date_display_yyyy_mm_dd(
				budget->as_of_date ) );

	if ( !budget->transaction_begin_date_string )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: transaction_begin_date_string() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	budget->begin_date =
		budget_begin_date(
			budget->transaction_begin_date_string );

	budget->days_so_far =
		budget_days_so_far(
			budget->begin_date,
			budget->as_of_date );

	budget->days_in_year =
		budget_days_in_year(
			budget->begin_date );

	budget->year_ratio =
		budget_year_ratio(
			budget->days_so_far,
			budget->days_in_year );

	budget->element_name_list =
		budget_element_name_list(
			ELEMENT_REVENUE,
			ELEMENT_EXPENSE );

	budget->transaction_closing_entry_exists =
		transaction_closing_entry_exists(
			TRANSACTION_TABLE,
			TRANSACTION_CLOSE_TIME,
			date_display_yyyy_mm_dd(
				budget->as_of_date ) );

	budget->transaction_date_time_closing =
		transaction_date_time_closing(
			TRANSACTION_PRECLOSE_TIME,
			TRANSACTION_CLOSE_TIME,
			date_display_yyyy_mm_dd(
				budget->as_of_date ),
			budget->transaction_closing_entry_exists
				/* preclose_time_boolean */ );

	budget->statement =
		statement_fetch(
			application_name,
			process_name,
			budget->element_name_list,
			budget->transaction_begin_date_string,
			date_display_yyyy_mm_dd(
				budget->as_of_date ),
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
			budget->year_ratio,
			budget->statement->element_statement_list,
			list_first( budget->statement_prior_year_list )
				/* statement_prior_year */ );

	budget->annualized_net_asset_change =
		budget_annualized_net_asset_change(
			budget->annualized_list );

	budget->annualized_budget_change =
		budget_annualized_budget_change(
			budget->annualized_list );

	if ( budget->statement_output_medium == statement_output_PDF )
	{
		budget->year_percent_subtitle =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			budget_year_percent_subtitle(
				budget->
					statement->
					statement_caption->
					combined,
				budget->year_ratio );

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
				budget->year_percent_subtitle,
				budget->annualized_list,
				budget->annualized_net_asset_change,
				budget->annualized_budget_change,
				getpid() /* process_id */ );
	}
	else
	if ( budget->statement_output_medium == statement_output_table )
	{
		budget->year_percent_subtitle =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			budget_year_percent_subtitle(
				budget->
					statement->
					statement_caption->
					subtitle,
				budget->year_ratio );

		budget->budget_html =
			budget_html_new(
				budget->year_percent_subtitle,
				budget->annualized_list,
				budget->annualized_net_asset_change,
				budget->annualized_budget_change );
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

DATE *budget_as_of_date(
			char *as_of_date_string,
			char *date_yyyy_mm_dd )
{
	char *use_date_string;

	if ( !as_of_date_string
	||   !*as_of_date_string
	||   strcmp( as_of_date_string, "as_of_date" ) == 0 )
	{
		use_date_string = date_yyyy_mm_dd;
	}
	else
	{
		use_date_string = as_of_date_string;
	}

	return date_yyyy_mm_dd_new( use_date_string );
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

	date_decrement_days( future_date, 1.0 );
	date_increment_years( future_date, 1 );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	date_display19( future_date );
}

char *budget_display( double budget )
{
	if ( budget )
	{
		return
		/* --------------------- */
		/* Returns heap memory   */
		/* Doesn't trim pennies  */
		/* --------------------- */
		timlib_place_commas_in_money( budget );
	}
	else
	{
		return (char *)0;
	}
}

LATEX_ROW *budget_latex_row(
			char *element_name,
			char *account_name,
			double account_amount,
			double annualized_amount,
			double budget,
			char *difference_cell )
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
		statement_cell_data_label( element_name /* name */ ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

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
		/* --------------------- */
		/* Returns heap memory   */
		/* Doesn't trim pennies  */
		/* --------------------- */
		timlib_place_commas_in_money( annualized_amount ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		budget_display( budget ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		difference_cell,
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
				budget_annualized->element_name,
				budget_annualized->account->account_name,
				budget_annualized->account_amount,
				budget_annualized->amount,
				budget_annualized->budget,
				budget_annualized->difference_cell ) );

	} while ( list_next( budget_annualized_list ) );

	return row_list;
}

LIST *budget_latex_heading_list( void )
{
	LIST *heading_list = list_new();

	list_set(
		heading_list,
		latex_table_heading_new(
			"Element",
			0 /* right_Justified_flag */,
			(char *)0 /* paragraph_size */ ) );

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
			char *budget_year_percent_subtitle,
			LIST *budget_annualized_list,
			double net_asset_change,
			double budget_change )
{
	LATEX_TABLE *latex_table;

	if ( !budget_year_percent_subtitle
	||   !list_length( budget_annualized_list ) )
	{
		return (LATEX_TABLE *)0;
	}

	latex_table =
		latex_table_new(
			budget_year_percent_subtitle );

	latex_table->heading_list =
		budget_latex_heading_list();

	latex_table->row_list =
		budget_latex_row_list(
			budget_annualized_list );

	list_set(
		latex_table->row_list,
		budget_latex_sum_row(
			net_asset_change,
			budget_change ) );

	return latex_table;
}

BUDGET_LATEX *budget_latex_new(
			char *tex_filename,
			char *dvi_filename,
			char *working_directory,
			char *statement_logo_filename,
			char *budget_year_percent_subtitle,
			LIST *budget_annualized_list,
			double net_asset_change,
			double budget_change )
{
	BUDGET_LATEX *budget_latex;

	if ( !tex_filename
	||   !dvi_filename
	||   !working_directory
	||   !budget_year_percent_subtitle )
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
			budget_year_percent_subtitle,
			budget_annualized_list,
			net_asset_change,
			budget_change ) );

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
			char *budget_year_percent_subtitle,
			LIST *budget_annualized_list,
			double net_asset_change,
			double budget_change,
			pid_t process_id )
{
	BUDGET_PDF *budget_pdf;

	if ( !application_name
	||   !process_name
	||   !document_root_directory
	||   !transaction_begin_date_string
	||   !transaction_as_of_date
	||   !budget_year_percent_subtitle
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
			budget_year_percent_subtitle,
			budget_annualized_list,
			net_asset_change,
			budget_change );

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
			char *budget_year_percent_subtitle,
			LIST *budget_annualized_list,
			double net_asset_change,
			double budget_change )
{
	BUDGET_HTML *budget_html;

	if ( !budget_year_percent_subtitle )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: budget_year_percent_subtitle is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( budget_annualized_list ) ) return (BUDGET_HTML *)0;

	budget_html = budget_html_calloc();

	budget_html->table =
		budget_html_table(
			budget_year_percent_subtitle,
			budget_annualized_list,
			net_asset_change,
			budget_change );

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
			"Element",
			0 /* not right_Justified_boolean */ ) );

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
				budget_annualized->element_name,
				budget_annualized->account->account_name,
				budget_annualized->account_amount,
				budget_annualized->amount,
				budget_annualized->budget,
				budget_annualized->difference_cell ) );

	} while ( list_next( budget_annualized_list ) );

	return row_list;
}

HTML_ROW *budget_html_sum_row(
			double net_asset_change,
			double budget_change )
{
	HTML_ROW *html_row = html_row_new();

	html_cell_data_set(
		html_row->cell_list,
		"Net",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		(char *)0,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		(char *)0,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		/* --------------------- */
		/* Returns heap memory   */
		/* Doesn't trim pennies  */
		/* --------------------- */
		timlib_place_commas_in_money( net_asset_change ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		/* --------------------- */
		/* Returns heap memory   */
		/* Doesn't trim pennies  */
		/* --------------------- */
		timlib_place_commas_in_money( budget_change ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		(char *)0,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	return html_row;
}

HTML_ROW *budget_html_row(
			char *element_name,
			char *account_name,
			double account_amount,
			double annualized_amount,
			double budget,
			char *difference_cell )
{
	HTML_ROW *html_row;

	if ( !account_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: account_name is is empty.\n",
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
		statement_cell_data_label( element_name /* name */ ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

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
		/* --------------------- */
		/* Returns heap memory   */
		/* Doesn't trim pennies  */
		/* --------------------- */
		timlib_place_commas_in_money( annualized_amount ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		budget_display( budget ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		difference_cell,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	return html_row;
}

HTML_TABLE *budget_html_table(
			char *budget_year_percent_subtitle,
			LIST *budget_annualized_list,
			double net_asset_change,
			double budget_change )
{
	HTML_TABLE *html_table;

	if ( !budget_year_percent_subtitle )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: budget_year_percent_subtitle is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( budget_annualized_list ) ) return (HTML_TABLE *)0;

	html_table =
		html_table_new(
			(char *)0 /* title */,
			budget_year_percent_subtitle,
			(char *)0 /* sub_sub_title */ );

	html_table->heading_list = budget_html_heading_list();

	html_table->row_list =
		budget_html_row_list(
			budget_annualized_list );

	list_set(
		html_table->row_list,
		budget_html_sum_row(
			net_asset_change,
			budget_change ) );

	return html_table;
}

LATEX_ROW *budget_latex_sum_row(
			double net_asset_change,
			double budget_change )
{
	LATEX_ROW *latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		"Net",
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		(char *)0,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		(char *)0,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		/* --------------------- */
		/* Returns heap memory   */
		/* Doesn't trim pennies  */
		/* --------------------- */
		timlib_place_commas_in_money( net_asset_change ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		/* --------------------- */
		/* Returns heap memory   */
		/* Doesn't trim pennies  */
		/* --------------------- */
		timlib_place_commas_in_money( budget_change ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		(char *)0,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	return latex_row;
}

double budget_annualized_net_asset_change(
			LIST *budget_annualized_list )
{
	double revenue_sum;
	double expense_sum;

	revenue_sum =
		budget_annualized_amount_sum(
			ELEMENT_REVENUE,
			budget_annualized_list );

	expense_sum =
		budget_annualized_amount_sum(
			ELEMENT_EXPENSE,
			budget_annualized_list );

	return revenue_sum - expense_sum;
}

double budget_annualized_amount_sum(
			char *element_name,
			LIST *budget_annualized_list )
{
	BUDGET_ANNUALIZED *budget_annualized;
	double sum;

	if ( !list_rewind( budget_annualized_list ) ) return 0.0;

	sum = 0.0;

	do {
		budget_annualized =
			list_get(
				budget_annualized_list );

		if ( string_strcmp(
			budget_annualized->element_name,
			element_name ) == 0 )
		{
			sum += budget_annualized->amount;
		}

	} while ( list_next( budget_annualized_list ) );

	return sum;
}

double budget_annualized_budget_change(
			LIST *budget_annualized_list )
{
	double revenue_sum;
	double expense_sum;

	revenue_sum =
		budget_annualized_budget_sum(
			ELEMENT_REVENUE,
			budget_annualized_list );

	expense_sum =
		budget_annualized_budget_sum(
			ELEMENT_EXPENSE,
			budget_annualized_list );

	return revenue_sum - expense_sum;
}

double budget_annualized_budget_sum(
			char *element_name,
			LIST *budget_annualized_list )
{
	BUDGET_ANNUALIZED *budget_annualized;
	double sum;

	if ( !list_rewind( budget_annualized_list ) ) return 0.0;

	sum = 0.0;

	do {
		budget_annualized =
			list_get(
				budget_annualized_list );

		if ( string_strcmp(
			budget_annualized->element_name,
			element_name ) == 0 )
		{
			sum += budget_annualized->budget;
		}

	} while ( list_next( budget_annualized_list ) );

	return sum;
}

char *budget_year_percent_subtitle(
			char *input_subtitle,
			double budget_year_ratio )
{
	static char subtitle[ 256 ];

	if ( !input_subtitle )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: input_subtitle is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

/*
	sprintf(subtitle,
		"%s; Year Percent: %d%c",
		input_subtitle,
		float_ratio_to_percent(
			budget_year_ratio ),
		'%' );
*/

	sprintf(subtitle,
		"%s, Year Percent: %d",
		input_subtitle,
		float_ratio_to_percent(
			budget_year_ratio ) );

	return subtitle;
}

char *budget_annualized_difference_cell(
			double budget_annualized_difference,
			int budget_annualized_difference_percent )
{
	char difference_cell[ 32 ];

	sprintf(difference_cell,
		"%d%c %s",
		budget_annualized_difference_percent,
		'%',
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		timlib_place_commas_in_dollars(
			budget_annualized_difference ) );

	return strdup( difference_cell );
}

int budget_annualized_difference_percent(
			double budget,
			double difference )
{
	double delta_percent;

	if ( !budget ) return 0;

	delta_percent = (difference / budget) * 100.0;

	return float_round_int( delta_percent );
}

