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

	security_entity->appaserver_user =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_user_login_fetch(
			login_name,
			0 /* not fetch_role_name_list */ );

	security_entity->where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		security_entity_where(
			security_entity->appaserver_user->full_name,
			security_entity->appaserver_user->street_address );

	return security_entity;
}

char *security_entity_where(
		char *full_name,
		char *street_address )
{
	return
	/* --------------------- */
	/* Returns static memory */
	/* --------------------- */
	entity_primary_where(
		full_name,
		street_address );
}
