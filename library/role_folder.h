/* $APPASERVER_HOME/library/role_folder.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef ROLE_FOLDER_H
#define ROLE_FOLDER_H

#include "list.h"
#include "boolean.h"

#define ROLE_FOLDER_TABLE	"role_folder"

#define ROLE_FOLDER_SELECT	"role,"			\
				"role_folder.folder,"	\
				"permission,"		\
				"subschema"

#define ROLE_FOLDER_INSERT	"insert"
#define ROLE_FOLDER_UPDATE	"update"
#define ROLE_FOLDER_LOOKUP	"lookup"

typedef struct
{
	char *role_name;
	char *folder_name;
	char *permission;
	char *subschema_name;
} ROLE_FOLDER;

/* Usage */
/* ----- */
LIST *role_folder_list(	char *role_name,
			char *folder_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *role_folder_where(
			char *role_name,
			char *folder_name );


/* Returns static memory */
/* --------------------- */
char *role_folder_left_join(
			char *role_folder_table,
			char *folder_table );

/* Returns heap memory */
/* ------------------- */
char *role_folder_system_string(
			char *role_folder_select,
			char *role_folder_table,
			char *where,
			char *role_folder_left_join );

/* Usage */
/* ----- */
LIST *role_folder_lookup_list(
			char *role_name );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *role_folder_lookup_where(
			char *role_folder_lookup,
			char *role_folder_update,
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
			char *role_folder_insert,
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

/* Public */
/* ------ */
boolean role_folder_insert_boolean(
			char *role_folder_insert,
			LIST *role_folder_list );

boolean role_folder_update_boolean(
			char *role_folder_update,
			LIST *role_folder_list );

boolean role_folder_lookup_boolean(
			char *role_folder_lookup,
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

#endif
