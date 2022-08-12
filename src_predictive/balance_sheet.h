/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/balance_sheet.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef BALANCE_SHEET_H
#define BALANCE_SHEET_H

#include "list.h"
#include "date.h"
#include "boolean.h"
#include "element.h"
#include "account.h"
#include "transaction.h"
#include "latex.h"
#include "html_table.h"
#include "statement.h"

#define BALANCE_SHEET_BEGIN_BALANCE_LABEL   	   "equity_begin_balance"
#define BALANCE_SHEET_TRANSACTION_AMOUNT_LABEL	   "equity_transactions"
#define BALANCE_SHEET_END_BALANCE_LABEL	   	   "equity_end_balance"
#define BALANCE_SHEET_LIABILITY_PLUS_EQUITY_LABEL  "liability_plus_equity"

typedef struct
{
	double begin_balance;
	double current_balance;
	double transaction_amount;
	double end_balance;
	double liability_balance;
	double liability_plus_equity;
} BALANCE_SHEET_EQUITY;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
BALANCE_SHEET_EQUITY *
	balance_sheet_equity_new(
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income );

/* Process */
/* ------- */
BALANCE_SHEET_EQUITY *balance_sheet_equity_calloc(
			void );

double balance_sheet_equity_begin_balance(
			ELEMENT *element_equity_begin );

double balance_sheet_equity_current_balance(
			ELEMENT *element_equity_current );

double balance_sheet_equity_transaction_amount(
			double balance_sheet_equity_current_balance,
			double balance_sheet_equity_begin_balance );

double balance_sheet_equity_end_balance(
			double element_equity_current_balance,
			double income_statement_fetch_net_income );

double balance_sheet_equity_liability_balance(
			ELEMENT *element_liability );

double balance_sheet_equity_liability_plus_equity(
			double balance_sheet_equity_liability_balance,
			double balance_sheet_equity_end_balance );

typedef struct
{
	STATEMENT_SUBCLASS_DISPLAY_HTML_LIST *
		statement_subclass_display_html_list;
	LIST *row_list;
} BALANCE_SHEET_SUBCLASS_DISPLAY_HTML;

/* Usage */
/* ----- */
BALANCE_SHEET_SUBCLASS_DISPLAY_HTML *
	balance_sheet_subclass_display_html_new(
			LIST *element_statement_list,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

/* Process */
/* ------- */
BALANCE_SHEET_SUBCLASS_DISPLAY_HTML *
	balance_sheet_subclass_display_html_calloc(
			void );

/* Usage */
/* ----- */
LIST *balance_sheet_subclass_display_html_equity_row_list(
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

/* Process */
/* ------- */
HTML_ROW *balance_sheet_subclass_display_html_equity_begin_row(
			char *balance_sheet_begin_balance_label,
			double element_equity_begin_sum );

HTML_ROW *balance_sheet_subclass_display_html_transaction_amount_row(
			char *balance_sheet_transaction_amount_label,
			double balance_sheet_equity_transaction_amount );

HTML_ROW *balance_sheet_subclass_display_html_net_income_row(
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

HTML_ROW *balance_sheet_subclass_display_html_equity_end_row(
			char *balance_sheet_end_balance_label,
			double balance_sheet_equity_end_balance );

HTML_ROW *balance_sheet_subclass_display_html_liability_plus_equity_row(
			char *balance_sheet_liability_plus_equity_label,
			double balance_sheet_liability_plus_equity );

typedef struct
{
	STATEMENT_SUBCLASS_OMIT_HTML_LIST *
		statement_subclass_omit_html_list;
	LIST *row_list;
} BALANCE_SHEET_SUBCLASS_OMIT_HTML;

/* Usage */
/* ----- */
BALANCE_SHEET_SUBCLASS_OMIT_HTML *
	balance_sheet_subclass_omit_html_new(
			LIST *element_statement_list,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

/* Process */
/* ------- */
BALANCE_SHEET_SUBCLASS_OMIT_HTML *
	balance_sheet_subclass_omit_html_calloc(
			void );

/* Usage */
/* ----- */
LIST *balance_sheet_subclass_omit_html_equity_row_list(
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

/* Process */
/* ------- */
HTML_ROW *balance_sheet_subclass_omit_html_equity_begin_row(
			char *balance_sheet_begin_balance_label,
			double element_equity_begin_sum );

HTML_ROW *balance_sheet_subclass_omit_html_transaction_amount_row(
			char *balance_sheet_transaction_amount_label,
			double balance_sheet_equity_transaction_amount );

HTML_ROW *balance_sheet_subclass_omit_html_net_income_row(
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

HTML_ROW *balance_sheet_subclass_omit_html_equity_end_row(
			char *balance_sheet_end_balance_label,
			double balance_sheet_equity_end_balance );

HTML_ROW *balance_sheet_subclass_omit_html_liability_plus_equity_row(
			char *balance_sheet_liability_plus_equity_label,
			double balance_sheet_liability_plus_equity );

typedef struct
{
	STATEMENT_SUBCLASS_AGGR_HTML_LIST *
		statement_subclass_aggr_html_list;
	LIST *row_list;
} BALANCE_SHEET_SUBCLASS_AGGR_HTML;

/* Usage */
/* ----- */
BALANCE_SHEET_SUBCLASS_AGGR_HTML *
	balance_sheet_subclass_aggr_html_new(
			LIST *element_statement_list,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

/* Process */
/* ------- */
BALANCE_SHEET_SUBCLASS_AGGR_HTML *
	balance_sheet_subclass_aggr_html_calloc(
			void );

/* Usage */
/* ----- */
LIST *balance_sheet_subclass_aggr_html_equity_row_list(
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

/* Process */
/* ------- */
HTML_ROW *balance_sheet_subclass_aggr_html_equity_begin_row(
			char *balance_sheet_begin_balance_label,
			double element_equity_begin_sum );

HTML_ROW *balance_sheet_subclass_aggr_html_transaction_amount_row(
			char *balance_sheet_transaction_amount_label,
			double balance_sheet_equity_transaction_amount );

HTML_ROW *balance_sheet_subclass_aggr_html_net_income_row(
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

HTML_ROW *balance_sheet_subclass_aggr_html_equity_end_row(
			char *balance_sheet_end_balance_label,
			double balance_sheet_equity_end_balance );

HTML_ROW *balance_sheet_subclass_aggr_html_liability_plus_equity_row(
			char *balance_sheet_liability_plus_equity_label,
			double balance_sheet_liability_plus_equity );

typedef struct
{
	ELEMENT *element_liability;
	BALANCE_SHEET_SUBCLASS_DISPLAY_HTML *
		balance_sheet_subclass_display_html;
	BALANCE_SHEET_SUBCLASS_OMIT_HTML *
		balance_sheet_subclass_omit_html;
	BALANCE_SHEET_SUBCLASS_AGGR_HTML *
		balance_sheet_subclass_aggr_html;
	HTML_TABLE *html_table;
} BALANCE_SHEET_HTML;

/* Usage */
/* ----- */
BALANCE_SHEET_HTML *balance_sheet_html_new(
			enum statement_subclassification_option
				statement_subclassification_option,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

/* Process */
/* ------- */
BALANCE_SHEET_HTML *
	balance_sheet_html_calloc(
			void );

/* Usage */
/* ----- */
HTML_TABLE *balance_sheet_html_table(
			char *statement_caption_subtitle,
			LIST *heading_list,
			LIST *row_list );

/* Process */
/* ------- */

typedef struct
{
	STATEMENT_SUBCLASS_DISPLAY_LATEX_LIST *
		statement_subclass_display_latex_list;
	LIST *row_list;
} BALANCE_SHEET_SUBCLASS_DISPLAY_LATEX;

/* Usage */
/* ----- */
BALANCE_SHEET_SUBCLASS_DISPLAY_LATEX *
	balance_sheet_subclass_display_latex_new(
			LIST *element_statement_list,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

/* Process */
/* ------- */
BALANCE_SHEET_SUBCLASS_DISPLAY_LATEX *
	balance_sheet_subclass_display_latex_calloc(
			void );

/* Usage */
/* ----- */
LIST *balance_sheet_subclass_display_latex_equity_row_list(
			int statement_prior_year_list_length,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

/* Process */
/* ------- */
LATEX_ROW *balance_sheet_subclass_display_latex_equity_begin_row(
			int statement_prior_year_list_length,
			char *balance_sheet_begin_balance_label,
			double element_equity_begin_sum );

LATEX_ROW *balance_sheet_subclass_display_latex_transaction_amount_row(
			int statement_prior_year_list_length,
			char *balance_sheet_transaction_amount_label,
			double balance_sheet_equity_transaction_amount );

LATEX_ROW *balance_sheet_subclass_display_latex_net_income_row(
			int statement_prior_year_list_length,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

LATEX_ROW *balance_sheet_subclass_display_latex_equity_end_row(
			int statement_prior_year_list_length,
			char *balance_sheet_end_balance_label,
			double balance_sheet_equity_end_balance );

LATEX_ROW *balance_sheet_subclass_display_latex_liability_plus_equity_row(
			int statement_prior_year_list_length,
			char *balance_sheet_liability_plus_equity_label,
			double balance_sheet_liability_plus_equity );

typedef struct
{
	STATEMENT_SUBCLASS_OMIT_LATEX_LIST *
		statement_subclass_omit_latex_list;
	LIST *row_list;
} BALANCE_SHEET_SUBCLASS_OMIT_LATEX;

/* Usage */
/* ----- */
BALANCE_SHEET_SUBCLASS_OMIT_LATEX *
	balance_sheet_subclass_omit_latex_new(
			LIST *element_statement_list,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

/* Process */
/* ------- */
BALANCE_SHEET_SUBCLASS_OMIT_LATEX *
	balance_sheet_subclass_omit_latex_calloc(
			void );

/* Usage */
/* ----- */
LIST *balance_sheet_subclass_omit_latex_equity_row_list(
			int statement_prior_year_list_length,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

/* Process */
/* ------- */
LATEX_ROW *balance_sheet_subclass_omit_latex_equity_begin_row(
			int statement_prior_year_list_length,
			char *balance_sheet_begin_balance_label,
			double element_equity_begin_sum );

LATEX_ROW *balance_sheet_subclass_omit_latex_transaction_amount_row(
			int statement_prior_year_list_length,
			char *balance_sheet_transaction_amount_label,
			double balance_sheet_equity_transaction_amount );

LATEX_ROW *balance_sheet_subclass_omit_latex_net_income_row(
			int statement_prior_year_list_length,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

LATEX_ROW *balance_sheet_subclass_omit_latex_equity_end_row(
			int statement_prior_year_list_length,
			char *balance_sheet_end_balance_label,
			double balance_sheet_equity_end_balance );

LATEX_ROW *balance_sheet_subclass_omit_latex_liability_plus_equity_row(
			int statement_prior_year_list_length,
			char *balance_sheet_liability_plus_equity_label,
			double balance_sheet_liability_plus_equity );

typedef struct
{
	STATEMENT_SUBCLASS_AGGR_LATEX_LIST *
		statement_subclass_aggr_latex_list;
	LIST *row_list;
} BALANCE_SHEET_SUBCLASS_AGGR_LATEX;

/* Usage */
/* ----- */
BALANCE_SHEET_SUBCLASS_AGGR_LATEX *
	balance_sheet_subclass_aggr_latex_new(
			LIST *element_statement_list,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

/* Process */
/* ------- */
BALANCE_SHEET_SUBCLASS_AGGR_LATEX *
	balance_sheet_subclass_aggr_latex_calloc(
			void );

/* Usage */
/* ----- */
LIST *balance_sheet_subclass_aggr_latex_equity_row_list(
			int statement_prior_year_list_length,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

/* Process */
/* ------- */
LATEX_ROW *balance_sheet_subclass_aggr_latex_equity_begin_row(
			int statement_prior_year_list_length,
			char *balance_sheet_begin_balance_label,
			double element_equity_begin_sum );

LATEX_ROW *balance_sheet_subclass_aggr_latex_transaction_amount_row(
			int statement_prior_year_list_length,
			char *balance_sheet_transaction_amount_label,
			double balance_sheet_equity_transaction_amount );

LATEX_ROW *balance_sheet_subclass_aggr_latex_net_income_row(
			int statement_prior_year_list_length,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

LATEX_ROW *balance_sheet_subclass_aggr_latex_equity_end_row(
			int statement_prior_year_list_length,
			char *balance_sheet_end_balance_label,
			double balance_sheet_equity_end_balance );

LATEX_ROW *balance_sheet_subclass_aggr_latex_liability_plus_equity_row(
			int statement_prior_year_list_length,
			char *balance_sheet_liability_plus_equity_label,
			double balance_sheet_liability_plus_equity );

typedef struct
{
	BALANCE_SHEET_SUBCLASS_DISPLAY_LATEX *
		balance_sheet_subclass_display_latex;
	BALANCE_SHEET_SUBCLASS_OMIT_LATEX *
		balance_sheet_subclass_omit_latex;
	BALANCE_SHEET_SUBCLASS_AGGR_LATEX *
		balance_sheet_subclass_aggr_latex;
	LATEX *latex;
} BALANCE_SHEET_LATEX;

/* Usage */
/* ----- */
BALANCE_SHEET_LATEX *
	balance_sheet_latex_new(
			enum statement_subclassification_option
				statement_subclassification_option,
			char *tex_filename,
			char *dvi_filename,
			char *working_directory,
			boolean statement_pdf_landscape_boolean,
			char *statement_logo_filename,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			ELEMENT *element_liability,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label );

/* Process */
/* ------- */
BALANCE_SHEET_LATEX *
	balance_sheet_latex_calloc(
			void );

/* Usage */
/* ----- */
LATEX_TABLE *balance_sheet_latex_table(
			char *statement_caption,
			LIST *heading_list,
			LIST *row_list );

/* Process */
/* ------- */

typedef struct
{
	ELEMENT *element_liability;
	boolean statement_pdf_landscape_boolean;
	STATEMENT_LINK *statement_link;
	BALANCE_SHEET_LATEX *balance_sheet_latex;
} BALANCE_SHEET_PDF;

/* Usage */
/* ----- */
BALANCE_SHEET_PDF *balance_sheet_pdf_new(
			char *application_name,
			char *process_name,
			char *document_root_directory,
			enum statement_subclassification_option
				statement_subclassification_option,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			ELEMENT *element_equity_begin,
			ELEMENT *element_equity_current,
			double income_statement_fetch_net_income,
			char *income_statement_net_income_label,
			pid_t process_id );

/* Process */
/* ------- */
BALANCE_SHEET_PDF *balance_sheet_pdf_calloc(
			void );

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
	STATEMENT *statement;
	LIST *statement_prior_year_list;
	char *prior_date_time_closing;
	ELEMENT *element_equity_begin;
	ELEMENT *element_equity_current;
	double income_statement_fetch_net_income;
	char *income_statement_net_income_label;
	BALANCE_SHEET_PDF *balance_sheet_pdf;
	BALANCE_SHEET_HTML *balance_sheet_html;
} BALANCE_SHEET;

/* Usage */
/* ----- */
BALANCE_SHEET *balance_sheet_fetch(
			char *argv_0,
			char *application_name,
			char *session_key,
			char *login_name,
			char *role_name,
			char *process_name,
			char *document_root_directory,
			char *as_of_date,
			int prior_year_count,
			char *subclassification_option_string,
			char *output_medium_string );

/* Process */
/* ------- */
BALANCE_SHEET *balance_sheet_calloc(
			void );

LIST *balance_sheet_element_name_list(
			char *element_asset,
			char *element_liability,
			char *element_equity );

/* Usage */
/* ----- */
char *balance_sheet_prior_date_time_closing(
			char *transaction_begin_date_string );

/* Process */
/* ------- */

#endif
