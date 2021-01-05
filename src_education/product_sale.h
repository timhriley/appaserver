/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/product_sale.h	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef PRODUCT_SALE_H
#define PRODUCT_SALE_H

#include "boolean.h"
#include "list.h"
#include "product.h"
#include "paypal_item.h"
#include "transaction.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define PRODUCT_SALE_TABLE		"product_sale"

#define PRODUCT_SALE_PRIMARY_KEY	"product_name,"			\
					"sale_date_time,"		\
					"payor_full_name,"		\
					"payor_street_address"

#define PRODUCT_SALE_INSERT_COLUMNS	"product_name,"			\
					"sale_date_time,"		\
					"payor_full_name,"		\
					"payor_street_address,"		\
					"quantity,"			\
					"retail_price,"			\
					"extended_price,"		\
					"net_payment_amount,"		\
					"transaction_date_time,"	\
					"merchant_fees_expense,"	\
					"paypal_date_time"

#define PRODUCT_SALE_MEMO		"Product Sale"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	PRODUCT *product;
	/* PAYPAL_DEPOSIT *paypal_deposit; */
	ENTITY *payor_entity;
	char *sale_date_time;
	int quantity;
	double retail_price;
	double merchant_fees_expense;
	char *paypal_date_time;

	/* Process */
	/* ------- */
	double extended_price;
	double net_payment_amount;

	TRANSACTION *product_sale_transaction;
	char *transaction_date_time;
} PRODUCT_SALE;

PRODUCT_SALE *product_sale_calloc(
			void );

PRODUCT_SALE *product_sale_new(
			char *product_name,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address );

PRODUCT_SALE *product_sale_fetch(
			char *product_name,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address,
			boolean fetch_product );

PRODUCT_SALE *product_sale_parse(
			char *input,
			boolean fetch_product );

PRODUCT_SALE *product_sale_steady_state(
			PRODUCT_SALE *product_sale,
			int quantity,
			double retail_price,
			double merchant_fees_expense );

void product_sale_list_set_transaction(
			int *seconds_to_add,
			LIST *product_sale_list );

/* ------------------------------------------------------- */
/* Sets product_sale->product_sale_transaction and
	product_sale->transaction_date_time		   */
/* ------------------------------------------------------- */
void product_sale_set_transaction(
			int *seconds_to_add,
			PRODUCT_SALE *product_sale,
			char *cash_account_name,
			char *account_fees_expense,
			char *revenue_account );

FILE *product_sale_insert_open(
			char *error_filename );

void product_sale_insert_pipe(
			FILE *insert_pipe,
			char *product_name,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address,
			int quantity,
			double retail_price,
			double extended_price,
			double net_payment_amount,
			char *transaction_date_time,
			double merchant_fees_expense,
			char *paypal_date_time );

LIST *product_sale_system_list(
			char *sys_string,
			boolean fetch_product );

char *product_sale_sys_string(
			char *where );

char *product_sale_primary_where(
			char *product_name,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address );

void product_sale_list_set_transaction(
			int *transaction_seconds_to_add,
			LIST *product_sale_list );

TRANSACTION *product_sale_transaction(
			int *seconds_to_add,
			char *payor_full_name,
			char *payor_street_address,
			char *payment_date_time,
			char *product_name,
			char *program_name,
			double extended_price,
			double merchant_fees_expense,
			double net_payment_amount,
			char *entity_self_paypal_cash_account_name,
			char *account_fees_expense,
			char *product_revenue_account );

void product_sale_update(
			double extended_price,
			double net_payment_amount,
			char *transaction_date_time,
			char *product_name,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address );

FILE *product_sale_update_open(
			void );

void product_sale_trigger(
			char *product_name,
			char *sale_date_time,
			char *payor_full_name,
			char *payor_street_address,
			char *state );

void product_sale_list_insert(
			LIST *product_sale_list );

double product_sale_total(
			LIST *product_sale_list );

double product_sale_fee_total(
			LIST *product_sale_list );

void product_sale_list_trigger(
			LIST *product_sale_list );

/* Safely returns heap memory */
/* -------------------------- */
char *product_sale_list_display(
			LIST *product_sale_list );

LIST *product_sale_transaction_list(
			LIST *product_sale_list );

LIST *product_sale_list_steady_state(
			LIST *product_sale_list );

/* Returns static memory */
/* --------------------- */
char *product_sale_memo(
			char *product_name );

void product_sale_list_payor_entity_insert(
			LIST *product_sale_list );

LIST *product_sale_list_paypal(
			ENTITY *payor_entity,
			char *paypal_date_time,
			LIST *paypal_item_list,
			LIST *product_list );

PRODUCT_SALE *product_sale_paypal(
			ENTITY *payor_entity,
			char *paypal_date_time,
			double item_value,
			double item_fee,
			PRODUCT *product );

#endif

