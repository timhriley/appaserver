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
					"discount_amount,"		\
					"work_hours,"			\
					"net_revenue"

typedef struct
{
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *service_name;
	char *service_description;
	double estimated_hours;
	double hourly_rate;
	double estimated_revenue;
	double discount_amount;
	double work_hours; /* from parse */
	double net_revenue; /* from parse */
	double hourly_service_sale_estimated_revenue;
	LIST *hourly_service_work_list;
	double hourly_service_work_hours; /* for update */
	double hourly_service_sale_net_revenue; /* for update */
} HOURLY_SERVICE_SALE;

/* Usage */
/* ----- */
LIST *hourly_service_sale_list(
		const char *hourly_service_sale_select,
		const char *hourly_service_sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time );

/* Usage */
/* ----- */
HOURLY_SERVICE_SALE *hourly_service_sale_parse(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
HOURLY_SERVICE_SALE *hourly_service_sale_new(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *service_description );

/* Process */
/* ------- */
HOURLY_SERVICE_SALE *hourly_service_sale_calloc(
		void );

/* Usage */
/* ----- */
HOURLY_SERVICE_SALE *hourly_service_sale_fetch(
		const char *hourly_service_sale_select,
		const char *hourly_service_sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *service_description );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *hourly_service_sale_primary_where(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *service_description );

/* Usage */
/* ----- */
double hourly_service_sale_estimated_revenue(
		double estimated_hours,
		double hourly_rate,
		double discount_amount );

/* Usage */
/* ----- */
double hourly_service_sale_net_revenue(
		double hourly_service_work_list_hours,
		double hourly_rate,
		double discount_amount );

/* Usage */
/* ----- */
double hourly_service_sale_total(
		LIST *hourly_service_sale_list );

/* Usage */
/* ----- */
HOURLY_SERVICE_SALE *hourly_service_sale_seek(
		LIST *hourly_service_sale_list,
		char *service_name,
		char *service_description );

/* Usage */
/* ----- */
void hourly_service_sale_update(
		const char *hourly_service_sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		double hourly_service_sale_estimated_revenue,
		double hourly_service_work_hours,
		double hourly_service_sale_net_revenue );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *hourly_service_sale_update_system_string(
		const char *hourly_service_sale_table );

#endif

