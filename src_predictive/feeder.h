/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FEEDER_H
#define FEEDER_H

#include "list.h"
#include "boolean.h"
#include "date.h"
#include "appaserver_user.h"
#include "html.h"
#include "transaction.h"
#include "journal.h"
#include "account.h"
#include "feeder_account.h"
#include "feeder_audit.h"
#include "feeder_load_event.h"

/* #define FEEDER_DEBUG_MODE 1 */

#define FEEDER_LOAD_TRANSACTION_DAYS_AGO				\
					"feeder_load_transaction_days_ago"

#define FEEDER_MATCH_DEFAULT_DAYS_AGO	366

#define FEEDER_DESCRIPTION_SIZE		140

#define FEEDER_MATCH_ERROR_TEMPLATE \
"<h3>Mismatched input file. Either:</h3><ol> " \
"<li>An existing transaction exists, but the File Row Description changed or " \
"<li>The bank's begin date is too recent or " \
"<li>A check written a long time ago cleared" \
"</ol><ol><li>The error amount is %.2lf" \
"<li>Probably the File Row Description changed." \
"<ol><li>Duplicate another browser tab." \
"<li>[Copy] the suspected Description into your clipboard." \
"<li>Lookup --> Transaction --> Journal." \
"<li>On the Drillthru Transaction screen, "\
"query the suspected transction date and transaction amount." \
"<li>On the Lookup Journal screen, " \
"[Fill] the Account with the feeder account then [Submit]." \
"<li>On the Update Journal screen, check [Drilldown], then [Submit]."\
"<li>On the Feeder Row block, [Paste} your clipboard onto the " \
"File Row Description, then [Submit].</ol>" \
"<li>Execute the Audit process called Feeder Row Journal Audit. " \
"Check the File Row Amount column for a clue.</ol>"

typedef struct
{
	char *feeder_account_name;
	boolean predictive_fund_boolean;
	char *predictive_fund_name;
	boolean entity_contact_key_boolean;
	FEEDER_ACCOUNT *feeder_account;
	LIST *feeder_load_row_list;
	char *account_uncleared_checks_string;
	LIST *feeder_phrase_list;
	LIST *feeder_row_exist_list;
	int match_days_ago;
	char *match_minimum_date;
	LIST *feeder_matched_journal_list;
	LIST *feeder_row_list;
	int feeder_row_count;
	boolean feeder_row_list_non_match_boolean;
	int feeder_row_insert_count;
	double feeder_load_event_prior_account_end_balance;
	boolean feeder_row_list_status_out_of_balance_boolean;
	FEEDER_LOAD_EVENT *feeder_load_event_latest_fetch;
	double feeder_load_event_error_double;
	char *feeder_row_account_end_date;
	double feeder_row_account_end_balance;
	char *feeder_load_date_time;
	FEEDER_LOAD_EVENT *feeder_load_event;
} FEEDER;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER *feeder_fetch(
		char *application_name,
		char *login_name,
		char *fund_name,
		char *feeder_account_name,
		boolean reverse_order_boolean,
		char *exchange_format_filename,
		LIST *exchange_journal_list,
		double exchange_journal_begin_amount,
		double exchange_balance_amount,
		char *exchange_minimum_date_string );

/* Process */
/* ------- */
FEEDER *feeder_calloc(
		void );

/* Safely returns */
/* -------------- */
int feeder_match_days_ago(
		const char *feeder_load_transaction_days_ago,
		const int feeder_match_default_days_ago );

/* Returns heap memory or "" */
/* ------------------------- */
char *feeder_match_minimum_date(
		char *exchange_minimum_date_string,
		int feeder_match_days_ago );

/* Driver */
/* ------ */
void feeder_execute(
		char *process_name,
		char *fund_name,
		FEEDER *feeder );

/* Usage */
/* ----- */
void feeder_display(
		boolean reverse_order_boolean,
		FEEDER *feeder );

/* Usage */
/* ----- */
boolean feeder_execute_boolean(
		boolean execute_boolean,
		boolean feeder_row_list_non_match_boolean,
		boolean feeder_row_list_out_of_balance_boolean,
		double feeder_load_event_error_double );

/* Usage */
/* ----- */
void feeder_process(
		char *application_name,
		char *process_name,
		char *login_name,
		char *fund_name,
		char *feeder_account_name,
		boolean reverse_order_boolean,
		boolean execute_boolean,
		FEEDER *feeder );

#endif
