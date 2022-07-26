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
			char *role_name,
			char *process_name,
			char *as_of_date,
			int prior_year_count,
			char *subclassifiction_option_string,
			char *output_medium_string )
{
	TRIAL_BALANCE *trial_balance;

	if ( !application_name
	||   !session_key
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

	trial_balance->statement_logo_filename =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		statement_logo_filename(
			application_name,
			STATEMENT_LOGO_FILENAME_KEY );

	trial_balance->statement_title =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		statement_title(
			application_name,
			trial_balance->statement_logo_filename,
			process_name );

	trial_balance->statement_subtitle =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		statement_subtitle(
			trial_balance->transaction_begin_date_string,
			trial_balance->transaction_as_of_date );

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
				trial_balance->filter_element_name_list,
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
			trial_balance->filter_element_name_list,
			trial_balance->transaction_date_time_closing );

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
	||   !account->journal_latest )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	trial_balance_account = trial_balance_account_calloc();

	trial_balance_account->balance = account->journal_latest->balance;

	if ( trial_balance_account->balance < 0.0 )
	{
		trial_balance_account->balance =
			float_abs( trial_balance_account->balance );

		element_accumulate_debit = 1 - element_accumulate_debit;
	}

	if ( element_accumulate_debit )
	{
		trial_balance_account->debit_string =
			timlib_place_commas_in_dollars(
				trial_balance_account->balance );
	}
	else
	{
		trial_balance_account->credit_string =
			timlib_place_commas_in_dollars(
				trial_balance_account->balance );
	}

	trial_balance_account->date_days_between =
		/* ---------------------- */
		/* Ignores trailing time. */
		/* ---------------------- */
		date_days_between(
			account->journal_latest->transaction_date_time,
			transaction_as_of_date );

	trial_balance_account->within_days_between_boolean =
		( trial_balance_account->date_days_between <=
		  TRIAL_BALANCE_DAYS_FOR_EMPHASIS );

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
			char *login_name,
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

	if ( !application_name,
	||   !login_name,
	||   !process_name,
	||   !document_root_directory,
	||   !transaction_begin_date_string,
	||   !transaction_as_of_date,
	||   !statement_title,
	||   !statement_subtitle,
	||   !statement,
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

	trial_balance_pdf->balance_pdf_landacape_boolean =
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
					latex_filename,
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
				trial_balance_pdf->
				statement_link->latex_filename,
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
			char *statement_link->latex_filename,
			char *statement_link->dvi_filename,
			char *statement_link->working_directory,
			boolean trial_balance_pdf_landscape_boolean,
			char *statement_logo_filename,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double debit_sum,
			double credit_sum )
{
	TRIAL_BALANCE_LATEX *trial_balance_latex;

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

