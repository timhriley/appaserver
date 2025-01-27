/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/database_table.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "appaserver_error.h"
#include "application.h"
#include "environ.h"

int main( int argc, char **argv )
{
	char *table_name;

	/* appaserver_error_stderr( argc, argv ); */

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s table_name\n",
			argv[ 0 ] );
		exit( 1 );
	}

	table_name = argv[ 1 ];

	printf( "%s\n",
		application_table_name(
			table_name ) );

	return 0;
}
