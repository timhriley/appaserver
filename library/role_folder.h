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

#define ROLE_FOLDER_SELECT	"role,"		\
				"folder,"	\
				"permission"

/* Structures */
/* ---------- */
typedef struct
{
	char *role_name;
	char *folder_name;
	char *permission;
} ROLE_FOLDER;

LIST *role_folder_fetch_list(
			char *role_name,
			char *folder_name );

ROLE_FOLDER *role_folder_new( 
			char *role_name,
			char *folder_name,
			char *permission );

ROLE_FOLDER *role_folder_parse(
			char *input );

/* Returns heap memory */
/* ------------------- */
char *role_folder_system_string(
			char *where );

LIST *role_folder_system_list(
			char *system_string );

boolean role_folder_lookup(
			LIST *role_folder_list );

boolean role_folder_update(
			LIST *role_folder_list );

boolean role_folder_insert(
			LIST *role_folder_list );

/* Returns static memory */
/* --------------------- */
char *role_folder_primary_where(
			char *role_name,
			char *folder_name )
#endif
