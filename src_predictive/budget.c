/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/budget.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include "String.h"
#include "date_convert.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "application.h"
#include "date.h"
#include "float.h"
#include "piece.h"
#include "element.h"
#include "journal.h"
#include "transaction.h"
#include "transaction_date.h"
#include "budget.h"

LIST *budget_annualized_list(
		char *application_name,
		char *session_key,
		char *process_name,
		char *appaserver_parameter_data_directory,
		char *transaction_date_begin_date_string,
		LIST *element_statement_list,
		STATEMENT_PRIOR_YEAR *statement_prior_year,
		char *budget_forecast_date_string,
		char *budget_forecast_julian_string,
		char *appaserver_link_working_directory )
{
	LIST *list;
	ELEMENT *element;
	ACCOUNT *account;
	BUDGET_ANNUALIZED *budget_annualized;

	if ( !application_name
	||   !session_key
	||   !process_name
	||   !appaserver_parameter_data_directory
	||   !transaction_date_begin_date_string
	||   !list_rewind( element_statement_list )
	||   !budget_forecast_date_string
	||   !budget_forecast_julian_string
	||   !appaserver_link_working_directory )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	list = list_new();

	do {
		element = list_get( element_statement_list );

		if ( list_rewind( element->account_statement_list ) )
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

			budget_annualized =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				budget_annualized_new(
					application_name,
					session_key,
					process_name,
					appaserver_parameter_data_directory,
					transaction_date_begin_date_string,
					element->element_name,
					account,
					statement_prior_year,
					budget_forecast_date_string,
					budget_forecast_julian_string,
					appaserver_link_working_directory );

			list_set_order(
				list,
				budget_annualized,
				budget_annualized_compare_function );

		} while ( list_next( element->account_statement_list ) );

	} while ( list_next( element_statement_list ) );

	return list;
}

BUDGET_ANNUALIZED *budget_annualized_new(
		char *application_name,
		char *session_key,
		char *process_name,
		char *appaserver_parameter_data_directory,
		char *transaction_date_begin_date_string,
		char *element_name,
		ACCOUNT *account,
		STATEMENT_PRIOR_YEAR *statement_prior_year,
		char *budget_forecast_date_string,
		char *budget_forecast_julian_string,
		char *appaserver_link_working_directory )
{
	BUDGET_ANNUALIZED *budget_annualized;

	if ( !account
	||   !account->account_journal_latest )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"account is empty or incomplete." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	budget_annualized = budget_annualized_calloc();

	budget_annualized->element_name = element_name;
	budget_annualized->account = account;

	budget_annualized->budget_regression =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		budget_regression_fetch(
			application_name,
			session_key,
			process_name,
			appaserver_parameter_data_directory,
			transaction_date_begin_date_string,
			account->account_name,
			budget_forecast_date_string,
			budget_forecast_julian_string,
			appaserver_link_working_directory );

	budget_annualized->budget_integer =
		budget_annualized_budget_integer(
			statement_prior_year,
			account->account_name,
			account->annual_budget );

	budget_annualized->amount_integer =
		budget_annualized_amount_integer(
			account->
				account_journal_latest->
				balance /* account_amount */,
			budget_annualized->
				budget_regression->
				forecast_integer,
			budget_annualized->budget_integer );

	budget_annualized->statement_delta =
		statement_delta_new(
			(double)budget_annualized->budget_integer
				/* base_value */,
			(double)budget_annualized->amount_integer
				/* change_value */ );

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
		DATE *date_now,
		DATE *begin_date )
{
	if ( !date_now
	||   !begin_date )
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
		date_now /* later_date */,
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

BUDGET *budget_fetch(
		char *application_name,
		char *session_key,
		char *process_name,
		char *output_medium_string,
		char *data_directory )
{
	BUDGET *budget;
	DATE *date_now;
	char *display_yyyy_mm_dd;

	if ( !application_name
	||   !session_key
	||   !process_name
	||   !output_medium_string
	||   !data_directory )
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

	date_now =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		date_now_new(
			date_utc_offset() );

	display_yyyy_mm_dd =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		date_display_yyyy_mm_dd(
			date_now );

	budget->transaction_date_begin_date_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		transaction_date_begin_date_string(
			TRANSACTION_TABLE,
			display_yyyy_mm_dd );

	if ( !budget->transaction_date_begin_date_string )
	{
		char message[ 128 ];

		sprintf(message,
		"transaction_date_begin_date_string() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	budget->begin_date =
		budget_begin_date(
			budget->
				transaction_date_begin_date_string );

	if ( !budget->begin_date )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"budget_begin_date() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	budget->days_so_far =
		budget_days_so_far(
			date_now,
			budget->begin_date );

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
			ELEMENT_EXPENSE,
			ELEMENT_EXPENDITURE );

	budget->transaction_date_close_boolean =
		transaction_date_close_boolean(
			TRANSACTION_TABLE,
			TRANSACTION_DATE_CLOSE_TIME,
			TRANSACTION_CLOSE_MEMO,
			display_yyyy_mm_dd );

	budget->transaction_date_close_date_time =
		transaction_date_close_date_time(
			TRANSACTION_DATE_PRECLOSE_TIME,
			TRANSACTION_DATE_CLOSE_TIME,
			display_yyyy_mm_dd
				/* transaction_as_of_date */,
			budget->transaction_date_close_boolean
				/* preclose_time_boolean */ );

	budget->statement =
		statement_fetch(
			application_name,
			process_name,
			0 /* prior_year_count */,
			budget->element_name_list,
			budget->transaction_date_begin_date_string,
			budget->transaction_date_close_date_time
				/* end_date_time */,
			0 /* not fetch_transaction */ );

	element_list_account_statement_list_set(
		budget->statement->element_statement_list );

	budget->end_date_time =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		budget_end_date_time(
			budget->transaction_date_begin_date_string );

	budget->statement_prior_year_list =
		statement_prior_year_list(
				budget->element_name_list,
				budget->end_date_time,
				1 /* prior_year_count */,
				budget->statement );

	budget->forecast_date =
		budget_forecast_date(
			budget->begin_date,
			budget->days_in_year );

	budget->forecast_date_string =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		budget_forecast_date_string(
			budget->forecast_date );

	budget->forecast_julian_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		budget_forecast_julian_string(
			budget->forecast_date_string );

	budget->appaserver_link_working_directory =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_working_directory(
			data_directory,
			application_name );

	budget->budget_annualized_list =
		budget_annualized_list(
			application_name,
			session_key,
			process_name,
			data_directory,
			budget->transaction_date_begin_date_string,
			budget->statement->element_statement_list,
			list_first( budget->statement_prior_year_list )
				/* statement_prior_year */,
			budget->forecast_date_string,
			budget->forecast_julian_string,
			budget->appaserver_link_working_directory );

	budget->amount_net =
		budget_amount_net(
			budget->budget_annualized_list );

	budget->annualized_amount_net =
		budget_annualized_amount_net(
			budget->budget_annualized_list );

	budget->annualized_budget_net =
		budget_annualized_budget_net(
			budget->budget_annualized_list );

	budget->statement_delta =
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
					end_date_time,
				budget->
					statement->
					statement_caption->
					logo_filename,
				budget->year_percent_sub_title,
				budget->budget_annualized_list,
				budget->amount_net,
				budget->annualized_amount_net,
				budget->annualized_budget_net,
				budget->
					statement_delta->
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
				budget->budget_annualized_list,
				budget->amount_net,
				budget->annualized_amount_net,
				budget->annualized_budget_net,
				budget->
					statement_delta->
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
		const char *element_revenue,
		const char *element_expense,
		const char *element_expenditure )
{
	LIST *name_list = list_new();

	list_set( name_list, (char *)element_revenue );
	list_set( name_list, (char *)element_expense );
	list_set( name_list, (char *)element_expenditure );

	return name_list;
}

char *budget_end_date_time(
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

char *budget_display( int budget_integer )
{
	if ( budget_integer )
	{
		return
		strdup(
			/* ----------------------- */
			/* Returns static memory   */
			/* ----------------------- */
			string_commas_integer( budget_integer ) );
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
		int annualized_amount_integer,
		int annualized_budget_integer,
		STATEMENT_DELTA *statement_delta,
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
			0 /* not first_row_boolean */,
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
			0 /* not first_row_boolean */,
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
			0 /* not first_row_boolean */,
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
			0 /* not first_row_boolean */,
			strdup(
				/* ---------------------- */
				/* Returns static memory. */
				/* ---------------------- */
				string_commas_integer(
					annualized_amount_integer ) ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			budget_display( annualized_budget_integer ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			(statement_delta)
				? statement_delta->cell_string
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
				budget_annualized->
					account->
					account_journal_latest->
					balance
						/* account_amount */,
				budget_annualized->amount_integer,
				budget_annualized->budget_integer,
				budget_annualized->statement_delta,
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
			STATEMENT_ELEMENT_HEADING,
			latex_column_text /* latex_column_enum */,
			0 /* right_Justified_flag */,
			(char *)0 /* paragraph_size */,
			1 /* first_column_boolean */ ) );

	list_set(
		column_list,
		latex_column_new(
			STATEMENT_ACCOUNT_HEADING,
			latex_column_text /* latex_column_enum */,
			0 /* float_decimal_count */,
			(char *)0 /* paragraph_size */,
			0 /* not first_column_boolean */ ) );

	list_set(
		column_list,
		latex_column_new(
			STATEMENT_BALANCE_HEADING,
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
		double budget_amount_net,
		int annualized_amount_net,
		int annualized_budget_net,
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
			budget_amount_net,
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
		char *statement_end_date_time,
		char *statement_caption_logo_filename,
		char *budget_year_percent_sub_title,
		LIST *budget_annualized_list,
		double budget_amount_net,
		int annualized_amount_net,
		int annualized_budget_net,
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
			statement_end_date_time,
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
			budget_amount_net,
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
		double budget_amount_net,
		int annualized_amount_net,
		int annualized_budget_net,
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
			budget_amount_net,
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
			STATEMENT_ELEMENT_HEADING,
			0 /* not right_Justified_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			STATEMENT_ACCOUNT_HEADING,
			0 /* not right_Justified_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			STATEMENT_BALANCE_HEADING,
			1 /* right_Justified_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"RowCount",
			1 /* right_Justified_boolean */ ) );

	list_set(
		column_list,
		html_column_new(
			"Confidence",
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
				budget_annualized->
					account->
					account_journal_latest->
					balance
					/* account_amount */,
				budget_annualized->
					budget_regression->
					row_count,
				budget_annualized->
					budget_regression->
					confidence_integer,
				budget_annualized->amount_integer,
				budget_annualized->budget_integer,
				budget_annualized->statement_delta ) );

	} while ( list_next( budget_annualized_list ) );

	return row_list;
}

HTML_ROW *budget_html_sum_row(
		double budget_amount_net,
		int annualized_amount_net,
		int annualized_budget_net,
		char *delta_cell_string )
{
	LIST *cell_list = list_new();

	list_set(
		cell_list,
		html_cell_new(
			"Net",
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	/* Account is blank */
	/* ---------------- */
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
					budget_amount_net ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	/* RowCount is blank */
	/* ----------------- */
	list_set(
		cell_list,
			html_cell_new(
			(char *)0,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	/* Confidence is blank */
	/* ------------------- */
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
				/* ---------------------- */
				string_commas_integer(
					annualized_amount_net ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
			html_cell_new(
			strdup(
				/* ---------------------- */
				/* Returns static memory. */
				/* ---------------------- */
				string_commas_integer(
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
		int row_count,
		int confidence_integer,
		int annualized_amount_integer,
		int annualized_budget_integer,
		STATEMENT_DELTA *statement_delta )
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
				/* --------------------------- */
				/* Returns static memory or "" */
				/* --------------------------- */
				string_commas_integer(
					row_count ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* --------------------------- */
				/* Returns static memory or "" */
				/* --------------------------- */
				string_commas_integer(
					confidence_integer ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* ---------------------- */
				/* Returns static memory. */
				/* ---------------------- */
				string_commas_integer(
					annualized_amount_integer ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );


	list_set(
		cell_list,
		html_cell_new(
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			budget_display( annualized_budget_integer ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			(statement_delta)
				? statement_delta->cell_string
				: (char *)0,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	return
	html_row_new( cell_list );
}

HTML_TABLE *budget_html_table(
		char *budget_year_percent_sub_title,
		LIST *budget_annualized_list,
		double budget_amount_net,
		int annualized_amount_net,
		int annualized_budget_net,
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
			budget_amount_net,
			annualized_amount_net,
			annualized_budget_net,
			delta_cell_string ) );

	return html_table;
}

LATEX_ROW *budget_latex_sum_row(
		double budget_amount_net,
		int annualized_amount_net,
		int annualized_budget_net,
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
			0 /* not first_row_boolean */,
			"Net" ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			(char *)0 ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			strdup(
			     /* ---------------------- */
			     /* Returns static memory. */
			     /* ---------------------- */
			     string_commas_money(
				    budget_amount_net ) ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			strdup(
			     /* ---------------------- */
			     /* Returns static memory. */
			     /* ---------------------- */
			     string_commas_integer(
				    annualized_amount_net ) ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			strdup(
			     /* ---------------------- */
			     /* Returns static memory. */
			     /* ---------------------- */
			     string_commas_integer(
				    annualized_budget_net ) ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			delta_cell_string ) );

	return
	latex_row_new( cell_list );
}

int budget_annualized_amount_net( LIST *budget_annualized_list )
{
	int revenue_sum;
	int expense_sum;

	revenue_sum =
		budget_annualized_amount_sum(
			ELEMENT_REVENUE,
			budget_annualized_list );

	expense_sum =
		budget_annualized_amount_sum(
			/* ------------------------ */
			/* Returns either parameter */
			/* ------------------------ */
			(char *)element_resolve_expense(
				ELEMENT_EXPENSE,
				ELEMENT_EXPENDITURE ),
			budget_annualized_list );

	return revenue_sum - expense_sum;
}

int budget_annualized_amount_sum(
		const char *element_name,
		LIST *budget_annualized_list )
{
	BUDGET_ANNUALIZED *budget_annualized;
	int sum = 0;

	if ( list_rewind( budget_annualized_list ) )
	do {
		budget_annualized =
			list_get(
				budget_annualized_list );

		if ( string_strcmp(
			budget_annualized->element_name,
			(char *)element_name ) == 0 )
		{
			sum += budget_annualized->amount_integer;
		}

	} while ( list_next( budget_annualized_list ) );

	return sum;
}

int budget_annualized_budget_net(
		LIST *budget_annualized_list )
{
	int revenue_sum;
	int expense_sum;

	revenue_sum =
		budget_annualized_budget_sum(
			ELEMENT_REVENUE,
			budget_annualized_list );

	expense_sum =
		budget_annualized_budget_sum(
			/* ------------------------ */
			/* Returns either parameter */
			/* ------------------------ */
			(char *)element_resolve_expense(
				ELEMENT_EXPENSE,
				ELEMENT_EXPENDITURE ),
			budget_annualized_list );

	return revenue_sum - expense_sum;
}

int budget_annualized_budget_sum(
		const char *element_name,
		LIST *budget_annualized_list )
{
	BUDGET_ANNUALIZED *budget_annualized;
	int sum = 0;

	if ( list_rewind( budget_annualized_list ) )
	do {
		budget_annualized =
			list_get(
				budget_annualized_list );

		if ( string_strcmp(
			budget_annualized->element_name,
			(char *)element_name ) == 0 )
		{
			sum += budget_annualized->budget_integer;
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

BUDGET_LINK *budget_link_new(
		char *application_name,
		char *session_key,
		char *process_name,
		char *appaserver_parameter_data_directory,
		char *account_name )
{
	BUDGET_LINK *budget_link;

	if ( !application_name
	||   !session_key
	||   !process_name
	||   !appaserver_parameter_data_directory
	||   !account_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	budget_link = budget_link_calloc();

	budget_link->appaserver_link =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_link_new(
			/* ---------------------- */
			/* Returns program memory */
			/* ---------------------- */
			application_http_prefix(
				application_ssl_support_boolean(
					application_name ) ),
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			application_server_address(),
			appaserver_parameter_data_directory,
			process_name /* filename_stem */,
			application_name,
			(pid_t)0 /* process_id */,
			session_key,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			budget_link_begin_date_string(),
			(char *)0 /* end_date_string */,
			"pdf" /* extension */ );

	budget_link->pdf_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_filename(
			budget_link->appaserver_link->filename_stem,
			budget_link->appaserver_link->begin_date_string,
			budget_link->appaserver_link->end_date_string,
			budget_link->appaserver_link->process_id,
			budget_link->appaserver_link->session_key,
			budget_link->appaserver_link->extension );

	budget_link->appaserver_link->appaserver_link_prompt =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_link_prompt_new(
			APPASERVER_LINK_PROMPT_DIRECTORY
				/* probably appaserver_data */,
			budget_link->appaserver_link->http_prefix,
			budget_link->appaserver_link->server_address,
			application_name,
			budget_link->appaserver_link->filename_stem,
			budget_link->appaserver_link->begin_date_string,
			budget_link->appaserver_link->end_date_string,
			budget_link->appaserver_link->process_id,
			budget_link->appaserver_link->session_key,
			budget_link->appaserver_link->extension );

	budget_link->pdf_anchor_html =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_link_anchor_html(
			budget_link->
				appaserver_link->
				appaserver_link_prompt->
				filename /* prompt_filename */,
			process_name /* target_window */,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			budget_link_prompt_string(
				BUDGET_LINK_PROMPT,
				account_name ) );

	return budget_link;
}

BUDGET_LINK *budget_link_calloc( void )
{
	BUDGET_LINK *budget_link;

	if ( ! ( budget_link = calloc( 1, sizeof ( BUDGET_LINK ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return budget_link;
}

char *budget_link_prompt_string(
		const char *budget_link_prompt,
		char *account_name )
{
	static char prompt_string[ 128 ];

	if ( !account_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"account_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		prompt_string,
		sizeof ( prompt_string ),
		"%s %s",
		budget_link_prompt,
		account_name );

	return prompt_string;
}

BUDGET_FILE *budget_file_new(
		char *session_key,
		char *appaserver_link_working_directory )
{
	BUDGET_FILE *budget_file;

	if ( !session_key
	||   !appaserver_link_working_directory )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	budget_file = budget_file_calloc();

	budget_file->text_specification =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		budget_file_specification(
			"txt" /* extension */,
			session_key,
			appaserver_link_working_directory );

	budget_file->rm_text_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		budget_file_rm_system_string(
			budget_file->text_specification );

	budget_file->julian_specification =
		budget_file_specification(
			"csv" /* extension */,
			session_key,
			appaserver_link_working_directory );

	budget_file->rm_julian_system_string =
		budget_file_rm_system_string(
			budget_file->julian_specification );

	budget_file->regression_specification =
		budget_file_specification(
			"dat" /* extension */,
			session_key,
			appaserver_link_working_directory );

	budget_file->rm_regression_system_string =
		budget_file_rm_system_string(
			budget_file->regression_specification );

	return budget_file;
}

BUDGET_FILE *budget_file_calloc( void )
{
	BUDGET_FILE *budget_file;

	if ( ! ( budget_file = calloc( 1, sizeof ( BUDGET_FILE ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return budget_file;
}

char *budget_file_specification(
		const char *extension,
		char *session_key,
		char *appaserver_link_working_directory )
{
	char specification[ 512 ];

	if ( !session_key
	||   !appaserver_link_working_directory )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		specification,
		sizeof ( specification ),
		"%s/budget_%s.%s",
		appaserver_link_working_directory,
		session_key,
		extension );

	return strdup( specification );
}

char *budget_file_rm_system_string( char *budget_file_specification )
{
	char system_string[ 1024 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"rm %s",
		budget_file_specification );

	return strdup( system_string );
}

BUDGET_PLOT *budget_plot_new(
		const char *budget_regression_date_label,
		const char *budget_regression_balance_label,
		char *session_key,
		char *account_name,
		char *budget_forecast_date_string,
		char *budget_forecast_julian_string,
		char *appaserver_link_working_directory,
		char *pdf_filename,
		char *julian_specification,
		int forecast_integer,
		int confidence_integer )
{
	BUDGET_PLOT *budget_plot;
	FILE *append_file;

	if ( !session_key
	||   !account_name
	||   !budget_forecast_date_string
	||   !budget_forecast_julian_string
	||   !appaserver_link_working_directory
	||   !pdf_filename
	||   !julian_specification )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	budget_plot = budget_plot_calloc();

	append_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_append_file(
			julian_specification );

	fprintf(
		append_file,
		"%s,%d\n",
		budget_forecast_julian_string,
		forecast_integer );

	fclose( append_file );

	if ( chdir( appaserver_link_working_directory ) != 0 )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"chdir(%s) returned in error.",
			appaserver_link_working_directory );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	budget_plot_write_Rscript(
		budget_regression_date_label,
		budget_regression_balance_label,
		BUDGET_PLOT_RSCRIPT_FILENAME,
		account_name,
		budget_forecast_date_string,
		julian_specification,
		confidence_integer );

	if ( system(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		budget_plot_system_string(
			BUDGET_PLOT_RSCRIPT_FILENAME ) ) ){}

	if ( system(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		budget_plot_rename_system_string(
			BUDGET_PLOT_DEFAULT_PDF,
			pdf_filename ) ) ){}


	if ( system(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		budget_plot_remove_system_string(
			BUDGET_PLOT_RSCRIPT_FILENAME ) ) ){}

	return budget_plot;
}

BUDGET_PLOT *budget_plot_calloc( void )
{
	BUDGET_PLOT *budget_plot;

	if ( ! ( budget_plot = calloc( 1, sizeof ( BUDGET_PLOT ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return budget_plot;
}

void budget_plot_write_Rscript(
		const char *budget_regression_date_label,
		const char *budget_regression_balance_label,
		const char *budget_plot_rscript_filename,
		char *account_name,
		char *budget_forecast_date_string,
		char *julian_specification,
		int confidence_integer )
{
	FILE *output_file;

	if ( !account_name
	||   !budget_forecast_date_string
	||   !julian_specification )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	output_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_file(
			(char *)budget_plot_rscript_filename );

	fprintf(
		output_file,
		"dataframe = read.csv( '%s' )\n"
		"x = dataframe[[ '%s' ]]\n"
		"y = dataframe[[ '%s' ]]\n"
		"linearModel <- lm( y ~ x )\n"
		"message <-\n"
		"\tpaste(\n"
		"\t\t'Forecast %s = $',\n"
		"\t\tmax( y ),\n"
		"\t\t', Confidence = ',\n"
		"\t\t'%d%c')\n"
		"plot(\n"
		"\tx,\n"
		"\ty,\n"
		"\tmain = '%s forecast',\n"
		"\tsub = message,\n"
		"\txlab = 'Transaction Date (Julian)',\n"
		"\tylab = '%s',\n"
		"\tabline( linearModel ) )\n",
		julian_specification,
		budget_regression_date_label,
		budget_regression_balance_label,
		budget_forecast_date_string,
		confidence_integer,
		'%',
		account_name,
		account_name );

	fclose( output_file );
}

char *budget_plot_system_string(
		const char *budget_plot_rscript_filename )
{
	char system_string[ 1024 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"Rscript.sh %s",
		budget_plot_rscript_filename );

	return strdup( system_string );
}

char *budget_plot_rename_system_string(
		const char *budget_plot_default_pdf,
		char *pdf_filename )
{
	char system_string[ 1024 ];

	if ( !pdf_filename )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"pdf_filename is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"mv %s %s",
		budget_plot_default_pdf,
		pdf_filename );

	return strdup( system_string );
}

char *budget_plot_remove_system_string(
		const char *budget_plot_rscript_filename )
{
	static char system_string[ 128 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"rm %s",
		budget_plot_rscript_filename );

	return system_string;
}

BUDGET_REGRESSION *budget_regression_fetch(
		char *application_name,
		char *session_key,
		char *process_name,
		char *appaserver_parameter_data_directory,
		char *transaction_date_begin_date_string,
		char *account_name,
		char *budget_forecast_date_string,
		char *budget_forecast_julian_string,
		char *appaserver_link_working_directory )
{
	BUDGET_REGRESSION *budget_regression;
	char *fetch;

	budget_regression = budget_regression_calloc();

	budget_regression->budget_link =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		budget_link_new(
			application_name,
			session_key,
			process_name,
			appaserver_parameter_data_directory,
			account_name );

	budget_regression->budget_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		budget_file_new(
			session_key,
			appaserver_link_working_directory );

	budget_regression->sql =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		budget_regression_sql(
			JOURNAL_TABLE,
			transaction_date_begin_date_string,
			account_name );

	budget_regression->text_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		budget_regression_text_system_string(
			budget_regression->budget_file->text_specification,
			budget_regression->sql );

	if ( system(
		budget_regression->
			text_system_string ) ){}

	budget_regression->row_count =
		budget_regression_row_count(
			budget_regression->
				budget_file->
					text_specification );

	if ( budget_regression->row_count < 3 )
		return budget_regression;

	budget_regression->julian_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		budget_regression_julian_system_string(
			BUDGET_REGRESSION_DATE_LABEL,
			BUDGET_REGRESSION_BALANCE_LABEL,
			budget_regression->budget_file->text_specification,
			budget_regression->budget_file->julian_specification );

	if ( system(
		budget_regression->
			julian_system_string ) ){}

	budget_regression->forecast_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		budget_regression_forecast_system_string(
			BUDGET_REGRESSION_DATE_LABEL,
			BUDGET_REGRESSION_BALANCE_LABEL,
			budget_forecast_julian_string,
			budget_regression->budget_file->julian_specification,
			budget_regression->
				budget_file->
				regression_specification );

	fetch =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		string_fetch(
			budget_regression->
				forecast_system_string );

	if ( !fetch )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"string_fetch(%s) returned empty.",
			budget_regression->forecast_system_string );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	budget_regression->forecast_integer =
		budget_regression_forecast_integer(
			fetch );

	if ( budget_regression->forecast_integer )
	{
		budget_regression->confidence_integer =
			budget_regression_confidence_integer(
				fetch );

		budget_regression->budget_plot =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			budget_plot_new(
				BUDGET_REGRESSION_DATE_LABEL,
				BUDGET_REGRESSION_BALANCE_LABEL,
				session_key,
				account_name,
				budget_forecast_date_string,
				budget_forecast_julian_string,
				appaserver_link_working_directory,
				budget_regression->budget_link->pdf_filename,
				budget_regression->
					budget_file->
					julian_specification
					/* will append */,
				budget_regression->forecast_integer,
				budget_regression->confidence_integer );
	}

	if ( system(
		budget_regression->
			budget_file->
			rm_text_system_string ) ){}

	if ( system(
		budget_regression->
			budget_file->
			rm_julian_system_string ) ){}

	if ( system(
		budget_regression->
			budget_file->
			rm_regression_system_string ) ){}

	return budget_regression;
}

BUDGET_REGRESSION *budget_regression_calloc( void )
{
	BUDGET_REGRESSION *budget_regression;

	if ( ! ( budget_regression =
			calloc( 1,
				sizeof ( BUDGET_REGRESSION ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return budget_regression;
}

char *budget_regression_sql(
		const char *journal_table,
		char *transaction_date_begin_date_string,
		char *account_name )
{
	char *where;
	char sql[ 1024 ];

	if ( !transaction_date_begin_date_string
	||   !account_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		budget_regression_where(
			transaction_date_begin_date_string,
			account_name );

	snprintf(
		sql,
		sizeof ( sql ),
		"select "
		"substr( transaction_date_time, 1, 10 ), "
		"balance "
		"from %s "
		"where %s "
		"order by transaction_date_time",
		journal_table,
		where );

	return strdup( sql );
}

char *budget_regression_julian_system_string(
		const char *budget_regression_date_label,
		const char *budget_regression_balance_label,
		char *text_specification,
		char *julian_specification )
{
	char julian_system_string[ 1024 ];

	if ( !text_specification
	||   !julian_specification )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		julian_system_string,
		sizeof ( julian_system_string ),
		"(\n"
		"echo '%s,%s'\n"
		"cat %s |\n"
		"date2julian.e 0 ','\n"
		") | cat > %s",
		budget_regression_date_label,
		budget_regression_balance_label,
		text_specification,
		julian_specification );

	return strdup( julian_system_string );
}

char *budget_regression_forecast_system_string(
		const char *budget_regression_date_label,
		const char *budget_regression_balance_label,
		char *budget_forecast_julian_string,
		char *julian_specification,
		char *regression_specification )
{
	char forecast_system_string[ 1024 ];

	if ( !budget_forecast_julian_string
	||   !julian_specification
	||   !regression_specification )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		forecast_system_string,
		sizeof ( forecast_system_string ),
		"linear_regression_forecast.sh %s %s %s %s %s",
		julian_specification,
		regression_specification,
		budget_regression_date_label,
		budget_regression_balance_label,
		budget_forecast_julian_string );

	return strdup( forecast_system_string );
}

char *budget_regression_where(
		char *transaction_date_begin_date_string,
		char *account_name )
{
	static char where[ 128 ];

	if ( !transaction_date_begin_date_string
	||   !account_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		where,
		sizeof ( where ),
		"transaction_date_time >= '%s' and account = '%s'",
		transaction_date_begin_date_string,
		account_name );

	return where;
}

char *budget_regression_text_system_string(
		char *text_specification,
		char *budget_regression_sql )
{
	char system_string[ 2048 ];

	if ( !text_specification
	||   !budget_regression_sql )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"%s;\" | sql.e ',' > %s",
		budget_regression_sql,
		text_specification );

	return strdup( system_string );
}

int budget_regression_row_count( char *text_specification )
{
	char system_string[ 1024 ];

	if ( !text_specification )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"text_specification is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"cat %s | wc -l",
		text_specification );

	return
	atoi( string_fetch( system_string ) );
}

int budget_regression_forecast_integer( char *string_fetch )
{
	if ( !string_fetch )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"string_fetch is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return atoi( string_fetch );
}

int budget_regression_confidence_integer( char *string_fetch )
{
	char buffer[ 128 ];

	if ( !string_fetch )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"string_fetch is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !string_character_boolean( 
		string_fetch /* datum */,
		',' /* c */ ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"comma not found in [%s].",
			string_fetch );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	piece( buffer, ',', string_fetch, 1 );

	return atoi( buffer );
}

DATE *budget_forecast_date(
		DATE *budget_begin_date,
		int budget_days_in_year )
{
	DATE *forecast_date;

	if ( !budget_begin_date )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"budget_begin_date is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	forecast_date = date_copy( (DATE *)0, budget_begin_date );

	return
	/* ------------ */
	/* Returns date */
	/* ------------ */
	date_increment_days(
		forecast_date,
		budget_days_in_year - 1 );
}

char *budget_forecast_date_string( DATE *budget_forecast_date )
{
	return
	/* ------------------------- */
	/* Returns heap memory or "" */
	/* ------------------------- */
	date_display_yyyy_mm_dd(
		budget_forecast_date );
}

char *budget_forecast_julian_string(
		char *budget_forecast_date_string )
{
	char system_string[ 128 ];

	if ( !budget_forecast_date_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"budget_forecast_date_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo %s | date2julian.e 0",
		budget_forecast_date_string );

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	string_fetch( system_string );
}

char *budget_link_begin_date_string( void )
{
	static int count;
	static char begin_date_string[ 16 ];

	snprintf(
		begin_date_string,
		sizeof ( begin_date_string ),
		"%d",
		++count );

	return begin_date_string;
}

int budget_annualized_amount_integer(
		double account_amount,
		int forecast_integer,
		int budget_annualized_budget_integer )
{
	int amount_integer;

	if ( forecast_integer )
		amount_integer = forecast_integer;
	else
		amount_integer =
			budget_annualized_budget_integer;

	if ( (double)amount_integer < account_amount )
	{
		amount_integer =
			float_round_integer(
				account_amount );
	}

	return amount_integer;
}

int budget_annualized_budget_integer(
		STATEMENT_PRIOR_YEAR *statement_prior_year,
		char *account_name,
		int account_annual_budget )
{
	int budget_integer = 0;

	if ( account_annual_budget )
	{
		budget_integer = account_annual_budget;
	}
	else
	if ( statement_prior_year )
	{
		ACCOUNT *prior_account =
			account_element_list_seek(
				account_name,
				statement_prior_year->
					element_statement_list );

		if ( prior_account )
		{
			budget_integer =
				float_round_integer(
					prior_account->
						account_journal_latest->
						balance );
		}
	}

	return budget_integer;
}

double budget_amount_net( LIST *budget_annualized_list )
{
	double revenue_sum;
	double expense_sum;

	revenue_sum =
		budget_amount_sum(
			ELEMENT_REVENUE,
			budget_annualized_list );

	expense_sum =
		budget_amount_sum(
			/* ------------------------ */
			/* Returns either parameter */
			/* ------------------------ */
			(char *)element_resolve_expense(
				ELEMENT_EXPENSE,
				ELEMENT_EXPENDITURE ),
			budget_annualized_list );

	return revenue_sum - expense_sum;
}

double budget_amount_sum(
		const char *element_name,
		LIST *budget_annualized_list )
{
	BUDGET_ANNUALIZED *budget_annualized;
	double sum = 0.0;

	if ( list_rewind( budget_annualized_list ) )
	do {
		budget_annualized =
			list_get(
				budget_annualized_list );

		if ( string_strcmp(
			budget_annualized->element_name,
			(char *)element_name ) == 0 )
		{
			sum += budget_annualized->
				account->
				account_journal_latest->
				balance;
		}

	} while ( list_next( budget_annualized_list ) );

	return sum;
}

int budget_annualized_compare_function(
		BUDGET_ANNUALIZED *from_list_budget_annualized,
		BUDGET_ANNUALIZED *compare_budget_annualized )
{
	if ( !from_list_budget_annualized
	||   !compare_budget_annualized )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if (	compare_budget_annualized->amount_integer ==
		from_list_budget_annualized->amount_integer )
	{
		return 0;
	}
	else
	if (	compare_budget_annualized->amount_integer >
		from_list_budget_annualized->amount_integer )
	{
		return 1;
	}
	else
	{
		return -1;
	}
}
