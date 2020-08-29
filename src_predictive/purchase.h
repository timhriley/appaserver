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

/* Constants */
/* --------- */
#define PURCHASE_MEMO		"Purchase Order"
#define PURCHASE_TABLE_NAME	"purchase_order"

#define Purchase_amount_due(						\
			purchase_amount,				\
			vendor_payment_total )				\
	( purchase_amount - vendor_payment_total )

#define Purchase_invoice_amount(					\
			equipment_total,				\
			sales_tax,					\
			freight_in )					\
	( equipment_total + sales + freight_in )

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
	double purchase_invoice_amount;
	double purchase_vendor_payment_total;
	double purchase_amount_due;
	char *purchase_asset_account_name;
	LIST *purchase_vendor_payment_list;
	LIST *purchase_equipment_list;
	double liability_payment_amount;
	char *program_name;
	char *property_street_address;
} PURCHASE;

/* Operations */
/* ---------- */
PURCHASE *purchase_fetch(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

/* --------------------------------- */
/* Allocates purchase->vendor_entity */
/* --------------------------------- */
PURCHASE *purchase_new(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

double purchase_equipment_total(
			LIST *equipment_purchase_list );

double purchase_vendor_payment_total(
			LIST *vendor_payment_list );

void purchase_update(	double purchase_equipment_total,
			double purchase_invoice_amount,
			double vendor_payment_total,
			double purchase_amount_due,
			char *transaction_date_time,
			char *full_name,
			char *street_address,
			char *purchase_date_time );

PURCHASE *purchase_calloc(
			void );

PURCHASE *purchase_seek(
			LIST *purchase_list,
			char *purchase_date_time );

LIST *purchase_vendor_payment_list(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

LIST *purchase_equipment_list(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

/* Returns program memory */
/* ---------------------- */
char *purchase_select(	void );

PURCHASE *purchase_parse(
			char *input );

char *purchase_sys_string(
			char *where,
			char *order );

/* Safely returns heap memory */
/* -------------------------- */
char *purchase_primary_where(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

double purchase_fetch_amount_due(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

/* Includes transaction->journal_list */
/* ---------------------------------- */
TRANSACTION *purchase_transaction(
			char *full_name,
			char *street_address,
			char *arrived_date_time,
			double purchase_invoice_amount,
			char *purchase_asset_account_name,
			char *account_payable );

/* Returns true transaction_date_time */
/* ---------------------------------- */
char *purchase_transaction_refresh(
			double transaction_amount,
			LIST *journal_list,
			char *purchase_transaction_memo,
			char *full_name,
			char *street_address,
			char *transaction_date_time );

LIST *purchase_journal_list(
			double purchase_invoice_amount,
			char *purchase_asset_account_name,
			char *account_payable );

char *purchase_asset_account_name(
			LIST *purchase_equipment_list );

LIST *purchase_system_list(
			char *sys_string );

LIST *purchase_amount_due_purchase_list(
			void );

FILE *purchase_update_open(
			void );

#endif
