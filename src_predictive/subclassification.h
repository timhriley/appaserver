/* ------------------------------------------------------------ */
/* $APPASERVER_HOME/src_predictive/subclassification.h		*/
/* ------------------------------------------------------------ */
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------ */

#ifndef SUBCLASSIFICATION_H
#define SUBCLASSIFICATION_H

#include "list.h"
#include "boolean.h"
#include "html_table.h"
#include "latex.h"
#include "account.h"

/* Constants */
/* --------- */
#define SUBCLASSIFICATION_TABLE_NAME		"subclassification"
#define SUBCLASSIFICATION_NET_ASSETS		"net_assets"
#define SUBCLASSIFICATION_CONTRIBUTED_CAPITAL	"contributed_capital"
#define SUBCLASSIFICATION_RETAINED_EARNINGS	"retained_earnings"
#define SUBCLASSIFICATION_DRAWING		"drawing"
#define SUBCLASSIFICATION_CURRENT_LIABILITY	"current_liability"
#define SUBCLASSIFICATION_RECEIVABLE		"receivable"

#define SUBCLASSIFICATION_NOTANUMBER	"NAN"

#define SUBCLASSIFICATION_CHANGE_IN_NET_ASSETS		\
					"change_in_net_assets"

#define SUBCLASSIFICATION_BEGINNING_BALANCE_LABEL	\
					"Equity Beginning Balance"

#define SUBCLASSIFICATION_NET_ASSETS	"net_assets"

/* Structures */
/* ---------- */
typedef struct
{
	char *subclassification_name;
	char *element_name;
	int display_order;
	LIST *account_list;
	double subclassification_total;
} SUBCLASSIFICATION;

/* Operations */
/* ---------- */
char *subclassification_primary_where(
			char *subclassification_name );

SUBCLASSIFICATION *subclassification_parse(
			char *input );

SUBCLASSIFICATION *subclassification_total_fetch(
			double *subclassification_total,
			char *subclassification_name,
			char *fund_name,
			char *date_time );

SUBCLASSIFICATION *subclassification_fetch(
			char *subclassification_name );

SUBCLASSIFICATION *subclassification_new(
			char *subclassification_name );

double subclassification_html_display(
			HTML_TABLE *html_table,
			LIST *subclassification_list,
			char *element_name,
			boolean element_accumulate_debit,
			double percent_denominator );

double subclassification_aggregate_html(
			HTML_TABLE *html_table,
			LIST *subclassification_list,
			char *element_name,
			double percent_denominator );

boolean subclassification_net_assets_equity_exists(
			LIST *subclassification_list );

double subclassification_net_assets_html_output(
			HTML_TABLE *html_table,
			LIST *subclassification_list,
			boolean element_accumulate_debit );

LIST *subclassification_beginning_latex_row_list(
			double *total_element,
			LIST *subclassification_list,
			boolean element_accumulate_debit );

LIST *subclassification_display_latex_row_list(
			double *total_element,
			LIST *subclassification_list,
			char *element_name,
			boolean element_accumulate_debit,
			double percent_denominator );

LIST *subclassification_aggregate_latex_row_list(
			double *total_element,
			LIST *subclassification_list,
			char *element_name,
			double percent_denominator );

LIST *subclassification_aggregate_beginning_row_list(
			double *total_element,
			LIST *subclassification_list,
			double percent_denominator );

LATEX_ROW *subclassification_latex_liabilities_plus_equity_row(
			double liabilities_plus_equity,
			int skip_columns );

void subclassification_aggregate_net_income_output(
			HTML_TABLE *html_table,
			double net_income,
			boolean is_statement_of_activities,
			double percent_denominator );

boolean subclassification_net_assets_exists(
			LIST *subclassification_list );

LIST *subclassification_account_list(
			double *subclassification_total,
			char *subclassification_name,
			char *fund_name,
			char *as_of_date );

SUBCLASSIFICATION *subclassification_fetch(
			char *subclassification_name );

LIST *subclassification_total_account_list(
			double *subclassification_total,
			char *subclassification_name,
			char *fund_name,
			char *transaction_date_time_closing );

LIST *subclassification_list(
			void );

LIST *subclassification_system_list(
			char *sys_string );

char *subclassification_sys_string(
			char *where );

SUBCLASSIFICATION *subclassification_seek(
			char *subclassification_name,
			LIST *subclassification_list );

boolean subclassification_current_liability(
			char *subclassification_name );

boolean subclassification_receivable(
			char *subclassification_name );

void subclassification_list_set_account_action_string(
			LIST *subclassification_list,
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *beginning_date,
			char *as_of_date );

#endif
