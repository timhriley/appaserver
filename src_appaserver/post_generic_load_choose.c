/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_generic_load_choose.c		*/
/* ---------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "security.h"
#include "generic_load.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_name;
	char *folder_name;
	GENERIC_LOAD_FOLDER *generic_load_folder;

	if ( argc != 7 )
	{
		fprintf(stderr,
		"Usage: %s application session login role process folder\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	process_name = argv[ 5 ];
	folder_name = argv[ 6 ];

	document_content_type_output();

	generic_load_folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		generic_load_folder_new(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			process_name,
			folder_name );

	printf( "%s\n",
		generic_load_folder->
			document_form_html );

	return 0;
}

