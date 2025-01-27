/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_appaserver/foreign_attribute.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "sql.h"
#include "piece.h"
#include "boolean.h"
#include "list.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "foreign_attribute.h"

FOREIGN_ATTRIBUTE *foreign_attribute_calloc( void )
{
	FOREIGN_ATTRIBUTE *foreign_attribute;

	if ( ! ( foreign_attribute =
			calloc( 1, sizeof ( FOREIGN_ATTRIBUTE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return foreign_attribute;
}

FOREIGN_ATTRIBUTE *foreign_attribute_new(
		char *folder_name,
		char *related_folder_name,
		char *related_attribute_name,
		char *foreign_attribute_name )
{
	FOREIGN_ATTRIBUTE *foreign_attribute = foreign_attribute_calloc();

	foreign_attribute->folder_name = folder_name;
	foreign_attribute->related_folder_name = related_folder_name;
	foreign_attribute->related_attribute_name = related_attribute_name;
	foreign_attribute->foreign_attribute_name = foreign_attribute_name;

	return foreign_attribute;
}

FOREIGN_ATTRIBUTE *foreign_attribute_parse( char *input )
{
	char folder_name[ 128 ];
	char related_folder_name[ 128 ];
	char related_attribute_name[ 128 ];
	char foreign_attribute_name[ 128 ];
	char foreign_key_index[ 128 ];
	FOREIGN_ATTRIBUTE *foreign_attribute;

	if ( !input || !*input ) return (FOREIGN_ATTRIBUTE *)0;

	/* See: FOREIGN_ATTRIBUTE_SELECT */
	/* ----------------------------- */
	piece( folder_name, SQL_DELIMITER, input, 0 );
	piece( related_folder_name, SQL_DELIMITER, input, 1 );
	piece( related_attribute_name, SQL_DELIMITER, input, 2 );
	piece( foreign_attribute_name, SQL_DELIMITER, input, 3 );

	foreign_attribute =
		foreign_attribute_new(
			strdup( folder_name ),
			strdup( related_folder_name ),
			strdup( related_attribute_name ),
			strdup( foreign_attribute_name ) );

	piece( foreign_key_index, SQL_DELIMITER, input, 4 );
	foreign_attribute->foreign_key_index = atoi( foreign_key_index );

	return foreign_attribute;
}

LIST *foreign_attribute_system_list( char *system_string )
{
	char input[ 1024 ];
	FILE *input_pipe;
	LIST *list = {0};

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


	input_pipe = appaserver_input_pipe( system_string );

	while ( string_input( input, input_pipe, 1024 ) )
	{
		if ( !list ) list = list_new();

		list_set(
			list,
			foreign_attribute_parse(
				input ) );
	}
	pclose( input_pipe );
	return list;
}

LIST *foreign_attribute_fetch_list( void )
{
	char *system_string;

	system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		foreign_attribute_system_string(
			FOREIGN_ATTRIBUTE_SELECT,
			FOREIGN_ATTRIBUTE_TABLE );

	return
	foreign_attribute_system_list(
		system_string );
}

char *foreign_attribute_system_string(
		char *foreign_attribute_select,
		char *foreign_attribute_table )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '%s' %s '1 = 1' table_name,foreign_key_index",
		foreign_attribute_select,
		foreign_attribute_table );

	return strdup( system_string );
}

LIST *foreign_attribute_list(
		char *many_folder_name,
		char *one_folder_name,
		char *related_attribute_name )
{
	static LIST *cache_list = {0};

	if ( !many_folder_name
	||   !one_folder_name
	||   !related_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !cache_list )
		cache_list =
			foreign_attribute_fetch_list();

	if ( !list_length( cache_list ) ) return NULL;

	return
	foreign_attribute_seek_list(
		many_folder_name,
		one_folder_name,
		related_attribute_name,
		cache_list /* foreign_attribute_list */ );
}

LIST *foreign_attribute_name_list(
		LIST *foreign_attribute_list )
{
	LIST *name_list = list_new();
	FOREIGN_ATTRIBUTE *foreign_attribute;

	if ( list_rewind( foreign_attribute_list ) )
	do {
		foreign_attribute = list_get( foreign_attribute_list );

		list_set(
			name_list,
			foreign_attribute->foreign_attribute_name );

	} while ( list_next( foreign_attribute_list ) );

	if ( !list_length( name_list ) )
	{
		list_free( name_list );
		name_list = NULL;
	}

	return name_list;
}

LIST *foreign_attribute_seek_list(
		char *many_folder_name,
		char *one_folder_name,
		char *related_attribute_name,
		LIST *foreign_attribute_list )
{
	LIST *list = {0};
	FOREIGN_ATTRIBUTE *foreign_attribute;

	if ( !many_folder_name
	||   !one_folder_name
	||   !related_attribute_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( foreign_attribute_list ) )
	do {
		foreign_attribute = list_get( foreign_attribute_list );

		if (	strcmp(
				foreign_attribute->folder_name,
				many_folder_name ) == 0
		&&	strcmp(
				foreign_attribute->related_folder_name,
				one_folder_name ) == 0
		&&	strcmp(
				foreign_attribute->related_attribute_name,
				related_attribute_name ) == 0 )
		{
			if ( !list ) list = list_new();

			list_set(
				list,
				foreign_attribute );
		}

	} while ( list_next( foreign_attribute_list ) );

	return list;
}

