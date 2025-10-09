/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/statement.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef STATEMENT_H
#define STATEMENT_H

#include <stdio.h>
#include <unistd.h>
#include "list.h"
#include "boolean.h"
#include "appaserver_link.h"
#include "latex.h"
#include "html.h"
#include "element.h"
#include "account.h"

#define STATEMENT_ROWS_BETWEEN_HEADING		10
#define STATEMENT_LOGO_FILENAME_KEY		"logo_filename"
#define STATEMENT_DAYS_FOR_EMPHASIS		35

#define STATEMENT_ACCOUNT_HEADING		"Account"
#define STATEMENT_ELEMENT_HEADING		"Element"
#define STATEMENT_SUBCLASSIFICATION_HEADING	"Subclassification"
#define STATEMENT_STANDARDIZED_HEADING		"Total%"
#define STATEMENT_STANDARDIZED_ESCAPED_HEADING	"Total\\%"
#define STATEMENT_BALANCE_HEADING		"Balance"
#define STATEMENT_ANNUALIZED_HEADING		"Annualized"
#define STATEMENT_BUDGET_HEADING		"Budget"
#define STATEMENT_DIFFERENCE_HEADING		"Difference"
#define STATEMENT_ROW_COUNT_HEADING		"RowCount"
#define STATEMENT_COUNT_HEADING			"Count"
#define STATEMENT_CONFIDENCE_HEADING		"Confidence"
#define STATEMENT_DEBIT_HEADING			"Debit"
#define STATEMENT_CREDIT_HEADING		"Credit"
#define STATEMENT_PERCENT_OF_ASSET_HEADING	"percent_of_asset"
#define STATEMENT_PERCENT_OF_INCOME_HEADING	"percent_of_income"

#define STATEMENT_GREATER_YEAR_MESSAGE		\
	"<h3>Warning: the date range exceeds a "\
	"year. Consider executing:"		\
	"<br>Alter --> Close Nominal Accounts"	\
	"<br>Use As Of Date = December 31st."	\
	"</h3>"

enum statement_subclassification_option {
		statement_subclassification_aggregate,
		statement_subclassification_display,
		statement_subclassification_omit };

enum statement_output_medium {
		statement_output_table,
		statement_output_PDF,
		statement_output_spreadsheet,
		statement_output_stdout };

typedef struct
{
	double balance;
	char *balance_string;
	char *debit_string;
	char *credit_string;
	char *asset_percent_string;
	char *revenue_percent_string;
	char *percent_string;
	ACCOUNT *account;
	int date_days_between;
	boolean within_days_between_boolean;
} STATEMENT_ACCOUNT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
STATEMENT_ACCOUNT *statement_account_new(
		char *end_date_time,
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
		int percent_of_income );

/* Returns heap memory */
/* ------------------- */
char *statement_account_percent_string(
		int percent_of_asset,
		int percent_of_income );

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

/* Usage */
/* ----- */
HTML_ROW *statement_subclass_display_html_element_label_row(
		char *element_name,
		int statement_prior_year_list_length );

/* Usage */
/* ----- */
HTML_ROW *statement_subclass_display_html_subclassification_label_row(
		char *subclassification_name,
		int statement_prior_year_list_length );


/* Usage */
/* ----- */
HTML_ROW *statement_subclass_display_html_subclassification_sum_row(
		char *subclassification_name,
		double sum,
		int percent_of_asset,
		int percent_of_income,
		LIST *statement_prior_year_list );

/* Usage */
/* ----- */
HTML_ROW *statement_subclass_display_html_element_sum_row(
		char *element_name,
		double sum,
		int percent_of_asset,
		int percent_of_income,
		LIST *statement_prior_year_list );

/* Usage */
/* ----- */
LIST *statement_subclass_display_html_account_row_list(
		LIST *account_statement_list,
		LIST *statement_prior_year_list );

/* Usage */
/* ----- */
HTML_ROW *statement_subclass_display_html_account_row(
		STATEMENT_ACCOUNT *statement_account,
		LIST *statement_prior_year_list );

typedef struct
{
	LIST *column_list;
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
LIST *statement_subclass_display_html_list_column_list(
		LIST *statement_prior_year_list );

/* Usage */
/* ----- */
LIST *statement_subclass_display_html_list_row_list(
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

/* Usage */
/* ----- */
HTML_ROW *statement_subclass_omit_html_element_label_row(
		char *element_name,
		int statement_prior_year_list_length );

/* Usage */
/* ----- */
HTML_ROW *statement_subclass_omit_html_element_sum_row(
		char *element_name,
		double sum,
		int percent_of_asset,
		int percent_of_income,
		LIST *statement_prior_year_list );

/* Usage */
/* ----- */
HTML_ROW *statement_subclass_omit_html_row(
		STATEMENT_ACCOUNT *statement_account,
		LIST *statement_prior_year_list );

typedef struct
{
	LIST *column_list;
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
LIST *statement_subclass_omit_html_list_column_list(
		LIST *statement_prior_year_list );

/* Usage */
/* ----- */
LIST *statement_subclass_omit_html_list_row_list(
	STATEMENT_SUBCLASS_OMIT_HTML_LIST *
		statement_subclass_omit_html_list );

typedef struct
{
	LIST *row_list;
} STATEMENT_SUBCLASS_AGGR_HTML;

/* Usage */
/* ----- */
STATEMENT_SUBCLASS_AGGR_HTML *
	statement_subclass_aggr_html_new(
		ELEMENT *element,
		LIST *statement_prior_year_list );

/* Process */
/* ------- */
STATEMENT_SUBCLASS_AGGR_HTML *
	statement_subclass_aggr_html_calloc(
		void );

/* Usage */
/* ----- */
HTML_ROW *statement_subclass_aggr_html_element_label_row(
		char *element_name,
		int statement_prior_year_list_length );

/* Usage */
/* ----- */
HTML_ROW *statement_subclass_aggr_html_element_sum_row(
		char *element_name,
		double sum,
		int percent_of_asset,
		int percent_of_income,
		LIST *statement_prior_year_list );

/* Usage */
/* ----- */
HTML_ROW *statement_subclass_aggr_html_row(
		SUBCLASSIFICATION *subclassification,
		LIST *statement_prior_year_list );

typedef struct
{
	LIST *column_list;
	LIST *list;
} STATEMENT_SUBCLASS_AGGR_HTML_LIST;

/* Usage */
/* ----- */
STATEMENT_SUBCLASS_AGGR_HTML_LIST *
	statement_subclass_aggr_html_list_new(
		LIST *element_statement_list,
		LIST *statement_prior_year_list );

/* Process */
/* ------- */
STATEMENT_SUBCLASS_AGGR_HTML_LIST *
	statement_subclass_aggr_html_list_calloc(
		void );

/* Usage */
/* ----- */
LIST *statement_subclass_aggr_html_list_column_list(
		LIST *statement_prior_year_list );

/* Usage */
/* ----- */
LIST *statement_subclass_aggr_html_list_row_list(
	STATEMENT_SUBCLASS_AGGR_HTML_LIST *
		statement_subclass_aggr_html_list );

typedef struct
{
	LIST *row_list;
} STATEMENT_SUBCLASS_DISPLAY_LATEX;

/* Usage */
/* ----- */
STATEMENT_SUBCLASS_DISPLAY_LATEX *
	statement_subclass_display_latex_new(
		ELEMENT *element,
		LIST *statement_prior_year_list,
		LIST *latex_column_list );

/* Process */
/* ------- */
STATEMENT_SUBCLASS_DISPLAY_LATEX *
	statement_subclass_display_latex_calloc(
		void );

/* Usage */
/* ----- */
LATEX_ROW *statement_subclass_display_latex_element_label_row(
		char *element_name,
		int statement_prior_year_list_length,
		LIST *latex_column_list );

/* Usage */
/* ----- */
LATEX_ROW *statement_subclass_display_latex_subclassification_label_row(
		char *subclassification_name,
		int statement_prior_year_list_length,
		LIST *latex_column_list );

/* Usage */
/* ----- */
LATEX_ROW *statement_subclass_display_latex_subclassification_sum_row(
		char *subclassification_name,
		double sum,
		int percent_of_asset,
		int percent_of_income,
		LIST *statement_prior_year_list,
		LIST *latex_column_list );

/* Usage */
/* ----- */
LATEX_ROW *statement_subclass_display_latex_element_sum_row(
		char *element_name,
		double sum,
		int percent_of_asset,
		int percent_of_income,
		LIST *statement_prior_year_list,
		LIST *latex_column_list );

/* Usage */
/* ----- */
LIST *statement_subclass_display_latex_account_row_list(
		LIST *account_statement_list,
		LIST *statement_prior_year_list,
		LIST *latex_column_list );

/* Usage */
/* ----- */
LATEX_ROW *statement_subclass_display_latex_account_row(
		STATEMENT_ACCOUNT *statement_account,
		LIST *statement_prior_year_list,
		LIST *latex_column_list );

typedef struct
{
	LIST *column_list;
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
LIST *statement_subclass_display_latex_list_column_list(
		LIST *statement_prior_year_list );

/* Usage */
/* ----- */
LIST *statement_subclass_display_latex_list_row_list(
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
		LIST *statement_prior_year_list,
		LIST *latex_column_list );

/* Process */
/* ------- */
STATEMENT_SUBCLASS_OMIT_LATEX *
	statement_subclass_omit_latex_calloc(
		void );

/* Usage */
/* ----- */
LATEX_ROW *statement_subclass_omit_latex_element_label_row(
		char *element_name,
		int statement_prior_year_list_length,
		LIST *latex_column_list );

/* Usage */
/* ----- */
LATEX_ROW *statement_subclass_omit_latex_element_sum_row(
		char *element_name,
		double element_sum,
		int percent_of_asset,
		int percent_of_income,
		LIST *statement_prior_year_list,
		LIST *latex_column_list );

/* Usage */
/* ----- */
LATEX_ROW *statement_subclass_omit_latex_row(
		STATEMENT_ACCOUNT *statement_account,
		LIST *statement_prior_year_list,
		LIST *latex_column_list );

typedef struct
{
	LIST *column_list;
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

LIST *statement_subclass_omit_latex_list_column_list(
		LIST *statement_prior_year_list );

/* Usage */
/* ----- */
LIST *statement_subclass_omit_latex_list_row_list(
	STATEMENT_SUBCLASS_OMIT_LATEX_LIST *
		statement_subclass_omit_latex_list );

typedef struct
{
	LIST *row_list;
} STATEMENT_SUBCLASS_AGGR_LATEX;

/* Usage */
/* ----- */
STATEMENT_SUBCLASS_AGGR_LATEX *
	statement_subclass_aggr_latex_new(
		ELEMENT *element,
		LIST *statement_prior_year_list,
		LIST *latex_column_list );

/* Process */
/* ------- */
STATEMENT_SUBCLASS_AGGR_LATEX *
	statement_subclass_aggr_latex_calloc(
		void );

/* Usage */
/* ----- */
LATEX_ROW *statement_subclass_aggr_latex_element_label_row(
		char *element_name,
		int statement_prior_year_list_length,
		LIST *latex_column_list );

/* Usage */
/* ----- */
LATEX_ROW *statement_subclass_aggr_latex_element_sum_row(
		char *element_name,
		double sum,
		int percent_of_asset,
		int percent_of_income,
		LIST *statement_prior_year_list,
		LIST *latex_column_list );

/* Usage */
/* ----- */
LATEX_ROW *statement_subclass_aggr_latex_row(
		SUBCLASSIFICATION *subclassification,
		LIST *statement_prior_year_list,
		LIST *latex_column_list );

typedef struct
{
	LIST *column_list;
	LIST *list;
} STATEMENT_SUBCLASS_AGGR_LATEX_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
STATEMENT_SUBCLASS_AGGR_LATEX_LIST *
	statement_subclass_aggr_latex_list_new(
		LIST *element_statement_list,
		LIST *statement_prior_year_list );

/* Process */
/* ------- */
STATEMENT_SUBCLASS_AGGR_LATEX_LIST *
	statement_subclass_aggr_latex_list_calloc(
		void );

/* Usage */
/* ----- */
LIST *statement_subclass_aggr_latex_list_column_list(
		LIST *statement_prior_year_list );

/* Usage */
/* ----- */
LIST *statement_subclass_aggr_latex_list_row_list(
	STATEMENT_SUBCLASS_AGGR_LATEX_LIST *
		statement_subclass_aggr_latex_list );

typedef struct
{
	char *logo_filename;
	char *title;
	char *sub_title;
	char *combined;
	char *date_now16;
	char *frame_title;
} STATEMENT_CAPTION;

/* Usage */
/* ----- */
STATEMENT_CAPTION *statement_caption_new(
		char *application_name,
		char *process_name,
		char *transaction_begin_date_string,
		char *end_date_time );

/* Process */
/* ------- */
STATEMENT_CAPTION *statement_caption_calloc(
		void );

/* Returns heap memory or null */
/* --------------------------- */
char *statement_caption_logo_filename(
		const char *statement_logo_filename_key );

/* Returns static memory */
/* --------------------- */
char *statement_caption_title(
		char *application_name,
		boolean exists_logo_filename,
		char *process_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *statement_caption_sub_title(
		char *begin_date_string,
		char *end_date_time );

/* Returns heap memory */
/* ------------------- */
char *statement_caption_combined(
		char *statement_caption_title,
		char *statement_caption_sub_title );

/* Returns static memory */
/* --------------------- */
char *statement_caption_frame_title(
		char *process_name,
		char *date_now16 );

typedef struct
{
	char *process_name;
	char *transaction_date_begin_date_string;
	char *end_date_time;
	LIST *element_statement_list;
	boolean pdf_landscape_boolean;
	STATEMENT_CAPTION *statement_caption;
	char *greater_year_message;
} STATEMENT;

/* Usage */
/* ----- */
STATEMENT *statement_fetch(
		char *application_name /* optional */,
		char *process_name /* optional */,
		int prior_year_count,
		LIST *element_name_list,
		char *transaction_date_begin_date_string /* optional */,
		char *end_date_time,
		boolean fetch_transaction,
		boolean latest_zero_balance_boolean );

/* Process */
/* ------- */
STATEMENT *statement_calloc(
		void );

boolean statement_pdf_landscape_boolean(
		int prior_year_count );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *statement_date_american(
		char *date_time );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *statement_date_convert(
		char *application_name,
		char *login_name,
		char *date_time );

/* Usage */
/* ----- */

/* Returns heap memory or "" */
/* ------------------------- */
char *statement_cell_label_datum(
		char *name );

/* Usage */
/* ----- */
boolean statement_subclassification_name_duplicated(
		char *element_name,
		char *subclassification_name );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *statement_element_sum_name(
		char *element_name );

/* Usage */
/* ----- */

/* Returns program memory or null */
/* ------------------------------ */
char *statement_greater_year_message(
		char *transaction_date_begin_date_string,
		char *end_date_time );

/* Usage */
/* ----- */
enum statement_subclassification_option
	statement_resolve_subclassification_option(
		char *subclassification_option_string );

/* Usage */
/* ----- */
enum statement_output_medium
	statement_resolve_output_medium(
		char *output_medium_string );

/* Returns static memory */
/* --------------------- */
char *statement_pdf_prompt(
		char *process_name );

/* Driver */
/* ------ */
void statement_html_output(
		HTML_TABLE *html_table,
		char *secondary_title );

typedef struct
{
	char *date_time;
	LIST *element_statement_list;
} STATEMENT_PRIOR_YEAR;

/* Usage */
/* ----- */
LIST *statement_prior_year_list(
		LIST *filter_element_name_list,
		char *end_date_time,
		int prior_year_count,
		STATEMENT *statement );

/* Usage */
/* ----- */
STATEMENT_PRIOR_YEAR *statement_prior_year_fetch(
		LIST *filter_element_name_list,
		char *end_date_time,
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
char *statement_prior_year_date_time(
		char *end_date_time,
		int years_ago );

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

/* Usage */
/* ----- */
LIST *statement_prior_year_heading_list(
		LIST *statement_prior_year_list );

typedef struct
{
	APPASERVER_LINK *appaserver_link;
	char *tex_filename;
	char *tex_anchor_html;
	char *pdf_anchor_html;
	char *appaserver_link_working_directory;
} STATEMENT_LINK;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
STATEMENT_LINK *statement_link_new(
		char *application_name,
		char *process_name,
		char *appaserver_parameter_data_directory,
		char *transaction_date_begin_date_string,
		char *end_date_time,
		pid_t process_id );

/* Process */
/* ------- */
STATEMENT_LINK *statement_link_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *statement_link_tex_prompt(
		char *process_name );

/* Returns static memory */
/* --------------------- */
char *statement_link_pdf_prompt(
		char *process_name );

typedef struct
{
	double difference;
	int percent;
	char *cell_string;
} STATEMENT_DELTA;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
STATEMENT_DELTA *statement_delta_new(
		double base_value,
		double change_value );

/* Process */
/* ------- */
STATEMENT_DELTA *statement_delta_calloc(
		void );

double statement_delta_difference(
		double base_value,
		double change_value );

int statement_delta_percent(
		double base_value,
		double statement_delta_difference );

/* Returns heap memory */
/* ------------------- */
char *statement_delta_cell_string(
		double statement_delta_difference,
		int statement_delta_percent );

#endif
