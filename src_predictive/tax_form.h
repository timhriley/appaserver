/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/tax_form.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef TAX_FORM_H
#define TAX_FORM_H

#include "list.h"
#include "boolean.h"
#include "statement.h"
#include "html.h"
#include "latex.h"
#include "journal.h"
#include "entity.h"

#define TAX_FORM_LINE_SELECT		"tax_form_line,"	\
					"tax_form_description"

#define TAX_FORM_LINE_TABLE		"tax_form_line"

typedef struct
{
	char *tax_form_name;
	char *string;
	char *description;
	LIST *tax_form_line_account_list;
	double total;
} TAX_FORM_LINE;

/* Usage */
/* ----- */
LIST *tax_form_line_list(
		char *tax_form_name,
		char *tax_form_fiscal_begin_date,
		char *tax_form_fiscal_end_date );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *tax_form_line_system_string(
		char *tax_form_line_select,
		char *tax_form_line_table,
		char *tax_form_primary_where );

/* Usage */
/* ----- */
LIST *tax_form_line_system_list(
		char *tax_form_name,
		char *tax_form_fiscal_begin_date,
		char *tax_form_fiscal_end_date,
		char *tax_form_line_system_string );

/* Process */
/* ------- */
FILE *tax_form_line_input_pipe(
		char *tax_form_line_system_string );

TAX_FORM_LINE *tax_form_line_parse(
		char *tax_form_name,
		char *tax_form_fiscal_begin_date,
		char *tax_form_fiscal_end_date,
		char *input );

/* Process */
/* ------- */
TAX_FORM_LINE *tax_form_line_calloc(
		void );

double tax_form_line_total(
	LIST *tax_form_line_account_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *tax_form_line_where(
		char *tax_form_name,
		char *tax_form_line_string );

typedef struct
{
	JOURNAL *journal;
	double amount;
} TAX_FORM_LINE_ACCOUNT_JOURNAL;

/* Usage */
/* ----- */
LIST *tax_form_line_account_journal_list(
		char *tax_form_fiscal_begin_date,
		char *tax_form_fiscal_end_date,
		const char *transaction_date_preclose_time,
		char *account_name,
		boolean element_accumulate_debit,
		boolean subclassification_current_liability_boolean,
		boolean subclassification_receivable_boolean );

/* Usage */
/* ----- */
TAX_FORM_LINE_ACCOUNT_JOURNAL *
	tax_form_line_account_journal_new(
		boolean element_accumulate_debit,
		boolean subclassification_current_liability_boolean,
		boolean subclassification_receivable_boolean,
		JOURNAL *journal );

/* Process */
/* ------- */
TAX_FORM_LINE_ACCOUNT_JOURNAL *
	tax_form_line_account_journal_calloc(
		void );

/* Usage */
/* ----- */
double tax_form_line_account_journal_amount(
		boolean element_accumulate_debit,
		boolean subclassification_current_liability_boolean,
		boolean subclassification_receivable_boolean,
		double debit_amount,
		double credit_amount );

/* Usage */
/* ----- */
double tax_form_line_account_journal_list_total(
		LIST *tax_form_line_account_journal_list );

#define TAX_FORM_LINE_ACCOUNT_SELECT	"account"
#define TAX_FORM_LINE_ACCOUNT_TABLE	"tax_form_line_account"

typedef struct
{
	char *tax_form_name;
	char *tax_form_line_string;
	char *account_name;
	ACCOUNT *account;
	boolean subclassification_current_liability_boolean;
	boolean subclassification_receivable_boolean;
	LIST *journal_list;
	double journal_list_total;
	LIST *tax_form_entity_list;
} TAX_FORM_LINE_ACCOUNT;

/* Usage */
/* ----- */
LIST *tax_form_line_account_list(
		char *tax_form_name,
		char *tax_form_fiscal_begin_date,
		char *tax_form_fiscal_end_date,
		char *tax_form_line_string );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *tax_form_line_account_system_string(
		char *tax_form_line_account_select,
		char *tax_form_line_account_table,
		char *tax_form_line_where );

/* Usage */
/* ----- */
LIST *tax_form_line_account_system_list(
		char *tax_form_name,
		char *tax_form_fiscal_begin_date,
		char *tax_form_fiscal_end_date,
		char *tax_form_line_string,
		char *tax_form_line_account_system_string );

/* Process */
/* ------- */
FILE *tax_form_line_account_input_pipe(
		char *tax_form_line_account_system_string );

/* Usage */
/* ----- */
TAX_FORM_LINE_ACCOUNT *tax_form_line_account_parse(
		char *tax_form_name,
		char *tax_form_fiscal_begin_date,
		char *tax_form_fiscal_end_date,
		char *tax_form_line_string,
		char *input );

/* Process */
/* ------- */
TAX_FORM_LINE_ACCOUNT *tax_form_line_account_calloc(
		void );

double tax_form_line_account_total(
		LIST *journal_tax_form_list );

typedef struct
{
	ENTITY *entity;
	double total;
} TAX_FORM_ENTITY;

/* Usage */
/* ----- */
LIST *tax_form_entity_list(
		LIST *journal_tax_form_list );

/* Usage */
/* ----- */
void tax_form_entity_getset(
		LIST *list,
		char *full_name,
		double journal_amount );

/* Process */
/* ------- */
TAX_FORM_ENTITY *tax_form_entity_calloc(
		void );

typedef struct
{
	char *caption;
	HTML_TABLE *html_table;
} TAX_FORM_LINE_HTML_TABLE;

/* Usage */
/* ----- */
TAX_FORM_LINE_HTML_TABLE *tax_form_line_html_table_new(
		char *tax_form_name,
		char *statement_caption_sub_title,
		LIST *tax_form_line_list );

/* Process */
/* ------- */
TAX_FORM_LINE_HTML_TABLE *tax_form_line_html_table_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *tax_form_line_html_table_caption(
		char *tax_form_name,
		char *statement_caption_sub_title );

/* Usage */
/* ----- */
LIST *tax_form_line_html_table_column_list(
		void );

/* Usage */
/* ----- */
LIST *tax_form_line_html_table_row_list(
		LIST *tax_form_line_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
HTML_ROW *tax_form_line_html_table_row(
		TAX_FORM_LINE *tax_form_line );

typedef struct
{
	/* stub */
} TAX_FORM_ACCOUNT_HTML;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
HTML_TABLE *tax_form_account_html_table(
		char *tax_form_line_string,
		LIST *tax_form_line_account_list,
		char *tax_form_line_description,
		double tax_form_line_total );

/* Returns heap memory */
/* ------------------- */
char *tax_form_account_html_caption(
		char *tax_form_line_string,
		char *tax_form_line_description,
		double tax_form_line_total );

/* Usage */
/* ----- */
LIST *tax_form_account_html_column_list(
		void );

/* Usage */
/* ----- */
LIST *tax_form_account_html_row_list(
		LIST *tax_form_line_account_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
HTML_ROW *tax_form_line_account_html_row(
		TAX_FORM_LINE_ACCOUNT *tax_form_line_account );

typedef struct
{
	LIST *html_table_list;
} TAX_FORM_ACCOUNT_HTML_LIST;

/* Usage */
/* ----- */
TAX_FORM_ACCOUNT_HTML_LIST *tax_form_account_html_list_new(
		LIST *tax_form_line_list );

/* Process */
/* ------- */
TAX_FORM_ACCOUNT_HTML_LIST *tax_form_account_html_list_calloc(
		void );

typedef struct
{
	/* stub */
} TAX_FORM_ENTITY_HTML;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
HTML_TABLE *tax_form_entity_html_table(
		char *tax_form_line_string,
		char *account_name,
		double journal_list_totald,
		LIST *tax_form_entity_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *tax_form_entity_html_caption(
		char *tax_form_line_string,
		char *account_name,
		double journal_list_total );


/* Usage */
/* ----- */
LIST *tax_form_entity_html_column_list(
		void );

/* Usage */
/* ----- */
LIST *tax_form_entity_html_row_list(
		LIST *tax_form_entity_list );

/* Usage */
/* ----- */
HTML_ROW *tax_form_entity_html_row(
		TAX_FORM_ENTITY *tax_form_entity );

typedef struct
{
	LIST *html_table_list;
} TAX_FORM_ENTITY_HTML_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
TAX_FORM_ENTITY_HTML_LIST *tax_form_entity_html_list_new(
		LIST *tax_form_line_list );

/* Process */
/* ------- */
TAX_FORM_ENTITY_HTML_LIST *tax_form_entity_html_list_calloc(
		void );

typedef struct
{
	LIST *html_table_list;
	TAX_FORM_LINE_HTML_TABLE *tax_form_line_html_table;
	TAX_FORM_ACCOUNT_HTML_LIST *tax_form_account_html_list;
	TAX_FORM_ENTITY_HTML_LIST *tax_form_entity_html_list;
} TAX_FORM_TABLE;

/* Usage */
/* ----- */
TAX_FORM_TABLE *tax_form_table_new(
		char *tax_form_name,
		STATEMENT_CAPTION *statement_caption,
		LIST *tax_form_line_list );

/* Process */
/* ------- */
TAX_FORM_TABLE *tax_form_table_calloc(
			void );

typedef struct
{
	LATEX_TABLE *latex_table;
} TAX_FORM_LINE_LATEX_TABLE;

/* Usage */
/* ----- */
TAX_FORM_LINE_LATEX_TABLE *
	tax_form_line_latex_table_new(
		char *tax_form_name,
		char *statement_caption_sub_title,
		LIST *tax_form_line_list );

/* Process */
/* ------- */
TAX_FORM_LINE_LATEX_TABLE *
	tax_form_line_latex_table_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *tax_form_line_latex_table_caption(
		char *tax_form_name,
		char *statement_caption_sub_title );

/* Usage */
/* ----- */
LIST *tax_form_line_latex_table_column_list(
		void );

/* Usage */
/* ----- */
LIST *tax_form_line_latex_table_row_list(
		LIST *tax_form_line_list,
		LIST *tax_form_line_latex_table_column_list );

/* Usage */
/* ----- */
LATEX_ROW *tax_form_line_latex_table_row(
		TAX_FORM_LINE *tax_form_line,
		LIST *latex_column_list );

typedef struct
{
	/* stub */
} TAX_FORM_ENTITY_LATEX;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LATEX_TABLE *tax_form_entity_latex_table(
		char *tax_form_line_string,
		char *account_name,
		double journal_list_total,
		LIST *tax_form_entity_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *tax_form_entity_latex_caption(
		char *tax_form_line_string,
		char *account_name,
		double journal_list_total );

/* Usage */
/* ----- */
LIST *tax_form_entity_latex_column_list(
		void );

/* Usage */
/* ----- */
LIST *tax_form_entity_latex_row_list(
		LIST *tax_form_entity_list,
		LIST *latex_column_list );

/* Usage */
/* ----- */
LATEX_ROW *tax_form_entity_latex_row(
		TAX_FORM_ENTITY *tax_form_entity,
		LIST *latex_column_list );

typedef struct
{
	LIST *latex_table_list;
} TAX_FORM_ENTITY_LATEX_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
TAX_FORM_ENTITY_LATEX_LIST *
	tax_form_entity_latex_list_new(
		LIST *tax_form_line_list );

/* Process */
/* ------- */
TAX_FORM_ENTITY_LATEX_LIST *
	tax_form_entity_latex_list_calloc(
		void );

typedef struct
{
	/* stub */
} TAX_FORM_ACCOUNT_LATEX;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
LATEX_TABLE *tax_form_account_latex_table(
		char *tax_form_line_string,
		LIST *tax_form_line_account_list,
		char *tax_form_line_description,
		double tax_form_line_total );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *tax_form_account_latex_caption(
		char *tax_form_line_string,
		char *tax_form_line_description,
		double tax_form_line_total );

/* Usage */
/* ----- */
LIST *tax_form_account_latex_column_list(
		void );

/* Usage */
/* ----- */
LIST *tax_form_account_latex_row_list(
		LIST *tax_form_line_account_list,
		LIST *tax_form_account_latex_column_list );

/* Usage */
/* ----- */
LATEX_ROW *tax_form_line_account_latex_row(
		TAX_FORM_LINE_ACCOUNT *tax_form_line_account,
		LIST *latex_column_list );

typedef struct
{
	LIST *latex_table_list;
} TAX_FORM_ACCOUNT_LATEX_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
TAX_FORM_ACCOUNT_LATEX_LIST *
	tax_form_account_latex_list_new(
		LIST *tax_form_line_list );


/* Process */
/* ------- */
TAX_FORM_ACCOUNT_LATEX_LIST *
	tax_form_account_latex_list_calloc(
		void );

typedef struct
{
	STATEMENT_LINK *statement_link;
	LATEX *latex;
	LIST *latex_table_list;
	TAX_FORM_LINE_LATEX_TABLE *tax_form_line_latex_table;
	TAX_FORM_ACCOUNT_LATEX_LIST *tax_form_account_latex_list;
	TAX_FORM_ENTITY_LATEX_LIST *tax_form_entity_latex_list;
} TAX_FORM_LATEX;

/* Usage */
/* ----- */
TAX_FORM_LATEX *tax_form_latex_new(
		char *application_name,
		char *process_name,
		char *appaserver_parameter_data_directory,
		char *tax_form_name,
		STATEMENT_CAPTION *statement_caption,
		LIST *tax_form_line_list );

/* Process */
/* ------- */
TAX_FORM_LATEX *tax_form_latex_calloc(
		void );

typedef struct
{
	enum statement_output_medium statement_output_medium;
	char *fiscal_begin_date;
	char *fiscal_end_date;
	STATEMENT_CAPTION *statement_caption;
	LIST *tax_form_line_list;
	TAX_FORM_TABLE *tax_form_table;
	TAX_FORM_LATEX *tax_form_latex;
} TAX_FORM;

/* Usage */
/* ----- */
TAX_FORM *tax_form_fetch(
		char *application_name,
		char *process_name,
		char *appaserver_parameter_data_directory,
		char *tax_form_name,
		int tax_year,
		int fiscal_begin_month /* zero or one based */,
		char *output_medium_string );

/* Process */
/* ------- */
TAX_FORM *tax_form_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *tax_form_fiscal_begin_date(
		int tax_year,
		int fiscal_begin_month );

/* Returns static memory */
/* --------------------- */
char *tax_form_fiscal_end_date(
		int tax_year,
		int fiscal_begin_month );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *tax_form_primary_where(
		char *tax_form_name );

#endif
