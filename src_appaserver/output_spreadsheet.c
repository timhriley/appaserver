/* ------------------------------------------------------------- */
/* $APPASERVER_HOME/src_appaserver/output_spreadsheet.c		 */
/* ------------------------------------------------------------- */
/* This process is triggered if you select the spreadsheet radio */
/* button in a lookup form.					 */
/*								 */
/* No warranty and freely available software: see Appaserver.org */
/* ------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "environ.h"
#include "appaserver_error.h"
#include "appaserver_parameter.h"
#include "lookup_spreadsheet.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	char *dictionary_string;
	APPASERVER_PARAMETER *appaserver_parameter;
	LOOKUP_SPREADSHEET *lookup_spreadsheet;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 6 )
	{
		fprintf(stderr, 
			"Usage: %s session login role folder dictionary\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	folder_name = argv[ 4 ];
	dictionary_string = argv[ 5 ];

	appaserver_parameter =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_parameter_new();

	lookup_spreadsheet =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		lookup_spreadsheet_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			dictionary_string,
			appaserver_parameter->data_directory );

	lookup_spreadsheet_output(
		application_name,
		lookup_spreadsheet );

	return 0;
}

