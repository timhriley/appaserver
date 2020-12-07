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
#include "deposit.h"
#include "program_payment_item_title.h"
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

#define PRODUCT_REFUND_MEMO		"Product"

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
	double product_refund_amount;
	double fees_expense;
	double net_payment_amount;

	PROGRAM_PAYMENT_ITEM_TITLE *program_payment_item_title;
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
			boolean fetch_deposit );

PRODUCT_REFUND *product_refund_parse(
			char *input,
			boolean fetch_product,
			boolean fetch_deposit );

PRODUCT_REFUND *product_refund_steady_state(
			PRODUCT_REFUND *product_refund,
			double deposit_amount,
			double deposit_transaction_fee,
			double deposit_net_payment_amount );

PRODUCT_REFUND *product_refund(
			char *item_title_P,
			int product_number,
			LIST *education_product_list,
			/* -------- */
			/* Set only */
			/* -------- */
			DEPOSIT *deposit );

/* ---------------------------------------- */
/* Place functions in product_refund_fns.h */
/* ---------------------------------------- */
#endif

