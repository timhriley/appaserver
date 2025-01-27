/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/entity_self.c				*/
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
#include "entity_self.h"

ENTITY_SELF *entity_self_calloc( void )
{
	ENTITY_SELF *entity_self;

	if ( ! ( entity_self = calloc( 1, sizeof ( ENTITY_SELF ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return entity_self;
}

ENTITY_SELF *entity_self_new(
		char *full_name,
		char *street_address )
{
	ENTITY_SELF *entity_self;

	if ( !full_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: full_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	entity_self = entity_self_calloc();

	entity_self->full_name = full_name;
	entity_self->street_address = street_address;

	return entity_self;
}

ENTITY_SELF *entity_self_fetch(
		boolean fetch_entity_boolean )
{
	FILE *input_pipe;
	char input[ 1024 ];
	ENTITY_SELF *entity_self;

	input_pipe =
		appaserver_input_pipe(
			entity_self_system_string(
				ENTITY_SELF_SELECT,
				ENTITY_SELF_TABLE ) );

	entity_self =
		entity_self_parse(
			/* ----------------------------------------- */
			/* Returns input_buffer or null if all done. */
			/* ----------------------------------------- */
			string_input(
				input, input_pipe, 1024 ),
			fetch_entity_boolean );

	pclose( input_pipe );

	return entity_self;
}

ENTITY_SELF *entity_self_parse(
		char *input,
		boolean fetch_entity_boolean )
{
	char full_name[ 128 ];
	char street_address[ 128 ];
	ENTITY_SELF *entity_self;

	if ( !input || !*input ) return NULL;

	piece( full_name, SQL_DELIMITER, input, 0 );
	piece( street_address, SQL_DELIMITER, input, 1 );

	entity_self =
		entity_self_new(
			strdup( full_name ),
			strdup( street_address ) );

	if ( fetch_entity_boolean )
	{
		entity_self->entity =
			entity_fetch(
				entity_self->full_name,
				entity_self->street_address );
	}

	return entity_self;
}

char *entity_self_system_string(
		char *select,
		char *table )
{
	char system_string[ 1024 ];

	sprintf( system_string,
		 "select.sh '%s' %s '' select",
		 select,
		 table );

	return strdup( system_string );
}

