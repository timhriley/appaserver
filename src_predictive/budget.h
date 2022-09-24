/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/budget.h			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef BUDGET_H
#define BUDGET_H

#include "list.h"
#include "boolean.h"
#include "statement.h"
#include "account.h"

typedef struct
{
	int days_so_far;
	int days_in_year;
	double year_ratio;
	double amount;
	ACCOUNT *account_element_list_seek;
	double difference;
} BUDGET_ANNUALIZED;

/* Usage */
/* ----- */
LIST *budget_annualized_list(
			char *budget_begin_date_string,
			char *budget_as_of_date,
			LIST *element_statement_list,
			STATEMENT_PRIOR_YEAR *statement_prior_year );

/* Process */
/* ------- */

/* Usage */
/* ----- */
BUDGET_ANNUALIZED *budget_annualized_new(
			char *budget_begin_date_string,
			char *budget_as_of_date,
			ACCOUNT *account,
			STATEMENT_PRIOR_YEAR *statement_prior_year );

/* Process */
/* ------- */
BUDGET_ANNUALIZED *budget_annualized_calloc(
			void );

int budget_annualized_days_so_far(
			char *budget_begin_date_string,
			char *budget_as_of_date );

int budget_annualized_days_in_year(
			char *budget_begin_date_string );

double budget_annualized_year_ratio(
			int budget_annualized_days_so_far,
			int budget_annualized_days_in_year );

double budget_annualized_amount(
			double account_latest_balance,
			double budget_annualized_year_ratio );

double budget_annualized_difference(
			double budget_annualized_amount,
			double prior_account_balance );

typedef struct
{
	char *process_name;
	char *document_root_directory;
	enum statement_output_medium;
	char *begin_date_string;
	char *as_of_date;
	LIST *element_name_list;
	boolean transaction_closing_entry_exists;
	char *transaction_date_time_closing;
	STATEMENT *statement;
	LIST *statement_prior_year_list;
	LIST *annualized_list;
} BUDGET;

/* Usage */
/* ----- */
LIST *budget_fetch(
			char *application_name,
			char *process_name,
			char *document_root_directory,
			char *output_medium_string );

/* Process */
/* ------- */
BUDGET *budget_calloc(	void );

char *budget_begin_date_string(
			int utc_offset );

char *budget_as_of_date(
			char *budget_begin_date_string );

LIST *budget_element_name_list(
			char *element_revenue,
			char *element_expense );

#endif
