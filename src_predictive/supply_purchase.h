/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/supply_purchase.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"

#define SUPPLY_PURCHASE_TABLE	"supply_purchase"

#define SUPPLY_PURCHASE_SELECT	"supply_name,"		\
				"quantity,"		\
				"unit_cost,"		\
				"extended_cost"
typedef struct
{
	char *supply_name;
	int quantity;
	double unit_cost;
	double extended_cost;
	char *update_string;
} SUPPLY_PURCHASE;

/* Usage */
/* ----- */
SUPPLY_PURCHASE *supply_purchase_parse(
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SUPPLY_PURCHASE *supply_purchase_new(
		char *supply_name );

/* Process */
/* ------- */
SUPPLY_PURCHASE *supply_purchase_calloc(
		void );

/* Usage */
/* ----- */
#define SUPPLY_PURCHASE_EXTENDED_COST(			\
		quantity,				\
		unit_cost )				\
	( (double)quantity * unit_cost )

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *supply_purchase_update_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		char *supply_name,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		double supply_purchase_extended_cost );

typedef struct
{
	LIST *list;
	LIST *primary_key_list;
	char *update_system_string;
	LIST *update_string_list;
} SUPPLY_PURCHASE_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SUPPLY_PURCHASE_LIST *supply_purchase_list_new(
		const char *supply_purchase_select,
		const char *supply_purchase_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *purchase_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *purchase_primary_where );

/* Process */
/* ------- */
SUPPLY_PURCHASE_LIST *supply_purchase_list_calloc(
		void );

/* Usage */
/* ----- */
LIST *supply_purchase_list_primary_key_list(
		const char *purchase_supply_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *supply_purchase_list_update_system_string(
		const char *supply_purchase_table,
		LIST *supply_purchase_list_primary_key_list );

/* Usage */
/* ----- */
double supply_purchase_list_total(
		LIST *supply_purchase_list );

/* Usage */
/* ----- */
LIST *supply_purchase_list_update_string_list(
		LIST *supply_purchase_list );

