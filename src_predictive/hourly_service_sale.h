/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/hourly_service_sale.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef HOURLY_SERVICE_SALE_H
#define HOURLY_SERVICE_SALE_H

#include "list.h"
#include "boolean.h"

#define HOURLY_SERVICE_SALE_TABLE	"hourly_service_sale"

#define HOURLY_SERVICE_SALE_SELECT	"service_name,"			\
					"service_description,"		\
					"estimated_hours,"		\
					"hourly_rate,"			\
					"estimated_revenue,"		\
					"work_hours,"			\
					"net_revenue"

typedef struct
{
	char *service_name;
	char *service_description;
	double estimated_hours;
	double hourly_rate;
	double estimated_revenue;
	double work_hours; /* from parse */
	double net_revenue; /* from parse */
	double hourly_service_sale_estimated_revenue;
	LIST *hourly_service_work_list;
	double hourly_service_work_list_hours; /* for update */
	double hourly_service_sale_net_revenue; /* for update */
	LIST *update_string_list;
	LIST *primary_key_list;
	char *sale_update_system_string;
} HOURLY_SERVICE_SALE;

/* Usage */
/* ----- */
HOURLY_SERVICE_SALE *hourly_service_sale_fetch(
		const char *hourly_service_sale_select,
		const char *hourly_service_sale_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		boolean hourly_service_work_boolean );

/* Usage */
/* ----- */
HOURLY_SERVICE_SALE *hourly_service_sale_parse(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		boolean hourly_service_work_boolean,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
HOURLY_SERVICE_SALE *hourly_service_sale_new(
		char *service_name,
		char *service_description );

/* Process */
/* ------- */
HOURLY_SERVICE_SALE *hourly_service_sale_calloc(
		void );

/* Usage */
/* ----- */
LIST *hourly_service_sale_list(
		const char *hourly_service_sale_select,
		const char *hourly_service_sale_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		boolean hourly_service_work_boolean );

/* Usage */
/* ----- */
LIST *hourly_service_sale_primary_key_list(
		const char *sale_service_name_column,
		const char *sale_service_description_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *hourly_service_sale_primary_where(
		const char *sale_service_name_column,
		const char *sale_service_description_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
#define HOURLY_SERVICE_SALE_ESTIMATED_REVENUE(			\
		estimated_hours,				\
		hourly_rate )					\
	( estimated_hours * hourly_rate )


/* Usage */
/* ----- */
#define HOURLY_SERVICE_SALE_NET_REVENUE(			\
		hourly_service_work_list_hours,			\
		hourly_rate )					\
	( hourly_service_work_list_hours * hourly_rate )

/* Usage */
/* ----- */
double hourly_service_sale_total(
		LIST *hourly_service_sale_list );

/* Usage */
/* ----- */
LIST *hourly_service_sale_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		double hourly_service_sale_estimated_revenue,
		double hourly_service_work_hours,
		double hourly_service_sale_net_revenue );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *hourly_service_sale_primary_data_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
HOURLY_SERVICE_SALE *hourly_service_sale_trigger(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		char *state );

/* Usage */
/* ----- */
void hourly_service_sale_update(
		LIST *hourly_service_sale_update_string_list,
		char *sale_update_system_string );

#endif

