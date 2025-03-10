/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_appaserver/appaserver_mount_point.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "appaserver_parameter.h"

int main( int argc, char **argv )
{
	char *output;

	if ( argc ){};

	if ( strcmp( *argv, "appaserver_data_directory" ) == 0 )
	{
		output =
		appaserver_parameter_data_directory();
	}
	else
	if ( strcmp( *argv, "document_root" ) == 0 )
	{
		output =
		appaserver_parameter_document_root();
	}
	else
	if ( strcmp( *argv, "appaserver_error_directory" ) == 0 )
	{
		output =
		appaserver_parameter_log_directory();
	}
	else
	if ( strcmp( *argv, "appaserver_log_directory" ) == 0 )
	{
		output =
		appaserver_parameter_log_directory();
	}
	else
	if ( strcmp( *argv, "appaserver_upload_directory" ) == 0 )
	{
		output =
		appaserver_parameter_upload_directory();
	}
	else
	if ( strcmp( *argv, "appaserver_backup_directory" ) == 0 )
	{
		output =
		appaserver_parameter_backup_directory();
	}
	else
	{
		output =
			appaserver_parameter_mount_point();
	}

	if ( !output )
	{
		fprintf( stderr,
	 		 "%s failed: Check appaserver.config\n",
			 argv[ 0 ] );
		exit( 1 );

	}

	printf( "%s\n", output );

	exit( 0 );
}

