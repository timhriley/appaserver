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
	char *subschema_name;
} ROLE_FOLDER;

/* ROLE_FOLDER operations */
/* ---------------------- */
LIST *role_folder_list(	char *role_name,
			char *folder_name );

ROLE_FOLDER *role_folder_calloc(
			 void );

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

boolean role_folder_insert(
			LIST *role_folder_list );

boolean role_folder_update(
			LIST *role_folder_list );

boolean role_folder_lookup(
			LIST *role_folder_list );

/* Returns static memory */
/* --------------------- */
char *role_folder_primary_where(
			char *role_name,
			char *folder_name );

/* ROLE_FOLDER subschema operations */
/* -------------------------------- */
LIST *role_folder_lookup_list(
			char *role_name );

LIST *role_folder_insert_list(
			char *role_name );

/* Safely returns heap memory */
/* -------------------------- */
char *role_folder_subschema_system_string(
			char *where );

/* Returns static memory */
/* --------------------- */
char *role_folder_subschema_select(
			void );

/* Safely returns heap memory */
/* -------------------------- */
char *role_folder_subschema_where(
			char *role_name );

ROLE_FOLDER *role_folder_subschema_parse(
			char *input );

LIST *role_folder_subschema_name_list(
			LIST *role_folder_lookup_insert_list );

LIST *role_folder_subschema_missing_folder_name_list(
			LIST *role_folder_lookup_insert_list );

LIST *role_folder_subschema_folder_name_list(
			char *subschema_name,
			LIST *role_folder_lookup_insert_list );

LIST *role_folder_name_list(
			LIST *role_folder_list );

#endif
