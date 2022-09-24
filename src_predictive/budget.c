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

	budget_annualized->amount =
		budget_annualized_amount(
			account->account_journal_latest->balance
				/* account_latest_balance */,
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

double budget_annualized_amount(
			double account_latest_balance,
			double budget_annualized_year_ratio )
{
	if ( double_virtually_same( budget_annualized_year_ratio, 0.0 ) )
	{
		return 0.0;
	}
	else
	{
		return
		account_latest_balance /
		budget_annualized_year_ratio;
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
			char *output_medium_string )
{
	BUDGET *budget;

	if ( !application_name
	||   !process_name
	||   !document_root_directory
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

	budget->document_root_directory = document_root_directory;

	budget->statement_output_medium =
		statement_resolve_output_medium(
			output_medium_string );

	budget->as_of_date =
		budget_as_of_date(
			date_utc_offset() );

	budget->as_of_date_string =
		strdup(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			date_yyyy_mm_dd(
				budget->as_of_date ) );

	budget->begin_date =
		budget_begin_date(
			budget->as_of_date );

	budget->element_name_list =
		budget_element_name_list(
			ELEMENT_REVENUE,
			ELEMENT_EXPENSE );

	budget->transaction_closing_entry_exists =
		transaction_closing_entry_exists(
			TRANSACTION_TABLE,
			TRANSACTION_CLOSE_TIME,
			budget->as_of_date_string );

	budget->transaction_date_time_closing =
		transaction_date_time_closing(
			TRANSACTION_PRECLOSE_TIME,
			TRANSACTION_CLOSE_TIME,
			budget->as_of_date_string,
			budget->transaction_closing_entry_exists
				/* preclose_time_boolean */ );

	budget->statement =
		statement_fetch(
			application_name,
			process_name,
			budget->element_name_list,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			date_yyyy_mm_dd( budget->begin_date )
				/* transaction_begin_date_string */,
			budget->as_of_date_string
				/* transaction_as_of_date */,
			budget->transaction_date_time_closing,
			0 /* not fetch_transaction */ );

	element_list_account_statement_list_set(
		budget->statement->element_statement_list );

	budget->statement_prior_year_list =
		statement_prior_year_list(
				budget->element_name_list,
				budget->transaction_date_time_closing,
				1 /* prior_year_count */,
				budget->statement );

	budget->annualized_list =
		budget_annualized_list(
			budget->begin_date,
			budget->as_of_date,
			budget->statement->element_statement_list,
			list_first( budget->statement_prior_year_list )
				/* statement_prior_year */ );

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

DATE *budget_as_of_date( int date_utc_offset )
{
	return date_now_new( date_utc_offset );
}

DATE *budget_begin_date( DATE *budget_as_of_date )
{
	char begin_date_string[ 11 ];

	if ( !budget_as_of_date )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: budget_as_of_date is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(begin_date_string,
		"%d-01-01",
		date_year( budget_as_of_date ) );

	return date_yyyy_mm_dd_new( begin_date_string );
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

