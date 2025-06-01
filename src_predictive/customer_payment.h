/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/customer_payment.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef CUSTOMER_PAYMENT_H
#define CUSTOMER_PAYMENT_H

#include "list.h"
#include "boolean.h"
#include "customer_payment_transaction.h"

#define CUSTOMER_PAYMENT_TABLE		"customer_payment"

#define CUSTOMER_PAYMENT_SELECT		"payment_date_time,"	\
					"account,"		\
					"payment_amount,"	\
					"check_number"

#define CUSTOMER_PAYMENT_MEMO		"Customer Payment"

typedef struct
{
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *payment_date_time;
	char *account;
	double payment_amount;
	int check_number;
	CUSTOMER_PAYMENT_TRANSACTION *customer_payment_transaction;
} CUSTOMER_PAYMENT;

/* Usage */
/* ----- */
LIST *customer_payment_list(
		const char *customer_payment_select,
		const char *customer_payment_table,
		char *full_name,
		char *street_address,
		char *sale_date_time );

/* Usage */
/* ----- */
CUSTOMER_PAYMENT *customer_payment_parse(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *string_input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CUSTOMER_PAYMENT *customer_payment_new(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *payment_date_time );


/* Process */
/* ------- */
CUSTOMER_PAYMENT *customer_payment_calloc(
		void );

/* Usage */
/* ----- */
double customer_payment_total(
		LIST *customer_payment_list );

/* Usage */
/* ----- */
CUSTOMER_PAYMENT *customer_payment_seek(
		char *payment_date_time,
		LIST *customer_payment_list );

#endif
