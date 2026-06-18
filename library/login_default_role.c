/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/login_default_role.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "role.h"
#include "entity.h"
#include "login_default_role.h"

char *login_default_role_name(
		const char *login_default_role_table,
		boolean contact_key_boolean,
		char *full_name,
		char *contact_key )
{
	if ( !full_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"login_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	string_system_string_input(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			ROLE_NAME_COLUMN /* select */,
			(char *)login_default_role_table,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			entity_primary_where(
				contact_key_boolean,
				full_name,
				contact_key ) ) );
}
