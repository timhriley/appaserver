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
		char *folder_name,
		boolean non_owner_forbid,
		boolean override_row_restrictions );

/* Process */
/* ------- */
SECURITY_ENTITY *security_entity_calloc(
		void );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *security_entity_where(
		char *table_name,
		char *full_name,
		char *contact_key );

/* Usage */
/* ----- */

/* Returns static memory or null */
/* ----------------------------- */
char *security_entity_table_name(
		char *folder_name );

#endif
