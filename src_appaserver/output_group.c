/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_appaserver/output_group.c			*/
/* -------------------------------------------------------------------- */
/* This process is triggered if you select the group radio		*/
/* button in a lookup form with mto1 relations.				*/
/*									*/
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "environ.h"
#include "appaserver_parameter.h"
#include "appaserver_error.h"
#include "group_count.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *role_name;
	char *folder_name;
	char dictionary_string[ STRING_64K ];
	GROUP_COUNT *group_count;

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

	group_count =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		group_count_new(
			application_name,
			login_name,
			role_name,
			folder_name,
			dictionary_string,
			appaserver_parameter_data_directory() );

	group_count_output(
		application_name,
		group_count );

	return 0;
}

