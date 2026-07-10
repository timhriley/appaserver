/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/customer_payment_transaction.h	*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef CUSTOMER_PAYMENT_TRANSACTION_H
#define CUSTOMER_PAYMENT_TRANSACTION_H

#include "list.h"
#include "boolean.h"
#include "account.h"
#include "subsidiary_transaction.h"
#include "subsidiary_transaction_state.h"

typedef struct
{
	ACCOUNT *debit_account;
	char *account_receivable_string;
	ACCOUNT *credit_account;
	LIST *journal_binary_list;
	SUBSIDIARY_TRANSACTION_STATE *subsidiary_transaction_state;
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;
} CUSTOMER_PAYMENT_TRANSACTION;

/* Usage */
/* ----- */
CUSTOMER_PAYMENT_TRANSACTION *customer_payment_transaction_new(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *payment_date_time,
		char *state,
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key,
		char *preupdate_payment_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *cash_account_string,
		double payment_amount );

/* Process */
/* ------- */
CUSTOMER_PAYMENT_TRANSACTION *customer_payment_transaction_calloc(
		void );

#endif
