/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/service_purchase.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"

#define SERVICE_PURCHASE_TABLE		"service_purchase"

#define SERVICE_PURCHASE_SELECT		"service_name,"		\
					"service_cost,"		\
					"account"
typedef struct
{
	char *service_name;
	char *account_name;
	double service_cost;
} SERVICE_PURCHASE;

/* Usage */
/* ----- */
LIST *service_purchase_list(
		const char *prepaid_purchase_select,
		const char *prepaid_purchase_table,
		char *purchase_primary_where );

/* Usage */
/* ----- */
SERVICE_PURCHASE *service_purchase_parse(
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SERVICE_PURCHASE *service_purchase_new(
		char *service_name );

/* Process */
/* ------- */
SERVICE_PURCHASE *service_purchase_calloc(
		void );

/* Usage */
/* ----- */
double service_purchase_list_total(
		LIST *service_purchase_list );

