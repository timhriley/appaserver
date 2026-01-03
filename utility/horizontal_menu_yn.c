/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/utility/horizontal_menu_yn.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "appaserver.h"
#include "timlib.h"
#include "environ.h"
#include "frameset.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *database_string = {0};

	if ( argc != 3 )
	{
		fprintf( stderr, 
			"Usage: %s application login_name\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	login_name = argv[ 2 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			"DATABASE",
			database_string );
	}

	if ( frameset_menu_horizontal_boolean(
		application_name,
		login_name ) )
	{
		printf( "y\n" );
	}
	else
	{
		printf( "n\n" );
	}

	return 0;
} /* main() */

