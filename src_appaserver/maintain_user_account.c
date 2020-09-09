/* $APPASERVER_HOME/src_appaserver/maintain_user_account.c		*/
/* -----------------------------------------------------------------	*/
/*									*/
/* This is the form that allows users to maintain their own account.	*/
/* Freely available software: see Appaserver.org			*/
/* -----------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "list_usage.h"
#include "form.h"
#include "folder.h"
#include "related_folder.h"
#include "appaserver.h"
#include "document.h"
#include "application.h"
#include "dictionary.h"
#include "query.h"
#include "piece.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "environ.h"
#include "appaserver_parameter_file.h"
#include "dictionary2file.h"
#include "attribute.h"
#include "session.h"

/* Constants */
/* --------- */
#define PROCESS_NAME			"change_password"
#define TARGET_FRAME			PROMPT_FRAME
#define INSERT_UPDATE_KEY		"edit"
#define FOLDER_NAME			"appaserver_user"

/* Prototypes */
/* ---------- */

LIST *get_attribute_element_list(	int *objects_outputted,
					LIST *attribute_list,
					char *attribute_name,
					LIST *primary_attribute_name_list );

char *get_order_clause(			DICTIONARY *query_dictionary,
					DICTIONARY *sort_dictionary );

LIST *get_element_list(		char *login_name,
				char *application_name,
				char *session,
				char *role_name,
				LIST *attribute_list,
				LIST *include_attribute_name_list,
				LIST *mto1_related_folder_list,
				DICTIONARY *query_dictionary,
				int row_dictionary_list_length,
				char *state,
				boolean row_level_non_owner_forbid );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session_key, *folder_name;
	char *role_name;
	char *message = {0};
	char *state = "update";
	char *insert_update_key = INSERT_UPDATE_KEY;
	char *target_frame;
	DOCUMENT *document;
	DICTIONARY *query_dictionary = {0};
	FORM *form;
	FOLDER *folder;
	LIST *row_dictionary_list = {0};
	int number_rows_outputted = 0;
	int row_dictionary_list_length = 0;
	LIST *ignore_attribute_name_list;
	LIST *mto1_related_folder_list = {0};
	LIST *include_attribute_name_list;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	int output_submit_reset_buttons_in_trailer = 1;
	int output_submit_reset_buttons_in_heading = 0;
	SESSION *session;
	char key[ 128 ];
	char action_string[ 512 ];
	QUERY *query;

	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 5 )
	{
		fprintf( stderr,
		"Usage: %s ignored session login_name role [message]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	session_key = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];

	if ( argc == 6 ) message = argv[ 5 ];

	folder_name = FOLDER_NAME;
	target_frame = TARGET_FRAME;

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

	appaserver_parameter_file = appaserver_parameter_file_new();

	query_dictionary = dictionary_new_dictionary();

	sprintf(key,
		"%s%s",
		NO_DISPLAY_PUSH_BUTTON_PREFIX,
		"login_name" );

	dictionary_set_string_index_key(query_dictionary,
					key,
					"yes",
					0 );

	sprintf(key,
		"%s%s",
		QUERY_RELATION_OPERATOR_STARTING_LABEL,
		"login_name" );

	dictionary_set_string_index_key(query_dictionary,
					key,
					EQUAL_OPERATOR,
					0 );

	/* Set login_name to dictionary */
	/* ---------------------------- */
	sprintf(key,
		"%s%s",
		QUERY_FROM_STARTING_LABEL,
		"login_name" );

	dictionary_set_string_index_key(query_dictionary,
					key,
					login_name,
					0 );

	/* Set role_name to dictionary */
	/* --------------------------- */
	sprintf(key,
		"%s%s",
		QUERY_FROM_STARTING_LABEL,
		"role_name" );

	dictionary_set_string_index_key(query_dictionary,
					key,
					role_name,
					0 );

	/* Set lookup option radio button to lookup in dictionary */
	/* ------------------------------------------------------ */
	sprintf(key,
		"%s",
		LOOKUP_OPTION_RADIO_BUTTON_NAME );

	dictionary_set_string(		query_dictionary,
					key,
					"lookup" );

	ignore_attribute_name_list = list_new();
	list_append_pointer(	ignore_attribute_name_list,
				"login_name" );

	appaserver_parameter_file = new_appaserver_parameter_file();

	folder = folder_new_folder( 	application_name,
					session_key,
					folder_name );

	folder->attribute_list =
		attribute_get_attribute_list(
			folder->application_name,
			folder->folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			(char *)0
			/* role_name: leave blank to select password */ );

	form = form_new( folder_name,
			 application_get_title_string(
				application_name ) );

	form_set_current_row( form, 1 );
	form_set_target_frame( form, target_frame );

	document = document_new( insert_update_key, application_name );

	document_set_javascript_module( document, "timlib" );
	document_set_javascript_module( document, "trim" );
	document_set_javascript_module( document, "null2slash" );
	document_set_javascript_module( document, "push_button_submit" );

	if ( !appaserver_frameset_menu_horizontal(
					application_name,
					login_name ) )
	{
		document->output_content_type = 1;
	}

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

	form_set_folder_parameters(	form,
					state,
					login_name,
					application_name,
					session_key,
					folder->folder_name,
					role_name );

	form_output_title(	form->application_title,
				form->state,
				form->form_title,
				form->folder_name,
				form->target_frame,
				form->subtitle_string,
				0 /* not omit_format_initial_capital */ );

	if ( message && *message ) printf( "<h3>%s</h3>\n", message );

	form->table_border = 1;
	form->insert_update_key = insert_update_key;
	form->target_frame = target_frame;
	form->process_id = getpid();

	sprintf(	action_string,
			"post_maintain_user_account?%s+%s+%s+%s+%s+%d",
			login_name,
			role_name,
			application_name,
			session_key,
			target_frame,
			form->process_id );

	form->action_string = strdup( action_string );

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
		output_submit_reset_buttons_in_heading,
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

	query = query_edit_table_new(
			query_dictionary,
			application_name,
			login_name,
			folder_name,
			role_new( application_name, role_name ) );

	attribute_list_remove_exclude_permission_list(
		query->folder->append_isa_attribute_list );

	query->query_output->select_clause =
		list_display(
			attribute_get_attribute_name_list(
				query->
					folder->
					append_isa_attribute_list ) );

	row_dictionary_list =
		query_row_dictionary_list(
			query->folder->application_name,
			query->query_output->select_clause,
			query->query_output->from_clause,
			query->query_output->where_clause,
			query->query_output->order_clause,
			query->max_rows,
			query->folder->append_isa_attribute_list,
			query->login_name );

	row_dictionary_list_length =
		list_length( row_dictionary_list );

	include_attribute_name_list =
		list_subtract(
			folder_get_attribute_name_list(
				folder->attribute_list ),
				ignore_attribute_name_list );

	mto1_related_folder_list =
		related_folder_get_mto1_related_folder_list(
			list_new_list(),
			application_name,
			session_key,
			folder_name,
			role_name,
			0 /* isa_flag */,
			related_folder_no_recursive,
			0 /* dont override_row_restrictions */,
			(LIST *)0 /* root_primary_attribute_name_list */,
			0 /* recursive_level */ );

	form->regular_element_list =
		get_element_list(
			login_name,
			application_name,
			session_key,
			role_name,
			folder->attribute_list,
			include_attribute_name_list,
			mto1_related_folder_list,
			query_dictionary,
			row_dictionary_list_length,
			state,
			folder->row_level_non_owner_forbid );

	form_output_table_heading(	form->regular_element_list,
					0 /* form_number */ );

	form->row_dictionary_list = row_dictionary_list;

	number_rows_outputted += form_output_body(
		&form->current_reference_number,
		form->hidden_name_dictionary,
		form->output_row_zero_only,
		form->row_dictionary_list,
		form->regular_element_list,
		form->viewonly_element_list,
		dictionary2file_get_filename(
			form->process_id,
			appaserver_parameter_file->
				appaserver_data_directory,
			insert_update_key,
			folder->folder_name,
			(char *)0 /* optional_related_attribute_name */ ),
		0 /* row_level_non_owner_view_only */,
		application_name,
		login_name,
		(char *)0 /* attribute_not_null */,
		(char *)0 /* appaserver_user_foreign_login_name */ );

	query_dictionary = dictionary_prepend_key(
				query_dictionary,
				QUERY_STARTING_LABEL );

	output_dictionary_as_hidden( query_dictionary );

	form_output_trailer(
		output_submit_reset_buttons_in_trailer,
		0 /* output_insert_flag */,
		form->submit_control_string,
		form->html_help_file_anchor,
		(char *)0 /* remember_keystrokes_onload_control_string */,
		(char *)0 /* preprompt_button_control_string */,
		application_name,
		0 /* not with_back_to_top_button */,
		0 /* form_number */,
		(LIST *)0 /* form_button_list */,
		(char *)0 /* post_change_javascript */ );

	document_close();

	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

LIST *get_element_list(		char *login_name,
				char *application_name,
				char *session_key,
				char *role_name,
				LIST *attribute_list,
				LIST *include_attribute_name_list,
				LIST *mto1_related_folder_list,
				DICTIONARY *query_dictionary,
				int row_dictionary_list_length,
				char *state,
				boolean row_level_non_owner_forbid )
{
	LIST *return_list;
	LIST *element_list;
	LIST *ignore_attribute_name_list;
	char *attribute_name;
	RELATED_FOLDER *related_folder;
	LIST *foreign_attribute_name_list = {0};
	int objects_outputted = 0;
	LIST *primary_attribute_name_list;
	int prompt_data_element_only = 0;
	int max_drop_down_size;
	boolean override_row_restrictions = 0;

	if ( !list_reset( include_attribute_name_list ) )
		return list_new_list();

	max_drop_down_size =
		application_get_max_drop_down_size(
			application_name );

	primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			attribute_list );

	return_list = list_new();
	ignore_attribute_name_list = list_new();

	/* For each attribute */
	/* ------------------ */
	do {
		attribute_name = 
			list_get_string( include_attribute_name_list );

		/* If the attribute is accounted for already */
		/* ----------------------------------------- */
		if ( list_exists_string( ignore_attribute_name_list,
					 attribute_name ) )
		{
			continue;
		}

		if ( ( related_folder =
		       related_folder_attribute_consumes_related_folder(
			       &foreign_attribute_name_list,
			       ignore_attribute_name_list,
			       (LIST *)0 /* omit_update_attribute_name_list */,
			       mto1_related_folder_list,
			       attribute_name,
			       (LIST *)0 /* include_attribute_name_list */ ) ) )
		{
			if ( related_folder->ignore_output ) continue;

			list_append_list(
				return_list,
				related_folder_update_element_list(
					(RELATED_FOLDER **)0,
					application_name,
					session_key,
					login_name,
					related_folder,
					foreign_attribute_name_list,
					'y' /* update_yn */,
					row_dictionary_list_length,
					query_dictionary /* preprompt */,
					(DICTIONARY *)0 /* query */,
					state,
					prompt_data_element_only,
					(char *)0 /* post_cha... */,
					max_drop_down_size,
					row_level_non_owner_forbid,
					override_row_restrictions,
					list_exists_string(	
						primary_attribute_name_list,
						attribute_name ),
					role_name,
				application_get_max_query_rows_for_drop_downs(
						application_name ),
					related_folder->
						drop_down_multi_select,
					related_folder->
						folder->
						no_initial_capital,
					(char *)0
					/* one2m_folder_name_for_processes */,
					related_folder->
						omit_lookup_before_drop_down
				) );

			related_folder->ignore_output = 1;
			objects_outputted++;

		}

		if ( !list_exists_string(	ignore_attribute_name_list,
						attribute_name ) )
		{
			element_list =
				get_attribute_element_list(
						&objects_outputted,
						attribute_list,
						attribute_name,
						primary_attribute_name_list );

			if ( element_list )
			{
				list_append_list(
					return_list,
					element_list );

				list_append_pointer(
					ignore_attribute_name_list,
					attribute_name );
			}
		}

	} while( list_next( include_attribute_name_list ) );

	return return_list;

} /* get_element_list() */

LIST *get_attribute_element_list(	int *objects_outputted,
					LIST *attribute_list,
					char *attribute_name,
					LIST *primary_attribute_name_list )
{
	ATTRIBUTE *attribute;
	LIST *element_list;
	LIST *return_list;
	
	attribute = attribute_seek_attribute( 
					attribute_list,
					attribute_name );
	
	if ( !attribute )
	{
		fprintf(stderr,
		"%s.%s() cannot find attribute = (%s)\n",
			__FILE__,
			__FUNCTION__,
			attribute_name );
		exit( 1 );
	}

	return_list = list_new();

	element_list =
		appaserver_library_get_update_lookup_attribute_element_list(
				'y' /* update_yn */,
				primary_attribute_name_list,
				attribute->exclude_permission_list,
				attribute->attribute_name,
				attribute->datatype,
				attribute->width,
				attribute->post_change_javascript,
				attribute->on_focus_javascript_function,
				list_exists_string(	
						primary_attribute_name_list,
						attribute->attribute_name ) );


	if ( element_list && list_length( element_list ) )
	{
		list_append_list( return_list, element_list );
		(*objects_outputted)++;
	}

	return return_list;
} /* get_attribute_element_list() */

