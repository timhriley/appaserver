/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/income_statement.c			*/
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
#include "income_statement.h"

INCOME_STATEMENT_SUBCLASS_DISPLAY_LATEX *
	income_statement_subclass_display_latex_new(
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *income_statement_net_income_label )
{
	INCOME_STATEMENT_SUBCLASS_DISPLAY_LATEX *
		income_statement_subclass_display_latex;

	if ( !statement
	||   !net_income_percent_of_revenue_display
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
		statement_subclass_display_latex_list_extract_row_list(
			income_statement_subclass_display_latex->
				statement_subclass_display_latex_list );

	list_set(
		income_statement_subclass_display_latex->row_list,
		income_statement_subclass_display_latex_net_income_row(
			statement_prior_year_list,
			element_net_income,
			net_income_percent_of_revenue_display,
			income_statement_net_income_label ) );

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
			sizeof( INCOME_STATEMENT_SUBCLASS_DISPLAY_LATEX ) ) ) )
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
			char *net_income_percent_of_revenue_display,
			char *income_statement_net_income_label )
{
	INCOME_STATEMENT_SUBCLASS_OMIT_LATEX *
		income_statement_subclass_omit_latex;

	if ( !statement
	||   !net_income_percent_of_revenue_display
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
		statement_subclass_omit_latex_list_extract_row_list(
			income_statement_subclass_omit_latex->
				statement_subclass_omit_latex_list );

	list_set(
		income_statement_subclass_omit_latex->row_list,
		income_statement_subclass_omit_latex_net_income_row(
			statement_prior_year_list,
			element_net_income,
			net_income_percent_of_revenue_display,
			income_statement_net_income_label ) );

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
			sizeof( INCOME_STATEMENT_SUBCLASS_OMIT_LATEX ) ) ) )
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

LATEX_TABLE *income_statement_subclass_display_latex_table(
			char *caption,
			LIST *heading_list,
			LIST *row_list )
{
	LATEX_TABLE *latex_table;

	if ( !caption
	||   !list_length( heading_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_table = latex_table_new( caption );
	latex_table->heading_list = heading_list;
	latex_table->row_list = row_list;

	return latex_table;
}

LATEX_TABLE *income_statement_subclass_omit_latex_table(
			char *caption,
			LIST *heading_list,
			LIST *row_list )
{
	LATEX_TABLE *latex_table;

	if ( !caption
	||   !list_length( heading_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_table = latex_table_new( caption );
	latex_table->heading_list = heading_list;
	latex_table->row_list = row_list;

	return latex_table;
}

LATEX_ROW *income_statement_subclass_display_latex_net_income_row(
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *income_statement_net_income_label )
{
	LATEX_ROW *latex_row;

	if ( !net_income_percent_of_revenue_display
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
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
		statement_cell_data_label( income_statement_net_income_label ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

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
		strdup( timlib_place_commas_in_money( element_net_income ) ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		net_income_percent_of_revenue_display,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		latex_column_data_list_set(
			latex_row->column_data_list,
			income_statement_prior_net_income_data_list(
				element_net_income /* current_net_income */,
				statement_prior_year_list ) );
	}

	return latex_row;
}

LATEX_ROW *income_statement_subclass_omit_latex_net_income_row(
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *income_statement_net_income_label )
{
	LATEX_ROW *latex_row;

	if ( !net_income_percent_of_revenue_display
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
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
		statement_cell_data_label( income_statement_net_income_label ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		(char *)0,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( timlib_place_commas_in_money( element_net_income ) ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		net_income_percent_of_revenue_display,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		latex_column_data_list_set(
			latex_row->column_data_list,
			income_statement_prior_net_income_data_list(
				element_net_income /* current_net_income */,
				statement_prior_year_list ) );
	}

	return latex_row;
}

INCOME_STATEMENT_PDF *income_statement_pdf_new(
			char *application_name,
			char *process_name,
			char *document_root_directory,
			enum statement_subclassification_option
				statement_subclassification_option,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *income_statement_net_income_label,
			pid_t process_id )
{
	INCOME_STATEMENT_PDF *income_statement_pdf;

	if ( !application_name
	||   !process_name
	||   !document_root_directory
	||   !statement
	||   !statement->statement_caption
	||   !process_id )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	income_statement_pdf = income_statement_pdf_calloc();

	income_statement_pdf->statement_pdf_landscape_boolean =
		statement_pdf_landscape_boolean(
			list_length( statement_prior_year_list )
				/* statement_prior_year_list_length */ );

	income_statement_pdf->statement_link =
		statement_link_new(
			application_name,
			process_name,
			document_root_directory,
			statement->transaction_begin_date_string,
			statement->transaction_as_of_date,
			(char *)0 /* statement_pdf_preclose_key */,
			process_id );

	income_statement_pdf->
		income_statement_latex =
			income_statement_latex_new(
				statement_subclassification_option,
				income_statement_pdf->
					statement_link->
					tex_filename,
				income_statement_pdf->
					statement_link->
					dvi_filename,
				income_statement_pdf->
					statement_link->
					working_directory,
				income_statement_pdf->
					statement_pdf_landscape_boolean,
				statement->statement_caption->logo_filename,
				statement,
				statement_prior_year_list,
				element_net_income,
				net_income_percent_of_revenue_display,
				income_statement_net_income_label );

	return income_statement_pdf;
}

INCOME_STATEMENT_PDF *income_statement_pdf_calloc( void )
{
	INCOME_STATEMENT_PDF *income_statement_pdf;

	if ( ! ( income_statement_pdf =
			calloc( 1, sizeof( INCOME_STATEMENT_PDF ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return income_statement_pdf;
}

INCOME_STATEMENT *income_statement_fetch(
			char *argv_0,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_name,
			char *document_root_directory,
			char *as_of_date,
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

	income_statement = income_statement_calloc();

	income_statement->statement_subclassification_option =
		statement_resolve_subclassification_option(
			subclassification_option_string );

	income_statement->statement_output_medium =
		statement_resolve_output_medium(
			output_medium_string );

	income_statement->transaction_as_of_date =
		transaction_as_of_date(
			TRANSACTION_TABLE,
			as_of_date );

	if ( !income_statement->transaction_as_of_date )
	{
		return (INCOME_STATEMENT *)0;
	}

	income_statement->transaction_begin_date_string =
		transaction_begin_date_string(
			TRANSACTION_TABLE,
			income_statement->transaction_as_of_date );

	if ( !income_statement->transaction_begin_date_string )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: transaction_begin_date_string(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			income_statement->transaction_as_of_date );
		exit( 1 );
	}


	income_statement->element_name_list =
		income_statement_element_name_list(
			ELEMENT_REVENUE,
			ELEMENT_EXPENSE,
			ELEMENT_GAIN,
			ELEMENT_LOSS );

	income_statement->transaction_closing_entry_exists =
		transaction_closing_entry_exists(
			TRANSACTION_TABLE,
			TRANSACTION_CLOSE_TIME,
			income_statement->transaction_as_of_date );

	income_statement->transaction_date_time_closing =
		transaction_date_time_closing(
			TRANSACTION_PRECLOSE_TIME,
			TRANSACTION_CLOSE_TIME,
			income_statement->transaction_as_of_date,
			income_statement->
				transaction_closing_entry_exists
					/* preclose_time_boolean */ );

	income_statement->statement =
		statement_fetch(
			application_name,
			process_name,
			income_statement->element_name_list,
			income_statement->transaction_begin_date_string,
			income_statement->transaction_as_of_date,
			income_statement->transaction_date_time_closing,
			0 /* not fetch_transaction */ );

	if ( income_statement->statement_output_medium ==
		statement_output_table )
	{
		element_account_transaction_count_set(
			income_statement->statement->element_statement_list,
			income_statement->transaction_begin_date_string,
			income_statement->transaction_date_time_closing );

		element_account_action_string_set(
			income_statement->statement->element_statement_list,
			application_name,
			session_key,
			login_name,
			role_name,
			income_statement->transaction_begin_date_string,
			income_statement->transaction_date_time_closing );
	}

	if ( prior_year_count )
	{
		income_statement->statement_prior_year_list =
			statement_prior_year_list(
				income_statement->element_name_list,
				income_statement->transaction_date_time_closing,
				prior_year_count,
				income_statement->statement );
	}

	if (	income_statement->statement_subclassification_option ==
		subclassification_omit )
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

	income_statement->net_income_percent_of_revenue_display =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		income_statement_net_income_percent_of_revenue_display(
			ELEMENT_REVENUE,
			income_statement->element_net_income,
			income_statement->statement->element_statement_list );

	income_statement->net_income_label =
		income_statement_net_income_label(
			argv_0 );

	if ( income_statement->statement_output_medium ==
		statement_output_PDF )
	{
		income_statement->income_statement_pdf =
			income_statement_pdf_new(
				application_name,
				process_name,
				document_root_directory,
				income_statement->
					statement_subclassification_option,
				income_statement->statement,
				income_statement->statement_prior_year_list,
				income_statement->element_net_income,
				income_statement->
					net_income_percent_of_revenue_display,
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
					net_income_percent_of_revenue_display,
				income_statement->net_income_label );
	}

	return income_statement;
}

INCOME_STATEMENT *income_statement_calloc( void )
{
	INCOME_STATEMENT *income_statement;

	if ( ! ( income_statement = calloc( 1, sizeof( INCOME_STATEMENT ) ) ) )
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

char *income_statement_net_income_percent_of_revenue_display(
			char *element_revenue,
			double element_net_income,
			LIST *element_statement_list )
{
	ELEMENT *element;

	if ( ! ( element =
			element_seek(
				element_revenue,
				element_statement_list ) ) )
	{
		return strdup( "" );
	}
	else
	{
		int percent_of_revenue =
			float_percent_of_total(
				element_net_income,
				element->sum );

		return
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		statement_account_percent_string(
			0 /* percent_of_asset */,
			percent_of_revenue );
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

	if ( strcmp( argv_0, "income_statement" ) == 0 )
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

INCOME_STATEMENT_LATEX *
	income_statement_latex_new(
			enum statement_subclassification_option
				statement_subclassification_option,
			char *tex_filename,
			char *dvi_filename,
			char *working_directory,
			boolean statement_pdf_landscape_boolean,
			char *statement_logo_filename,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *income_statement_net_income_label )
{
	INCOME_STATEMENT_LATEX *income_statement_latex;

	if ( !tex_filename
	||   !dvi_filename
	||   !working_directory
	||   !statement
	||   !statement->statement_caption
	||   !net_income_percent_of_revenue_display
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	income_statement_latex = income_statement_latex_calloc();

	income_statement_latex->latex =
		latex_new(
			tex_filename,
			dvi_filename,
			working_directory,
			statement_pdf_landscape_boolean,
			statement_logo_filename );

	if ( statement_subclassification_option == subclassification_display )
	{
		income_statement_latex->
			income_statement_subclass_display_latex =
				income_statement_subclass_display_latex_new(
					statement,
					statement_prior_year_list,
					element_net_income,
					net_income_percent_of_revenue_display,
					income_statement_net_income_label );

		if ( !income_statement_latex->
			income_statement_subclass_display_latex )
		{
			free( income_statement_latex );
			return (INCOME_STATEMENT_LATEX *)0;
		}

		list_set(
			income_statement_latex->latex->table_list,
			income_statement_latex_table(
				statement->statement_caption->string,
				income_statement_latex->
				   income_statement_subclass_display_latex->
				   statement_subclass_display_latex_list->
				   heading_list,
				income_statement_latex->
				   income_statement_subclass_display_latex->
				   row_list ) );
	}
	else
	if ( statement_subclassification_option == subclassification_omit )
	{
		income_statement_latex->
			income_statement_subclass_omit_latex =
				income_statement_subclass_omit_latex_new(
					statement,
					statement_prior_year_list,
					element_net_income,
					net_income_percent_of_revenue_display,
					income_statement_net_income_label );

		if ( !income_statement_latex->
			income_statement_subclass_omit_latex )
		{
			free( income_statement_latex );
			return (INCOME_STATEMENT_LATEX *)0;
		}

		list_set(
			income_statement_latex->latex->table_list,
			income_statement_latex_table(
				statement->statement_caption->string,
				income_statement_latex->
				   income_statement_subclass_omit_latex->
				   statement_subclass_omit_latex_list->
				   heading_list,
				income_statement_latex->
				   income_statement_subclass_omit_latex->
				   row_list ) );
	}
	else
	if ( statement_subclassification_option == subclassification_aggregate )
	{
		income_statement_latex->
			income_statement_subclass_aggr_latex =
				income_statement_subclass_aggr_latex_new(
					statement,
					statement_prior_year_list,
					element_net_income,
					net_income_percent_of_revenue_display,
					income_statement_net_income_label );

		if ( !income_statement_latex->
			income_statement_subclass_aggr_latex )
		{
			free( income_statement_latex );
			return (INCOME_STATEMENT_LATEX *)0;
		}

		list_set(
			income_statement_latex->latex->table_list,
			income_statement_latex_table(
				statement->statement_caption->string,
				income_statement_latex->
				   income_statement_subclass_aggr_latex->
				   statement_subclass_aggr_latex_list->
				   heading_list,
				income_statement_latex->
				   income_statement_subclass_aggr_latex->
				   row_list ) );
	}

	return income_statement_latex;
}

INCOME_STATEMENT_LATEX *income_statement_latex_calloc( void )
{
	INCOME_STATEMENT_LATEX *income_statement_latex;

	if ( ! ( income_statement_latex =
			calloc( 1, sizeof( INCOME_STATEMENT_LATEX ) ) ) )
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
			char *net_income_percent_of_revenue_display,
			char *net_income_label )
{
	INCOME_STATEMENT_HTML *income_statement_html;

	if ( !statement
	||   !statement->statement_caption
	||   !net_income_percent_of_revenue_display
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
		subclassification_display )
	{
		income_statement_html->
			income_statement_subclass_display_html =
				income_statement_subclass_display_html_new(
					statement,
					statement_prior_year_list,
					element_net_income,
					net_income_percent_of_revenue_display,
					net_income_label );

		if ( !income_statement_html->
			income_statement_subclass_display_html )
		{
			free( income_statement_html );
			return (INCOME_STATEMENT_HTML *)0;
		}

		income_statement_html->html_table =
			income_statement_html_table(
			     	statement->statement_caption->subtitle,
			     	income_statement_html->
					income_statement_subclass_display_html->
					statement_subclass_display_html_list->
					heading_list,
			     	income_statement_html->
					income_statement_subclass_display_html->
					row_list );
	}
	else
	if (	statement_subclassification_option ==
		subclassification_omit )
	{
		income_statement_html->
			income_statement_subclass_omit_html =
				income_statement_subclass_omit_html_new(
					statement,
					statement_prior_year_list,
					element_net_income,
					net_income_percent_of_revenue_display,
					net_income_label );

		if ( !income_statement_html->
			income_statement_subclass_omit_html )
		{
			free( income_statement_html );
			return (INCOME_STATEMENT_HTML *)0;
		}

		income_statement_html->html_table =
			income_statement_html_table(
				statement->statement_caption->subtitle,
				income_statement_html->
					income_statement_subclass_omit_html->
					statement_subclass_omit_html_list->
					heading_list,
				income_statement_html->
					income_statement_subclass_omit_html->
					row_list );
	}
	else
	if ( statement_subclassification_option == subclassification_aggregate )
	{
		income_statement_html->
			income_statement_subclass_aggr_html =
				income_statement_subclass_aggr_html_new(
					statement,
					statement_prior_year_list,
					element_net_income,
					net_income_percent_of_revenue_display,
					net_income_label );

		if ( !income_statement_html->
			income_statement_subclass_aggr_html )
		{
			free( income_statement_html );
			return (INCOME_STATEMENT_HTML *)0;
		}

		income_statement_html->html_table =
			income_statement_html_table(
				statement->statement_caption->subtitle,
				income_statement_html->
					income_statement_subclass_aggr_html->
					statement_subclass_aggr_html_list->
					heading_list,
				income_statement_html->
					income_statement_subclass_aggr_html->
					row_list );
	}
	else
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: invalid statement_subclassification_option.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return income_statement_html;
}

INCOME_STATEMENT_HTML *income_statement_html_calloc( void )
{
	INCOME_STATEMENT_HTML *income_statement_html;

	if ( ! ( income_statement_html =
			calloc( 1, sizeof( INCOME_STATEMENT_HTML ) ) ) )
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
			char *net_income_percent_of_revenue_display,
			char *net_income_label )
{
	INCOME_STATEMENT_SUBCLASS_DISPLAY_HTML *
		income_statement_subclass_display_html;

	if ( !statement
	||   !net_income_percent_of_revenue_display
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
		statement_subclass_display_html_list_extract_row_list(
			income_statement_subclass_display_html->
				statement_subclass_display_html_list );

	list_set(
		income_statement_subclass_display_html->row_list,
		income_statement_subclass_display_html_net_income_row(
			statement_prior_year_list,
			element_net_income,
			net_income_percent_of_revenue_display,
			net_income_label ) );

	return income_statement_subclass_display_html;
}

INCOME_STATEMENT_SUBCLASS_DISPLAY_HTML *
	income_statement_subclass_display_html_calloc(
			void )
{
	INCOME_STATEMENT_SUBCLASS_DISPLAY_HTML *
		income_statement_subclass_display_html;

	if ( ! ( income_statement_subclass_display_html =
		   calloc(1,
			  sizeof( INCOME_STATEMENT_SUBCLASS_DISPLAY_HTML ) ) ) )
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

HTML_ROW *income_statement_subclass_display_html_net_income_row(
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *net_income_label )
{
	HTML_ROW *html_row;

	if ( !net_income_percent_of_revenue_display
	||   !net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
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
		statement_cell_data_label( net_income_label ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

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
		strdup( timlib_place_commas_in_money( element_net_income ) ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		net_income_percent_of_revenue_display,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		html_cell_data_list_set(
			html_row->cell_list,
			income_statement_prior_net_income_data_list(
				element_net_income /* current_net_income */,
				statement_prior_year_list ) );
	}

	return html_row;
}

INCOME_STATEMENT_SUBCLASS_OMIT_HTML *
	income_statement_subclass_omit_html_new(
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *net_income_label )
{
	INCOME_STATEMENT_SUBCLASS_OMIT_HTML *
		income_statement_subclass_omit_html;

	if ( !statement
	||   !net_income_percent_of_revenue_display
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
		statement_subclass_omit_html_list_extract_row_list(
			income_statement_subclass_omit_html->
				statement_subclass_omit_html_list );

	list_set(
		income_statement_subclass_omit_html->row_list,
		income_statement_subclass_omit_html_net_income_row(
			statement_prior_year_list,
			element_net_income,
			net_income_percent_of_revenue_display,
			net_income_label ) );

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
			  sizeof( INCOME_STATEMENT_SUBCLASS_OMIT_HTML ) ) ) )
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

HTML_ROW *income_statement_subclass_omit_html_net_income_row(
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *net_income_label )
{
	HTML_ROW *html_row;

	if ( !net_income_percent_of_revenue_display
	||   !net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
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
		statement_cell_data_label( net_income_label ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		(char *)0,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		strdup( timlib_place_commas_in_money( element_net_income ) ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		net_income_percent_of_revenue_display,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		html_cell_data_list_set(
			html_row->cell_list,
			income_statement_prior_net_income_data_list(
				element_net_income /* current_net_income */,
				statement_prior_year_list ) );
	}

	return html_row;
}

INCOME_STATEMENT_SUBCLASS_AGGR_HTML *
	income_statement_subclass_aggr_html_new(
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *net_income_label )
{
	INCOME_STATEMENT_SUBCLASS_AGGR_HTML *
		income_statement_subclass_aggr_html;

	if ( !statement
	||   !net_income_percent_of_revenue_display
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
		return (INCOME_STATEMENT_SUBCLASS_AGGR_HTML *)0;

	income_statement_subclass_aggr_html =
		income_statement_subclass_aggr_html_calloc();

	income_statement_subclass_aggr_html->
		statement_subclass_aggr_html_list =
			statement_subclass_aggr_html_list_new(
				statement->element_statement_list,
				statement_prior_year_list );

	income_statement_subclass_aggr_html->row_list =
		statement_subclass_aggr_html_list_extract_row_list(
			income_statement_subclass_aggr_html->
				statement_subclass_aggr_html_list );

	list_set(
		income_statement_subclass_aggr_html->row_list,
		income_statement_subclass_aggr_html_net_income_row(
			statement_prior_year_list,
			element_net_income,
			net_income_percent_of_revenue_display,
			net_income_label ) );

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
			  sizeof( INCOME_STATEMENT_SUBCLASS_AGGR_HTML ) ) ) )
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

HTML_ROW *income_statement_subclass_aggr_html_net_income_row(
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *net_income_label )
{
	HTML_ROW *html_row;

	if ( !net_income_percent_of_revenue_display
	||   !net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
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
		statement_cell_data_label( net_income_label ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		(char *)0,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		strdup( timlib_place_commas_in_money( element_net_income ) ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	html_cell_data_set(
		html_row->cell_list,
		net_income_percent_of_revenue_display,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		html_cell_data_list_set(
			html_row->cell_list,
			income_statement_prior_net_income_data_list(
				element_net_income /* current_net_income */,
				statement_prior_year_list ) );
	}

	return html_row;
}

INCOME_STATEMENT_SUBCLASS_AGGR_LATEX *
	income_statement_subclass_aggr_latex_new(
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *income_statement_net_income_label )
{
	INCOME_STATEMENT_SUBCLASS_AGGR_LATEX *
		income_statement_subclass_aggr_latex;

	if ( !statement
	||   !net_income_percent_of_revenue_display
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
		statement_subclass_aggr_latex_list_extract_row_list(
			income_statement_subclass_aggr_latex->
				statement_subclass_aggr_latex_list );

	list_set(
		income_statement_subclass_aggr_latex->row_list,
		income_statement_subclass_aggr_latex_net_income_row(
			statement_prior_year_list,
			element_net_income,
			net_income_percent_of_revenue_display,
			income_statement_net_income_label ) );

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
			    sizeof( INCOME_STATEMENT_SUBCLASS_AGGR_LATEX ) ) ) )
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

LATEX_TABLE *income_statement_subclass_aggr_latex_table(
			char *caption,
			LIST *heading_list,
			LIST *row_list )
{
	LATEX_TABLE *latex_table;

	if ( !caption
	||   !list_length( heading_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_table = latex_table_new( caption );
	latex_table->heading_list = heading_list;
	latex_table->row_list = row_list;

	return latex_table;
}

LATEX_ROW *income_statement_subclass_aggr_latex_net_income_row(
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *income_statement_net_income_label )
{
	LATEX_ROW *latex_row;

	if ( !net_income_percent_of_revenue_display
	||   !income_statement_net_income_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
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
		statement_cell_data_label( income_statement_net_income_label ),
		1 /* large_boolean */,
		1 /* bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		(char *)0,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( timlib_place_commas_in_money( element_net_income ) ),
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	latex_column_data_set(
		latex_row->column_data_list,
		net_income_percent_of_revenue_display,
		0 /* not large_boolean */,
		0 /* not bold_boolean */ );

	if ( list_length( statement_prior_year_list ) )
	{
		latex_column_data_list_set(
			latex_row->column_data_list,
			income_statement_prior_net_income_data_list(
				element_net_income /* current_net_income */,
				statement_prior_year_list ) );
	}

	return latex_row;
}

HTML_TABLE *income_statement_html_table(
			char *statement_caption_subtitle,
			LIST *heading_list,
			LIST *row_list )
{
	HTML_TABLE *html_table;

	if ( !statement_caption_subtitle
	||   !list_length( heading_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: () returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	html_table =
		html_table_new(
			(char *)0 /* title */,
			statement_caption_subtitle,
			(char *)0 /* sub_sub_title */ );

	html_table->heading_list = heading_list;
	html_table->row_list = row_list;

	return html_table;
}

LATEX_TABLE *income_statement_latex_table(
			char *statement_caption,
			LIST *heading_list,
			LIST *row_list )
{
	LATEX_TABLE *latex_table;

	if ( !statement_caption
	||   !list_length( heading_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: () returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_table =
		latex_table_new(
			statement_caption );

	latex_table->heading_list = heading_list;
	latex_table->row_list = row_list;

	return latex_table;
}

double income_statement_fetch_net_income(
			char *transaction_date_time_closing )
{
	LIST *element_name_list;
	STATEMENT *statement;

	if ( !transaction_date_time_closing )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: transaction_date_time_closing is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
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
			element_name_list,
			(char *)0 /* transaction_begin_date_string */,
			(char *)0 /* transaction_as_of_date */,
			transaction_date_time_closing,
			0 /* not fetch_transaction */ );

	if ( !statement ) return 0.0;

	return
	element_net_income(
		statement->element_statement_list );
}

