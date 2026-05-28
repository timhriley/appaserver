/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_audit_journal.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FEEDER_AUDIT_JOURNAL_H
#define FEEDER_AUDIT_JOURNAL_H

#include "list.h"
#include "boolean.h"
#include "feeder_row.h"

typedef struct
{
	LIST *row_list;
	double balance;
} FEEDER_AUDIT_JOURNAL;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER_AUDIT_JOURNAL *feeder_audit_journal_fetch(
		char *feeder_account_name,
		char *feeder_load_date_time );

/* Process */
/* ------- */
FEEDER_AUDIT_JOURNAL *feeder_audit_journal_calloc(
		void );

/* Usage */
/* ----- */
LIST *feeder_audit_journal_row_list(
		char *feeder_account_name,
		char *feeder_load_date_time );

/* Usage */
/* ----- */
LIST *feeder_audit_journal_non_fund_row_list(
		char *feeder_account_name,
		char *feeder_load_date_time );

/* Usage */
/* ----- */
LIST *feeder_audit_journal_fund_row_list(
		char *feeder_account_name,
		char *feeder_load_date_time );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER_ROW *feeder_audit_feeder_row_fetch(
		char *fund_name,
		char *feeder_account_name,
		char *feeder_load_date_time );

/* Usage */
/* ----- */
double feeder_audit_journal_balance(
		LIST *feeder_audit_row_list );

#endif
