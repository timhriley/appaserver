/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/rgraph.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef RGRAPH_H
#define RGRAPH_H

#include "boolean.h"
#include "list.h"
#include "appaserver_link.h"
#include "query.h"

typedef struct
{
	char *date_time_string;
	double value;
} RGRAPH_POINT;

/* Usage */
/* ----- */
LIST *rgraph_point_list(
		char *date_attribute_name,
		char *time_attribute_name,
		char *number_attribute_name,
		LIST *query_fetch_row_list );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *rgraph_point_date_javascript(
		char *yyyy_mm_dd,
		char *hhmm );

/* Process */
/* ------- */
boolean rgraph_point_null_boolean(
		char *number_query_cell_select_datum );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *rgraph_point_date_time_string(
		QUERY_CELL *date_query_cell,
		QUERY_CELL *time_query_cell );

/* Process */
/* ------- */

/* Returns static memory or null */
/* ----------------------------- */
char *rgraph_point_hhmmss(
		QUERY_CELL *time_query_cell );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RGRAPH_POINT *rgraph_point_new(
		char *rgraph_point_date_time_string,
		double rgraph_point_value );

/* Process */
/* ------- */
RGRAPH_POINT *rgraph_point_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *rgraph_point_combine_string(
		char *attribute_name,
		LIST *rgraph_point_list,
		boolean date_time_boolean );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *rgraph_point_string(
		char *rgraph_point_date_time_string,
		double rgraph_point_value,
		boolean date_time_boolean );

#endif
