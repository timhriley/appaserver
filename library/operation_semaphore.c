/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/operation_semaphore.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "application.h"
#include "appaserver.h"
#include "piece.h"
#include "appaserver_error.h"
#include "operation_semaphore.h"

OPERATION_SEMAPHORE *operation_semaphore_calloc( void )
{
	OPERATION_SEMAPHORE *operation_semaphore;

	operation_semaphore =
		calloc( 1, sizeof ( OPERATION_SEMAPHORE ) );

	if ( !operation_semaphore )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return operation_semaphore;
}

OPERATION_SEMAPHORE *operation_semaphore_new(
		char *application_name,
		char *operation_name,
		char *appaserver_data_directory,
		pid_t parent_process_id,
		int operation_row_checked_count )
{
	OPERATION_SEMAPHORE *operation_semaphore;

	if ( !application_name
	||   !operation_name
	||   !appaserver_data_directory
	||   !parent_process_id
	||   !operation_row_checked_count )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	operation_semaphore = operation_semaphore_calloc();

	operation_semaphore->filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		operation_semaphore_filename(
			application_name,
			operation_name,
			appaserver_data_directory,
			parent_process_id );

	if ( ( operation_semaphore->group_first_time =
		operation_semaphore_group_first_time(
			operation_semaphore->filename ) ) )
	{
		operation_semaphore->row_current = 1;
	}
	else
	{
		operation_semaphore->row_current =
			operation_semaphore_row_current(
				operation_semaphore->filename );

		operation_semaphore->row_current++;
	}

	if ( ( operation_semaphore->group_last_time =
			operation_semaphore_group_last_time(
				operation_semaphore->row_current,
				operation_row_checked_count ) ) )
	{
		if ( !operation_semaphore->group_first_time )
		{
			operation_semaphore_remove_file(
				operation_semaphore->filename );
		}
	}
	else
	{
		operation_semaphore_write(
			operation_semaphore->filename,
			operation_semaphore->row_current );
	}

	return operation_semaphore;
}

void operation_semaphore_remove_file( char *filename )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"rm -f %s",
		filename );

	if ( system( system_string ) ){};
}

char *operation_semaphore_filename(
		char *application_name,
		char *operation_name,
		char *appaserver_data_directory,
		pid_t parent_process_id )
{
	char semaphore_filename[ 512 ];

	if ( !application_name
	||   !appaserver_data_directory
	||   !operation_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(semaphore_filename,
		"%s/%s/%s_%d.dat",
		appaserver_data_directory,
		application_name,
		operation_name,
		parent_process_id );

	return strdup( semaphore_filename );
}

boolean operation_semaphore_group_first_time( char *filename )
{
	return
	!application_file_exists_boolean(
		filename );
}

void operation_semaphore_initialize_file( char *filename )
{
	operation_semaphore_write( filename, 1 /* current_row */ );
}

int operation_semaphore_row_current( char *filename )
{
	FILE *input_file;
	char buffer[ 128 ];

	/* Safely returns */
	/* -------------- */
	input_file = appaserver_input_file( filename );

	string_input( buffer, input_file, sizeof ( buffer ) );
	fclose( input_file );

	if ( !*buffer )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"buffer is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return atoi( buffer );
}

boolean operation_semaphore_group_last_time(
		int row_current,
		int operation_row_checked_count )
{
	return ( row_current == operation_row_checked_count );
}

void operation_semaphore_write(
		char *filename,
		int row_current )
{
	FILE *output_file;

	/* Safely returns */
	/* -------------- */
	output_file = appaserver_output_file( filename );

	fprintf( output_file, "%d\n", row_current );
	fclose( output_file );
}

