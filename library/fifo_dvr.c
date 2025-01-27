/* fifo_dvr.c */
/* ---------- */

#include <stdio.h>
#include <stdlib.h>
#include "fifo.h"

int main( void )
{
	FIFO *fifo;
	char *results_string;

/*
	fifo = fifo_new(	"sed 's/hello/hello world/'",
				getpid() );
*/
	fifo = fifo_new(	"sed 's/hello/hello world/'",
				9999 );

	/* Set the data */
	/* ------------ */
	fifo_set( fifo->output_file, "hello" );

	/* Close the output file */
	/* --------------------- */
	fclose( fifo->output_file );

	/* Start the read stream. */
	/* ---------------------- */
	fifo->input_pipe =
		fifo_open_input_pipe(
			fifo->spool_filename,
			fifo->process );

	if ( ! ( results_string = fifo_get( fifo->input_pipe ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannnot get from fifo.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
	}
	else
	{
		printf( "%s\n", results_string );
	}

	if ( fifo_get( fifo->input_pipe ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: unexpected input from fifo.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
	}

	fifo_close( fifo );

	return 0;
}
