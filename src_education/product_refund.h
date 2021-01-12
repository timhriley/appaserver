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
#include "product_sale.h"
#include "paypal_item.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define PRODUCT_REFUND_TABLE		"product_refund"

#define PRODUCT_REFUND_PRIMARY_KEY	"product_name,"			\
					"sale_date_time,"		\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"refund_date_time"

#define PRODUCT_REFUND_INSERT_COLUMNS	"product_name,"			\
					"sale_date_time,"		\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"refund_date_time,"		\
					"refund_amount,"		\
					"net_payment_amount,"		\
					"transaction_date_time,"	\
					"merchant_fees_expense,"	\
					"paypal_date_time"

#define PRODUCT_REFUND_MEMO		"Product refund"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	PRODUCT_SALE *product_sale;
	ENTITY *payor_entity;
	char *sale_date_time;
	char *refund_date_time;
	double refund_amount;
	double merchant_fees_expense;
	char *paypal_date_time;

	/* Process */
	/* ------- */
	double net_refund_amount;

	TRANSACTION *product_refund_transaction;
	char *transaction_date_time;
} PRODUCT_REFUND;

PRODUCT_REFUND *product_refund_calloc(
			void );

PRODUCT_REFUND *product_refund_fetch(
			char *product_name,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time,
			boolean fetch_sale );

PRODUCT_REFUND *product_refund_parse(
			char *input,
			boolean fetch_sale );

PRODUCT_REFUND *product_refund_steady_state(
			PRODUCT_REFUND *product_refund,
			double refund_amount,
			double merchant_fees_expense );

FILE *product_refund_insert_open(
			char *error_filename );

void product_refund_insert_pipe(
			FILE *insert_pipe,
			char *product_name,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time,
			double refund_amount,
			double net_payment_amount,
			char *transaction_date_time,
			double merchant_fees_expense,
			char *paypal_date_time );

LIST *product_refund_system_list(
			char *sys_string,
			boolean fetch_sale );

char *product_refund_sys_string(
			char *where );

char *product_refund_primary_where(
			char *product_name,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time );

void product_refund_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *product_refund_list );

/* ------------------------------------------------------- */
/* Sets product_refund->product_refund_transaction and
	product_refund->transaction_date_time		   */
/* ------------------------------------------------------- */
void product_refund_set_transaction(
			int *transaction_seconds_to_add,
			PRODUCT_REFUND *product_refund,
			char *cash_account_name,
			char *account_fees_expense,
			char *revenue_account );

TRANSACTION *product_refund_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time,
			char *product_name,
			char *program_name,
			double refund_amount,
			double merchant_fees_expense,
			double net_payment_amount,
			char *entity_self_paypal_cash_account_name,
			char *account_fees_expense,
			char *product_revenue_account );

void product_refund_update(
			double net_refund_amount,
			char *transaction_date_time,
			char *product_name,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time );

FILE *product_refund_update_open(
			void );

void product_refund_trigger(
			char *product_name,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *refund_date_time,
			char *state );

void product_refund_list_insert(
			LIST *product_refund_list );

double product_refund_total(
			LIST *product_refund_list );

void product_refund_list_trigger(
			LIST *product_refund_list );

/* Safely returns heap memory */
/* -------------------------- */
char *product_refund_list_display(
			LIST *product_refund_list );

LIST *product_refund_transaction_list(
			LIST *product_refund_list );

LIST *product_refund_list_steady_state(
			LIST *product_refund_list,
			double refund_amount,
			double merchant_fees_expense );

/* Returns static memory */
/* --------------------- */
char *product_refund_memo(
			char *product_name );

void product_refund_list_payor_entity_insert(
			LIST *product_refund_list );

LIST *product_refund_list_paypal(
			ENTITY *payor_entity,
			char *paypal_date_time,
			LIST *paypal_item_list,
			LIST *product_list );

PRODUCT_REFUND *product_refund_paypal(
			ENTITY *payor_entity,
			char *paypal_date_time,
			double item_value,
			double item_fee,
			PRODUCT *product );

LIST *product_refund_list(
			char *where );

void product_refund_fetch_update(
			char *product_name );

LIST *product_refund_product_name_list(
			LIST *refund_list );

void product_refund_list_fetch_update(
			LIST *product_name_list );

#endif

