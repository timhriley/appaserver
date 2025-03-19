/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/income_statement.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "float.h"
#include "timlib.h"
#include "appaserver_error.h"
#include "transaction.h"
#include "statement.h"
#include "element.h"
#include "latex.h"
#include "income_statement.h"

INCOME_STATEMENT_SUBCLASS_DISPLAY_LATEX *
	income_statement_subclass_display_latex_new(
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double element_net_income,
		char *net_income_percent_of_income_display,
		char *income_statement_net_income_label )
{
	INCOME_STATEMENT_SUBCLASS_DISPLAY_LATEX *
		income_statement_subclass_display_latex;

	if ( !statement
	||   !net_income_percent_of_income_display
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	income_statement_subclass_display_latex =
		income_statement_subclass_display_latex_calloc();

	income_statement_subclass_display_latex->
		statement_subclass_display_latex_list =
			statement_subclass_display_latex_list_new(
				statement->element_statement_list,
				statement_prior_year_list );

	income_statement_subclass_display_latex->row_list =
		statement_subclass_display_latex_list_row_list(
			income_statement_subclass_display_latex->
				statement_subclass_display_latex_list );

	list_set(
		income_statement_subclass_display_latex->row_list,
		income_statement_latex_net_income_row(
			statement_prior_year_list,
			element_net_income,
			net_income_percent_of_income_display,
			income_statement_net_income_label,
			income_statement_subclass_display_latex->
				statement_subclass_display_latex_list->
				column_list
				/* latex_column_list */,
			2 /* empty_cell_count */ ) );

	return income_statement_subclass_display_latex;
}

INCOME_STATEMENT_SUBCLASS_DISPLAY_LATEX *
	income_statement_subclass_display_latex_calloc(
		void )
{
	INCOME_STATEMENT_SUBCLASS_DISPLAY_LATEX *
		income_statement_subclass_display_latex;

	if ( ! ( income_statement_subclass_display_latex =
		   calloc(
			1,
			sizeof ( INCOME_STATEMENT_SUBCLASS_DISPLAY_LATEX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return income_statement_subclass_display_latex;
}

INCOME_STATEMENT_SUBCLASS_OMIT_LATEX *
	income_statement_subclass_omit_latex_new(
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double element_net_income,
		char *net_income_percent_of_income_display,
		char *income_statement_net_income_label )
{
	INCOME_STATEMENT_SUBCLASS_OMIT_LATEX *
		income_statement_subclass_omit_latex;

	if ( !statement
	||   !net_income_percent_of_income_display
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	income_statement_subclass_omit_latex =
		income_statement_subclass_omit_latex_calloc();

	income_statement_subclass_omit_latex->
		statement_subclass_omit_latex_list =
			statement_subclass_omit_latex_list_new(
				statement->element_statement_list,
				statement_prior_year_list );

	income_statement_subclass_omit_latex->row_list =
		statement_subclass_omit_latex_list_row_list(
			income_statement_subclass_omit_latex->
				statement_subclass_omit_latex_list );

	list_set(
		income_statement_subclass_omit_latex->row_list,
		income_statement_latex_net_income_row(
			statement_prior_year_list,
			element_net_income,
			net_income_percent_of_income_display,
			income_statement_net_income_label,
			income_statement_subclass_omit_latex->
				statement_subclass_omit_latex_list->
				column_list
				/* latex_column_list */,
			1 /* empty_cell_count */ ) );

	return income_statement_subclass_omit_latex;
}

INCOME_STATEMENT_SUBCLASS_OMIT_LATEX *
	income_statement_subclass_omit_latex_calloc(
		void )
{
	INCOME_STATEMENT_SUBCLASS_OMIT_LATEX *
		income_statement_subclass_omit_latex;

	if ( ! ( income_statement_subclass_omit_latex =
		   calloc(
			1,
			sizeof ( INCOME_STATEMENT_SUBCLASS_OMIT_LATEX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return income_statement_subclass_omit_latex;
}

LATEX_ROW *income_statement_latex_net_income_row(
		LIST *statement_prior_year_list,
		double element_net_income,
		char *percent_of_income_display,
		char *income_statement_net_income_label,
		LIST *latex_column_list,
		int empty_cell_count )
{
	LIST *cell_list;
	LATEX_COLUMN *latex_column;
	LATEX_CELL *latex_cell;
	char *cell_label_datum;
	int i;
	char *commas_money;

	if ( !percent_of_income_display
	||   !income_statement_net_income_label
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

	cell_label_datum =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_label_datum(
			income_statement_net_income_label );

	latex_cell =
		latex_cell_new(
			latex_column,
			0 /* not first_row_boolean */,
			cell_label_datum,
			1 /* large_boolean */,
			1 /* bold_boolean */ );

	list_set( cell_list, latex_cell );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	for (	i = 0;
		i < empty_cell_count;
		i++ )
	{
		list_set(
			cell_list,
			latex_cell_small_new(
				latex_column,
				0 /* not first_row_boolean */,
				(char *)0 /* datum */ ) );

		latex_column = list_get( latex_column_list );
		list_next( latex_column_list );
	}

	commas_money =
		/* ----------------------*/
		/* Returns static memory */
		/* ----------------------*/
		string_commas_money(
			element_net_income );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			strdup( commas_money ) ) );

	latex_column = list_get( latex_column_list );
	list_next( latex_column_list );

	list_set(
		cell_list,
		latex_cell_small_new(
			latex_column,
			0 /* not first_row_boolean */,
			percent_of_income_display ) );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_net_income_data_list =
			income_statement_prior_net_income_data_list(
				element_net_income /* current_net_income */,
				statement_prior_year_list );

		list_set_list(
			cell_list,
			latex_cell_list(
				__FUNCTION__,
				latex_column_list,
				prior_net_income_data_list ) );
	}

	return
	latex_row_new( cell_list );
}

INCOME_STATEMENT_LATEX *income_statement_latex_new(
		char *application_name,
		char *process_name,
		char *data_directory,
		enum statement_subclassification_option
			statement_subclassification_option,
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double element_net_income,
		char *net_income_percent_of_income_display,
		char *income_statement_net_income_label,
		pid_t process_id )
{
	INCOME_STATEMENT_LATEX *income_statement_latex;

	if ( !application_name
	||   !process_name
	||   !data_directory
	||   !statement
	||   !statement->statement_caption
	||   !process_id )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	income_statement_latex = income_statement_latex_calloc();

	income_statement_latex->statement_link =
		statement_link_new(
			application_name,
			process_name,
			data_directory,
			statement->transaction_date_begin_date_string,
			statement->end_date_time_string,
			process_id );

	income_statement_latex->latex =
		latex_new(
			income_statement_latex->
				statement_link->
				tex_filename,
			income_statement_latex->
				statement_link->
				appaserver_link_working_directory,
			statement->pdf_landscape_boolean,
			statement->statement_caption->logo_filename );

	if (	statement_subclassification_option ==
		statement_subclassification_display )
	{
		income_statement_latex->
			income_statement_subclass_display_latex =
				income_statement_subclass_display_latex_new(
					statement,
					statement_prior_year_list,
					element_net_income,
					net_income_percent_of_income_display,
					income_statement_net_income_label );

		if ( !income_statement_latex->
			income_statement_subclass_display_latex )
		{
			free( income_statement_latex );
			return (INCOME_STATEMENT_LATEX *)0;
		}

		income_statement_latex->latex_table =
			income_statement_latex_table(
				statement->statement_caption->combined,
				income_statement_latex->
				   income_statement_subclass_display_latex->
				   statement_subclass_display_latex_list->
				   column_list
				   /* latex_column_list */ ,
				income_statement_latex->
				   income_statement_subclass_display_latex->
				   row_list
				   /* latex_row_list */ );
	}
	else
	if (	statement_subclassification_option ==
		statement_subclassification_omit )
	{
		income_statement_latex->
			income_statement_subclass_omit_latex =
				income_statement_subclass_omit_latex_new(
					statement,
					statement_prior_year_list,
					element_net_income,
					net_income_percent_of_income_display,
					income_statement_net_income_label );

		if ( !income_statement_latex->
			income_statement_subclass_omit_latex )
		{
			free( income_statement_latex );
			return (INCOME_STATEMENT_LATEX *)0;
		}

		income_statement_latex->latex_table =
			income_statement_latex_table(
				statement->statement_caption->combined,
				income_statement_latex->
				   income_statement_subclass_omit_latex->
				   statement_subclass_omit_latex_list->
				   column_list
				   /* latex_column_list */,
				income_statement_latex->
				   income_statement_subclass_omit_latex->
				   row_list
				   /* latex_row_list */ );
	}
	else
	if (	statement_subclassification_option ==
		statement_subclassification_aggregate )
	{
		income_statement_latex->
			income_statement_subclass_aggr_latex =
				income_statement_subclass_aggr_latex_new(
					statement,
					statement_prior_year_list,
					element_net_income,
					net_income_percent_of_income_display,
					income_statement_net_income_label );

		if ( !income_statement_latex->
			income_statement_subclass_aggr_latex )
		{
			free( income_statement_latex );
			return (INCOME_STATEMENT_LATEX *)0;
		}

		income_statement_latex->latex_table =
			income_statement_latex_table(
				statement->statement_caption->combined,
				income_statement_latex->
				   income_statement_subclass_aggr_latex->
				   statement_subclass_aggr_latex_list->
				   column_list
				   /* latex_column_list */,
				income_statement_latex->
				   income_statement_subclass_aggr_latex->
				   row_list
				   /* latex_row_list */ );
	}

	return income_statement_latex;
}

INCOME_STATEMENT *income_statement_fetch(
		char *argv_0,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		char *data_directory,
		char *as_of_date_string,
		int prior_year_count,
		char *subclassification_option_string,
		char *output_medium_string )
{
	INCOME_STATEMENT *income_statement;

	if ( !argv_0
	||   !application_name
	||   !session_key
	||   !role_name
	||   !process_name
	||   !data_directory
	||   !as_of_date_string
	||   !subclassification_option_string
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

	income_statement = income_statement_calloc();

	income_statement->statement_subclassification_option =
		statement_resolve_subclassification_option(
			subclassification_option_string );

	income_statement->statement_output_medium =
		statement_resolve_output_medium(
			output_medium_string );

	income_statement->transaction_date_statement =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		transaction_date_statement_new(
			as_of_date_string );

	if ( !income_statement->
		transaction_date_statement->
		transaction_date_as_of )
	{
		return NULL;
	}

	income_statement->element_name_list =
		income_statement_element_name_list(
			ELEMENT_REVENUE,
			ELEMENT_EXPENSE,
			ELEMENT_GAIN,
			ELEMENT_LOSS );

	income_statement->statement =
		statement_fetch(
			application_name,
			process_name,
			prior_year_count,
			income_statement->element_name_list,
			income_statement->
				transaction_date_statement->
				transaction_date_begin_date_string,
			income_statement->
				transaction_date_statement->
				end_date_time_string,
			0 /* not fetch_transaction */ );

	if ( income_statement->statement_output_medium ==
		statement_output_table )
	{
		element_account_transaction_count_set(
			income_statement->statement->element_statement_list,
			income_statement->
				transaction_date_statement->
				transaction_date_begin_date_string,
			income_statement->
				transaction_date_statement->
				end_date_time_string );

		element_account_action_string_set(
			income_statement->statement->element_statement_list,
			application_name,
			session_key,
			login_name,
			role_name,
			income_statement->
				transaction_date_statement->
				transaction_date_begin_date_string,
			income_statement->
				transaction_date_statement->
				end_date_time_string );
	}

	if ( prior_year_count )
	{
		income_statement->statement_prior_year_list =
			statement_prior_year_list(
				income_statement->element_name_list,
				income_statement->
					transaction_date_statement->
					end_date_time_string,
				prior_year_count,
				income_statement->statement );
	}

	if (	income_statement->statement_subclassification_option ==
		statement_subclassification_omit )
	{
		element_list_account_statement_list_set(
			income_statement->
				statement->
				element_statement_list );
	}

	income_statement->element_net_income =
		element_net_income(
			income_statement->
				statement->
				element_statement_list );

	income_statement->net_income_percent_of_income_display =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		income_statement_net_income_percent_of_income_display(
			ELEMENT_REVENUE,
			ELEMENT_GAIN,
			income_statement->element_net_income,
			income_statement->statement->element_statement_list );

	income_statement->net_income_label =
		income_statement_net_income_label(
			argv_0 );

	if ( income_statement->statement_output_medium ==
		statement_output_PDF )
	{
		income_statement->income_statement_latex =
			income_statement_latex_new(
				application_name,
				process_name,
				data_directory,
				income_statement->
					statement_subclassification_option,
				income_statement->statement,
				income_statement->statement_prior_year_list,
				income_statement->element_net_income,
				income_statement->
					net_income_percent_of_income_display,
				income_statement->net_income_label,
				getpid() /* process_id */ );
	}
	else
	if ( income_statement->statement_output_medium ==
		statement_output_table )
	{
		income_statement->income_statement_html =
			income_statement_html_new(
				income_statement->
					statement_subclassification_option,
				income_statement->statement,
				income_statement->statement_prior_year_list,
				income_statement->element_net_income,
				income_statement->
					net_income_percent_of_income_display,
				income_statement->net_income_label );
	}

	return income_statement;
}

INCOME_STATEMENT *income_statement_calloc( void )
{
	INCOME_STATEMENT *income_statement;

	if ( ! ( income_statement = calloc( 1, sizeof ( INCOME_STATEMENT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return income_statement;
}

LIST *income_statement_element_name_list(
		char *element_revenue,
		char *element_expense,
		char *element_gain,
		char *element_loss )
{
	LIST *element_name_list = list_new();

	list_set(
		element_name_list,
		element_revenue );

	list_set(
		element_name_list,
		element_expense );

	list_set(
		element_name_list,
		element_gain );

	list_set(
		element_name_list,
		element_loss );

	return element_name_list;
}

char *income_statement_net_income_percent_of_income_display(
		const char *element_revenue,
		const char *element_gain,
		double element_net_income,
		LIST *element_statement_list )
{
	ELEMENT *revenue;
	ELEMENT *gain;
	double income;

	revenue =
		element_seek(
			(char *)element_revenue,
			element_statement_list );

	gain =
		element_seek(
			(char *)element_gain,
			element_statement_list );

	income = element_income( revenue, gain );

	if ( float_virtually_same( income, 0.0 ) )
	{
		return strdup( "" );
	}
	else
	{
		int percent_of_income =
			float_percent_of_total(
				element_net_income,
				income );

		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_percent_string(
			0 /* percent_of_asset */,
			percent_of_income );
	}
}

char *income_statement_net_income_label( char *argv_0 )
{
	if ( !argv_0 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: argv_0 is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( strcmp( argv_0, "income_statement_output" ) == 0
	||   strcmp( argv_0, "balance_sheet_output" ) == 0 )
		return "Net Income";
	else
		return "Change in Net Assets";
}

LIST *income_statement_prior_net_income_data_list(
		double current_net_income,
		LIST *statement_prior_year_list )
{
	LIST *data_list;
	STATEMENT_PRIOR_YEAR *statement_prior_year;
	double prior_net_income;
	int delta_prior_percent;

	if ( !list_rewind( statement_prior_year_list ) ) return (LIST *)0;

	data_list = list_new();

	do {
		statement_prior_year =
			list_get(
				statement_prior_year_list );

	prior_net_income =
		element_net_income(
			statement_prior_year->element_statement_list );

	delta_prior_percent =
		float_delta_prior_percent(
			prior_net_income,
			current_net_income );

	list_set(
		data_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_prior_year_cell_display(
			0 /* not cell_empty */,
			delta_prior_percent,
			prior_net_income /* prior_year_amount */ ) );

	} while ( list_next( statement_prior_year_list ) );

	return data_list;
}

INCOME_STATEMENT_LATEX *income_statement_latex_calloc( void )
{
	INCOME_STATEMENT_LATEX *income_statement_latex;

	if ( ! ( income_statement_latex =
			calloc( 1, sizeof ( INCOME_STATEMENT_LATEX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return income_statement_latex;
}

INCOME_STATEMENT_HTML *income_statement_html_new(
		enum statement_subclassification_option
			statement_subclassification_option,
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double element_net_income,
		char *net_income_percent_of_income_display,
		char *net_income_label )
{
	INCOME_STATEMENT_HTML *income_statement_html;

	if ( !statement
	||   !statement->statement_caption
	||   !net_income_percent_of_income_display
	||   !net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	income_statement_html = income_statement_html_calloc();

	if (	statement_subclassification_option ==
		statement_subclassification_display )
	{
		income_statement_html->
			income_statement_subclass_display_html =
				income_statement_subclass_display_html_new(
					statement,
					statement_prior_year_list,
					element_net_income,
					net_income_percent_of_income_display,
					net_income_label );

		if ( !income_statement_html->
			income_statement_subclass_display_html )
		{
			free( income_statement_html );
			return (INCOME_STATEMENT_HTML *)0;
		}

		income_statement_html->html_table =
			income_statement_html_table(
			     	statement->statement_caption->sub_title,
			     	income_statement_html->
					income_statement_subclass_display_html->
					statement_subclass_display_html_list->
					column_list,
			     	income_statement_html->
					income_statement_subclass_display_html->
					row_list );
	}
	else
	if (	statement_subclassification_option ==
		statement_subclassification_omit )
	{
		income_statement_html->
			income_statement_subclass_omit_html =
				income_statement_subclass_omit_html_new(
					statement,
					statement_prior_year_list,
					element_net_income,
					net_income_percent_of_income_display,
					net_income_label );

		if ( !income_statement_html->
			income_statement_subclass_omit_html )
		{
			free( income_statement_html );
			return (INCOME_STATEMENT_HTML *)0;
		}

		income_statement_html->html_table =
			income_statement_html_table(
				statement->statement_caption->sub_title,
				income_statement_html->
					income_statement_subclass_omit_html->
					statement_subclass_omit_html_list->
					column_list,
				income_statement_html->
					income_statement_subclass_omit_html->
					row_list );
	}
	else
	if (	statement_subclassification_option ==
		statement_subclassification_aggregate )
	{
		income_statement_html->
			income_statement_subclass_aggr_html =
				income_statement_subclass_aggr_html_new(
					statement,
					statement_prior_year_list,
					element_net_income,
					net_income_percent_of_income_display,
					net_income_label );

		if ( !income_statement_html->
			income_statement_subclass_aggr_html )
		{
			free( income_statement_html );
			return (INCOME_STATEMENT_HTML *)0;
		}

		income_statement_html->html_table =
			income_statement_html_table(
				statement->statement_caption->sub_title,
				income_statement_html->
					income_statement_subclass_aggr_html->
					statement_subclass_aggr_html_list->
					column_list,
				income_statement_html->
					income_statement_subclass_aggr_html->
					row_list );
	}
	else
	{
		char message[ 128 ];

		sprintf(message,
		"invalid statement_subclassifiction_option=%d.",
			statement_subclassification_option );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return income_statement_html;
}

INCOME_STATEMENT_HTML *income_statement_html_calloc( void )
{
	INCOME_STATEMENT_HTML *income_statement_html;

	if ( ! ( income_statement_html =
			calloc( 1, sizeof ( INCOME_STATEMENT_HTML ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return income_statement_html;
}

INCOME_STATEMENT_SUBCLASS_DISPLAY_HTML *
	income_statement_subclass_display_html_new(
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double element_net_income,
		char *net_income_percent_of_income_display,
		char *net_income_label )
{
	INCOME_STATEMENT_SUBCLASS_DISPLAY_HTML *
		income_statement_subclass_display_html;

	if ( !statement
	||   !net_income_percent_of_income_display
	||   !net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( statement->element_statement_list ) )
		return (INCOME_STATEMENT_SUBCLASS_DISPLAY_HTML *)0;

	income_statement_subclass_display_html =
		income_statement_subclass_display_html_calloc();


	income_statement_subclass_display_html->
		statement_subclass_display_html_list =
			statement_subclass_display_html_list_new(
				statement->element_statement_list,
				statement_prior_year_list );

	income_statement_subclass_display_html->row_list =
		statement_subclass_display_html_list_row_list(
			income_statement_subclass_display_html->
				statement_subclass_display_html_list );

	list_set(
		income_statement_subclass_display_html->row_list,
		income_statement_html_net_income_row(
			statement_prior_year_list,
			element_net_income,
			net_income_percent_of_income_display,
			net_income_label,
			2 /* empty_cell_count */ ) );

	return income_statement_subclass_display_html;
}

INCOME_STATEMENT_SUBCLASS_DISPLAY_HTML *
	income_statement_subclass_display_html_calloc(
		void )
{
	INCOME_STATEMENT_SUBCLASS_DISPLAY_HTML *
		income_statement_subclass_display_html;

	if ( ! ( income_statement_subclass_display_html =
		   calloc(
			1,
			sizeof ( INCOME_STATEMENT_SUBCLASS_DISPLAY_HTML ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return income_statement_subclass_display_html;
}

INCOME_STATEMENT_SUBCLASS_OMIT_HTML *
	income_statement_subclass_omit_html_new(
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double element_net_income,
		char *net_income_percent_of_income_display,
		char *net_income_label )
{
	INCOME_STATEMENT_SUBCLASS_OMIT_HTML *
		income_statement_subclass_omit_html;

	if ( !statement
	||   !net_income_percent_of_income_display
	||   !net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( statement->element_statement_list ) )
		return (INCOME_STATEMENT_SUBCLASS_OMIT_HTML *)0;

	income_statement_subclass_omit_html =
		income_statement_subclass_omit_html_calloc();

	income_statement_subclass_omit_html->
		statement_subclass_omit_html_list =
			statement_subclass_omit_html_list_new(
				statement->element_statement_list,
				statement_prior_year_list );

	income_statement_subclass_omit_html->row_list =
		statement_subclass_omit_html_list_row_list(
			income_statement_subclass_omit_html->
				statement_subclass_omit_html_list );

	list_set(
		income_statement_subclass_omit_html->row_list,
		income_statement_html_net_income_row(
			statement_prior_year_list,
			element_net_income,
			net_income_percent_of_income_display,
			net_income_label,
			1 /* empty_cell_count */ ) );

	return income_statement_subclass_omit_html;
}

INCOME_STATEMENT_SUBCLASS_OMIT_HTML *
	income_statement_subclass_omit_html_calloc(
		void )
{
	INCOME_STATEMENT_SUBCLASS_OMIT_HTML *
		income_statement_subclass_omit_html;

	if ( ! ( income_statement_subclass_omit_html =
		   calloc(1,
			  sizeof ( INCOME_STATEMENT_SUBCLASS_OMIT_HTML ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return income_statement_subclass_omit_html;
}

HTML_ROW *income_statement_html_net_income_row(
		LIST *statement_prior_year_list,
		double element_net_income,
		char *net_income_percent_of_income_display,
		char *net_income_label,
		int empty_cell_count )
{
	LIST *cell_list;
	char *cell_label_datum;
	int i;

	if ( !net_income_percent_of_income_display
	||   !net_income_label )
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

	cell_label_datum =
		/* ------------------------- */
		/* Returns heap memory or "" */
		/* ------------------------- */
		statement_cell_label_datum( net_income_label );

	list_set(
		cell_list,
		html_cell_new(
			cell_label_datum,
			1 /* large_boolean */,
			1 /* bold_boolean */ ) );

	for (	i = 0;
		i < empty_cell_count;
		i++ )
	{
		list_set(
			cell_list,
			html_cell_new(
				(char *)0 /* datum */,
				0 /* not large_boolean */,
				0 /* bold_boolean */ ) );
	}

	list_set(
		cell_list,
		html_cell_new(
			strdup(
				/* ----------------------*/
				/* Returns static memory */
				/* ----------------------*/
				string_commas_money(
					element_net_income ) ),
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	list_set(
		cell_list,
		html_cell_new(
			net_income_percent_of_income_display,
			0 /* not large_boolean */,
			0 /* not bold_boolean */ ) );

	if ( list_length( statement_prior_year_list ) )
	{
		LIST *prior_net_income_data_list =
			income_statement_prior_net_income_data_list(
				element_net_income /* current_net_income */,
				statement_prior_year_list );

		list_set_list(
			cell_list,
			html_cell_list(
				prior_net_income_data_list ) );
	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	html_row_new( cell_list );
}

INCOME_STATEMENT_SUBCLASS_AGGR_HTML *
	income_statement_subclass_aggr_html_new(
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double element_net_income,
		char *net_income_percent_of_income_display,
		char *net_income_label )
{
	INCOME_STATEMENT_SUBCLASS_AGGR_HTML *
		income_statement_subclass_aggr_html;

	if ( !statement
	||   !net_income_percent_of_income_display
	||   !net_income_label )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( statement->element_statement_list ) ) return NULL;

	income_statement_subclass_aggr_html =
		income_statement_subclass_aggr_html_calloc();

	income_statement_subclass_aggr_html->
		statement_subclass_aggr_html_list =
			statement_subclass_aggr_html_list_new(
				statement->element_statement_list,
				statement_prior_year_list );

	income_statement_subclass_aggr_html->row_list =
		statement_subclass_aggr_html_list_row_list(
			income_statement_subclass_aggr_html->
				statement_subclass_aggr_html_list );

	list_set(
		income_statement_subclass_aggr_html->row_list,
		income_statement_html_net_income_row(
			statement_prior_year_list,
			element_net_income,
			net_income_percent_of_income_display,
			net_income_label,
			1 /* empty_cell_count */ ) );

	return income_statement_subclass_aggr_html;
}

INCOME_STATEMENT_SUBCLASS_AGGR_HTML *
	income_statement_subclass_aggr_html_calloc(
		void )
{
	INCOME_STATEMENT_SUBCLASS_AGGR_HTML *
		income_statement_subclass_aggr_html;

	if ( ! ( income_statement_subclass_aggr_html =
		   calloc(1,
			  sizeof ( INCOME_STATEMENT_SUBCLASS_AGGR_HTML ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return income_statement_subclass_aggr_html;
}

INCOME_STATEMENT_SUBCLASS_AGGR_LATEX *
	income_statement_subclass_aggr_latex_new(
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double element_net_income,
		char *net_income_percent_of_income_display,
		char *income_statement_net_income_label )
{
	INCOME_STATEMENT_SUBCLASS_AGGR_LATEX *
		income_statement_subclass_aggr_latex;

	if ( !statement
	||   !net_income_percent_of_income_display
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	income_statement_subclass_aggr_latex =
		income_statement_subclass_aggr_latex_calloc();

	income_statement_subclass_aggr_latex->
		statement_subclass_aggr_latex_list =
			statement_subclass_aggr_latex_list_new(
				statement->element_statement_list,
				statement_prior_year_list );

	income_statement_subclass_aggr_latex->row_list =
		statement_subclass_aggr_latex_list_row_list(
			income_statement_subclass_aggr_latex->
				statement_subclass_aggr_latex_list );

	list_set(
		income_statement_subclass_aggr_latex->row_list,
		income_statement_latex_net_income_row(
			statement_prior_year_list,
			element_net_income,
			net_income_percent_of_income_display,
			income_statement_net_income_label,
			income_statement_subclass_aggr_latex->
				statement_subclass_aggr_latex_list->
				column_list
				/* latex_column_list */,
			1 /* empty_cell_count */ ) );

	return income_statement_subclass_aggr_latex;
}

INCOME_STATEMENT_SUBCLASS_AGGR_LATEX *
	income_statement_subclass_aggr_latex_calloc(
		void )
{
	INCOME_STATEMENT_SUBCLASS_AGGR_LATEX *
		income_statement_subclass_aggr_latex;

	if ( ! ( income_statement_subclass_aggr_latex =
		    calloc(
			1,
			sizeof ( INCOME_STATEMENT_SUBCLASS_AGGR_LATEX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return income_statement_subclass_aggr_latex;
}

HTML_TABLE *income_statement_html_table(
		char *statement_caption_sub_title,
		LIST *column_list,
		LIST *row_list )
{
	HTML_TABLE *html_table;

	if ( !statement_caption_sub_title
	||   !list_length( column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	html_table =
		html_table_new(
			(char *)0 /* title */,
			statement_caption_sub_title,
			(char *)0 /* sub_sub_title */ );

	html_table->column_list = column_list;
	html_table->row_list = row_list;

	return html_table;
}

LATEX_TABLE *income_statement_latex_table(
		char *statement_caption_combined,
		LIST *latex_column_list,
		LIST *latex_row_list )
{
	if ( !statement_caption_combined
	||   !list_length( latex_column_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	latex_table_new(
		statement_caption_combined /* title */,
		latex_column_list,
		latex_row_list );
}

double income_statement_fetch_net_income(
		char *end_date_time_string )
{
	LIST *element_name_list;
	STATEMENT *statement;

	if ( !end_date_time_string )
	{
		char message[ 128 ];

		sprintf(message, "end_date_time_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	element_name_list =
		income_statement_element_name_list(
			ELEMENT_REVENUE,
			ELEMENT_EXPENSE,
			ELEMENT_GAIN,
			ELEMENT_LOSS );

	statement =
		statement_fetch(
			(char *)0 /* application_name */,
			(char *)0 /* process_name */,
			0 /* prior_year_count */,
			element_name_list,
			(char *)0 /* transaction_begin_date_string */,
			end_date_time_string,
			0 /* not fetch_transaction */ );

	if ( !statement ) return 0.0;

	return
	element_net_income(
		statement->element_statement_list );
}

