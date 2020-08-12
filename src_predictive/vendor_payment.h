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
#include "entity.h"

/* Constants */
/* --------- */
#define VENDOR_PAYMENT_PAYMENT_MEMO		"Vendor Payment"

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

LIST *vendor_payment_list(
			char *purchase_order_where );

/* Returns program memory */
/* ---------------------- */
char *vendor_payment_select(
			void );

VENDOR_PAYMENT *vendor_payment_parse(
			char *input );

LIST *vendor_payment_list_fetch(
			char *where );

LIST *vendor_payment_system_list(
			char *sys_string );

double vendor_payment_total(
			LIST *vendor_payment_list );

#endif
