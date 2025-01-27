/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_hydrology/station_datatype_list.c		*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "environ.h"
#include "appaserver_error.h"
#include "station_datatype.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *station_name;
	char *datatype_name;
	char *many_folder_name;
	char *where;
	LIST *list;

	application_name = environment_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf(stderr,
			"Usage: %s station datatype many_folder where\n",
			argv[ 0 ] );
		exit( 1 );
	}

	station_name = argv[ 1 ];
	datatype_name = argv[ 2 ];
	many_folder_name = argv[ 3 ];
	where = argv[ 4 ];

	list =
		station_datatype_list(
			station_name,
			datatype_name,
			where,
			1 /* fetch_datatype */ );

	if ( strcmp( many_folder_name, "site_visit_calibrate" ) == 0 )
	{
		station_datatype_site_visit_calibrate_output( list );
	}
	else
	{
		station_datatype_output( list );
	}

	return 0;
}
