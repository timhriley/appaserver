/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/upload_source_file.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "environ.h"
#include "process.h"
#include "document.h"
#include "upload_source.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *directory_filename_session;
	APPASERVER_PARAMETER *appaserver_parameter;
	UPLOAD_SOURCE *upload_source;

	application_name = environment_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 3 )
	{
		fprintf(stderr, 
			"Usage: %s process_name filename\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	process_name = argv[ 1 ];
	directory_filename_session = argv[ 2 ];

	/* Safely returns */
	/* -------------- */
	appaserver_parameter = appaserver_parameter_new();

	upload_source =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		upload_source_new(
			application_name,
			directory_filename_session,
			appaserver_parameter->mount_point );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name );

	if ( !upload_source->file_exists_boolean )
	{
		printf( "<h3>Please select a filename</h3>\n" );
	}
	else
	if ( !upload_source->destination_directory_exists_boolean )
	{
		printf(
		"<h3>ERROR: the source directory is not found.</h3>\n" );
	}
	else
	{
		if ( system( upload_source->copy_system_string ) ){}
		if ( system( upload_source->chmod_system_string ) ) {}

		fflush( stdout );
		printf( "<h3>Process executed. Here is the result:</h3>\n" );
		fflush( stdout );

		if ( system( upload_source->confirm_system_string ) ) {}

		process_execution_count_increment( process_name );

	}

	return 0;
}
