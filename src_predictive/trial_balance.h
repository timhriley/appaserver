/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/trial_balance.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef TRIAL_BALANCE_H
#define TRIAL_BALANCE_H

#include "list.h"
#include "date.h"
#include "boolean.h"
#include "element.h"
#include "account.h"
#include "latex.h"
#include "html.h"
#include "statement.h"

typedef struct
{
	LIST *column_list;
	LIST *row_list;
} TRIAL_BALANCE_SUBCLASS_DISPLAY_HTML;

/* Usage */
/* ----- */
TRIAL_BALANCE_SUBCLASS_DISPLAY_HTML *
	trial_balance_subclass_display_html_new(
		char *application_name,
		char *login_name,
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double debit_sum,
		double credit_sum );

/* Process */
/* ------- */
TRIAL_BALANCE_SUBCLASS_DISPLAY_HTML *
	trial_balance_subclass_display_html_calloc(
		void );

/* Usage */
/* ----- */
LIST *trial_balance_subclass_display_html_column_list(
		LIST *statement_prior_year_list );

/* Usage */
/* ----- */
LIST *trial_balance_subclass_display_html_row_list(
		char *application_name,
		char *login_name,
		char *end_date_time_string,
		char *element_name,
		boolean element_accumulate_debit,
		char *subclassification_name,
		LIST *account_statement_list,
		LIST *statement_prior_year_list );

/* Usage */
/* ----- */
HTML_ROW *trial_balance_subclass_display_html_row(
		char *application_name,
		char *login_name,
		char *element_name,
		char *subclassification_name,
		STATEMENT_ACCOUNT *statement_account,
		LIST *statement_prior_year_list );

typedef struct
{
	LIST *column_list;
	LIST *row_list;
} TRIAL_BALANCE_SUBCLASS_OMIT_HTML;

/* Usage */
/* ----- */
TRIAL_BALANCE_SUBCLASS_OMIT_HTML *
	trial_balance_subclass_omit_html_new(
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double debit_sum,
		double credit_sum );

/* Process */
/* ------- */
TRIAL_BALANCE_SUBCLASS_OMIT_HTML *
	trial_balance_subclass_omit_html_calloc(
		void );

/* Usage */
/* ----- */
LIST *trial_balance_subclass_omit_html_column_list(
		LIST *statement_prior_year_list );

/* Usage */
/* ----- */
LIST *trial_balance_subclass_omit_html_row_list(
		char *end_date_time_string,
		char *element_name,
		boolean element_accumulate_debit,
		LIST *account_statement_list,
		LIST *statement_prior_year_list );

/* Usage */
/* ----- */
HTML_ROW *trial_balance_subclass_omit_html_row(
		char *element_name,
		STATEMENT_ACCOUNT *statement_account,
		LIST *statement_prior_year_list );

typedef struct
{
	TRIAL_BALANCE_SUBCLASS_DISPLAY_HTML *
		trial_balance_subclass_display_html;

	TRIAL_BALANCE_SUBCLASS_OMIT_HTML *
		trial_balance_subclass_omit_html;

	HTML_TABLE *html_table;
} TRIAL_BALANCE_HTML;

/* Usage */
/* ----- */
TRIAL_BALANCE_HTML *trial_balance_html_new(
		char *application_name,
		char *login_name,
		enum statement_subclassification_option
			statement_subclassification_option,
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double debit_sum,
		double credit_sum );

/* Process */
/* ------- */
TRIAL_BALANCE_HTML *trial_balance_html_calloc(
		void );

/* Usage */
/* ----- */
HTML_TABLE *trial_balance_html_table(
		char *statement_caption_sub_title,
		LIST *column_list,
		LIST *row_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *trial_balance_html_account_datum(
		char *account_name,
		char *full_name,
		double debit_amount,
		double credit_amount,
		char *transaction_date_american,
		char *memo );

typedef struct
{
	TRIAL_BALANCE_HTML *preclose_trial_balance_html;
	TRIAL_BALANCE_HTML *trial_balance_html;
} TRIAL_BALANCE_TABLE;

/* Usage */
/* ----- */
TRIAL_BALANCE_TABLE *trial_balance_table_new(
		char *application_name,
		char *login_name,
		enum statement_subclassification_option
			statement_subclassification_option,
		STATEMENT *preclose_statement,
		LIST *preclose_statement_prior_year_list,
		double preclose_debit_sum,
		double preclose_credit_sum,
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double debit_sum,
		double credit_sum );

/* Process */
/* ------- */
TRIAL_BALANCE_TABLE *trial_balance_table_calloc(
		void );

typedef struct
{
	LIST *column_list;
	LIST *row_list;
} TRIAL_BALANCE_SUBCLASS_OMIT_LATEX;

/* Usage */
/* ----- */
TRIAL_BALANCE_SUBCLASS_OMIT_LATEX *
	trial_balance_subclass_omit_latex_new(
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double debit_sum,
		double credit_sum );

/* Process */
/* ------- */
TRIAL_BALANCE_SUBCLASS_OMIT_LATEX *
	trial_balance_subclass_omit_latex_calloc(
		void );

/* Usage */
/* ----- */
LIST *trial_balance_subclass_omit_latex_column_list(
		LIST *statement_prior_year_list );

/* Usage */
/* ----- */
LIST *trial_balance_subclass_omit_latex_row_list(
		char *end_date_time_string,
		char *element_name,
		boolean element_accumulate_debit,
		LIST *account_statement_list,
		LIST *statement_prior_year_list,
		LIST *latex_column_list );

/* Usage */
/* ----- */
LATEX_ROW *trial_balance_subclass_omit_latex_row(
		char *element_name,
		STATEMENT_ACCOUNT *statement_account,
		LIST *statement_prior_year_list,
		LIST *latex_column_list );

typedef struct
{
	LIST *column_list;
	LIST *row_list;
} TRIAL_BALANCE_SUBCLASS_DISPLAY_LATEX;

/* Usage */
/* ----- */
TRIAL_BALANCE_SUBCLASS_DISPLAY_LATEX *
	trial_balance_subclass_display_latex_new(
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double debit_sum,
		double credit_sum );

/* Process */
/* ------- */
TRIAL_BALANCE_SUBCLASS_DISPLAY_LATEX *
	trial_balance_subclass_display_latex_calloc(
		void );

/* Usage */
/* ----- */
LIST *trial_balance_subclass_display_latex_column_list(
		LIST *statement_prior_year_list );

/* Usage */
/* ----- */
LIST *trial_balance_subclass_display_latex_row_list(
		char *end_date_time_string,
		char *element_name,
		boolean element_accumulate_debit,
		char *subclassification_name,
		LIST *account_statement_list,
		LIST *statement_prior_year_list,
		LIST *latex_column_list );

/* Usage */
/* ----- */
LATEX_ROW *trial_balance_subclass_display_latex_row(
		char *element_name,
		char *subclassification_name,
		STATEMENT_ACCOUNT *statement_account,
		LIST *statement_prior_year_list,
		LIST *latex_column_list );

typedef struct
{
	LATEX *latex;
	TRIAL_BALANCE_SUBCLASS_DISPLAY_LATEX *
		trial_balance_subclass_display_latex;
	TRIAL_BALANCE_SUBCLASS_OMIT_LATEX *
		trial_balance_subclass_omit_latex;
	LATEX_TABLE *latex_table;
} TRIAL_BALANCE_LATEX;

/* Usage */
/* ----- */
TRIAL_BALANCE_LATEX *trial_balance_latex_new(
		enum statement_subclassification_option
			statement_subclassification_option,
		char *tex_filename,
		char *working_directory,
		boolean statement_pdf_landscape_boolean,
		char *statement_caption_logo_filename,
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double debit_sum,
		double credit_sum );

/* Process */
/* ------- */
TRIAL_BALANCE_LATEX *trial_balance_latex_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *trial_balance_latex_account_datum(
		char *account_name,
		char *full_name,
		double debit_amount,
		double credit_amount,
		char *transaction_date_american,
		char *memo );

/* Usage */
/* ----- */
LATEX_ROW *trial_balance_latex_total_row(
		double debit_sum,
		double credit_sum,
		LIST *latex_column_list,
		int column_skip_count );

#define TRIAL_BALANCE_PDF_PRECLOSE_KEY	"preclose"

typedef struct
{
	boolean statement_pdf_landscape_boolean;
	STATEMENT_LINK *preclose_statement_link;
	TRIAL_BALANCE_LATEX *preclose_trial_balance_latex;
	STATEMENT_LINK *statement_link;
	TRIAL_BALANCE_LATEX *trial_balance_latex;
} TRIAL_BALANCE_PDF;

/* Usage */
/* ----- */
TRIAL_BALANCE_PDF *trial_balance_pdf_new(
		char *application_name,
		char *appaserver_parameter_data_directory,
		enum statement_subclassification_option
			statement_subclassification_option,
		STATEMENT *preclose_statement,
		LIST *preclose_statement_prior_year_list,
		double preclose_debit_sum,
		double preclose_credit_sum,
		STATEMENT *statement,
		LIST *statement_prior_year_list,
		double debit_sum,
		double credit_sum,
		pid_t process_id );

/* Process */
/* ------- */
TRIAL_BALANCE_PDF *trial_balance_pdf_calloc(
		void );

typedef struct
{
	enum statement_subclassification_option
		statement_subclassification_option;
	enum statement_output_medium
		statement_output_medium;
	TRANSACTION_DATE_TRIAL_BALANCE *transaction_date_trial_balance;
	LIST *element_name_list;
	STATEMENT *preclose_statement;
	LIST *preclose_statement_prior_year_list;
	STATEMENT *statement;
	LIST *statement_prior_year_list;
	double preclose_debit_sum;
	double preclose_credit_sum;
	double debit_sum;
	double credit_sum;
	TRIAL_BALANCE_PDF *trial_balance_pdf;
	TRIAL_BALANCE_TABLE *trial_balance_table;
} TRIAL_BALANCE;

/* Usage */
/* ----- */
TRIAL_BALANCE *trial_balance_fetch(
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		char *process_name,
		char *appaserver_parameter_data_directory,
		char *as_of_date_string,
		int prior_year_count,
		char *subclassification_option_string,
		char *output_medium_string );

/* Process */
/* ------- */
TRIAL_BALANCE *trial_balance_calloc(
		void );

LIST *trial_balance_element_name_list(
		char *element_asset,
		char *element_liability,
		char *element_revenue,
		char *element_expense,
		char *element_gain,
		char *element_loss,
		char *element_equity );

/* Returns static memory */
/* --------------------- */
char *trial_balance_preclose_process_name(
		char *process_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *trial_balance_transaction_count_string(
		int transaction_count );

/* Usage */
/* ----- */
HTML_ROW *trial_balance_html_total_row(
		double debit_sum,
		double credit_sum,
		int column_skip_count );

#endif
