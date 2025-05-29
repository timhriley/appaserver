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
	char *account_loss;
	ACCOUNT *debit_account;
	char *account_receivable;
	ACCOUNT *credit_account;
	LIST *journal_binary_list;
	SUBSIDIARY_TRANSACTION_STATE *subsidiary_transaction_state;
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;
} SALE_LOSS_TRANSACTION;

/* Usage */
/* ----- */
SALE_LOSS_TRANSACTION *sale_loss_transaction_new(
		char *full_name,
		char *street_address,
		char *uncollectible_date_time,
		double sale_amount_due,
		char *state,
		char *preupdate_full_name,
		char *preupdate_street_address,
		char *preupdate_uncollectible_date_time );

/* Process */
/* ------- */
SALE_LOSS_TRANSACTION *sale_loss_transaction_calloc(
		void );

#endif
