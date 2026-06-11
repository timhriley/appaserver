/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/entity_key.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "piece.h"
#include "sql.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "entity_key.h"

ENTITY_KEY *entity_key_new(
		const char *entity_table,
		const char *entity_contact_key_column )
{
	ENTITY_KEY *entity_key;

	entity_key  = entity_key_calloc();

	entity_key->contact_key_boolean =
		entity_key_contact_key_boolean(
			entity_table /* table_name */,
			entity_contact_key_column /* column_name */ );

	return entity_key;
}

ENTITY_KEY *entity_key_calloc( void )
{
	ENTITY_KEY *entity_key;

	if ( ! ( entity_key = calloc( 1, sizeof ( ENTITY_KEY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return entity_key;
}

boolean entity_key_contact_key_boolean(
		const char *table_name,
		const char *column_name )
{
	static boolean contact_key_boolean = -1;

	if ( contact_key_boolean == -1 )
	{
		contact_key_boolean =
			appaserver_table_column_boolean(
				table_name,
				column_name );
	}

	return contact_key_boolean;
}

