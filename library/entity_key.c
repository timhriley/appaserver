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

ENTITY_KEY *entity_key_new( void )
{
	ENTITY_KEY *entity_key;

	entity_key  = entity_key_calloc();

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

