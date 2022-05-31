/* ---------------------------------------------------- */
/* $APPASERVER_HOME/library/semaphore.c			*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "timlib.h"
#include "semaphore.h"

int semaphore( key_t key )
{
	int status = 0;
	int semid;

	if ( ( semid = semget( key, 1, 0666|IPC_CREAT|IPC_EXCL ) ) == -1 )
	{
		if ( errno == EEXIST )
		{
			semid = semget( key, 1, 0 );
		}
	}
	else
	{
		status = semctl( semid, 0, SETVAL, 1 );
	}

	if ( semid == -1 || status == -1 )
	{
		char msg[ 128 ];
		sprintf( msg, "%s() failed", __FUNCTION__ );
		perror( msg );
		return -1;
	}

	return semid;
}

void semaphore_wait( int semaphore_id )
{
	struct sembuf p_buf;

	p_buf.sem_num = 0;
	p_buf.sem_op = -1;
	p_buf.sem_flg = SEM_UNDO;

	if ( semop( semaphore_id, &p_buf, 1 ) == -1 )
	{
		char msg[ 128 ];
		sprintf( msg, "%s() failed", __FUNCTION__ );
		perror( msg );
		exit( 1 );
	}
}

void semaphore_signal( int semaphore_id )
{
	struct sembuf v_buf;

	v_buf.sem_num = 0;
	v_buf.sem_op = 1;
	v_buf.sem_flg = SEM_UNDO;

	if ( semop( semaphore_id, &v_buf, 1 ) == -1 )
	{
		char msg[ 128 ];
		sprintf( msg, "%s() failed", __FUNCTION__ );
		perror( msg );
		exit( 1 );
	}
}

