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

typedef struct
{
	LATEX *latex;
	LATEX_TABLE *latex_table;
} TRIAL_BALANCE_SUBCLASSIFICATION_LATEX;

/* Usage */
/* ----- */
TRIAL_BALANCE_SUBCLASSIFICATION_LATEX *
	trial_balance_subclassification_latex_new(
			char *transaction_as_of_date,
			char *latex_filename,
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
			char *statement_link->latex_filename,
			char *statement_link->dvi_filename,
			char *statement_link->working_directory,
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
			char *login_name,
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

boolean trial_balance_pdf_landacape_boolean(
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
} TRIAL_BALANCE;

/* Usage */
/* ----- */
TRIAL_BALANCE *trial_balance_fetch(
			char *application_name,
			char *session_key,
			char *role_name,
			char *process_name,
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

typedef struct
{
	double balance;
	char *debit_string;
	char *credit_string;
	int date_days_between;
	boolean within_days_between_boolean;
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

#endif
