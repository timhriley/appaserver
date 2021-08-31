/* library/operation.h							*/
/* -------------------------------------------------------------------- */
/* This is the appaserver operation ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef OPERATION_H
#define OPERATION_H

#include <getpid.h>
#include "process.h"
#include "boolean.h"

#define OPERATION_ROW_TOTAL_LABEL	"operation_row_total"
#define OPERATION_ROW_ITERATION_LABEL	"operation_row_iteration"

typedef struct
{
	/* Input */
	/* ----- */
	char *application_name;
	char *operation_name;
	char *appaserver_data_directory,
	pid_t parent_process_id;
	int operation_row_total;

	/* Process */
	/* ------- */
	char filename;
	boolean group_first_time;
	int row_current;
	boolean group_last_time;
} OPERATION_SEMAPHORE;

typedef struct
{
	PROCESS *process;
	boolean output;
	boolean empty_placeholder;
	char *label;
} OPERATION;

/* OPERATION operations */
/* -------------------- */
OPERATION *operation_new(
			char *operation_name );

boolean operation_perform(
			DICTIONARY *send_dictionary,
			DICTIONARY *row_dictionary,
			char *application_name,
			char *session_key,
			char *login_name,
			char *folder_name,
			char *role_name,
			LIST *primary_key_list,
			char *process_name,
			char *command_line,
			boolean output,
			boolean non_owner_forbid_deletion,
			char *target_frame,
			int operation_row_total,
			char *state );

int operation_row_total(
			DICTIONARY *row_dictionary,
			char *operation_name,
			int highest_index );

/* OPERATION_SEMAPHORE operations */
/* ------------------------------ */
OPERATION_SEMAPHORE *operation_semaphore_fetch(
			char *operation_name,
			char *appaserver_data_directory,
			pid_t parent_process_id,
			int operation_row_total );

/* Returns heap memory */
/* ------------------- */
char *operation_semaphore_filename(
			char *operation_name,
			char *appaserver_data_directory,
			int parent_process_id );

OPERATION_SEMAPHORE *operation_semaphore_calloc(
			void );

boolean operation_semaphore_group_first_time(
			char *filename );

void operation_semaphore_initialize_file(
			char *filename );

int operation_semaphore_row_current(
			char *filename );

boolean operation_semaphore_group_last_time(
			int row_current,
			int operation_row_total );

void operation_semaphore_increment(
			char *filename,
			int row_current );

void operation_semaphore_remove_file(
			char *filename );

#endif

