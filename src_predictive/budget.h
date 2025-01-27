/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/budget.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef BUDGET_H
#define BUDGET_H

#include <unistd.h>
#include "list.h"
#include "boolean.h"
#include "date.h"
#include "html.h"
#include "latex.h"
#include "statement.h"
#include "account.h"

typedef struct
{
	HTML_TABLE *html_table;
} BUDGET_HTML;

/* Usage */
/* ----- */
BUDGET_HTML *budget_html_new(
		char *budget_year_percent_sub_title,
		LIST *budget_annualized_list,
		double budget_annualized_amount_net,
		double budget_annualized_budget_net,
		char *delta_cell_string );

/* Process */
/* ------- */
BUDGET_HTML *budget_html_calloc(
		void );

/* Usage */
/* ----- */
HTML_TABLE *budget_html_table(
		char *budget_year_percent_sub_title,
		LIST *budget_annualized_list,
		double budget_annualized_amount_net,
		double budget_annualized_budget_net,
		char *delta_cell_string );

/* Process */
/* ------- */
HTML_ROW *budget_html_sum_row(
		double budget_annualized_amount_net,
		double budget_annualized_budget_net,
		char *delta_cell_string );

/* Usage */
/* ----- */
LIST *budget_html_heading_list(
		void );

/* Usage */
/* ----- */
LIST *budget_html_row_list(
		LIST *budget_annualized_list );

/* Usage */
/* ----- */
HTML_ROW *budget_html_row(
		char *element_name,
		char *account_name,
		double account_amount,
		double annualized_amount,
		double budget,
		STATEMENT_DELTA *budget_statement_delta );

/* Usage */
/* ----- */
LIST *budget_html_heading_list(
		void );

typedef struct
{
	STATEMENT_LINK *statement_link;
	LATEX *latex;
	LATEX_TABLE *latex_table;
} BUDGET_LATEX;

/* Usage */
/* ----- */
BUDGET_LATEX *budget_latex_new(
		char *application_name,
		char *process_name,
		char *appaserver_parameter_data_directory,
		char *transaction_date_begin_date_string,
		char *statement_end_date_time_string,
		char *statement_logo_filename,
		char *budget_year_percent_sub_title,
		LIST *budget_annualized_list,
		double annualized_amount_net,
		double annualized_budget_net,
		char *delta_cell_string,
		pid_t process_id );

/* Process */
/* ------- */
BUDGET_LATEX *budget_latex_calloc(
		void );

/* Usage */
/* ----- */
LATEX_TABLE *budget_latex_table(
		char *budget_year_percent_sub_title,
		LIST *budget_annualized_list,
		double budget_annualized_amount_net,
		double budget_annualized_budget_net,
		char *delta_cell_string );

/* Process */
/* ------- */
LATEX_ROW *budget_latex_sum_row(
		double budget_annualized_amount_net,
		double budget_annualize_budget_net,
		char *delta_cell_string,
		LIST *latex_column_list );

/* Usage */
/* ----- */
LIST *budget_latex_column_list(
		void );

/* Usage */
/* ----- */
LIST *budget_latex_row_list(
		LIST *budget_annualized_list,
		LIST *budget_latex_column_list );

/* Usage */
/* ----- */
LATEX_ROW *budget_latex_row(
		char *element_name,
		char *account_name,
		double account_amount,
		double annualized_amount,
		double budget,
		STATEMENT_DELTA *budget_statement_delta,
		LIST *latex_column_list );

typedef struct
{
	char *element_name;
	ACCOUNT *account;
	double account_amount;
	double amount;
	ACCOUNT *prior_account;
	double budget;
	STATEMENT_DELTA *budget_statement_delta;
} BUDGET_ANNUALIZED;

/* Usage */
/* ----- */
LIST *budget_annualized_list(
			double budget_year_ratio,
			LIST *element_statement_list,
			STATEMENT_PRIOR_YEAR *statement_prior_year );

/* Usage */
/* ----- */
BUDGET_ANNUALIZED *budget_annualized_new(
		double budget_year_ratio,
		char *element_name,
		ACCOUNT *account,
		STATEMENT_PRIOR_YEAR *statement_prior_year );

/* Process */
/* ------- */
BUDGET_ANNUALIZED *budget_annualized_calloc(
		void );

double budget_annualized_account_amount(
		double account_latest_balance,
		int account_annual_amount );

double budget_annualized_amount(
		double budget_annualized_account_amount,
		double budget_year_ratio );

double budget_annualized_budget(
		double prior_account_balance );

/* Usage */
/* ----- */
double budget_annualized_amount_net(
		LIST *budget_annualized_list );

/* Usage */
/* ----- */
double budget_annualized_amount_sum(
		char *element_name,
		LIST *budget_annualized_list );

/* Usage */
/* ----- */
double budget_annualized_budget_net(
		LIST *budget_annualized_list );

/* Usage */
/* ----- */
double budget_annualized_budget_sum(
		char *element_name,
		LIST *budget_annualized_list );

typedef struct
{
	enum statement_output_medium statement_output_medium;
	DATE *as_of_date;
	char *transaction_date_begin_date_string;
	DATE *begin_date;
	int days_so_far;
	int days_in_year;
	double year_ratio;
	LIST *element_name_list;
	boolean transaction_date_close_boolean;
	char *transaction_date_close_date_time_string;
	STATEMENT *statement;
	char *end_date_time_string;
	LIST *statement_prior_year_list;
	LIST *annualized_list;
	double annualized_amount_net;
	double annualized_budget_net;
	STATEMENT_DELTA *budget_statement_delta;
	char *year_percent_sub_title;
	BUDGET_LATEX *budget_latex;
	BUDGET_HTML *budget_html;
} BUDGET;

/* Usage */
/* ----- */
BUDGET *budget_fetch(
		char *application_name,
		char *process_name,
		char *document_root_directory,
		char *as_of_date_string,
		char *output_medium_string );

/* Process */
/* ------- */
BUDGET *budget_calloc(
		void );

DATE *budget_as_of_date(
		char *as_of_date_string,
		char *date_now_yyyy_mm_dd );

DATE *budget_begin_date(
		char *transaction_date_begin_date_string );

int budget_days_so_far(
		DATE *budget_begin_date,
		DATE *budget_as_of_date );

int budget_days_in_year(
		DATE *budget_begin_date );

double budget_year_ratio(
		int budget_days_so_far,
		int budget_days_in_year );

LIST *budget_element_name_list(
		char *element_revenue,
		char *element_expense );

/* Returns heap memory */
/* ------------------- */
char *budget_end_date_time_string(
		char *transaction_date_begin_date_string );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *budget_display(
		double budget );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *budget_year_percent_sub_title(
		char *input_subt_itle,
		double budget_year_ratio );

#endif
