/* $APPASERVER_HOME/library/folder_attribute.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "sql.h"
#include "piece.h"
#include "attribute.h"
#include "folder_attribute.h"

FOLDER_ATTRIBUTE *folder_attribute_new(
			char *folder_name,
			char *attribute_name )
{
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( ! ( folder_attribute = calloc( 1, sizeof( FOLDER_ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	folder_attribute->folder_name = folder_name;
	folder_attribute->attribute_name = attribute_name;

	return folder_attribute;
}

char *folder_attribute_where(
			char *sql_injection_escape_folder_name,
			LIST *exclude_attribute_name_list )
{
	char where[ STRING_WHERE_BUFFER ];
	char *ptr;

	if ( list_length( exclude_attribute_name_list ) )
	{
		sprintf(where,
			"folder = '%s' and attribute not in (%s)",
			sql_injection_escape_folder_name,
			/* -------------------------- */
			/* Safely returns heap memory */
			/* -------------------------- */
			( ptr = string_in_clause(
				exclude_attribute_name_list ) ) );
		free( ptr );
	}
	else
	{
		sprintf(where,
			"folder = '%s'",
			sql_injection_escape_folder_name );
	}

	return strdup( where );
}

LIST *folder_attribute_list(
			char *sql_injection_escape_folder_name,
			LIST *exclude_attribute_name_list )
{
	return
	folder_attribute_system_list(
		folder_attribute_system_string(
			folder_attribute_where(
				sql_injection_escape_folder_name,
				exclude_attribute_name_list ) ) );
}

char *folder_attribute_system_string( char *where )
{
	char system_string[ STRING_WHERE_BUFFER ];
	char *order;

	order = "ifnull(primary_key_index,0), ifnull(display_order,0)";

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\" \"%s\"",
		FOLDER_ATTRIBUTE_SELECT,
		FOLDER_ATTRIBUTE_TABLE,
		where,
		order );

	return strdup( system_string );
}

LIST *folder_attribute_system_list( char *system_string )
{
	char input[ 1024 ];
	FILE *pipe;
	LIST *list = {0};
	FOLDER_ATTRIBUTE *folder_attribute;

	pipe = popen( system_string, "r"  );

	while ( string_input( input, pipe, 2048 ) )
	{
		if ( ( folder_attribute = folder_attribute_parse( input ) ) )
		{
			if ( !list ) list = list_new();

			list_set( list, folder_attribute );
		}
	}

	pclose( pipe );
	return list;
}

FOLDER_ATTRIBUTE *folder_attribute_parse( char *input )
{
	char folder_name[ 128 ];
	char attribute_name[ 128 ];
	char buffer[ 128 ];
	FOLDER_ATTRIBUTE *folder_attribute;

	if ( !input || !*input ) return (FOLDER_ATTRIBUTE *)0;

	/* See FOLDER_ATTRIBUTE_SELECT */
	/* --------------------------- */
	piece( folder_name, SQL_DELIMITER, input, 0 );
	piece( attribute_name, SQL_DELIMITER, input, 1 );

	folder_attribute =
		folder_attribute_new(
			strdup( folder_name ),
			strdup( attribute_name ) );

	if ( ! ( folder_attribute->attribute =
			attribute_fetch(
				attribute_name ) ) )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: attribute_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			attribute_name );

		return (FOLDER_ATTRIBUTE *)0;
	}

	piece( buffer, SQL_DELIMITER, input, 2 );
	folder_attribute->primary_key_index = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 3 );
	folder_attribute->display_order = atoi( buffer );

	piece( buffer, SQL_DELIMITER, input, 4 );
	folder_attribute->omit_insert = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 5 );
	folder_attribute->omit_insert_prompt = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 6 );
	folder_attribute->omit_update = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 7 );
	folder_attribute->additional_unique_index = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 8 );
	folder_attribute->additional_index = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 9 );
	folder_attribute->insert_required = ( *buffer == 'y' );

	piece( buffer, SQL_DELIMITER, input, 10 );
	folder_attribute->lookup_required = ( *buffer == 'y' );

	return folder_attribute;
}

