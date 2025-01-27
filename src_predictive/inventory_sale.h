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
#define INVENTORY_SALE_TABLE		"inventory_sale"

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
	double extended_price_database;
	double cost_of_goods_sold_database;
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

double inventory_sale_total(
			LIST *inventory_sale_list );

double inventory_sale_extended_price(
			double retail_price,
			double discount_amount,
			int quantity );

LIST *inventory_sale_list(
			char *full_name,
			char *street_address,
			char *sale_date_time );

/* Everything is strdup() in. */
/* -------------------------- */
INVENTORY_SALE *inventory_sale_steady_state(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *inventory_name,
			int quantity,
			double retail_price,
			double discount_amount,
			double extended_price_database,
			double cost_of_goods_sold_database,
			LIST *inventory_purchase_list,
			LIST *inventory_sale_list );

double inventory_sale_extended_price(
			double retail_price,
			double discount_amount,
			int quantity );

double inventory_sale_cost_of_goods_sold(
			int quantity,
			LIST *inventory_purchase_list,
			LIST *inventory_sale_list );

char *inventory_sale_system_string(
			char *where );

LIST *inventory_sale_system_list(
			char *system_string );

#endif
