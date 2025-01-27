/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/semaphore_dvr.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "semaphore.h"

#define SEMAPHORE_TEST_KEY1		1
#define SEMAPHORE_TEST_KEY2		2
#define SEMAPHORE_TEST_KEY3		3
#define SEMAPHORE_TEST_KEY4		4
#define SEMAPHORE_TEST_KEY5		5
#define SEMAPHORE_TEST_KEY6		6
#define SEMAPHORE_TEST_KEY7		7
#define SEMAPHORE_TEST_KEY8		8
#define SEMAPHORE_TEST_KEY9		9
#define SEMAPHORE_TEST_KEY54331		54331
#define SEMAPHORE_TEST_KEY65536		65536

int main( void )
{
	SEMAPHORE *semaphore;
	key_t key = SEMAPHORE_TEST_KEY54331;
	pid_t pid = getpid();

	semaphore =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		semaphore_new(
			key );


	printf( "Process %d with semaphore-id = %d before critical section.\n",
		pid, semaphore->id);

	semaphore_wait( semaphore->id );
	printf( "Process %d inside critical section.\n", pid );
	sleep( 5 );
	semaphore_signal( semaphore->id );
	printf( "Process %d outside critical section.\n", pid );

	return 0;
}

