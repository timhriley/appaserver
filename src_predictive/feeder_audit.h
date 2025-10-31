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
#include "feeder.h"
#include "feeder_load_event.h"

#define FEEDER_AUDIT_HTML_TITLE		"Feeder After Execute Audit"

typedef struct
{
	FEEDER_LOAD_EVENT *feeder_load_event;
	int feeder_row_final_number;
	FEEDER_ROW *feeder_row_fetch;
	char *end_transaction_date_time;
	JOURNAL *journal_latest;
	ACCOUNT *account_fetch;
	double credit_balance_negate;
	double balance_difference;
	boolean difference_zero;
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

/* Returns static memory */
/* --------------------- */
char *feeder_audit_end_transaction_date_time(
		const char *transaction_date_close_time,
		char *feeder_date );

LIST *feeder_audit_html_column_list(
		void );

double feeder_audit_credit_balance_negate(
		double journal_balance,
		boolean element_accumulate_debit );

double feeder_audit_balance_difference(
		double calculate_balance,
		double journal_balance );

boolean feeder_audit_difference_zero(
		double feeder_audit_balance_difference );

/* Usage */
/* ----- */
HTML_ROW *feeder_audit_html_row(
		char *application_name,
		char *login_name,
		FEEDER_ROW *feeder_row,
		JOURNAL *journal,
		double journal_balance,
		double feeder_audit_balance_difference,
		boolean feeder_audit_difference_zero );

/* Process */
/* ------- */
LIST *feeder_audit_html_cell_list(
		int feeder_row_number,
		char *feeder_row_full_name,
		char *file_row_description,
		char *feeder_row_transaction_date_time,
		char *feeder_date,
		double feeder_row_file_row_amount,
		double feeder_row_calculate_balance,
		char *journal_full_name,
		char *journal_transaction_date_time,
		double journal_balance,
		double feeder_audit_balance_difference,
		boolean feeder_audit_difference_zero );

#endif
