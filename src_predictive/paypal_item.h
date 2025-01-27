/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/paypal_item.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef PAYPAL_ITEM_H
#define PAYPAL_ITEM_H

#include "boolean.h"
#include "list.h"
#include "String.h"
#include "entity.h"

typedef struct
{
	char *entity_delimited_item_title_P;
	char *transaction_type_E;
	LIST *allowed_item_list;
	double expected_revenue;
	ENTITY *benefit_entity;
	char *item_data;
	double item_value;
	double item_fee;
	double item_gain;
	boolean taken;
} PAYPAL_ITEM_PAYMENT;

/* Usage */
/* ----- */
PAYPAL_ITEM_PAYMENT *paypal_item_payment_new(
		char *item_data );

/* Process */
/* ------- */
PAYPAL_ITEM_PAYMENT *paypal_item_payment_calloc(
			void );

/* Usage */
/* ----- */
double paypal_item_expected_revenue_total(
		LIST *paypal_item_list );

/* Usage */
/* ----- */
int paypal_item_expected_revenue_length(
		LIST *paypal_item_list );

/* Usage */
/* ----- */
int paypal_item_nonexpected_revenue_length(
		LIST *paypal_item_list );

/* Usage */
/* ----- */

/* Returns item_title_P */
/* -------------------- */
char *paypal_item_entity_delimit(
		char *item_title_P );

double paypal_item_payment_value(
		double paypal_amount,
		double expected_revenue,
		double expected_revenue_total,
		int nonexpected_revenue_length );

double paypal_item_refund_value(
		double paypal_amount,
		double expected_revenue,
		double expected_revenue_total,
		int nonexpected_revenue_length );

double paypal_item_gain(
		double paypal_amount,
		double expected_revenue_total,
		int nonexpected_revenue_length,
		int expected_revenue_length );

double paypal_item_payment_fee(
		double paypal_amount,
		double expected_revenue,
		double transaction_fee,
		double item_value );

double paypal_item_refund_fee(
		double paypal_amount,
		double transaction_fee,
		double item_value );

typedef struct
{
	LIST *list;
} PAYPAL_ITEM_LIST;

/* Usage */
/* ----- */
LIST *paypal_item_list_benefit_entity(
		/* ------------------- */
		/* Expect stack memory */
		/* ------------------- */
		char *item_title_P,
		LIST *item_list );

/* Usage */
/* ----- */
LIST *paypal_item_list_no_entity(
		/* ------------------- */
		/* Expect stack memory */
		/* ------------------- */
		char *item_title_P,
		LIST *item_list );

/* Usage */
/* ----- */
boolean paypal_item_is_entity(
		char *entity_piece );

/* Usage */
/* ----- */
LIST *paypal_item_list(
		char *entity_delimited_item_title_P,
		char *transaction_type_E,
		LIST *allowed_item_list,
		LIST *event_label_list );

LIST *paypal_item_list_event(
			char *item_title_P,
			LIST *event_label_list );

#endif
