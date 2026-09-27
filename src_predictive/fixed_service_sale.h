/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_service_sale.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"

#define FIXED_SERVICE_SALE_SELECT	"full_name,"			\
					"sale_date_time,"		\
					"service_name,"			\
					"fixed_price,"			\
					"estimated_hours,"		\
					"discount_amount,"		\
					"work_hours,"			\
					"net_revenue"

#define FIXED_SERVICE_SALE_TABLE	"fixed_service_sale"

typedef struct
{
	char *fund_name;
	char *full_name;
	char *contact_key;
	char *sale_date_time;
	char *service_name;
	double fixed_price;
	double estimated_hours;
	double discount_amount;
	double work_hours; /* from parse */
	double net_revenue; /* from parse */
	LIST *fixed_service_work_list;
	double fixed_service_work_hours; /* for update */
	double fixed_service_sale_net_revenue; /* for update */
	LIST *update_string_list;
} FIXED_SERVICE_SALE;

/* Usage */
/* ----- */
FIXED_SERVICE_SALE *fixed_service_sale_parse(
		boolean fund_boolean,
		boolean contact_key_boolean,
		boolean fixed_service_work_boolean,
		char *input );


/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FIXED_SERVICE_SALE *fixed_service_sale_new(
		char *service_name );

/* Process */
/* ------- */
FIXED_SERVICE_SALE *fixed_service_sale_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *fixed_service_sale_primary_where(
		const char *sale_service_name_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
#define FIXED_SERVICE_SALE_NET_REVENUE(				\
		fixed_price,					\
		discount_amount )				\
	( fixed_price - discount_amount )

/* Usage */
/* ----- */
LIST *fixed_service_sale_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		double fixed_service_work_hours,
		double fixed_service_sale_net_revenue );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *fixed_service_sale_primary_data_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

typedef struct
{
	LIST *list;
	LIST *primary_key_list;
	char *update_system_string;
	LIST *update_string_list;
	double revenue_total;
} FIXED_SERVICE_SALE_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FIXED_SERVICE_SALE_LIST *fixed_service_sale_list_new(
		const char *fixed_service_sale_select,
		const char *fixed_service_sale_table,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *where,
		boolean fixed_service_work_boolean );

/* Process */
/* ------- */
FIXED_SERVICE_SALE_LIST *fixed_service_sale_list_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *fixed_service_sale_list_select(
		const char *fixed_service_sale_select,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
LIST *fixed_service_sale_list_primary_key_list(
		const char *sale_service_name_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *fixed_service_sale_list_update_system_string(
		const char *fixed_service_sale_table,
		LIST *fixed_service_sale_list_primary_key_list );

/* Usage */
/* ----- */
LIST *fixed_service_sale_list_update_string_list(
		LIST *fixed_service_sale_list );

/* Usage */
/* ----- */
double fixed_service_sale_list_revenue_total(
		LIST *fixed_service_sale_list );

