/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/investment_transaction.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef INVESTMENT_TRANSACTION_H
#define INVESTMENT_TRANSACTION_H

#include "boolean.h"
#include "list.h"
#include "account.h"
#include "entity_self.h"
#include "transaction.h"

#define INVESTMENT_TRANSACTION_MEMO		"Mark to market"

#define INVESTMENT_TRANSACTION_NO_CHANGE	\
	"<p>No change from current balance."

typedef struct
{
	char *account_key_account_name;
	char *date_now19;
	ACCOUNT_JOURNAL *account_journal_latest;
	double current_balance;
	double delta;
	ENTITY_SELF *entity_self;
	TRANSACTION *transaction;
} INVESTMENT_TRANSACTION;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVESTMENT_TRANSACTION *investment_transaction_new(
		double investment_account_sum );

/* Process */
/* ------- */
INVESTMENT_TRANSACTION *investment_transaction_calloc(
		void );

double investment_transaction_current_balance(
		ACCOUNT_JOURNAL *account_journal_latest );

double investment_transaction_delta(
		double investment_account_sum,
		double investment_transaction_current_balance );

/* Usage */
/* ----- */
TRANSACTION *investment_negative_transaction(
		char *investment_account_name,
		char *transaction_date_time,
		double transaction_amount,
		ENTITY_SELF *entity_self );

/* Usage */
/* ----- */
TRANSACTION *investment_positive_transaction(
		char *investment_account_name,
		char *transaction_date_time,
		double transaction_amount,
		ENTITY_SELF *entity_self );

#endif
