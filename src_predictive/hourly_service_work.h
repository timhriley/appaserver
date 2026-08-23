/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/hourly_service_work.h		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#pragma once

#include "list.h"
#include "boolean.h"

#define HOURLY_SERVICE_WORK_TABLE	"hourly_service_work"

#define HOURLY_SERVICE_WORK_SELECT	"begin_work_date_time,"		\
					"end_work_date_time,"		\
					"work_description,"		\
					"activity,"			\
					"discount_hours,"		\
					"work_hours,"			\
					"appaserver_user_full_name"

typedef struct
{
	char *begin_work_date_time;
	char *end_work_date_time;
	char *work_description;
	char *activity;
	double discount_hours;
	char *appaserver_full_name;
	double work_hours; /* from parse */
	double hourly_service_work_hours; /* for update */
	LIST *primary_key_list;
	LIST *update_string_list;
	char *sale_update_system_string;
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
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
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
LIST *hourly_service_work_primary_key_list(
		const char *sale_begin_work_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
double hourly_service_work_hours(
		char *begin_work_date_time,
		char *end_work_date_time,
		double discount_hours );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *hourly_service_work_primary_where(
		const char *sale_begin_work_column,
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
double hourly_service_work_list_hours(
		LIST *hourly_service_work_list );

/* Usage */
/* ----- */
LIST *hourly_service_work_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		char *begin_work_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		double hourly_service_work_hours );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *hourly_service_work_primary_data_string(
		const char sql_delimiter,
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
HOURLY_SERVICE_WORK *hourly_service_work_trigger(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		char *begin_work_date_time,
		char *state );

/* Usage */
/* ----- */
void hourly_service_work_update(
		LIST *hourly_service_work_update_string_list,
		char *sale_update_system_string );
