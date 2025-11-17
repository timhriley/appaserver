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
#include "appaserver_link.h"
#include "statement.h"
#include "account.h"

#define BUDGET_REGRESSION_DATE_LABEL	"transaction_date"
#define BUDGET_REGRESSION_BALANCE_LABEL	"balance"
#define BUDGET_PLOT_RSCRIPT_FILENAME	"budget_plot.R"
#define BUDGET_PLOT_DEFAULT_PDF		"Rplots.pdf"
#define BUDGET_LINK_PROMPT		"Press to view plot for "
#define BUDGET_CONFIDENCE_THRESHOLD	85

typedef struct
{
	/* ---- */
	/* Stub */
	/* ---- */
} BUDGET_PLOT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
BUDGET_PLOT *budget_plot_new(
		const char *budget_regression_date_label,
		const char *budget_regression_balance_label,
		char *session_key,
		char *account_name,
		char *budget_forecast_date_string,
		char *budget_forecast_julian_string,
		char *appaserver_link_working_directory,
		char *pdf_filename,
		char *julian_specification /* will append */,
		int forecast_integer,
		int confidence_integer );

/* Process */
/* ------- */
BUDGET_PLOT *budget_plot_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *budget_plot_remove_system_string(
		const char *budget_plot_rscript_filename );

/* Usage */
/* ----- */
void budget_plot_write_Rscript(
		const char *budget_regression_date_label,
		const char *budget_regression_balance_label,
		const char *budget_plot_rscript_filename,
		char *account_name,
		char *budget_forecast_date_string,
		char *julian_specification,
		int confidence_integer );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *budget_plot_system_string(
		const char *budget_plot_rscript_filename );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *budget_plot_rename_system_string(
		const char *budget_plot_default_pdf,
		char *pdf_filename );

typedef struct
{
	char *text_specification;
	char *rm_text_system_string;
	char *julian_specification;
	char *rm_julian_system_string;
	char *regression_specification;
	char *rm_regression_system_string;
} BUDGET_FILE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
BUDGET_FILE *budget_file_new(
		char *session_key,
		char *appaserver_link_working_directory );

/* Process */
/* ------- */
BUDGET_FILE *budget_file_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *budget_file_specification(
		const char *extension,
		char *session_key,
		char *appaserver_link_working_directory );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *budget_file_rm_system_string(
		char *budget_file_specification );

typedef struct
{
	APPASERVER_LINK *appaserver_link;
	char *pdf_filename;
	char *pdf_anchor_html;
} BUDGET_LINK;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
BUDGET_LINK *budget_link_new(
		char *application_name,
		char *session_key,
		char *process_name,
		char *appaserver_parameter_data_directory,
		char *account_name );

/* Process */
/* ------- */
BUDGET_LINK *budget_link_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *budget_link_prompt_string(
		const char *budget_link_prompt,
		char *account_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *budget_link_begin_date_string(
		void );

typedef struct
{
	char *account_name;
	BUDGET_LINK *budget_link;
	BUDGET_FILE *budget_file;
	char *sql;
	char *text_system_string;
	int row_count;
	char *julian_system_string;
	char *forecast_system_string;
	char *string_fetch;
	int forecast_integer;
	int confidence_integer;
	BUDGET_PLOT *budget_plot;
} BUDGET_REGRESSION;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
BUDGET_REGRESSION *budget_regression_fetch(
		char *application_name,
		char *session_key,
		char *process_name,
		char *appaserver_parameter_data_directory,
		char *transaction_date_begin_date_string,
		char *account_name,
		char *budget_forecast_date_string,
		char *budget_forecast_julian_string,
		char *appaserver_link_working_directory );

/* Process */
/* ------- */
BUDGET_REGRESSION *budget_regression_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *budget_regression_text_system_string(
		char *text_specification,
		char *budget_regression_sql );

int budget_regression_row_count(
		char *text_specification );

int budget_regression_forecast_integer(
		char *string_fetch );

int budget_regression_confidence_integer(
		char *string_fetch );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *budget_regression_sql(
		const char *journal_table,
		char *transaction_date_begin_date_string,
		char *account_name );

/* Returns static memory */
/* --------------------- */
char *budget_regression_where(
		char *transaction_date_begin_date_string,
		char *account_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *budget_regression_julian_system_string(
		const char *budget_regression_date_label,
		const char *budget_regression_balance_label,
		char *text_specification,
		char *julian_specification );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *budget_regression_forecast_system_string(
		const char *budget_regression_date_label,
		const char *budget_regression_balance_label,
		char *budget_forecast_julian_string,
		char *julian_specification,
		char *regression_specification );

typedef struct
{
	HTML_TABLE *html_table;
} BUDGET_HTML;

/* Usage */
/* ----- */
BUDGET_HTML *budget_html_new(
		char *budget_sub_title,
		LIST *budget_annualized_list,
		double budget_amount_net,
		int budget_annualized_amount_net,
		int budget_annualized_budget_net,
		char *delta_cell_string );

/* Process */
/* ------- */
BUDGET_HTML *budget_html_calloc(
		void );

/* Usage */
/* ----- */
HTML_TABLE *budget_html_table(
		char *budget_sub_title,
		LIST *budget_annualized_list,
		double budget_amount_net,
		int budget_annualized_amount_net,
		int budget_annualized_budget_net,
		char *delta_cell_string );

/* Process */
/* ------- */
HTML_ROW *budget_html_sum_row(
		double budget_amount_net,
		int budget_annualized_amount_net,
		int budget_annualized_budget_net,
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
		int row_count,
		int confidence_integer,
		int annualized_amount_integer,
		int annualized_budget_integer,
		STATEMENT_DELTA *statement_delta );

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
		char *statement_end_date_time,
		char *statement_logo_filename,
		char *budget_sub_title,
		LIST *budget_annualized_list,
		double budget_amount_net,
		int annualized_amount_net,
		int annualized_budget_net,
		char *delta_cell_string,
		pid_t process_id );

/* Process */
/* ------- */
BUDGET_LATEX *budget_latex_calloc(
		void );

/* Usage */
/* ----- */
LATEX_TABLE *budget_latex_table(
		char *budget_sub_title,
		LIST *budget_annualized_list,
		double budget_amount_net,
		int budget_annualized_amount_net,
		int budget_annualized_budget_net,
		char *delta_cell_string );

/* Process */
/* ------- */
LATEX_ROW *budget_latex_sum_row(
		double budget_amount_net,
		int budget_annualized_amount_net,
		int budget_annualize_budget_net,
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
		int confidence_integer,
		int annualized_amount_integer,
		int annualized_budget_integer,
		STATEMENT_DELTA *statement_delta,
		LIST *latex_column_list );

typedef struct
{
	char *element_name;
	ACCOUNT *account;
	BUDGET_REGRESSION *budget_regression;
	double journal_balance;
	int amount_integer;
	int budget_integer;
	STATEMENT_DELTA *statement_delta;
} BUDGET_ANNUALIZED;

/* Usage */
/* ----- */
LIST *budget_annualized_list(
		char *application_name,
		char *session_key,
		char *process_name,
		char *appaserver_parameter_data_directory,
		char *transaction_date_begin_date_string,
		LIST *element_statement_list,
		STATEMENT_PRIOR_YEAR *statement_prior_year,
		char *budget_forecast_date_string,
		char *budget_forecast_julian_string,
		char *appaserver_link_working_directory );

/* Usage */
/* ----- */
BUDGET_ANNUALIZED *budget_annualized_new(
		char *application_name,
		char *session_key,
		char *process_name,
		char *appaserver_parameter_data_directory,
		char *transaction_date_begin_date_string,
		char *element_name,
		ACCOUNT *account,
		STATEMENT_PRIOR_YEAR *statement_prior_year,
		char *budget_forecast_date_string,
		char *budget_forecast_julian_string,
		char *appaserver_link_working_directory );

/* Process */
/* ------- */
BUDGET_ANNUALIZED *budget_annualized_calloc(
		void );

/* Returns account_journal_latest->balance or 0.0 */
/* ---------------------------------------------- */
double budget_annualized_journal_balance(
		ACCOUNT_JOURNAL *account_journal );

/* Usage */
/* ----- */
int budget_annualized_amount_net(
		LIST *budget_annualized_list );

/* Usage */
/* ----- */
int budget_annualized_amount_sum(
		const char *element_name,
		LIST *budget_annualized_list );

/* Usage */
/* ----- */
int budget_annualized_budget_net(
		LIST *budget_annualized_list );

/* Usage */
/* ----- */
int budget_annualized_amount_integer(
		const int budget_confidence_threshold,
		int budget_regression_confidence_integer,
		int budget_regression_forecast_integer,
		double budget_annualized_journal_balance );

/* Usage */
/* ----- */
int budget_annualized_budget_integer(
		STATEMENT_PRIOR_YEAR *statement_prior_year,
		char *account_name,
		int account_annual_budget,
		int budget_annualized_amount_integer );

/* Usage */
/* ----- */
int budget_annualized_budget_sum(
		const char *element_name,
		LIST *budget_annualized_list );

/* Usage */
/* ----- */
int budget_annualized_compare_function(
		BUDGET_ANNUALIZED *from_list_budget_annualized,
		BUDGET_ANNUALIZED *compare_budget_annualized );

typedef struct
{
	enum statement_output_medium statement_output_medium;
	char *transaction_date_begin_date_string;
	DATE *begin_date;
	int days_so_far;
	int days_in_year;
	double year_ratio;
	LIST *element_name_list;
	boolean transaction_date_close_boolean;
	char *transaction_date_close_date_time;
	STATEMENT *statement;
	char *end_date_time;
	LIST *statement_prior_year_list;
	DATE *forecast_date;
	char *forecast_date_string;
	char *forecast_julian_string;
	char *appaserver_link_working_directory;
	LIST *budget_annualized_list;
	double amount_net;
	int annualized_amount_net;
	int annualized_budget_net;
	STATEMENT_DELTA *statement_delta;
	char *sub_title;
	BUDGET_LATEX *budget_latex;
	BUDGET_HTML *budget_html;
} BUDGET;

/* Usage */
/* ----- */
BUDGET *budget_fetch(
		char *application_name,
		char *session_key,
		char *process_name,
		char *output_medium_string,
		char *appaserver_parameter_data_directory );

/* Process */
/* ------- */
BUDGET *budget_calloc(
		void );

DATE *budget_begin_date(
		char *transaction_date_begin_date_string );

int budget_days_so_far(
		DATE *date_now_new,
		DATE *budget_begin_date );

int budget_days_in_year(
		DATE *budget_begin_date );

double budget_year_ratio(
		int budget_days_so_far,
		int budget_days_in_year );

LIST *budget_element_name_list(
		const char *element_revenue,
		const char *element_expense,
		const char *element_expenditure,
		const char *element_gain,
		const char *element_loss );

/* Returns heap memory */
/* ------------------- */
char *budget_end_date_time(
		char *transaction_date_begin_date_string );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *budget_display(
		int budget_integer );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *budget_sub_title(
		const int budget_confidence_threshold,
		char *input_subt_itle );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
DATE *budget_forecast_date(
		DATE *budget_begin_date,
		int budget_days_in_year );

/* Usage */
/* ----- */

/* Returns heap memory or "" */
/* ------------------------- */
char *budget_forecast_date_string(
		DATE *budget_forecast_date );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *budget_forecast_julian_string(
		char *budget_forecast_date_string );

/* Usage */
/* ----- */
double budget_amount_net(
		LIST *budget_annualized_list );

/* Usage */
/* ----- */
double budget_amount_sum(
		const char *element_name,
		LIST *budget_annualized_list );

#endif
