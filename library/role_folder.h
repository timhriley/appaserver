/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/role_folder.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef ROLE_FOLDER_H
#define ROLE_FOLDER_H

#include "list.h"
#include "boolean.h"
#include "role.h"

#define ROLE_FOLDER_TABLE	"role_table"

#define ROLE_FOLDER_SELECT	"role,"				\
				"role_table.table_name,"	\
				"permission,"			\
				"subschema"

typedef struct
{
	char *role_name;
	char *folder_name;
	char *permission;
	char *subschema_name;
} ROLE_FOLDER;

/* Usage */
/* ----- */
LIST *role_folder_list(
		char *role_name,
		char *folder_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *role_folder_where(
		const char *role_folder_table,
		const char *folder_primary_key,
		char *role_name,
		char *folder_name );

/* Usage */
/* ----- */
LIST *role_folder_lookup_list(
		char *role_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *role_folder_lookup_where(
		const char *role_folder_lookup,
		const char *role_folder_update,
		char *role_name );

/* Usage */
/* ----- */
LIST *role_folder_insert_list(
		char *role_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *role_folder_insert_where(
		const char *role_permssion_insert,
		char *role_name );

/* Usage */
/* ----- */
LIST *role_folder_system_list(
		char *role_folder_system_string );

/* Process */
/* ------- */
FILE *role_folder_input_pipe(
		char *role_folder_system_string );

/* Usage */
/* ----- */
ROLE_FOLDER *role_folder_parse(
		char *input );

/* Process */
/* ------- */

/* Usage */
/* ----- */
ROLE_FOLDER *role_folder_new( 
		char *role_name,
		char *folder_name,
		char *permission );

/* Process */
/* ------- */
ROLE_FOLDER *role_folder_calloc(
		 void );

/* Usage */
/* ----- */

/* Returns static memory */
/* --------------------- */
char *role_folder_left_join(
		const char *role_folder_table,
		const char *folder_table,
		const char *folder_primary_key );

/* Usage */
/* ----- */
 
/* Returns static memory */
/* --------------------- */
char *role_folder_lookup_in_clause(
		const char *role_folder_table,
		const char *role_permission_lookup,
		const char *role_permission_update,
		const char *folder_primary_key,
		char *role_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *role_folder_permission_where(
		const char *role_permission_lookup,
		const char *role_permission_update,
		char *role_name );

/* Usage */
/* ----- */
LIST *role_folder_fetch_name_list(
		const char *role_folder_table,
		const char *folder_primary_key,
		char *role_name );

/* Usage */
/* ----- */

/* Returns heap memory */
/* ------------------- */
char *role_folder_system_string(
		const char *role_folder_select,
		const char *role_folder_table,
		char *where,
		char *role_folder_left_join );

/* Public */
/* ------ */
boolean role_folder_insert_boolean(
		const char *role_permssion_insert,
		char *folder_name,
		LIST *role_folder_list );

boolean role_folder_update_boolean(
		const char *role_permssion_update,
		char *folder_name,
		LIST *role_folder_list );

boolean role_folder_lookup_boolean(
		const char *role_permission_lookup,
		const char *role_permission_update,
		char *folder_name,
		LIST *role_folder_list );

LIST *role_folder_name_list(
		LIST *role_folder_list );

ROLE_FOLDER *role_folder_permission_seek(
		char *folder_name,
		char *permission,
		LIST *role_folder_list );

boolean role_folder_insert_exists(
		char *folder_name,
		LIST *role_folder_list );

LIST *role_folder_subschema_folder_name_list(
		char *subschema_name,
		LIST *role_folder_list );

LIST *role_folder_subschema_name_list(
		LIST *role_folder_lookup_list );

LIST *role_folder_subschema_missing_folder_name_list(
		LIST *role_folder_lookup_list );

/* Returns heap memory */
/* ------------------- */
char *role_folder_list_display(
		LIST *role_folder_list );

#endif
