/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/hourly_service_work.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef HOURLY_SERVICE_WORK_H
#define HOURLY_SERVICE_WORK_H

#include "list.h"
#include "boolean.h"

#define HOURLY_SERVICE_WORK_TABLE		"hourly_service_work"

#define HOURLY_SERVICE_WORK_SELECT		"begin_work_date_time,"	     \
						"end_work_date_time,"	     \
						"work_description,"	     \
						"activity,"		     \
						"appaserver_full_name,"	     \
						"appaserver_street_address," \
						"work_hours"

typedef struct
{
	char *full_name;
	char *street_address;
	char *sale_date_time;
	char *service_name;
	char *begin_work_date_time;
	char *end_work_date_time;
	char *work_description;
	char *activity;
	char *appaserver_full_name;
	char *appaserver_street_address;
	double work_hours;
} HOURLY_SERVICE_WORK;

/* Usage */
/* ----- */
LIST *hourly_service_work_list(
		const char *hourly_service_work_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *hourly_service_work_where(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name );

/* Usage */
/* ----- */
HOURLY_SERVICE_WORK *hourly_service_work_parse(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *string_input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
HOURLY_SERVICE_WORK *hourly_service_work_new(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *servicxe_name,
		char *begin_work_date_time );

/* Process */
/* ------- */
HOURLY_SERVICE_WORK *hourly_service_work_calloc(
		void );

/* Usage */
/* ----- */
double hourly_service_work_hours(
		char *begin_work_date_time,
		char *end_work_date_time );

/* Usage */
/* ----- */
double hourly_service_work_list_hours(
		LIST *hourly_service_work_list );

#endif
