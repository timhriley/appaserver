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
			LIST *statement_fund_list );

STATEMENT_FUND *statement_fund_steady_state(
			LIST *preclose_element_list,
			LIST *postclose_element_list,
			int prior_year_count,
			STATEMENT_FUND *statement_fund );

boolean statement_fund_exists_postclose(
			LIST *statement_fund_list );

LIST *statement_prior_year_heading_list(
			LIST *prior_year_list );

LIST *statement_prior_year_delta_list(
			char *account_name,
			LIST *prior_year_list );

LIST *statement_html_prior_year_delta_list(
			char *account_name,
			LIST *prior_year_list );

LIST *statement_fund_steady_state_prior_year_list(
			int prior_year_count,
			LIST *preclose_element_list,
			STATEMENT_FUND *statement_fund );

boolean statement_fund_exists_prior_year(
			LIST *statement_fund_list );

LIST *statement_PDF_prior_year_delta_list(
			char *account_name,
			LIST *prior_year_list );

STATEMENT_FUND *statement_fund_seek(
			char *fund_name,
			LIST *statement_fund_list );

void statement_fund_list_net_income_set(
			LIST *statement_fund_list );

double statement_fund_net_income(
			double revenue_total,
			double expense_total,
			double gain_total,
			double loss_total );

int statement_fund_net_income_percent(
			double net_income,
			double revenue_total );

#endif

