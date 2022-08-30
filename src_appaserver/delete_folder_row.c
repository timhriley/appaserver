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
#include "delete_database.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "document.h"

/* Prototypes */
/* ---------- */
void delete_where_data_carrot_list_string(
				char *application_name,
				char *login_name,
				char *session,
				char *role_name,
				char *folder_name,
				char *where_data_carrot_list_string,
				char *sql_executable,
				boolean dont_delete_mto1_isa );

int main( int argc, char **argv )
{
	char *application_name;
	char *login_name;
	char *session;
	char *role_name;
	char *folder_name;
	char *primary_attribute_data_list_string;
	char primary_attribute_data_list_string_buffer[ 1024 ];
	char *sql_executable;
	boolean dont_delete_mto1_isa = 0;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 6 )
	{
		fprintf( stderr,
"Usage: %s session login_name folder role_name primary_attribute_data_list [dont_delete_mto1_isa_yn]\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	session = argv[ 1 ];
	login_name = argv[ 2 ];
	folder_name = argv[ 3 ];
	role_name = argv[ 4 ];
	primary_attribute_data_list_string = argv[ 5 ];
	if ( argc == 7 ) dont_delete_mto1_isa = (*argv[ 6 ] == 'y');

	sql_executable = "sql.e";
	/* sql_executable = "html_paragraph_wrapper"; */

/*
	if ( timlib_strncmp( sql_executable, "html_paragraph_wrapper" ) == 0 )
	{
		document_output_content_type();
	}
*/

	if ( strcmp( primary_attribute_data_list_string, "stdin" ) == 0 )
	{
		while( get_line(
				primary_attribute_data_list_string_buffer,
				stdin ) )
		{
			delete_where_data_carrot_list_string(
				application_name,
				login_name,
				session,
				role_name,
				folder_name,
				primary_attribute_data_list_string_buffer,
				sql_executable,
				dont_delete_mto1_isa );
		}
	}
	else
	{
		delete_where_data_carrot_list_string(
				application_name,
				login_name,
				session,
				role_name,
				folder_name,
				primary_attribute_data_list_string,
				sql_executable,
				dont_delete_mto1_isa );
	}

	return 0;

} /* main() */

void delete_where_data_carrot_list_string(
				char *application_name,
				char *login_name,
				char *session,
				char *role_name,
				char *folder_name,
				char *primary_attribute_data_list_string,
				char *sql_executable,
				boolean dont_delete_mto1_isa )
{
	DELETE_DATABASE *delete_database;

	delete_database =
		delete_database_new(
			application_name,
			login_name,
			role_name,
			dont_delete_mto1_isa,
			folder_name,
			list_string2list( primary_attribute_data_list_string,
					  '^' /* delimiter */ ),
			sql_executable );

	if ( !delete_database )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty delete_database.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !delete_database->folder )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	delete_database->delete_folder_list =
		delete_database_get_delete_folder_list(
			application_name,
			delete_database->folder->folder_name,
			delete_database->folder->post_change_process,
			delete_database->folder->primary_attribute_name_list,
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

} /* delete_where_data_carrot_list_string() */

