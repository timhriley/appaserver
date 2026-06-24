/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/customer.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "boolean.h"
#include "list.h"
#include "entity.h"

#define CUSTOMER_TABLE			"customer"

#define CUSTOMER_SELECT			"sales_tax_exempt_yn"

typedef struct
{
	char *customer_full_name;
	char *customer_contact_key;
	boolean sales_tax_exempt_boolean;
	ENTITY *entity;
	double invoice_balance;
} CUSTOMER;

/* Usage */
/* ----- */
CUSTOMER *customer_fetch(
		char *customer_full_name,
		char *customer_contact_key,
		boolean entity_contact_key_boolean,
		boolean fetch_entity_boolean,
		boolean fetch_payable_balance_boolean );

/* Usage */
/* ----- */
CUSTOMER *customer_parse(
		char *customer_full_name,
		char *customer_contact_key,
		boolean entity_contact_key_boolean,
		boolean fetch_entity_boolean,
		boolean fetch_payable_balance_boolean,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CUSTOMER *customer_new(
		char *customer_full_name,
		char *customer_contact_key );

/* Process */
/* ------- */
CUSTOMER *customer_calloc(
		void );

/* Usage */
/* ----- */
double customer_invoice_balance(
		const char *account_payable_key,
		const char *account_receivable_key,
		char *customer_full_name,
		char *customer_contact_key );

#endif

