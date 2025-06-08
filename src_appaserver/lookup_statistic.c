/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_appaserver/lookup_statistic.c			*/
/* -------------------------------------------------------------------- */
/* This process is triggered if you select the statistics radio		*/
/* button in a lookup form. 						*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "String.h"
#include "appaserver_error.h"
#include "environ.h"
#include "lookup_statistic.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *role_name;
	char *folder_name;
	char dictionary_string[ STRING_SIZE_HASH_TABLE ];
	LOOKUP_STATISTIC *lookup_statistic;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf(stderr, 
			"Usage: %s login role folder\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	role_name = argv[ 2 ];
	folder_name = argv[ 3 ];

	string_input( dictionary_string, stdin, sizeof ( dictionary_string )  );

	lookup_statistic =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		lookup_statistic_new(
			application_name,
			login_name,
			role_name,
			folder_name,
			dictionary_string );

	lookup_statistic_output(
		application_name,
		lookup_statistic );

	return 0;
}

