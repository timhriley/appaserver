/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_transaction.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FEEDER_TRANSACTION_H
#define FEEDER_TRANSACTION_H

#include "list.h"
#include "boolean.h"
#include "date.h"
#include "appaserver_user.h"
#include "html.h"
#include "transaction.h"
#include "journal.h"
#include "account.h"
#include "feeder_phrase.h"
#include "feeder_transaction.h"

typedef struct
{
	char *debit_account;
	char *credit_account;
	TRANSACTION *transaction;
} FEEDER_TRANSACTION;

/* Usage */
/* ----- */
FEEDER_TRANSACTION *feeder_transaction_new(
		char *fund_name,
		char *feeder_account_name,
		FEEDER_PHRASE *feeder_phrase_seek,
		double exchange_journal_amount,
		char *transaction_date_time,
		char *memo );

/* Process */
/* ------- */
FEEDER_TRANSACTION *feeder_transaction_calloc(
		void );

#endif
