/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_audit.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FEEDER_AUDIT_H
#define FEEDER_AUDIT_H

#include "list.h"
#include "boolean.h"
#include "html.h"
#include "journal.h"
#include "account.h"
#include "feeder_audit_journal.h"
#include "feeder_row.h"
#include "feeder_load_event.h"

#define FEEDER_AUDIT_HTML_TITLE		"Feeder After Execute Audit"

#define FEEDER_AUDIT_DIFFERENCE_MESSAGE			\
	"Warning: Execute the Audit process 		\
	\"Feeder Journal Orphan Audit\" or		\
	\"Feeder Row Journal Audit\""

typedef struct
{
	FEEDER_LOAD_EVENT *feeder_load_event;
	FEEDER_AUDIT_JOURNAL *feeder_audit_journal;
	ACCOUNT *account_fetch;
	double journal_balance;
	double balance_difference;
	boolean difference_zero_boolean;
	HTML_TABLE *html_table;
} FEEDER_AUDIT;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER_AUDIT *feeder_audit_fetch(
		char *application_name,
		char *login_name,
		char *feeder_account_name );

/* Process */
/* ------- */
FEEDER_AUDIT *feeder_audit_calloc(
		void );

LIST *feeder_audit_html_column_list(
		void );

double feeder_audit_accumulate_debit_journal_balance(
		double journal_fetch_balance,
		boolean element_accumulate_debit );

double feeder_audit_balance_difference(
		double file_row_balance,
		double feeder_audit_journal_balance );

boolean feeder_audit_difference_zero_boolean(
		double feeder_audit_balance_difference );

/* Usage */
/* ----- */
HTML_ROW *feeder_audit_html_row(
		char *application_name,
		char *login_name,
		double event_account_end_balance,
		char *event_account_end_date,
		double journal_balance,
		double feeder_audit_balance_difference,
		boolean feeder_audit_difference_zero_boolean );

/* Process */
/* ------- */
LIST *feeder_audit_html_cell_list(
		char *feeder_date,
		double event_account_end_balance,
		double journal_balance,
		double feeder_audit_balance_difference,
		boolean feeder_audit_difference_zero_boolean );

#endif
