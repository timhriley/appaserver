/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/budget.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include "String.h"
#include "timlib.h"
#include "date_convert.h"
#include "appaserver_error.h"
#include "date.h"
#include "float.h"
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

			budget_annualized->budget_statement_delta =
				statement_delta_new(
					budget_annualized->budget
						/* base_value */,
					budget_annualized->amount
						/* change_value */ );
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
			calloc( 1, sizeof ( BUDGET_ANNUALIZED ) ) ) )
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

BUDGET *budget_fetch(
		char *application_name,
		char *process_name,
		char *data_directory,
		char *as_of_date_string,
		char *output_medium_string )
{
	BUDGET *budget;

	if ( !application_name
	||   !process_name
	||   !data_directory
	||   !as_of_date_string
	||   !output_medium_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
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

	budget->transaction_date_begin_date_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		transaction_date_begin_date_string(
			TRANSACTION_TABLE,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			date_yyyy_mm_dd_string(
				budget->as_of_date ) );

	if ( !budget->transaction_date_begin_date_string )
	{
		char message[ 128 ];

		sprintf(message,
		"transaction_date_begin_date_string(%s) returned empty.",
			date_yyyy_mm_dd_string( budget->as_of_date ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	budget->begin_date =
		budget_begin_date(
			budget->transaction_date_begin_date_string );

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

	budget->transaction_date_close_boolean =
		transaction_date_close_boolean(
			TRANSACTION_TABLE,
			TRANSACTION_DATE_CLOSE_TIME,
			TRANSACTION_CLOSE_MEMO,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			date_yyyy_mm_dd_string(
				budget->as_of_date ) );

	budget->transaction_date_close_date_time_string =
		transaction_date_close_date_time_string(
			TRANSACTION_DATE_PRECLOSE_TIME,
			TRANSACTION_DATE_CLOSE_TIME,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			date_yyyy_mm_dd_string(
				budget->as_of_date ),
			budget->transaction_date_close_boolean
				/* preclose_time_boolean */ );

	budget->statement =
		statement_fetch(
			application_name,
			process_name,
			0 /* prior_year_count */,
			budget->element_name_list,
			budget->transaction_date_begin_date_string,
			budget->transaction_date_close_date_time_string
				/* end_date_time_string */,
			0 /* not fetch_transaction */ );

	element_list_account_statement_list_set(
		budget->statement->element_statement_list );

	budget->end_date_time_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		budget_end_date_time_string(
			budget->transaction_date_begin_date_string );

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

	budget->annualized_amount_net =
		budget_annualized_amount_net(
			budget->annualized_list );

	budget->annualized_budget_net =
		budget_annualized_budget_net(
			budget->annualized_list );

	budget->budget_statement_delta =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		statement_delta_new(
			budget->annualized_budget_net /* base_value */,
			budget->annualized_amount_net /* change_value */ );

	if ( budget->statement_output_medium == statement_output_PDF )
	{
		budget->year_percent_sub_title =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			budget_year_percent_sub_title(
				budget->
					statement->
					statement_caption->
					combined,
				budget->year_ratio );

		budget->budget_latex =
			budget_latex_new(
				application_name,
				process_name,
				data_directory,
				budget->
					statement->
					transaction_date_begin_date_string,
				budget->
					statement->
					end_date_time_string,
				budget->
					statement->
					statement_caption->
					logo_filename,
				budget->year_percent_sub_title,
				budget->annualized_list,
				budget->annualized_amount_net,
				budget->annualized_budget_net,
				budget->
					budget_statement_delta->
					cell_string,
				getpid() /* process_id */ );
	}
	else
	if ( budget->statement_output_medium == statement_output_table )
	{
		budget->year_percent_sub_title =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			budget_year_percent_sub_title(
				budget->
					statement->
					statement_caption->
					sub_title,
				budget->year_ratio );

		budget->budget_html =
			budget_html_new(
				budget->year_percent_sub_title,
				budget->annualized_list,
				budget->annualized_amount_net,
				budget->annualized_budget_net,
				budget->
					budget_statement_delta->
					cell_string );
	}

	return budget;
}

BUDGET *budget_calloc( void )
{
	BUDGET *budget;

	if ( ! ( budget = calloc( 1, sizeof ( BUDGET ) ) ) )
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
	char *date_string;
	boolean as_of_date_populated;

	as_of_date_populated =
		transaction_date_as_of_date_populated(
			as_of_date_string );

	if ( as_of_date_populated )
	{
		date_string = as_of_date_string;
	}
	else
	{
		date_string = date_yyyy_mm_dd;
	}

	return date_yyyy_mm_dd_new( date_string );
}

DATE *budget_begin_date( char *transaction_date_begin_date_string )
{
	if ( !transaction_date_begin_date_string )
	{
		char message[ 128 ];

		sprintf(message,
			"transaction_date_begin_date_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	date_yyyy_mm_dd_new(
		transaction_date_begin_date_string );
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
		char *transaction_date_begin_date_string )
{
	DATE *future_date;

	if ( !transaction_date_begin_date_string )
	{
		char message[ 128 ];

		sprintf(message,
			"transaction_date_begin_date_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	future_date =
		date_yyyy_mm_dd_new(
			transaction_date_begin_date_string );

	if ( !date_set_time_hhmmss(
		future_date,
		TRANSACTION_DATE_PRECLOSE_TIME ) )
	{
		char message[ 128 ];

		sprintf(message,
			"date_set_time_hhmmss() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
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
		STATEMENT_DELTA *budget_statement_delta,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;

	if ( !account_name
	||   !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				element_name ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				account_name ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			strdup(
				/* ---------------------- */
				/* Returns static memory. */
				/* Doesn't trim pennies.  */
				/* ---------------------- */
				string_commas_money(
					account_amount ) ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			strdup(
				/* ---------------------- */
				/* Returns static memory. */
				/* Doesn't trim pennies.  */
				/* ---------------------- */
				string_commas_money(
					annualized_amount ) ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			budget_display( budget ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			(budget_statement_delta)
				? budget_statement_delta->cell_string
				: (char *)0 ) );

	return
	latex_row_new( cell_list );
}

LIST *budget_latex_row_list(
		LIST *budget_annualized_list,
		LIST *budget_latex_column_list )
{
	LIST *row_list;
	BUDGET_ANNUALIZED *budget_annualized;

	if ( !list_length( budget_latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "budget_latex_column_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( budget_annualized_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		budget_annualized = list_get( budget_annualized_list );

		if ( !budget_annualized->account
		||   !budget_annualized->account->account_journal_latest )
		{
			char message[ 128 ];

			sprintf(message,
			"for account=[%s], account is empty or incomplete.",
				(budget_annualized->account)
					? budget_annualized->
						account->
						account_name
					: "null" );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set(
			row_list,
			budget_latex_row(
				budget_annualized->element_name,
				budget_annualized->account->account_name,
				budget_annualized->account_amount,
				budget_annualized->amount,
				budget_annualized->budget,
				budget_annualized->budget_statement_delta,
				budget_latex_column_list ) );

	} while ( list_next( budget_annualized_list ) );

	return row_list;
}

LIST *budget_latex_column_list( void )
{
	LIST *column_list = list_new();

	list_set(
		column_list,
		latex_column_new(
			"Element",
			latex_column_text /* latex_column_enum */,
			0 /* right_Justified_flag */,
			(char *)0 /* paragraph_size */,
			1 /* first_column_boolean */ ) );

	list_set(
		column_list,
		latex_column_new(
			"Account",
			latex_column_text /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	list_set(
		column_list,
		latex_column_new(
			"Balance",
			latex_column_float /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	list_set(
		column_list,
		latex_column_new(
			"Annualized",
			latex_column_float /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	list_set(
		column_list,
		latex_column_new(
			"Budget",
			latex_column_float /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	list_set(
		column_list,
		latex_column_new(
			"Difference",
			latex_column_float /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	return column_list;
}

LATEX_TABLE *budget_latex_table(
		char *budget_year_percent_sub_title,
		LIST *budget_annualized_list,
		double annualized_amount_net,
		double annualized_budget_net,
		char *delta_cell_string )
{
	LIST *latex_column_list;
	LIST *latex_row_list;

	if ( !budget_year_percent_sub_title
	||   !list_length( budget_annualized_list ) )
	{
		return NULL;
	}

	latex_column_list = budget_latex_column_list();

	latex_row_list =
		budget_latex_row_list(
			budget_annualized_list,
			latex_column_list );

	list_set(
		latex_row_list,
		budget_latex_sum_row(
			annualized_amount_net,
			annualized_budget_net,
			delta_cell_string,
			latex_column_list ) );

	return
	latex_table_new(
		budget_year_percent_sub_title /* title */,
		latex_column_list,
		latex_row_list );
}

BUDGET_LATEX *budget_latex_new(
		char *application_name,
		char *process_name,
		char *data_directory,
		char *transaction_date_begin_date_string,
		char *statement_end_date_time_string,
		char *statement_caption_logo_filename,
		char *budget_year_percent_sub_title,
		LIST *budget_annualized_list,
		double annualized_amount_net,
		double annualized_budget_net,
		char *delta_cell_string,
		pid_t process_id )
{
	BUDGET_LATEX *budget_latex;

	if ( !application_name
	||   !process_name
	||   !data_directory
	||   !budget_year_percent_sub_title )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( budget_annualized_list ) ) return NULL;

	budget_latex = budget_latex_calloc();

	budget_latex->statement_link =
		statement_link_new(
			application_name,
			process_name,
			data_directory,
			transaction_date_begin_date_string,
			statement_end_date_time_string,
			process_id );

	budget_latex->latex =
		latex_new(
			budget_latex->statement_link->tex_filename,
			budget_latex->
				statement_link->
				appaserver_link_working_directory,
			0 /* not landscape_boolean */,
			statement_caption_logo_filename );

	budget_latex->latex_table =
		budget_latex_table(
			budget_year_percent_sub_title,
			budget_annualized_list,
			annualized_amount_net,
			annualized_budget_net,
			delta_cell_string );

	return budget_latex;
}

BUDGET_LATEX *budget_latex_calloc( void )
{
	BUDGET_LATEX *budget_latex;

	if ( ! ( budget_latex = calloc( 1, sizeof ( BUDGET_LATEX ) ) ) )
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

BUDGET_HTML *budget_html_new(
		char *budget_year_percent_sub_title,
		LIST *budget_annualized_list,
		double annualized_amount_net,
		double annualized_budget_net,
		char *delta_cell_string )
{
	BUDGET_HTML *budget_html;

	if ( !budget_year_percent_sub_title )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: budget_year_percent_sub_title is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( budget_annualized_list ) ) return (BUDGET_HTML *)0;

	budget_html = budget_html_calloc();

	budget_html->html_table =
		budget_html_table(
			budget_year_percent_sub_title,
			budget_annualized_list,
			annualized_amount_net,
			annualized_budget_net,
			delta_cell_string );

	return budget_html;
}

BUDGET_HTML *budget_html_calloc( void )
{
	BUDGET_HTML *budget_html;

	if ( ! ( budget_html = calloc( 1, sizeof ( BUDGET_HTML ) ) ) )
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

LIST *budget_html_column_list( void )
{
	LIST *column_list = list_new();

	list_set(
		column_list,
		html_column_new(
			"Element",
			0 /* not right_Justified_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"Account",
			0 /* not right_Justified_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"Balance",
			1 /* right_Justified_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"Annualized",
			1 /* right_Justified_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"Budget",
			1 /* right_Justified_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"Difference",
			1 /* right_Justified_boolean */ ) );

	return column_list;
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
				budget_annualized->budget_statement_delta ) );

	} while ( list_next( budget_annualized_list ) );

	return row_list;
}

HTML_ROW *budget_html_sum_row(
		double annualized_amount_net,
		double annualized_budget_net,
		char *delta_cell_string )
{
	LIST *cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			"Net",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
			html_cell_new(
			(char *)0,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
			html_cell_new(
			strdup(
				/* ---------------------- */
				/* Returns static memory. */
				/* Doesn't trim pennies. */
				/* ---------------------- */
				string_commas_money(
					annualized_amount_net ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
			html_cell_new(
			strdup(
				/* ---------------------- */
				/* Returns static memory. */
				/* Doesn't trim pennies. */
				/* ---------------------- */
				string_commas_money(
					annualized_budget_net ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );


	list_set(
		cell_list,
			html_cell_new(
				delta_cell_string,
				0 /* not large_boolean */,
				0 /* not bold_boolean */ ) );

	return
	html_row_new( cell_list );
}

HTML_ROW *budget_html_row(
		char *element_name,
		char *account_name,
		double account_amount,
		double annualized_amount,
		double budget,
		STATEMENT_DELTA *budget_statement_delta )
{
	LIST *cell_list;

	if ( !account_name )
	{
		char message[ 128 ];

		sprintf(message, "account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				element_name ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			/* ------------------------- */
			/* Returns heap memory or "" */
			/* ------------------------- */
			statement_cell_label_datum(
				account_name ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* ---------------------- */
				/* Returns static memory. */
				/* Doesn't trim pennies. */
				/* ---------------------- */
				string_commas_money(
					account_amount ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* ---------------------- */
				/* Returns static memory. */
				/* Doesn't trim pennies. */
				/* ---------------------- */
				string_commas_money(
					annualized_amount ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );


	list_set(
		cell_list,
		html_cell_new(
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			budget_display( budget ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(budget_statement_delta)
				? budget_statement_delta->cell_string
				: (char *)0,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	return
	html_row_new( cell_list );
}

HTML_TABLE *budget_html_table(
		char *budget_year_percent_sub_title,
		LIST *budget_annualized_list,
		double annualized_amount_net,
		double annualized_budget_net,
		char *delta_cell_string )
{
	HTML_TABLE *html_table;

	if ( !budget_year_percent_sub_title )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: budget_year_percent_sub_title is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( budget_annualized_list ) ) return (HTML_TABLE *)0;

	html_table =
		html_table_new(
			(char *)0 /* title */,
			budget_year_percent_sub_title,
			(char *)0 /* sub_sub_title */ );

	html_table->column_list = budget_html_column_list();

	html_table->row_list =
		budget_html_row_list(
			budget_annualized_list );

	list_set(
		html_table->row_list,
		budget_html_sum_row(
			annualized_amount_net,
			annualized_budget_net,
			delta_cell_string ) );

	return html_table;
}

LATEX_ROW *budget_latex_sum_row(
		double annualized_amount_net,
		double annualized_budget_net,
		char *delta_cell_string,
		LIST *latex_column_list )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;

	if ( !list_rewind( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "latex_column_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			"Net" ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			(char *)0 ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			(char *)0 ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			strdup(
			     string_commas_money(
				    annualized_amount_net ) ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			strdup(
			     string_commas_money(
				    annualized_budget_net ) ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			delta_cell_string ) );

	return
	latex_row_new( cell_list );
}

double budget_annualized_amount_net( LIST *budget_annualized_list )
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

double budget_annualized_budget_net(
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

char *budget_year_percent_sub_title(
		char *input_sub_title,
		double budget_year_ratio )
{
	static char sub_title[ 256 ];

	if ( !input_sub_title )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: input_sub_title is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(sub_title,
		"%s, Year Percent: %d",
		input_sub_title,
		float_ratio_to_percent(
			budget_year_ratio ) );

	return sub_title;
}

