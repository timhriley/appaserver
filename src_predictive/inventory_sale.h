/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/inventory_sale.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef INVENTORY_SALE_H
#define INVENTORY_SALE_H

#include "list.h"
#include "boolean.h"

#define INVENTORY_SALE_TABLE		"inventory_sale"

#define INVENTORY_SALE_SELECT		"inventory_name,"		\
					"quantity,"			\
					"retail_price,"			\
					"discount_amount,"		\
					"extended_price,"		\
					"cost_of_goods_sold"

typedef struct
{
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *inventory_name;
	int quantity;
	double retail_price;
	double discount_amount;
	double extended_price;
	double cost_of_goods_sold;
	double sale_extended_price;
} INVENTORY_SALE;

/* Usage */
/* ----- */
LIST *inventory_sale_list(
		const char *inventory_sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time );

/* Usage */
/* ----- */
INVENTORY_SALE *inventory_sale_parse(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
INVENTORY_SALE *inventory_sale_new(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *inventory_name );

/* Process */
/* ------- */
INVENTORY_SALE *inventory_sale_calloc(
		void );

/* Usage */
/* ----- */
double inventory_sale_total(
		LIST *inventory_sale_list );

/* Usage */
/* ----- */
double inventory_sale_CGS_total(
		LIST *inventory_sale_list );

/* Usage */
/* ----- */
INVENTORY_SALE *inventory_sale_seek(
		LIST *inventory_sale_list,
		char *inventory_name );

/* Usage */
/* ----- */
void inventory_sale_update(
		const char *inventory_sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *inventory_name,
		double sale_extended_price );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *inventory_sale_update_system_string(
		const char *inventory_sale_table );

#endif

