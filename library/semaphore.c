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

void semaphore_wait( int semid )
{
	struct sembuf p_buf;

	p_buf.sem_num = 0;
	p_buf.sem_op = -1;
	p_buf.sem_flg = SEM_UNDO;

	if ( semop( semid, &p_buf, 1 ) == -1 )
	{
		char msg[ 128 ];
		sprintf( msg, "%s() failed", __FUNCTION__ );
		perror( msg );
		exit( 1 );
	}
}

void semaphore_signal( int semid )
{
	struct sembuf v_buf;

	v_buf.sem_num = 0;
	v_buf.sem_op = 1;
	v_buf.sem_flg = SEM_UNDO;

	if ( semop( semid, &v_buf, 1 ) == -1 )
	{
		char msg[ 128 ];
		sprintf( msg, "%s() failed", __FUNCTION__ );
		perror( msg );
		exit( 1 );
	}
}

SEMAPHORE_OPERATION *semaphore_operation_new(
				char *application_name,
				char *appaserver_data_directory,
				int parent_process_id,
				int operation_row_total )
{
	SEMAPHORE_OPERATION *semaphore_operation;

	if ( ! ( semaphore_operation =
			(SEMAPHORE_OPERATION *)calloc(
				1, sizeof( SEMAPHORE_OPERATION ) ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	semaphore_operation->application_name = application_name;

	semaphore_operation->appaserver_data_directory =
		appaserver_data_directory;

	semaphore_operation->parent_process_id = parent_process_id;
	semaphore_operation->operation_row_total = operation_row_total;

	return semaphore_operation;
}

void semaphore_operation_check(
			boolean *group_first_time,
			boolean *group_last_time,
			int operation_row_total,
			char *semaphore_filename )
{
	char *operation_row_current_string;
	int operation_row_current;
	char sys_string[ 1024 ];

	if ( operation_row_total == 1 )
	{
		*group_first_time = 1;
		*group_last_time = 1;
	}

	if ( !timlib_file_exists( semaphore_filename ) )
	{
		sprintf( sys_string, "echo 1 > %s", semaphore_filename );

		fflush( stdout );
		if ( system( sys_string ) ){};
		fflush( stdout );

		*group_first_time = 1;

		return;
	}

	*group_first_time = 0;

	sprintf( sys_string, "cat %s", semaphore_filename );

	operation_row_current_string = pipe2string( sys_string );

	operation_row_current = atoi( operation_row_current_string ) + 1;

	sprintf( sys_string,
		 "echo %d > %s",
		 operation_row_current,
		 semaphore_filename );

	if ( system( sys_string ) ){};

	*group_last_time = ( operation_row_current == operation_row_total );
}

