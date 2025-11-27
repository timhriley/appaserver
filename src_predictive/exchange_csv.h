/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/exchange_csv.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef EXCHANGE_CSV_H
#define EXCHANGE_CSV_H

#include "list.h"
#include "boolean.h"
#include "exchange.h"

typedef struct
{
	char *csv_format_filename;
	char *exchange_filespecification;
	LIST *exchange_csv_journal_list;
	LIST *exchange_journal_list;
	double balance_double;
	double exchange_journal_begin_amount;
	char *exchange_minimum_date_string;
} EXCHANGE_CSV;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
EXCHANGE_CSV *exchange_csv_fetch(
		char *application_name,
		char *csv_format_filename,
		int date_column /* one_based */,
		int description_column /* one_based */,
		int debit_column /* one_based */,
		int credit_column /* one_based */,
		int balance_column /* one_based */,
		int reference_column /* one_based */,
		boolean reverse_order_boolean,
		double balance_amount /* optional */,
		char *upload_directory );

/* Process */
/* ------- */
EXCHANGE_CSV *exchange_csv_calloc(
		void );

LIST *exchange_csv_exchange_journal_list(
		LIST *exchange_csv_journal_list );

double exchange_csv_balance_double(
		int balance_column,
		double balance_amount /* optional */,
		EXCHANGE_JOURNAL *last_exchange_journal );

typedef struct
{
	char *american_date;
	char *international_date;
	char *description;
	double amount_double;
	char *amount_string;
	double balance_double;
	char *reference;
	char *description_reference;
	EXCHANGE_JOURNAL *exchange_journal;
} EXCHANGE_CSV_JOURNAL;

/* Usage */
/* ----- */
EXCHANGE_CSV_JOURNAL *exchange_csv_journal_new(
		int date_column /* one based */,
		int description_column /* one based */,
		int debit_column /* one based */,
		int credit_column /* one based */,
		int balance_column /* one based */,
		int reference_column /* one based */,
		double balance_amount /* optional */,
		char *input );

/* Process */
/* ------- */
EXCHANGE_CSV_JOURNAL *exchange_csv_journal_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *exchange_csv_journal_american_date(
		int date_column,
		char *input );

/* Returns heap memory or null */
/* --------------------------- */
char *exchange_csv_journal_international_date(
		char *exchange_csv_journal_american_date );

/* Returns heap memory or null */
/* --------------------------- */
char *exchange_csv_journal_description(
		int description_column,
		char *input );

/* Returns heap memory or null */
/* --------------------------- */
char *exchange_csv_journal_reference(
		int reference_column,
		char *input );

/* Returns exchange_csv_journal_description or heap memory */
/* ------------------------------------------------------- */
char *exchange_csv_journal_description_reference(
		char *exchange_csv_journal_description,
		char *exchange_csv_journal_reference );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
EXCHANGE_JOURNAL *exchange_csv_journal_resolve(
		char *exchange_csv_journal_international_date,
		char *amount_string,
		double exchange_csv_journal_balance_double,
		char *exchange_csv_journal_description_reference );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *exchange_csv_journal_date_posted(
		char *exchange_csv_journal_international_date );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *exchange_csv_journal_piece_string(
		int column_one_based,
		char *input );

/* Usage */
/* ----- */
double exchange_csv_journal_amount_double(
		int debit_column,
		int credit_column,
		char *input );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *exchange_csv_journal_amount_string(
		double exchange_csv_journal_amount_double );

/* Usage */
/* ----- */
double exchange_csv_journal_balance_double(
		int balance_column,
		double balance_amount,
		char *input );

#endif
