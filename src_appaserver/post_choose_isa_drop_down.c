/* -----------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_choose_isa_drop_down.c	*/
/* -----------------------------------------------------------	*/
/*								*/
/* This script is attached to the submit button on 		*/
/* the choose isa drop down form.				*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "dictionary.h"
#include "folder.h"
#include "insert_database.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "application.h"
#include "appaserver_user.h"
#include "appaserver_parameter_file.h"
#include "appaserver.h"
#include "environ.h"
#include "post2dictionary.h"
#include "related_folder.h"
#include "query.h"
#include "session.h"
#include "role.h"
#include "dictionary_appaserver.h"

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *login_name, *session, *application_name, *folder_name;
	char *database_string;
	char *role_name, *state;
	char sys_string[ 1024 ];
	DICTIONARY *original_post_dictionary;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *insert_update_key;
	char *target_frame;
	LIST *posted_primary_attribute_name_list;
	FOLDER *folder;
	LIST *isa_related_folder_list;
	pid_t dictionary_process_id;
	ROLE *role;
	DICTIONARY_APPASERVER *dictionary_appaserver;
	LIST *non_primary_attribute_name_list;

	if ( argc < 10 )
	{
		fprintf( stderr, 
"Usage: %s login_name application session folder role state insert_update_key target_frame dictionary_process_id\n",
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
		session_access_failed_message_and_exit(
				application_name, session, login_name );
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

	role = role_new_role(	application_name,
				role_name );

	appaserver_parameter_file = appaserver_parameter_file_new();

	original_post_dictionary =
		post2dictionary(stdin,
			appaserver_parameter_file->
				appaserver_data_directory,
			session );

	folder = folder_new_folder(
				application_name,
				session,
				folder_name );

	folder->attribute_list =
		attribute_get_attribute_list(
			application_name,
			folder->folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			role_name );

	if ( ! ( dictionary_appaserver =
			dictionary_appaserver_new(
				original_post_dictionary,
				application_name,
				folder->attribute_list,
				(LIST *)0 /* operation_name_list */ ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: exiting early.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	dictionary_appaserver->non_prefixed_dictionary =
		dictionary_remove_prefix(
			dictionary_appaserver->non_prefixed_dictionary,
			APPASERVER_ISA_PROMPT_PREFIX );

	if ( dictionary_key_exists_index_zero(
			dictionary_appaserver->non_prefixed_dictionary,
			LOOKUP_PUSH_BUTTON_NAME ) )
	{
		state = "lookup";

		/* Need to parse the relational operator of equals */
		/* ----------------------------------------------- */
		dictionary_parse_multi_attribute_keys(
			dictionary_appaserver->non_prefixed_dictionary, 
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			(char *)0 /* prefix */,
			0 /* not dont_include_relational_operators */ );

		sprintf( sys_string,
	"echo \"%s\" 							  |"
	"post_prompt_edit_form %s %s %s %s '%s' '%s' %s %s %d null 2>>%s   ",
			dictionary_appaserver_escaped_send_dictionary_string(
				dictionary_appaserver,
				1 /* with_non_prefixed_dictionary */ ),
		 	login_name,
			application_name,
		 	session,
		 	folder_name,
			role_name,
			state,
			insert_update_key,
			target_frame,
			dictionary_process_id,
			appaserver_error_get_filename(
				application_name ) );

		if ( system( sys_string ) ){};
		exit( 0 );
	}

	if ( ! ( isa_related_folder_list =
			related_folder_get_isa_related_folder_list(
			application_name,
			session,
			folder_name,
			role_name,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			related_folder_recursive_all ) ) )
	{
		char msg[ 1024 ];
		sprintf( msg,
"ERROR in %s/%s(): there is no isa related folder for %s\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	folder_name );
		appaserver_output_error_message(
			application_name, msg, login_name );
	}

	non_primary_attribute_name_list =
		folder_get_non_primary_attribute_name_list(
					folder->attribute_list );

	/* ------------------------------------------------------------- */
	/* Insert here if selected from the drop-down and		 */
	/* there are no non-primary attributes.				 */
	/* ------------------------------------------------------------- */
	if ( !list_length( non_primary_attribute_name_list ) )
	{
		RELATED_FOLDER *isa_related_folder;

		isa_related_folder =
			list_get_first_pointer(
				isa_related_folder_list );

		isa_related_folder->folder->attribute_list =
			attribute_get_attribute_list(
				application_name,
				isa_related_folder->
					folder->
						folder_name,
				(char *)0 /* attribute_name */,
				(LIST *)0 /* mto1_isa_related_folder_list */,
				role_name );

		posted_primary_attribute_name_list =
			dictionary_get_index_zero_key_list(
				dictionary_appaserver->non_prefixed_dictionary, 
				folder_get_primary_attribute_name_list(
					isa_related_folder->
						folder->
						attribute_list ) );

		if ( list_length( posted_primary_attribute_name_list ) )
		{
			char *message = "";
			int rows_inserted;
			char sys_string[ 1024 ];
			INSERT_DATABASE *insert_database;

			insert_database =
				insert_database_calloc(
					application_name,
					session,
					folder_name );

			insert_database->attribute_name_list =
				folder_get_attribute_name_list(
					folder->attribute_list );

			insert_database->primary_attribute_name_list =
				folder_get_primary_attribute_name_list(
					folder->attribute_list );

			dictionary_new_index_key_list_for_data_list(
					dictionary_appaserver->
						non_prefixed_dictionary,
					posted_primary_attribute_name_list,
					insert_database->attribute_name_list,
					0 );

			if (	isa_related_folder->
				folder->
				row_level_non_owner_forbid )
			{
				dictionary_set_pointer(
						dictionary_appaserver->
							non_prefixed_dictionary,
						"login_name_0",
						login_name );
			}

			/* insert_database->dont_remove_tmp_file = 1; */
			insert_database->insert_row_zero_only = 1;

			rows_inserted =
				insert_database_execute(
				&message,
				insert_database->application_name,
				insert_database->session,
				insert_database->folder_name,
				role_name,
				insert_database->primary_attribute_name_list,
				(LIST *)0 /* insert_required_attribute... */,
				insert_database->attribute_name_list,
				dictionary_appaserver->non_prefixed_dictionary,
				insert_database->ignore_attribute_name_list,
				insert_database->insert_row_zero_only,
				insert_database->dont_remove_tmp_file,
				isa_related_folder->folder->post_change_process,
				login_name,
				(LIST *)0 /* mto1_related_folder_list */,
				(LIST *)0 /* common_non_primary_attri... */,
				(LIST *)0 /* attribute_list */,
				0 /* exists_reference_number */,
				appaserver_parameter_file_get_data_directory()
					/* tmp_file_directory */ );

			if ( appaserver_frameset_menu_horizontal(
				application_name,
				login_name ) )
			{
				sprintf(sys_string,
"output_choose_role_folder_process_form '%s' '%s' '%s' '%s' '%s' %s 2>>%s",
					application_name,
					session,
					login_name,
					role_name,
					"" /* title */,
					"y" /* content_type_yn */,
					appaserver_error_get_filename(
						application_name ) );
				if ( system( sys_string ) ){};
				fflush( stdout );
			}

			sprintf( sys_string,
	 		"output_results %s %s %s %s %s %d \"%s\" '' n 2>>%s",
				application_name,
	 			folder_name,
				session,
				login_name,
				role_name,
				rows_inserted,
				(message) ? message : "",
				appaserver_error_get_filename(
					application_name ) );
			if ( system( sys_string ) ){};
			exit( 0 );
		}
	}

	sprintf(sys_string, 
"output_prompt_insert_form %s %s %s %s %s %s y \"%s\" 2>>%s",
 	 	login_name,
		application_name,
		session,
	 	folder_name,
		role_name,
		state,
		dictionary_appaserver_escaped_send_dictionary_string(
			dictionary_appaserver,
			1 /* with_non_prefixed_dictionary */ ),
	 	appaserver_error_get_filename(
			application_name ) );

	if ( system( sys_string ) ){};

	return 0;

}

