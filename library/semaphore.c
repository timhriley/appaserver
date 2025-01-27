/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/semaphore.c					*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include "appaserver_error.h"
#include "semaphore.h"

extern int errno;

SEMAPHORE *semaphore_new( key_t key )
{
	SEMAPHORE *semaphore = semaphore_calloc();

	semaphore->key = key;

	if ( ( semaphore->id =
		semaphore_id(
			key ) ) < 0 )
	{
		char message[ 128 ];

		sprintf(message,
			"semaphore_id(%d) returned %d.",
			(int)key,
			semaphore->id );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return semaphore;
}

SEMAPHORE *semaphore_calloc( void )
{
	SEMAPHORE *semaphore;

	if ( ! ( semaphore = calloc( 1, sizeof ( SEMAPHORE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return semaphore;
}

int semaphore_id( key_t key )
{
	int id;

	id = semget( key, 1, 0600|IPC_CREAT|IPC_EXCL );

	if ( id > -1 )
	{
		if ( semctl( id, 0, SETVAL, 1 ) == -1 )
		{
			char message[ 128 ];

			sprintf(message,
				"semctl(%d) returned -1.",
				id );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}
	}
	else
	{
		id = semget( key, 1, 0 );
	}

	return id;
}

void semaphore_wait( int id )
{
	struct sembuf semaphore_buffer;

	semaphore_buffer.sem_num = 0;
	semaphore_buffer.sem_op = -1;
	semaphore_buffer.sem_flg = SEM_UNDO;

	if ( semop( id, &semaphore_buffer, 1 ) == -1 )
	{
		char message[ 128 ];

		sprintf(message,
			"semop(%d) returned -1.",
			id );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}
}

void semaphore_signal( int id )
{
	struct sembuf semaphore_buffer;

	semaphore_buffer.sem_num = 0;
	semaphore_buffer.sem_op = 1;
	semaphore_buffer.sem_flg = SEM_UNDO;

	if ( semop( id, &semaphore_buffer, 1 ) == -1 )
	{
		char message[ 128 ];

		sprintf(message,
			"semop(%d) returned empty.",
			id );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}
}

