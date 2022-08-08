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
#include "appaserver_link_file.h"
#include "latex.h"
#include "html_table.h"
#include "element.h"
#include "account.h"

#define STATEMENT_ROWS_BETWEEN_HEADING		10
#define STATEMENT_LOGO_FILENAME_KEY		"logo_filename"
#define STATEMENT_DAYS_FOR_EMPHASIS		35

enum statement_subclassification_option	{
			subclassification_aggregate,
			subclassification_display,
			subclassification_omit };

enum statement_output_medium 		{
			output_table,
			output_PDF,
			output_spreadsheet,
			output_stdout };

typedef struct
{
	/* Public attributes */
	/* ----------------- */
	double balance;
	char *balance_string;
	char *debit_string;
	char *credit_string;
	char *asset_percent_string;
	char *revenue_percent_string;
	char *percent_string;
	ACCOUNT *account;

	/* Private attributes */
	/* ------------------ */
	int date_days_between;
	boolean within_days_between_boolean;
} STATEMENT_ACCOUNT;

/* Usage */
/* ----- */

/* Always succeeds */
/* --------------- */
STATEMENT_ACCOUNT *statement_account_new(
			char *transaction_as_of_date,
			boolean element_accumulate_debit,
			ACCOUNT_JOURNAL *account_journal,
			char *account_action_string,
			boolean round_dollar_boolean,
			ACCOUNT *account );

/* Process */
/* ------- */
STATEMENT_ACCOUNT *statement_account_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *statement_account_asset_percent_string(
			int percent_of_asset );

/* Returns heap memory */
/* ------------------- */
char *statement_account_revenue_percent_string(
			int percent_of_revenue );

/* Returns heap memory */
/* ------------------- */
char *statement_account_percent_string(
			int percent_of_asset,
			int percent_of_revenue );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *statement_account_balance_string(
			double balance,
			char *account_action_string,
			boolean round_dollar_boolean );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *statement_account_money_string(
			double balance,
			boolean round_dollar_boolean );

typedef struct
{
	LIST *row_list;
} STATEMENT_SUBCLASS_DISPLAY_HTML;

/* Usage */
/* ----- */
STATEMENT_SUBCLASS_DISPLAY_HTML *
	statement_subclass_display_html_new(
			ELEMENT *element,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */
STATEMENT_SUBCLASS_DISPLAY_HTML *
	statement_subclass_display_html_calloc(
			void );

HTML_ROW *statement_subclass_display_html_element_label_row(
			char *element_name,
			int statement_prior_year_list_length );

HTML_ROW *statement_subclass_display_html_subclassification_label_row(
			char *subclassification_name,
			int statement_prior_year_list_length );


HTML_ROW *statement_subclass_display_html_subclassification_sum_row(
			char *subclassification_name,
			double sum,
			int percent_of_asset,
			int percent_of_revenue,
			LIST *statement_prior_year_list );

HTML_ROW *statement_subclass_display_html_element_sum_row(
			char *element_name,
			double sum,
			int percent_of_asset,
			int percent_of_revenue,
			LIST *statement_prior_year_list );

/* Usage */
/* ----- */
LIST *statement_subclass_display_html_account_row_list(
			LIST *account_statement_list,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
HTML_ROW *statement_subclass_display_html_account_row(
			STATEMENT_ACCOUNT *statement_account,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */
typedef struct
{
	LIST *heading_list;
	LIST *list;
} STATEMENT_SUBCLASS_DISPLAY_HTML_LIST;

/* Usage */
/* ----- */
STATEMENT_SUBCLASS_DISPLAY_HTML_LIST *
	statement_subclass_display_html_list_new(
			LIST *element_statement_list,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */
STATEMENT_SUBCLASS_DISPLAY_HTML_LIST *
	statement_subclass_display_html_list_calloc(
			void );

/* Usage */
/* ----- */
LIST *statement_subclass_display_html_list_heading_list(
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Pubic */
/* ------ */
LIST *statement_subclass_display_html_list_extract_row_list(
	STATEMENT_SUBCLASS_DISPLAY_HTML_LIST *
			statement_subclass_display_html_list );

typedef struct
{
	LIST *row_list;
} STATEMENT_SUBCLASS_OMIT_HTML;

/* Usage */
/* ----- */
STATEMENT_SUBCLASS_OMIT_HTML *
	statement_subclass_omit_html_new(
			ELEMENT *element,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */
STATEMENT_SUBCLASS_OMIT_HTML *
	statement_subclass_omit_html_calloc(
			void );

HTML_ROW *statement_subclass_omit_html_element_label_row(
			char *element_name,
			int statement_prior_year_list_length );

HTML_ROW *statement_subclass_omit_html_element_sum_row(
			char *element_name,
			double sum,
			int percent_of_asset,
			int percent_of_revenue,
			LIST *statement_prior_year_list );

/* Usage */
/* ----- */
HTML_ROW *statement_subclass_omit_html_account_row(
			STATEMENT_ACCOUNT *statement_account,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

typedef struct
{
	LIST *heading_list;
	LIST *list;
} STATEMENT_SUBCLASS_OMIT_HTML_LIST;

/* Usage */
/* ----- */
STATEMENT_SUBCLASS_OMIT_HTML_LIST *
	statement_subclass_omit_html_list_new(
			LIST *element_statement_list,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */
STATEMENT_SUBCLASS_OMIT_HTML_LIST *
	statement_subclass_omit_html_list_calloc(
			void );

/* Usage */
/* ----- */
LIST *statement_subclass_omit_html_list_heading_list(
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Pubic */
/* ------ */
LIST *statement_subclass_omit_html_list_extract_row_list(
	STATEMENT_SUBCLASS_OMIT_HTML_LIST *
			statement_subclass_omit_html_list );

typedef struct
{
	LIST *row_list;
} STATEMENT_SUBCLASS_DISPLAY_LATEX;

/* Usage */
/* ----- */
STATEMENT_SUBCLASS_DISPLAY_LATEX *
	statement_subclass_display_latex_new(
			ELEMENT *element,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */
STATEMENT_SUBCLASS_DISPLAY_LATEX *
	statement_subclass_display_latex_calloc(
			void );

LATEX_ROW *statement_subclass_display_latex_element_label_row(
			char *element_name,
			int statement_prior_year_list_length );

LATEX_ROW *statement_subclass_display_latex_subclassification_label_row(
			char *subclassification_name,
			int statement_prior_year_list_length );

LATEX_ROW *statement_subclass_display_latex_subclassification_sum_row(
			char *subclassification_name,
			double sum,
			int percent_of_asset,
			int percent_of_revenue,
			LIST *statement_prior_year_list );

LATEX_ROW *statement_subclass_display_latex_element_sum_row(
			char *element_name,
			double sum,
			int percent_of_asset,
			int percent_of_revenue,
			LIST *statement_prior_year_list );

/* Usage */
/* ----- */
LIST *statement_subclass_display_latex_account_row_list(
			LIST *account_statement_list,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LATEX_ROW *statement_subclass_display_latex_account_row(
			STATEMENT_ACCOUNT *statement_account,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

typedef struct
{
	LIST *heading_list;
	LIST *list;
} STATEMENT_SUBCLASS_DISPLAY_LATEX_LIST;

/* Usage */
/* ----- */
STATEMENT_SUBCLASS_DISPLAY_LATEX_LIST *
	statement_subclass_display_latex_list_new(
			LIST *element_statement_list,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */
STATEMENT_SUBCLASS_DISPLAY_LATEX_LIST *
	statement_subclass_display_latex_list_calloc(
			void );

/* Usage */
/* ----- */
LIST *statement_subclass_display_latex_list_heading_list(
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Public */
/* ------ */
LIST *statement_subclass_display_latex_list_extract_row_list(
	STATEMENT_SUBCLASS_DISPLAY_LATEX_LIST *
			statement_subclass_display_latex_list );

typedef struct
{
	LIST *row_list;
} STATEMENT_SUBCLASS_OMIT_LATEX;

/* Usage */
/* ----- */
STATEMENT_SUBCLASS_OMIT_LATEX *
	statement_subclass_omit_latex_new(
			ELEMENT *element,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */
STATEMENT_SUBCLASS_OMIT_LATEX *
	statement_subclass_omit_latex_calloc(
			void );

LATEX_ROW *statement_subclass_omit_latex_element_label_row(
			char *element_name,
			int statement_prior_year_list_length );

LATEX_ROW *statement_subclass_omit_latex_element_sum_row(
			char *element_name,
			double sum,
			int percent_of_asset,
			int percent_of_revenue,
			LIST *statement_prior_year_list );

/* Usage */
/* ----- */
LATEX_ROW *statement_subclass_omit_latex_account_row(
			STATEMENT_ACCOUNT *statement_account,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

typedef struct
{
	LIST *heading_list;
	LIST *list;
} STATEMENT_SUBCLASS_OMIT_LATEX_LIST;

/* Usage */
/* ----- */
STATEMENT_SUBCLASS_OMIT_LATEX_LIST *
	statement_subclass_omit_latex_list_new(
			LIST *element_statement_list,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */
STATEMENT_SUBCLASS_OMIT_LATEX_LIST *
	statement_subclass_omit_latex_list_calloc(
			void );

LIST *statement_subclass_omit_latex_list_heading_list(
			LIST *statement_prior_year_list );

typedef struct
{
	LIST *element_statement_list;
	char *logo_filename;
	char *caption_title;
	char *caption_subtitle;
	char *caption;
	char *date_time_string;
	char *frame_title;
	char *date_american;
	char *transaction_begin_date_string;
	char *transaction_as_of_date;
	char *transaction_date_time_closing;
} STATEMENT;

/* Usage */
/* ----- */
STATEMENT *statement_fetch(
			char *application_name,
			char *process_name,
			LIST *element_name_list,
			char *transaction_begin_date_string,
			char *transaction_as_of_date,
			char *transaction_date_time_closing,
			boolean fetch_transaction );

/* Process */
/* ------- */
STATEMENT *statement_calloc(
			void );

/* Returns heap memory or null */
/* --------------------------- */
char *statement_logo_filename(
			char *application_name,
			char *statement_logo_filename_key );

/* Returns static memory */
/* --------------------- */
char *statement_caption_title(
			char *application_name,
			boolean exists_logo_filename,
			char *process_name );

/* Returns static memory or null */
/* ----------------------------- */
char *statement_caption_subtitle(
			char *begin_date_string,
			char *as_of_date );

/* Returns heap memory */
/* ------------------- */
char *statement_caption(
			char *statement_caption_title,
			char *statement_caption_subtitle );

/* Returns date_time_string */
/* ------------------------ */
char *statement_date_time_string(
			char *date_time_string );

/* Returns static memory */
/* --------------------- */
char *statement_frame_title(
			char *process_name,
			char *statement_date_time_string );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *statement_date_american(
			char *date_time_string );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *statement_latex_account_title(
			char *account_name );

/* Public */
/* ------ */
enum statement_subclassification_option
	statement_resolve_subclassification_option(
			char *subclassification_option_string );

enum statement_output_medium
	statement_resolve_output_medium(
			char *output_medium_string );

boolean statement_pdf_landscape_boolean(
			int statement_prior_year_list_length );

/* Returns heap memory or "" */
/* ------------------------- */
char *statement_cell_data_label(
			char *name );

/* Returns static memory */
/* --------------------- */
char *statement_pdf_prompt(
			char *process_name );

/* Driver */
/* ------ */
void statement_latex_output(
			LATEX *latex,
			char *ftp_output_filename,
			char *prompt,
			char *process_name,
			char *statement_date_time_string );

/* Driver */
/* ------ */
void statement_html_output(
			HTML_TABLE *html_table,
			char *secondary_title );

/* Process */
/* ------- */

typedef struct
{
	char *date_string;
	LIST *element_statement_list;
} STATEMENT_PRIOR_YEAR;

/* Usage */
/* ----- */
LIST *statement_prior_year_list(
			LIST *filter_element_name_list,
			char *transaction_date_time_closing,
			int prior_year_count,
			STATEMENT *statement );

/* Process */
/* ------- */

/* Usage */
/* ----- */
STATEMENT_PRIOR_YEAR *statement_prior_year_fetch(
			LIST *filter_element_name_list,
			char *transaction_date_time_closing,
			int years_ago,
			STATEMENT *statement );

/* Process */
/* ------- */
STATEMENT_PRIOR_YEAR *statement_prior_year_calloc(
			void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *statement_prior_year_date_string(
			char *transaction_date_time_closing,
			int years_ago );

/* Process */
/* ------- */

/* Usage */
/* ----- */
LIST *statement_prior_year_account_data_list(
			char *account_name,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *statement_prior_year_account_data(
			ACCOUNT *prior_account );

/* Usage */
/* ----- */
LIST *statement_prior_year_subclassification_data_list(
			char *subclassification_name,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *statement_prior_year_subclassification_data(
			SUBCLASSIFICATION *prior_subclassification );

/* Usage */
/* ----- */
LIST *statement_prior_year_element_data_list(
			char *element_name,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *statement_prior_year_element_data(
			ELEMENT *prior_element );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *statement_prior_year_cell_display(
			boolean cell_empty,
			int delta_prior_percent,
			double prior_year_amount );

/* Process */
/* ------- */

/* Public */
/* ------ */
LIST *statement_prior_year_heading_list(
			LIST *statement_prior_year_list );

typedef struct
{
	char *filename_stem;
	APPASERVER_LINK_FILE *appaserver_link_file;
	char *tex_filename;
	char *dvi_filename;
	char *ftp_output_filename;
	char *working_directory;
} STATEMENT_LINK;

/* Usage */
/* ----- */
STATEMENT_LINK *statement_link_new(
			char *application_name,
			char *process_name,
			char *document_root_directory,
			char *transaction_begin_date_string,
			char *transaction_as_of_date,
			char *preclose_key,
			pid_t process_id );

/* Process */
/* ------- */
STATEMENT_LINK *statement_link_calloc(
			void );

/* Returns heap memory or process_name */
/* ----------------------------------- */
char *statement_link_filename_stem(
			char *process_name,
			char *preclose_key );

#endif
