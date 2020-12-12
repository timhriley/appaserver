/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/product_payment.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef PRODUCT_PAYMENT_H
#define PRODUCT_PAYMENT_H

#include "boolean.h"
#include "list.h"
#include "product.h"
#include "deposit.h"
#include "paypal_item.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define PRODUCT_PAYMENT_TABLE		"product_payment"

#define PRODUCT_PAYMENT_PRIMARY_KEY	"product_name"

#define PRODUCT_PAYMENT_INSERT_COLUMNS	"product_name,"			\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"season_name,"			\
					"year,"				\
					"deposit_date_time,"		\
					"product_payment_amount,"	\
					"fees_expense,"			\
					"net_payment_amount,"		\
					"transaction_date_time"

#define PRODUCT_PAYMENT_MEMO		"Product"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	PRODUCT *product;
	DEPOSIT *deposit;

	/* Process */
	/* ------- */
	double product_payment_amount;
	double fees_expense;
	double net_payment_amount;

	char *transaction_date_time;
	TRANSACTION *product_payment_transaction;
} PRODUCT_PAYMENT;

PRODUCT_PAYMENT *product_payment_calloc(
			void );

PRODUCT_PAYMENT *product_payment_fetch(
			char *product_name,
			char *payor_full_name,
			char *payor_street_address,
			char *season_name,
			int year,
			char *deposit_date_time,
			boolean fetch_product,
			boolean fetch_deposit );

PRODUCT_PAYMENT *product_payment_parse(
			char *input,
			boolean fetch_product,
			boolean fetch_deposit );

PRODUCT_PAYMENT *product_payment_steady_state(
			PRODUCT_PAYMENT *product_payment,
			double deposit_amount,
			double deposit_transaction_fee );

PRODUCT_PAYMENT *product_payment(
			PRODUCT *product,
			DEPOSIT *deposit );

/* ---------------------------------------- */
/* Place functions in product_payment_fns.h */
/* ---------------------------------------- */
#endif

