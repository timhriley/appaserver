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
#include "latex.h"
#include "html_table.h"
#include "statement.h"

typedef struct
{
	LIST *heading_list;
	LIST *row_list;
} TRIAL_BALANCE_SUBCLASS_DISPLAY_HTML;

/* Usage */
/* ----- */
TRIAL_BALANCE_SUBCLASS_DISPLAY_HTML *
	trial_balance_subclass_display_html_new(
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
LIST *trial_balance_subclass_display_html_heading_list(
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *trial_balance_subclass_display_html_row_list(
			char *transaction_as_of_date,
			LIST *element_statement_list,
			LIST *statement_prior_year_list,
			double debit_sum,
			double credit_sum );

/* Process */
/* ------- */
HTML_ROW *trial_balance_subclass_display_html_sum_row(
			double debit_sum,
			double credit_sum );

/* Usage */
/* ----- */
LIST *trial_balance_subclass_display_html_element_row_list(
			char *transaction_as_of_date,
			char *element_name,
			boolean element_accumulate_debit,
			LIST *subclassification_statement_list,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *trial_balance_subclass_display_html_account_row_list(
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
HTML_ROW *trial_balance_subclass_display_html_account_row(
			char *element_name,
			char *subclassification_name,
			STATEMENT_ACCOUNT *statement_account,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

typedef struct
{
	LIST *heading_list;
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
LIST *trial_balance_subclass_omit_html_heading_list(
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *trial_balance_subclass_omit_html_row_list(
			char *transaction_as_of_date,
			LIST *element_statement_list,
			LIST *statement_prior_year_list,
			double debit_sum,
			double credit_sum );

/* Process */
/* ------- */
HTML_ROW *trial_balance_subclass_omit_html_sum_row(
			double debit_sum,
			double credit_sum );

/* Usage */
/* ----- */
LIST *trial_balance_subclass_omit_html_element_row_list(
			char *transaction_as_of_date,
			char *element_name,
			boolean element_accumulate_debit,
			LIST *account_statement_list,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
HTML_ROW *trial_balance_subclass_omit_html_account_row(
			char *element_name,
			STATEMENT_ACCOUNT *statement_account,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

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
			char *statement_caption_subtitle,
			LIST *heading_list,
			LIST *row_list );

/* Process */
/* ------- */

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
	LIST *heading_list;
	LIST *row_list;
} TRIAL_BALANCE_SUBCLASS_OMIT_LATEX;

/* Usage */
/* ----- */
TRIAL_BALANCE_SUBCLASS_OMIT_LATEX *
	trial_balance_subclass_omit_latex_new(
			char *transaction_as_of_date,
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
LIST *trial_balance_subclass_omit_latex_heading_list(
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *trial_balance_subclass_omit_latex_row_list(
			char *transaction_as_of_date,
			LIST *element_statement_list,
			LIST *statement_prior_year_list,
			double debit_sum,
			double credit_sum );

/* Process */
/* ------- */
LATEX_ROW *trial_balance_subclass_omit_latex_sum_row(
			double debit_sum,
			double credit_sum );


/* Usage */
/* ----- */
LIST *trial_balance_subclass_omit_latex_element_row_list(
			char *transaction_as_of_date,
			char *element_name,
			boolean element_accumulate_debit,
			LIST *account_statement_list,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LATEX_ROW *trial_balance_subclass_omit_latex_account_row(
			char *element_name,
			STATEMENT_ACCOUNT *statement_account,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

typedef struct
{
	LIST *heading_list;
	LIST *row_list;
} TRIAL_BALANCE_SUBCLASS_DISPLAY_LATEX;

/* Usage */
/* ----- */
TRIAL_BALANCE_SUBCLASS_DISPLAY_LATEX *
	trial_balance_subclass_display_latex_new(
			char *transaction_as_of_date,
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
LIST *trial_balance_subclass_display_latex_heading_list(
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *trial_balance_subclass_display_latex_row_list(
			char *transaction_as_of_date,
			LIST *element_statement_list,
			LIST *statement_prior_year_list,
			double debit_sum,
			double credit_sum );

/* Process */
/* ------- */
LATEX_ROW *trial_balance_subclass_display_latex_sum_row(
			double debit_sum,
			double credit_sum );

/* Usage */
/* ----- */
LIST *trial_balance_subclass_display_latex_element_row_list(
			char *transaction_as_of_date,
			char *element_name,
			boolean element_accumulate_debit,
			LIST *subclassification_statement_list,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *trial_balance_subclass_display_latex_account_row_list(
			char *transaction_as_of_date,
			char *element_name,
			boolean element_accumulate_debit,
			char *subclassification_name,
			LIST *account_statement_list,
			LIST *statement_prior_year_list );

/* Usage */
/* ----- */
LATEX_ROW *trial_balance_subclass_display_latex_account_row(
			char *element_name,
			char *subclassification_name,
			STATEMENT_ACCOUNT *statement_account,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Returns heap memory or null */
/* --------------------------- */
char *trial_balance_subclass_display_latex_account_row_title(
			char *name );

typedef struct
{
	TRIAL_BALANCE_SUBCLASS_DISPLAY_LATEX *
		trial_balance_subclass_display_latex;

	TRIAL_BALANCE_SUBCLASS_OMIT_LATEX *
		trial_balance_subclass_omit_latex;

	LATEX *latex;
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
			boolean statement_pdf_landscape_boolean,
			char *statement_logo_filename,
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
LATEX_TABLE *trial_balance_latex_table(
			char *statement_caption,
			LIST *heading_list,
			LIST *row_list );

/* Process */
/* ------- */

/* Public */
/* ------ */

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
	boolean statement_pdf_landscape_boolean;
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

/* Returns trial_balance_pdf_preclose_key */
/* -------------------------------------- */
char *trial_balance_pdf_preclose_key(
			char *trial_balance_pdf_preclose_key );

typedef struct
{
	enum statement_subclassification_option
		statement_subclassification_option;
	enum statement_output_medium
		statement_output_medium;
	char *transaction_as_of_date;
	char *transaction_begin_date_string;
	LIST *element_name_list;
	boolean transaction_closing_entry_exists;
	char *transaction_date_time_closing;
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
			char *document_root_directory,
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

/* Public */
/* ------ */

/* Returns heap memory */
/* ------------------- */
char *trial_balance_transaction_count_string(
			int transaction_count );

#endif
