To retire.
/* $APPASERVER_HOME/library/operation_list.h				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver operation and operation_list ADTs.		*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef OPERATION_LIST_H
#define OPERATION_LIST_H

#include "list.h"
#include "operation.h"

enum omit_delete_operation{	omit_delete,
				omit_delete_with_placeholder,
				dont_omit_delete,
				omit_delete_dont_care };

typedef struct
{
	LIST *operation_list;
	int performed_any_output;
} OPERATION_LIST_STRUCTURE;

/* Operations */
/* ---------- */
OPERATION_LIST_STRUCTURE *
	operation_list_structure_new(	char *application_name,
					char *session,
					char *folder_name,
					char *role,
					enum omit_delete_operation
						omit_delete_operation );

OPERATION *operation_get( 		LIST *operation_list, 
					char *operation_string );

int operation_list_perform_operations(
				DICTIONARY *send_dictionary,
				DICTIONARY *row_dictionary,
				LIST *operation_list,
				char *application_name,
				char *session,
				char *person,
				char *folder_name,
				char *role_name,
				LIST *primary_key_list,
				boolean non_owner_forbid_deletion,
				char *target_frame );

LIST *operation_list_get_operation_list(
					char *application_name,
					char *session,
					char *folder_name,
					char *role,
					enum omit_delete_operation
						omit_delete_operation );

char *operation_list_display(	LIST *operation_list );

LIST *operation_list_get_name_list(
				LIST *operation_list );

LIST *operation_list_get_operation_name_list(
				LIST *operation_list );

#endif

