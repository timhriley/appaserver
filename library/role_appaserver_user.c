/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/role_appaserver_user.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "appaserver.h"
#include "entity.h"
#include "role.h"
#include "role_appaserver_user.h"

LIST *role_appaserver_user_name_list(
		const char *role_appaserver_user_table,
		char *full_name,
		char *contact_key,
		boolean contact_key_boolean )
{
	if ( !full_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"full_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	list_system_string_fetch(
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_system_string(
			ROLE_NAME_COLUMN /* select */,
			(char *)role_appaserver_user_table,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			entity_primary_where(
				ENTITY_FULL_NAME_COLUMN,
				ENTITY_CONTACT_KEY_COLUMN,
				full_name,
				contact_key,
				contact_key_boolean ) ) );
}
