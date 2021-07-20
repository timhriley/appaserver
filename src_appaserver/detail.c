/* ---------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/detail.c				*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Note: this is also called from post_edit_table_form.			*/
/* ---------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "list_usage.h"
#include "appaserver_library.h"
#include "appaserver_user.h"
#include "appaserver_error.h"
#include "folder.h"
#include "query.h"
#include "html_table.h"
#include "related_folder.h"
#include "document.h"
#include "application.h"
#include "appaserver.h"
#include "appaserver_parameter_file.h"
#include "dictionary.h"
#include "form.h"
#include "role_folder.h"
#include "role.h"
#include "dictionary2file.h"
#include "operation_list.h"
#include "session.h"
#include "boolean.h"
#include "environ.h"
#include "row_security.h"
#include "lookup_before_drop_down.h"
#include "javascript.h"
#include "dictionary_appaserver.h"

/* Constants */
/* --------- */

/* See site_visit_device_addition -> site_visit */
/*--------------------------------------------- */
#define MAKE_LONG_DROP_DOWNS_NON_EDIT 1

#define INSERT_UPDATE_KEY	"detail"
#define PROCESS_NAME		"detail"

#define OUTPUT_INSERT_BUTTON	1

/* Prototypes */
/* ---------- */
void detail_mto1_isa_related_folder_list(
			LIST *mto1_isa_related_folder_list,
			LIST *where_attribute_data_list );

void detail_set_folder_javascript_files(
			DOCUMENT *document,
			char *application_name,
			char *folder_name,
			LIST *one2m_related_folder_list,
			LIST *mto1_related_folder_list );

char *detail_onload_control_string(
			FOLDER *folder,
			LIST *one2m_related_folder_list,
			LIST *mto1_related_folder_list );

void save_ending_form_number(
			char *appaserver_data_directory,
			char *folder_name,
			int parent_pid,
			int form_number );

int detail_starting_form_number(
			char *appaserver_data_directory,
			char *folder_name,
			int parent_pid );

char *detail_form_number_semaphore_filename(
			char *appaserver_data_directory,
			char *folder_name,
			int parent_pid );

DICTIONARY *output_folder_detail(
			int *form_number,
			char *application_name,
			char *session,
			char *login_name,
			FOLDER *folder,
			char *role_name,
			char *target_frame,
			LIST *primary_attribute_data_list,
			char *appaserver_data_directory,
			boolean remove_update_permission,
			char *base_folder_name,
			boolean omit_insert_flag,
			boolean omit_operation_buttons,
			boolean output_even_if_not_populated,
			DICTIONARY_APPASERVER *dictionary_appaserver,
			boolean override_row_restrictions,
			enum omit_delete_operation
				regular_omit_delete_operation,
			enum omit_delete_operation
				viewonly_omit_delete_operation,
			LIST *non_edit_folder_name_list,
			boolean make_primary_keys_non_edit );

void output_table_heading(
			HTML_TABLE *html_table, 
			LIST *attribute_name_list );

void output_table_data_heading(
			HTML_TABLE *html_table, 
			LIST *attribute_name_list );

void output_mto1_folder_detail(
			int *form_number,
			LIST *non_edit_folder_name_list,
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			char *role_name,
			char *target_frame,
			DICTIONARY *primary_dictionary,
			char *appaserver_data_directory,
			char *base_folder_name,
			DICTIONARY_APPASERVER *dictionary_appaserver,
			boolean override_row_restrictions,
			LIST *role_folder_insert_list,
			LIST *role_folder_update_list,
			LIST *role_folder_lookup_list );

void output_1tom_folder_detail(
			int *form_number,
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			char *role_name,
			char *target_frame,
			LIST *primary_data_list,
			char *appaserver_mount_point,
			char *appaserver_data_directory,
			char *base_folder_name,
			boolean output_even_if_not_populated,
			DICTIONARY_APPASERVER *dictionary_appaserver,
			boolean override_row_restrictions,
			LIST *role_folder_insert_list,
			LIST *role_folder_update_list,
			LIST *role_folder_lookup_list );

int main( int argc, char **argv )
{
	char *application_name, *session, *folder_name;
	char *login_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *role_name;
	char *target_frame;
	DICTIONARY *primary_dictionary = {0};
	char *primary_data_list_string;
	LIST *primary_attribute_data_list;
	LIST *ignore_attribute_name_list;
	char *base_folder_name;
	DICTIONARY *original_post_dictionary;
	FOLDER *folder;
	ROLE *role;
	char *output_content_type_yn;
	LIST *role_folder_insert_list;
	LIST *role_folder_update_list;
	LIST *role_folder_lookup_list;
	DICTIONARY_APPASERVER *dictionary_appaserver;
	char form_title[ 128 ];
	LOOKUP_BEFORE_DROP_DOWN *lookup_before_drop_down;
	LIST *non_edit_folder_name_list = {0};
	int parent_pid;
	int form_number;

	application_name = environ_exit_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 9 )
	{
		fprintf( stderr,
"Usage: %s ignored session login_name folder role target_frame primary_data_bar_list dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	/* application_name = argv[ 1 ]; */
	session = argv[ 2 ];
	login_name = argv[ 3 ];
	base_folder_name = folder_name = argv[ 4 ];
	role_name = argv[ 5 ];
	target_frame = argv[ 6 ];
	primary_data_list_string = argv[ 7 ];
	original_post_dictionary = dictionary_string2dictionary( argv[ 8 ] );

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
				"lookup" ) )
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

	role =
		role_new_role(
			application_name,
			role_name );

	primary_attribute_data_list =
		string2list(
			primary_data_list_string,
			FOLDER_DATA_DELIMITER );

	appaserver_parameter_file = appaserver_parameter_file_new();

	folder =
		folder_load_new(
			application_name,
			session,
			folder_name );

	if ( ! ( dictionary_appaserver =
			dictionary_appaserver_new(
				original_post_dictionary,
				application_name,
				appaserver->
					folder->
					append_isa_attribute_list,
				(LIST *)0 /* operation_name_list */ ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: exiting early.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	lookup_before_drop_down =
		lookup_before_drop_down_new(
			application_name,
			dictionary_appaserver->
				lookup_before_drop_down_dictionary,
			(char *)0 /* state */ );

	lookup_before_drop_down->lookup_before_drop_down_state =
		lookup_before_drop_down_get_state(
			lookup_before_drop_down->
				lookup_before_drop_down_folder_list,
			dictionary_appaserver->
				lookup_before_drop_down_dictionary,
			dictionary_appaserver->preprompt_dictionary,
			appaserver->folder->lookup_before_drop_down );

	if ( lookup_before_drop_down->
		lookup_before_drop_down_state ==
			lookup_skipped )
	{
		non_edit_folder_name_list =
			lookup_before_drop_down_get_non_edit_folder_name_list(
				lookup_before_drop_down->
					lookup_before_drop_down_folder_list,
				folder->mto1_related_folder_list );
	}

	/* Get from non_prefixed_dictionary and repopulate it. */
	/* --------------------------------------------------- */
	output_content_type_yn =
		dictionary_get_pointer(
			dictionary_appaserver->non_prefixed_dictionary,
			CONTENT_TYPE_YN );

	parent_pid =
		atoi( dictionary_safe_fetch(
			dictionary_appaserver->non_prefixed_dictionary,
			PROCESS_ID_LABEL ) );

	dictionary_appaserver->non_prefixed_dictionary =
		dictionary_small_new();

	dictionary_set_pointer( 
		dictionary_appaserver->non_prefixed_dictionary,
			CONTENT_TYPE_YN,
			"n" );

	dictionary_set_string(
		dictionary_appaserver->non_prefixed_dictionary,
		PRIMARY_DATA_LIST_KEY,
		primary_data_list_string );

	/* ------------------------------------ */
	/* Set the primary data list string	*/
	/* so parameter_dictionary will get it. */
	/* ------------------------------------ */
	dictionary_set_string(
		dictionary_appaserver->preprompt_dictionary,
		PRIMARY_DATA_LIST_KEY,
		primary_data_list_string );

	document = document_new( "", application_name );

	if ( !output_content_type_yn
	||   *output_content_type_yn != 'n' )
	{
		document_set_output_content_type( document );
	}

	document_set_javascript_module( document, "trim" );
	document_set_javascript_module( document, "null2slash" );
	document_set_javascript_module( document, "push_button_submit" );
	document_set_javascript_module( document, "timlib" );
	document_set_javascript_module( document, "form" );

	detail_set_folder_javascript_files(
		document,
		application_name,
		folder_name,
		appaserver->folder->one2m_related_folder_list,
		appaserver->folder->mto1_related_folder_list );

	document_output_head(
		document->application_name,
		document->title,
		document->output_content_type,
		appaserver_parameter_file->appaserver_mount_point,
		document->javascript_module_list,
		document->stylesheet_filename,
		application_relative_source_directory(
			application_name ),
		0 /* not with_dynarch_menu */ );

	document->onload_control_string =
		detail_onload_control_string(
			appaserver->folder,
			appaserver->folder->one2m_related_folder_list,
			appaserver->folder->mto1_related_folder_list );

	document_output_body(
		document->application_name,
		document->onload_control_string );

	sprintf( form_title,
		 "%s %s",
		 PROCESS_NAME,
		 folder_name );

	format_initial_capital( form_title, form_title );

	printf( "<h1>%s</h1>\n", form_title );

	appaserver_library_list_database_convert_dates(
		primary_data_list,
		application_name,
		appaserver->folder->append_isa_attribute_list );

	form_number =
		detail_starting_form_number(
			appaserver_parameter_file->
				appaserver_data_directory,
			folder_name,
			parent_pid );

	primary_dictionary =
		output_folder_detail(
			&form_number,
			application_name,
			session,
			login_name,
			folder,
			role->role_name,
			target_frame,
			primary_attribute_data_list,
			appaserver_parameter_file->appaserver_data_directory,
			0 /* not remove_update_permission */,
			base_folder_name,
			1 /* omit_insert_flag */,
			0 /* dont omit_operation_buttons */,
			1 /* output_even_if_not_populated */,
			dictionary_appaserver,
			role_override_row_restrictions(
				role->override_row_restrictions_yn ),
			dont_omit_delete
				/* regular_omit_delete_operation */,
			omit_delete_with_placeholder
				/* viewonly_omit_delete_operation */,
			non_edit_folder_name_list,
			0 /* not make_primary_keys_non_edit */ );

	if ( list_length( folder->mto1_isa_related_folder_list ) )
	{
		detail_mto1_isa_related_folder_list(
			folder->mto1_isa_related_folder_list,
			where_attribute_data_list );
	}

	role_folder_insert_list =
		role_folder_insert_list_fetch(
			application_name,
			session,
			role_name );

	role_folder_update_list =
		role_folder_update_list_fetch(
			application_name,
			session,
			role_name );

	role_folder_lookup_list =
		role_folder_lookup_list_fetch(
			application_name,
			session,
			role_name );

	mto1_isa_related_folder_list =
		related_folder_get_mto1_related_folder_list(
			list_new() /* existing_related_folder_list */,
			application_name,
			session,
			folder_name,
			role_name,
			1 /* isa_flag */,
			related_folder_recursive_all,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			(LIST *)0 /* root_primary_attribute_name_list */,
			0 /* recursive_level */ );

	if ( list_length( mto1_isa_related_folder_list ) )
	{
	} /* if mto1_isa_related_folder */

	output_1tom_folder_detail(
		&form_number,
		application_name,
		session,
		folder_name,
		login_name,
		role_name,
		target_frame,
		primary_data_list,
		appaserver_parameter_file->
			appaserver_mount_point,
		appaserver_parameter_file->
			appaserver_data_directory,
		base_folder_name,
		0 /* dont output_even_if_not_populated */,
		dictionary_appaserver,
		role_get_override_row_restrictions(
			role->override_row_restrictions_yn ),
		role_folder_insert_list,
		role_folder_update_list,
		role_folder_lookup_list );

	output_mto1_folder_detail(
		&form_number,
		non_edit_folder_name_list,
		application_name,
		session,
		folder_name,
		login_name,
		role_name,
		target_frame,
		primary_dictionary,
		appaserver_parameter_file->
			appaserver_data_directory,
		base_folder_name,
		dictionary_appaserver,
		role_get_override_row_restrictions(
			role->override_row_restrictions_yn ),
		role_folder_insert_list,
		role_folder_update_list,
		role_folder_lookup_list );

	save_ending_form_number(
		appaserver_parameter_file->
			appaserver_data_directory,
		folder_name,
		parent_pid,
		form_number );

	document_close();
	return 0;
}

void detail_mto1_isa_related_folder_list(
			LIST *mto1_isa_related_folder_list,
			LIST *where_attribute_data_list )
{
		RELATED_FOLDER *related_folder;
		LIST *where_clause_data_list;

		related_folder =
			list_get_first_pointer(
				mto1_isa_related_folder_list );

		if ( list_equivalent_string_list(
			primary_attribute_name_list,
			related_folder->
				one2m_folder->
				primary_attribute_name_list ) )
		{
			where_clause_data_list = primary_data_list;
		}
		else
		{
			where_clause_data_list =
			related_folder_fetch_non_primary_foreign_data_list(
				application_name,
				related_folder->foreign_attribute_name_list
					/* select_list */,
				folder_name,
				primary_attribute_name_list,
				primary_data_list );
		}

		if ( !related_folder->folder->append_isa_attribute_list )
		{
			related_folder->folder->mto1_isa_related_folder_list =
				folder_mto1_isa_related_folder_list(
					list_new(),
					application_name,
					related_folder->
						folder->
						folder_name,
					role_name,
					0 /* recursive_level */ );

			related_folder->folder->append_isa_attribute_list =
				folder_append_isa_attribute_list(
					application_name,
					related_folder->folder->folder_name,
					related_folder->
						folder->
						mto1_isa_related_folder_list,
					role_name );
		}

		if ( !list_length( related_folder->
					folder->
					append_isa_attribute_list ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: empty append_isa_attribute_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );

		}

		output_folder_detail(
			&form_number,
			application_name,
			session,
			related_folder->folder->folder_name,
			role_name,
			target_frame,
			related_folder->folder->append_isa_attribute_list,
			related_folder->folder->primary_attribute_name_list
				/* where_clause_attribute_name_list */,
			where_clause_data_list,
			login_name,
			appaserver_parameter_file->appaserver_data_directory,
			1 /* remove_update_permission */,
			base_folder_name,
			1 /* omit_insert_flag */,
			0 /* dont omit_operation_buttons */,
			(char *)0 /* last_related_attribute_name */,
			0 /* dont output_even_if_not_populated */,
			dictionary_appaserver,
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			dont_omit_delete
				/* regular_omit_delete_operation */,
			omit_delete_with_placeholder
				/* viewonly_omit_delete_operation */,
			non_edit_folder_name_list,
			1 /* make_primary_keys_non_edit */ );


char *detail_form_number_semaphore_filename(
				char *appaserver_data_directory,
				char *folder_name,
				int parent_pid )
{
	static char semaphore_filename[ 128 ];

	sprintf( semaphore_filename,
		 "%s/%s_semaphore_%d",
		 appaserver_data_directory,
		 folder_name,
		 parent_pid );

	return semaphore_filename;

}

int detail_starting_form_number(
			char *appaserver_data_directory,
			char *folder_name,
			int parent_pid )
{
	char *semaphore_filename;
	FILE *file;
	char form_number[ 8 ];

	if ( !parent_pid ) return 0;

	semaphore_filename =
		detail_form_number_semaphore_filename(
			appaserver_data_directory,
			folder_name,
			parent_pid );

	if ( ! ( file = fopen( semaphore_filename, "r" ) ) )
		return 0;

	if ( !get_line( form_number, file ) )
	{
		fclose( file );
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty file = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 semaphore_filename );
		exit( 1 );
	}

	fclose( file );
	return atoi( form_number );

}

void save_ending_form_number(	char *appaserver_data_directory,
				char *folder_name,
				int parent_pid,
				int form_number )
{
	char *semaphore_filename;
	FILE *file;

	if ( !parent_pid ) return;

	semaphore_filename =
		detail_form_number_semaphore_filename(
			appaserver_data_directory,
			folder_name,
			parent_pid );

	if ( ! ( file = fopen( semaphore_filename, "w" ) ) )
	{
		fclose( file );
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot write file = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 semaphore_filename );
		exit( 1 );
	}

	fprintf( file, "%d\n", form_number );

}

void output_1tom_folder_detail(
			int *form_number,
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			char *role_name,
			char *target_frame,
			LIST *primary_data_list,
			char *appaserver_mount_point,
			char *appaserver_data_directory,
			char *base_folder_name,
			boolean output_even_if_not_populated,
			DICTIONARY_APPASERVER *dictionary_appaserver,
			boolean override_row_restrictions,
			LIST *role_folder_insert_list,
			LIST *role_folder_update_list,
			LIST *role_folder_lookup_list )
{
	RELATED_FOLDER *related_folder;
	APPASERVER *appaserver;
	boolean omit_insert_flag = 1;
	LIST *non_edit_folder_name_list = {0};
	DOCUMENT *document;
	boolean local_omit_insert_flag;
	boolean local_output_even_if_not_populated;
	boolean remove_update_permission;

	omit_insert_flag = 1 - OUTPUT_INSERT_BUTTON;

	/* Executing this again; need to optimize. */
	/* --------------------------------------- */
	appaserver =
		appaserver_folder_new(
			application_name,
			session,
			folder_name );

	if ( !appaserver->folder->one2m_related_folder_list )
	{
		appaserver->folder->one2m_related_folder_list =
		   related_folder_1tom_related_folder_list(
			appaserver->application_name,
			appaserver->session,
			appaserver->folder->folder_name,
			role_name,
			detail,
			primary_data_list,
			list_new() /* related_folder_list */,
			0 /* dont omit_isa_relations */,
			related_folder_no_recursive,
			(LIST *)0 /* parent_primary_attribute_name_list */,
			(LIST *)0 /* original_primary_attribute_name_list */,
			(char *)0 /* prior_related_attribute_name */ );
	}

	if ( !appaserver->folder->append_isa_attribute_list )
	{
		appaserver->folder->mto1_isa_related_folder_list =
			folder_mto1_isa_related_folder_list(
				list_new(),
				application_name,
				appaserver->
					folder->
					folder_name,
				role_name,
				0 /* recursive_level */ );

		appaserver->folder->append_isa_attribute_list =
			folder_append_isa_attribute_list(
				application_name,
				appaserver->folder->folder_name,
				appaserver->
					folder->
					mto1_isa_related_folder_list,
				role_name );
	}

	if ( !list_rewind( appaserver->folder->one2m_related_folder_list ) )
		return;

	do {
		related_folder =
			list_get_pointer(
				appaserver->folder->
					one2m_related_folder_list );

		if ( !list_exists_string(
			related_folder->
				one2m_folder->
					folder_name,
			role_folder_insert_list )
		&&   !list_exists_string(
			related_folder->
				one2m_folder->
					folder_name,
			role_folder_update_list )
		&&   !list_exists_string(
			related_folder->
				one2m_folder->
					folder_name,
			role_folder_lookup_list ) )
		{
			continue;
		}

		remove_update_permission = 0;

		document = document_new( "", application_name );

/*
		document_set_folder_javascript_files(
			document,
			application_name,
			related_folder->
				one2m_folder->
					folder_name );

		document_output_each_javascript_source(
			application_name,
			document->javascript_module_list,
			appaserver_mount_point,
			application_relative_source_directory(
				application_name ) );
*/

		if ( related_folder->relation_type_isa )
		{
			remove_update_permission = 1;
			local_output_even_if_not_populated = 0;
			local_omit_insert_flag = 1;
		}
		else
		{
			local_output_even_if_not_populated =
				output_even_if_not_populated;
			local_omit_insert_flag = omit_insert_flag;
		}

		if ( !list_length(
			related_folder->
				one2m_folder->
				append_isa_attribute_list ) )
		{
			related_folder->
				one2m_folder->
				mto1_isa_related_folder_list =
				   folder_mto1_isa_related_folder_list(
					list_new(),
					application_name,
					related_folder->
						one2m_folder->
						folder_name,
					role_name,
					0 /* recursive_level */ );

			related_folder->
				one2m_folder->
				append_isa_attribute_list =
				   folder_append_isa_attribute_list(
					   application_name,
					   related_folder->
						one2m_folder->
						folder_name,
					related_folder->
				   		one2m_folder->
				   	mto1_isa_related_folder_list,
					role_name );
		}

		if ( !list_length(
			related_folder->
				one2m_folder->
				append_isa_attribute_list ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: empty append_isa_attribute_list.\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__ );
			exit( 1 );
		}

		if ( !related_folder->folder->mto1_related_folder_list )
		{
			related_folder->
				folder->
				mto1_related_folder_list =
				related_folder_get_mto1_related_folder_list(
					(LIST *)0,
					application_name,
					session,
					related_folder->
						one2m_folder->
						folder_name,
					role_name,
					0 /* isa_flag */,
					related_folder_no_recursive,
					override_row_restrictions,
					(LIST *)0
					/* root_primary_attribute_name_list */,
					0 /* recursive_level */ );
		}

		if ( MAKE_LONG_DROP_DOWNS_NON_EDIT )
		{
		    non_edit_folder_name_list = list_new();

		    list_append_string_list(
			       non_edit_folder_name_list,
			       related_folder_get_mto1_multi_key_name_list(
					related_folder->
						folder->
						mto1_related_folder_list ) );
		}

		related_folder->folder_foreign_attribute_name_list =
			related_folder_foreign_attribute_name_list(
			   folder_get_primary_attribute_name_list(
				related_folder->
					folder->
					attribute_list ),
			   related_folder->related_attribute_name,
			   related_folder->
				   folder_foreign_attribute_name_list );

		output_folder_detail(
			form_number,
			appaserver->application_name,
			session,
			related_folder->
				one2m_folder->
				folder_name,
			role_name,
			target_frame,
			related_folder->
				one2m_folder->
				append_isa_attribute_list,
			related_folder->
				folder_foreign_attribute_name_list
				/* where_attribute_name_list */,
			primary_data_list
				/* where_data_list */,
			login_name,
			appaserver_data_directory,
			remove_update_permission,
			base_folder_name,
			local_omit_insert_flag,
			0 /* dont omit_operation_buttons */,
			related_folder->related_attribute_name,
			local_output_even_if_not_populated,
			dictionary_appaserver,
			override_row_restrictions,
			dont_omit_delete
				/* regular_omit_delete_operation */,
			omit_delete_with_placeholder
				/* viewonly_omit_delete_operation */,
			non_edit_folder_name_list,
			0 /* not make_primary_keys_non_edit */ );

	} while( list_next(
			appaserver->folder->one2m_related_folder_list ) );

}

void output_table_data_heading(	HTML_TABLE *html_table, 
				LIST *attribute_name_list )
{
	char *attribute_name;
	char buffer[ 1024 ];

	html_table_reset_data_heading( html_table );

	if ( list_rewind( attribute_name_list ) )
	{
		do {
			attribute_name = list_get_string( attribute_name_list );

			html_table_set_heading(
				html_table,
				format_initial_capital( buffer,
							attribute_name ) );

		} while( list_next( attribute_name_list ) );

		html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );
	}
}

void output_mto1_folder_detail(
			int *form_number,
			LIST *non_edit_folder_name_list,
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			char *role_name,
			char *target_frame,
			DICTIONARY *primary_dictionary,
			char *appaserver_data_directory,
			char *base_folder_name,
			DICTIONARY_APPASERVER *dictionary_appaserver,
			boolean override_row_restrictions,
			LIST *role_folder_insert_list,
			LIST *role_folder_update_list,
			LIST *role_folder_lookup_list )
{
	RELATED_FOLDER *related_folder;
	APPASERVER *appaserver;
	LIST *primary_data_list = {0};
	ROLE *role;

	if ( ! ( role = role_new( application_name, role_name ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: role_new(%s) returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 role_name );
	}

	appaserver =
		appaserver_folder_new(
			application_name,
			session,
			folder_name );

	appaserver->folder->mto1_isa_related_folder_list =
		folder_mto1_isa_related_folder_list(
			list_new() /* existing_related_folder_list */,
			application_name,
			folder_name,
			role_name,
			0 /* recursive_level */ );

	appaserver->folder->append_isa_attribute_list =
		folder_append_isa_attribute_list(
			application_name,
			appaserver->folder->folder_name,
			appaserver->
				folder->
				mto1_isa_related_folder_list,
				role_name );

	appaserver->folder->mto1_related_folder_list =
		related_folder_get_mto1_related_folder_list(
			appaserver->folder->mto1_related_folder_list,
			appaserver->application_name,
			appaserver->session,
			appaserver->folder->folder_name,
			role_name,
			0 /* isa_flag */,
			related_folder_no_recursive,
			override_row_restrictions,
			(LIST *)0 /* root_primary_attribute_name_list */,
			0 /* recursive_level */ );

	if ( !list_rewind( appaserver->folder->mto1_related_folder_list ) )
		return;

	do {
		related_folder =
			list_get(
				appaserver->
					folder->
					mto1_related_folder_list );

		if ( !list_exists_string(
			related_folder->folder->folder_name,
			role_folder_insert_list )
		&&   !list_exists_string(
			related_folder->folder->folder_name,
			role_folder_update_list )
		&&   !list_exists_string(
			related_folder->folder->folder_name,
			role_folder_lookup_list ) )
		{
			continue;
		}

		related_folder->folder->mto1_isa_related_folder_list =
			folder_mto1_isa_related_folder_list(
				list_new(),
				application_name,
				related_folder->folder->folder_name,
				role_name,
				0 /* recursive_level */ );

		related_folder->folder->append_isa_attribute_list =
			folder_append_isa_attribute_list(
				application_name,
				related_folder->
					folder->
					folder_name,
				related_folder->
				   folder->
				   mto1_isa_related_folder_list,
				role_name );

		/* If no non-primary attributes and no operations, ignore */
		/* ------------------------------------------------------ */
		if ( !list_length(
			folder_get_non_primary_attribute_name_list(
				related_folder->
					folder->
					append_isa_attribute_list ) )
		&&   !list_length( operation_list_get_operation_list(
					application_name,
					BOGUS_SESSION,
					related_folder->folder->folder_name,
					role_name,
					omit_delete
					/* omit_delete_operation */ ) ) )

		{
			continue;
		}

		if ( primary_dictionary )
		{
			primary_data_list =
			   dictionary_using_list_get_index_data_list( 
			      primary_dictionary,
			      related_folder_foreign_attribute_name_list(
			         folder_get_primary_attribute_name_list(
			            related_folder->
					folder->
					append_isa_attribute_list ),
			            related_folder->related_attribute_name,
				    related_folder->
					folder_foreign_attribute_name_list ),
			      0 /* row */ );

			appaserver_library_list_database_convert_dates(
					primary_data_list,
					application_name,
					related_folder->
						folder->
						append_isa_attribute_list );
		}
		else
		{
			primary_data_list = (LIST *)0;
		}

		/* ---------------------------------------------------- */
		/* Some relations exists just for joins to work,	*/
		/* like PROCESS_PARAMETER -> ATTRIBUTE.			*/
		/* ---------------------------------------------------- */
		if ( primary_data_list
		&&   list_length( primary_data_list ) == 1
		&&   strcmp(	(char *)list_get_first_pointer(
						primary_data_list ),
				"null" ) == 0 )
		{
			continue;
		}

		if ( list_length( primary_data_list )
		&&   list_string_list_all_populated(
			primary_data_list ) )
		{
			related_folder->folder->mto1_isa_related_folder_list =
				folder_mto1_isa_related_folder_list(
					list_new()
					   /* existing_related_folder_list */,
					application_name,
					related_folder->folder->folder_name,
					role_name,
					0 /* recursive_level */ );

			related_folder->
				folder->
				append_isa_attribute_list =
					folder_append_isa_attribute_list(
						application_name,
						related_folder->
							folder->
							folder_name,
						related_folder->
						   folder->
						   mto1_isa_related_folder_list,
						role_name );

			if ( !list_length( 
				related_folder->
					folder->
					append_isa_attribute_list ) )
			{
				fprintf( stderr,
		"ERROR in %s/%s()/%d: empty append_isa_attribute_list\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			output_folder_detail(
				form_number,
				appaserver->application_name,
				session,
				related_folder->folder->folder_name,
				role_name,
				target_frame,
				related_folder->
					folder->
					append_isa_attribute_list,
				folder_get_primary_attribute_name_list(
					related_folder->
						folder->
						append_isa_attribute_list )
					/* where_clause_attribute_name_list */,
				primary_data_list
					/* where_clause_data_list */,
				login_name,
				appaserver_data_directory,
				1 /* remove_update_permission */,
				base_folder_name,
				1 /* omit_insert_flag */,
				0 /* dont omit_operation_buttons */,
				related_folder->related_attribute_name,
				0 /* not output_even_if_not_populated */,
				dictionary_appaserver,
				override_row_restrictions,
				omit_delete
					/* regular_omit_delete_operation */,
				omit_delete
					/* viewonly_omit_delete_operation */,
				non_edit_folder_name_list,
				1 /* make_primary_keys_non_edit */ );
		}

	} while( list_next(
		appaserver->folder->mto1_related_folder_list ) );
}

/* Returns the last dictionary fetched */
/* ----------------------------------- */
DICTIONARY *output_folder_detail(
			int *form_number,
			char *application_name,
			char *session,
			char *folder_name,
			char *role_name,
			char *target_frame,
			LIST *append_isa_attribute_list,
			LIST *primary_attribute_data_list,
			char *login_name,
			char *appaserver_data_directory,
			int remove_update_permission,
			char *base_folder_name,
			boolean omit_insert_flag,
			boolean omit_operation_buttons,
			char *last_related_attribute_name,
			boolean output_even_if_not_populated,
			DICTIONARY_APPASERVER *dictionary_appaserver,
			boolean override_row_restrictions,
			enum omit_delete_operation
				regular_omit_delete_operation,
			enum omit_delete_operation
				viewonly_omit_delete_operation,
			LIST *non_edit_folder_name_list,
			boolean make_primary_keys_non_edit )
{
	LIST *fetched_dictionary_list;
	FORM *form;
	ROLE_FOLDER *role_folder;
	ROLE *role;
	int output_submit_reset_buttons_in_trailer;
	char action_string[ 1024 ];
	char caption_string[ 512 ];
	ROW_SECURITY *row_security;
	char update_yn;
	FOLDER *folder;
	char *appaserver_user_foreign_login_name = {0};

	if ( !list_length( append_isa_attribute_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty append_isa_attribute_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	role =
		role_new(
			application_name,
			role_name );

	if ( ! ( folder =
			folder_load_new(
				application_name,
				session,
				folder_name,
				role ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: folder_with_load_new(%s) returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 folder_name );
		exit( 1 );
	}

	role_folder =
		role_folder_new(
			application_name,
			session,
			role_name,
			folder_name );

	if ( remove_update_permission )
		update_yn = 'n';
	else
		update_yn = role_folder->update_yn;

	row_security =
		row_security_new(
			application_name,
			folder,
			role,
			login_name,
			"update" /* state */,
			dictionary_appaserver->
				preprompt_dictionary,
			dictionary_appaserver->
				query_dictionary,
			dictionary_appaserver->
				sort_dictionary,
			(LIST *)0 /* no_display_pressed_attribute_name_list */);

	if ( !list_length( 
			row_security->
				folder->
				append_isa_attribute_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: for folder_name = %s, got empty append_isa_attribute_list\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			folder_name );
		exit( 1 );
	}

	row_security->row_security_element_list_structure =
		/* ------------------ */
		/* Never returns null */
		/* ------------------ */
		row_security_detail_structure_new(
			application_name,
			row_security->row_security_state,
			primary_attribute_data_list,
			row_security->folder,
			ignore_attribute_name_list,
			row_security->role->role_name,
			row_security->login_name,
			row_security->state,
			row_security->preprompt_dictionary,
			row_security->query_dictionary,
			row_security->sort_dictionary,
			row_security->attribute_not_null_join,
			(row_security->attribute_not_null_folder)	?
				row_security->
					attribute_not_null_folder->
					folder_name			:
				(char *)0,
			row_security->foreign_login_name_folder,
			make_primary_keys_non_edit,
			regular_omit_delete_operation,
			viewonly_omit_delete_operation,
			omit_operation_buttons,
			row_security->row_security_is_participating );

	fetched_dictionary_list =
		row_security->
			row_security_element_list_structure->
			row_dictionary_list;

	if ( !list_rewind( fetched_dictionary_list ) )
	{
		return (DICTIONARY *)0;
	}

	if ( !list_rewind( fetched_dictionary_list )
	&&   !output_even_if_not_populated )
	{
		return (DICTIONARY *)0;
	}

	form = form_new( INSERT_UPDATE_KEY,
			 application_title_string(
				application_name ) );

	form->table_border = 1;
	form_set_current_row( form, 1 );
	form->target_frame = target_frame;
	form_set_insert_update_key( form, INSERT_UPDATE_KEY );
	form->process_id = getpid();

	sprintf(action_string,
	"%s/post_edit_table_form?%s+%s+%s+%s+%s+%s+detail!%s+%s+%d+%s",
		appaserver_parameter_file_get_cgi_directory(),
		login_name, 
		application_name,
		session, 
		folder_name,
		role_name,
		"update",
		base_folder_name,
		target_frame,
		form->process_id,
		(last_related_attribute_name)
			? last_related_attribute_name
			: "" );

	form->action_string = action_string;

	form_set_folder_parameters(
		form,
		"update",
		login_name,
		application_name,
		session,
		folder_name,
		role_name );

	if ( last_related_attribute_name
	&&   *last_related_attribute_name
	&&   strcmp( last_related_attribute_name, "null" ) != 0 )
	{
		sprintf(caption_string,
			"%s (%s)",
			form->folder_name,
			last_related_attribute_name );
	}
	else
	{
		strcpy( caption_string, form->folder_name );
	}

	form->regular_element_list =
		row_security->
			row_security_element_list_structure->
			regular_element_list;

	form->viewonly_element_list =
		row_security->
			row_security_element_list_structure->
			viewonly_element_list;

	if ( attribute_exists_date_attribute(
		append_isa_attribute_list ) )
	{
		appaserver_library_output_calendar_javascript();
	}

	form_output_heading(
		form->login_name,
		form->application_name,
		form->session,
		folder_name /* form_name */,
		form->post_process,
		form->action_string,
		form->folder_name,
		form->role_name,
		form->state,
		form->insert_update_key,
		form->target_frame,
		0 /* output_submit_reset_buttons_in_heading */,
		0 /* not with_prelookup_skip_button */,
		form->submit_control_string,
		form->table_border,
		caption_string,
		form->html_help_file_anchor,
		form->process_id,
		appaserver_library_get_server_address(),
		form->optional_related_attribute_name,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		folder->post_change_javascript );

	form_output_table_heading( form->regular_element_list, *form_number );

	form->row_dictionary_list = fetched_dictionary_list;

	if ( strcmp(	row_security->folder->folder_name,
			"appaserver_user" ) == 0 )
	{
		appaserver_user_foreign_login_name = "login_name";
	}
	else
	{
		appaserver_user_foreign_login_name =
			related_folder_get_appaserver_user_foreign_login_name(
				row_security->
				folder->
				mto1_append_isa_related_folder_list );
	}

	form_output_body(
		&form->current_reference_number,
		form->hidden_name_dictionary,
		form->output_row_zero_only,
		form->row_dictionary_list,
		form->regular_element_list,
		form->viewonly_element_list,
		dictionary2file_get_filename(
			form->process_id,
			appaserver_data_directory,
			form->insert_update_key,
			folder_name,
			last_related_attribute_name ),
		folder->row_level_non_owner_view_only,
		application_name,
		login_name,
		row_security->attribute_not_null_string,
		appaserver_user_foreign_login_name,
		non_edit_folder_name_list );

	dictionary_appaserver_output_as_hidden(
		dictionary_appaserver,
		1 /* with non_prefixed_dictionary */ );

	output_submit_reset_buttons_in_trailer =
		( update_yn == 'y' ||
		  !omit_operation_buttons );

/* There's a bug. */
/* -------------- */
omit_insert_flag = 1;

	printf( "</table>\n" );
	printf( "<table border=0>\n" );

	form_output_trailer(
		output_submit_reset_buttons_in_trailer,
		1 - omit_insert_flag /* output_insert_button */,
		form->submit_control_string,
		form->html_help_file_anchor,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(char *)0 /* preprompt_button_control_string */,
		application_name,
		0 /* not with_back_to_top_button */,
		*form_number,
		(LIST *)0 /* form_button_list */,
		(char *)0 /* bugs folder->post_change_javascript */ );

	printf( "</table>\n" );
	printf( "</form>\n" );

	(*form_number)++;

	printf( "<hr>\n" );

	return (DICTIONARY *)list_get_last_pointer( fetched_dictionary_list );
}

char *detail_onload_control_string(
			FOLDER *folder,
			LIST *one2m_related_folder_list,
			LIST *mto1_related_folder_list )
{
	char post_change_javascript[ 1024 ];
	RELATED_FOLDER *related_folder;
	char control_string[ 65536 ];
	char *ptr = control_string;
	boolean got_one = 0;

	if ( folder->post_change_javascript
	&&   *folder->post_change_javascript )
	{
		strcpy(	post_change_javascript,
			folder->post_change_javascript );

		javascript_replace_state(
			post_change_javascript,
			"update" );

		javascript_replace_row(
			post_change_javascript,
			"0" );

		ptr += sprintf( ptr, "%s", post_change_javascript );
		got_one = 1;
	}

	if ( list_rewind( one2m_related_folder_list ) )
	{
		do {

			related_folder = list_get( one2m_related_folder_list );

			if ( related_folder->
				one2m_folder->
				post_change_javascript
			&&   *related_folder->
				one2m_folder->
				post_change_javascript )
			{
				strcpy(	post_change_javascript,
					related_folder->
						one2m_folder->
						post_change_javascript );
		
				javascript_replace_state(
					post_change_javascript,
					"update" );

				javascript_replace_row(
					post_change_javascript,
					"0" );

				if ( got_one )
				{
					ptr += sprintf( ptr, ";" );
				}

				ptr += sprintf(
					ptr,
					"%s",
					post_change_javascript );

				got_one = 1;
			}

		} while ( list_next( one2m_related_folder_list ) );
	}

	if ( list_rewind( mto1_related_folder_list ) )
	{
		do {

			related_folder = list_get( mto1_related_folder_list );

			if ( related_folder->
				folder->
				post_change_javascript
			&&   *related_folder->
				folder->
				post_change_javascript )
			{
				strcpy(	post_change_javascript,
					related_folder->
						folder->
						post_change_javascript );
		
				javascript_replace_state(
					post_change_javascript,
					"update" );

				javascript_replace_row(
					post_change_javascript,
					"0" );

				if ( got_one )
				{
					ptr += sprintf( ptr, ";" );
				}

				ptr += sprintf(
					ptr,
					"%s",
					post_change_javascript );

				got_one = 1;
			}

		} while ( list_next( mto1_related_folder_list ) );
	}

	return strdup( control_string );
}

void detail_set_folder_javascript_files(
			DOCUMENT *document,
			char *application_name,
			char *folder_name,
			LIST *one2m_related_folder_list,
			LIST *mto1_related_folder_list )
{
	RELATED_FOLDER *related_folder;

	document_set_folder_javascript_files(
		document,
		application_name,
		folder_name );

	if ( list_rewind( one2m_related_folder_list ) )
	{
		do {
			related_folder = list_get( one2m_related_folder_list );

			document_set_folder_javascript_files(
				document,
				application_name,
				related_folder->one2m_folder->folder_name );

		} while ( list_next( one2m_related_folder_list ) );
	}

	if ( list_rewind( mto1_related_folder_list ) )
	{
		do {

			related_folder = list_get( mto1_related_folder_list );

			document_set_folder_javascript_files(
				document,
				application_name,
				related_folder->folder->folder_name );

		} while ( list_next( mto1_related_folder_list ) );
	}
}
