/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/exchange.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef EXCHANGE_H
#define EXCHANGE_H

#include "list.h"
#include "boolean.h"
#include "journal.h"

#define EXCHANGE_OFX_TAG	"<OFX>"
#define EXCHANGE_ORG_TAG	"<ORG>"
#define EXCHANGE_BALAMT_TAG	"<BALAMT>"
#define EXCHANGE_TRNTYPE_TAG	"<TRNTYPE>"

typedef struct
{
	JOURNAL *journal;
	char *description;
} EXCHANGE_JOURNAL;

/* Usage */
/* ----- */
EXCHANGE_JOURNAL *exchange_journal_extract(
		const char *exchange_trntype_tag,
		LIST *list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
EXCHANGE_JOURNAL *exchange_journal_new(
		char *date_posted,
		char *amount_string,
		char *description );

/* Process */
/* ------- */
EXCHANGE_JOURNAL *exchange_journal_calloc(
		void );

boolean exchange_open_tag_boolean(
		const char *exchange_ofx_tag,
		LIST *list_stream_fetch );

/* Returns heap memory */
/* ------------------- */
char *exchange_journal_transaction_date_time(
		char *date_posted );

double exchange_journal_debit_amount(
		char *amount_string );

double exchange_journal_credit_amount(
		char *amount_string );

/* Usage */
/* ----- */
double exchange_journal_begin_amount(
		LIST *exchange_journal_list,
		double exchange_balance_amount );

/* Process */
/* ------- */
LIST *exchange_journal_extract_list(
		LIST *exchange_journal_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *exchange_journal_datum(
		char *list_get );

/* Usage */
/* ----- */
int exchange_journal_compare_function(
		EXCHANGE_JOURNAL *exchange_journal_from_list,
		EXCHANGE_JOURNAL *exchange_journal_compare );

typedef struct
{
	LIST *list_stream_fetch;
	boolean open_tag_boolean;
	char *financial_institution;
	LIST *exchange_journal_list;
	double balance_amount;
	double exchange_journal_begin_amount;
} EXCHANGE;

/* Usage */
/* ----- */
EXCHANGE *exchange_parse(
		char *filespecification );

/* Process */
/* ------- */
EXCHANGE *exchange_calloc(
		void );

/* Returns heap memory or null */
/* --------------------------- */
char *exchange_financial_institution(
		const char *exchange_org_tag,
		LIST *list_stream_fetch );

double exchange_balance_amount(
		const char *exchange_balamt_tag,
		LIST *list_stream_fetch );

/* Usage */
/* ----- */

/* Returns list */
/* ------------ */
LIST *exchange_tag_seek_list(
		const char *tag,
		LIST *list );

/* Usage */
/* ----- */
boolean exchange_open_tag_boolean(
		const char *exchange_ofx_tag,
		LIST *list );

#endif
