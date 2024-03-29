/* $APPASERVER_HOME/library/role.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef ROLE_H
#define ROLE_H

#include "boolean.h"

#define FOLDER_COUNT_YN_PIECE		0

typedef struct
{
	char *role_name;
	char *attribute_name;
	char *permission;
} ROLE_ATTRIBUTE_EXCLUDE;

typedef struct
{
	char *application_name;
	char *role_name;
	char folder_count_yn;
	char override_row_restrictions_yn;
	char grace_no_cycle_colors_yn;
	LIST *role_attribute_exclude_list;
} ROLE;

ROLE *role_new( 			char *application_name,
					char *role_name );

ROLE *role_new_role( 			char *application_name,
					char *role_name );

boolean role_fetch( 			char *folder_count_yn,
					char *override_row_restrictions,
					char *grace_no_cycle_colors_yn,
					char *application_name,
					char *role_name );

boolean role_get_override_row_restrictions(
				char override_row_restrictions_yn );

boolean role_get_exists_folder_count_y(
					char *application_name,
					char *login_name );

void role_free(				ROLE *role );

ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude_new(
			char *role_name,
			char *attribute_name,
			char *permission );

LIST *role_get_attribute_exclude_list(	char *application_name,
					char *role_name );

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
