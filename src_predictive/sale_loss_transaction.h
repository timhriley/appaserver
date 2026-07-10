/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/sale_loss_transaction.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef SALE_LOSS_TRANSACTION_H
#define SALE_LOSS_TRANSACTION_H

#include "list.h"
#include "boolean.h"
#include "account.h"
#include "subsidiary_transaction.h"
#include "subsidiary_transaction_state.h"

typedef struct
{
	boolean predictive_fund_boolean;
	boolean entity_contact_key_boolean;
	char *account_loss_string;
	ACCOUNT *debit_account;
	char *account_receivable_string;
	ACCOUNT *credit_account;
	LIST *journal_binary_list;
	SUBSIDIARY_TRANSACTION_STATE *subsidiary_transaction_state;
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;
} SALE_LOSS_TRANSACTION;

/* Usage */
/* ----- */
SALE_LOSS_TRANSACTION *sale_loss_transaction_new(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *uncollectible_date_time,
		char *state,
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key,
		char *preupdate_uncollectible_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		double sale_amount_due );

/* Process */
/* ------- */
SALE_LOSS_TRANSACTION *sale_loss_transaction_calloc(
		void );

#endif
