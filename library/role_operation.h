/* $APPASERVER_HOME/library/role_operation.h				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef ROLE_OPERATION_H
#define ROLE_OPERATION_H

#include "list.h"
#include "boolean.h"
#include "operation.h"

/* Constants */
/* --------- */
#define ROLE_OPERATION_TABLE	"role_operation"

#define ROLE_OPERATION_SELECT	"folder,"	\
				"role,"		\
				"operation"

/* Structures */
/* ---------- */
typedef struct
{
	/* Attributes */
	/* ---------- */
	char *folder_name;
	char *role_name;
	char *operation_name;

	/* Generic */
	/* ------- */
	OPERATION *operation;
} ROLE_OPERATION;

/* Operations */
/* ---------- */
LIST *role_operation_list(
			char *folder_name,
			char *role_name );

ROLE_OPERATION *role_operation_new( 
			char *folder_name,
			char *role_name,
			char *operation_name );

ROLE_OPERATION *role_operation_parse(
			char *input );

/* Returns heap memory */
/* ------------------- */
char *role_operation_system_string(
			char *where );

LIST *role_operation_system_list(
			char *system_string );

LIST *role_operation_name_list(
			LIST *role_operation_list );

/* Returns static memory */
/* --------------------- */
char *role_operation_primary_where(
			char *folder_name,
			char *role_name,
			char *operation_name );

boolean role_operation_delete(
			LIST *role_operation_list );

#endif
