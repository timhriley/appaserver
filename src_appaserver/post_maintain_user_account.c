/* ------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_maintain_user_account.c	*/
/* ------------------------------------------------------------	*/
/*								*/
/* This script is attached to the submit button on 		*/
/* the maintain user account form.				*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "document.h"
#include "piece.h"
#include "list.h"
#include "dictionary2file.h"
#include "folder.h"
#include "operation_list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "process.h"
#include "insert_database.h"
#include "update_database.h"
#include "query.h"
#include "appaserver_parameter_file.h"
#include "appaserver.h"
#include "environ.h"
#include "dictionary.h"
#include "post2dictionary.h"
#include "form.h"
#include "session.h"
#include "dictionary_appaserver.h"

/* Constants */
/* --------- */
#define INSERT_UPDATE_KEY	"edit"
#define FOLDER_NAME		"appaserver_user"

/* Prototypes */
/* ---------- */
void execute_output_process(	char *application_name,
				char *login_name,
				char *role_name,
				char *session_key,
				char *message );

boolean post_state_update(	DICTIONARY_APPASERVER *dictionary_appaserver,
				char **message,
				char *application_name,
				char *session,
				char *folder_name,
				char *login_name,
				char *role_name,
				char *insert_update_key,
				char *appaserver_data_directory,
				char *appaserver_mount_point,
				pid_t dictionary_process_id );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session_key, *folder_name;
	char *role_name;
	char *message = {0};
	char *insert_update_key = INSERT_UPDATE_KEY;
	DICTIONARY *original_post_dictionary;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	SESSION *session;
	pid_t dictionary_process_id;
	char *database_string = {0};
	boolean changed_horizontal_menu_flag;
	DICTIONARY_APPASERVER *dictionary_appaserver;

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s login_name role application session ignored dictionary_process_id\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	role_name = argv[ 2 ];
	application_name = argv[ 3 ];
	session_key = argv[ 4 ];
	/* target_frame = argv[ 5 ]; */
	dictionary_process_id = atoi( argv[ 6 ] );

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	add_src_appaserver_to_path();
	environ_set_utc_offset( application_name );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	environ_prepend_dot_to_path();
	add_utility_to_path();
	add_relative_source_directory_to_path( application_name );
	environ_appaserver_home();

	folder_name = FOLDER_NAME;

	session = session_new_session();
	session->session = session_key;

	if ( session_remote_ip_address_changed(
		application_name,
		session->session ) )
	{
		session_message_ip_address_changed_exit(
				application_name,
				login_name );
	}

	if ( !session_load(
			&session->login_name,
			&session->last_access_date,
			&session->last_access_time,
			&session->http_user_agent,
			application_name,
			session->session ) )
	{
		session_access_failed_message_and_exit(
				application_name, session_key, login_name );
	}

	if ( strcmp( session->login_name, login_name ) != 0 )
	{
		session_access_failed_message_and_exit(
				application_name, session_key, login_name );
	}

	if ( !appaserver_user_exists_role(
					application_name,
					login_name,
					role_name ) )
	{
		session_access_failed_message_and_exit(
				application_name, session_key, login_name );
	}

	session_update_access_date_time( application_name, session->session );
	appaserver_library_purge_temporary_files( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	original_post_dictionary =
		post2dictionary(stdin,
			(char *)0 /* appaserver_data_directory */,
			(char *)0 /* session */ );

	if ( ! ( dictionary_appaserver =
			dictionary_appaserver_new(
				original_post_dictionary,
				(char *)0 /* application_name */,
				(LIST *)0 /* attribute_list */,
				(LIST *)0 /* operation_name_list */ ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: exiting early.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	changed_horizontal_menu_flag =
		post_state_update(	dictionary_appaserver,
					&message,
					application_name,
					session_key,
					folder_name,
					login_name,
					role_name,
					insert_update_key,
					appaserver_parameter_file->
						appaserver_data_directory,
					appaserver_parameter_file->
						appaserver_mount_point,
					dictionary_process_id );

	if ( changed_horizontal_menu_flag )
	{
		document_quick_output_body(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point );
		printf(
"<p>Since you changed the horizontal menu flag, please log in again.\n" );
		document_close();
	}
	else
	{
		execute_output_process(
				application_name,
				login_name,
				role_name,
				session_key,
				message );

	}
	exit( 0 );

} /* main() */

boolean post_state_update(	DICTIONARY_APPASERVER *dictionary_appaserver,
				char **message,
				char *application_name,
				char *session_key,
				char *folder_name,
				char *login_name,
				char *role_name,
				char *insert_update_key,
				char *appaserver_data_directory,
				char *appaserver_mount_point,
				pid_t dictionary_process_id )
{
	DICTIONARY *file_dictionary;
	UPDATE_DATABASE *update_database;
	FOLDER *folder;
	char *results_string;

	folder = folder_new_folder(
				application_name, 
				session_key,
				folder_name );

	folder->attribute_list =
		attribute_get_attribute_list(
			application_name,
			folder->folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			role_name );

	folder->attribute_name_list =
		attribute_get_attribute_name_list(
			folder->attribute_list );

	folder_load(	&folder->insert_rows_number,
			&folder->lookup_email_output,
			&folder->row_level_non_owner_forbid,
			&folder->row_level_non_owner_view_only,
			&folder->populate_drop_down_process,
			&folder->post_change_process,
			&folder->folder_form,
			&folder->notepad,
			&folder->html_help_file_anchor,
			&folder->post_change_javascript,
			&folder->lookup_before_drop_down,
			&folder->data_directory,
			&folder->index_directory,
			&folder->no_initial_capital,
			&folder->subschema_name,
			&folder->create_view_statement,
			application_name,
			session_key,
			folder->folder_name,
			0 /* dont override_row_restrictions */,
			role_name,
			(LIST *)0 /* mto1_related_folder_list */ );

	file_dictionary =
		dictionary2file_get_dictionary(
			dictionary_process_id,
			appaserver_data_directory,
			insert_update_key,
			folder->folder_name,
			(char *)0 /* optional_related_attribute_name */,
			ELEMENT_DICTIONARY_DELIMITER );

	if ( !file_dictionary )
	{
		document_quick_output_body(
				application_name,
				appaserver_mount_point );
		printf(
		"<p>Your form has expired. Please resubmit your query.\n" );
		document_close();
		exit( 0 );
	}

	dictionary_appaserver_parse_multi_attribute_keys(
		file_dictionary,
		(char *)0 /* prefix */ );

	dictionary_appaserver->row_dictionary =
		dictionary_appaserver_get_row_dictionary_multi_row(
			dictionary_appaserver->
				working_post_dictionary,
			folder->attribute_name_list,
			(LIST *)0 /* operation_name_list */ );

	update_database =
		update_database_new(
			application_name,
			session_key,
			login_name,
			role_name,
			folder_name,
			dictionary_appaserver->row_dictionary,
			file_dictionary );

	/* Set for the where clause generator. */
	/* ----------------------------------- */
	dictionary_set_pointer(
		update_database->post_dictionary,
		"login_name_1",
		login_name );

	dictionary_set_pointer(
		update_database->file_dictionary,
		"login_name_1",
		login_name );

	update_database->update_row_list =
		update_database_update_row_list(
			update_database->post_dictionary,
			update_database->file_dictionary,
			update_database->folder->folder_name,
			update_database->folder->attribute_list,
			update_database->folder->one2m_recursive_relation_list,
			update_database->folder->post_change_process );

	results_string =
		update_database_execute(
			update_database->application_name,
			update_database->session,
			update_database->update_row_list,
			update_database->post_dictionary,
			login_name,
			update_database->role_name,
			(LIST *)0 /* additional_update_attribute_name_list */,
			(LIST *)0 /* additional_update_data_list */,
			1 /* abort_if_first_update_failed */ );

	if ( results_string && *results_string )
	{
		document_quick_output_body(
			application_name,
			appaserver_mount_point );
		printf( "<p>Update failed: %s\n", results_string );
		document_close();
		exit( 0 );
	}
	else
	if ( list_length( update_database->update_row_list ) )
	{
		*message = "Update complete.";
	}

	return update_database_changed_attribute(
			"frameset_menu_horizontal_yn",
			update_database->update_row_list );

} /* post_state_update() */

void execute_output_process(	char *application_name,
				char *login_name,
				char *role_name,
				char *session_key,
				char *message )
{
	char sys_string[ 1024 ];

	if ( appaserver_frameset_menu_horizontal(
					application_name,
					login_name ) )
	{
		sprintf(sys_string,
"output_choose_role_folder_process_form '%s' '%s' '%s' '%s' '%s' %s 2>>%s",
			application_name,
			session_key,
			login_name,
			role_name,
			"" /* title */,
			"y" /* content_type_yn */,
			appaserver_error_get_filename(
				application_name ) );
		system( sys_string );
		fflush( stdout );
	}

	sprintf(sys_string,
"maintain_user_account %s %s %s %s \"%s\" 2>>%s",
		 application_name,
		 session_key,
		 login_name,
		 role_name,
		 (message) ? message : "",
		 appaserver_error_get_filename(
			application_name ) );

	system( sys_string );

} /* execute_output_process() */

