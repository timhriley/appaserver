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

	trial_balance->postclose_statement =
		statement_fetch(
			trial_balance->filter_element_name_list,
			trial_balance->transaction_date_time_closing );

	if ( prior_year_count )
	{
		trial_balance->postclose_prior_year_list =
			statement_prior_year_list(
				trial_balance->
					filter_element_name_list,
				trial_balance->
					transaction_date_time_closing,
				prior_year_count,
				trial_balance->postclose_statement
					/* statement */ );
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

	trial_balance->postclose_debit_sum =
		element_list_debit_sum(
			trial_balance->
				postclose_statement->
				element_statement_list );

	trial_balance->postclose_credit_sum =
		element_list_credit_sum(
			trial_balance->
				postclose_statement->
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
				postclose_statement->
				element_statement_list );

	trial_balance->element_asset =
		element_seek(
			ELEMENT_ASSET,
			trial_balance->
				postclose_statement->
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
				postclose_statement->
				element_statement_list );

	trial_balance->element_revenue =
		element_seek(
			ELEMENT_REVENUE,
			trial_balance->
				postclose_statement->
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
					preclose_statement->
					element_statement_list );
		}
	}

	if (	trial_balance->statement_subclassification_option ==
		subclassification_omit )
	{
		element_list_account_statement_list_set(
			trial_balance->
				postclose_statement->
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

