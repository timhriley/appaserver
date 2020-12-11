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
	ENTITY *entity;
	char *item_data;
	double value;
	double fee;
	double gain;
} PAYPAL_ITEM;

/* Prototypes */
/* ---------- */
PAYPAL_ITEM *paypal_item_calloc(
			void );

PAYPAL_ITEM *paypal_item(
			LIST *not_exists_item_list,
			char *entity_delimited_item_title_P,
			char *transaction_type_E,
			int position_number,
			LIST *allowed_item_list );

double paypal_item_value(
			double expected_revenue,
			double deposit_amount,
			double expected_revenue_total,
			double nonexpected_list_length );

double paypal_item_fee(
			double deposit_amount,
			double transaction_fee,
			double paypal_item_value );

double paypal_gain_donation(
			double expected_revenue,
			double deposit_amount,
			double expected_revenue_total,
			double nonexpected_list_length );

ENTITY *paypal_entity(	char *full_name );

LIST *paypal_entity_item_list(
			LIST *not_found_item_list,
			char *item_title_P_piece,
			LIST *allowed_item_list );

/* Returns list of 1 */
/* ----------------- */
LIST *paypal_nonentity_item_list(
			LIST *not_found_item_list,
			char *item_data,
			LIST *allowed_item_list );

boolean paypal_item_is_entity(
			char *entity_piece );

LIST *paypal_item_list(
			LIST *not_found_item_list,
			char *entity_delimited_date_removed,
			char *transaction_type_E,
			LIST *allowed_item_list );

PAYPAL_ITEM *paypal_item_steady_state(
			PAYPAL_ITEM *paypal_item,
			double expected_revenue,
			double deposit_amount,
			double transaction_fee,
			double expected_revenue_total,
			int nonexpected_list_length );

#endif
