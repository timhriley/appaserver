/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_sale.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef INVENTORY_SALE_H
#define INVENTORY_SALE_H

#include "list.h"
#include "boolean.h"
#include "entity.h"

/* Constants */
/* --------- */

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	ENTITY *customer_entity;
	char *sale_date_time;
	char *inventory_name;
	int quantity;
	double retail_price;
	double discount_amount;
	double inventory_sale_extended_price;
	double inventory_sale_cost_of_goods_sold;
} INVENTORY_SALE;

/* Operations */
/* ---------- */
INVENTORY_SALE *inventory_sale_new(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *inventory_name );

/* Returns program memory */
/* ---------------------- */
char *inventory_sale_select(
			void );

INVENTORY_SALE *inventory_sale_parse(
			char *input );

double inventory_sale_extended_price_total(
			LIST *inventory_sale_list );

double inventory_sale_sales_tax(
			double extended_price_total,
			double entity_state_sales_tax_rate );

double inventory_sale_extended_price(
			int quantity,
			double retail_price );

#endif
