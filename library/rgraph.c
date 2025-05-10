/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/rgraph.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "appaserver_error.h"
#include "rgraph.h"

LIST *rgraph_point_list(
		char *date_attribute_name,
		char *time_attribute_name,
		char *number_attribute_name,
		LIST *query_fetch_row_list )
{
	LIST *point_list = list_new();
	QUERY_ROW *query_row;
	QUERY_CELL *number_query_cell;
	QUERY_CELL *date_query_cell;
	QUERY_CELL *time_query_cell = {0};
	char *date_time_string;
	double value;
	RGRAPH_POINT *rgraph_point;

	if ( list_rewind( query_fetch_row_list ) )
	do {
		query_row = list_get( query_fetch_row_list );

		if ( ! ( number_query_cell =
				query_cell_seek(
					number_attribute_name,
					query_row->cell_list ) ) )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"query_cell_seek(%s) returned empty.",
				number_attribute_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( rgraph_point_null_boolean(
			number_query_cell->select_datum ) )
				continue;

		if ( ! ( date_query_cell =
				query_cell_seek(
					date_attribute_name,
					query_row->cell_list ) ) )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"query_cell_seek(%s) returned empty.",
				date_attribute_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( time_attribute_name )
		{
			time_query_cell =
				query_cell_seek(
					time_attribute_name,
					query_row->cell_list );
		}

		date_time_string =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			rgraph_point_date_time_string(
				date_query_cell,
				time_query_cell );

		value = rgraph_point_value( number_query_cell );

		rgraph_point =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			rgraph_point_new(
				date_time_string,
				value );

		list_set( point_list, rgraph_point );

	} while ( list_next( query_fetch_row_list ) );

	if ( !list_length( point_list ) )
	{
		list_free( point_list );
		point_list = NULL;
	}

	return point_list;
}

boolean rgraph_point_null_boolean( char *select_datum )
{
	if ( select_datum && *select_datum ) 
		return 0;
	else
		return 1;
}

char *rgraph_point_date_time_string(
		QUERY_CELL *date_query_cell,
		QUERY_CELL *time_query_cell )
{
	char date_time_string[ 128 ];

	if ( !date_query_cell )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"date_query_cell is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	strcpy( date_time_string, date_query_cell->select_datum );

	if ( time_query_cell )
	{
		sprintf(
			date_time_string + strlen( date_time_string ),
			" %s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			rgraph_point_hhmmss(
				time_query_cell ) );
	}

	return strdup( date_time_string );
}

char *rgraph_point_hhmmss( QUERY_CELL *time_query_cell )
{
	char *select_datum = time_query_cell->select_datum;
	char destination[ 4 ];
	static char hhmmss[ 9 ];
	int how_many;

	/* Set hour: */
	/* --------- */
	if ( strlen( select_datum ) == 3 )
	{
		how_many = 1;

		sprintf(hhmmss,
			"0%s:",
			string_left(
				destination /* destination */,
				select_datum /* source */,
				how_many ) );
	}
	else
	{
		how_many = 2;

		sprintf(hhmmss,
			"%s:",
			string_left(
				destination /* destination */,
				select_datum /* source */,
				how_many ) );
	}

	/* Set minute:second */
	/* ----------------- */
	sprintf(
		hhmmss + strlen( hhmmss ),
		"%s:00",
		select_datum + how_many );

	return hhmmss;
}

RGRAPH_POINT *rgraph_point_new(
		char *date_time_string,
		double rgraph_point_value )
{
	RGRAPH_POINT *rgraph_point;

	if ( !date_time_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"date_time_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	rgraph_point = rgraph_point_calloc();

	rgraph_point->date_time_string = date_time_string;
	rgraph_point->value = rgraph_point_value;

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
	char combine_string[ STRING_64K ];
	char *ptr = combine_string;
	register int first_time = 1;
	RGRAPH_POINT *rgraph_point;

	if ( !attribute_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"attribute_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf( ptr,
		"%s <- c(\n",
		attribute_name );

	if ( list_rewind( rgraph_point_list ) )
	do {
		rgraph_point = list_get( rgraph_point_list );

		if ( string_strlen( combine_string ) + 2 >= STRING_64K )
		{
			char message[ 128 ];

			sprintf(message,
				STRING_OVERFLOW_TEMPLATE,
				STRING_64K );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( first_time )
			first_time = 0;
		else
		if ( !list_last_boolean( rgraph_point_list ) )
			ptr += sprintf( ptr, ",\n" );

		ptr += sprintf(
			ptr,
			"%s",
		/* Returns rgraph_point_date_time_string or static memory */
		/* ------------------------------------------------------ */
			rgraph_point_string(
				rgraph_point->date_time_string,
				rgraph_point->value,
				date_time_boolean ) );

	} while ( list_next( rgraph_point_list ) );

	ptr += sprintf( ptr, ")" );

	if ( first_time )
		return NULL;
	else
		return strdup( combine_string );
}

char *rgraph_point_string(
		char *date_time_string,
		double value,
		boolean date_time_boolean )
{
	static char point_string[ 128 ];

	if ( date_time_boolean )
	{
		if ( !date_time_string )
		{
			char message[ 128 ];

			snprintf(
				message,
				sizeof ( message ),
				"date_time_string is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		return date_time_string;
	}

	snprintf(
		point_string,
		sizeof ( point_string ),
		"%.2lf",
		value );

	return point_string;
}

double rgraph_point_value( QUERY_CELL *number_query_cell )
{
	if ( !number_query_cell )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"number_query_cell is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return atof( number_query_cell->select_datum );
}

