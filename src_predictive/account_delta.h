/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/account_delta.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef ACCOUNT_DELTA_H
#define ACCOUNT_DELTA_H

#include "boolean.h"
#include "list.h"
#include "account.h"
#include "entity_self.h"
#include "transaction.h"

#define ACCOUNT_DELTA_MEMO		"Mark to market"

#define ACCOUNT_DELTA_NO_CHANGE	\
	"<p>No change from current balance."

typedef struct
{
	char *balance_account_name;
	char *date_now19;
	ACCOUNT_JOURNAL *account_journal_latest;
	double amount;
	double current_balance;
	double delta;
	ENTITY_SELF *entity_self_fetch;
	boolean negative_boolean;
	char *debit_account_name;
	char *credit_account_name;
	double transaction_amount;
	TRANSACTION *transaction_binary;
} ACCOUNT_DELTA;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ACCOUNT_DELTA *account_delta_new(
		char *fund_name,
		char *debit_account_name,
		char *credit_account_name,
		boolean accumulate_debit_boolean,
		double ending_balance /* if liability, positive amount */,
		char *memo );

/* Process */
/* ------- */
ACCOUNT_DELTA *account_delta_calloc(
		void );

/* Usage */
/* ----- */

/* Returns either parameter */
/* ------------------------ */
char *account_delta_balance_account_name(
		char *debit_account_name,
		char *credit_account_name,
		boolean accumulate_debit_boolean );

/* Usage */
/* ----- */

/* Returns a component of account_journal_latest */
/* --------------------------------------------- */
double account_delta_current_balance(
		ACCOUNT_JOURNAL *account_journal_latest );

/* Usage */
/* ----- */
double account_delta_amount(
		double ending_balance,
		double account_delta_current_balance );

/* Usage */
/* ----- */
double account_delta_negative_boolean(
		double account_delta_amount );

/* Usage */
/* ----- */

/* Returns either parameter */
/* ------------------------ */
char *account_delta_debit_account_name(
		char *debit_account_name,
		char *credit_account_name,
		boolean account_delta_negative_boolean );

/* Usage */
/* ----- */

/* Returns either parameter */
/* ------------------------ */
char *account_delta_credit_account_name(
		char *debit_account_name,
		char *credit_account_name,
		boolean account_delta_negative_boolean );

/* Usage */
/* ----- */
double account_delta_transaction_amount(
		double account_delta_amount,
		boolean account_delta_negative_boolean );

#endif
