/* ---------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/paypal_item.h	*/
/* ---------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------	*/

#ifndef PAYPAL_ITEM_H
#define PAYPAL_ITEM_H

#include "boolean.h"
#include "list.h"
#include "entity.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *entity_delimited_item_title_P;
	char *transaction_type_E;
	int position_number;
	LIST *allowed_item_list;
	double expected_revenue;

	/* Process */
	/* ------- */
	ENTITY *benefit_entity;
	char *item_data;
	double item_value;
	double item_fee;
	double item_gain;
} PAYPAL_ITEM;

/* Prototypes */
/* ---------- */
PAYPAL_ITEM *paypal_item_calloc(
			void );

double paypal_item_expected_revenue_total(
			LIST *paypal_item_list );

int paypal_item_expected_revenue_length(
			LIST *paypal_item_list );

int paypal_item_nonexpected_revenue_length(
			LIST *paypal_item_list );

LIST *paypal_entity_item_list(
			/* ------------------- */
			/* Expect stack memory */
			/* ------------------- */
			char *item_title_P_piece,
			LIST *allowed_item_list );

/* Returns list of 1 */
/* ----------------- */
LIST *paypal_nonentity_item_list(
			/* ------------------- */
			/* Expect stack memory */
			/* ------------------- */
			char *item_title_P_piece,
			LIST *allowed_item_list );

boolean paypal_item_is_entity(
			char *entity_piece );

LIST *paypal_item_list(
			char *entity_delimited_item_title_P,
			char *transaction_type_E,
			LIST *allowed_item_list );

LIST *paypal_item_steady_state_list(
			LIST *paypal_item_list,
			double paypal_amount,
			double transaction_fee,
			double expected_revenue_total,
			int nonexpected_revenue_length,
			int expected_revenue_length );

PAYPAL_ITEM *paypal_item_steady_state(
			PAYPAL_ITEM *paypal_item,
			double expected_revenue,
			double paypal_amount,
			double transaction_fee,
			double expected_revenue_total,
			int nonexpected_revenue_length,
			int expected_revenue_length );

double paypal_item_value(
			double paypal_amount,
			double expected_revenue,
			double expected_revenue_total,
			int nonexpected_revenue_length );

double paypal_item_fee(	double paypal_amount,
			double expected_revenue,
			double transaction_fee,
			double item_value );

double paypal_item_gain(
			double paypal_amount,
			double expected_revenue_total,
			int nonexpected_revenue_length,
			int expected_revenue_length );

/* Returns item_title_P */
/* -------------------- */
char *paypal_item_entity_delimit(
			char *item_title_P );

#endif
