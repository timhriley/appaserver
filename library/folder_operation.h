/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/folder_operation.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FOLDER_OPERATION_H
#define FOLDER_OPERATION_H

#include "list.h"
#include "boolean.h"
#include "operation.h"

#define FOLDER_OPERATION_TABLE	"table_operation"

#define FOLDER_OPERATION_SELECT	"table_name,"	\
				"role,"		\
				"operation"

typedef struct
{
	char *folder_name;
	char *role_name;
	char *operation_name;
	OPERATION *operation;
} FOLDER_OPERATION;

/* Usage */
/* ------ */
LIST *folder_operation_list(
		char *folder_name,
		char *role_name,
		boolean fetch_operation,
		boolean fetch_process );

/* Process */
/* ------- */

/* Returns static memory */
/* --------------------- */
char *folder_operation_where(
		const char *folder_primary_key,
		char *folder_name,
		char *role_name );

/* Returns static memory */
/* --------------------- */
char *folder_operation_system_string(
		char *folder_operation_select,
		char *folder_operation_table,
		char *folder_operation_where );

LIST *folder_operation_system_list(
		char *folder_operation_system_string,
		boolean fetch_operation,
		boolean fetch_process );

/* Usage */
/* ----- */
FOLDER_OPERATION *folder_operation_parse(
		char *input,
		boolean fetch_operation,
		boolean fetch_process );

/* Process */
/* ------- */
FOLDER_OPERATION *folder_operation_new( 
		char *folder_name,
		char *role_name,
		char *operation_name );

FOLDER_OPERATION *folder_operation_calloc(
		void );

/* Public */
/* ------ */
LIST *folder_operation_name_list(
		LIST *folder_operation_list );

boolean folder_operation_delete_boolean(
		LIST *folder_operation_list );

#endif
