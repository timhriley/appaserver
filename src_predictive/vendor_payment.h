/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/vendor_payment.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef VENDOR_PAYMENT_H
#define VENDOR_PAYMENT_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"
#include "purchase.h"
#include "entity.h"

/* Constants */
/* --------- */
#define VENDOR_PAYMENT_MEMO		"Vendor Payment"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	ENTITY *vendor_entity;
	char *purchase_date_time;
	char *payment_date_time;
	double payment_amount;
	int check_number;
	TRANSACTION *vendor_payment_transaction;
	PURCHASE *purchase;
} VENDOR_PAYMENT;

/* Operations */
/* ---------- */
VENDOR_PAYMENT *vendor_payment_fetch(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *payment_date_time );

VENDOR_PAYMENT *vendor_payment_new(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *payment_date_time );

LIST *vendor_payment_list_fetch(
			char *full_name,
			char *street_address,
			char *purchase_date_time );

/* Returns program memory */
/* ---------------------- */
char *vendor_payment_select(
			void );

VENDOR_PAYMENT *vendor_payment_parse(
			char *input );

LIST *vendor_payment_system_list(
			char *system_string );

double vendor_payment_total(
			LIST *vendor_payment_list );

void vendor_payment_insert(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *payment_date_time,
			double payment_amount,
			int check_number,
			char *transaction_date_time );

TRANSACTION *vendor_payment_transaction(
			char *full_name,
			char *street_address,
			char *payment_date_time,
			double payment_amount,
			int check_number,
			char *account_payable,
			char *account_cash,
			char *account_uncleared_checks );

VENDOR_PAYMENT *vendor_payment_seek(
			LIST *vendor_payment_list,
			char *payment_date_time );

LIST *vendor_payment_journal_list(
			double payment_amount,
			int check_number,
			char *account_payable,
			char *account_cash,
			char *account_uncleared_checks );

void vendor_payment_update(
			char *transaction_date_time,
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *payment_date_time );

FILE *vendor_payment_update_open(
			void );

/* Safely returns heap memory */
/* -------------------------- */
char *vendor_payment_purchase_where(
			char *full_name,
			char *street_address,
			char *purchase_date_time,
			char *payment_date_time );

/* Returns heap memory */
/* ------------------- */
char *vendor_payment_system_string(
			char *where );

#endif
