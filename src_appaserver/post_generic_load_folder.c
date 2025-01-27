/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_generic_load_folder.c		*/
/* ---------------------------------------------------------------	*/
/* No warranty and freely available software: see Appaserver.org	*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "security.h"
#include "application.h"
#include "document.h"
#include "generic_load.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *process_name;
	char *folder_name;
	GENERIC_LOAD_INSERT *generic_load_insert;

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

	generic_load_insert =
		generic_load_insert_new(
			argc,
			argv,
			application_name,
			session_key,
			login_name,
			role_name,
			process_name,
			folder_name );

	application_name =
		/* ---------------------------- */
		/* Returns heap memory or datum */
		/* ---------------------------- */
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			application_name /* datum */ ),

	folder_name =
		security_sql_injection_escape(
			SECURITY_ESCAPE_CHARACTER_STRING,
			folder_name );

	document_process_output(
		application_name,
		(LIST *)0 /* javascript_filename_list */,
		process_name );

	printf( "<h1>%s</h1>\n",
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		generic_load_folder_title_string(
			application_title_string(
				application_name ),
				folder_name ) );

	if ( !generic_load_insert )
	{
		printf( "<h3>No rows loaded.</h3>\n" );
	}
	else
	{
		if ( generic_load_insert->generic_load_orphan_list )
		{
			generic_load_orphan_list_output(
				generic_load_insert->generic_load_orphan_list );
		}
		else
		if ( generic_load_insert->
				generic_load_input->
				execute )
		{
			generic_load_row_insert(
				generic_load_insert->generic_load_row_list,
				generic_load_insert->system_string );
		}
		else
		{
			generic_load_row_display(
				folder_name,
				generic_load_insert->generic_load_row_list );
		}
	}

	document_close();

	return 0;
}

