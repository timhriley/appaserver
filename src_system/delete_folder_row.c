/* --------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_system/delete_folder_row.c			*/
/* --------------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* --------------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "appaserver_error.h"
#include "environ.h"
#include "sql.h"
#include "folder_menu.h"
#include "delete_primary.h"

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *role_name;
	char *folder_name;
	char *primary_data_list_string;
	boolean delete_omit_isa_boolean;
	DELETE_PRIMARY *delete_primary;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s session login_name role folder primary_data_list delete_omit_isa_yn\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	role_name = argv[ 3 ];
	folder_name = argv[ 4 ];
	primary_data_list_string = argv[ 5 ];
	delete_omit_isa_boolean = (*argv[ 6 ] == 'y');

	delete_primary =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		delete_primary_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			primary_data_list_string,
			delete_omit_isa_boolean );

	if ( delete_primary->delete
	&&   list_length( delete_primary->delete->delete_sql_list ) )
	{
		delete_execute(
			SQL_EXECUTABLE,
			delete_primary->
				delete->
				delete_sql_list );
	}

	return 0;
}

