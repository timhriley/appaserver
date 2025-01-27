/* library/fifo.c */
/* -------------- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "fifo.h"
#include "timlib.h"

FIFO *fifo_new(	char *process,
		pid_t pid )
{
	FIFO *fifo;

	if ( ! ( fifo = (FIFO *)calloc( 1, sizeof( FIFO ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	fifo->process = process;

/*
	fifo->fifo_filename = fifo_get_fifo_filename( pid );
	fifo_make( fifo->fifo_filename );
*/

	fifo->spool_filename = fifo_get_spool_filename( pid );
	if ( ! ( fifo->output_file = fopen( fifo->spool_filename, "w" ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot open %s for write.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			fifo->spool_filename );
		exit( 1 );
	}

	return fifo;

} /* fifo_new() */

void fifo_set(		FILE *output_file,
			char *string )
{
	if ( !output_file )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: output_file is null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	fprintf( output_file, "%s\n", string );
} /* fifo_set() */

char *fifo_get( FILE *input_pipe )
{
	static char string[ 1024 ];

	if ( get_line( string, input_pipe ) )
		return string;
	else
		return (char *)0;
} /* fifo_get() */

void fifo_close( FIFO *fifo )
{
	char sys_string[ 1024 ];
	int results;

	pclose( fifo->input_pipe );

	sprintf( sys_string, "rm -f %s", fifo->spool_filename );
	results = system( sys_string );

	free( fifo );
} /* fifo_close() */

char *fifo_get_fifo_filename( pid_t pid )
{
	static char fifo_filename[ 128 ];

	sprintf( fifo_filename,
		 FIFO_FILENAME_TEMPLATE,
		 pid );

	return fifo_filename;
} /* fifo_get_fifo_filename() */

char *fifo_get_spool_filename( pid_t pid )
{
	static char spool_filename[ 128 ];

	sprintf( spool_filename,
		 FIFO_SPOOL_FILENAME_TEMPLATE,
		 pid );

	return spool_filename;
} /* fifo_get_spool_filename() */

void fifo_make( char *fifo_filename )
{
	char sys_string[ 1024 ];

	sprintf( sys_string, "mkfifo %s 2>/dev/null", fifo_filename );
	if ( system( sys_string ) )
	{
		fprintf( stderr,
			 "Warning in %s/%s()/%d: cannot make fifo %s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 fifo_filename );
	}
} /* fifo_make() */

void fifo_execute_process(	char *spool_filename,
				char *process,
				char *fifo_filename )
{
	char sys_string[ 1024 ];
	int results;

	sprintf( sys_string,
		 "cat %s | %s | fifowrite.e %s &",
		 spool_filename,
		 process,
		 fifo_filename );

	results = system( sys_string );
fprintf( stderr, "sleeping...\n" );
	sleep( 1 );

} /* fifo_execute_process() */

FILE *fifo_open_input_pipe(	char *spool_filename,
				char *process )
{
	char sys_string[ 1024 ];

/*
	sprintf( sys_string,
		 "fiforead.e %s",
		 fifo_filename );
*/
	sprintf( sys_string,
		 "cat %s | %s",
		 spool_filename,
		 process );


	return popen( sys_string, "r" );

} /* fifo_open_input_pipe() */

