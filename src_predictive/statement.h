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
	boolean transaction_closing_entry_exists;
	char *transaction_date_time_nominal;
	char *transaction_date_time_fixed;
	LIST *preclose_element_list;
	LIST *postclose_element_list;
} STATEMENT;

/* Usage */
/* ----- */
STATEMENT *statement_fetch(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			LIST *filter_element_name_list,
			char *statement_begin_date_string,
			char *transaction_as_of_date,
			int prior_year_count,
			enum subclassification_option subclassification_option,
			boolean with_postclose );

/* Process */
/* ------- */
STATEMENT *statement *statement_calloc(
			void );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *statement_begin_date_string(
			char *transaction_table,
			char *transaction_as_of_date );

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

/* Returns heap memory or null */
/* --------------------------- */
char *statement_logo_filename(
			char *application_name,
			char *statement_logo_filename_key );

/* Returns static memory */
/* --------------------- */
char *statement_title(	char *application_name,
			char *statement_logo_filename,
			char *process_name );

/* Returns static memory */
/* --------------------- */
char *statement_subtitle(
			char *begin_date_string,
			char *as_of_date );

#endif
