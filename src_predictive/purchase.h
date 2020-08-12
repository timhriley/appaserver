/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/purchase.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef PURCHASE_H
#define PURCHASE_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"
#include "entity.h"
#include "depreciation.h"
#include "account.h"

/* Constants */
/* --------- */
#define PURCHASE_ORDER_MEMO		"Purchase Order"

#define Purchase_amount_due(						\
			purchase_amount,				\
			vendor_payment_total )				\
	( purchase_amount - vendor_payment_total )

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	ENTITY *vendor_entity;
	char *purchase_date_time;
	double sales_tax;
	double freight_in;
	char *arrived_date_time;
	TRANSACTION *purchase_transaction;
	double purchase_equipment_total;
	double invoice_amount;
	double purchase_vendor_payment_total;
	double purchase_amount_due;
	char *purchase_asset_account_name;
	LIST *purchase_vendor_payment_list;
	LIST *purchase_equipment_list;
	char *program_name;
	char *property_street_address;
} PURCHASE_ORDER;

/* Operations */
/* ---------- */
PURCHASE_ORDER *purchase_order_fetch(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

PURCHASE_ORDER *purchase_order_new(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

double purchase_equipment_total(
			LIST *equipment_purchase_list );

double purchase_vendor_payment_total(
			LIST *vendor_payment_list );

void purchase_update(	double purchase_price_total,
			double invoice_amount,
			double vendor_payment_total,
			double amount_due,
			char *transaction_date_time,
			char *full_name,
			char *street_address,
			char *purchase_date_time );

PURCHASE_ORDER *purchase_order_calloc(
			void );

PURCHASE_ORDER *purchase_order_seek(
			LIST *purchase_order_list,
			char *purchase_date_time );

LIST *purchase_vendor_payment_list(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

LIST *purchase_fixed_asset_list(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

TRANSACTION *purchase_transaction(
			char *full_name,
			char *street_address,
			char *arrived_date_time,
			double purchase_amount,
			char *asset_account_name,
			char *account_payable );

void purchase_transaction_refresh(
			double transaction_amount,
			LIST *journal_list,
			char *full_name,
			char *street_address,
			char *transaction_date_time );

/* Returns program memory */
/* ---------------------- */
char *purchase_select(	void );

PURCHASE_ORDER *purchase_parse(
			char *input );

/* Safely returns heap memory */
/* -------------------------- */
char *purchase_order_primary_where(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

#endif
