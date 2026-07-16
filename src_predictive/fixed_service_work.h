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

#define FIXED_SERVICE_WORK_SELECT	"begin_work_date_time,"	\
					"end_work_date_time,"	\
					"activity,"		\
					"work_hours"

typedef struct
{
	char *begin_work_date_time;
	char *end_work_date_time;
	char *activity;
	double work_hours; /* from parse */
	double sale_work_hours; /* for update */
	LIST *primary_key_list;
	LIST *update_string_list;
	char *sale_update_system_string;
} FIXED_SERVICE_WORK;

/* Usage */
/* ----- */
LIST *fixed_service_work_list(
		const char *fixed_service_work_select,
		const char *fixed_service_work_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
FIXED_SERVICE_WORK *fixed_service_work_parse(
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		char *string_input );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
FIXED_SERVICE_WORK *fixed_service_work_new(
		char *begin_work_date_time,
		char *end_work_date_time,
		char *activity,
		double work_hours );

/* Process */
/* ------- */
FIXED_SERVICE_WORK *fixed_service_work_calloc(
		void );

/* Usage */
/* ----- */
FIXED_SERVICE_WORK *fixed_service_work_fetch(
		const char *fixed_service_work_select,
		const char *fixed_service_work_table,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *begin_work_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
LIST *fixed_service_work_primary_key_list(
		const char *sale_begin_work_column,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
void fixed_service_work_update(
		LIST *update_string_list,
		char *sale_update_system_string );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *fixed_service_work_primary_where(
		const char *sale_begin_work_column,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *begin_work_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Usage */
/* ----- */
double fixed_service_work_hours(
		LIST *fixed_service_work_list );

/* Usage */
/* ----- */
LIST *fixed_service_work_update_string_list(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *begin_work_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean,
		double sale_work_hours );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *fixed_service_work_primary_data_string(
		const char sql_delimiter,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *begin_work_date_time,
		boolean predictive_fund_boolean,
		boolean entity_contact_key_boolean );

/* Driver */
/* ------ */
void fixed_service_work_trigger(
		char *application_name,
		char *fund_name,
		char *full_name,
		char *contact_key,
		char *sale_date_time,
		char *service_name,
		char *begin_work_date_time,
		char *state );

#endif
