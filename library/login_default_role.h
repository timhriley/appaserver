/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/login_default_role.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef LOGIN_DEFAULT_ROLE_H
#define LOGIN_DEFAULT_ROLE_H

#include "boolean.h"
#include "list.h"

#define LOGIN_DEFAULT_ROLE_TABLE	"login_default_role"

typedef struct
{
	/* Stub */
} LOGIN_DEFAULT_ROLE;

/* Usage */
/* ----- */
char *login_default_role_name(
		const char *login_default_role_table,
		char *full_name,
		char *street_address );

#endif
