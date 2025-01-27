To retire.
/* operation_list.c 							*/
/* -------------------------------------------------------------------- */
/* This is the appaserver operation and operation_list ADT.		*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_library.h"
#include "operation_list.h"
#include "appaserver_error.h"
#include "folder.h"
#include "dictionary.h"
#include "timlib.h"
#include "list.h"
#include "element.h"

OPERATION_LIST_STRUCTURE *operation_list_structure_new(
			char *application_name,
			char *session,
			char *folder_name,
			char *role,
			enum omit_delete_operation
				omit_delete_operation )
{
	OPERATION_LIST_STRUCTURE *operation_list_structure;

	operation_list_structure = (OPERATION_LIST_STRUCTURE *)
				calloc( 1, sizeof( OPERATION_LIST_STRUCTURE ) );

	operation_list_structure->operation_list = 
		operation_list_get_operation_list(
			application_name,
			session,
			folder_name,
			role,
			omit_delete_operation );

	return operation_list_structure;

}

LIST *operation_list_get_operation_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *role,
			enum omit_delete_operation
				omit_delete_operation )
{
	char sys_string[ 1024 ];
	LIST *operation_list;
	OPERATION *operation;
	LIST *operation_string_list;
	char *operation_name;
	boolean empty_placeholder_instead;

	sprintf( sys_string, 
		 "operations4folder.sh %s %s %s 2>>%s",
		 application_name,
		 folder_name,
		 role,
		 appaserver_error_get_filename( application_name ) );

	operation_string_list = pipe2list( sys_string );

	if ( !list_rewind( operation_string_list ) ) return (LIST *)0;

	operation_list = list_new();

	do {
		operation_name = 
			list_get_pointer( operation_string_list );

		empty_placeholder_instead = 0;

		if ( timlib_strncmp( operation_name, "delete" ) == 0 )
		{
			/* Mto1 detail shouldn't have delete */
			/* --------------------------------- */
			if ( omit_delete_operation == omit_delete )
			{
				continue;
			}
			else
			/* Viewonly rows should have an empty column */
			/* ----------------------------------------- */
			if ( omit_delete_operation ==
				omit_delete_with_placeholder )
			{
				empty_placeholder_instead = 1;
			}
		}

		operation =
			operation_new_operation(
				application_name,
				session,
				strdup( operation_name ),
				empty_placeholder_instead );

		list_set(
			operation_list,
			operation );

	} while( list_next( operation_string_list ) );

	return operation_list;
}

int operation_list_perform_operations(
			DICTIONARY *send_dictionary,
			DICTIONARY *row_dictionary,
			LIST *operation_list,
			char *application_name,
			char *session,
			char *login_name,
			char *folder_name,
			char *role_name,
			LIST *primary_key_list,
			boolean non_owner_forbid_deletion,
			char *target_frame )
{
	OPERATION *operation;
	int performed_any_output = 0;
	char *state;

	if ( !list_rewind( operation_list ) ) return 0;

	do {
		operation = list_get_pointer( operation_list );

		if ( timlib_strncmp(
			operation->process->process_name,
			"delete" ) == 0 )
		{
			state = "delete";
		}
		else
		{
			state = "update";
		}

		if ( !operation->empty_placeholder_instead )
		{
			char *operation_row_count_string;

			operation_row_count_string =
				operation_get_operation_row_count_string(
					row_dictionary,
					operation->process->process_name,
					0 /* highest_index */ );

			performed_any_output +=
				operation_perform(
					send_dictionary,
					row_dictionary,
					application_name,
					session,
					login_name,
					folder_name,
					role_name,
					primary_key_list,
					operation->process->process_name,
					operation->process->executable,
					operation->output_yn,
					non_owner_forbid_deletion,
					target_frame,
					operation_row_count_string,
					state );
		}
	} while( list_next( operation_list ) );

	return performed_any_output;
}

LIST *operation_list_get_operation_name_list(
				LIST *operation_list )
{
	return operation_list_get_name_list( operation_list );
}

LIST *operation_list_get_name_list(
				LIST *operation_list )
{
	OPERATION *operation;
	LIST *operation_name_list;

	if ( !list_rewind( operation_list ) ) return (LIST *)0;

	operation_name_list = list_new();

	do {
		operation = list_get_pointer( operation_list );

		list_append_pointer(	operation_name_list,
					operation->process->process_name );

	} while( list_next( operation_list ) );
	return operation_name_list;
}

char *operation_list_display( LIST *operation_list )
{
	char buffer[ 4096 ];
	char *ptr = buffer;
	OPERATION *operation;

	if ( !list_rewind( operation_list ) ) return "";

	*ptr = '\0';

	do {
		operation = list_get_pointer( operation_list );

		if ( !list_at_head( operation_list ) )
			ptr += sprintf( ptr, "," );

		ptr += sprintf(	ptr,
				"%s: output=%c",
				operation->process->process_name,
				(operation->output_yn)
					? operation->output_yn
					: 'n' );
	} while( list_next( operation_list ) );
	return strdup( buffer );
}

