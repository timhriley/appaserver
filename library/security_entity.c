/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/security_entity.c		   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "entity.h"
#include "appaserver_user.h"
#include "security_entity.h"

SECURITY_ENTITY *security_entity_calloc( void )
{
	SECURITY_ENTITY *security_entity;

	if ( ! ( security_entity =
			calloc( 1, sizeof ( SECURITY_ENTITY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty..\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return security_entity;
}

SECURITY_ENTITY *security_entity_new(
		char *login_name,
		boolean non_owner_forbid,
		boolean override_row_restrictions )
{
	SECURITY_ENTITY *security_entity;
	boolean login_name_boolean;

	if ( !login_name )
	{
		char message[ 128 ];

		sprintf(message, "login_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	security_entity = security_entity_calloc();

	security_entity->login_name = login_name;
	security_entity->non_owner_forbid = non_owner_forbid;
	security_entity->override_row_restrictions = override_row_restrictions;

	if ( !non_owner_forbid
	||   override_row_restrictions )
	{
		return security_entity;
	}

	login_name_boolean =
		entity_login_name_boolean(
			ENTITY_TABLE,
			APPASERVER_USER_PRIMARY_KEY );

	if ( login_name_boolean )
	{
		ENTITY *entity;

		entity =
			entity_login_name_fetch(
				ENTITY_TABLE,
				APPASERVER_USER_PRIMARY_KEY,
				login_name );

		if ( !entity )
		{
			char message[ 128 ];

			sprintf(message,
				"entity_login_name_fetch(%s) returned empty.",
				login_name );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		security_entity->full_name =
			entity->full_name;

		security_entity->street_address =
			entity->street_address;
	}

	security_entity->where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		security_entity_where(
			APPASERVER_USER_PRIMARY_KEY,
			security_entity );

	return security_entity;
}

char *security_entity_where(
		const char *appaserver_user_primary_key,
		SECURITY_ENTITY *security_entity )
{
	static char where[ 256 ];

	if ( !security_entity )
	{
		char message[ 128 ];

		sprintf(message, "security_entity is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( security_entity->full_name )
	{
		sprintf(where,
			"full_name = '%s' and "
			"street_address = '%s'",
			security_entity->full_name,
			security_entity->street_address );
	}
	else
	{
		sprintf(where,
			"%s = '%s'",
			appaserver_user_primary_key,
			security_entity->login_name );
	}

	return where;
}
