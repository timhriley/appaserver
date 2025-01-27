/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/appaserver_error_filename.c		*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "environ.h"
#include "appaserver_error.h"

int main( int argc, char **argv )
{
	APPASERVER_ERROR *appaserver_error;

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s application_name\n",
			argv[ 0 ] );
		exit( 1 );
	}

	appaserver_error =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_error_new(
			argv[ 1 ] /* application_name */ );

	printf( "%s\n",
		appaserver_error->filename );

	return 0;
}
