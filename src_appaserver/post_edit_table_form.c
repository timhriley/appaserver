/* ------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_edit_table_form.c	*/
/* ------------------------------------------------------	*/
/*								*/
/* This script is attached to the submit button on 		*/
/* the edit table form.						*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "appaserver_user.h"
#include "piece.h"
#include "list.h"
#include "dictionary2file.h"
#include "folder.h"
#include "document.h"
#include "operation_list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
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
#include "role.h"
#include "lookup_before_drop_down.h"
#include "dictionary_appaserver.h"
#include "pair_one2m.h"
#include "folder_menu.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
int post_state_update_folder(
			char **changed_folder_name_list_string,
			DICTIONARY_APPASERVER *dictionary_appaserver,
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			char *role_name,
			char *insert_update_key,
			char *appaserver_data_directory,
			char *appaserver_mount_point,
			pid_t dictionary_process_id,
			char *folder_name_comma_list_string,
			char *optional_related_attribute_name );

boolean get_insert_flag(DICTIONARY *non_prefixed_dictionary );

void set_insert_flag(	DICTIONARY *non_prefixed_dictionary );

void execute_output_process(	
			DICTIONARY_APPASERVER *dictionary_appaserver,
			char *application_name,
			char *login_name,
			char *session,
			char *folder_name,
			char *role_name,
			char *state,
			char *insert_update_key,
			int rows_inserted,
			char *target_frame,
			char *message,
			char *detail_base_folder_name,
			boolean insert_flag,
			char *vertical_new_button_base_folder_name,
			char *pair_one2m_folder_name,
			char *primary_data_list_string,
			char *folder_form,
			int cells_updated,
			char *changed_folder_name_list_string );

void post_state_update(		
			DICTIONARY_APPASERVER *dictionary_appaserver,
			char *application_name,
			char *session,
			char *folder_name_comma_list_string,
			char *login_name,
			char *role_name,
			char *insert_update_key,
			char *target_frame,
			char *state,
			char *appaserver_data_directory,
			char *appaserver_mount_point,
			char *detail_base_folder_name,
			boolean insert_flag,
			pid_t dictionary_process_id,
			boolean role_override_row_restrictions,
			char *optional_related_attribute_name,
			char *primary_data_list_string );

void post_state_insert(	DICTIONARY_APPASERVER *dictionary_appaserver,
			char *application_name,
			char *session,
			FOLDER *folder,
			char *login_name,
			char *role_name,
			char *insert_update_key,
			char *target_frame,
			char *state,
			char *detail_base_folder_name,
			boolean insert_flag,
			boolean role_override_row_restrictions,
			char *vertical_new_button_base_folder_name,
			char *primary_data_list_string );

void post_state_lookup(	DICTIONARY_APPASERVER *dictionary_appaserver,
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			char *role_name,
			char *insert_update_key,
			char *target_frame,
			char *state,
			char *detail_base_folder_name,
			boolean insert_flag,
			boolean role_override_row_restrictions,
			char *primary_data_list_string );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session, *folder_name;
	char *role_name, *state;
	char *insert_update_key;
	char detail_base_folder_name[ 128 ] = {0};
	char *target_frame;
	DICTIONARY_APPASERVER *dictionary_appaserver;
	DICTIONARY *original_post_dictionary;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	boolean insert_flag;
	pid_t dictionary_process_id;
	ROLE *role;
	char *optional_related_attribute_name;
	FOLDER *folder;
	char *vertical_new_button_base_folder_name;
	char *primary_data_list_string;
	OPERATION_LIST_STRUCTURE *operation_list_structure = {0};

	if ( argc < 10 )
	{
		fprintf( stderr,
"Usage: %s login_name application session folder role state insert_update_key target_frame dictionary_process_id [optional_related_attribute_name]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	application_name = argv[ 2 ];
	session = argv[ 3 ];
	folder_name = argv[ 4 ];
	role_name = argv[ 5 ];
	state = argv[ 6 ];
	insert_update_key = argv[ 7 ];
	target_frame = argv[ 8 ];
	dictionary_process_id = atoi( argv[ 9 ] );

	if ( argc == 11 )
		optional_related_attribute_name = argv[ 10 ];
	else
		optional_related_attribute_name = (char *)0;

	environ_set_environment(
		APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
		application_name );

	add_src_appaserver_to_path();
	environ_set_utc_offset( application_name );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	environ_prepend_dot_to_path();
	add_utility_to_path();
	add_local_bin_to_path();
	add_relative_source_directory_to_path( application_name );
	environ_appaserver_home();

	/* ------------------------------------------------------------ */
	/* Coming from the detail screen is communicated via the	*/
	/* insert_update_key. The syntax is:				*/
	/* detail!${detail_base_folder_name}				*/
	/* ------------------------------------------------------------ */
	if ( instr( "!", insert_update_key, 1 ) > -1 )
	{
		piece( detail_base_folder_name, '!', insert_update_key, 1 );
		insert_update_key[
			instr( "!", insert_update_key, 1 ) ] = '\0';
	}

	if ( session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
				application_name,
				login_name );
	}

	if ( !session_access_folder(
				application_name,
				session,
				folder_name,
				role_name,
				state ) )
	{
		if ( strcmp( state, "update" ) == 0
		&&  !( state = session_degrade_state(
				application_name,
				session,
				folder_name,
				role_name ) ) )
		{
			session_access_failed_message_and_exit(
				application_name, session, login_name );
		}
	}

	if ( !appaserver_user_exists_role(
					application_name,
					login_name,
					role_name ) )
	{
		session_access_failed_message_and_exit(
				application_name, session, login_name );
	}

	session_update_access_date_time( application_name, session );
	appaserver_library_purge_temporary_files( application_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	role = role_new_role(	application_name,
				role_name );

	folder = folder_new_folder(
				application_name, 
				session,
				folder_name );

	folder->mto1_isa_related_folder_list =
		folder_mto1_isa_related_folder_list(
			list_new() /* existing_related_folder_list */,
			application_name,
			folder->folder_name,
			role_name,
			0 /* recursive_level */ );

	folder->append_isa_attribute_list =
		folder_append_isa_attribute_list(
			application_name,
			folder->folder_name,
			folder->mto1_isa_related_folder_list,
			role_name );

	folder->append_isa_attribute_name_list =
		folder_append_isa_attribute_name_list(
			folder->append_isa_attribute_list );

	operation_list_structure =
		operation_list_structure_new(
			application_name,
			session,
			folder->folder_name,
			role_name,
			dont_omit_delete );

	original_post_dictionary =
		post2dictionary(stdin,
				appaserver_parameter_file->
					appaserver_data_directory,
				session );

	if ( ! ( dictionary_appaserver =
			dictionary_appaserver_new(
				original_post_dictionary,
				application_name,
				folder->append_isa_attribute_list,
				operation_list_get_operation_name_list(
					operation_list_structure->
						operation_list ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: exiting early.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Set the current time, if expected but not there. */
	/* ------------------------------------------------ */
	dictionary_set_indexed_date_time_to_current(
		dictionary_appaserver->row_dictionary,
		folder->append_isa_attribute_list );

	dictionary_remove_symbols_in_numbers(
		dictionary_appaserver->row_dictionary,
		folder->append_isa_attribute_list );

	/* Get from the non_prefixed_dictionary. */
	/* ------------------------------------- */
	insert_flag = get_insert_flag(
			dictionary_appaserver->
				non_prefixed_dictionary );

	vertical_new_button_base_folder_name =
		appaserver_library_get_vertical_new_button_folder_name(
			dictionary_appaserver->non_prefixed_dictionary,
			VERTICAL_NEW_PUSH_BUTTON_BASE_PREFIX );

	primary_data_list_string =
		dictionary_get_string(
			dictionary_appaserver->
				non_prefixed_dictionary,
			PRIMARY_DATA_LIST_KEY );

	/* Repopulate non_prefixed_dictionary */
	/* ---------------------------------- */
	dictionary_appaserver->non_prefixed_dictionary =
		dictionary_small_new();

	if ( insert_flag )
	{
		set_insert_flag(
			dictionary_appaserver->
				non_prefixed_dictionary );
	}

	if ( timlib_strlen( primary_data_list_string ) )
	{
		dictionary_set_string(	dictionary_appaserver->
						non_prefixed_dictionary,
					PRIMARY_DATA_LIST_KEY,
					primary_data_list_string );
	}

	if ( strcmp( target_frame, PROMPT_FRAME ) == 0 )
	{
		dictionary_set_pointer(
			dictionary_appaserver->non_prefixed_dictionary,
			CONTENT_TYPE_YN,
			"n" );
	}

	if ( strcmp( state, "insert" ) == 0 )
	{
		post_state_insert(
				dictionary_appaserver,
				application_name,
				session,
				folder,
				login_name,
				role_name,
				insert_update_key,
				target_frame,
				state,
				detail_base_folder_name,
				insert_flag,
				role_get_override_row_restrictions(
					role->override_row_restrictions_yn ),
				vertical_new_button_base_folder_name,
				primary_data_list_string );
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_state_update(
				dictionary_appaserver,
				application_name,
				session,
				folder_name,
				login_name,
				role_name,
				insert_update_key,
				target_frame,
				state,
				appaserver_parameter_file->
					appaserver_data_directory,
				appaserver_parameter_file->
					appaserver_mount_point,
				detail_base_folder_name,
				insert_flag,
				dictionary_process_id,
				role_get_override_row_restrictions(
					role->override_row_restrictions_yn ),
				optional_related_attribute_name,
				primary_data_list_string );
	}
	else
	{
		post_state_lookup(
				dictionary_appaserver,
				application_name,
				session,
				folder_name,
				login_name,
				role_name,
				insert_update_key,
				target_frame,
				state,
				detail_base_folder_name,
				insert_flag,
				role_get_override_row_restrictions(
					role->override_row_restrictions_yn ),
				primary_data_list_string );
	}

	exit( 0 );

}

void post_state_insert(
			DICTIONARY_APPASERVER *dictionary_appaserver,
			char *application_name,
			char *session,
			FOLDER *folder,
			char *login_name,
			char *role_name,
			char *insert_update_key,
			char *target_frame,
			char *state,
			char *detail_base_folder_name,
			int insert_flag,
			boolean role_override_row_restrictions,
			char *vertical_new_button_base_folder_name,
			char *primary_data_list_string )
{
	LIST *mto1_related_folder_list;
	int rows_inserted = 0;
	INSERT_DATABASE *insert_database;
	char *message = "";
	LIST *isa_related_folder_list = {0};
	char *pair_one2m_folder_name;
	LIST *insert_required_attribute_name_list = {0};
	char rows_inserted_string[ 128 ];
	char *folder_form;
	DICTIONARY *ignore_dictionary;

	/* If coming from the detail, then don't ignore any attributes. */
	/* ------------------------------------------------------------ */
	if ( *detail_base_folder_name )
	{
		ignore_dictionary = (DICTIONARY *)0;
	}
	else
	{
		ignore_dictionary = dictionary_appaserver->ignore_dictionary;
	}

	folder->primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			folder->append_isa_attribute_list );

	folder->attribute_name_list =
		folder_get_attribute_name_list(
			folder->append_isa_attribute_list );

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
			session,
			folder->folder_name,
			role_override_row_restrictions,
			role_name,
			(LIST *)0 /* mto1_related_folder_list */ );

	folder_form = folder->folder_form;

	/* ---------------------------------------------------------- */
	/* The query_dictionary contains the row zero attributes that */
	/* need to be inserted into each row.			      */
	/* ---------------------------------------------------------- */
	dictionary_append_row_zero(
		dictionary_appaserver->row_dictionary,
		dictionary_appaserver->query_dictionary );

	dictionary_appaserver_set_primary_data_list_string(
		dictionary_appaserver->row_dictionary,
		primary_data_list_string,
		folder->primary_attribute_name_list,
		FOLDER_DATA_DELIMITER );

	if ( folder->row_level_non_owner_forbid
	||   attribute_exists_omit_insert_login_name(
			folder->append_isa_attribute_list ) )
	{
		dictionary_add_login_name_if_necessary(
				dictionary_appaserver->
					row_dictionary,
				folder->attribute_name_list,
				login_name );
	}

	insert_required_attribute_name_list =
		attribute_get_insert_required_attribute_name_list(
			folder->append_isa_attribute_list );

	mto1_related_folder_list =
		related_folder_get_mto1_related_folder_list(
			list_new_list(),
			application_name,
			session,
			folder->folder_name,
			role_name,
			0 /* isa_flag */,
			related_folder_no_recursive,
			role_override_row_restrictions,
			(LIST *)0 /* root_primary_attribute_name_list */,
			0 /* recursive_level */ );

	insert_database =
		insert_database_new(
			application_name,
			session,
			folder->folder_name,
			folder->primary_attribute_name_list,
			folder->attribute_name_list,
			dictionary_appaserver->row_dictionary,
			ignore_dictionary,
			folder->append_isa_attribute_list );

	insert_database->dont_remove_tmp_file =
		INSERT_DATABASE_DONT_REMOVE_TMP_FILE;

	rows_inserted =
	insert_database_execute(
		&message,
		insert_database->application_name,
		insert_database->session,
		insert_database->folder_name,
		role_name,
		insert_database->primary_attribute_name_list,
		insert_required_attribute_name_list,
		attribute_folder_name_list(
			insert_database->attribute_list,
			insert_database->folder_name ),
		insert_database->row_dictionary,
		insert_database->ignore_attribute_name_list,
		insert_database->insert_row_zero_only,
		insert_database->dont_remove_tmp_file,
		folder->post_change_process,
		login_name,
		mto1_related_folder_list,
		related_folder_get_common_non_primary_attribute_name_list(
			application_name,
			folder->folder_name,
			mto1_related_folder_list ),
		insert_database->attribute_list,
		attribute_exists_reference_number(
			folder->append_isa_attribute_list ),
		appaserver_parameter_file_get_data_directory()
			/* tmp_file_directory */ );

	if ( rows_inserted )
	{
		folder_menu_refresh_row_count(
			application_name,
			folder->folder_name,
			session,
			appaserver_parameter_file_get_data_directory(),
			role_name );

		sprintf( rows_inserted_string,
			 "%d",
			 rows_inserted );

		dictionary_set_pointer(
			dictionary_appaserver->
				non_prefixed_dictionary,
			ROWS_INSERTED_COUNT_KEY,
			strdup( rows_inserted_string ) );
	}

	isa_related_folder_list =
		related_folder_get_isa_related_folder_list(
				application_name,
				session,
				folder->folder_name,
				role_name,
				role_override_row_restrictions,
				related_folder_recursive_all );

	if ( isa_related_folder_list  )
	{
		RELATED_FOLDER *isa_related_folder;
		LIST *isa_attribute_name_list;
		LIST *isa_primary_attribute_name_list;

		list_rewind( isa_related_folder_list );
		do {
			isa_related_folder =
				list_get_pointer(
					isa_related_folder_list );

			isa_related_folder->folder->attribute_list =
				attribute_get_attribute_list(
				application_name,
				isa_related_folder->folder->folder_name,
				(char *)0 /* attribute_name */,
				(LIST *)0 /* mto1_isa_related_... */,
				role_name );

			isa_attribute_name_list =
				folder_get_attribute_name_list(
					isa_related_folder->
					folder->
					attribute_list );

			isa_primary_attribute_name_list =
				folder_get_primary_attribute_name_list(
					isa_related_folder->
					folder->
					attribute_list );

			dictionary_new_index_key_list_for_data_list(
					dictionary_appaserver->
						row_dictionary,
					folder->primary_attribute_name_list,
					isa_primary_attribute_name_list,
					0 );

			insert_database =
			insert_database_new(
				application_name,
				session,
				isa_related_folder->
					folder->folder_name,
				isa_primary_attribute_name_list,
				isa_attribute_name_list,
				dictionary_appaserver->row_dictionary,
				ignore_dictionary,
				isa_related_folder->
					folder->
					attribute_list );

			insert_database->dont_remove_tmp_file =
				INSERT_DATABASE_DONT_REMOVE_TMP_FILE;

			insert_database_execute(
				&message,
				insert_database->application_name,
				insert_database->session,
				insert_database->folder_name,
				role_name,
				insert_database->
					primary_attribute_name_list,
				insert_required_attribute_name_list,
				insert_database->attribute_name_list,
				insert_database->row_dictionary,
				insert_database->ignore_attribute_name_list,
				insert_database->insert_row_zero_only,
				insert_database->dont_remove_tmp_file,
				isa_related_folder->folder->
					post_change_process,
				login_name,
				(LIST *)0 /* mto1_related_folder_... */,
				(LIST *)0 /* common_non_primar_... */,
				(LIST *)0 /* attribute_list */,
				0 /* not exists_reference_number */,
				appaserver_parameter_file_get_data_directory()
					/* tmp_file_directory */ );

		} while( list_next( isa_related_folder_list ) );
	}

	pair_one2m_folder_name =
		dictionary_get_pointer(
			dictionary_appaserver->pair_1tom_dictionary,
			PAIR_ONE2M_FOLDER_NAME );

	execute_output_process(	dictionary_appaserver,
				application_name,
				login_name,
				session,
				folder->folder_name,
				role_name,
				state,
				insert_update_key,
				rows_inserted,
				target_frame,
				message,
				detail_base_folder_name,
				insert_flag,
				vertical_new_button_base_folder_name,
				pair_one2m_folder_name,
				primary_data_list_string,
				folder_form,
				0 /* cells_updated */,
				(char *)0
				/* changed_folder_name_list_string */ );

}

void post_state_update(
			DICTIONARY_APPASERVER *dictionary_appaserver,
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			char *role_name,
			char *insert_update_key,
			char *target_frame,
			char *state,
			char *appaserver_data_directory,
			char *appaserver_mount_point,
			char *detail_base_folder_name,
			int insert_flag,
			pid_t dictionary_process_id,
			boolean role_override_row_restrictions,
			char *optional_related_attribute_name,
			char *primary_data_list_string )
{
	FOLDER *folder;
	OPERATION_LIST_STRUCTURE *operation_list_structure = {0};
	char *changed_folder_name_list_string = (char *)0;
	int cells_updated;

	dictionary_appaserver->send_dictionary =
		dictionary_appaserver_get_send_dictionary(
			dictionary_appaserver->sort_dictionary,
			dictionary_appaserver->query_dictionary,
			dictionary_appaserver->preprompt_dictionary,
			dictionary_appaserver->
				lookup_before_drop_down_dictionary,
			dictionary_appaserver->ignore_dictionary,
			dictionary_appaserver->pair_1tom_dictionary,
			dictionary_appaserver->non_prefixed_dictionary );

	folder = folder_new_folder(
			application_name, 
			session,
			folder_name );

	folder->mto1_isa_related_folder_list =
		folder_mto1_isa_related_folder_list(
			list_new() /* existing_related_folder_list */,
			application_name,
			folder->folder_name,
			role_name,
			0 /* recursive_level */ );

	folder->append_isa_attribute_list =
		folder_append_isa_attribute_list(
			application_name,
			folder->folder_name,
			folder->mto1_isa_related_folder_list,
			role_name );

	folder->primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			folder->append_isa_attribute_list );

	folder->attribute_name_list =
		folder_get_attribute_name_list(
			folder->append_isa_attribute_list );

	folder_load(		&folder->insert_rows_number,
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
				session,
				folder->folder_name,
				role_override_row_restrictions,
				role_name,
				(LIST *)0 /* mto1_related_folder_list */ );

	operation_list_structure =
		operation_list_structure_new(
			application_name,
			session,
			folder_name,
			role_name,
			dont_omit_delete );

	cells_updated =
		post_state_update_folder(
				&changed_folder_name_list_string,
				dictionary_appaserver,
				application_name,
				session,
				folder_name,
				login_name,
				role_name,
				insert_update_key,
				appaserver_data_directory,
				appaserver_mount_point,
				dictionary_process_id,
				folder_name,
				optional_related_attribute_name );

	operation_list_structure->performed_any_output =
		operation_list_perform_operations(
			dictionary_appaserver->send_dictionary,
			dictionary_appaserver->row_dictionary,
			operation_list_structure->operation_list,
			application_name,
			session,
			login_name,
			folder_name,
			role_name,
			folder->primary_attribute_name_list,
			(folder->row_level_non_owner_forbid ||
			 folder->row_level_non_owner_view_only),
			target_frame );

	if ( operation_list_structure->performed_any_output )
	{
		return;
	}

	execute_output_process(
			dictionary_appaserver,
			application_name,
			login_name,
			session,
			folder_name,
			role_name,
			state,
			insert_update_key,
			0 /* rows_inserted */,
			target_frame,
			(char *)0 /* message */,
			detail_base_folder_name,
			insert_flag,
			(char *)0 /* vertical_new_button_base_folder_name */,
			(char *)0 /* pair_one2m_folder_name */,
			primary_data_list_string,
			(char *)0 /* folder_form */,
			cells_updated,
			changed_folder_name_list_string );
}

int post_state_update_folder(
			char **changed_folder_name_list_string,
			DICTIONARY_APPASERVER *dictionary_appaserver,
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			char *role_name,
			char *insert_update_key,
			char *appaserver_data_directory,
			char *appaserver_mount_point,
			pid_t dictionary_process_id,
			char *folder_name_comma_list_string,
			char *optional_related_attribute_name )
{
	DICTIONARY *file_dictionary;
	UPDATE_DATABASE *update_database;
	char *results_string;
	int cells_updated;

	file_dictionary =
		dictionary2file_get_dictionary(
			dictionary_process_id,
			appaserver_data_directory,
			insert_update_key,
			folder_name_comma_list_string,
			optional_related_attribute_name,
			ELEMENT_DICTIONARY_DELIMITER );

	if ( !file_dictionary )
	{
		document_quick_output_body(
				application_name,
				appaserver_mount_point );
		printf(
		"<p>Your form has expired. Please resubmit the lookup.\n" );
		document_close();
		exit( 0 );
	}

	dictionary_appaserver_parse_multi_attribute_keys(
		file_dictionary,
		(char *)0 /* prefix */ );

	update_database =
		update_database_new(
			application_name,
			session,
			login_name,
			role_name,
			folder_name,
			dictionary_appaserver->row_dictionary
				/* post_dictionary */,
			file_dictionary );

	if ( !update_database ) return 0;

	update_database->update_row_list =
		update_database_update_row_list(
			update_database->post_dictionary,
			update_database->file_dictionary,
			update_database->
				folder->
				folder_name,
			update_database->
				folder->
				attribute_list,
			update_database->
				folder->
				one2m_recursive_relation_list,
			update_database->
				folder->
				mto1_isa_recursive_relation_list,
			update_database->
				folder->
				post_change_process );

	if ( !list_length( update_database->update_row_list ) )
		return 0;
/*
{
char msg[ 65536 ];
sprintf( msg, "%s/%s()/%d: got update_row_list = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
update_database_update_row_list_display( update_database->update_row_list ) );
m2( application_name, msg );
}
*/
	cells_updated =
		update_database_cells_updated(
			update_database->update_row_list );

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

	*changed_folder_name_list_string =
		list_display_delimited_plus_space(
			update_database_changed_folder_name_list(
				update_database->update_row_list ),
			',' );

	return cells_updated;
}

void post_state_lookup(
				DICTIONARY_APPASERVER *dictionary_appaserver,
				char *application_name,
				char *session,
				char *folder_name,
				char *login_name,
				char *role_name,
				char *insert_update_key,
				char *target_frame,
				char *state,
				char *detail_base_folder_name,
				int insert_flag,
				boolean role_override_row_restrictions,
				char *primary_data_list_string )
{
	OPERATION_LIST_STRUCTURE *operation_list_structure;
	FOLDER *folder;

	dictionary_appaserver->send_dictionary =
		dictionary_appaserver_get_send_dictionary(
			dictionary_appaserver->sort_dictionary,
			dictionary_appaserver->query_dictionary,
			dictionary_appaserver->preprompt_dictionary,
			dictionary_appaserver->
				lookup_before_drop_down_dictionary,
			dictionary_appaserver->ignore_dictionary,
			dictionary_appaserver->pair_1tom_dictionary,
			dictionary_appaserver->non_prefixed_dictionary );

	operation_list_structure =
		operation_list_structure_new(
			application_name,
			session,
			folder_name,
			role_name,
			dont_omit_delete );

	folder = folder_new_folder(
				application_name, 
				session,
				folder_name );

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
			session,
			folder->folder_name,
			role_override_row_restrictions,
			role_name,
			(LIST *)0 /* mto1_related_folder_list */ );

	if ( folder->row_level_non_owner_forbid )
	{
		dictionary_set_pointer(	dictionary_appaserver->
						row_dictionary,
					"login_name_0",
					login_name );
	}

	folder->mto1_isa_related_folder_list =
		related_folder_get_mto1_related_folder_list(
			list_new(),
			application_name,
			session,
			folder_name,
			role_name,
			1 /* isa_flag */,
			related_folder_recursive_all,
			role_override_row_restrictions,
			(LIST *)0 /* root_primary_attribute_name_list */,
			0 /* recursive_level */ );

	folder->attribute_list =
		attribute_get_attribute_list(
			application_name,
			folder_name,
			(char *)0 /* attribute_name */,
			folder->mto1_isa_related_folder_list,
			role_name );

	folder->primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			folder->attribute_list );

	folder->attribute_name_list =
		folder_get_attribute_name_list(
			folder->attribute_list );

	folder->primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			folder->attribute_list );

	operation_list_structure->performed_any_output =
		operation_list_perform_operations(
			dictionary_appaserver->send_dictionary,
			dictionary_appaserver->row_dictionary,
			operation_list_structure->operation_list,
			application_name,
			session,
			login_name,
			folder_name,
			role_name,
			folder->primary_attribute_name_list,
			(folder->row_level_non_owner_forbid ||
			 folder->row_level_non_owner_view_only),
			target_frame );

	if ( operation_list_structure->performed_any_output ) return;

	execute_output_process(
			dictionary_appaserver,
			application_name,
			login_name,
			session,
			folder_name,
			role_name,
			state,
			insert_update_key,
			0 /* rows_inserted */,
			target_frame,
			(char *)0 /* message */,
			detail_base_folder_name,
			insert_flag,
			(char *)0 /* vertical_new_button_base_folder_name */,
			(char *)0 /* pair_one2m_folder_name */,
			primary_data_list_string,
			(char *)0 /* folder_form */,
			0 /* cells_updated */,
			(char *)0
			/* changed_folder_name_list_string */ );

}

void execute_output_process(	
				DICTIONARY_APPASERVER *dictionary_appaserver,
				char *application_name,
				char *login_name,
				char *session,
				char *folder_name,
				char *role_name,
				char *state,
				char *insert_update_key,
				int rows_inserted,
				char *target_frame,
				char *message,
				char *detail_base_folder_name,
				int insert_flag,
				char *vertical_new_button_base_folder_name,
				char *pair_one2m_folder_name,
				char *primary_data_list_string,
				char *folder_form,
				int cells_updated,
				char *changed_folder_name_list_string )
{
	char sys_string[ 65536 ];

	if ( !dictionary_appaserver )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: no dictionary appaserver.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( cells_updated )
	{
		char cells_updated_string[ 16 ];

		sprintf( cells_updated_string,
			 "%d",
			 cells_updated );

		if ( !dictionary_appaserver->non_prefixed_dictionary )
		{
			dictionary_appaserver->non_prefixed_dictionary =
				dictionary_small_new();
		}

		dictionary_set_pointer(
			dictionary_appaserver->non_prefixed_dictionary,
			COLUMNS_UPDATED_KEY,
			strdup( cells_updated_string ) );

		if ( changed_folder_name_list_string )
		{
			dictionary_set_pointer(
				dictionary_appaserver->
					non_prefixed_dictionary,
				COLUMNS_UPDATED_CHANGED_FOLDER_KEY,
				changed_folder_name_list_string );
		}
	}

	if ( strcmp( insert_update_key, "detail" ) == 0 && insert_flag )
	{
		LIST *primary_data_list;
		DICTIONARY *base_primary_data_dictionary;
		FOLDER *base_folder;

		base_folder = folder_new_folder(
					application_name, 
					session,
					detail_base_folder_name );

		base_folder->attribute_list =
			attribute_get_attribute_list(
				application_name,
				base_folder->folder_name,
				(char *)0 /* attribute_name */,
				base_folder->
					mto1_isa_related_folder_list,
				role_name );

		base_folder->primary_attribute_name_list =
			folder_get_primary_attribute_name_list(
				base_folder->attribute_list );

		primary_data_list =
			list_string2list(
				primary_data_list_string,
				FOLDER_DATA_DELIMITER );

		base_primary_data_dictionary =
			dictionary_merge_lists2dictionary(
				base_folder->primary_attribute_name_list,
				primary_data_list );

		dictionary_append_dictionary(
			dictionary_appaserver->
				query_dictionary,
			base_primary_data_dictionary );

		sprintf( sys_string,
"echo \"%s\" 								|"
"output_insert_table_form %s %s %s %s %s %s \"detail!%s\" %s 2>>%s	 ",
			dictionary_appaserver_escaped_send_dictionary_string(
				dictionary_appaserver,
				1 /* with_non_prefixed_dictionary */ ),
	 	 	login_name,
		 	application_name,
		 	session,
		 	folder_name,
		 	role_name,
		 	"insert" /* state */,
		 	detail_base_folder_name,
		 	target_frame,
		 	appaserver_error_get_filename( application_name ) );
	}
	else
	if ( strcmp( insert_update_key, "detail" ) == 0 )
	{
		if ( strcmp( target_frame, PROMPT_FRAME ) == 0
		&&   appaserver_frameset_menu_horizontal(
						application_name,
						login_name ) )
		{
			char sys_string[ 1024 ];

			sprintf(sys_string,
"output_choose_role_folder_process_form %s %s %s %s '%s' %s 2>>%s",
				application_name,
				session,
				login_name,
				role_name,
				"" /* title */,
				"y" /* content_type_yn */,
				appaserver_error_get_filename(
					application_name ) );

			if ( system( sys_string ) ) {};
			fflush( stdout );

			dictionary_set_pointer(
				dictionary_appaserver->non_prefixed_dictionary,
				CONTENT_TYPE_YN,
				"n" );
		}

		sprintf(sys_string,
			"detail %s %s %s %s \"%s\" \"%s\" \"%s\" \"%s\" 2>>%s",
		 	application_name,
		 	session,
		 	login_name,
		 	detail_base_folder_name,
		 	role_name,
			target_frame,
		 	primary_data_list_string,
			dictionary_appaserver_escaped_send_dictionary_string(
				dictionary_appaserver,
				1 /* with_non_prefixed_dictionary */ ),
		 	appaserver_error_get_filename( application_name ) );
	}
	else
	if ( strcmp( state, "insert" ) == 0 && pair_one2m_folder_name )
	{
		sprintf( sys_string, 
"echo \"%s\" 								|"
"output_insert_table_form \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" 2>>%s	 ",
			 dictionary_appaserver_escaped_send_dictionary_string(
				dictionary_appaserver,
				1 /* with_non_prefixed_dictionary */ ),
	 	 	 login_name,
		 	 application_name,
		 	 session,
		 	 pair_one2m_folder_name,
		 	 role_name,
		 	 state,
		 	 insert_update_key,
		 	 target_frame,
			 (message) ? message : "",
		 	 appaserver_error_get_filename( application_name ) );
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		char content_type_yn;

		if ( strcmp( target_frame, PROMPT_FRAME ) == 0
		&&   appaserver_frameset_menu_horizontal(
						application_name,
						login_name ) )
		{
			char sys_string[ 1024 ];

			sprintf(sys_string,
"output_choose_role_folder_process_form %s %s %s %s '%s' %s 2>>%s",
				application_name,
				session,
				login_name,
				role_name,
				"" /* title */,
				"y" /* content_type_yn */,
				appaserver_error_get_filename(
					application_name ) );

			if ( system( sys_string ) ) {};
			fflush( stdout );
			content_type_yn = 'n';
		}
		else
		{
			content_type_yn = 'y';
		}

		if ( timlib_strcmp( folder_form, "table" ) == 0
		&&   !vertical_new_button_base_folder_name )
		{
			content_type_yn = 'n';

			sprintf( sys_string, 
"echo \"%s\" 								|"
"output_edit_table_form \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" '' \"%s\" \"%s\" \"%c\" 2>>%s	 ",
			 dictionary_appaserver_escaped_send_dictionary_string(
				dictionary_appaserver,
				1 /* with_non_prefixed_dictionary */ ),
	 	 	 login_name,
		 	 application_name,
		 	 session,
		 	 folder_name,
		 	 role_name,
		 	 insert_update_key,
		 	 target_frame,
			 content_type_yn,
		 	 appaserver_error_get_filename( application_name ) );
		}
		else
		{
			sprintf( sys_string,
	 "output_results %s %s %s %s %s %d \"%s\" \"%s\" %c \"%s\" 2>>%s",
			application_name,
		 	folder_name,
			session,
			login_name,
			role_name,
			rows_inserted,
			(message) ? message : "",
			(vertical_new_button_base_folder_name)		?
				vertical_new_button_base_folder_name	:
				"",
			content_type_yn,
			dictionary_appaserver_escaped_send_dictionary_string(
				dictionary_appaserver,
				1 /* with_non_prefixed_dictionary */ ),
			appaserver_error_get_filename( application_name ) );
		}
	}
	else
	/* ------------------------ */
	/* Must be update or lookup */
	/* ------------------------ */
	{
		sprintf( sys_string,
"echo \"%s\" 								|"
"output_edit_table_form \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" '' \"%s\" \"%s\" 2>>%s	 ",
			 dictionary_appaserver_escaped_send_dictionary_string(
				dictionary_appaserver,
				1 /* with_non_prefixed_dictionary */ ),
	 	 	 login_name,
		 	 application_name,
		 	 session,
		 	 folder_name,
		 	 role_name,
		 	 insert_update_key,
		 	 target_frame,
		 	 appaserver_error_get_filename( application_name ) );
	}

	if ( system( sys_string ) ) {};

}

boolean get_insert_flag( DICTIONARY *non_prefixed_dictionary )
{
	char search_key[ 128 ];
	char *results_string;

	sprintf( search_key, "%s_0", INSERT_PUSH_BUTTON_NAME );
	results_string =
		dictionary_get_string( 	non_prefixed_dictionary,
					search_key );
	if ( !results_string )
		return 0;
	else
		return ( strcmp( results_string, "yes" ) == 0 );

}

void set_insert_flag( DICTIONARY *non_prefixed_dictionary )
{
	char search_key[ 128 ];

	sprintf( search_key, "%s_0", INSERT_PUSH_BUTTON_NAME );

	dictionary_set_string( 	non_prefixed_dictionary,
				strdup( search_key ),
				"yes" );
}

