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
} INCOME_STATEMENT_SUBCLASSIFICATION_LATEX;

/* Usage */
/* ----- */
NCOME_STATEMENT_SUBCLASSIFICATION_LATEX *
	income_statement_subclassification_latex_new(
			char *tex_filename,
			char *dvi_filename,
			char *working_directory,
			boolean statement_pdf_landscape_boolean,
			char *statement_logo_filename,
			STATEMENT *statement,
			LIST *statement_prior_year_list,
			double income_statement_net_income );

/* Process */
/* ------- */
INCOME_STATEMENT_SUBCLASSIFICATION_LATEX *
	income_statement_subclassification_latex_new(
			void );

typedef struct
{
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
			LIST *stastement_prior_year_list,
			double income_statement_net_income,
			pid_t process_id );

/* Process */
/* ------- */
INCOME_STATEMENT_PDF *income_statement_pdf_calloc(
			void );

typedef struct
{
} INCOME_STATEMENT;

/* Usage */
/* ----- */
INCOME_STATEMENT *income_statement_fetch(
			char *application_name,
			char *session_key,
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

double income_statement_net_income(
			char *element_revenue,
			char *element_expense,
			char *element_gain,
			char *element_loss,
			LIST *element_statement_list );

#endif
