/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/specific_inventory_sale.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef SPECIFIC_INVENTORY_SALE_H
#define SPECIFIC_INVENTORY_SALE_H

#include "list.h"
#include "boolean.h"

#define SPECIFIC_INVENTORY_SALE_TABLE	"specific_inventory_sale"

#define SPECIFIC_INVENTORY_SALE_SELECT	"inventory_name,"		\
					"serial_label,"			\
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
	char *serial_label;
	double retail_price;
	double discount_amount;
	double extended_price;
	double cost_of_goods_sold;
	double sale_extended_price;
} SPECIFIC_INVENTORY_SALE;

/* Usage */
/* ----- */
LIST *specific_inventory_sale_list(
		const char *specific_inventory_sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time );

/* Usage */
/* ----- */
SPECIFIC_INVENTORY_SALE *specific_inventory_sale_parse(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SPECIFIC_INVENTORY_SALE *specific_inventory_sale_new(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *inventory_name,
		char *serial_label );

/* Process */
/* ------- */
SPECIFIC_INVENTORY_SALE *specific_inventory_sale_calloc(
		void );

/* Usage */
/* ----- */
double specific_inventory_sale_total(
		LIST *specific_inventory_sale_list );

/* Usage */
/* ----- */
double specific_inventory_sale_CGS_total(
		LIST *specific_inventory_sale_list );

/* Usage */
/* ----- */
SPECIFIC_INVENTORY_SALE *specific_inventory_sale_seek(
		LIST *specific_inventory_sale_list,
		char *inventory_name,
		char *serial_label );

/* Usage */
/* ----- */
void specific_inventory_sale_update(
		const char *specific_inventory_sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *inventory_name,
		char *serial_label,
		double sale_extended_price );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *specific_inventory_sale_update_system_string(
		const char *specific_inventory_sale_table );

#endif

