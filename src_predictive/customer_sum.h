/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/customer_sum.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "boolean.h"
#include "list.h"
#include "folder.h"

typedef struct
{
	char *entity_primary_where;
	char *first_sale_date;
	int sale_count;
	double gross_revenue;
	FOLDER *folder_fetch;
	boolean return_count_boolean;
	boolean inventory_sale_return_boolean;
	boolean specific_inventory_sale_return_boolean;
	int return_count;
	double return_total;
	double net_revenue;
	char *customer_sum_primary_data_string;
	boolean receivable_expected_boolean;
	boolean payable_due_boolean;
	CUSTOMER_ACCRUAL *customer_accrual;
	LIST *update_string_list;
} CUSTOMER_SUM;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
CUSTOMER_SUM *customer_sum_fetch(
		char *customer_full_name,
		char *customer_contact_key,
		boolean entity_contact_key_boolean );

/* Process */
/* ------- */
CUSTOMER_SUM *customer_sum_calloc(
		void );

#define CUSTOMER_SUM_NET_REVENUE(				\
		gross_revenue,					\
		return_total )					\
	( gross_revenue - return_total )

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *customer_sum_first_sale_date(
		const char *sale_table,
		char *entity_primary_where );

/* Usage */
/* ----- */
int customer_sum_sale_count(
		const char *sale_table,
		char *entity_primary_where );

/* Usage */
/* ----- */
double customer_sum_gross_revenue(
		const char *sale_table,
		char *entity_primary_where );

/* Usage */
/* ----- */
int customer_sum_return_count(
		const char *inventory_sale_return_table,
		const char *specific_inventory_sale_return_table,
		char *entity_primary_where,
		boolean inventory_sale_return_boolean,
		boolean specific_inventory_sale_return_boolean );

/* Usage */
/* ----- */
double customer_sum_return_total(
		const char *inventory_sale_table,
		const char *inventory_sale_return_table,
		const char *specific_inventory_sale_return_table,
		char *entity_primary_where,
		boolean entity_contact_key_boolean,
		boolean inventory_sale_return_boolean,
		boolean specific_inventory_sale_return_boolean );

/* Usage */
/* ----- */
LIST *customer_sum_update_string_list(
		char *customer_sum_primary_data_string,
		char *first_sale_date,
		int sale_count;
		double gross_revenue;
		boolean return_count_boolean;
		int return_count;
		double return_total;
		double net_revenue;
		LIST *customer_accrual_update_string_list );

/* Usage */

/* Returns heap memory */
/* ------------------- */
char *customer_sum_primary_data_string(
		const char sql_delimiter,
		char *full_name,
		char *contact_key,
		boolean entity_contact_key_boolean );

