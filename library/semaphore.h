/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/semaphore.h					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit Appaserver.org	*/
/* -------------------------------------------------------------------- */

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

typedef struct
{
	key_t key;
	int id;
} SEMAPHORE;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
SEMAPHORE *semaphore_new(
		key_t key );

/* Process */
/* ------- */
SEMAPHORE *semaphore_calloc(
		void );

/* Usage */
/* ----- */
int semaphore_id(
		key_t key );

/* Usage */
/* ----- */
void semaphore_wait(
		int semaphore_id );

/* Usage */
/* ----- */
void semaphore_signal(
		int semaphore_id );

#endif
