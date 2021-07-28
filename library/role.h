/* $APPASERVER_HOME/library/role.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef ROLE_H
#define ROLE_H

#include "boolean.h"

/* Constants */
/* --------- */
#define ROLE_TABLE_NAME		"role"

typedef struct
{
	char *role_name;
	char *attribute_name;
	char *permission;
} ROLE_ATTRIBUTE_EXCLUDE;

typedef struct
{
	char *role_name;
	boolean folder_count;
	boolean override_row_restrictions;
	boolean grace_no_cycle_colors;
	LIST *role_attribute_exclude_list;
} ROLE;

ROLE *role_new( 	char *role_name );

ROLE *role_fetch(	char *role_name,
			boolean fetch_role_attribute_exclude_list );

void role_free(		ROLE *role );

ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude_new(
			char *role_name,
			char *attribute_name,
			char *permission );

boolean role_exists_attribute_exclude_insert(
			LIST *attribute_exclude_list,
			char *attribute_name );

/* Returns static memory */
/* --------------------- */
char *role_primary_where(
			char *role_name );

ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude_parse(
			char *input );

LIST *role_attribute_exclude_system_list(
			char *system_string );

/* Returns heap memory */
/* ------------------- */
char *role_attribute_exclude_system_string(
			char *where );

ROLE *role_calloc(	void );

LIST *role_system_list(	char *system_string,
			boolean fetch_attribute_exclude_list );

ROLE *role_parse(	char *input,
			boolean fetch_attribute_exclude_list );

/* Returns heap memory */
/* ------------------- */
char *role_system_string(
			char *where );

/* Returns program memory */
/* ---------------------- */
char *role_select(	void );

#endif
