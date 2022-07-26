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
#include "account.h"

#define STATEMENT_ROWS_BETWEEN_HEADING		10
#define STATEMENT_PROMPT			"Press here to view statement."
#define STATEMENT_LOGO_FILENAME_KEY		"logo_filename"

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
	LIST *element_statement_list;
	char *logo_filename;
	char *title;
	char *subtitle;
	char *caption;
	char *date_american;
} STATEMENT;

/* Usage */
/* ----- */
STATEMENT *statement_fetch(
			char *application_name,
			char *process_name,
			LIST *filter_element_name_list,
			char *transaction_begin_date_string,
			char *transaction_as_of_date,
			char *transaction_date_time_closing );

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
char *statement_title(	char *application_name,
			boolean exists_logo_filename,
			char *process_name );

/* Returns static memory or null */
/* ----------------------------- */
char *statement_subtitle(
			char *begin_date_string,
			char *as_of_date );

/* Returns heap memory */
/* ------------------- */
char *statement_caption(
			char *statement_title,
			char *statement_subtitle );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *statement_date_american(
			char *date_time_string );

/* Process */
/* ------- */

/* Public */
/* ------ */
enum statement_subclassification_option
	statement_resolve_subclassification_option(
			char *subclassification_option_string );

enum statement_output_medium
	statement_resolve_output_medium(
			char *output_medium_string );

typedef struct
{
	char *date_string;
	LIST *element_statement_list;
} STATEMENT_PRIOR_YEAR;

/* Usage */
/* ----- */
LIST *statement_prior_year_list(
			LIST *filter_element_name_list,
			char *transation_date_time_closing,
			int prior_year_count,
			STATEMENT *statement );

/* Process */
/* ------- */

/* Usage */
/* ----- */
STATEMENT_PRIOR_YEAR *statement_prior_year_fetch(
			LIST *filter_element_name_list,
			char *transation_date_time_closing,
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
LIST *statement_prior_year_latex_account_data_list(
			char *account_name,
			LIST *statement_prior_year_list );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *statement_prior_year_latex_account_data(
			ACCOUNT *prior_account );

/* Public */
/* ------ */
LIST *statement_prior_year_heading_list(
			LIST *statement_prior_year_list );

LIST *statement_prior_year_latex_row_column_data_list(
			char *account_name,
			LIST *statement_prior_year_list );

typedef struct
{
	char *filename_stem;
	APPASERVER_LINK_FILE *appaserver_link_file;
	char *latex_filename;
	char *dvi_filename;
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
