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
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *service_name;
	double fixed_price;
	double estimated_hours;
	double discount_amount;
	double work_hours;
	double net_revenue;
	LIST *fixed_service_work_list;
	double fixed_service_work_hours;
	double fixed_service_sale_net_revenue;
} FIXED_SERVICE_SALE;

/* Usage */
/* ----- */
LIST *fixed_service_sale_list(
		const char *fixed_service_sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time );

/* Usage */
/* ----- */
FIXED_SERVICE_SALE *fixed_service_sale_parse(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FIXED_SERVICE_SALE *fixed_service_sale_new(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name );

/* Process */
/* ------- */
FIXED_SERVICE_SALE *fixed_service_sale_calloc(
		void );

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
FIXED_SERVICE_SALE *fixed_service_sale_seek(
		LIST *fixed_service_sale_list,
		char *service_name );

/* Usage */
/* ----- */
void fixed_service_sale_update(
		const char *fixed_service_sale_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		double fixed_service_work_hours,
		double fixed_service_sale_net_revenue );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *fixed_service_sale_update_system_string(
		const char *fixed_service_sale_table );

#endif

