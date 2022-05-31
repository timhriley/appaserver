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

/* Usage */
/* ----- */
SEMAPHORE *semaphore_new(
			key_t semephore_key );

int semaphore_id(	key_t semephore_key );

void semaphore_wait(	int semid );

void semaphore_signal(	int semid );

#endif
