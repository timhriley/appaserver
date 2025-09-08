/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/income_statement.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef INCOME_STATEMENT_H
#define INCOME_STATEMENT_H

#include "list.h"
#include "date.h"
#include "boolean.h"
#include "element.h"
#include "account.h"
#include "transaction.h"
#include "latex.h"
#include "html.h"
#include "transaction_date.h"
#include "statement.h"

typedef struct
{
	STATEMENT_SUBCLASS_DISPLAY_HTML_LIST *
		statement_subclass_display_html_list;
	LIST *row_list;
} INCOME_STATEMENT_SUBCLASS_DISPLAY_HTML;

/* Usage */
/* ----- */
INCOME_STATEMENT_SUBCLASS_DISPLAY_HTML *
	income_statement_subclass_display_html_new(
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double element_net_income,
		char *net_income_percent_of_income_display,
		char *net_income_label );

/* Process */
/* ------- */
INCOME_STATEMENT_SUBCLASS_DISPLAY_HTML *
	income_statement_subclass_display_html_calloc(
		void );

typedef struct
{
	STATEMENT_SUBCLASS_OMIT_HTML_LIST *
		statement_subclass_omit_html_list;
	LIST *row_list;
} INCOME_STATEMENT_SUBCLASS_OMIT_HTML;

/* Usage */
/* ----- */
INCOME_STATEMENT_SUBCLASS_OMIT_HTML *
	income_statement_subclass_omit_html_new(
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double element_net_income,
		char *net_income_percent_of_income_display,
		char *net_income_label );

/* Process */
/* ------- */
INCOME_STATEMENT_SUBCLASS_OMIT_HTML *
	income_statement_subclass_omit_html_calloc(
		void );

typedef struct
{
	STATEMENT_SUBCLASS_AGGR_HTML_LIST *
		statement_subclass_aggr_html_list;
	LIST *row_list;
} INCOME_STATEMENT_SUBCLASS_AGGR_HTML;

/* Usage */
/* ----- */
INCOME_STATEMENT_SUBCLASS_AGGR_HTML *
	income_statement_subclass_aggr_html_new(
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double element_net_income,
		char *net_income_percent_of_income_display,
		char *net_income_label );

/* Process */
/* ------- */
INCOME_STATEMENT_SUBCLASS_AGGR_HTML *
	income_statement_subclass_aggr_html_calloc(
		void );

typedef struct
{
	INCOME_STATEMENT_SUBCLASS_DISPLAY_HTML *
		income_statement_subclass_display_html;
	INCOME_STATEMENT_SUBCLASS_OMIT_HTML *
		income_statement_subclass_omit_html;
	INCOME_STATEMENT_SUBCLASS_AGGR_HTML *
		income_statement_subclass_aggr_html;
	HTML_TABLE *html_table;
} INCOME_STATEMENT_HTML;

/* Usage */
/* ----- */
INCOME_STATEMENT_HTML *income_statement_html_new(
		enum statement_subclassification_option
			statement_subclassification_option,
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double element_net_income,
		char *net_income_percent_of_income_display,
		char *net_income_label );

/* Process */
/* ------- */
INCOME_STATEMENT_HTML *
	income_statement_html_calloc(
		void );

/* Usage */
/* ----- */
HTML_TABLE *income_statement_html_table(
		char *statement_caption_sub_title,
		LIST *column_list,
		LIST *row_list );

/* Usage */
/* ----- */
HTML_ROW *income_statement_html_net_income_row(
		LIST *statement_prior_year_list,
		double element_net_income,
		char *net_income_percent_of_income_display,
		char *net_income_label,
		int empty_cell_count );

typedef struct
{
	STATEMENT_SUBCLASS_DISPLAY_LATEX_LIST *
		statement_subclass_display_latex_list;
	LIST *row_list;
} INCOME_STATEMENT_SUBCLASS_DISPLAY_LATEX;

/* Usage */
/* ----- */
INCOME_STATEMENT_SUBCLASS_DISPLAY_LATEX *
	income_statement_subclass_display_latex_new(
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double element_net_income,
		char *net_income_percent_of_income_display,
		char *income_statement_net_income_label );

/* Process */
/* ------- */
INCOME_STATEMENT_SUBCLASS_DISPLAY_LATEX *
	income_statement_subclass_display_latex_calloc(
		void );

typedef struct
{
	STATEMENT_SUBCLASS_OMIT_LATEX_LIST *
		statement_subclass_omit_latex_list;
	LIST *row_list;
} INCOME_STATEMENT_SUBCLASS_OMIT_LATEX;

/* Usage */
/* ----- */
INCOME_STATEMENT_SUBCLASS_OMIT_LATEX *
	income_statement_subclass_omit_latex_new(
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double element_net_income,
		char *net_income_percent_of_income_display,
		char *income_statement_net_income_label );

/* Process */
/* ------- */
INCOME_STATEMENT_SUBCLASS_OMIT_LATEX *
	income_statement_subclass_omit_latex_calloc(
		void );

typedef struct
{
	STATEMENT_SUBCLASS_AGGR_LATEX_LIST *
		statement_subclass_aggr_latex_list;
	LIST *row_list;
} INCOME_STATEMENT_SUBCLASS_AGGR_LATEX;

/* Usage */
/* ----- */
INCOME_STATEMENT_SUBCLASS_AGGR_LATEX *
	income_statement_subclass_aggr_latex_new(
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double element_net_income,
		char *net_income_percent_of_income_display,
		char *income_statement_net_income_label );

/* Process */
/* ------- */
INCOME_STATEMENT_SUBCLASS_AGGR_LATEX *
	income_statement_subclass_aggr_latex_calloc(
		void );

typedef struct
{
	STATEMENT_LINK *statement_link;
	INCOME_STATEMENT_SUBCLASS_DISPLAY_LATEX *
		income_statement_subclass_display_latex;
	INCOME_STATEMENT_SUBCLASS_OMIT_LATEX *
		income_statement_subclass_omit_latex;
	INCOME_STATEMENT_SUBCLASS_AGGR_LATEX *
		income_statement_subclass_aggr_latex;
	LATEX *latex;
	LATEX_TABLE *latex_table;
} INCOME_STATEMENT_LATEX;

/* Usage */
/* ----- */
INCOME_STATEMENT_LATEX *income_statement_latex_new(
		char *application_name,
		char *process_name,
		char *appaserver_parameter_data_directory,
		enum statement_subclassification_option
			statement_subclassification_option,
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double element_net_income,
		char *net_income_percent_of_income_display,
		char *income_statement_net_income_label,
		pid_t process_id );

/* Process */
/* ------- */
INCOME_STATEMENT_LATEX *income_statement_latex_calloc(
		void );

/* Usage */
/* ----- */
LATEX_TABLE *income_statement_latex_table(
		char *statement_caption_combined,
		LIST *latex_column_list,
		LIST *latex_row_list );

/* Usage */
/* ----- */
LATEX_ROW *income_statement_latex_net_income_row(
		LIST *statement_prior_year_list,
		double element_net_income,
		char *net_income_percent_of_income_display,
		char *income_statement_net_income_label,
		LIST *latex_column_list,
		int empty_cell_count );

typedef struct
{
	enum statement_subclassification_option
		statement_subclassification_option;
	enum statement_output_medium
		statement_output_medium;
	TRANSACTION_DATE_STATEMENT *transaction_date_statement;
	LIST *element_name_list;
	STATEMENT *statement;
	LIST *statement_prior_year_list;
	double element_net_income;
	char *net_income_percent_of_income_display;
	char *net_income_label;
	INCOME_STATEMENT_LATEX *income_statement_latex;
	INCOME_STATEMENT_HTML *income_statement_html;
} INCOME_STATEMENT;

/* Usage */
/* ----- */
INCOME_STATEMENT *income_statement_fetch(
		char *argv_0,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		char *appaserver_parameter_data_directory,
		char *as_of_date_string,
		int prior_year_count,
		char *subclassifiction_option_string,
		char *output_medium_string );

/* Process */
/* ------- */
INCOME_STATEMENT *income_statement_calloc(
		void );

LIST *income_statement_element_name_list(
		const char *element_revenue,
		const char *element_expense,
		const char *element_expenditure,
		const char *element_gain,
		const char *element_loss );

/* Returns heap memory */
/* ------------------- */
char *income_statement_net_income_percent_of_income_display(
		const char *element_revenue,
		const char *element_gain,
		double element_net_income,
		LIST *element_statement_list );

/* Returns program memory */
/* ---------------------- */
char *income_statement_net_income_label(
		char *argv_0 );

/* Usage */
/* ----- */
LIST *income_statement_prior_net_income_data_list(
		double current_net_income,
		LIST *statement_prior_year_list );

/* Usage */
/* ----- */
double income_statement_fetch_net_income(
		char *end_date_time_string );

#endif
