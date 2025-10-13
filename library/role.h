/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/role.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef ROLE_H
#define ROLE_H

#include "boolean.h"
#include "list.h"
#include "process.h"

#define ROLE_TABLE 			"role"

#define ROLE_SYSTEM			"system"
#define ROLE_SUPERVISOR			"supervisor"
#define ROLE_NAME_COLUMN		"role"
#define ROLE_LOGIN_NAME_COLUMN		"login_name"

#define ROLE_PROCESS_SET_MEMBER_SELECT	"process,process_set"
#define ROLE_PROCESS_SET_MEMBER_TABLE	"role_process_set_member"

#define ROLE_ATTRIBUTE_EXCLUDE_TABLE	"column_exclude"
#define ROLE_ATTRIBUTE_EXCLUDE_SELECT	"column_name,permission"

#define ROLE_PROCESS_SELECT		"process"
#define ROLE_PROCESS_TABLE		"role_process"

typedef struct
{
	char *process_name;
	char *role_name;
	char *process_set_name;
	PROCESS_SET *process_set;
} ROLE_PROCESS_SET_MEMBER;

/* Usage */
/* ----- */
LIST *role_process_set_member_list(
		char *role_name,
		boolean fetch_process_set );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *role_process_set_member_system_string(
		char *role_process_set_member_select,
		char *role_process_set_member_table,
		char *where );

FILE *role_process_set_member_input_pipe(
		char *role_process_set_member_system_string );

/* Usage */
/* ----- */
ROLE_PROCESS_SET_MEMBER *role_process_set_member_parse(
		char *role_name,
		boolean fetch_process_set,
		char *input );

/* Usage */
/* ----- */
ROLE_PROCESS_SET_MEMBER *role_process_set_member_new(
		char *process_name,
		char *process_set_name,
		char *role_name );

/* Process */
/* ------- */
ROLE_PROCESS_SET_MEMBER *role_process_set_member_calloc(
		void );

/* Public */
/* ------ */
LIST *role_process_set_member_process_name_list(
		char *process_group,
		LIST *role_process_set_member_list );

typedef struct
{
	char *attribute_name;
	char *permission;
} ROLE_ATTRIBUTE_EXCLUDE;

/* Usage */
/* ----- */
LIST *role_attribute_exclude_list(
		char *role_name );

/* Usage */
/* ----- */
LIST *role_attribute_exclude_system_list(
		char *appaserver_system_string );

/* Usage */
/* ----- */
ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude_parse(
		char *input );

/* Process */
/* ------- */
ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude_calloc(
		void );

/* Usage */
/* ----- */
LIST *role_attribute_exclude_lookup_name_list(
		char *appaserver_lookup_state,
		char *appaserver_update_state,
		LIST *role_attribute_exclude_list );

/* Usage */
/* ----- */
LIST *role_attribute_exclude_insert_name_list(
		char *appaserver_insert_state,
		LIST *role_attribute_exclude_list );

/* Usage */
/* ----- */
LIST *role_attribute_exclude_name_list(
		char *role_permission,
		LIST *role_attribute_exclude_list );

/* Usage */
/* ----- */
boolean role_attribute_exclude_exists_permission(
		char *permission,
		char *attribute_name,
		LIST *role_attribute_exclude_list );

typedef struct
{
	char *role_name;
	boolean folder_count;
	boolean override_row_restrictions;
	boolean grace_no_cycle_colors;
	LIST *role_attribute_exclude_list;
} ROLE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ROLE *role_fetch(
		char *role_name,
		boolean fetch_role_attribute_exclude_list );

/* Process */
/* ------- */

/* Returns program memory */
/* ---------------------- */
char *role_select(
		boolean grace_no_cycle_colors_exists );

/* Returns static memory */
/* --------------------- */
char *role_primary_where(
		char *role_name );

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
ROLE *role_parse(
		char *role_name,
		boolean fetch_role_attribute_exclude_list,
		char *input );

/* Process */
/* ------- */
ROLE *role_calloc(
		void );


/* Usage */
/* ----- */
LIST *role_name_list(
		const char *role_table );

typedef struct
{
	char *role_name;
	char *process_name;
	PROCESS *process;
} ROLE_PROCESS;

/* Usage */
/* ----- */
LIST *role_process_list(
		char *role_name,
		boolean fetch_process );

/* Process */
/* ------- */

/* Returns heap memory */
/* ------------------- */
char *role_process_system_string(
		char *role_process_select,
		char *role_process_table,
		char *where );

FILE *role_process_input_pipe(
		char *role_process_system_string );

/* Usage */
/* ----- */
ROLE_PROCESS *role_process_parse(
		char *role_name,
		boolean fetch_process,
		char *input );

/* Usage */
/* ----- */
ROLE_PROCESS *role_process_new(
		char *role_name,
		char *process_name );

/* Process */
/* ------- */
ROLE_PROCESS *role_process_calloc(
		void );

/* Public */
/* ------ */
LIST *role_process_or_set_group_name_list(
		LIST *role_process_list,
		LIST *role_process_set_member_list );

LIST *role_process_or_set_name_list(
		char *process_group,
		LIST *role_process_list,
		LIST *role_process_set_member_list );

LIST *role_process_or_set_missing_group_name_list(
		LIST *role_process_list,
		LIST *role_process_set_member_list );

boolean role_process_exists(
		char *process_name,
		LIST *role_process_list );

#endif
