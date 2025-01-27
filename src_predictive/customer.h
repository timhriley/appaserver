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
	char *customer_street_address;
	boolean sales_tax_exempt_boolean;
	ENTITY *entity;

	/* Not used, yet */
	/* ------------- */
	char *customer_key;
} CUSTOMER;

/* Usage */
/* ----- */
CUSTOMER *customer_fetch(
		char *customer_full_name,
		char *customer_street_address,
		boolean fetch_entity_boolean );

/* Usage */
/* ----- */
CUSTOMER *customer_parse(
		char *customer_full_name,
		char *customer_street_address,
		boolean fetch_entity_boolean,
		char *string_pipe_input );


/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CUSTOMER *customer_new(
		char *customer_full_name,
		char *customer_street_address );

/* Process */
/* ------- */
CUSTOMER *customer_calloc(
		void );

#endif

