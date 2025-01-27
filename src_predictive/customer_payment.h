/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/customer_payment.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef CUSTOMER_PAYMENT_H
#define CUSTOMER_PAYMENT_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"
#include "entity.h"

/* Constants */
/* --------- */
#define CUSTOMER_PAYMENT_MEMO		"Customer Payment"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	ENTITY *customer_entity;
	char *sale_date_time;
	char *payment_date_time;
	double payment_amount;
	int check_number;
	TRANSACTION *customer_payment_transaction;
} CUSTOMER_PAYMENT;

/* Operations */
/* ---------- */
CUSTOMER_PAYMENT *customer_payment_new(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *payment_date_time );

LIST *customer_payment_list(
			char *full_name,
			char *street_address,
			char *sale_date_time );

/* Returns program memory */
/* ---------------------- */
char *customer_payment_select(
			void );

CUSTOMER_PAYMENT *customer_payment_parse(
			char *input );

char *customer_payment_sys_string(
			char *where );

LIST *customer_payment_system_list(
			char *sys_string );

double customer_payment_total(
			LIST *customer_payment_list );

TRANSACTION *customer_payment_transaction(
			char *full_name,
			char *street_address,
			char *payment_date_time,
			double payment_amount,
			char *account_cash,
			char *account_receivable );

#endif
