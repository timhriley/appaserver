/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/work.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
#include "list.h"
#include "sql.h"
#include "list.h"
#include "boolean.h"
#include "entity.h"
#include "work.h"

WORK *work_new(	char *full_name,
		char *street_address,
		char *service_name,
		char *service_description,
		char *begin_work_date_time )
{
if ( full_name ){}
if ( street_address ){}
if ( service_name ){}
if ( service_description ){}
if ( begin_work_date_time ){}

	return (WORK *)0;
}

char *work_fixed_select( void )
{
	return (char *)0;
}

char *work_hourly_select( void )
{
	return (char *)0;
}

WORK *work_parse( char *input )
{
if ( input ){}
	return (WORK *)0;
}

void work_fixed_update(
		double work_hours,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *begin_work_date_time )
{
if ( work_hours ){}
if ( full_name ){}
if ( street_address ){}
if ( sale_date_time ){}
if ( service_name ){}
if ( begin_work_date_time ){}
}

void work_hourly_update(
		double work_hours,
		char *full_name,
		char *street_address,
		char *sale_date_time,
		char *service_name,
		char *service_description,
		char *begin_work_date_time )
{
if ( work_hours ){}
if ( full_name ){}
if ( street_address ){}
if ( sale_date_time ){}
if ( service_name ){}
if ( service_description ){}
if ( begin_work_date_time ){}
}

FILE *work_fixed_update_open( void )
{
	return (FILE *)0;
}

FILE *work_hourly_update_open( void )
{
	return (FILE *)0;
}

double work_work_hours( LIST *work_list )
{
if ( work_list ){}
	return 0.0;
}

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
			double work_hours_database )
{
if ( full_name ){}
if ( street_address ){}
if ( sale_date_time ){}
if ( service_name ){}
if ( service_description ){}
if ( begin_work_date_time ){}
if ( end_work_date_time ){}
if ( login_name ){}
if ( activity_name ){}
if ( work_hours_database ){}
	return (WORK *)0;
}

LIST *work_fixed_list(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name )
{
if ( full_name ){}
if ( street_address ){}
if ( sale_date_time ){}
if ( service_name ){}
	return (LIST *)0;
}

LIST *work_hourly_list(
			char *full_name,
			char *street_address,
			char *sale_date_time,
			char *service_name,
			char *service_description )
{
if ( full_name ){}
if ( street_address ){}
if ( sale_date_time ){}
if ( service_name ){}
if ( service_description ){}
	return (LIST *)0;
}

double work_list_hours( LIST *work_list )
{
if ( work_list ){}
	return 0.0;
}

double work_hours(	char *begin_date_time,
			char *end_date_time )
{
if ( begin_date_time ){}
if ( end_date_time ){}
	return 0.0;
}
