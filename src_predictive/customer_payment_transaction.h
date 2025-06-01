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

/* Safely returns */
/* -------------- */
CUSTOMER_PAYMENT_TRANSACTION *customer_payment_transaction_new(
		char *full_name,
		char *street_address,
		char *payment_date_time,
		char *state,
		char *preupdate_full_name,
		char *preupdate_street_address,
		char *preupdate_payment_date_time,
		char *account_cash_string,
		double payment_amount );

/* Process */
/* ------- */
CUSTOMER_PAYMENT_TRANSACTION *customer_payment_transaction_calloc(
		void );

#endif
