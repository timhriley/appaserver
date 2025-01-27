/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/work.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef WORK_H
#define WORK_H

#include "list.h"
#include "boolean.h"
#include "entity.h"

/* Constants */
/* --------- */
#define FIXED_SERVICE_WORK_TABLE	"fixed_service_work"
#define HOURLY_SERVICE_WORK_TABLE	"hourly_service_work"

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	ENTITY *customer_entity;
	char *begin_work_date_time;
	char *end_work_date_time;
	char *login_name;
	char *service_name;
	char *activity_name;
	double work_hours;
	double work_hours_database;
	double work_list_hours;
} WORK;

/* Operations */
/* ---------- */
WORK *work_new(
			char *full_name,
			char *street_address,
			char *service_name,
			char *service_description,
			char *begin_work_date_time );

/* Returns program memory */
/* ---------------------- */
char *work_fixed_select(
			void );
char *work_hourly_select(
			void );

WORK *work_parse(	char *input );

void work_fixed_update(
			double work_hours,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *begin_work_date_time );

void work_hourly_update(
			double work_hours,
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *service_description,
			char *begin_work_date_time );

FILE *work_fixed_update_open(
			void );

FILE *work_hourly_update_open(
			void );

double work_work_hours(
			LIST *work_list );

WORK *work_steady_state(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *service_description,
			char *begin_work_date_time,
			char *end_work_date_time,
			char *login_name,
			char *activity_name,
			double work_hours_database );

LIST *work_fixed_list(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name );

LIST *work_hourly_list(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *service_description );

double work_list_hours(
			LIST *work_list );

double work_hours(	char *begin_date_time,
			char *end_date_time );

#endif
