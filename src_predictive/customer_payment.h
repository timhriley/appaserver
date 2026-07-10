/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/customer_payment.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef CUSTOMER_PAYMENT_H
#define CUSTOMER_PAYMENT_H

#include "list.h"
#include "boolean.h"
#include "customer_payment_transaction.h"

#define CUSTOMER_PAYMENT_TABLE		"customer_payment"

#define CUSTOMER_PAYMENT_SELECT		"payment_date_time,"	\
					"account,"		\
					"payment_amount,"	\
					"check_number"

#define CUSTOMER_PAYMENT_MEMO		"Customer payment"
#define CUSTOMER_PAYMENT_DATE_COLUMN	"payment_date_time"

typedef struct
{
	char *payment_date_time;
	char *cash_account;
	double payment_amount;
	int check_number;

	/* Set by customer_payment_trigger_new() */
	/* ------------------------------------- */
	CUSTOMER_PAYMENT_TRANSACTION *customer_payment_transaction;
} CUSTOMER_PAYMENT;

/* Usage */
/* ----- */
LIST *customer_payment_list(
		const char *customer_payment_select,
		const char *customer_payment_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
CUSTOMER_PAYMENT *customer_payment_parse(
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CUSTOMER_PAYMENT *customer_payment_new(
		char *payment_date_time );

/* Process */
/* ------- */
CUSTOMER_PAYMENT *customer_payment_calloc(
		void );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CUSTOMER_PAYMENT *customer_payment_trigger_new(
		const char *customer_payment_select,
		const char *customer_payment_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *payment_date_time,
		char *state,
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key,
		char *preupdate_payment_date_time,
		boolean fund_boolean,
		boolean contact_key_boolean );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CUSTOMER_PAYMENT *customer_payment_fetch(
		const char *customer_payment_select,
		const char *customer_payment_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *payment_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *customer_payment_primary_where(
		const char *sale_payment_date_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *payment_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
double customer_payment_total(
		char *cash_account,
		double invoice_amount,
		LIST *customer_payment_list );

/* Driver */
/* ------ */
void customer_payment_trigger(
		char *application_name,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *payment_date_time,
		char *state,
		char *preupdate_fund_name,
		char *preupdate_full_name,
		char *preupdate_contact_key,
		char *preupdate_payment_date_time );

#endif
