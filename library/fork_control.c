/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/fork_control.c				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include "fork_control.h"

FORK_CONTROL *fork_control_calloc( void )
{
	FORK_CONTROL *fork_control;

	if ( ! ( fork_control = calloc( 1, sizeof ( FORK_CONTROL ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return fork_control;
}

FORK_CONTROL *fork_control_new( int parallel_count )
{
	FORK_CONTROL *fork_control = fork_control_calloc();

	fork_control->parallel_count = parallel_count;
	fork_control->fork_control_process_list = list_new();

	return fork_control;
}

FORK_CONTROL_PROCESS *fork_control_process_calloc( void )
{
	FORK_CONTROL_PROCESS *fork_process;

	if ( ! ( fork_process = calloc( 1, sizeof ( FORK_CONTROL_PROCESS ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return fork_process;
}

FORK_CONTROL_PROCESS *fork_control_process_new( char *command_line )
{
	FORK_CONTROL_PROCESS *fork_control_process;

	if ( !command_line )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: command_line is empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	fork_control_process = fork_control_process_calloc();

	fork_control_process->command_line = command_line;

	fork_control_process->fork_control_process_state =
		fork_control_process_initial;

	return fork_control_process;
}

void fork_control_process_set(
			LIST *fork_control_process_list,
			char *command_line )
{
	FORK_CONTROL_PROCESS *fork_control_process;

	if ( !fork_control_process_list
	||   !command_line )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: parameter is empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	fork_control_process = fork_control_process_new( command_line );
	list_set( fork_control_process_list, fork_control_process );
}

void fork_control_execute(
			int parallel_count,
			LIST *process_list )
{
	int process_count = 0;
	FORK_CONTROL_PROCESS *fork_control_process;
	pid_t done_process_id;

	if ( !process_list ) return;

	if ( !parallel_count ) parallel_count = 1;

	while ( 1 )
	{
		if ( ( fork_control_process =
			fork_control_process_next(
				process_list ) ) )
		{
			fork_control_process_execute(
				&fork_control_process->
					fork_control_process_state,
				&fork_control_process->
					running_process_id,
				fork_control_process->command_line );

			process_count++;
		}

		if ( !process_count ) break;

		if ( process_count < parallel_count )
		{
			if ( fork_control_process_pending_boolean(
				process_list ) )
			{
				continue;
			}
		}

		if ( ( done_process_id = wait( (int *)0 ) ) )
		{
			if ( ! ( fork_control_process =
					fork_control_process_seek(
						process_list,
						done_process_id ) ) )
			{
				fprintf( stderr,
	"ERROR in %s/%s()/%d: fork_control_process_seek(%d) returned empty.\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					 done_process_id );
				exit( 1 );
			}

			fork_control_process->
				fork_control_process_state =
					fork_control_process_done;

			process_count--;
		}
	}
}

FORK_CONTROL_PROCESS *fork_control_process_next( LIST *process_list )
{
	FORK_CONTROL_PROCESS *fork_control_process;

	if ( !list_rewind( process_list ) ) return NULL;

	do {
		fork_control_process = list_get( process_list );

		if ( fork_control_process->
			fork_control_process_state ==
				fork_control_process_initial )
		{
			return fork_control_process;
		}

	} while( list_next( process_list ) );

	return NULL;
}

void fork_control_process_execute(
				enum fork_control_process_state *state,
				pid_t *running_process_id,
				char *command_line )
{
	if ( ( *running_process_id = fork() ) == 0 )
	{
		execlp( "/bin/sh", "sh", "-c", command_line, (char *)0 );

		fprintf( stderr,
			 "ERROR in %s/%s()/%d: execlp() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	else
	{
		*state = fork_control_process_executing;
	}
}

FORK_CONTROL_PROCESS *fork_control_process_seek(
			LIST *process_list,
			pid_t done_process_id )
{
	FORK_CONTROL_PROCESS *fork_control_process;

	if ( !list_rewind( process_list ) ) return NULL;

	do {
		fork_control_process = list_get( process_list );

		if ( fork_control_process->
			running_process_id ==
				done_process_id )
		{
			return fork_control_process;
		}

	} while( list_next( process_list ) );

	return NULL;
}

boolean fork_control_process_pending_boolean(
				LIST *process_list )
{
	FORK_CONTROL_PROCESS *fork_control_process;

	if ( !list_rewind( process_list ) ) return 0;

	do {
		fork_control_process = list_get( process_list );

		if ( fork_control_process->
			fork_control_process_state ==
				fork_control_process_initial )
		{
			return 1;
		}

	} while( list_next( process_list ) );

	return 0;
}

void fork_control_process_list_display( LIST *process_list )
{
	FORK_CONTROL_PROCESS *fork_control_process;

	if ( !list_rewind( process_list ) ) return;

	do {
		fork_control_process = list_get( process_list );

		printf( "%s\n",fork_control_process->command_line );

	} while( list_next( process_list ) );
}

