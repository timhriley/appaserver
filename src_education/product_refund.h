/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/product_refund.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef PRODUCT_REFUND_H
#define PRODUCT_REFUND_H

#include "boolean.h"
#include "list.h"
#include "product.h"
#include "paypal_deposit.h"
#include "paypal_item.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define PRODUCT_REFUND_TABLE		"product_refund"

#define PRODUCT_REFUND_PRIMARY_KEY	"product_name"

#define PRODUCT_REFUND_INSERT_COLUMNS	"product_name,"			\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"season_name,"			\
					"year,"				\
					"deposit_date_time,"		\
					"product_refund_amount,"	\
					"fees_expense,"			\
					"net_payment_amount,"		\
					"transaction_date_time"

#define PRODUCT_REFUND_MEMO		"Product refund"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	PRODUCT *product;
	PAYPAL_DEPOSIT *paypal_deposit;

	/* Process */
	/* ------- */
	double product_refund_amount;
	double fees_expense;
	double net_payment_amount;

	char *transaction_date_time;
	TRANSACTION *product_refund_transaction;
} PRODUCT_REFUND;

PRODUCT_REFUND *product_refund_calloc(
			void );

PRODUCT_REFUND *product_refund_fetch(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_product,
			boolean fetch_paypal );

PRODUCT_REFUND *product_refund_parse(
			char *input,
			boolean fetch_product,
			boolean fetch_paypal );

PRODUCT_REFUND *product_refund_steady_state(
			int *transaction_seconds_to_add,
			PRODUCT_REFUND *product_refund,
			double deposit_amount,
			double paypal_transaction_fee );

PRODUCT_REFUND *product_refund(
			LIST *product_list,
			char *item_data,
			double item_value,
			double item_fee,
			/* -------- */
			/* Set only */
			/* -------- */
			PAYPAL_DEPOSIT *paypal_deposit );

void product_refund_set_transaction(
			int *transaction_seconds_to_add,
			PRODUCT_REFUND *product_refund,
			char *cash_account_name,
			char *account_fees_expense,
			char *revenue_account );

/* ---------------------------------------- */
/* Place functions in product_refund_fns.h */
/* ---------------------------------------- */
#endif

