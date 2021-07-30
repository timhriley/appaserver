/* $APPASERVER_HOME/library/folder_attribute.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "timlib.h"

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

LIST *folder_attribute_list(
			char *escaped_replaced_folder_name,
			LIST *exclude_attribute_name_list )
{
}

char *folder_attribute_where( char *folder_name )
{
}

char *folder_attribute_system_string( char *where )
{
}

FOLDER_ATTRIBUTE *folder_attribute_parse( char *input )
{
}

