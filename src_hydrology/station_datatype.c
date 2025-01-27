/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/station_datatype.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "station.h"
#include "widget.h"
#include "station_datatype.h"

LIST *station_datatype_list(
		char *station_name,
		char *datatype_name,
		char *where,
		boolean fetch_datatype )
{
	char *system_string;
	FILE *input_pipe;
	LIST *list;
	char input[ 1024 ];
	STATION_DATATYPE *station_datatype;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			STATION_DATATYPE_SELECT,
			STATION_DATATYPE_TABLE,
			/* ------------------------------ */
			/* Returns static memory or where */
			/* ------------------------------ */
			station_datatype_where(
				station_name,
				datatype_name,
				where ) );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		station_datatype =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			station_datatype_parse(
				fetch_datatype,
				input );

		list_set(
			list,
			station_datatype );
	}

	pclose( input_pipe );

	return list;
}

STATION_DATATYPE *station_datatype_parse(
		boolean fetch_datatype,
		char *input )
{
	char station_name[ 128 ];
	char datatype_name[ 128 ];
	char buffer[ 128 ];
	STATION_DATATYPE *station_datatype;

	if ( !input || !*input )
	{
		char message[ 128 ];

		sprintf(message, "input is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}
 
	/* See STATION_DATATYPE_SELECT */
	/* --------------------------- */
	piece( station_name, SQL_DELIMITER, input, 0 );
	piece( datatype_name, SQL_DELIMITER, input, 1 );

	station_datatype =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		station_datatype_new(
			strdup( station_name ),
			strdup( datatype_name ) );

	piece( buffer, SQL_DELIMITER, input, 2 );
	station_datatype->validation_required_boolean = (*buffer == 'y');

	piece( buffer, SQL_DELIMITER, input, 3 );
	station_datatype->plot_for_station_check_boolean = (*buffer == 'y');

	piece( buffer, SQL_DELIMITER, input, 4 );
	if ( *buffer )
		station_datatype->sensor_depth_ft =
			atof( buffer );

	piece( buffer, SQL_DELIMITER, input, 5 );
	if ( *buffer )
		station_datatype->manipulate_agency =
			strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 6 );
	station_datatype->bypass_data_collection_frequency_boolean =
		(*buffer == 'y');

	if ( fetch_datatype )
	{
		if ( ! ( station_datatype->datatype =
				datatype_fetch(
					station_datatype->
						datatype_name ) ) )
		{
			char message[ 128 ];

			sprintf(message,
				"datatype_fetch(%s) returned empty.",
				station_datatype->datatype_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}
	}

	return station_datatype;
}

STATION_DATATYPE *station_datatype_new(
		char *station_name,
		char *datatype_name )
{
	STATION_DATATYPE *station_datatype;

	if ( !station_name
	||   !datatype_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	station_datatype = station_datatype_calloc();
	station_datatype->station_name = station_name;
	station_datatype->datatype_name = datatype_name;

	return station_datatype;
}

STATION_DATATYPE *station_datatype_calloc( void )
{
	STATION_DATATYPE *station_datatype;

	if ( ! ( station_datatype =
			calloc( 1,
				sizeof ( STATION_DATATYPE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return station_datatype;
}

void station_datatype_output(
		LIST *station_datatype_list )
{
	STATION_DATATYPE *station_datatype;

	if ( !list_rewind( station_datatype_list ) ) return;

	do {
		station_datatype =
			list_get(
				station_datatype_list );

		if ( !station_datatype->datatype )
		{
			char message[ 128 ];

			sprintf(message,
				"station_datatype->datatype is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		printf(	"%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			station_datatype_string(
				WIDGET_DROP_DOWN_LABEL_DELIMITER,
				station_datatype->station_name,
				station_datatype->datatype_name,
			       	station_datatype->datatype->units ) );

	} while ( list_next( station_datatype_list ) );
}

void station_datatype_site_visit_calibrate_output(
		LIST *station_datatype_list )
{
	STATION_DATATYPE *station_datatype;

	if ( !list_rewind( station_datatype_list ) ) return;

	do {
		station_datatype =
			list_get(
				station_datatype_list );

		if ( !station_datatype->datatype )
		{
			char message[ 128 ];

			sprintf(message,
				"station_datatype->datatype is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		if ( !station_datatype->datatype->calibrated_boolean )
			continue;

		printf(	"%s\n",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			station_datatype_string(
				WIDGET_DROP_DOWN_LABEL_DELIMITER,
				station_datatype->station_name,
				station_datatype->datatype_name,
			       	station_datatype->datatype->units ) );

	} while ( list_next( station_datatype_list ) );
}

char *station_datatype_string(
		char widget_drop_down_label_delimiter,
		char *station_name,
		char *datatype_name,
		char *units )
{
	char buffer[ 128 ];
	static char string[ 256 ];
	char *ptr = string;

	if ( !station_name
	||   !datatype_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		"%s%c%s%c%s%c%s",
		station_name,
		SQL_DELIMITER,
		datatype_name,
		widget_drop_down_label_delimiter,
		station_name,
		SQL_DELIMITER,
		string_initial_capital(
			buffer,
			datatype_name ) );

	if ( units )
	{
		ptr += sprintf(
			ptr,
			" [%s]",
			string_initial_capital(
				buffer,
				units ) );
	}

	return string;
}

char *station_datatype_where(
		char *station_name,
		char *datatype_name,
		char *where )
{
	static char return_where[ 128 ];
	char *ptr = return_where;

	if ( where && *where ) return where;

	*return_where = '\0';

	if ( station_name
	&&   *station_name
	&&   strcmp( station_name, "station" ) != 0 )
	{
		char *in_clause;
		LIST *data_list;

		data_list =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			list_string_list(
				station_name,
				SQL_DELIMITER );

		in_clause =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			string_in_clause( data_list );

		ptr += sprintf( ptr, "station in (%s)", in_clause );

		free( in_clause );
		list_free( data_list );
	}

	if ( datatype_name
	&&   *datatype_name
	&&   strcmp( datatype_name, "datatype" ) != 0 )
	{
		char *in_clause;
		LIST *data_list;

		data_list =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			list_string_list(
				datatype_name,
				SQL_DELIMITER );

		in_clause =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			string_in_clause( data_list );

		if ( ptr != return_where ) ptr += sprintf( ptr, " and " );

		ptr += sprintf( ptr, "datatype in (%s)", in_clause );

		free( in_clause );
		list_free( data_list );
	}

	return return_where;
}
