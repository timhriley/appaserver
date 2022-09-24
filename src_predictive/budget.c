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
#include "appaserver_library.h"
#include "html_table.h"
#include "latex.h"
#include "budget.h"

LIST *budget_annualized_list(
			char *budget_begin_date_string,
			char *budget_as_of_date,
			LIST *element_statement_list,
			STATEMENT_PRIOR_YEAR *statement_prior_year )
{
}

BUDGET_ANNUALIZED *budget_annualized_new(
			char *budget_begin_date_string,
			char *budget_as_of_date,
			ACCOUNT *account,
			STATEMENT_PRIOR_YEAR *statement_prior_year )
{
}

BUDGET_ANNUALIZED *budget_annualized_calloc( void )
{
}

int budget_annualized_days_so_far(
			char *budget_begin_date_string,
			char *budget_as_of_date )
{
}

int budget_annualized_days_in_year(
			char *budget_begin_date_string )
{
}

double budget_annualized_year_ratio(
			int budget_annualized_days_so_far,
			int budget_annualized_days_in_year )
{
}

double budget_annualized_amount(
			double account_latest_balance,
			double budget_annualized_year_ratio )
{
}

double budget_annualized_difference(
			double budget_annualized_amount,
			double prior_account_balance )
{
}

LIST *budget_fetch(
			char *application_name,
			char *process_name,
			char *document_root_directory,
			char *output_medium_string )
{
}

BUDGET *budget_calloc( void )
{
}

char *budget_begin_date_string( int utc_offset )
{
}

char *budget_as_of_date( char *budget_begin_date_string )
{
}

LIST *budget_element_name_list(
			char *element_revenue,
			char *element_expense )
{
	LIST *name_list = list_new();

	list_set( name_list, element_revenue );
	list_set( name_list, element_expense );

	return name_list
}

