/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_service_sale.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FIXED_SERVICE_SALE_H
#define FIXED_SERVICE_SALE_H

#include "list.h"
#include "boolean.h"
#include "transaction.h"
#include "entity.h"

/* Constants */
/* --------- */
#define FIXED_SERVICE_SALE_TABLE	"fixed_service_sale"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	ENTITY *customer_entity;
	char *sale_date_time;
	char *service_name;
	double fixed_price;
	int estimated_hours;
	double work_hours_database;
	double fixed_service_work_hours;
	double fixed_service_sale_net_revenue;
	char *fixed_service_sale_revenue_account_name;
	LIST *fixed_service_work_list;
} FIXED_SERVICE_SALE;

/* Operations */
/* ---------- */
FIXED_SERVICE_SALE *fixed_service_sale_new(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name );

/* Returns program memory */
/* ---------------------- */
char *fixed_service_sale_select(
			void );

char *fixed_service_sale_primary_where(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name );

/* Returns fixed_service_sale_steady_state() */
/* ----------------------------------------- */
FIXED_SERVICE_SALE *fixed_service_sale_parse(
			char *input );

void fixed_service_sale_update(
			double fixed_service_sale_work_hours,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name );

FILE *fixed_service_sale_update_open(
			void );

FIXED_SERVICE_SALE *fixed_service_sale_steady_state(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			double fixed_price,
			int estimated_hours,
			double work_hours_database,
			LIST *fixed_service_work_list );

LIST *fixed_service_sale_list(
			char *full_name,
			char *street_address,
			char *sale_date_time );

char *fixed_service_sale_sys_string(
			char *where );

LIST *fixed_service_sale_system_list(
			char *sys_string );

double fixed_service_sale_total(
			LIST *fixed_service_sale_list );

double fixed_service_sale_net_revenue(
			double fixed_price,
			double discount_amount );

FIXED_SERVICE_SALE *fixed_service_sale_seek(
			LIST *fixed_service_sale_list,
			char *service_name );

#endif

