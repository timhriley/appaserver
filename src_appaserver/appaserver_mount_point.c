/* $APPASERVER_HOME/src_appaserver/appaserver_mount_point.c		*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
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

	if ( strcmp( *argv, "appaserver_data_directory" ) == 0
	||   strcmp( *argv, "appaserver_data_directory.e" ) == 0 )
	{
		output =
		appaserver_parameter_data_directory();
	}
	else
	if ( strcmp( *argv, "document_root" ) == 0
	||   strcmp( *argv, "document_root.e" ) == 0 )
	{
		output =
		appaserver_parameter_document_root();
	}
	else
	if ( strcmp( *argv, "appaserver_error_directory" ) == 0
	||   strcmp( *argv, "appaserver_error_directory.e" ) == 0 )
	{
		output =
		appaserver_parameter_error_directory();
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
	return 0;
}

