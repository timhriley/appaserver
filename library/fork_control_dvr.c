/* fork_control_dvr.c */
/* ------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fork_control.h"

char *get_process( int sleep_seconds );

int main( int argc, char **argv )
{
	FORK_CONTROL *fork_control;

	if ( argc != 2 )
	{
		fprintf(	stderr,
				"Usage: %s processes_in_parallel\n",
				argv[ 0 ] );
		exit( 1 );
	}

	fork_control = fork_control_new();

	fork_control->processes_in_parallel = atoi( argv[ 1 ] );

	fork_control_append_process(
		fork_control->process_list, get_process( 50 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 18 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 30 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 6 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 4 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 25 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 8 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 6 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 8 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 6 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 4 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 25 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 6 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 15 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 10 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 8 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 26 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 10 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 4 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 4 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 8 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 6 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 4 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 8 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 1 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 10 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 8 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 16 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 4 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 10 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 8 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 6 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 4 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 8 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 6 ) );

	fork_control_append_process(
		fork_control->process_list, get_process( 4 ) );

	fprintf( stderr,
		 "Executing %d processes\n",
		 list_length( fork_control->process_list ) );

	fork_control_execute(
		fork_control->process_list,
		fork_control->processes_in_parallel );

	return 0;
} /* main() */

char *get_process( int sleep_seconds )
{
	char process[ 128 ];
	static int process_number = 0;

	process_number++;

	sprintf( process,
	"sleep %d; echo 'done %d'",
		 sleep_seconds,
		 process_number );

	return strdup( process );

} /* get_process() */

