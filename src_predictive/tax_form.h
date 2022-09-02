/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/tax_form.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef TAX_FORM_H
#define TAX_FORM_H

#include "list.h"
#include "boolean.h"
#include "statement.h"
#include "html_table.h"
#include "latex.h"
#include "journal.h"
#include "entity.h"

#define TAX_FORM_LINE_SELECT		"tax_form_line,"	\
					"tax_form_description"

#define TAX_FORM_LINE_TABLE		"tax_form_line"

typedef struct
{
	char *tax_form_name;
	char *tax_form_line_string;
	char *tax_form_description;
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

/* Public */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *tax_form_line_where(
			char *tax_form_name,
			char *tax_form_line_string );

#define TAX_FORM_LINE_ACCOUNT_SELECT	"account"
#define TAX_FORM_LINE_ACCOUNT_TABLE	"tax_form_line_account"

typedef struct
{
	char *tax_form_name;
	char *tax_form_line_string;
	char *account_name;
	LIST *journal_tax_form_list;
	double total;
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
	double balance;
} TAX_FORM_ENTITY;

/* Usage */
/* ----- */
LIST *tax_form_entity_list(
			LIST *journal_tax_form_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
void tax_form_entity_getset(
			LIST *list,
			char *full_name,
			char *street_address,
			double journal_balance );

/* Process */
/* ------- */
TAX_FORM_ENTITY *tax_form_entity_calloc(
			void );

typedef struct
{
} TAX_FORM_TABLE;

/* Usage */
/* ----- */
TAX_FORM_TABLE *tax_form_table_new(
			STATEMENT_CAPTION *statement_caption,
			LIST *tax_form_line_list );

/* Process */
/* ------- */

typedef struct
{
	STATEMENT_LINK *statement_link;
	LATEX *latex;
} TAX_FORM_PDF;

/* Usage */
/* ----- */
TAX_FORM_PDF *tax_form_pdf_new(
			char *application_name,
			char *process_name,
			char *document_root_directory,
			char *tax_form_name,
			STATEMENT_CAPTION *statement_caption,
			LIST *tax_form_line_list );

/* Process */
/* ------- */
TAX_FORM_PDF *tax_form_pdf_calloc(
			void );

typedef struct
{
	enum statement_output_medium statement_output_medium;
	char *fiscal_begin_date;
	char *fiscal_end_date;
	STATEMENT_CAPTION *statement_caption;
	LIST *tax_form_line_list;
	TAX_FORM_TABLE *tax_form_table;
	TAX_FORM_PDF *tax_form_PDF;
} TAX_FORM;

/* Usage */
/* ----- */
TAX_FORM *tax_form_fetch(
			char *application_name,
			char *process_name,
			char *document_root_directory,
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

/* Public */
/* ------ */

/* Returns static memory */
/* --------------------- */
char *tax_form_primary_where(
			char *tax_form_name );

#endif
