/* ---------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/post_prompt_insert_form.c	*/
/* ---------------------------------------------------------	*/
/*								*/
/* This script is attached to the submit button on 		*/
/* the prompt form.						*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* ---------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "String.h"
#include "timlib.h"
#include "list.h"
#include "dictionary.h"
#include "folder.h"
#include "insert_database.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_user.h"
#include "document.h"
#include "application.h"
#include "appaserver_parameter_file.h"
#include "appaserver.h"
#include "environ.h"
#include "related_folder.h"
#include "relation.h"
#include "session.h"
#include "role.h"
#include "query.h"
#include "dictionary_separate.h"
#include "post_dictionary.h"
#include "pair_one2m.h"
#include "folder_menu.h"
#include "vertical_new_button.h"
#include "prompt_insert_form.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void set_null_operator_data_to_null(
			DICTIONARY *query_dictionary,
			LIST *attribute_name_list );

void remove_primary_key_reference_number(
			DICTIONARY *query_dictionary,
			LIST *attribute_list );

void output_missing_information_message(
			char *application_name,
			char *appaserver_mount_point,
			LIST *missing_attribute_name_list );

int post_prompt_insert_database(
			char **message,
			char **isa_message,
			DICTIONARY *row_dictionary,
			DICTIONARY *ignore_dictionary,
			char *application_name,
			char *session,
			char *login_name,
			char *folder_name,
			char *role_name,
			LIST *primary_key_list,
			LIST *insert_required_attribute_name_list,
			LIST *attribute_name_list,
			LIST *mto1_isa_related_folder_list,
			PROCESS *post_change_process,
			LIST *mto1_related_folder_list,
			LIST *attribute_list );

int main( int argc, char **argv )
{
	char *login_name;
	char *session_key;
	char *application_name;
	char *folder_name;
	char *role_name;
	SESSION *session;
	DICTIONARY_SEPARATE *dictionary_separate;
	POST_DICTIONARY *post_dictionary;
	PROMPT_INSERT_FORM *prompt_insert_form;
	LIST *posted_attribute_name_list;
	LIST *non_populated_attribute_name_list;
	LIST *ignore_primary_key_list;
	LIST *subtracted_primary_key_list;
	LIST *insert_required_attribute_name_list;
	char sys_string[ 65536 ];
	int rows_inserted;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *insert_update_key;
	char *target_frame;
	LIST *mto1_isa_related_folder_list = {0};
	ROLE *role;
	char *message = "";
	char *isa_message = "";
	PAIR_ONE2M *pair_one2m;
	LIST *missing_attribute_name_list;
	LIST *ignore_attribute_name_list;
	VERTICAL_NEW_BUTTON *vertical_new_button;
	FOLDER *folder;

	if ( argc != 7 )
	{
		fprintf( stderr, 
"Usage: %s login_name application session folder role target_frame\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	application_name = argv[ 2 ];
	session_key = argv[ 3 ];
	folder_name = argv[ 4 ];
	role_name = argv[ 5 ];
	target_frame = argv[ 6 ];

	session =
		session_folder_integrity_exit(
			argc,
			argv,
			application_name,
			login_name,
			session_key,
			folder_name,
			role_name,
			"insert" /* state */ );

	appaserver_parameter_file = appaserver_parameter_file_new();

	role =
		role_fetch(
			role_name,
			1 /* fetch_attribute_exclude_list */ );

	folder =
		folder_fetch(
			folder_name,
			/* ------------------------- */
			/* Fetching role_folder_list */
			/* ------------------------- */
			role_name,
			role->exclude_insert_attribute_name_list,
			/* --------------------------------------- */
			/* Also sets folder_attribute_primary_list */
			/* and primary_key_list.		   */
			/* ---------------------------------------- */
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_relation_mto1_non_isa_list */,
			0 /* not fetch_relation_mto1_isa_list */,
			boolean fetch_relation_one2m_list,
			boolean fetch_relation_one2m_recursive_list,
			boolean fetch_process,
			boolean fetch_role_folder_list,
			boolean fetch_row_level_restriction,
			boolean fetch_role_operation_list );

	post_dictionary =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		post_dictionary_stdin_new(
			appaserver_parameter_file->
				appaserver_mount_point,
			session_key );


	dictionary_separate =
		/* --------------- */
		/* Always succeeds */
		/* --------------- */
		dictionary_separate_stdin_new(
			post_dictionary->dictionary,
			application_name,
			login_name,
			folder_attribute_date_name_list(
				folder->folder_attribute_list ) );

	/* If pressed the new button next to a drop-down. */
	/* ---------------------------------------------- */
	vertical_new_button = vertical_new_button_calloc();

	if ( ( vertical_new_button->one_folder_name =
		vertical_new_button_dictionary_one_folder_name(
			VERTICAL_NEW_BUTTON_ONE_PREFIX,
			dictionary_separate->
				non_prefixed_dictionary ) ) )
	{
		vertical_new_button_dictionary_set(
			dictionary_separate->non_prefixed_dictionary,
			VERTICAL_NEW_BUTTON_ONE_HIDDEN_LABEL,
			vertical_new_button->one_folder_name );

		vertical_new_button_dictionary_set(
			dictionary_separate->non_prefixed_dictionary,
			VERTICAL_NEW_BUTTON_MANY_HIDDEN_LABEL,
			folder_name );

		sprintf(sys_string,
"echo \"%s\" 								|"
"output_insert_table_form %s %s %s %s '%s' '%s' '%s' 2>>%s		 ",
			dictionary_separate_send_string(
				dictionary_separate_send_dictionary(
					dictionary_separate->
						sort_dictionary,
					dictionary_separate->
						query_dictionary,
					dictionary_separate->
						drilldown_dictionary,
					dictionary_separate->
						ignore_dictionary,
					dictionary_separate->
						pair_one2m_dictionary,
					dictionary_separate->
						non_prefixed_dictionary ) ),
		 	login_name,
			application_name,
		 	session,
		 	vertical_new_button->one_folder_name,
			role_name,
			insert_update_key,
			target_frame,
			appaserver_error_filename(
				application_name ) );

		if ( system( sys_string ) ){};

		exit( 0 );

	} /* if vertical_new_button_folder_name */

	/* =================================== */
	/* Process the non_prefixed_dictionary */
	/* =================================== */

	/* Copy to the query dictionary */
	/* ---------------------------- */
	dictionary_append_dictionary(
		dictionary_separate->query_dictionary,
		dictionary_separate->non_prefixed_dictionary );

	dictionary_separate->non_prefixed_dictionary =
		dictionary_small();

	/* Promote source frame */
	/* -------------------- */
	insert_update_key = target_frame;

	/* If pressed the "lookup" button */
	/* ------------------------------ */
	if ( dictionary_key_exists_index_zero(
			dictionary_separate->working_post_dictionary,
			LOOKUP_PUSH_BUTTON_NAME ) )
	{
		remove_primary_key_reference_number(
			dictionary_separate->query_dictionary,
			folder->folder_attribute_list );

		sprintf(sys_string,
	"echo \"%s\" 						|"
	"output_edit_table_form %s %s %s '%s' '%s' '%s' 2>>%s	 ",
			dictionary_separate_send_string(
				dictionary_separate_send_dictionary(
					dictionary_separate->
						sort_dictionary,
					dictionary_separate->
						query_dictionary,
					dictionary_separate->
						drilldown_dictionary,
					dictionary_separate->
						ignore_dictionary,
					dictionary_separate->
						pair_one2m_dictionary,
					(DICTIONARY *)0
					      /* non_prefixed_dictionary */ ) ),
		 	login_name,
		 	session,
		 	folder_name,
			role_name,
			insert_update_key,
			target_frame,
			appaserver_error_filename( application_name ) );

		if ( system( sys_string ) ){};

		exit( 0 );
	}

	/* -------------------------------------------- */
	/* If pressed <Insert> from the lookup screen	*/
	/* with NULL_OPERATOR selected.			*/
	/* -------------------------------------------- */
	set_null_operator_data_to_null(
		dictionary_separate->query_dictionary,
		folder->attribute_name_list );

	posted_attribute_name_list =
		dictionary_separate_posted_attribute_name_list(
			dictionary_separate->query_dictionary, 
			folder->attribute_name_list );

	ignore_primary_key_list =
		appaserver_library_ignore_pressed_attribute_name_list( 	
			dictionary_separate->ignore_dictionary, 
			folder->primary_key_list,
			(DICTIONARY *)0 /* query_dictionary */ );

	if ( attribute_exists_omit_insert_login_name(
		appaserver->folder->attribute_list ) )
	{
		list_append_pointer(	ignore_primary_key_list,
					"login_name" );
	}

	insert_required_attribute_name_list =
		attribute_insert_required_attribute_name_list(
			appaserver->folder->attribute_list );

	non_populated_attribute_name_list =
		dictionary_get_non_populated_index_zero_key_list(
			dictionary_separate->query_dictionary, 
			insert_required_attribute_name_list );

	mto1_isa_related_folder_list =
		related_folder_get_isa_related_folder_list(
			application_name,
			session,
			folder_name,
			role_name,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			related_folder_recursive_all );

	if ( list_length( mto1_isa_related_folder_list ) )
	{
		appaserver_library_populate_last_foreign_attribute_key(
			dictionary_separate->query_dictionary,
			mto1_isa_related_folder_list,
			appaserver->folder->primary_key_list );
	}

	subtracted_primary_key_list = 
		list_subtract( 	appaserver->
					folder->
					primary_key_list, 
				posted_attribute_name_list );

	list_append_string_list(
			subtracted_primary_key_list, 
			non_populated_attribute_name_list );

	subtracted_primary_key_list =
		list_subtract( 	subtracted_primary_key_list, 
				ignore_primary_key_list );

	ignore_attribute_name_list =
		insert_database_get_trim_indices_dictionary_key_list(
			dictionary_separate->
				ignore_dictionary );

	/* If pair_one2m */
	/* ------------- */
	if ( pair_one2m_participating(
		dictionary_separate->
			pair_one2m_dictionary )
	||    list_length( mto1_isa_related_folder_list ) )
	{
		if ( ( missing_attribute_name_list =
			insert_database_get_missing_attribute_name_list(
				0 /* row */,
				dictionary_separate->query_dictionary,
				ignore_attribute_name_list,
				subtracted_primary_key_list,
				insert_required_attribute_name_list ) ) )
		{
			output_missing_information_message(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point,
				missing_attribute_name_list );
			exit( 0 );
		}
	}

	/* --------------------------------------------- */
	/* If all the needed attributes are populated or */
	/* has an isa relation. 			 */
	/* --------------------------------------------- */
	if ( !list_length( subtracted_primary_key_list )
	||    list_length( mto1_isa_related_folder_list ) )
	{
		rows_inserted =
			post_prompt_insert_database(
				&message,
				&isa_message,
				dictionary_separate->query_dictionary
					/* row_dictionary */,
				dictionary_separate->ignore_dictionary,
				application_name,
				session,
				login_name,
				folder_name,
				role_name,
				appaserver->
					folder->
					primary_key_list,
				insert_required_attribute_name_list,
				appaserver->
					folder->
					attribute_name_list,
				mto1_isa_related_folder_list,
				appaserver->
					folder->
						post_change_process,
				appaserver->folder->
					mto1_related_folder_list,
				appaserver->folder->
					attribute_list );

		if ( rows_inserted )
		{
			folder_menu_refresh_row_count(
				application_name,
				folder_name,
				session,
				appaserver_parameter_file->
					appaserver_data_directory,
				role_name );
		}

		/* If pair_one2m */
		/* ------------- */
		if ( pair_one2m_participating(
			dictionary_separate->pair_one2m_dictionary ) )
		{
			pair_one2m =
				pair_one2m_post_new(
					dictionary_separate->
						pair_one2m_dictionary );

			pair_one2m->many_folder_name =
				pair_one2m_folder_name(
					PAIR_ONE2M_MANY_FOLDER_LABEL,
					pair_one2m->pair_one2m_dictionary );

			pair_one2m->one_folder_name =
				pair_one2m_folder_name(
					PAIR_ONE2M_ONE_FOLDER_LABEL,
					pair_one2m->pair_one2m_dictionary );

			if ( strcmp(
				pair_one2m->one_folder_name,
				pair_one2m->many_folder_name ) == 0 )
			{
				sprintf(sys_string,
	 		"output_results '' %s %s %s %s %d \"%s\" '' y 2>>%s",
	 				pair_one2m->one_folder_name,
					session,
					login_name,
					role_name,
					rows_inserted,
					(message) ? message : "",
						appaserver_error_get_filename(
						application_name ) );

				goto execute_sys_string;
			}

			if ( ( pair_one2m->duplicate =
				pair_one2m_duplicate(
					MYSQL_DUPLICATE_ERROR_MESSAGE_KEY,
					message ) ) )
			{
				pair_one2m_duplicate_set(
					pair_one2m->
						pair_one2m_dictionary,
					PAIR_ONE2M_DUPLICATE_KEY );
			}

			dictionary_separate->pair_one2m_dictionary =
				pair_one2m_fulfilled_dictionary(
					/* ---------------- */
					/* Sets and returns */
					/* ---------------- */
					pair_one2m->
						pair_one2m_dictionary,
					PAIR_ONE2M_FULFILLED_LIST_LABEL,
					pair_one2m->
					   fulfilled_folder_name_list );

			sprintf(
			sys_string,
"echo \"%s\" 								|"
"output_insert_table_form %s %s %s %s '%s' '%s' '%s' 2>>%s		 ",
			dictionary_separate_send_string(
				dictionary_separate_send_dictionary(
					dictionary_separate->
						sort_dictionary,
					dictionary_separate->
						query_dictionary,
					dictionary_separate->
						drilldown_dictionary,
					dictionary_separate->
						ignore_dictionary,
					dictionary_separate->
						pair_one2m_dictionary,
					(DICTIONARY *)0
					     /* non_prefixed_dictionary */ ) ),
		 	login_name,
			application_name,
		 	session,
		 	pair_one2m->one_folder_name,
			role_name,
			insert_update_key,
			target_frame,
			appaserver_error_filename( application_name ) );
		} /* if pair_one2m */
		else
		{
			sprintf(sys_string,
	 		"output_results '' %s %s %s %s %d \"%s\" '' y 2>>%s",
	 			folder_name,
				session,
				login_name,
				role_name,
				rows_inserted,
				(message) ? message : "",
				appaserver_error_filename(
					application_name ) );
		}
	}
	else
	{
		sprintf(sys_string,
"echo \"%s\" 								|"
"output_insert_table_form %s %s %s %s '%s' '%s' '%s' 2>>%s		 ",
			dictionary_separate_send_string(
				dictionary_separate_send_dictionary(
					dictionary_separate->
						sort_dictionary,
					dictionary_separate->
						query_dictionary,
					dictionary_separate->
						drilldown_dictionary,
					dictionary_separate->
						ignore_dictionary,
					dictionary_separate->
						pair_one2m_dictionary,
					(DICTIONARY *)0
					     /* non_prefixed_dictionary */ ) ),
		 	login_name,
			application_name,
		 	session,
		 	folder_name,
			role_name,
			insert_update_key,
			target_frame,
			appaserver_error_get_filename(
					application_name ) );
	}

execute_sys_string:

	if ( system( sys_string ) ){};

	return 0;
}

int post_prompt_insert_database(
			char **message,
			char **isa_message,
			DICTIONARY *row_dictionary,
			DICTIONARY *ignore_dictionary,
			char *application_name,
			char *session,
			char *login_name,
			char *folder_name,
			char *role_name,
			LIST *primary_key_list,
			LIST *insert_required_attribute_name_list,
			LIST *attribute_name_list,
			LIST *mto1_isa_related_folder_list,
			PROCESS *post_change_process,
			LIST *mto1_related_folder_list,
			LIST *attribute_list )
{
	int results;
	INSERT_DATABASE *insert_database;

	dictionary_set_date_time_to_current(
		row_dictionary,
		attribute_list );

	insert_database =
		insert_database_new(
				application_name,
				session,
				folder_name,
				primary_key_list,
				attribute_name_list,
				row_dictionary,
				ignore_dictionary,
				attribute_list );

	insert_database->dont_remove_tmp_file =
		INSERT_DATABASE_DONT_REMOVE_TMP_FILE;

	insert_database->insert_row_zero_only = 1;

	results =
	insert_database_execute(
		message,
		insert_database->application_name,
		insert_database->session,
		insert_database->folder_name,
		role_name,
		insert_database->primary_key_list,
		insert_required_attribute_name_list,
		insert_database->attribute_name_list,
		insert_database->row_dictionary,
		insert_database->ignore_attribute_name_list,
		insert_database->insert_row_zero_only,
		insert_database->dont_remove_tmp_file,
		post_change_process,
		login_name,
		mto1_related_folder_list,
		related_folder_common_non_primary_key_list(
			folder_name,
			mto1_related_folder_list ),
		insert_database->attribute_list,
		attribute_exists_reference_number(
			insert_database->attribute_list ),
		appaserver_parameter_file_get_data_directory()
			/* tmp_file_directory */ );

	if ( list_rewind( mto1_isa_related_folder_list  ) )
	{
		RELATED_FOLDER *isa_related_folder;
		LIST *isa_attribute_name_list;
		LIST *isa_primary_key_list;

		do {
			isa_related_folder =
				list_get_pointer(
					mto1_isa_related_folder_list );

			isa_related_folder->folder->attribute_list =
				attribute_get_attribute_list(
				application_name,
				isa_related_folder->folder->folder_name,
				(char *)0 /* attribute_name */,
				(LIST *)0 /* mto1_isa_related_folder_list */,
				role_name );

			isa_attribute_name_list =
				folder_get_attribute_name_list(
					isa_related_folder->
					folder->attribute_list );

			isa_primary_key_list =
				folder_get_primary_key_list(
					isa_related_folder->
						folder->
						attribute_list );

			dictionary_new_index_key_list_for_data_list(
					row_dictionary,
					primary_key_list,
					isa_primary_key_list,
					0 );
					
			insert_database =
				insert_database_new(
					application_name,
					session,
					isa_related_folder->
						folder->
						folder_name,
					isa_primary_key_list,
					isa_attribute_name_list,
					row_dictionary,
					ignore_dictionary,
					isa_related_folder->
						folder->
						attribute_list );

			insert_database->dont_remove_tmp_file =
				INSERT_DATABASE_DONT_REMOVE_TMP_FILE;

			insert_database->insert_row_zero_only = 1;

			insert_database_execute(
				isa_message,
				insert_database->application_name,
				insert_database->session,
				insert_database->folder_name,
				role_name,
				insert_database->primary_key_list,
				insert_required_attribute_name_list,
				insert_database->attribute_name_list,
				insert_database->row_dictionary,
				insert_database->ignore_attribute_name_list,
				insert_database->insert_row_zero_only,
				insert_database->dont_remove_tmp_file,
				isa_related_folder->folder->post_change_process,
				login_name,
				(LIST *)0 /* mto1_related_folder_list */,
				(LIST *)0 /* common_non_primary_attri... */,
				insert_database->attribute_list,
				attribute_exists_reference_number(
					insert_database->attribute_list ),
				appaserver_parameter_file_get_data_directory()
					/* tmp_file_directory */ );

		} while( list_next( mto1_isa_related_folder_list ) );

	} /* if list_rewind() */

	return results;

}

void output_missing_information_message(char *application_name,
					char *appaserver_mount_point,
					LIST *missing_attribute_name_list )
{
	DOCUMENT *d = document_new( "Missing Information",
				    application_name );

	document_output_content_type();

	document_output_heading(
		d->application_name,
		d->title,
		d-> output_content_type,
		appaserver_mount_point,
		d->javascript_module_list,
		d->stylesheet_filename,
		application_relative_source_directory(
			application_name ),
		0 /* not with_dynarch_menu */ );

	document_output_body(
			d->application_name,
			d->onload_control_string );

	printf( "<h3>Error: Missing primary information of %s.</h3>\n",
		list_display( missing_attribute_name_list ) );
	document_close();

}

void remove_primary_key_reference_number(
			DICTIONARY *post_dictionary,
			LIST *attribute_list )
{
	ATTRIBUTE *attribute;

	if ( !list_rewind( attribute_list ) ) return;

	do {
		attribute = list_get_pointer( attribute_list );

		if ( strcmp( attribute->datatype, "reference_number" ) == 0
		&&   attribute->primary_key_index )
		{
			char key[ 128 ];

			dictionary_remove_key(
				post_dictionary,
				attribute->attribute_name );

			sprintf( key, "%s_0", attribute->attribute_name );
			dictionary_remove_key(
				post_dictionary,
				key );
		}
	} while( list_next( attribute_list ) );
}

void set_null_operator_data_to_null(
			DICTIONARY *query_dictionary,
			LIST *attribute_name_list )
{
	char *attribute_name;
	char *data;
	char key[ 128 ];

	if ( !list_rewind( attribute_name_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty attribute_name_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
	}

	do {
		attribute_name =
			list_get(
				attribute_name_list );

		sprintf( key,
			 "%s_0",
			 attribute_name );

		data = dictionary_fetch( key, query_dictionary );

		if ( timlib_strcmp( data, NULL_OPERATOR ) == 0 )
		{
			dictionary_delete(
				query_dictionary,
				key );
		}
		else
		if ( timlib_strcmp( data, NOT_NULL_OPERATOR ) == 0 )
		{
			dictionary_delete(
				query_dictionary,
				key );
		}

		/* Also without the suffix. */
		/* ------------------------ */
		data =
			dictionary_fetch(
				attribute_name,
				query_dictionary );

		if ( timlib_strcmp( data, NULL_OPERATOR ) == 0 )
		{
			dictionary_delete(
				query_dictionary,
				attribute_name );
		}
		else
		if ( timlib_strcmp( data, NOT_NULL_OPERATOR ) == 0 )
		{
			dictionary_delete(
				query_dictionary,
				attribute_name );
		}

	} while( list_next( attribute_name_list ) );

}

