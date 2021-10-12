/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/delete_folder_row.c			*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "timlib.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "document.h"
#include "delete.h"

/* Prototypes */
/* ---------- */
void delete_folder_row(
			char *application_name,
			char *session_key,
			char *login_name,
			char *folder_name,
			char *role_name,
			LIST *primary_data_list,
			boolean dont_delete_mto1_isa );

int main( int argc, char **argv )
{
	char *application_name;
	char *session_key;
	char *login_name;
	char *folder_name;
	char *role_name;
	char *primary_data_list_string;
	boolean dont_delete_mto1_isa;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s session login_name folder role_name primary_data_list dont_delete_mto1_isa_yn\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	session_key = argv[ 1 ];
	login_name = argv[ 2 ];
	folder_name = argv[ 3 ];
	role_name = argv[ 4 ];
	primary_data_list_string = argv[ 5 ];
	dont_delete_mto1_isa = (*argv[ 6 ] == 'y');

	delete_folder_row(
		application_name,
		session_key,
		login_name,
		folder_name,
		role_name,
		list_string_list( primary_data_list_string, '^' ),
		dont_delete_mto1_isa );

	return 0;
}

void delete_folder_row(
			char *application_name,
			char *session_key,
			char *login_name,
			char *folder_name,
			char *role_name,
			LIST *primary_data_list,
			boolean dont_delete_mto1_isa )
{
	DELETE *delete;

	delete =
		delete_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			primary_data_list,
			dont_delete_mto1_isa );

	if ( !delete )
	{
		fprintf( stderr,
			"ERROR in %s/%s()/%d: delete_new(%s) returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			role_name );
		exit( 1 );
	}

	delete_database->delete_folder_list =
		delete_database_get_delete_folder_list(
			application_name,
			delete_database->folder->folder_name,
			delete_database->folder->post_change_process,
			delete_database->folder->primary_key_list,
			delete_database->folder->row_level_non_owner_forbid,
			delete_database->folder->row_level_non_owner_view_only,
			delete_database->
				folder->
				mto1_isa_related_folder_list,
			delete_database->
				folder->
				one2m_recursive_related_folder_list,
			delete_database->dont_delete_mto1_isa,
			delete_database->primary_attribute_data_list,
			delete_database->login_name );

	if ( !list_length( delete_database->delete_folder_list ) )
	{
		fprintf( stderr,
"Error in %s/%s()/%d: for folder = (%s), have empty delete_folder_list for primary_attribute_data_list = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 delete_database->folder->folder_name,
			 list_display(
				delete_database->
					primary_attribute_data_list ) );
		exit( 1 );
	}

/*
fprintf( stderr, "%s/%s()/%d: Delete database = %s\n",
__FILE__,
__FUNCTION__,
__LINE__,
delete_database_display( delete_database ) );
*/

	delete_database_execute_delete_folder_list(
		delete_database->application_name,
		delete_database->delete_folder_list,
		sql_executable );

	delete_database_refresh_row_count(
		delete_database->application_name,
		delete_database->delete_folder_list,
		session,
		role_name );

}

