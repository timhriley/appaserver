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
		char *full_name,
		char *street_address )
{
	if ( !full_name
	||   !street_address )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	string_pipe_fetch(
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
				full_name,
				street_address ) ) );
}
