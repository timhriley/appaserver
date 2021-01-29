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
#include "related_folder.h"
#include "foreign_attribute.h"

FOREIGN_ATTRIBUTE *foreign_attribute_new(
			char *folder,
			char *related_folder,
			char *related_attribute,
			char *foreign_attribute_name )
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

	foreign_attribute->folder = folder;
	foreign_attribute->folder = related_folder;
	foreign_attribute->folder = related_attribute;
	foreign_attribute->folder = foreign_attribute_name;
	return foreign_attribute;
}

FOREIGN_ATTRIBUTE *foreign_attribute_parse(
			char *input )
{
	char folder[ 128 ];
	char related_folder[ 128 ];
	char related_attribute[ 128 ];
	char foreign_attribute_name[ 128 ];
	char foreign_key_index[ 128 ];
	FOREIGN_ATTRIBUTE *foreign_attribute;

	if ( !input || !*input ) return (FOREIGN_ATTRIBUTE *)0;

	/* See: attribute_list foreign_attribute */
	/* ------------------------------------- */
	piece( folder, SQL_DELIMITER, input, 0 );
	piece( related_folder, SQL_DELIMITER, input, 1 );
	piece( related_attribute, SQL_DELIMITER, input, 2 );
	piece( foreign_attribute_name, SQL_DELIMITER, input, 3 );

	foreign_attribute =
		foreign_attribute_new(
			strdup( folder ),
			strdup( related_folder ),
			strdup( related_attribute ),
			strdup( foreign_attribute_name ) );


	piece( foreign_key_index, SQL_DELIMITER, input, 4 );
	foreign_attribute->foreign_key_index = atoi( foreign_key_index );

	return foreign_attribute;
}

LIST *foreign_attribute_system_list(
			char *sys_string )
{
	char input[ 1024 ];
	FILE *input_pipe = popen( sys_string, "r" );
	LIST *foreign_attribute_list = list_new();

	while ( string_input( input, input_pipe, 1024 ) )
	{
		list_set(
			foreign_attribute_list,
			foreign_attribute_parse(
				input ) );
	}
	pclose( input_pipe );
	return foreign_attribute_list;
}

char *foreign_attribute_sys_string(
			char *where )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
		 "select.sh '*' %s \"%s\" folder,foreign_key_index",
		 FOREIGN_ATTRIBUTE_TABLE,
		 where );

	return strdup( sys_string );
}

LIST *foreign_attribute_list( char *one_folder_name )
{
	char where[ 128 ];

	sprintf(where,
		"related_folder = '%s'",
		one_folder_name );

	return foreign_attribute_system_list(
		foreign_attribute_sys_string(
			where ) );
}

LIST *foreign_attribute_list_folder(
			char *many_folder_name,
			LIST *foreign_attribute_list )
{
	LIST *return_list = {0};
	FOREIGN_ATTRIBUTE *foreign_attribute;

	if ( !list_rewind( foreign_attribute_list ) ) return (LIST *)0;

	do {
		foreign_attribute = list_get( foreign_attribute_list );

		if ( strcmp(	foreign_attribute->folder,
				many_folder_name ) == 0 )
		{
			if ( !return_list ) return_list = list_new();

			list_set(
				return_list,
				foreign_attribute );
		}

	} while ( list_next( foreign_attribute_list ) );

	return return_list;
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

