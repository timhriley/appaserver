/* library/fork_control.c						*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include "fork_control.h"

FORK_CONTROL *fork_control_new( void )
{
	FORK_CONTROL *fork_control;

	if ( ! ( fork_control = calloc( 1, sizeof( FORK_CONTROL ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	fork_control->process_list = list_new();
	return fork_control;

}

FORK_CONTROL_PROCESS *fork_control_process_new( char *command_line )
{
	FORK_CONTROL_PROCESS *fork_process;

	if ( ! ( fork_process = calloc( 1, sizeof( FORK_CONTROL_PROCESS ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	fork_process->command_line = command_line;
	fork_process->state = fork_control_initial;
	return fork_process;

}

void fork_control_append_process(
		LIST *process_list,
		char *command_line )
{
	FORK_CONTROL_PROCESS *fork_control_process;

	fork_control_process = fork_control_process_new( command_line );
	list_append_pointer( process_list, fork_control_process );

}

void fork_control_execute(
		LIST *process_list,
		int processes_in_parallel )
{
	int processes_executing = 0;
	FORK_CONTROL_PROCESS *fork_control_process;
	pid_t done_process_id;

	while( 1 )
	{
		if ( ( fork_control_process =
				fork_control_get_next_process(
				process_list ) ) )
		{
			fork_control_execute_process(
				&fork_control_process->state,
				&fork_control_process->running_process_id,
				fork_control_process->command_line );

			processes_executing++;
		}

		if ( processes_executing == 0 ) break;

		if ( processes_executing < processes_in_parallel )
		{
			if ( fork_control_get_any_processes_pending(
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
			"ERROR in %s/%s()/%d: cannot find process = %d\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					 done_process_id );
				exit( 1 );
			}

			fork_control_process->state = fork_control_done;
			processes_executing--;
		}
	}
}

FORK_CONTROL_PROCESS *fork_control_get_next_process(
			LIST *process_list )
{
	FORK_CONTROL_PROCESS *fork_control_process;

	if ( !list_rewind( process_list ) )
		return (FORK_CONTROL_PROCESS *)0;

	do {
		fork_control_process = list_get_pointer( process_list );

		if ( fork_control_process->state == fork_control_initial )
			return fork_control_process;

	} while( list_next( process_list ) );

	return (FORK_CONTROL_PROCESS *)0;

}

void fork_control_execute_process(
				enum fork_control_state *state,
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
		*state = fork_control_executing;
	}
}

FORK_CONTROL_PROCESS *fork_control_process_seek(
			LIST *process_list,
			pid_t process_id )
{
	FORK_CONTROL_PROCESS *fork_control_process;

	if ( !list_rewind( process_list ) ) return (FORK_CONTROL_PROCESS *)0;

	do {
		fork_control_process = list_get_pointer( process_list );

		if ( fork_control_process->running_process_id == process_id )
			return fork_control_process;

	} while( list_next( process_list ) );

	return (FORK_CONTROL_PROCESS *)0;

}

boolean fork_control_get_any_processes_pending(
				LIST *process_list )
{
	FORK_CONTROL_PROCESS *fork_control_process;

	if ( !list_rewind( process_list ) ) return 0;

	do {
		fork_control_process = list_get_pointer( process_list );

		if ( fork_control_process->state == fork_control_initial )
			return 1;

	} while( list_next( process_list ) );

	return 0;

}

void fork_control_process_list_display(
				LIST *process_list )
{
	FORK_CONTROL_PROCESS *fork_control_process;

	if ( !list_rewind( process_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty process_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		fork_control_process = list_get_pointer( process_list );

		printf( "%s\n",fork_control_process->command_line );

	} while( list_next( process_list ) );

}

