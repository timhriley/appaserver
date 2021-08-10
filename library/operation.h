/* library/operation.h							*/
/* -------------------------------------------------------------------- */
/* This is the appaserver operation ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef OPERATION_H
#define OPERATION_H

#include "process.h"
#include "boolean.h"

#define OPERATION_ROW_COUNT_LABEL	"operation_row_count"
#define OPERATION_ROW_ITERATION_LABEL	"operation_row_iteration"
#define OPERATION_SEMAPHORE_TEMPLATE	"%s/%s_%s_operation_%s.dat"

typedef struct
{
	char semaphore_filename[ 256 ];
	boolean group_first_time;
	boolean group_last_time;
} OPERATION_SEMAPHORE;

typedef struct
{
	PROCESS *process;
	char output_yn;
	boolean empty_placeholder_instead;
	char *label;
} OPERATION;

/* Operations */
/* ---------- */
OPERATION *operation_new_operation(	char *application_name,
					char *session,
					char *operation_name,
					boolean empty_placeholder_instead );

boolean operation_perform(		DICTIONARY *send_dictionary,
					DICTIONARY *row_dictionary,
					char *application_name,
					char *session,
					char *person,
					char *folder_name,
					char *role_name,
					LIST *primary_key_list,
					char *process_name,
					char *executable,
					char output_yn,
					boolean non_owner_forbid_deletion,
					char *target_frame,
					char *operation_row_count_string,
					char *state );

char *operation_get_operation_row_count_string(
					DICTIONARY *row_dictionary,
					char *operation_name,
					int highest_index );

OPERATION_SEMAPHORE *operation_semaphore_new(
					char *application_name,
					char *process_name,
					char *parent_process_id_string,
					char *appaserver_data_directory,
					char *operation_row_count_string );

void operation_semaphore_remove_file(	char *semaphore_filename );

#endif

