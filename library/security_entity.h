/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/security_entity.h		   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef SECURITY_ENTITY_H
#define SECURITY_ENTITY_H

#include "boolean.h"
#include "list.h"
#include "appaserver_user.h"

typedef struct
{
	char *login_name;
	boolean non_owner_forbid;
	boolean override_row_restrictions;
	APPASERVER_USER *appaserver_user;
	char *where;
} SECURITY_ENTITY;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SECURITY_ENTITY *security_entity_new(
		char *login_name,
		boolean non_owner_forbid,
		boolean override_row_restrictions );

/* Process */
/* ------- */
SECURITY_ENTITY *security_entity_calloc(
		void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *security_entity_where(
		char *full_name,
		char *street_address );

#endif
