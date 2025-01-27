/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/datatype.c				*/
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
#include "datatype.h"

LIST *datatype_list( void )
{
	char *system_string;
	FILE *input_pipe;
	LIST *list;
	char input[ 1024 ];
	DATATYPE *datatype;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			DATATYPE_SELECT,
			DATATYPE_TABLE,
			(char *)0 /* where */ );

	input_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_pipe(
			system_string );

	list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		datatype = datatype_parse( input );

		list_set(
			list,
			datatype );
	}

	pclose( input_pipe );

	return list;
}

DATATYPE *datatype_parse( char *input )
{
	char datatype_name[ 128 ];
	char buffer[ 128 ];
	DATATYPE *datatype;

	if ( !input )
	{
		char message[ 128 ];

		sprintf(message, "input is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	/* See DATATYPE_SELECT */
	/* ------------------- */
	piece( datatype_name, SQL_DELIMITER, input, 0 );

	datatype = 
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		datatype_new(
			strdup( datatype_name ) );

	piece( buffer, SQL_DELIMITER, input, 1 );
	if ( *buffer ) datatype->units = strdup( buffer );

	piece( buffer, SQL_DELIMITER, input, 2 );
	datatype->aggregation_sum_boolean = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 3 );
	datatype->bar_graph_boolean = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 4 );
	datatype->scale_graph_to_zero_boolean = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 5 );
	datatype->set_negative_values_to_zero_boolean = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 6 );
	datatype->calibrated_boolean = ( *buffer == 'y' );

	return datatype;
}

DATATYPE *datatype_new( char *datatype_name )
{
	DATATYPE *datatype;

	if ( !datatype_name )
	{
		char message[ 128 ];

		sprintf(message, "datatype_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	datatype = datatype_calloc();
	datatype->datatype_name = datatype_name;

	return datatype;
}

DATATYPE *datatype_calloc( void )
{
	DATATYPE *datatype;

	if ( ! ( datatype = calloc( 1, sizeof ( DATATYPE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return datatype;
}

DATATYPE *datatype_fetch( char *datatype_name )
{
	static LIST *list = {0};

	if ( !datatype_name )
	{
		char message[ 128 ];

		sprintf(message, "datatype_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list ) list = datatype_list();

	return
	datatype_seek(
		datatype_name,
		list );
}

DATATYPE *datatype_seek(
		char *datatype_name,
		LIST *datatype_list )
{
	DATATYPE *datatype;

	if ( !datatype_name )
	{
		char message[ 128 ];

		sprintf(message, "datatype_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( datatype_list ) ) return NULL;

	do {
		datatype = list_get( datatype_list );

		if ( strcasecmp( datatype->datatype_name, datatype_name ) == 0 )
		{
			return datatype;
		}

	} while ( list_next( datatype_list ) );

	return NULL;
}

