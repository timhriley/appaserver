/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/security_entity.h		   		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef SECURITY_ENTITY_H
#define SECURITY_ENTITY_H

#include "boolean.h"
#include "list.h"

typedef struct
{
	char *login_name;
	boolean non_owner_forbid;
	boolean override_row_restrictions;
	char *full_name;
	char *street_address;
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
		const char *appaserver_user_primary_key,
		SECURITY_ENTITY *security_entity );

#endif
