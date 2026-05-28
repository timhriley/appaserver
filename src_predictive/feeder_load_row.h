/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_load_row.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FEEDER_LOAD_ROW_H
#define FEEDER_LOAD_ROW_H

#include "list.h"
#include "boolean.h"
#include "date.h"
#include "appaserver_user.h"
#include "html.h"
#include "transaction.h"
#include "journal.h"
#include "account.h"
#include "feeder_account.h"
#include "feeder_load_event.h"

#define FEEDER_LOAD_ROW_DESCRIPTION_SIZE	140

typedef struct
{
	double exchange_journal_amount;
	char *transaction_date_time;
	double debit_amount;
	double credit_amount;
	double journal_balance;
	char *international_date;
	int check_number;
	char *description_space_trim;
	char *description_embedded;

	/* Set externally */
	/* -------------- */
	double file_row_balance;
} FEEDER_LOAD_ROW;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FEEDER_LOAD_ROW *feeder_load_row_new(
		double exchange_journal_amount,
		char *exchange_journal_description,
		char *transaction_date_time,
		double debit_amount,
		double credit_amount,
		double journal_balance );

/* Process */
/* ------- */
FEEDER_LOAD_ROW *feeder_load_row_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *feeder_load_row_international_date(
		char *transaction_date_time );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *feeder_load_row_description_space_trim(
		char *exchange_journal_description );

/* Usage */
/* ----- */

/* Returns heap memory or description_space_trim */
/* --------------------------------------------- */
char *feeder_load_row_description_embedded(
		double journal_balance,
		char *description_space_trim,
		int feeder_load_row_check_number );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *feeder_load_row_trim_date(
		char *description_space_trim );

/* Returns heap memory */
/* ------------------- */
char *feeder_load_row_description_build(
		double journal_balance,
		char *feeder_load_row_trim_date );

/* Returns feeder_load_row_description_build */
/* ----------------------------------------- */
char *feeder_load_row_description_crop(
		const int feeder_load_row_description_size,
		char *feeder_load_row_description_build );

/* Usage */
/* ----- */
void feeder_load_row_file_row_balance_set(
		double exchange_balance_amount,
		LIST *feeder_load_row_list
			/* reads exchange_journal_amount */
			/* sets file_row_balance */ );

/* Process */
/* ------- */

/* Returns exchange_balance_amount */
/* ------------------------------- */
double feeder_load_row_file_row_balance(
		double exchange_balance_amount );

/* Usage */
/* ----- */
LIST *feeder_load_row_list(
		LIST *exchange_journal_list );

/* Usage */
/* ----- */
int feeder_load_row_check_number(
		char *feeder_load_row_description_space_trim );

/* Process */
/* ------- */

/* If like "VETERANS AFFAIRS DES:PAYMENT CHECK #:1827" */
/* --------------------------------------------------- */
int feeder_load_row_pound_colon_number(
		char *feeder_load_row_description_space_trim );

/* If like "#1827" */
/* --------------- */
int feeder_load_row_pound_number(
		char *feeder_load_row_description_space_trim );

/* If like "check 1827" or "CHECK 1827" */
/* ------------------------------------ */
int feeder_load_row_check_text_number(
		char *feeder_load_row_description_space_trim );

/* Usage */
/* ----- */
void feeder_load_row_list_raw_display(
		FILE *stream,
		LIST *feeder_load_row_list );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *feeder_load_row_raw_display(
		FEEDER_LOAD_ROW *feeder_load_row );

#endif
