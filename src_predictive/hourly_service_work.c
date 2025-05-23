/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/hourly_service_work.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "sql.h"
#include "hourly_service_work.h"

LIST *hourly_service_work_list(
		const char *hourly_service_work_table,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name )
{
}

char *hourly_service_work_where(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name )
{
}

HOURLY_SERVICE_WORK *hourly_service_work_parse(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *string_input )
{
}

HOURLY_SERVICE_WORK *hourly_service_work_new(
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *servicxe_name,
		char *begin_work_date_time )
{
}

HOURLY_SERVICE_WORK *hourly_service_work_calloc( void )
{
}

double hourly_service_work_hours(
		char *begin_work_date_time,
		char *end_work_date_time )
{
}

double hourly_service_work_list_hours( LIST *hourly_service_work_list )
{
}
