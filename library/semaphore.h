/* ---------------------------------------------------- */
/* $APPASERVER_HOME/library/semaphore.h			*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

/* --------------------------------------------------------------- */
/* Notes: $ ipcs          <-- lists semaphores.			   */
/* 	  $ ipcrm -s $key <-- removes semaphore previously listed. */
/* --------------------------------------------------------------- */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <errno.h>
#include "boolean.h"

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

/* External variables */
/* ------------------ */
extern int errno;

/* Constants */
/* --------- */
#define SEMAPHORE_TEMPLATE	"%s/%s_semaphore_%d.dat"
#define SEMPERM 0600

/* Structures */
/* ---------- */
typedef struct
{
	key_t semkey;
} SEMAPHORE;

typedef struct
{
	char *application_name;
	char *appaserver_data_directory;
	char *semaphore_filename;
	boolean group_first_time;
	boolean group_last_time;
	int parent_process_id;
	int operation_row_total;
	int operation_row_current;
} SEMAPHORE_OPERATION;

/* Operations */
/* ---------- */
SEMAPHORE *semaphore_new(
			key_t semkey );

void semaphore_wait(	int semid );

void semaphore_signal(	int semid );

SEMAPHORE_OPERATION *semaphore_operation_new(
			char *application_name,
			char *appaserver_data_directory,
			int parent_process_id,
			int operation_row_total );

void semaphore_operation_check(
			boolean *group_first_time,
			boolean *group_last_time,
			int operation_row_total,
			char *semaphore_filename );

#endif
