/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/income_statement.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
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
#include "html_table.h"
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
			char *net_income_percent_of_revenue_display,
			char *net_income_label );

/* Process */
/* ------- */
INCOME_STATEMENT_SUBCLASS_DISPLAY_HTML *
	income_statement_subclass_display_html_calloc(
			void );

/* Usage */
/* ----- */
HTML_ROW *income_statement_subclass_display_html_net_income_row(
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *net_income_label );

/* Process */
/* ------- */

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
			char *net_income_percent_of_revenue_display,
			char *net_income_label );

/* Process */
/* ------- */
INCOME_STATEMENT_SUBCLASS_OMIT_HTML *
	income_statement_subclass_omit_html_calloc(
			void );

/* Usage */
/* ----- */
HTML_ROW *income_statement_subclass_omit_html_net_income_row(
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *net_income_label );

/* Process */
/* ------- */

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
			char *net_income_percent_of_revenue_display,
			char *net_income_label );

/* Process */
/* ------- */
INCOME_STATEMENT_SUBCLASS_AGGR_HTML *
	income_statement_subclass_aggr_html_calloc(
			void );

/* Usage */
/* ----- */
HTML_ROW *income_statement_subclass_aggr_html_net_income_row(
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *net_income_label );

/* Process */
/* ------- */

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
			char *net_income_percent_of_revenue_display,
			char *net_income_label );

/* Process */
/* ------- */
INCOME_STATEMENT_HTML *
	income_statement_html_calloc(
			void );

/* Usage */
/* ----- */
HTML_TABLE *income_statement_html_table(
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
} INCOME_STATEMENT_SUBCLASS_DISPLAY_LATEX;

/* Usage */
/* ----- */
INCOME_STATEMENT_SUBCLASS_DISPLAY_LATEX *
	income_statement_subclass_display_latex_new(
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *income_statement_net_income_label );

/* Process */
/* ------- */
INCOME_STATEMENT_SUBCLASS_DISPLAY_LATEX *
	income_statement_subclass_display_latex_calloc(
			void );

/* Usage */
/* ----- */
LATEX_ROW *income_statement_subclass_display_latex_net_income_row(
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *income_statement_net_income_label );

/* Process */
/* ------- */

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
			char *net_income_percent_of_revenue_display,
			char *income_statement_net_income_label );

/* Process */
/* ------- */
INCOME_STATEMENT_SUBCLASS_OMIT_LATEX *
	income_statement_subclass_omit_latex_calloc(
			void );

/* Usage */
/* ----- */
LATEX_ROW *income_statement_subclass_omit_latex_net_income_row(
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *income_statement_net_income_label );

/* Process */
/* ------- */

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
			char *net_income_percent_of_revenue_display,
			char *income_statement_net_income_label );

/* Process */
/* ------- */
INCOME_STATEMENT_SUBCLASS_AGGR_LATEX *
	income_statement_subclass_aggr_latex_calloc(
			void );

/* Usage */
/* ----- */
LATEX_ROW *income_statement_subclass_aggr_latex_net_income_row(
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *income_statement_net_income_label );

/* Process */
/* ------- */

typedef struct
{
	INCOME_STATEMENT_SUBCLASS_DISPLAY_LATEX *
		income_statement_subclass_display_latex;

	INCOME_STATEMENT_SUBCLASS_OMIT_LATEX *
		income_statement_subclass_omit_latex;

	INCOME_STATEMENT_SUBCLASS_AGGR_LATEX *
		income_statement_subclass_aggr_latex;

	LATEX *latex;
} INCOME_STATEMENT_LATEX;

/* Usage */
/* ----- */
INCOME_STATEMENT_LATEX *
	income_statement_latex_new(
			enum statement_subclassification_option
				statement_subclassification_option,
			char *tex_filename,
			char *dvi_filename,
			char *working_directory,
			boolean statement_pdf_landscape_boolean,
			char *statement_logo_filename,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *income_statement_net_income_label );

/* Process */
/* ------- */
INCOME_STATEMENT_LATEX *
	income_statement_latex_calloc(
			void );

/* Usage */
/* ----- */
LATEX_TABLE *income_statement_latex_table(
			char *statement_caption,
			LIST *heading_list,
			LIST *row_list );

/* Process */
/* ------- */

typedef struct
{
	boolean statement_pdf_landscape_boolean;
	STATEMENT_LINK *statement_link;
	INCOME_STATEMENT_LATEX *income_statement_latex;
} INCOME_STATEMENT_PDF;

/* Usage */
/* ----- */
INCOME_STATEMENT_PDF *income_statement_pdf_new(
			char *application_name,
			char *process_name,
			char *document_root_directory,
			enum statement_subclassification_option
				statement_subclassification_option,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double element_net_income,
			char *net_income_percent_of_revenue_display,
			char *income_statement_net_income_label,
			pid_t process_id );

/* Process */
/* ------- */
INCOME_STATEMENT_PDF *income_statement_pdf_calloc(
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
	double element_net_income;
	char *net_income_percent_of_revenue_display;
	char *net_income_label;
	INCOME_STATEMENT_PDF *income_statement_pdf;
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
			char *document_root_directory,
			char *as_of_date,
			int prior_year_count,
			char *subclassifiction_option_string,
			char *output_medium_string );

/* Process */
/* ------- */
INCOME_STATEMENT *income_statement_calloc(
			void );

LIST *income_statement_element_name_list(
			char *element_revenue,
			char *element_expense,
			char *element_gain,
			char *element_loss );

/* Returns heap memory */
/* ------------------- */
char *income_statement_net_income_percent_of_revenue_display(
			char *element_revenue,
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

/* Process */
/* ------- */

#endif
