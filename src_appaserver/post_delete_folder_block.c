/* $APPASERVER_HOME/src_appaserver/post_delete_folder_block.c	*/
/* ------------------------------------------------------------	*/
/* This process is triggered if you select the delete radio	*/
/* button on some of the lookup forms.				*/
/*								*/
/* Freely available software: see Appaserver.org		*/
/* -----------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "form.h"
#include "folder.h"
#include "session.h"
#include "related_folder.h"
#include "appaserver.h"
#include "operation_list.h"
#include "document.h"
#include "application.h"
#include "appaserver_user.h"
#include "dictionary.h"
#include "query.h"
#include "query_attribute_statistics_list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "post2dictionary.h"
#include "environ.h"
#include "decode_html_post.h"
#include "role.h"
#include "role_folder.h"
#include "dictionary_appaserver.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void delete_folder_block_state_one(
				DICTIONARY_APPASERVER *dictionary_appaserver,
				char *application_name,
				FOLDER *folder,
				char *where_clause,
				char *login_name,
				char *session,
				char *role_name );

void delete_folder_block_state_two(
				char *application_name,
				FOLDER *folder,
				char *where_clause,
				char *session,
				char *login_name,
				char *role_name );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session, *folder_name;
	DOCUMENT *document;
	char decoded_dictionary_string[ MAX_INPUT_LINE ];
	char *dictionary_string;
	DICTIONARY *post_dictionary;
	FOLDER *folder;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *where_clause;
	ROLE *role;
	ROLE_FOLDER *role_folder;
	char *role_name;
	char *database_string = {0};
	char *state;
	DICTIONARY_APPASERVER *dictionary_appaserver;
	QUERY *query;

	if ( argc < 7 )
	{
		fprintf( stderr, 
"Usage: %s login_name application session folder role state [dictionary]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	application_name = argv[ 2 ];

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

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	session = argv[ 3 ];
	folder_name = argv[ 4 ];
	role_name = argv[ 5 ];
	state = argv[ 6 ];

	environ_prepend_dot_to_path();
	add_utility_to_path();
	add_relative_source_directory_to_path( application_name );
	environ_appaserver_home();

	appaserver_parameter_file = appaserver_parameter_file_new();

	if ( argc == 8 )
	{
		dictionary_string = argv[ 7 ];

		decode_html_post(	decoded_dictionary_string, 
					dictionary_string );

		post_dictionary = 
			dictionary_index_string2dictionary( 
				decoded_dictionary_string );
	}
	else
	{
		post_dictionary =
			post2dictionary(
				stdin,
				appaserver_parameter_file->
					appaserver_data_directory,
				session );
	}

	if ( ! ( dictionary_appaserver =
			dictionary_appaserver_new(
				post_dictionary,
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

	post_dictionary = dictionary_appaserver->working_post_dictionary;

	role_folder = role_folder_new_role_folder(
					application_name,
					session,
					role_name,
					folder_name );

	if ( session_remote_ip_address_changed(
		application_name,
		session ) )
	{
		session_message_ip_address_changed_exit(
				application_name,
				login_name );
	}

	if ( role_folder->delete_yn != 'y'
	||   strcmp(	login_name,
			session_get_login_name(
					application_name,
					session ) ) != 0 )
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

	folder = folder_new_folder( 	application_name,
					session,
					folder_name );

	role = role_new_role(	application_name,
				role_name );

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
			role_get_override_row_restrictions(
				role->override_row_restrictions_yn ),
			role_name,
			(LIST *)0 /* mto1_related_folder_list */ );

	folder->attribute_list =
		attribute_get_attribute_list(
		folder->application_name,
		folder->folder_name,
		(char *)0 /* attribute_name */,
		(LIST *)0 /* mto1_isa_related_folder_list */,
		(char *)0 /* role_name */ );

	if ( folder->row_level_non_owner_view_only )
		folder->row_level_non_owner_forbid = 1;

	query = query_edit_table_new(
			dictionary_appaserver->query_dictionary,
			application_name,
			login_name,
			folder_name,
			role_new( application_name, role_name ) );

	where_clause = query->query_output->where_clause;

	document = document_new( "", application_name );
	document->output_content_type = 1;

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	if ( strcmp( state, "one" ) == 0 )
	{
		delete_folder_block_state_one(
				dictionary_appaserver,
				application_name,
				folder,
				where_clause,
				login_name,
				session,
				role_name );
	}
	else
	if ( strcmp( state, "two" ) == 0 )
	{
		delete_folder_block_state_two(
				application_name,
				folder,
				where_clause,
				session,
				login_name,
				role_name );
		printf( "<h3>Delete Complete</h3\n" );
	}
	else
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: invalid state = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 state );
		exit( 1 );
	}

	document_close();

	exit( 0 );
} /* main() */

void delete_folder_block_state_one(
				DICTIONARY_APPASERVER *dictionary_appaserver,
				char *application_name,
				FOLDER *folder,
				char *where_clause,
				char *login_name,
				char *session,
				char *role_name )
{
	QUERY_ATTRIBUTE_STATISTICS_LIST *query_attribute_statistics_list;
	char *query_folder_name;
	char *sys_string;
	FORM *form;
	char action_string[ 512 ];

	query_folder_name = folder->folder_name;

	query_attribute_statistics_list =
			query_attribute_statistics_list_new(
					folder->application_name,
					query_folder_name );

	query_attribute_statistics_list->list =
		query_attribute_statistics_list_get_primary_list(
					folder->attribute_list );

	sys_string =
	query_attribute_statistics_list_get_build_each_temp_file_sys_string(
			query_attribute_statistics_list->application_name,
			query_attribute_statistics_list->folder_name,
			query_attribute_statistics_list->list,
			where_clause );

	if ( !sys_string )
	{
		printf(
	"<p>ERROR at %s/%s()/%d: An internal error occurred. Exiting early.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		document_close();
		return;
	}

	if ( system( sys_string ) ){};

	query_attribute_statistics_list_populate_list(
					query_attribute_statistics_list->list );

	query_attribute_statistics_list_output_folder_count(
			application_name,
			query_attribute_statistics_list->folder_name,
			where_clause,
			query_attribute_statistics_list->list );

	query_attribute_statistics_remove_temp_file(
					query_attribute_statistics_list->list );

	form = form_new( "delete",
			 application_get_title_string( application_name ) );

	sprintf(	action_string,
			"%s/post_delete_folder_block?%s+%s+%s+%s+%s+two",
			appaserver_parameter_file_get_cgi_directory(),
			login_name,
			application_name,
			session,
			folder->folder_name,
			role_name );

	form->action_string = action_string;

	form_output_heading(
		form->login_name,
		form->application_name,
		form->session,
		form->form_name,
		form->post_process,
		form->action_string,
		form->folder_name,
		form->role_name,
		form->state,
		form->insert_update_key,
		form->target_frame,
		0 /* not output_submit_reset_buttons_in_heading */,
		0 /* not with_prelookup_skip_button */,
		form->submit_control_string,
		form->table_border,
		(char *)0 /* caption_string */,
		form->html_help_file_anchor,
		form->process_id,
		appaserver_library_get_server_address(),
		form->optional_related_attribute_name,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(LIST *)0 /* form_button_list */,
		(char *)0 /* post_change_javascript */ );

	dictionary_appaserver->send_dictionary =
		dictionary_appaserver_get_send_dictionary(
			dictionary_appaserver->sort_dictionary,
			dictionary_appaserver->query_dictionary,
			dictionary_appaserver->preprompt_dictionary,
			dictionary_appaserver->
				lookup_before_drop_down_dictionary,
			dictionary_appaserver->ignore_dictionary,
			dictionary_appaserver->pair_1tom_dictionary,
			(DICTIONARY *)0 /* non_prefixed_dictionary */ );

	output_dictionary_as_hidden( dictionary_appaserver->send_dictionary );

	form_output_submit_button(
			(char *)0 /* submit_control_string */,
			"Delete",
			0 /* form_number */,
			(LIST *)0 /* pair_one2m_related_folder_name_list */ );

} /* delete_folder_block_state_one() */

void delete_folder_block_state_two(
				char *application_name,
				FOLDER *folder,
				char *where_clause,
				char *session,
				char *login_name,
				char *role_name )
{
	LIST *primary_attribute_name_list;
	char *table_name;
	char sys_string[ 1024 ];
	char where_clause_construct[ 1024 ];

	primary_attribute_name_list =
		attribute_get_primary_attribute_name_list(
			folder->attribute_list );

	table_name = get_table_name( application_name, folder->folder_name );

	if ( where_clause && *where_clause )
		sprintf( where_clause_construct, "where %s", where_clause );
	else
		*where_clause_construct = '\0';

	sprintf( sys_string,
		 "echo \"select %s from %s %s;\"			|"
		 "sql.e '^'						|"
		 "delete_folder_row %s %s %s %s stdin sql.e n 2>>%s	 ",
		 list_display_delimited( primary_attribute_name_list, ',' ),
		 table_name,
		 where_clause_construct,
		 session,
		 login_name,
		 folder->folder_name,
		 role_name,
		 appaserver_error_get_filename( application_name ) );

	if ( system( sys_string ) ){};

} /* delete_folder_block_state_two() */

