/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/trial_balance.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef TRIAL_BALANCE_H
#define TRIAL_BALANCE_H

#include "list.h"
#include "date.h"
#include "boolean.h"
#include "element.h"
#include "account.h"
#include "statement.h"
#include "latex.h"
#include "html_table.h"

typedef struct
{
	double balance;
	char *debit_string;
	char *credit_string;
	int date_days_between;
	boolean within_days_between_boolean;
	char *asset_percent_string;
	char *revenue_percent_string;
	char *percent_string;
	ACCOUNT *account;
} TRIAL_BALANCE_ACCOUNT;

/* Usage */
/* ----- */
TRIAL_BALANCE_ACCOUNT *trial_balance_account_new(
			char *transaction_as_of_date,
			boolean element_accumulate_debit,
			ACCOUNT *account );

/* Process */
/* ------- */
TRIAL_BALANCE_ACCOUNT *trial_balance_account_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *trial_balance_account_asset_percent_string(
			int percent_of_asset );

/* Returns heap memory */
/* ------------------- */
char *trial_balance_account_revenue_percent_string(
			int percent_of_revenue );

/* Returns heap memory */
/* ------------------- */
char *trial_balance_account_percent_string(
			int percent_of_asset,
			int percent_of_revenue );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *trial_balance_account_balance_string(
			double balance,
			char *account_action_string );

/* Process */
/* ------- */

typedef struct
{
	HTML_TABLE *html_table;
} TRIAL_BALANCE_SUBCLASSIFICATION_HTML;

/* Usage */
/* ----- */
TRIAL_BALANCE_SUBCLASSIFICATION_HTML *
	trial_balance_subclassification_html_new(
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double debit_sum,
			double credit_sum );

/* Process */
/* ------- */
TRIAL_BALANCE_SUBCLASSIFICATION_HTML *
	trial_balance_subclassification_html_calloc(
			void );

/* Usage */
/* ----- */
HTML_TABLE *trial_balance_subclassification_html_table(
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double debit_sum,
			double credit_sum );

/* Process */
/* ------- */
LIST *trial_balance_subclassification_html_heading_list(
			LIST *statement_prior_year_list );

/* Usage */
/* ----- */
LIST *trial_balance_subclassification_html_row_list(
			char *transaction_as_of_date,
			LIST *element_statement_list,
			LIST *statement_prior_year_list,
			double debit_sum,
			double credit_sum );

/* Process */
/* ------- */
HTML_ROW *trial_balance_subclassification_html_sum_row(
			double debit_sum,
			double credit_sum );

/* Usage */
/* ----- */
LIST *trial_balance_subclassification_html_element_row_list(
			char *transaction_as_of_date,
			char *element_name,
			boolean element_accumulate_debit,
			LIST *subclassification_statement_list,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *trial_balance_subclassification_html_account_row_list(
			char *transaction_as_of_date,
			char *element_name,
			boolean element_accumulate_debit,
			char *subclassification_name,
			LIST *account_statement_list,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
HTML_ROW *trial_balance_subclassification_html_account_row(
			char *element_name,
			char *subclassification_name,
			TRIAL_BALANCE_ACCOUNT *trial_balance_account,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

typedef struct
{
	TRIAL_BALANCE_SUBCLASSIFICATION_HTML *
		trial_balance_subclassification_html;

/* 
	TRIAL_BALANCE_ACCOUNT_HTML *
		trial_balance_account_html;
*/
} TRIAL_BALANCE_HTML;

/* Usage */
/* ----- */
TRIAL_BALANCE_HTML *trial_balance_html_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_name,
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

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *trial_balance_html_account_title(
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
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_name,
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
	LATEX *latex;
	LATEX_TABLE *table;
	LIST *heading_list;
	LIST *row_list;
	LIST *element_row_list;
	LIST *account_row_list;
	LATEX_ROW *account_row;
	char *trial_balance_latex_account_title;
} TRIAL_BALANCE_SUBCLASSIFICATION_LATEX;

/* Usage */
/* ----- */
TRIAL_BALANCE_SUBCLASSIFICATION_LATEX *
	trial_balance_subclassification_latex_new(
			char *transaction_as_of_date,
			char *tex_filename,
			char *dvi_filename,
			char *working_directory,
			boolean trial_balance_pdf_landscape_boolean,
			char *statement_logo_filename,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double debit_sum,
			double credit_sum );

/* Process */
/* ------- */
TRIAL_BALANCE_SUBCLASSIFICATION_LATEX *
	trial_balance_subclassification_latex_calloc(
			void );

LATEX_TABLE *trial_balance_subclassification_latex_table(
			char *transaction_as_of_date,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double debit_sum,
			double credit_sum );

LIST *trial_balance_subclassification_latex_heading_list(
			LIST *statement_prior_year_list );

/* Usage */
/* ----- */
LIST *trial_balance_subclassification_latex_row_list(
			char *transaction_as_of_date,
			LIST *element_statement_list,
			LIST *statement_prior_year_list,
			double debit_sum,
			double credit_sum );

/* Process */
/* ------- */
LATEX_ROW *trial_balance_subclassification_latex_sum_row(
			double debit_sum,
			double credit_sum );

/* Usage */
/* ----- */
LIST *trial_balance_subclassification_latex_element_row_list(
			char *transaction_as_of_date,
			char *element_name,
			boolean element_accumulate_debit,
			LIST *subclassification_statement_list,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *trial_balance_subclassification_latex_account_row_list(
			char *transaction_as_of_date,
			char *element_name,
			boolean element_accumulate_debit,
			char *subclassification_name,
			LIST *account_statement_list,
			LIST *statement_prior_year_list );

/* Usage */
/* ----- */
LATEX_ROW *trial_balance_subclassification_latex_account_row(
			char *element_name,
			char *subclassification_name,
			TRIAL_BALANCE_ACCOUNT *trial_balance_account,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Returns heap memory or null */
/* --------------------------- */
char *trial_balance_subclassification_latex_account_row_title(
			char *name );

typedef struct
{
	TRIAL_BALANCE_SUBCLASSIFICATION_LATEX *
		trial_balance_subclassification_latex;
} TRIAL_BALANCE_LATEX;

/* Usage */
/* ----- */
TRIAL_BALANCE_LATEX *trial_balance_latex_new(
			enum statement_subclassification_option
				statement_subclassification_option,
			char *transaction_as_of_date,
			char *tex_filename,
			char *dvi_filename,
			char *working_directory,
			boolean trial_balance_pdf_landscape_boolean,
			char *statement_logo_filename,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double debit_sum,
			double credit_sum );

/* Process */
/* ------- */
TRIAL_BALANCE_LATEX *trial_balance_latex_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *trial_balance_latex_account_title(
			char *account_name,
			char *full_name,
			double debit_amount,
			double credit_amount,
			char *transaction_date_american,
			char *memo );

#define TRIAL_BALANCE_PDF_PRECLOSE_KEY	"preclose"

typedef struct
{
	boolean landscape_boolean;
	char *preclose_key;
	STATEMENT_LINK *preclose_statement_link;
	TRIAL_BALANCE_LATEX *preclose_trial_balance_latex;
	STATEMENT_LINK *statement_link;
	TRIAL_BALANCE_LATEX *trial_balance_latex;
} TRIAL_BALANCE_PDF;

/* Usage */
/* ----- */
TRIAL_BALANCE_PDF *trial_balance_pdf_new(
			char *application_name,
			char *process_name,
			char *document_root_directory,
			enum statement_subclassification_option
				statement_subclassification_option,
			char *transaction_begin_date_string,
			char *transaction_as_of_date,
			char *statement_logo_filename,
			char *statement_title,
			char *statement_subtitle,
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

boolean trial_balance_pdf_landscape_boolean(
	int statement_prior_year_list_length );

/* Returns trial_balance_pdf_preclose_key */
/* -------------------------------------- */
char *trial_balance_pdf_preclose_key(
			char *trial_balance_pdf_preclose_key );

#define TRIAL_BALANCE_DAYS_FOR_EMPHASIS		35

typedef struct
{
	enum statement_subclassification_option
		statement_subclassification_option;
	enum statement_output_medium
		statement_output_medium;
	char *transaction_as_of_date;
	char *transaction_begin_date_string;
	char *statement_logo_filename;
	char *statement_title;
	char *statement_subtitle;
	LIST *filter_element_name_list;
	boolean transaction_closing_entry_exists;
	char *transaction_date_time_closing;
	STATEMENT *preclose_statement;
	LIST *preclose_prior_year_list;
	STATEMENT *statement;
	LIST *prior_year_list;
	double preclose_debit_sum;
	double preclose_credit_sum;
	double debit_sum;
	double credit_sum;
	LIST *element_list_non_nominal_account_list;
	ELEMENT *element_asset;
	LIST *element_list_nominal_account_list;
	ELEMENT *element_revenue;
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
			char *document_root_directory,
			char *as_of_date,
			int prior_year_count,
			char *subclassifiction_option_string,
			char *output_medium_string );

/* Process */
/* ------- */
TRIAL_BALANCE *trial_balance_calloc(
			void );

LIST *trial_balance_filter_element_name_list(
			void );

/* Public */
/* ------ */

/* Returns heap memory or "" */
/* ------------------------- */
char *trial_balance_column_row_title(
			char *name );

/* Returns heap memory */
/* ------------------- */
char *trial_balance_transaction_count_string(
			int transaction_count );

#endif
