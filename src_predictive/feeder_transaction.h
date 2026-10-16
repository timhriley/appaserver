/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/feeder_transaction.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"
#include "date.h"
#include "appaserver_user.h"
#include "html.h"
#include "transaction.h"
#include "journal.h"
#include "account.h"
#include "liability.h"
#include "feeder_phrase.h"
#include "feeder_transaction.h"

typedef struct
{
	char *nominal_account_name;
	char *full_name;
	char *contact_key;
	TRANSACTION *transaction;
} FEEDER_TRANSACTION;

/* Usage */
/* ----- */
FEEDER_TRANSACTION *feeder_transaction_new(
		char *fund_name,
		char *feeder_account_name,
		FEEDER_PHRASE *feeder_phrase_seek
			/* Mutually exclusive */,
		LIABILITY_ENTITY *liability_entity
			/* Mutually exclusive */,
		double exchange_journal_amount,
		char *transaction_date_time,
		char *memo );

/* Process */
/* ------- */
FEEDER_TRANSACTION *feeder_transaction_calloc(
		void );

/* Returns a component of either parameter */
/* --------------------------------------- */
char *feeder_transaction_full_name(
		FEEDER_PHRASE *feeder_phrase_seek,
		LIABILITY_ENTITY *feeder_row_liability_entity );

/* Returns a component of either parameter */
/* --------------------------------------- */
char *feeder_transaction_contact_key(
		FEEDER_PHRASE *feeder_phrase_seek,
		LIABILITY_ENTITY *feeder_row_liability_entity );

/* Usage */
/* ----- */

/* Returns a component of either parameter */
/* --------------------------------------- */
char *feeder_transaction_nominal_account_name(
		FEEDER_PHRASE *feeder_phrase_seek,
		LIABILITY_ENTITY *feeder_row_liability_entity );

