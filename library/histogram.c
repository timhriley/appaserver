/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/histogram.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "appaserver.h"
#include "appaserver_error.h"
#include "String.h"
#include "query.h"
#include "histogram.h"

HISTOGRAM *histogram_new(
		LIST *query_fetch_row_list,
		char *number_attribute_name,
		char *title,
		char *where_string,
		char *histogram_output_pdf )
{
	HISTOGRAM *histogram;

	if ( !number_attribute_name
	||   !histogram_output_pdf )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_length( query_fetch_row_list ) ) return NULL;

	histogram = histogram_calloc();

	histogram->number_list =
		histogram_number_list(
			query_fetch_row_list,
			number_attribute_name );

	histogram->query_where_display =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_where_display(
			where_string,
			80 /* max_length */ );

	histogram->system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		histogram_system_string(
			title,
			histogram_output_pdf,
			histogram->query_where_display );

	return histogram;
}

HISTOGRAM *histogram_calloc( void )
{
	HISTOGRAM *histogram;

	if ( ! ( histogram = calloc( 1, sizeof ( HISTOGRAM ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return histogram;
}

LIST *histogram_number_list(
		LIST *query_fetch_row_list,
		char *number_attribute_name )
{
	LIST *number_list;
	QUERY_ROW *query_row;
	QUERY_CELL *query_cell;

	if ( !number_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "number_attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( query_fetch_row_list ) ) return (LIST *)0;

	number_list = list_new();

	do {
		query_row = list_get( query_fetch_row_list );

		if ( ! ( query_cell =
				query_cell_seek(
					number_attribute_name,
					query_row->cell_list ) ) )
		{
			char message[ 128 ];

			sprintf(message,
				"query_cell_seek(%s) returned empty.",
				number_attribute_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set(
			number_list,
			query_cell->select_datum );

	} while ( list_next( query_fetch_row_list ) );

	return number_list;
}

char *histogram_system_string(
		char *title,
		char *histogram_output_pdf,
		char *query_where_display )
{
	char system_string[ 1024 ];

	if ( !histogram_output_pdf )
	{
		char message[ 128 ];

		sprintf(message, "histogram_output_pdf is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}


	sprintf(system_string,
		"r_histogram.sh %s \"%s\" \"%s\"",
		histogram_output_pdf,
		(title) ? title : "",
		(query_where_display) ? query_where_display : ""
	       		/* sub_title */ );

	return strdup( system_string );
}

void histogram_pdf_create(
		LIST *number_list,
		char *system_string )
{
	FILE *output_pipe;

	if ( !system_string )
	{
		char message[ 128 ];

		sprintf(message, "system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( number_list ) ) return;

	output_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			system_string );

	do {

		fprintf(output_pipe,
			"%s\n",
			(char *)list_get( number_list ) );

	} while ( list_next( number_list ) );

	pclose( output_pipe );
}

