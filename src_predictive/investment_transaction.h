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

#define INVESTMENT_TRANSACTION_ASSET_MEMO	"Asset mark to market"
#define INVESTMENT_TRANSACTION_LIABILITY_MEMO	"Liability mark to market"

#define INVESTMENT_TRANSACTION_NO_CHANGE	\
	"<p>No change in account balance."

typedef struct
{
	char *date_now19;
	char *check_account_name;
	ACCOUNT_JOURNAL *account_journal_latest;
	double balance;
	double delta;
	ENTITY_SELF *entity_self_fetch;
	char *negative_debit_account;
	char *negative_credit_account;
	TRANSACTION *transaction_binary;
} INVESTMENT_TRANSACTION;

/* Usage */
/* ----- */
INVESTMENT_TRANSACTION *investment_transaction_new(
		const char *memo,
		char *fund_name,
		double account_sum,
		char *debit_account_name,
		char *credit_account_name,
		char *negative_account_name,
		boolean accumulate_debit_boolean );

/* Process */
/* ------- */
INVESTMENT_TRANSACTION *investment_transaction_calloc(
		void );

/* Returns either parameter */
/* ------------------------ */
char *investment_transaction_check_account_name(
		char *debit_account_name,
		char *credit_account_name,
		boolean accumulate_debit_boolean );

double investment_transaction_balance(
		ACCOUNT_JOURNAL *account_journal_latest );

double investment_transaction_delta(
		double account_sum,
		double investment_transaction_balance );

/* Returns either parameter */
/* ------------------------ */
char *investment_transaction_negative_debit_account(
		char *credit_account_name,
		char *negative_account_name );

/* Usage */
/* ----- */
INVESTMENT_TRANSACTION *investment_transaction_asset(
		const char *investment_account_key,
		const char *investment_gain_account_key,
		const char *investment_loss_account_key,
		const char *investment_transaction_asset_memo,
		char *fund_name,
		double investment_account_asset_sum );

/* Usage */
/* ----- */
INVESTMENT_TRANSACTION *investment_transaction_liability(
		const char *equity_key,
		const char *ira_tax_payable_key,
		const char *investment_transaction_liability_memo,
		char *fund_name,
		double investment_transaction_liability_amount );

/* Usage */
/* ----- */
boolean investment_transaction_boolean(
		char *full_name,
		char *as_of_date,
		char *investment_purpose );

/* Process */
/* ------- */
boolean investment_transaction_date_populated_boolean(
		char *as_of_date );

/* Usage */
/* ----- */
double investment_transaction_liability_amount(
		const double investment_ira_tax_percent,
		double investment_account_liability_sum );

/* Driver */
/* ------ */
void investment_transaction_output(
		const char *memo,
		boolean execute_boolean,
		INVESTMENT_TRANSACTION *investment_transaction );

#endif
