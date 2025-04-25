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
	list_pipe_fetch(
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
				full_name,
				street_address ) ) );
}
