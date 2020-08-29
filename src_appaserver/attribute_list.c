/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/attribute_list.c	*/
/* ---------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "list.h"
#include "piece.h"
#include "appaserver_error.h"
#include "environ.h"
#include "boolean.h"
#include "attribute.h"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *folder_name;
	LIST *local_attribute_name_list;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 2 )
	{
		fprintf(stderr,
			"Usage: %s folder\n",
			argv[ 0 ] );
		exit( 1 );
	}

	folder_name = argv[ 1 ];

	local_attribute_name_list =
		attribute_name_list(
			attribute_list(
				folder_name ) );

	if ( list_rewind( local_attribute_name_list ) )
	{
		do {
			printf( "%s\n",
				(char *)list_get(
					local_attribute_name_list ) );

		} while ( list_next( local_attribute_name_list ) );
	}

	return 0;
}

