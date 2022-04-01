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

#define ROLE_OPERATION_TABLE	"role_operation"

#define ROLE_OPERATION_SELECT	"folder,"	\
				"role,"		\
				"operation"

typedef struct
{
	char *folder_name;
	char *role_name;
	char *operation_name;
	OPERATION *operation;
} ROLE_OPERATION;

/* ROLE_OPERATION operations */
/* ------------------------- */

/* Usage */
/* ------ */
LIST *role_operation_list(
			char *folder_name,
			char *role_name,
			boolean fetch_operation,
			boolean fetch_process );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *role_operation_where(
			char *folder_name,
			char *role_name );

/* Returns static memory */
/* --------------------- */
char *role_operation_system_string(
			char *role_operation_select,
			char *role_operation_table,
			char *role_operation_where );

LIST *role_operation_system_list(
			char *role_operation_system_string,
			boolean fetch_operation,
			boolean fetch_process );

ROLE_OPERATION *role_operation_parse(
			char *input,
			boolean fetch_operation,
			boolean fetch_process );

ROLE_OPERATION *role_operation_new( 
			char *folder_name,
			char *role_name,
			char *operation_name );

/* Private */
/* ------- */
ROLE_OPERATION *role_operation_calloc(
			void );

/* Public */
/* ------ */
LIST *role_operation_name_list(
			LIST *role_operation_list );

boolean role_operation_list_delete_boolean(
			LIST *role_operation_list );

#endif
