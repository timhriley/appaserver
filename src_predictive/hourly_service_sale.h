/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/hourly_service_sale.h		*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef HOURLY_SERVICE_SALE_H
#define HOURLY_SERVICE_SALE_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"
#include "entity.h"

/* Constants */
/* --------- */
#define HOURLY_SERVICE_SALE_TABLE	"hourly_service_sale"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	ENTITY *customer_entity;
	char *sale_date_time;
	char *service_name;
	char *service_description;
	double hourly_rate;
	double discount_amount;
	int estimated_hours;
	double work_hours_database;
	double hourly_service_work_hours;
	char *hourly_service_sale_revenue_account_name;
} HOURLY_SERVICE_SALE;

/* Operations */
/* ---------- */
HOURLY_SERVICE_SALE *hourly_service_sale_new(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *service_description );

/* Returns program memory */
/* ---------------------- */
char *hourly_service_sale_select(
			void );

char *hourly_service_sale_primary_where(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *service_description );

HOURLY_SERVICE_SALE *hourly_service_sale_parse(
			char *input );

void hourly_service_sale_update(
			double hourly_service_sale_work_hours,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *service_description );

FILE *hourly_service_sale_update_open(
			void );

double hourly_service_sale_work_hours(
			LIST *hourly_service_work_list );

HOURLY_SERVICE_SALE *hourly_service_sale_steady_state(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *service_description,
			double hourly_rate,
			int estimated_hours,
			double work_hours_database,
			LIST *hourly_service_work_list );

LIST *hourly_service_sale_list(
			char *full_name,
			char *street_address,
			char *sale_date_time );

char *hourly_service_sale_sys_string(
			char *where );

LIST *hourly_service_sale_system_list(
			char *sys_string );

char *fixed_service_sale_description_escape(
			char *service_description );

#endif

