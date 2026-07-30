/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/hourly_service_work.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef HOURLY_SERVICE_WORK_H
#define HOURLY_SERVICE_WORK_H

#include "list.h"
#include "boolean.h"

#define HOURLY_SERVICE_WORK_TABLE	"hourly_service_work"

#define HOURLY_SERVICE_WORK_SELECT	"begin_work_date_time,"		\
					"end_work_date_time,"		\
					"work_description,"		\
					"activity,"			\
					"appaserver_full_name,"		\
					"appaserver_street_address,"	\
					"work_hours"

typedef struct
{
	char *begin_work_date_time;
	char *end_work_date_time;
	char *work_description;
	char *activity;
	char *appaserver_full_name;
	char *appaserver_street_address;
	double work_hours; /* from parse */
	double sale_work_hours; /* for update */
} HOURLY_SERVICE_WORK;

/* Usage */
/* ----- */
LIST *hourly_service_work_list(
		const char *hourly_service_work_select,
		const char *hourly_service_work_table,
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
HOURLY_SERVICE_WORK *hourly_service_work_parse(
		char *input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
HOURLY_SERVICE_WORK *hourly_service_work_new(
		char *begin_work_date_time );

/* Process */
/* ------- */
HOURLY_SERVICE_WORK *hourly_service_work_calloc(
		void );

/* Usage */
/* ----- */
HOURLY_SERVICE_WORK *hourly_service_work_fetch(
		const char *hourly_service_work_select,
		const char *hourly_service_work_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		char *begin_work_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *hourly_service_work_primary_where(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		char *begin_work_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
double hourly_service_work_hours(
		LIST *hourly_service_work_list );

#endif
