/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_matched_journal.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FEEDER_MATCHED_JOURNAL_H
#define FEEDER_MATCHED_JOURNAL_H

#include "list.h"
#include "boolean.h"
#include "date.h"
#include "appaserver_user.h"
#include "html.h"
#include "transaction.h"
#include "journal.h"
#include "account.h"

typedef struct
{
	char *full_name;
	char *contact_key;
	char *transaction_date_time;
	char *account_name;
	double debit_amount;
	double credit_amount;
	double amount;

	/* Set externally */
	/* -------------- */
	int check_number;
	boolean taken;
	char *check_update_statement;
} FEEDER_MATCHED_JOURNAL;

/* Usage */
/* ----- */
LIST *feeder_matched_journal_list(
		const char *feeder_row_table,
		char *feeder_account_name,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *feeder_match_minimum_date,
		char *account_uncleared_checks_string );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *feeder_matched_journal_where(
		char *feeder_account_name,
		char *account_uncleared_checks,
		char *feeder_matched_journal_subquery,
		char *feeder_match_minimum_date );

/* Usage */
/* ----- */
FEEDER_MATCHED_JOURNAL *feeder_matched_journal_new(
		JOURNAL *journal );

/* Process */
/* ------- */
FEEDER_MATCHED_JOURNAL *feeder_matched_journal_calloc(
		void );

double feeder_matched_journal_amount(
		double debit_amount,
		double credit_amount );

/* Usage */
/* ----- */

/* Independent of fund */
/* ------------------- */
FEEDER_MATCHED_JOURNAL *feeder_matched_journal_check_seek(
		char *feeder_account_name,
		boolean entity_contact_key_boolean,
		char *account_uncleared_checks_string,
		int check_number,
		double exchange_journal_amount,
		LIST *feeder_matched_journal_list );

/* Usage */
/* ----- */
FEEDER_MATCHED_JOURNAL *
	feeder_matched_journal_amount_seek(
		double exchange_journal_amount,
		LIST *feeder_matched_journal_list );

/* Usage */
/* ----- */

/* ------------------- */
/* Independent of fund */
/* Returns heap memory */
/* ------------------- */
char *feeder_matched_journal_subquery(
		const char *journal_table,
		const char *feeder_row_table,
		char *feeder_account_name,
		char *account_uncleared_checks_string,
		boolean entity_contact_key_boolean );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *feeder_matched_journal_contact_join(
		const char *journal_table,
		const char *feeder_row_table,
		const char *entity_contact_key_column,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* ------------------- */
/* Independent of fund */
/* Returns heap memory */
/* ------------------- */
char *feeder_matched_journal_check_update_statement(
		const char *journal_table,
		char *feeder_account_name,
		boolean entity_contact_key_boolean,
		char *account_uncleared_checks_string,
		char *full_name,
		char *contact_key,
		char *transaction_date_time );

#endif
