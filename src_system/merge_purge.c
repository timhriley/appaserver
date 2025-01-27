/* --------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_appaserver/merge_purge.c 			*/
/* --------------------------------------------------------------- 	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------- 	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "appaserver_error.h"
#include "environ.h"
#include "merge_purge.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_name;
	MERGE_PURGE_CHOOSE *merge_purge_choose;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 5 )
	{
		fprintf(stderr,
			"Usage: %s session login role process\n",
			argv[ 0 ] );
		exit( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	process_name = argv[ 4 ];

	merge_purge_choose =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		merge_purge_choose_new(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			process_name );

	printf( "%s\n",
		merge_purge_choose->
			document_form_html );

	return 0;
}

