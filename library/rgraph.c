/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/rgraph.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rgraph.h"

LIST *rgraph_point_list(
		char *date_attribute_name,
		char *time_attribute_name,
		char *number_attribute_name,
		LIST *query_fetch_row_list )
{
}

char *rgraph_point_date_javascript(
		char *yyyy_mm_dd,
		char *hhmm )
{
}

boolean rgraph_point_null_boolean( char *number_query_cell_select_datum )
{
}

char *rgraph_point_date_time_string(
		QUERY_CELL *date_query_cell,
		QUERY_CELL *time_query_cell )
{
}

char *rgraph_point_hhmmss( QUERY_CELL *time_query_cell )
{
}

RGRAPH_POINT *rgraph_point_new(
		char *rgraph_point_date_time_string,
		double rgraph_point_value )
{
	RGRAPH_POINT *rgraph_point;


	return rgraph_point;
}

RGRAPH_POINT *rgraph_point_calloc( void )
{
	RGRAPH_POINT *rgraph_point;

	if ( ! ( rgraph_point = calloc( 1, sizeof ( RGRAPH_POINT ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return rgraph_point;
}

char *rgraph_point_combine_string(
		char *attribute_name,
		LIST *rgraph_point_list,
		boolean date_time_boolean )
{
}

char *rgraph_point_string(
		char *rgraph_point_date_time_string,
		double rgraph_point_value,
		boolean date_time_boolean )
{
}
