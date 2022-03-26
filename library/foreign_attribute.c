/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_appaserver/foreign_attribute.c	*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "sql.h"
#include "piece.h"
#include "boolean.h"
#include "list.h"
#include "foreign_attribute.h"

FOREIGN_ATTRIBUTE *foreign_attribute_calloc( void )
{
	FOREIGN_ATTRIBUTE *foreign_attribute;

	if ( ! ( foreign_attribute =
			calloc( 1, sizeof( FOREIGN_ATTRIBUTE ) ) ) )
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
			char *related_folder,
			char *related_attribute,
			char *foreign_attribute_name )
{
	FOREIGN_ATTRIBUTE *foreign_attribute = foreign_attribute_calloc();

	foreign_attribute->folder_name = folder_name;
	foreign_attribute->related_folder = related_folder;
	foreign_attribute->related_attribute = related_attribute;
	foreign_attribute->foreign_attribute = foreign_attribute_name;

	return foreign_attribute;
}

FOREIGN_ATTRIBUTE *foreign_attribute_parse(
			char *input )
{
	char folder_name[ 128 ];
	char related_folder[ 128 ];
	char related_attribute[ 128 ];
	char foreign_attribute_name[ 128 ];
	char foreign_key_index[ 128 ];
	FOREIGN_ATTRIBUTE *foreign_attribute;

	if ( !input || !*input ) return (FOREIGN_ATTRIBUTE *)0;

	/* See: FOREIGN_ATTRIBUTE_SELECT */
	/* ----------------------------- */
	piece( folder_name, SQL_DELIMITER, input, 0 );
	piece( related_folder, SQL_DELIMITER, input, 1 );
	piece( related_attribute, SQL_DELIMITER, input, 2 );
	piece( foreign_attribute_name, SQL_DELIMITER, input, 3 );

	foreign_attribute =
		foreign_attribute_new(
			strdup( folder_name ),
			strdup( related_folder ),
			strdup( related_attribute ),
			strdup( foreign_attribute_name ) );

	piece( foreign_key_index, SQL_DELIMITER, input, 4 );
	foreign_attribute->foreign_key_index = atoi( foreign_key_index );

	return foreign_attribute;
}

LIST *foreign_attribute_system_list( char *system_string )
{
	char input[ 1024 ];
	FILE *input_pipe = popen( system_string, "r" );
	LIST *list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			list,
			foreign_attribute_parse(
				input ) );
	}
	pclose( input_pipe );
	return list;
}

char *foreign_attribute_system_string(
			char *foreign_attribute_select,
			char *foreign_attribute_table,
			char *where )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"select.sh '%s' %s \"%s\" folder,foreign_key_index",
		foreign_attribute_select,
		foreign_attribute_table,
		where );

	return strdup( system_string );
}

char *foreign_attribute_where(
			char *many_folder_name,
			char *one_folder_name )
{
	static char where[ 128 ];

	if ( !many_folder_name
	||   !one_folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(where,
		"folder = '%s' and	"
		"related_folder = '%s'	",
		many_folder_name,
		one_folder_name );

	return where;
}

LIST *foreign_attribute_list(
			char *many_folder_name,
			char *one_folder_name )
{
	if ( !many_folder_name
	||   !one_folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return foreign_attribute_system_list(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		foreign_attribute_system_string(
			FOREIGN_ATTRIBUTE_SELECT,
			FOREIGN_ATTRIBUTE_TABLE,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			foreign_attribute_where(
				many_folder_name,
	 			one_folder_name ) ) );
}

LIST *foreign_related_attribute_name_list(
			LIST *foreign_attribute_list )
{
	LIST *name_list;
	FOREIGN_ATTRIBUTE *foreign_attribute;

	if ( !list_rewind( foreign_attribute_list ) ) return (LIST *)0;

	name_list = list_new();

	do {
		foreign_attribute = list_get( foreign_attribute_list );

		list_set(
			name_list,
			foreign_attribute->related_attribute );

	} while ( list_next( foreign_attribute_list ) );

	return name_list;
}

LIST *foreign_attribute_name_list(
			LIST *foreign_attribute_list )
{
	LIST *name_list;
	FOREIGN_ATTRIBUTE *foreign_attribute;

	if ( !list_rewind( foreign_attribute_list ) ) return (LIST *)0;

	name_list = list_new();

	do {
		foreign_attribute = list_get( foreign_attribute_list );

		list_set(
			name_list,
			foreign_attribute->foreign_attribute );

	} while ( list_next( foreign_attribute_list ) );

	return name_list;
}

