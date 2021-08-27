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
#include "dictionary_separate.h"

/* Constants */
/* --------- */
#define INSERT_UPDATE_KEY	"edit"

/* Prototypes */
/* ---------- */
void execute_output_process(
			char *application_name,
			char *login_name,
			char *role_name,
			char *session_key,
			char *message );

boolean post_state_update(
			DICTIONARY_SEPARATE *dictionary_separate,
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
	char *application_name
	char *login_name;
	char *session_key;
	char *folder_name;
	char *role_name;
	POST_DICTIONARY *post_dictionary;
	char *message = {0};
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	SESSION *session;
	boolean changed_horizontal_menu_flag;
	DICTIONARY_SEPARATE *dictionary_separate;

	if ( argc != 6 )
	{
		fprintf(stderr, 
			"Usage: %s login_name role application session\n",
			argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	role_name = argv[ 2 ];
	application_name = argv[ 3 ];
	session_key = argv[ 4 ];

	session =
		session_folder_integrity_exit(
			argc,
			argv,
			application_name,
			login_name,
			session_key,
			APPASERVER_USER_TABLE,
			role_name,
			"update" /* state */ );

	appaserver_parameter_file = appaserver_parameter_file_new();

	post_dictionary =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		post_dictionary_stdin_new(
			/* ---------------------------------- */
			/* Used when expecting a spooled file */
			/* ---------------------------------- */
			(char *)0 /* appaserver_data_directory */,
			(char *)0 /* session_key );

	dictionary_separate =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		dictionary_separate_stream_new(
			post_dictionary,
			application_name,
			login_name,

	changed_horizontal_menu_flag =
		post_state_update(	dictionary_separate,
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

	return 0;
}

boolean post_state_update(
			DICTIONARY_SEPARATE *dictionary_separate,
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
	char system_string[ 1024 ];

	folder =
		folder_new_folder(
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

	dictionary_separate_parse_multi_attribute_keys(
		file_dictionary,
		(char *)0 /* prefix */ );

	dictionary_separate->row_dictionary =
		dictionary_separate_multi_row_dictionary(
			dictionary_separate->
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
			dictionary_separate->row_dictionary,
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
			(LIST *)0 /* one2m_recursive_relation_list */,
			(LIST *)0 /* mto1_isa_recursive_relation_list */,
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

	sprintf(system_string,
		"appaserver_user_trigger \"%s\" update",
		login_name );

	if ( system( system_string ) ){};

	return update_database_attribute_exists(
			"frameset_menu_horizontal_yn",
			update_database->update_row_list );
}

void execute_output_process(
			char *application_name,
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
		if ( system( sys_string ) ){};
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

	if ( system( sys_string ) ){};
}
