/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/utility/date_convert_login_format.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "date_convert.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	enum date_convert_format_enum date_convert_format_enum;

	if ( argc != 3 )
	{
		fprintf(stderr,
			"Usage: %s application login\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	login_name = argv[ 2 ];

	date_convert_format_enum =
		date_convert_login_name_enum(
			application_name,
			login_name );

	printf( "%s\n",
		/* ---------------------- */
		/* Returns program memory */
		/* ---------------------- */
		date_convert_format_string(
			date_convert_format_enum ) );

	return 0;
}
