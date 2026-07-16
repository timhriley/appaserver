/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_service_sale.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FIXED_SERVICE_SALE_H
#define FIXED_SERVICE_SALE_H

#include "list.h"
#include "boolean.h"

#define FIXED_SERVICE_SALE_TABLE	"fixed_service_sale"

#define FIXED_SERVICE_SALE_SELECT	"service_name,"			\
					"fixed_price,"			\
					"estimated_hours,"		\
					"discount_amount,"		\
					"work_hours,"			\
					"net_revenue"

typedef struct
{
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
	LIST *primary_key_list;
	char *sale_update_system_string;
} FIXED_SERVICE_SALE;

/* Usage */
/* ----- */
LIST *fixed_service_sale_list(
		const char *fixed_service_sale_select,
		const char *fixed_service_sale_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		boolean fixed_service_work_boolean );

/* Usage */
/* ----- */
FIXED_SERVICE_SALE *fixed_service_sale_parse(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
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
FIXED_SERVICE_SALE *fixed_service_sale_fetch(
		const char *fixed_service_sale_select,
		const char *fixed_service_sale_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		boolean fixed_service_work_boolean );

/* Usage */
/* ----- */
LIST *fixed_service_sale_primary_key_list(
		const char *sale_service_name_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
void fixed_service_sale_update(
		LIST *update_string_list,
		char *sale_update_system_string );

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
double fixed_service_sale_net_revenue(
		double fixed_price,
		double discount_amount );

/* Usage */
/* ----- */
double fixed_service_sale_total(
		LIST *fixed_service_sale_list );

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

/* Driver */
/* ------ */
void fixed_service_sale_trigger(
		char *application_name,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *state );

#endif

