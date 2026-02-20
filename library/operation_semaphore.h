/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/operation_semaphore.h			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef OPERATION_SEMAPHORE_H
#define OPERATION_SEMAPHORE_H

#include <sys/types.h>
#include <unistd.h>
#include "process.h"
#include "boolean.h"

typedef struct
{
	char *filename;
	boolean group_first_time;
	int row_current;
	boolean group_last_time;
} OPERATION_SEMAPHORE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
OPERATION_SEMAPHORE *operation_semaphore_new(
		char *application_name,
		char *operation_name,
		char *data_directory,
		pid_t parent_process_id,
		int operation_row_checked_count );

/* Process */
/* ------- */
OPERATION_SEMAPHORE *operation_semaphore_calloc(
		void );

/* Returns heap memory */
/* ------------------- */
char *operation_semaphore_filename(
		char *application_name,
		char *operation_name,
		char *data_directory,
		pid_t parent_process_id );

boolean operation_semaphore_group_last_time(
		int row_current,
		int operation_row_checked_count );

void operation_semaphore_remove_file(
		char *filename );

/* Usage */
/* ----- */
boolean operation_semaphore_group_first_time(
		char *operation_semaphore_filename );

/* Usage */
/* ----- */
int operation_semaphore_row_current(
			char *filename );

/* Usage */
/* ----- */
void operation_semaphore_write(
		char *filename,
		int row_current );

#endif

