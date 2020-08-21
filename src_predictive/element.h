/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/element.h			*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef ELEMENT_H
#define ELEMENT_H

#include "list.h"
#include "boolean.h"
#include "latex.h"
#include "html_table.h"
#include "account.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *element_name;
	boolean accumulate_debit;

	/* -------------------------- */
	/* if !omit_subclassification */
	/* -------------------------- */
	LIST *subclassification_list;

	/* ------------------------- */
	/* if omit_subclassification */
	/* ------------------------- */
	LIST *account_list;

	double element_total;
} ELEMENT;

/* Operations */
/* ---------- */

/* Returns static memory */
/* --------------------- */
ELEMENT *element_parse(	char *input );

ELEMENT *element_new(	char *element_name ); 

ELEMENT *element_account_name_fetch(
			char *account_name );

char *element_select(	void );

boolean element_accumulate_debit(
			char *element_name );

ELEMENT *element_fetch(	char *element_name );

ELEMENT *element_seek(	LIST *element_list,
			char *element_name );

LIST *element_subclassification_list(
			double *element_total,
			char *element_name,
			char *fund_name,
			char *as_of_date );

LIST *element_account_list(
			double *element_total,
			char *element_name,
			char *fund_name,
			char *as_of_date );

LIST *element_list_sort(
			LIST *element_list );

LIST *element_list(	LIST *filter_element_name_list,
			char *fund_name,
			char *as_of_date,
			boolean omit_subclassification );

boolean element_is_period(
			char *element_name );

boolean element_account_accumulate_debit(
			char *account_name );

double element_value(	LIST *subclassification_list,
			boolean element_accumulate_debit );

ELEMENT *element_list_seek(
			LIST *element_list,
			char *element_name );

double element_subclassification_aggregate_html_output(
			HTML_TABLE *html_table,
			LIST *subclassification_list,
			char *element_name,
			double percent_denominator );

LATEX_ROW *element_latex_net_income_row(
			double net_income,
			boolean is_statement_of_activities,
			double percent_denominator,
			boolean omit_subclassification );

LATEX_ROW *element_latex_liabilities_plus_equity_row(
			double liabilities_plus_equity,
			int skip_columns );

void element_list_propagate(
			LIST *element_list,
			char *transaction_date_time_string );

char *element_primary_where(
			char *element_name );

char *element_sys_string(
			char *where,
			char *order );

ACCOUNT *element_account_seek(
			LIST *element_list,
			char *account_name );

LIST *element_system_list(
			char *sys_string,
			LIST *filter_element_name_list,
			char *fund_name,
			char *as_of_date,
			boolean omit_subclassification );

#endif

