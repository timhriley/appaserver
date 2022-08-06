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
			char *tex_filename,
			char *dvi_filename,
			char *working_directory,
			boolean statement_pdf_landscape_boolean,
			char *statement_logo_filename,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *income_statement_label )
{
	INCOME_STATEMENT_SUBCLASS_DISPLAY_LATEX *
		income_statement_subclass_display_latex;

	if ( !tex_filename
	||   !dvi_filename
	||   !working_directory
	||   !statement
	||   !net_income_percent_of_revenue_display
	||   !income_statement_label )
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
			income_statement_label ) );

	income_statement_subclass_display_latex->latex =
		latex_new(
			tex_filename,
			dvi_filename,
			working_directory,
			statement_pdf_landscape_boolean,
			statement_logo_filename );

	list_set(
		income_statement_subclass_display_latex->latex->table_list,
		income_statement_subclass_display_latex_table(
			statement->caption,
			income_statement_subclass_display_latex->
				statement_subclass_display_latex_list->
				heading_list,
			income_statement_subclass_display_latex->row_list ) );

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

LATEX_ROW *income_statement_subclass_display_latex_net_income_row(
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *income_statement_label )
{
	LATEX_ROW *latex_row;

	if ( !net_income_percent_of_revenue_display
	||   !income_statement_label )
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
		statement_cell_data_label( income_statement_label ),
		1 /* large_bold */ );

	latex_column_data_set(
		latex_row->column_data_list,
		(char *)0,
		0 /* not large_bold */ );

	latex_column_data_set(
		latex_row->column_data_list,
		(char *)0,
		0 /* not large_bold */ );

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( timlib_place_commas_in_money( element_net_income ) ),
		0 /* not large_bold */ );

	latex_column_data_set(
		latex_row->column_data_list,
		net_income_percent_of_revenue_display,
		0 /* not large_bold */ );

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
			char *income_statement_label,
			pid_t process_id )
{
	INCOME_STATEMENT_PDF *income_statement_pdf;

	if ( !application_name
	||   !process_name
	||   !document_root_directory
	||   !statement
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
				statement->logo_filename,
				statement,
				statement_prior_year_list,
				element_net_income,
				net_income_percent_of_revenue_display,
				income_statement_label );

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

	if ( income_statement->statement_output_medium == output_table )
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

	income_statement->label =
		income_statement_label(
			argv_0 );

	if ( income_statement->statement_output_medium == output_PDF )
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
				income_statement->label,
				getpid() /* process_id */ );
	}

	if ( income_statement->statement_output_medium == output_table )
	{
/*
		income_statement->income_statement_table =
			income_statement_table_new(
				income_statement->
					statement_subclassification_option,
				income_statement->statement,
				income_statement->statement_prior_year_list,
				income_statement->element_net_income,
				income_statement->
					net_income_percent_of_revenue_display,
				income_statement->label );
*/
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
		statement_account_revenue_percent_string(
			percent_of_revenue );
	}
}

char *income_statement_label( char *argv_0 )
{
	char label[ 128 ];

	if ( !argv_0 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: argv_0 is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	format_initial_capital( label, argv_0 );

	return strdup( label );
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
			char *income_statement_label )
{
	INCOME_STATEMENT_LATEX *income_statement_latex;

	if ( !tex_filename
	||   !dvi_filename
	||   !working_directory
	||   !statement
	||   !net_income_percent_of_revenue_display
	||   !income_statement_label )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	income_statement_latex = income_statement_latex_calloc();

	if ( statement_subclassification_option == subclassification_display )
	{
		income_statement_latex->
			income_statement_subclass_display_latex =
				income_statement_subclass_display_latex_new(
					tex_filename,
					dvi_filename,
					working_directory,
					statement_pdf_landscape_boolean,
					statement_logo_filename,
					statement,
					statement_prior_year_list,
					element_net_income,
					net_income_percent_of_revenue_display,
					income_statement_label );
	}
	else
	if ( statement_subclassification_option == subclassification_omit )
	{
/*
		income_statement_latex->
			income_statement_subclass_omit_latex =
				income_statement_subclass_omit_latex_new(
					tex_filename,
					dvi_filename,
					working_directory,
					statement_pdf_landscape_boolean,
					statement_logo_filename,
					statement,
					statement_prior_year_list,
					element_net_income,
					net_income_percent_of_revenue_display,
					income_statement_label );
*/
	}
	else
	if ( statement_subclassification_option == subclassification_aggregate )
	{
/*
		income_statement_latex->
			income_statement_subclass_aggr_latex =
				income_statement_subclass_aggr_latex_new(
					tex_filename,
					dvi_filename,
					working_directory,
					statement_pdf_landscape_boolean,
					statement_logo_filename,
					statement,
					statement_prior_year_list,
					element_net_income,
					net_income_percent_of_revenue_display,
					income_statement_label );
*/
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

