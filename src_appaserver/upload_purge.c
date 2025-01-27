/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/upload_purge.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "boolean.h"
#include "list.h"
#include "environ.h"
#include "appaserver_parameter.h"
#include "upload_purge.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *key;
	char *table_name;
	char *column_name;
	UPLOAD_PURGE *upload_purge;

	application_name = environ_exit_application_name( argv[ 0 ] );

	if ( argc != 4 )
	{
		fprintf(stderr,
			"Usage: %s key table column\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	key = argv[ 1 ];
	table_name = argv[ 2 ];
	column_name = argv[ 3 ];

	upload_purge =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		upload_purge_new(
			application_name,
			key,
			table_name,
			column_name,
			appaserver_parameter_upload_directory() );

	upload_purge_execute(
		upload_purge->directory,
		upload_purge->system_list,
		0 /* not display_only_boolean */ );

	return 0;
}
