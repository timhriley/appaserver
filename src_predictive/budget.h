/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/budget.h			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef BUDGET_H
#define BUDGET_H

#include <unistd.h>
#include "list.h"
#include "boolean.h"
#include "date.h"
#include "html_table.h"
#include "latex.h"
#include "statement.h"
#include "account.h"

typedef struct
{
	LATEX *latex;
} BUDGET_LATEX;

/* Usage */
/* ----- */
BUDGET_LATEX *budget_latex_new(
			char *tex_filename,
			char *dvi_filename,
			char *working_directory,
			char *statement_logo_filename,
			char *statement_caption,
			LIST *budget_annualized_list );

/* Process */
/* ------- */
BUDGET_LATEX *budget_latex_calloc(
			void );

/* Usage */
/* ----- */
LATEX_TABLE *budget_latex_table(
			char *statement_caption,
			LIST *budget_annualized_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *budget_latex_heading_list(
			void );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *budget_latex_row_list(
			LIST *budget_annualized_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LATEX_ROW *budget_latex_row(
			char *account_name,
			double account_latest_balance,
			ACCOUNT *prior_account,
			double difference );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *budget_latex_amount_display(
			ACCOUNT *prior_account );

/* Process */
/* ------- */

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *budget_latex_difference_display(
			double difference );

/* Process */
/* ------- */

typedef struct
{
	STATEMENT_LINK *statement_link;
	BUDGET_LATEX *budget_latex;
} BUDGET_PDF;

/* Usage */
/* ----- */
BUDGET_PDF *budget_pdf_new(
			char *application_name,
			char *process_name,
			char *document_root_directory,
			char *transaction_begin_date_string,
			char *transaction_as_of_date,
			char *statement_logo_filename,
			char *statement_caption,
			LIST *budget_annualized_list,
			pid_t process_id );

/* Process */
/* ------- */
BUDGET_PDF *budget_pdf_calloc(
			void );

typedef struct
{
} BUDGET_HTML;

/* Usage */
/* ----- */

/* Process */
/* ------- */

typedef struct
{
	ACCOUNT *account;
	int days_so_far;
	int days_in_year;
	double year_ratio;
	double amount;
	ACCOUNT *prior_account;
	double difference;
} BUDGET_ANNUALIZED;

/* Usage */
/* ----- */
LIST *budget_annualized_list(
			DATE *budget_begin_date,
			DATE *budget_as_of_date,
			LIST *element_statement_list,
			STATEMENT_PRIOR_YEAR *statement_prior_year );

/* Process */
/* ------- */

/* Usage */
/* ----- */
BUDGET_ANNUALIZED *budget_annualized_new(
			DATE *budget_begin_date,
			DATE *budget_as_of_date,
			ACCOUNT *account,
			STATEMENT_PRIOR_YEAR *statement_prior_year );

/* Process */
/* ------- */
BUDGET_ANNUALIZED *budget_annualized_calloc(
			void );

int budget_annualized_days_so_far(
			DATE *budget_begin_date,
			DATE *budget_as_of_date );

int budget_annualized_days_in_year(
			DATE *budget_begin_date );

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
	enum statement_output_medium statement_output_medium;
	char *transaction_as_of_date;
	DATE *as_of_date;
	char *transaction_begin_date_string;
	DATE *begin_date;
	LIST *element_name_list;
	boolean transaction_closing_entry_exists;
	char *transaction_date_time_closing;
	STATEMENT *statement;
	char *end_date_time_string;
	LIST *statement_prior_year_list;
	LIST *annualized_list;
	BUDGET_PDF *budget_pdf;
	BUDGET_HTML *budget_html;
} BUDGET;

/* Usage */
/* ----- */
BUDGET *budget_fetch(	char *application_name,
			char *process_name,
			char *document_root_directory,
			char *as_of_date_string,
			char *output_medium_string );

/* Process */
/* ------- */
BUDGET *budget_calloc(	void );

DATE *budget_as_of_date(
			char *transaction_as_of_date );

DATE *budget_begin_date(
			char *transaction_begin_date_string );

LIST *budget_element_name_list(
			char *element_revenue,
			char *element_expense );

/* Returns heap memory */
/* ------------------- */
char *budget_end_date_time_string(
			char *transaction_begin_date_string );

#endif
