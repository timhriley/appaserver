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
#define ROLE_TABLE			"role"
#define ROLE_PROCESS_TABLE		"role_process"
#define ROLE_PROCESS_SET_MEMBER_TABLE	"role_process_set_member"
#define ROLE_APPASERVER_USER_TABLE	"role_appaserver_user"

typedef struct
{
	char *role_name;
	char *process_name;
	char *process_group_name;
} ROLE_PROCESS;

typedef struct
{
	char *role_name;
	char *process_set_name;
	char *process_group_name;
} ROLE_PROCESS_SET;

typedef struct
{
	char *login_name;
	char *role_name;
} ROLE_APPASERVER_USER;

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
	LIST *attribute_exclude_list;
} ROLE;

/* ROLE operations */
/* --------------- */
ROLE *role_calloc(	void );

LIST *role_list_fetch(	char *login_name );

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

LIST *role_exclude_lookup_attribute_name_list(
			LIST *attribute_exclude_list );

LIST *role_exclude_insert_attribute_name_list(
			LIST *attribute_exclude_list );

LIST *role_exclude_update_attribute_name_list(
			LIST *attribute_exclude_list );

/* ROLE_ATTRIBUTE_EXCLUDE operations */
/* --------------------------------- */
ROLE_ATTRIBUTE_EXCLUDE *role_attribute_exclude_parse(
			char *input );

LIST *role_attribute_exclude_system_list(
			char *system_string );

/* Returns heap memory */
/* ------------------- */
char *role_attribute_exclude_system_string(
			char *where );

/* ROLE_APPASERVER_USER operations */
/* ------------------------------- */
ROLE_APPASERVER_USER *role_appaserver_user_fetch(
			char *session_login_name,
			char *sql_injection_escape_role_name );

LIST *role_appaserver_user_system_list(
			char *system_string );

/* Returns static memory */
/* --------------------- */
char *role_appaserver_user_primary_where(
			char *login_name,
			char *role_name );

/* Returns heap memory */
/* ------------------- */
char *role_appaserver_user_system_string(
			char *where );

ROLE_APPASERVER_USER *role_appaserver_user_calloc(
			void );

ROLE_APPASERVER_USER *role_appaserver_user_parse(
			char *input );

/* ROLE_PROCESS operations */
/* ----------------------- */
LIST *role_process_list(
			char *role_name );

ROLE_PROCESS *role_process_calloc(
			void );

/* Returns static memory */
/* --------------------- */
char *role_process_select(
			void );

/* Safely returns heap memory */
/* -------------------------- */
char *role_process_where(
			char *role_name );

/* Safely returns heap memory */
/* -------------------------- */
char *role_process_system_string(
			char *where );

ROLE_PROCESS *role_process_parse(
			char *input );

LIST *role_process_group_name_list(
			LIST *role_process_list );

LIST *role_process_name_list(
			char *process_group_name,
			LIST *role_process_list );

LIST *role_process_or_set_name_list(
			char *process_group_name,
			LIST *role_process_list,
			LIST *role_process_set_list );

/* ROLE_PROCESS_SET operations */
/* --------------------------- */
LIST *role_process_set_list(
			char *role_name );

ROLE_PROCESS_SET *role_process_set_calloc(
			void );

/* Returns static memory */
/* --------------------- */
char *role_process_set_select(
			void );

/* Safely returns heap memory */
/* -------------------------- */
char *role_process_set_where(
			char *role_name );

/* Safely returns heap memory */
/* -------------------------- */
char *role_process_set_system_string(
			char *where );

ROLE_PROCESS_SET *role_process_set_parse(
			char *input );

LIST *role_process_set_group_name_list(
			LIST *role_process_set_list );

LIST *role_process_set_name_list(
			char *process_group_name,
			LIST *role_process_set_list );

#endif
