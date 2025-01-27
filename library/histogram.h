/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/histogram.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "list.h"
#include "boolean.h"

typedef struct
{
	LIST *number_list;
	char *query_where_display;
	char *system_string;
} HISTOGRAM;

/* Usage */
/* ----- */
HISTOGRAM *histogram_new(
		LIST *query_fetch_row_list,
		char *number_attribute_name,
		char *title,
		char *where_string,
		char *histogram_output_pdf );

/* Process */
/* ------- */
HISTOGRAM *histogram_calloc(
		void );

/* Usage */
/* ----- */
LIST *histogram_number_list(
		LIST *query_fetch_row_list,
		char *number_attribute_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *histogram_system_string(
		char *title,
		char *histogram_output_pdf,
		char *query_where_display );

/* Driver */
/* ------ */
void histogram_pdf_create(
		LIST *number_list,
		char *system_string );

#endif
