/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/spool.c	 				*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver.h"
#include "appaserver_error.h"
#include "spool.h"

SPOOL *spool_new(
		char *system_string,
		boolean capture_stderr_boolean )
{
	SPOOL *spool;

	if ( !system_string )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: system_string is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	spool = spool_calloc();

	spool->output_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		spool_output_filename(
			getpid() /* process_id */ );

	spool->create_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		spool_create_system_string(
			spool->output_filename );

	if ( system( spool->create_system_string ) ){}

	spool->output_system_string =
		spool_output_system_string(
			system_string,
			capture_stderr_boolean,
			spool->output_filename );

	spool->output_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		spool_output_pipe(
			spool->output_system_string );

	return spool;
}

SPOOL *spool_calloc( void )
{
	SPOOL *spool;

	if ( ! ( spool = calloc( 1, sizeof ( SPOOL ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return spool;
}

char *spool_output_filename( pid_t process_id )
{
	static char output_filename[ 128 ];

	snprintf(
		output_filename,
		sizeof ( output_filename ),
		"/tmp/spool_%d",
		process_id );

	return output_filename;
}

char *spool_create_system_string( char *output_filename )
{
	static char create_system_string[ 512 ];

	snprintf(
		create_system_string,
		sizeof ( create_system_string ),
		"echo '' > %s && chmod go-rw %s",
		output_filename,
		output_filename );

	return create_system_string;
}

char *spool_output_system_string(
		char *system_string, 
		boolean capture_stderr_boolean,
		char *output_filename )
{
	static char output_system_string[ 512 ];
	char *ptr = output_system_string;

	ptr += sprintf( ptr,
		"%s >%s",
		system_string,
		output_filename );

	if ( capture_stderr_boolean ) sprintf( ptr, " 2>&1" );

	return output_system_string;
}

FILE *spool_output_pipe( char *output_system_string )
{
	return
	/* -------------- */
	/* Safely returns */
	/* -------------- */
	appaserver_output_pipe(
		output_system_string );
}

LIST *spool_list( char *output_filename )
{
	FILE *input_file;
	LIST *list;
	char *remove_system_string;

	input_file =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_input_file(
			output_filename );

	list = list_file_fetch( input_file );

	fclose( input_file );

	remove_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		spool_remove_system_string(
			output_filename );

	if ( system( remove_system_string ) ){}

	return list;
}

char *spool_remove_system_string( char *output_filename )
{
	static char remove_system_string[ 128 ];

	snprintf(
		remove_system_string,
		sizeof ( remove_system_string ),
		"rm %s",
		output_filename );

	return remove_system_string;
}

char *spool_fetch(
		char *system_string,
		boolean capture_stderr_boolean )
{
	return
	spool_pipe(
		system_string,
		capture_stderr_boolean );
}

char *spool_pipe(
		char *system_string,
		boolean capture_stderr_boolean )
{
	SPOOL *spool;
	LIST *list;

	if ( !system_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"system_string is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	spool =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		spool_new(
			system_string,
			capture_stderr_boolean );

	pclose( spool->output_pipe );

	list = spool_list( spool->output_filename );

	if ( !list_length( list ) ) return NULL;

	return (char *)list_first( list );
}

