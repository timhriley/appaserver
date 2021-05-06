/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/statement.h			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef STATEMENT_H
#define STATEMENT_H

#include "list.h"
#include "boolean.h"
#include "element.h"

/* Constants */
/* --------- */

/* Enumerated types */
/* ---------------- */
enum subclassification_option	{	subclassification_aggregate,
					subclassification_display,
					subclassification_omit };

enum fund_aggregation		{	single_fund,
					sequential,
					consolidated,
					no_fund };

enum output_medium 		{	output_table,
					output_PDF,
					output_stdout };

/* Structures */
/* ---------- */
typedef struct
{
	/* Attributes */
	/* ---------- */
	char *application_name;
	char *session;
	char *login_name;
	char *role_name;
	LIST *filter_element_name_list;
	char *as_of_date;
	int years_ago;
	char *fund_name;
	enum subclassification_option subclassification_option;

	/* Process */
	/* ------- */
	char *prior_date_string;
	char *begin_date_string;
	LIST *prior_year_element_list;
	double revenue_total;
	double expense_total;
	double gain_total;
	double loss_total;
	double statement_prior_year_net_income;
	int delta_prior;

} STATEMENT_PRIOR_YEAR;

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *application_name;
	char *session;
	char *login_name;
	char *role_name;
	LIST *filter_element_name_list;
	char *begin_date_string;
	char *as_of_date;
	int prior_year_count;
	char *fund_name;
	enum subclassification_option subclassification_option;

	/* Process */
	/* ------- */
	char *transaction_date_time;
	LIST *preclose_element_list;
	LIST *postclose_element_list;
	LIST *prior_year_list;
	double preclose_debit_total;
	double preclose_credit_total;
	double postclose_debit_total;
	double postclose_credit_total;
	double revenue_total;
	double expense_total;
	double gain_total;
	double loss_total;
	double net_income;
	int net_income_percent;
	char *statement_fund_caption;

} STATEMENT_FUND;

typedef struct
{
	/* Attributes */
	/* ---------- */
	char *application_name;
	char *session;
	char *login_name;
	char *role_name;
	char *process_name;
	boolean exists_logo_filename;
	LIST *filter_element_name_list;
	char *as_of_date;
	int prior_year_count;
	LIST *fund_name_list;
	char *subclassification_option_string;
	char *fund_aggregation_string;
	char *output_medium_string;

	/* Process */
	/* ------- */
	enum subclassification_option statement_subclassification_option;
	enum fund_aggregation statement_fund_aggregation;
	enum output_medium statement_output_medium;
	char *begin_date_string;
	char *title;
	char *subtitle;
	LIST *statement_fund_list;

} STATEMENT;

/* Operations */
/* ---------- */
STATEMENT *statement_new(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *process_name,
			boolean exists_logo_filename,
			LIST *filter_element_name_list,
			char *as_of_date,
			int prior_year_count,
			LIST *fund_name_list,
			char *subclassification_option_string,
			char *fund_aggregation_string,
			char *output_medium_string );

LIST *statement_fund_element_list(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			LIST *filter_element_name_list,
			char *begin_date_string,
			char *transaction_date_time,
			char *fund_name,
			enum subclassification_option );

enum subclassification_option statement_subclassification_option(
			char *subclassification_option_string );

enum fund_aggregation statement_fund_aggregation(
			LIST *fund_name_list,
			char *fund_aggregation_string );

enum output_medium statement_output_medium(
			char *output_medium_string );

STATEMENT *statement_steady_state(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *process_name,
			boolean exists_logo_filename,
			LIST *filter_element_name_list,
			char *as_of_date,
			int prior_year_count,
			LIST *fund_name_list,
			char *subclassification_option_string,
			char *fund_aggregation_string,
			char *output_medium_string,
			STATEMENT *statement );

STATEMENT_PRIOR_YEAR *statement_prior_year_new(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			LIST *filter_element_name_list,
			char *as_of_date,
			int years_ago,
			char *fund_name,
			enum subclassification_option );

STATEMENT_FUND *statement_fund_fetch(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			LIST *filter_element_name_list,
			char *begin_date_string,
			char *as_of_date,
			int prior_year_count,
			char *fund_name,
			enum subclassification_option );

STATEMENT_FUND *statement_fund_new(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			LIST *filter_element_name_list,
			char *begin_date_string,
			char *as_of_date,
			int prior_year_count,
			char *fund_name,
			enum subclassification_option );

LIST *statement_fund_list(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			LIST *filter_element_name_list,
			char *begin_date_string,
			char *as_of_date,
			int prior_year_count,
			LIST *fund_name_list,
			enum subclassification_option,
			enum fund_aggregation );

char *statement_begin_date_string(
			char *as_of_date );

boolean statement_fund_attribute_exists(
			void );

LIST *statement_fund_name_list(
			void );

/* Returns static memory */
/* --------------------- */
char *statement_title(
			char *application_name,
			boolean exists_logo_filename,
			char *process_name );

/* Returns static memory */
/* --------------------- */
char *statement_subtitle(
			char *begin_date_string,
			char *as_of_date,
			enum fund_aggregation fund_aggregation );

void statement_fund_list_steady_state(
			LIST *statement_fund_list,
			char *title,
			char *subtitle,
			boolean exists_logo_filename );

STATEMENT_FUND *statement_fund_steady_state(
			LIST *preclose_element_list,
			LIST *postclose_element_list,
			int prior_year_count,
			char *title,
			char *subtitle,
			boolean exists_logo_filename,
			STATEMENT_FUND *statement_fund );

boolean statement_fund_exists_postclose(
			LIST *statement_fund_list );

LIST *statement_prior_year_heading_list(
			LIST *prior_year_list );

LIST *statement_account_delta_list(
			char *account_name,
			LIST *prior_year_list );

LIST *statement_html_account_delta_list(
			char *account_name,
			LIST *prior_year_list );

LIST *statement_PDF_account_delta_list(
			char *account_name,
			LIST *prior_year_list );

LIST *statement_subclassification_delta_list(
			char *subclassification_name,
			LIST *prior_year_list );

LIST *statement_html_subclassification_delta_list(
			char *subclassification_name,
			LIST *prior_year_list );

LIST *statement_PDF_subclassification_delta_list(
			char *subclassification_name,
			LIST *prior_year_list );

LIST *statement_html_element_delta_list(
			char *element_name,
			LIST *prior_year_list );

LIST *statement_PDF_element_delta_list(
			char *element_name,
			LIST *prior_year_list );

LIST *statement_fund_steady_state_prior_year_list(
			int prior_year_count,
			LIST *preclose_element_list,
			STATEMENT_FUND *statement_fund );

boolean statement_fund_exists_prior_year(
			LIST *statement_fund_list );

STATEMENT_FUND *statement_fund_seek(
			char *fund_name,
			LIST *statement_fund_list );

void statement_fund_list_net_income_set(
			LIST *statement_fund_list );

void statement_fund_list_equity_set(
			LIST *statement_fund_list,
			boolean is_financial_position );

double statement_fund_net_income(
			double revenue_total,
			double expense_total,
			double gain_total,
			double loss_total );

int statement_fund_net_income_percent(
			double net_income,
			double revenue_total );

LIST *statement_html_heading_list(
			LIST *prior_year_list,
			boolean include_account,
			boolean include_subclassification );

/* Returns program memory */
/* ---------------------- */
char *statement_html_net_income_label(
			boolean is_statement_of_activities );

/* Returns program memory */
/* ---------------------- */
char *statement_PDF_net_income_label(
			boolean is_statement_of_activities );

/* Returns static memory */
/* --------------------- */
char *statement_html_subclassification_label(
			char *subclassification_name );

void statement_prior_year_list_net_income_set(
			LIST *prior_year_list,
			double statement_fund_net_income );

void statement_prior_year_net_income_set(
			STATEMENT_PRIOR_YEAR *statement_prior_year,
			double statement_fund_net_income );

int statement_delta_prior(
			double element_prior_total,
			double element_total );

LIST *statement_html_net_income_delta_list(
			LIST *prior_year_list );

LIST *statement_PDF_net_income_delta_list(
			LIST *prior_year_list );

LIST *statement_PDF_heading_list(
			LIST *prior_year_list,
			boolean include_account,
			boolean include_subclassification );

char *statement_fund_caption(
			char *title,
			char *subtitle,
			char *fund_name,
			boolean exists_logo_filename );

void statement_html_display_element_list(
			HTML_TABLE *html_table,
			LIST *preclose_element_list,
			LIST *prior_year_list,
			boolean is_percent_of_revenue );

void statement_html_display_element(
			HTML_TABLE *html_table,
			ELEMENT *element,
			LIST *prior_year_list,
			boolean is_percent_of_revenue );

void statement_html_omit_element_list(
			HTML_TABLE *html_table,
			LIST *preclose_element_list,
			LIST *prior_year_list,
			boolean is_percent_of_revenue );

void statement_html_omit_element(
			HTML_TABLE *html_table,
			ELEMENT *element,
			LIST *prior_year_list,
			boolean is_percent_of_revenue );

void statement_html_aggregate_element_list(
			HTML_TABLE *html_table,
			LIST *preclose_element_list,
			LIST *prior_year_list,
			boolean is_percent_of_revenue );

void statement_html_aggregate_element(
			HTML_TABLE *html_table,
			ELEMENT *element,
			LIST *prior_year_list,
			boolean is_percent_of_revenue );

SUBCLASSIFICATION *statement_net_income_subclassification(
			double *element_balance_total,
			int *percent_of_asset,
			SUBCLASSIFICATION *last_subclassification,
			EQUITY_ELEMENT *equity_element,
			char *fund_name,
			char *transaction_date_time,
			double asset_balance_total,
			boolean is_financial_position );

void statement_net_income_account(
			double *element_balance_total,
			int *percent_of_asset,
			LIST *account_list,
			EQUITY_ELEMENT *equity_element,
			char *fund_name,
			char *transaction_date_time,
			double asset_balance_total,
			boolean is_financial_position );

ELEMENT *statement_equity_liability_element(
			double equity_element_balance_total,
			double asset_element_balance_total,
			double liability_element_balance_total );

LIST *statement_PDF_display_row_list(
			ELEMENT *element,
			LIST *prior_year_list,
			boolean is_percent_of_revenue );

LIST *statement_PDF_row_list(
			LIST *preclose_element_list,
			LIST *prior_year_list,
			enum subclassification_option,
			boolean is_percent_of_revenue );

LIST *statement_PDF_account_row_list(
			LIST *account_list,
			LIST *prior_year_list,
			boolean include_subclassification,
			boolean is_percent_of_revenue );

LIST *statement_PDF_omit_row_list(
			ELEMENT *element,
			LIST *prior_year_list,
			boolean is_percent_of_revenue );

LIST *statement_PDF_aggregate_row_list(
			ELEMENT *element,
			LIST *prior_year_list,
			boolean is_percent_of_revenue );

#endif

