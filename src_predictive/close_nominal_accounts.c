/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_predictive/close_nominal_accounts.c		*/
/* ----------------------------------------------------------------	*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "environ.h"
#include "appaserver_error.h"
#include "document.h"
#include "close_nominal.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *as_of_date;
	boolean execute;
	CLOSE_NOMINAL *close_nominal;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf(stderr,
			"Usage: %s process as_of_date execute_yn\n",
			argv[ 0 ] );

		exit ( 1 );
	}

	process_name = argv[ 1 ];
	as_of_date = argv[ 2 ];
	execute = (*argv[ 3 ] == 'y');

	return 0;
}
