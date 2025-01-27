/* --------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_appaserver/post_merge_purge_choose.c		*/
/* --------------------------------------------------------------- 	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------- 	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "appaserver_error.h"
#include "merge_purge.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_name;
	char *folder_name;
	MERGE_PURGE_FOLDER *merge_purge_folder;

	if ( argc != 7 )
	{
		fprintf(stderr, 
		"Usage: %s application session login role process folder\n",
		argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	process_name = argv[ 5 ];
	folder_name = argv[ 6 ];

	document_content_type_output();

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	merge_purge_folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		merge_purge_folder_new(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			process_name,
			folder_name );

	printf( "%s\n",
		merge_purge_folder->
			document_form_html );

	return 0;
}

