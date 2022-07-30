/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/trial_balance.c			*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "float.h"
#include "timlib.h"
#include "date.h"
#include "transaction.h"
#include "statement.h"
#include "element.h"
#include "account.h"
#include "trial_balance.h"

TRIAL_BALANCE *trial_balance_calloc( void )
{
	TRIAL_BALANCE *trial_balance;

	if ( ! ( trial_balance = calloc( 1, sizeof( TRIAL_BALANCE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: () returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return trial_balance;
}

TRIAL_BALANCE *trial_balance_fetch(
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
	TRIAL_BALANCE *trial_balance;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !process_name
	||   !as_of_date
	||   !subclassifiction_option_string
	||   !output_medium_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	trial_balance = trial_balance_calloc();

	trial_balance->statement_subclassification_option =
		statement_resolve_subclassification_option(
			subclassifiction_option_string );

	trial_balance->statement_output_medium =
		statement_resolve_output_medium(
			output_medium_string );

	if ( ! ( trial_balance->transaction_as_of_date =
			transaction_as_of_date(
				TRANSACTION_TABLE,
				as_of_date ) ) )
	{
		free( trial_balance );
		return (TRIAL_BALANCE *)0;
	}

	if ( ! ( trial_balance->transaction_begin_date_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			transaction_begin_date_string(
				TRANSACTION_TABLE,
				trial_balance->transaction_as_of_date ) ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: transaction_begin_date_string(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			trial_balance->transaction_as_of_date );
		exit( 1 );
	}

	trial_balance->filter_element_name_list =
		trial_balance_filter_element_name_list();

	trial_balance->transaction_closing_entry_exists =
		transaction_closing_entry_exists(
			TRANSACTION_TABLE,
			TRANSACTION_CLOSE_TIME,
			trial_balance->transaction_as_of_date );

	if ( trial_balance->transaction_closing_entry_exists )
	{
		trial_balance->transaction_date_time_closing =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			transaction_date_time_closing(
				TRANSACTION_PRECLOSE_TIME,
				TRANSACTION_CLOSE_TIME,
				trial_balance->transaction_as_of_date,
				1 /* preclose_time_boolean */ );

		trial_balance->preclose_statement =
			statement_fetch(
				application_name,
				process_name,
				trial_balance->filter_element_name_list,
				trial_balance->transaction_begin_date_string,
				trial_balance->transaction_as_of_date,
				trial_balance->transaction_date_time_closing );

		element_account_transaction_count_set(
			trial_balance->
				preclose_statement->
				element_statement_list,
			trial_balance->transaction_begin_date_string,
			trial_balance->transaction_date_time_closing );

		if ( prior_year_count )
		{
			trial_balance->preclose_prior_year_list =
				statement_prior_year_list(
					trial_balance->
						filter_element_name_list,
					trial_balance->
						transaction_date_time_closing,
					prior_year_count,
					trial_balance->preclose_statement
						/* statement */ );
		}
	}

	trial_balance->transaction_date_time_closing =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		transaction_date_time_closing(
			TRANSACTION_PRECLOSE_TIME,
			TRANSACTION_CLOSE_TIME,
			trial_balance->transaction_as_of_date,
			0 /* not preclose_time_boolean */ );

	trial_balance->statement =
		statement_fetch(
			application_name,
			process_name,
			trial_balance->filter_element_name_list,
			trial_balance->transaction_begin_date_string,
			trial_balance->transaction_as_of_date,
			trial_balance->transaction_date_time_closing );

	if ( !trial_balance->statement )
	{
		return (TRIAL_BALANCE *)0;
	}

	element_account_transaction_count_set(
		trial_balance->statement->element_statement_list,
		trial_balance->transaction_begin_date_string,
		trial_balance->transaction_date_time_closing );

	if ( prior_year_count )
	{
		trial_balance->prior_year_list =
			statement_prior_year_list(
				trial_balance->
					filter_element_name_list,
				trial_balance->
					transaction_date_time_closing,
				prior_year_count,
				trial_balance->statement );
	}

	if ( trial_balance->transaction_closing_entry_exists )
	{
		trial_balance->preclose_debit_sum =
			element_list_debit_sum(
				trial_balance->
					preclose_statement->
					element_statement_list );

		trial_balance->preclose_credit_sum =
			element_list_credit_sum(
				trial_balance->
					preclose_statement->
					element_statement_list );
	}

	trial_balance->debit_sum =
		element_list_debit_sum(
			trial_balance->
				statement->
				element_statement_list );

	trial_balance->credit_sum =
		element_list_credit_sum(
			trial_balance->
				statement->
				element_statement_list );

	if ( trial_balance->transaction_closing_entry_exists )
	{
		trial_balance->element_list_non_nominal_account_list =
			element_list_non_nominal_account_list(
				trial_balance->
					preclose_statement->
					element_statement_list );

		trial_balance->element_asset =
			element_seek(
				ELEMENT_ASSET,
				trial_balance->
					preclose_statement->
					element_statement_list );

		if ( trial_balance->element_asset )
		{
			trial_balance->element_asset->sum =
				element_sum(
					trial_balance->element_asset );

			account_list_percent_of_asset_set(
				trial_balance->
					element_list_non_nominal_account_list,
				trial_balance->element_asset->sum );
		}

		trial_balance->element_list_nominal_account_list =
			element_list_nominal_account_list(
				trial_balance->
					preclose_statement->
					element_statement_list );

		trial_balance->element_revenue =
			element_seek(
				ELEMENT_REVENUE,
				trial_balance->
					preclose_statement->
					element_statement_list );

		if ( trial_balance->element_revenue )
		{
			trial_balance->element_revenue->sum =
				element_sum(
					trial_balance->element_revenue );

			account_list_percent_of_revenue_set(
				trial_balance->
					element_list_nominal_account_list,
				trial_balance->element_revenue->sum );
		}

	}

	trial_balance->element_list_non_nominal_account_list =
		element_list_non_nominal_account_list(
			trial_balance->
				statement->
				element_statement_list );

	trial_balance->element_asset =
		element_seek(
			ELEMENT_ASSET,
			trial_balance->
				statement->
				element_statement_list );

	if ( trial_balance->element_asset )
	{
		trial_balance->element_asset->sum =
			element_sum(
				trial_balance->element_asset );

		account_list_percent_of_asset_set(
			trial_balance->element_list_non_nominal_account_list,
			trial_balance->element_asset->sum );
	}

	trial_balance->element_list_nominal_account_list =
		element_list_nominal_account_list(
			trial_balance->
				statement->
				element_statement_list );

	trial_balance->element_revenue =
		element_seek(
			ELEMENT_REVENUE,
			trial_balance->
				statement->
				element_statement_list );

	if ( trial_balance->element_revenue )
	{
		trial_balance->element_revenue->sum =
			element_sum(
				trial_balance->element_revenue );

		account_list_percent_of_revenue_set(
			trial_balance->element_list_nominal_account_list,
			trial_balance->element_revenue->sum );
	}

	if ( trial_balance->transaction_closing_entry_exists )
	{
		if (	trial_balance->statement_subclassification_option ==
			subclassification_omit )
		{
			element_list_account_statement_list_set(
				trial_balance->
					statement->
					element_statement_list );
		}
	}

	if (	trial_balance->statement_subclassification_option ==
		subclassification_omit )
	{
		element_list_account_statement_list_set(
			trial_balance->
				statement->
				element_statement_list );
	}

	if ( trial_balance->statement_output_medium == output_PDF )
	{
		trial_balance->trial_balance_pdf =
			trial_balance_pdf_new(
				application_name,
				process_name,
				document_root_directory,
				trial_balance->
					statement_subclassification_option,
				trial_balance->transaction_begin_date_string,
				trial_balance->transaction_as_of_date,
				trial_balance->statement->logo_filename,
				trial_balance->statement->title,
				trial_balance->statement->subtitle,
				trial_balance->preclose_statement,
				trial_balance->preclose_prior_year_list,
				trial_balance->preclose_debit_sum,
				trial_balance->preclose_credit_sum,
				trial_balance->statement,
				trial_balance->prior_year_list,
				trial_balance->debit_sum,
				trial_balance->credit_sum,
				getpid() /* process_id */ );
	}

	return trial_balance;
}

LIST *trial_balance_filter_element_name_list( void )
{
	LIST *element_name_list = list_new();

	list_set( element_name_list, ELEMENT_ASSET );
	list_set( element_name_list, ELEMENT_LIABILITY );
	list_set( element_name_list, ELEMENT_REVENUE );
	list_set( element_name_list, ELEMENT_EXPENSE );
	list_set( element_name_list, ELEMENT_GAIN );
	list_set( element_name_list, ELEMENT_LOSS );
	list_set( element_name_list, ELEMENT_EQUITY );

	return element_name_list;
}

TRIAL_BALANCE_ACCOUNT *trial_balance_account_new(
			char *transaction_as_of_date,
			boolean element_accumulate_debit,
			ACCOUNT *account )
{
	TRIAL_BALANCE_ACCOUNT *trial_balance_account;

	if ( !transaction_as_of_date
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

	trial_balance_account = trial_balance_account_calloc();

	trial_balance_account->balance =
		account->account_journal_latest->balance;

	if ( trial_balance_account->balance < 0.0 )
	{
		trial_balance_account->balance =
			float_abs( trial_balance_account->balance );

		element_accumulate_debit = 1 - element_accumulate_debit;
	}

	if ( element_accumulate_debit )
	{
		trial_balance_account->debit_string =
			strdup( 
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				timlib_place_commas_in_dollars(
					trial_balance_account->balance ) );
	}
	else
	{
		trial_balance_account->credit_string =
			strdup(
				/* --------------------- */
				/* Returns static memory */
				/* --------------------- */
				timlib_place_commas_in_dollars(
					trial_balance_account->balance ) );
	}

	trial_balance_account->date_days_between =
		/* ---------------------- */
		/* Ignores trailing time. */
		/* ---------------------- */
		date_days_between(
			account->account_journal_latest->transaction_date_time,
			transaction_as_of_date );

	trial_balance_account->within_days_between_boolean =
		( trial_balance_account->date_days_between <=
		  TRIAL_BALANCE_DAYS_FOR_EMPHASIS );

	trial_balance_account->percent_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		trial_balance_account_percent_string(
			account->percent_of_asset,
			account->percent_of_revenue );

	trial_balance_account->account = account;

	return trial_balance_account;
}

TRIAL_BALANCE_ACCOUNT *trial_balance_account_calloc( void )
{
	TRIAL_BALANCE_ACCOUNT *trial_balance_account;

	if ( ! ( trial_balance_account =
			calloc( 1, sizeof( TRIAL_BALANCE_ACCOUNT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return trial_balance_account;
}

TRIAL_BALANCE_PDF *trial_balance_pdf_new(
			char *application_name,
			char *process_name,
			char *document_root_directory,
			enum statement_subclassification_option
				statement_subclassification_option,
			char *transaction_begin_date_string,
			char *transaction_as_of_date,
			char *statement_logo_filename,
			char *statement_title,
			char *statement_subtitle,
			STATEMENT *preclose_statement,
			LIST *preclose_statement_prior_year_list,
			double preclose_debit_sum,
			double preclose_credit_sum,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double debit_sum,
			double credit_sum,
			pid_t process_id )
{
	TRIAL_BALANCE_PDF *trial_balance_pdf;

	if ( !application_name
	||   !process_name
	||   !document_root_directory
	||   !transaction_begin_date_string
	||   !transaction_as_of_date
	||   !statement_title
	||   !statement_subtitle
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

	trial_balance_pdf = trial_balance_pdf_calloc();

	trial_balance_pdf->landscape_boolean =
		trial_balance_pdf_landacape_boolean(
			list_length( statement_prior_year_list )
				/* statement_prior_year_list_length */ );

	if ( preclose_statement )
	{
		trial_balance_pdf->preclose_key =
			trial_balance_pdf_preclose_key(
				TRIAL_BALANCE_PDF_PRECLOSE_KEY );

		trial_balance_pdf->preclose_statement_link =
			statement_link_new(
				application_name,
				process_name,
				document_root_directory,
				transaction_begin_date_string,
				transaction_as_of_date,
				trial_balance_pdf->preclose_key,
				process_id );

		trial_balance_pdf->preclose_trial_balance_latex =
			trial_balance_latex_new(
				statement_subclassification_option,
				transaction_as_of_date,
				trial_balance_pdf->
					preclose_statement_link->
					tex_filename,
				trial_balance_pdf->
					preclose_statement_link->
					dvi_filename,
				trial_balance_pdf->
					preclose_statement_link->
					working_directory,
				trial_balance_pdf->landscape_boolean,
				statement_logo_filename,
				preclose_statement,
				preclose_statement_prior_year_list,
				preclose_debit_sum,
				preclose_credit_sum );
	}

	trial_balance_pdf->statement_link =
		statement_link_new(
			application_name,
			process_name,
			document_root_directory,
			transaction_begin_date_string,
			transaction_as_of_date,
			(char *)0 /* trial_balance_pdf_preclose_key */,
			process_id );

	trial_balance_pdf->trial_balance_latex =
		trial_balance_latex_new(
			statement_subclassification_option,
			transaction_as_of_date,
			trial_balance_pdf->
				statement_link->
				tex_filename,
			trial_balance_pdf->
				statement_link->
				dvi_filename,
			trial_balance_pdf->
				statement_link->
				working_directory,
			trial_balance_pdf->landscape_boolean,
			statement_logo_filename,
			statement,
			statement_prior_year_list,
			debit_sum,
			credit_sum );

	return trial_balance_pdf;
}

TRIAL_BALANCE_PDF *trial_balance_pdf_calloc( void )
{
	TRIAL_BALANCE_PDF *trial_balance_pdf;

	if ( ! ( trial_balance_pdf =
			calloc( 1, sizeof( TRIAL_BALANCE_PDF ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return trial_balance_pdf;
}

boolean trial_balance_pdf_landacape_boolean(
	int statement_prior_year_list_length )
{
	return (boolean)( statement_prior_year_list_length > 2 );
}

char *trial_balance_pdf_preclose_key(
			char *trial_balance_pdf_preclose_key )
{
	return trial_balance_pdf_preclose_key;
}

TRIAL_BALANCE_LATEX *trial_balance_latex_new(
			enum statement_subclassification_option
				statement_subclassification_option,
			char *transaction_as_of_date,
			char *tex_filename,
			char *dvi_filename,
			char *working_directory,
			boolean trial_balance_pdf_landscape_boolean,
			char *statement_logo_filename,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double debit_sum,
			double credit_sum )
{
	TRIAL_BALANCE_LATEX *trial_balance_latex;

	if ( !transaction_as_of_date
	||   !tex_filename
	||   !dvi_filename
	||   !working_directory
	||   !statement )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	trial_balance_latex = trial_balance_latex_calloc();

	if (	statement_subclassification_option ==
		subclassification_display )
	{
		trial_balance_latex->trial_balance_subclassification_latex =
			trial_balance_subclassification_latex_new(
				transaction_as_of_date,
				tex_filename,
				dvi_filename,
				working_directory,
				trial_balance_pdf_landscape_boolean,
				statement_logo_filename,
				statement,
				statement_prior_year_list,
				debit_sum,
				credit_sum );
	}
	else
	{
/*
		trial_balance_latex->trial_balance_account_latex =
			trial_balance_account_latex =
				trial_balance_account_latex_new(
					transaction_as_of_date,
					tex_filename,
					dvi_filename,
					working_directory,
					trial_balance_pdf_landscape_boolean,
					statement_logo_filename,
					statement,
					statement_prior_year_list,
					debit_sum,
					credit_sum );
*/
	}

	return trial_balance_latex;
}

TRIAL_BALANCE_LATEX *trial_balance_latex_calloc( void )
{
	TRIAL_BALANCE_LATEX *trial_balance_latex;

	if ( ! ( trial_balance_latex =
			calloc( 1, sizeof( TRIAL_BALANCE_LATEX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return trial_balance_latex;
}

char *trial_balance_latex_account_title(
			char *account_name,
			char *full_name,
			double debit_amount,
			double credit_amount,
			char *transaction_date_american,
			char *memo )
{
	char account_title[ 1024 ];
	char *ptr = account_title;
	char account_name_formatted[ 128 ];
	char full_name_formatted[ 128 ];
	char *transaction_amount_string;

	if ( !account_name
	||   !full_name
	||   !transaction_date_american )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	*ptr = '\0';

	format_initial_capital( account_name_formatted, account_name );
	ptr += sprintf( ptr, "\\textbf{%s}", account_name_formatted );

	ptr += sprintf( ptr, " (\\scriptsize{%s:", transaction_date_american );

	format_initial_capital( full_name_formatted, full_name );
	ptr += sprintf( ptr, " %s", full_name_formatted );

	if ( memo && *memo )
	{
		ptr += sprintf( ptr, "; %s", memo );
	}

	if ( debit_amount )
	{
		transaction_amount_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			timlib_place_commas_in_money(
				debit_amount );
	}
	else
	if ( credit_amount )
	{
		transaction_amount_string =
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			timlib_place_commas_in_money(
				credit_amount );
	}
	else
	{
		transaction_amount_string = "Unknown";
	}

	ptr += sprintf( ptr, " \\$%s})", transaction_amount_string );

	return strdup( account_title );
}

char *trial_balance_latex_transaction_count_string(
			int transaction_count )
{
	char count_string[ 16 ];

	sprintf(count_string,
		"%d",
		transaction_count );

	return strdup( count_string );
}

TRIAL_BALANCE_SUBCLASSIFICATION_LATEX *
	trial_balance_subclassification_latex_new(
			char *transaction_as_of_date,
			char *tex_filename,
			char *dvi_filename,
			char *working_directory,
			boolean trial_balance_pdf_landscape_boolean,
			char *statement_logo_filename,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double debit_sum,
			double credit_sum )
{
	TRIAL_BALANCE_SUBCLASSIFICATION_LATEX *
		trial_balance_subclassification_latex;

	if ( !transaction_as_of_date
	||   !tex_filename
	||   !dvi_filename
	||   !working_directory
	||   !statement )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	trial_balance_subclassification_latex =
		trial_balance_subclassification_latex_calloc();

	trial_balance_subclassification_latex->latex =
		latex_new(
			tex_filename,
			dvi_filename,
			working_directory,
			trial_balance_pdf_landscape_boolean,
			statement_logo_filename );

	trial_balance_subclassification_latex->table =
		trial_balance_subclassification_latex_table(
			transaction_as_of_date,
			statement,
			statement_prior_year_list,
			debit_sum,
			credit_sum );

	list_set(
		trial_balance_subclassification_latex->latex->table_list,
		trial_balance_subclassification_latex->table );

	return trial_balance_subclassification_latex;
}

TRIAL_BALANCE_SUBCLASSIFICATION_LATEX *
	trial_balance_subclassification_latex_calloc(
			void )
{
	TRIAL_BALANCE_SUBCLASSIFICATION_LATEX *
		trial_balance_subclassification_latex;

	if ( ! ( trial_balance_subclassification_latex =
		   calloc( 1,
			   sizeof( TRIAL_BALANCE_SUBCLASSIFICATION_LATEX ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}


	return trial_balance_subclassification_latex;
}

LATEX_TABLE *trial_balance_subclassification_latex_table(
			char *transaction_as_of_date,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double debit_sum,
			double credit_sum )
{
	LATEX_TABLE *latex_table;

	if ( !statement )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: statement is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_table =
		latex_table_new(
			statement->caption );

	latex_table->heading_list =
		trial_balance_subclassification_latex_heading_list(
			statement_prior_year_list );

	latex_table->row_list =
		trial_balance_subclassification_latex_row_list(
			transaction_as_of_date,
			statement->element_statement_list,
			statement_prior_year_list,
			debit_sum,
			credit_sum );

	return latex_table;
}

LIST *trial_balance_subclassification_latex_heading_list(
			LIST *statement_prior_year_list )
{
	LIST *heading_list;

	heading_list = list_new();

	list_set(
		heading_list,
		latex_table_heading_new(
			"Element",
			0 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Subclassification",
			0 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Account",
			0 /* right_justified_flag */,
			"6.0cm" /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Count",
			1 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Debit",
			1 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Credit",
			1 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	list_set(
		heading_list,
		latex_table_heading_new(
			"Percent",
			1 /* right_justified_flag */,
			(char *)0 /* paragraph_size */ ) );

	if ( list_length( statement_prior_year_list ) )
	{
		list_set_list(
			heading_list,
			latex_table_right_heading_list(
				statement_prior_year_heading_list(
					statement_prior_year_list ) ) );
	}

	return heading_list;
}

LIST *trial_balance_subclassification_latex_row_list(
			char *transaction_as_of_date,
			LIST *element_statement_list,
			LIST *statement_prior_year_list,
			double debit_sum,
			double credit_sum )
{
	LIST *row_list;
	ELEMENT *element;

	if ( !list_rewind( element_statement_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		element = list_get( element_statement_list );

		if ( !list_length( element->subclassification_statement_list ) )
			continue;

		list_set_list(
			row_list,
			trial_balance_subclassification_latex_element_row_list(
				transaction_as_of_date,
				element->element_name,
				element->accumulate_debit,
				element->subclassification_statement_list,
				statement_prior_year_list ) );

	} while( list_next( element_statement_list ) );

	list_set(
		row_list,
		trial_balance_subclassification_latex_sum_row(
			debit_sum,
			credit_sum ) );

	return row_list;
}

LATEX_ROW *trial_balance_subclassification_latex_sum_row(
			double debit_sum,
			double credit_sum )
{
	LATEX_ROW *latex_row;
	char *debit_total_string;
	char *credit_total_string;

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		"Total",
		0 /* not large_bold */ );

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
		(char *)0,
		0 /* not large_bold */ );

	debit_total_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		timlib_place_commas_in_dollars(
			debit_sum );

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( debit_total_string ),
		0 /* not large_bold */ );

	credit_total_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		timlib_place_commas_in_dollars(
			credit_sum );

	latex_column_data_set(
		latex_row->column_data_list,
		strdup( credit_total_string ),
		0 /* not large_bold */ );

	return latex_row;
}

LIST *trial_balance_subclassification_latex_element_row_list(
			char *transaction_as_of_date,
			char *element_name,
			boolean element_accumulate_debit,
			LIST *subclassification_statement_list,
			LIST *statement_prior_year_list )
{
	SUBCLASSIFICATION *subclassification;
	LIST *row_list;

	if ( !transaction_as_of_date )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: transaction_as_of_date is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( subclassification_statement_list ) )
		return (LIST *)0;

	row_list = list_new();

	do {
		subclassification =
			list_get(
				subclassification_statement_list );

		if ( !list_length( subclassification->account_statement_list ) )
			continue;

		list_set_list(
			row_list,
			trial_balance_subclassification_latex_account_row_list(
				transaction_as_of_date,
				element_name,
				element_accumulate_debit,
				subclassification->subclassification_name,
				subclassification->account_statement_list,
				statement_prior_year_list ) );

		element_name = (char *)0;

	} while( list_next( subclassification_statement_list ) );

	return row_list;
}

LIST *trial_balance_subclassification_latex_account_row_list(
			char *transaction_as_of_date,
			char *element_name,
			boolean element_accumulate_debit,
			char *subclassification_name,
			LIST *account_statement_list,
			LIST *statement_prior_year_list )
{
	LIST *row_list;
	ACCOUNT *account;
	TRIAL_BALANCE_ACCOUNT *trial_balance_account;

	if ( !transaction_as_of_date )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: transaction_as_of_date is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( account_statement_list ) ) return (LIST *)0;

	row_list = list_new();

	do {
		account = list_get( account_statement_list );

		trial_balance_account =
			trial_balance_account_new(
				transaction_as_of_date,
				element_accumulate_debit,
				account );

		list_set(
			row_list,
			trial_balance_subclassification_latex_account_row(
				element_name,
				subclassification_name,
				trial_balance_account,
				statement_prior_year_list ) );

		element_name = (char *)0;
		subclassification_name = (char *)0;

	} while ( list_next( account_statement_list ) );

	return row_list;
}

LATEX_ROW *trial_balance_subclassification_latex_account_row(
			char *element_name,
			char *subclassification_name,
			TRIAL_BALANCE_ACCOUNT *trial_balance_account,
			LIST *statement_prior_year_list )
{
	LATEX_ROW *latex_row;
	char *account_title;

	if ( !trial_balance_account )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: trial_balance_account is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !trial_balance_account->account )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: trial_balance_account->account is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	latex_row = latex_row_new();

	latex_column_data_set(
		latex_row->column_data_list,
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		trial_balance_subclassification_latex_account_row_title(
			element_name ) /* name */,
		0 /* not large_bold */ );

	latex_column_data_set(
		latex_row->column_data_list,
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		trial_balance_subclassification_latex_account_row_title(
			subclassification_name /* name */ ),
		0 /* not large_bold */ );

	account_title =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		trial_balance_latex_account_title(
			trial_balance_account->account->account_name,
			trial_balance_account->
				account->
				account_journal_latest->
				full_name,
			trial_balance_account->
				account->
				account_journal_latest->
				debit_amount,
			trial_balance_account->
				account->
				account_journal_latest->
				credit_amount,
			statement_date_american(
				trial_balance_account->
					account->
					account_journal_latest->
					transaction_date_time )
						/* transaction_date_american */,
			trial_balance_account->
				account->
				account_journal_latest->
				transaction->
				memo );

	latex_column_data_set(
		latex_row->column_data_list,
		account_title,
		0 /* not large_bold */ );

	latex_column_data_set(
		latex_row->column_data_list,
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		trial_balance_latex_transaction_count_string(
			trial_balance_account->account->transaction_count ),
		0 /* not large_bold */ );

	latex_column_data_set(
		latex_row->column_data_list,
		trial_balance_account->debit_string,
		trial_balance_account->
			within_days_between_boolean /* large_bold */ );

	latex_column_data_set(
		latex_row->column_data_list,
		trial_balance_account->credit_string,
		trial_balance_account->
			within_days_between_boolean /* large_bold */ );

	latex_column_data_set(
		latex_row->column_data_list,
		trial_balance_account->percent_string,
		0 /* not large_bold */ );

	if ( list_length( statement_prior_year_list ) )
	{
		latex_column_data_list_set(
			latex_row->column_data_list,
			statement_prior_year_latex_account_data_list(
				trial_balance_account->account->account_name,
				statement_prior_year_list ) );
	}

	return latex_row;
}

char *trial_balance_subclassification_latex_account_row_title( char *name )
{
	char buffer[ 128 ];

	if ( !name ) return (char *)0;

	format_initial_capital( buffer, name );

	return strdup( buffer );
}

char *trial_balance_account_percent_string(
			int percent_of_asset,
			int percent_of_revenue )
{
	char percent_string[ 16 ];

	if ( percent_of_asset )
	{
		sprintf(percent_string,
			"%d%c",
			percent_of_asset,
			'%' );
	}
	else
	if ( percent_of_revenue )
	{
		sprintf(percent_string,
			"%d%c",
			percent_of_revenue,
			'%' );
	}
	else
	{
		*percent_string = '\0';
	}

	return strdup( percent_string );
}
