/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/role_appaserver_user.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef ROLE_APPASERVER_USER_H
#define ROLE_APPASERVER_USER_H

#include "boolean.h"
#include "list.h"

#define ROLE_APPASERVER_USER_TABLE	"role_appaserver_user"

typedef struct
{
	/* Stub */
} ROLE_APPASERVER_USER;

/* Usage */
/* ----- */
LIST *role_appaserver_user_name_list(
		const char *role_appaserver_user_table,
		char *full_name,
		char *street_address );

#endif
