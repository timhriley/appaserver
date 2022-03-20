/* $APPASERVER_HOME/library/role_folder.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef ROLE_FOLDER_H
#define ROLE_FOLDER_H

#include "list.h"
#include "boolean.h"

/* Constants */
/* --------- */
#define ROLE_FOLDER_TABLE	"role_folder"

#define ROLE_FOLDER_SELECT	"role,"			\
				"role_folder.folder,"	\
				"permission,"		\
				"subschema"

typedef struct
{
	char *role_name;
	char *folder_name;
	char *permission;
	char *subschema_name;
} ROLE_FOLDER;

/* ROLE_FOLDER operations */
/* ---------------------- */

/* Usage */
/* ----- */
LIST *role_folder_list(	char *role_name,
			char *folder_name );

LIST *role_folder_lookup_list(
			char *role_name );

LIST *role_folder_insert_list(
			char *role_name );

/* Private */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *role_folder_where(
			char *role_name,
			char *folder_name );

/* Returns static memory */
/* --------------------- */
char *role_folder_lookup_where(
			char *role_name );


/* Returns static memory */
/* --------------------- */
char *role_folder_insert_where(
			char *role_name );

/* Returns static memory */
/* --------------------- */
char *role_folder_join_where(
			char *role_folder_table,
			char *folder_table );

/* Returns heap memory */
/* ------------------- */
char *role_folder_system_string(
			char *role_folder_select,
			char *role_folder_table,
			char *folder_table,
			char *where,
			char *role_folder_join_where );

LIST *role_folder_system_list(
			char *role_folder_system_string );

ROLE_FOLDER *role_folder_parse(
			char *input );

ROLE_FOLDER *role_folder_new( 
			char *role_name,
			char *folder_name,
			char *permission );

ROLE_FOLDER *role_folder_calloc(
			 void );

/* Public */
/* ------ */
boolean role_folder_insert(
			LIST *role_folder_list );

boolean role_folder_update(
			LIST *role_folder_list );

boolean role_folder_lookup(
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
