/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/fixed_service_work.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FIXED_SERVICE_WORK_H
#define FIXED_SERVICE_WORK_H

#include "list.h"
#include "boolean.h"

#define FIXED_SERVICE_WORK_TABLE	"fixed_service_work"

#define FIXED_SERVICE_WORK_SELECT	"begin_work_date_time,"		\
					"end_work_date_time,"		\
					"work_description,"		\
					"activity,"			\
					"appaserver_full_name,"		\
					"appaserver_street_address,"	\
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
	double work_hours; /* from parse */
	double sale_work_hours; /* for update */
} FIXED_SERVICE_WORK;

/* Usage */
/* ----- */
LIST *fixed_service_work_list(
		const char *fixed_service_work_select,
		const char *fixed_service_work_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *fixed_service_work_where(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name );

/* Usage */
/* ----- */
FIXED_SERVICE_WORK *fixed_service_work_parse(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *string_input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FIXED_SERVICE_WORK *fixed_service_work_new(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *begin_work_date_time );

/* Process */
/* ------- */
FIXED_SERVICE_WORK *fixed_service_work_calloc(
		void );

/* Usage */
/* ----- */
FIXED_SERVICE_WORK *fixed_service_work_fetch(
		const char *fixed_service_work_select,
		const char *fixed_service_work_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *begin_work_date_time );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *fixed_service_work_primary_where(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *begin_work_date_time );

/* Usage */
/* ----- */
double fixed_service_work_hours(
		LIST *fixed_service_work_list );

/* Usage */
/* ----- */
void fixed_service_work_update(
		const char *fixed_service_work_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *begin_work_date_time,
		double sale_work_hours );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *fixed_service_sale_update_system_string(
		const char *fixed_service_work_table );

#endif
