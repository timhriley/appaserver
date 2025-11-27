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

#define EXCHANGE_OFX_TAG		"<OFX>"
#define EXCHANGE_ORG_TAG		"<ORG>"
#define EXCHANGE_DTPOSTED_TAG		"<DTPOSTED>"
#define EXCHANGE_TRNAMT_TAG		"<TRNAMT>"
#define EXCHANGE_NAME_TAG		"<NAME>"
#define EXCHANGE_MEMO_TAG		"<MEMO>"
#define EXCHANGE_STMTTRN_END_TAG	"</STMTTRN>"
#define EXCHANGE_BALAMT_TAG		"<BALAMT>"
#define EXCHANGE_TRNTYPE_TAG		"<TRNTYPE>"

typedef struct
{
	double amount;
	char *description;
	JOURNAL *journal;
} EXCHANGE_JOURNAL;

/* Usage */
/* ----- */
EXCHANGE_JOURNAL *exchange_journal_extract(
		LIST *exchange_file_list );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
EXCHANGE_JOURNAL *exchange_journal_new(
		char *date_posted,
		char *amount_string,
		char *description,
		double balance_double );

/* Process */
/* ------- */
EXCHANGE_JOURNAL *exchange_journal_calloc(
		void );

boolean exchange_open_tag_boolean(
		const char *exchange_ofx_tag,
		LIST *exchange_file_list );

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
LIST *exchange_journal_list(
		LIST *exchange_file_list );

/* Usage */
/* ----- */
double exchange_journal_begin_amount(
		LIST *exchange_journal_list
			/* Sets journal->previous_balance and balance */,
		double exchange_balance_amount );

/* Process */
/* ------- */
LIST *exchange_journal_extract_list(
		LIST *exchange_journal_list );

/* Usage */
/* ----- */
int exchange_journal_compare_function(
		EXCHANGE_JOURNAL *exchange_journal_from_list,
		EXCHANGE_JOURNAL *exchange_journal_compare );

/* Usage */
/* ----- */

/* Returns name or heap memory */
/* --------------------------- */
char *exchange_journal_description(
		char *name,
		char *memo );

typedef struct
{
	char *exchange_format_filename;
	char *filespecification;
	char *system_string;
	LIST *file_list;
	boolean open_tag_boolean;
	char *financial_institution;
	LIST *exchange_journal_list;
	double balance_amount;
	double exchange_journal_begin_amount;
	char *minimum_date_string;
} EXCHANGE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
EXCHANGE *exchange_fetch(
		char *application_name,
		char *exchange_format_filename,
		char *upload_directory );

/* Process */
/* ------- */
EXCHANGE *exchange_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *exchange_filespecification(
		char *application_name,
		char *exchange_format_filename,
		char *upload_directory );

/* Returns heap memory */
/* ------------------- */
char *exchange_minimum_date_string(
		EXCHANGE_JOURNAL *first_exchange_journal );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *exchange_system_string(
		char *exchange_filespecification );

/* Usage */
/* ----- */
LIST *exchange_file_list(
		FILE *appaserver_input_pipe );

/* Usage */
/* ----- */

/* Returns exchange_file_list */
/* -------------------------- */
LIST *exchange_tag_seek_list(
		const char *tag,
		char *stop_tag,
		LIST *exchange_file_list );

/* Usage */
/* ----- */
boolean exchange_open_tag_boolean(
		const char *exchange_ofx_tag,
		LIST *exchange_file_list );

/* Usage */
/* ----- */
boolean exchange_tag_boolean(
		const char *tag,
		char *list_get );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *exchange_financial_institution(
		const char *exchange_org_tag,
		LIST *exchange_file_list );

/* Usage */
/* ----- */
double exchange_balance_amount(
		const char *exchange_balamt_tag,
		LIST *exchange_file_list );

#endif
