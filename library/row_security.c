/* $APPASERVER_HOME/library/row_security.c		   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "row_security.h"
#include "timlib.h"
#include "piece.h"
#include "appaserver_parameter_file.h"
#include "dictionary.h"
#include "operation_list.h"
#include "role_folder.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "application.h"
#include "list.h"
#include "query.h"

ROW_SECURITY *row_security_new(
			char *application_name,
			ROLE *login_role,
			char *select_folder_name,
			char *login_name,
			char *state,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			LIST *no_display_pressed_attribute_name_list )
{
	ROW_SECURITY *row_security;

	if ( ! ( row_security =
		(ROW_SECURITY *)
			calloc( 1, sizeof( ROW_SECURITY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocation memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	row_security->login_role = login_role;
	row_security->login_name = login_name;
	row_security->state = state;
	row_security->preprompt_dictionary = preprompt_dictionary;
	row_security->query_dictionary = query_dictionary;
	row_security->sort_dictionary = sort_dictionary;
	row_security->no_display_pressed_attribute_name_list =
		no_display_pressed_attribute_name_list;

	if ( ! ( row_security->select_folder =
			folder_with_load_new(
				application_name,
				BOGUS_SESSION,
				select_folder_name,
				login_role ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load folder = %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 select_folder_name );
		exit( 1 );
	}

	/* Make sure to select the login_name attribute. */
	/* --------------------------------------------- */
	if ( row_security->select_folder->row_level_non_owner_view_only )
	{
		RELATED_FOLDER *related_folder;
		char *appaserver_user_foreign_login_name;
		ATTRIBUTE *attribute;
		LIST *exclude_permission_list;

		/* If APPASERVER_USER is marked view only. */
		/* --------------------------------------- */
		if ( strcmp(	row_security->select_folder->folder_name,
				"appaserver_user" )  == 0 )
		{
			goto non_owner_view_only_dont_append;
		}

		appaserver_user_foreign_login_name =
			related_folder_get_appaserver_user_foreign_login_name(
				row_security->
					select_folder->
					mto1_append_isa_related_folder_list );

		if ( !appaserver_user_foreign_login_name )
			goto non_owner_view_only_dont_append;

		if ( attribute_list_exists(
			row_security->select_folder->attribute_list,
			appaserver_user_foreign_login_name ) )
		{
			goto non_owner_view_only_dont_append;
		}

		/* Need to join to the folder with the login name attribute. */
		/* --------------------------------------------------------- */
		if ( !( related_folder =
			related_folder_get_view_only_related_folder(
				row_security->
				select_folder->
				mto1_append_isa_related_folder_list ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot find view only related folder.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		related_folder->folder =
			folder_with_load_new(
				application_name,
				BOGUS_SESSION,
				related_folder->folder->folder_name,
				login_role );

		row_security->foreign_login_name_folder =
			related_folder->folder;

		/* Make the added attribute viewonly. */
		/* ---------------------------------- */
		if ( ! ( attribute =
				attribute_seek_attribute(
					appaserver_user_foreign_login_name,
					related_folder->
						folder->
						attribute_list ) ) )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot find attribute = (%s)\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				appaserver_user_foreign_login_name );
			exit( 1 );
		}
	
		exclude_permission_list = list_new();
		list_append_pointer(	exclude_permission_list,
					"update" );
	
		attribute->exclude_permission_list =
			exclude_permission_list;
	
		list_append_pointer(
			row_security->
				select_folder->
				append_isa_attribute_list,
			attribute );

	} /* if row_level_non_owner_view_only */

non_owner_view_only_dont_append:

	row_security->role_update_list =
		row_security_get_role_update_list(
			application_name );

	row_security->row_security_state =
		row_security_get_row_security_state(
			&row_security->attribute_not_null_folder,
			&row_security->attribute_not_null_string,
			row_security->role_update_list,
			row_security->select_folder->folder_name,
			( row_security->
				login_role->
				override_row_restrictions_yn == 'y' ) );

	if ( row_security->row_security_state == security_supervisor
	||   row_security->row_security_state == security_user )
	{
		/* Make sure to select the attribute_not_null. */
		/* ------------------------------------------- */
		if ( !attribute_list_exists(
				row_security->
					select_folder->
					append_isa_attribute_list,
				row_security->attribute_not_null_string ) )
		{
			ATTRIBUTE *attribute;
			LIST *exclude_permission_list;

			if ( ! ( attribute =
				    attribute_seek_attribute(
					row_security->
						attribute_not_null_string,
					row_security->
						attribute_not_null_folder->
						attribute_list ) ) )
			{
				fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot find attribute = (%s)\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					 row_security->
						attribute_not_null_string );
				exit( 1 );
			}
	
			/* Make the added attribute viewonly. */
			/* ---------------------------------- */
			exclude_permission_list = list_new();
			list_append_pointer(	exclude_permission_list,
						"update" );
	
			attribute->exclude_permission_list =
				exclude_permission_list;
	
			list_append_pointer(
					row_security->
					   select_folder->
					   append_isa_attribute_list,
					attribute );
		}
	}

	return row_security;
}

ROW_SECURITY_ELEMENT_LIST_STRUCTURE *
	row_security_element_list_structure_calloc( void )
{
	ROW_SECURITY_ELEMENT_LIST_STRUCTURE *element_list_structure;

	if ( ! ( element_list_structure =
		(ROW_SECURITY_ELEMENT_LIST_STRUCTURE *)
			calloc(
			   1,
			   sizeof( ROW_SECURITY_ELEMENT_LIST_STRUCTURE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocation memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return element_list_structure;
}

ROW_SECURITY_ELEMENT_LIST_STRUCTURE *
		/* ------------------ */
		/* Never returns null */
		/* ------------------ */
		row_security_detail_element_list_structure(
			char *application_name,
			enum row_security_state row_security_state,
			char *login_name,
			char *state,
			ROLE *login_role,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			LIST *no_display_pressed_attribute_name_list,
			FOLDER *select_folder,
			FOLDER *attribute_not_null_folder,
			FOLDER *foreign_login_name_folder,
			LIST *where_clause_attribute_name_list,
			LIST *where_clause_data_list,
			LIST *non_edit_folder_name_list,
			boolean make_primary_keys_non_edit,
			enum omit_delete_operation omit_delete_operation,
			boolean omit_operation_buttons,
			char update_yn,
			boolean ajax_fill_drop_down_omit,
			LIST *append_isa_attribute_list )
{
	ROW_SECURITY_ELEMENT_LIST_STRUCTURE *element_list_structure;
	int row_dictionary_list_length;
	char query_select_folder_name[ 128 ];
	boolean prompt_data_separate_folder;

	if ( !list_length( append_isa_attribute_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty append_isa_attribute_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !select_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: select_folder is null.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	element_list_structure = row_security_element_list_structure_calloc();

	if ( row_security_state == security_supervisor
	||   row_security_state == security_user )
	{
		prompt_data_separate_folder = 0;
	}
	else
	{
		prompt_data_separate_folder = 1;
	}

	if ( attribute_not_null_folder
	&&   strcmp(	attribute_not_null_folder->folder_name,
			select_folder->folder_name ) != 0 )
	{
		sprintf( query_select_folder_name,
			 "%s,%s",
			 select_folder->folder_name,
			 attribute_not_null_folder->folder_name );
	}
	else
	{
		strcpy( query_select_folder_name,
			list_display_delimited(
				attribute_distinct_folder_name_list(
					append_isa_attribute_list ),
				',' ) );
	}

	if ( foreign_login_name_folder )
	{
		sprintf(	query_select_folder_name +
				strlen( query_select_folder_name ),
				",%s",
				foreign_login_name_folder->folder_name );
	}

	element_list_structure->row_dictionary_list =
		row_security_detail_dictionary_list(
			append_isa_attribute_list,
			application_name,
			sort_dictionary,
			login_role,
			select_folder->folder_name,
			where_clause_attribute_name_list,
			where_clause_data_list,
			select_folder->join_1tom_related_folder_list );

	row_dictionary_list_length =
		list_length( element_list_structure->
				row_dictionary_list );

	element_list_structure->regular_element_list =
		row_security_get_element_list(
			&element_list_structure->
				ajax_fill_drop_down_related_folder,
			application_name,
			select_folder,
			select_folder->mto1_append_isa_related_folder_list,
			login_role,
			no_display_pressed_attribute_name_list,
			preprompt_dictionary,
			query_dictionary,
			row_dictionary_list_length,
			state,
			non_edit_folder_name_list,
			login_name,
			update_yn,
			omit_delete_operation,
			omit_operation_buttons,
			select_folder->join_1tom_related_folder_list,
			make_primary_keys_non_edit,
			prompt_data_separate_folder );

	if ( ajax_fill_drop_down_omit )
	{
		element_list_structure->
			ajax_fill_drop_down_related_folder =
				(RELATED_FOLDER *)0;
	}

	if ( row_security_state == security_user && update_yn == 'y' )
	{
		element_list_structure->viewonly_element_list =
			row_security_get_element_list(
				(RELATED_FOLDER **)0
				     /* ajax_fill_drop_down_related_folder */,
				application_name,
				select_folder,
				select_folder->
					mto1_append_isa_related_folder_list,
				login_role,
				no_display_pressed_attribute_name_list,
				preprompt_dictionary,
				query_dictionary,
				row_dictionary_list_length,
				state,
				non_edit_folder_name_list,
				login_name,
				'n' /* update_yn */,
				omit_delete_with_placeholder,
				omit_operation_buttons,
				select_folder->join_1tom_related_folder_list,
				make_primary_keys_non_edit,
				prompt_data_separate_folder );
	}

	return element_list_structure;
}

RELATED_FOLDER *row_security_seek_related_folder(
			LIST *one2m_recursive_related_folder_list,
			char *select_folder_name )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( one2m_recursive_related_folder_list ) )
	{
		return (RELATED_FOLDER *)0;
	}

	do {
		related_folder =
			list_get_pointer(
				one2m_recursive_related_folder_list );

		if ( timlib_strcmp(
				related_folder->
					one2m_folder->
					folder_name, 
				select_folder_name ) == 0 )
		{
			return related_folder;
		}

	} while( list_next( one2m_recursive_related_folder_list ) );

	return (RELATED_FOLDER *)0;

} /* row_security_seek_related_folder() */

ROW_SECURITY_ROLE_UPDATE *row_security_role_update_new(
				char *application_name,
				char *folder_name,
				char *attribute_not_null_string )
{
	ROW_SECURITY_ROLE_UPDATE *row_security_role_update;

	if ( !attribute_not_null_string || !*attribute_not_null_string )
	{
		return (ROW_SECURITY_ROLE_UPDATE *)0;
	}

	if ( ! ( row_security_role_update =
		(ROW_SECURITY_ROLE_UPDATE *)
			calloc( 1, sizeof( ROW_SECURITY_ROLE_UPDATE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocation memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	row_security_role_update->folder =
		folder_with_load_new(
			application_name,
			BOGUS_SESSION,
			folder_name,
			(ROLE *)0 /* role */ );

	row_security_role_update->attribute_not_null_string =
		attribute_not_null_string;

	return row_security_role_update;
}

LIST *row_security_get_role_update_list(
			char *application_name )
{
	char sys_string[ 1024 ];
	char folder_name[ 128 ];
	char attribute_name[ 128 ];
	char input_buffer[ 256 ];
	char *select;
	FILE *input_pipe;
	ROW_SECURITY_ROLE_UPDATE *row_security_role_update;
	LIST *role_update_list;

	select = "folder,attribute_not_null";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=\"%s\"			"
		 "			folder=row_security_role_update	",
		 application_name,
		 select );

	input_pipe = popen( sys_string, "r" );

	role_update_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( folder_name, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		piece( attribute_name, FOLDER_DATA_DELIMITER, input_buffer, 1 );

		row_security_role_update =
			row_security_role_update_new(
				application_name,
				strdup( folder_name ),
				strdup( attribute_name ) );

		if ( row_security_role_update )
		{
			list_append_pointer(
				role_update_list,
				row_security_role_update );
		}
	}

	pclose( input_pipe );

	return role_update_list;

} /* row_security_get_role_update_list() */

LIST *row_security_get_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *application_name,
			FOLDER *select_folder,
			LIST *mto1_append_isa_related_folder_list,
			ROLE *login_role,
			LIST *no_display_pressed_attribute_name_list,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			int row_dictionary_list_length,
			char *state,
			LIST *non_edit_folder_name_list,
			char *login_name,
			char update_yn,
			enum omit_delete_operation omit_delete_operation,
			boolean omit_operation_buttons,
			LIST *join_1tom_related_folder_list,
			boolean make_primary_keys_non_edit,
			boolean prompt_data_separate_folder )
{
	LIST *ignore_attribute_name_list;
	LIST *include_attribute_name_list;
	LIST *element_list = (LIST *)0;
	OPERATION_LIST_STRUCTURE *operation_list_structure = {0};
	LIST *operation_list = {0};

	if ( omit_delete_operation == dont_omit_delete
	&&   update_yn != 'y' )
	{
		omit_delete_operation = omit_delete;
	}

	if ( !omit_operation_buttons )
	{
		operation_list_structure =
			operation_list_structure_new(
					application_name,
					BOGUS_SESSION,
					select_folder->folder_name,
					login_role->role_name,
					omit_delete_operation );

		operation_list = operation_list_structure->operation_list;
	}

	ignore_attribute_name_list = list_new_list();

	list_append_unique_string_list(
			ignore_attribute_name_list,
			no_display_pressed_attribute_name_list );

	include_attribute_name_list =
		list_subtract(
			folder_get_attribute_name_list(
				select_folder->
					append_isa_attribute_list ),
			ignore_attribute_name_list );

	element_list =
		row_security_get_update_element_list(
			ajax_fill_drop_down_related_folder,
			login_name,
			application_name,
			BOGUS_SESSION,
			select_folder->folder_name,
			login_role->role_name,
			select_folder->attribute_list,
			select_folder->append_isa_attribute_list,
			include_attribute_name_list,
			mto1_append_isa_related_folder_list,
			preprompt_dictionary,
			query_dictionary,
			operation_list,
			row_dictionary_list_length,
			no_display_pressed_attribute_name_list,
			update_yn,
			state,
			non_edit_folder_name_list,
			role_get_override_row_restrictions(
				login_role->override_row_restrictions_yn ),
			select_folder->post_change_javascript,
			application_get_max_query_rows_for_drop_downs(
				application_name ),
			select_folder->folder_name
				/* one2m_folder_name_for_processes */,
			join_1tom_related_folder_list,
			make_primary_keys_non_edit,
			prompt_data_separate_folder,
			select_folder->row_level_non_owner_forbid );

	return element_list;
}

LIST *row_security_detail_dictionary_list(
			LIST *append_isa_attribute_list,
			char *application_name,
			DICTIONARY *sort_dictionary,
			ROLE *login_role,
			char *select_folder_name,
			LIST *where_clause_attribute_name_list,
			LIST *where_clause_data_list,
			LIST *join_1tom_related_folder_list )
{
	QUERY *query;
	LIST *row_dictionary_list;

	if ( !list_length( append_isa_attribute_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty append_isa_attribute_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query = query_simple_new(
			application_name,
			select_folder_name,
			login_role,
			where_clause_attribute_name_list,
			where_clause_data_list,
			append_isa_attribute_list );

	if ( !query->query_output )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: query_simple_new() returned an empty query_output.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	query->sort_dictionary = sort_dictionary;

	if ( dictionary_length( query->sort_dictionary ) )
	{
		query->query_output->order_clause =
			query_get_order_clause(
				query->sort_dictionary,
				select_folder_name,
				append_isa_attribute_list );
	}

/*
	query->query_output->from_clause =
		list_display_delimited(
			attribute_distinct_folder_name_list(
				append_isa_attribute_list ),
			',' );
*/

	row_dictionary_list =
		query_row_dictionary_list(
			query->folder->application_name,
			query->query_output->select_clause,
			query->query_output->from_clause,
			query->query_output->where_clause,
			query->query_output->order_clause,
			query->max_rows,
			append_isa_attribute_list,
			query->login_name  );

	if ( list_length( join_1tom_related_folder_list ) )
	{
		row_security_append_join_1tom_related_folder_list(
			row_dictionary_list,
			join_1tom_related_folder_list,
			application_name,
			attribute_primary_attribute_name_list(
				append_isa_attribute_list ) );
	}

	return row_dictionary_list;
}

void row_security_append_join_1tom_related_folder_list(
			LIST *row_dictionary_list,
			LIST *join_1tom_related_folder_list,
			char *application_name,
			LIST *primary_attribute_name_list )
{
	DICTIONARY *row_dictionary;
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( row_dictionary_list ) ) return;
	if ( !list_length( join_1tom_related_folder_list ) ) return;

	do {
		row_dictionary = list_get_pointer( row_dictionary_list );

		list_rewind( join_1tom_related_folder_list );

		do {
		     related_folder =
				list_get_pointer(
					join_1tom_related_folder_list );

		     related_folder_populate_one2m_foreign_attribute_dictionary(
				row_dictionary,
				list_get_last_pointer(
					related_folder->
						folder->
						primary_attribute_name_list ),
				related_folder->related_attribute_name );

		     row_security_set_dictionary_related_folder(
				row_dictionary,
				related_folder,
				application_name,
				primary_attribute_name_list );

		} while( list_next( join_1tom_related_folder_list ) );

	} while( list_next( row_dictionary_list ) );

} /* row_security_append_join_1tom_related_folder_list() */

void row_security_set_dictionary_related_folder(
				DICTIONARY *row_dictionary,
				RELATED_FOLDER *related_folder,
				char *application_name,
				LIST *primary_attribute_name_list )
{
	char *related_data_list_string;
	char *where_clause;
	char sys_string[ 1024 ];
	LIST *select_list;

	select_list = list_subtract(
			related_folder->
				one2m_folder->
				primary_attribute_name_list,
			primary_attribute_name_list );

	where_clause =
		dictionary_get_attribute_where_clause(
			row_dictionary,
			related_folder_foreign_attribute_name_list( 
				primary_attribute_name_list,
				related_folder->related_attribute_name,
				related_folder->
					folder_foreign_attribute_name_list ) );

	if ( strcmp( where_clause, "1 = 1" ) == 0 )
	{
		where_clause =
			dictionary_get_attribute_where_clause(
				row_dictionary,
				primary_attribute_name_list );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s		 "
		 "			select=%s		 "
		 "			folder=%s		 "
		 "			where=\"%s\"		 "
		 "			order=select		|"
		 "joinlines.e '%c'				 ",
		 application_name,
		 list_display( select_list ),
		 related_folder->one2m_folder->folder_name,
		 where_clause,
		 MULTI_ATTRIBUTE_DATA_LABEL_DELIMITER );

	related_data_list_string = pipe2string( sys_string );

	if ( related_data_list_string && *related_data_list_string )
	{
		dictionary_set_pointer(
			row_dictionary,
		 	related_folder->
				one2m_folder->
				folder_name,
		 	related_data_list_string );
	}
	else
	{
		dictionary_set_pointer(
			row_dictionary,
		 	related_folder->
				one2m_folder->
				folder_name,
		 	"" );
	}
}

LIST *row_security_get_update_element_list(
			RELATED_FOLDER **ajax_fill_drop_down_related_folder,
			char *login_name,
			char *application_name,
			char *session,
			char *folder_name,
			char *role_name,
			LIST *attribute_list,
			LIST *append_isa_attribute_list,
			LIST *include_attribute_name_list,
			LIST *mto1_append_isa_related_folder_list,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			LIST *operation_list_list,
			int row_dictionary_list_length,
			LIST *no_display_pressed_attribute_name_list,
			char update_yn,
			char *state,
			LIST *non_edit_folder_name_list,
			boolean override_row_restrictions,
			char *folder_post_change_javascript,
			int max_query_rows_for_drop_downs,
			char *one2m_folder_name_for_processes,
			LIST *join_1tom_related_folder_list,
			boolean make_primary_keys_non_edit,
			boolean prompt_data_separate_folder,
			boolean row_level_non_owner_forbid )
{
	LIST *return_list;
	LIST *element_list;
	LIST *ignore_attribute_name_list;
	char *attribute_name;
	RELATED_FOLDER *related_folder;
	ELEMENT_APPASERVER *element;
	LIST *foreign_attribute_name_list = {0};
	int objects_outputted = 0;
	LIST *primary_attribute_name_list;
	boolean prompt_data_element_only;
	LIST *isa_folder_list;
	int max_drop_down_size = 0;
	boolean is_primary_attribute;
	ATTRIBUTE *attribute;

	if ( !list_length( include_attribute_name_list ) )
		return list_new_list();

	max_drop_down_size =
		application_get_max_drop_down_size(
			application_name );

	if ( row_dictionary_list_length <= MAX_QUERY_ROWS_FORCE_DROP_DOWNS )
		max_drop_down_size = INT_MAX;

	isa_folder_list =
		appaserver_get_isa_folder_list(
			application_name );

	primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			attribute_list );

	return_list = list_new_list();
	ignore_attribute_name_list = list_new();

	objects_outputted =
		appaserver_library_add_operations(
			return_list,
			objects_outputted,
			operation_list_list,
			folder_name /* delete_isa_only_folder_name */ );

	related_folder_set_no_ignore_output(
		mto1_append_isa_related_folder_list );

	/* For each attribute */
	/* ------------------ */
	list_rewind( include_attribute_name_list );

	do {
		attribute_name = 
			list_get_pointer(
				include_attribute_name_list );

		if ( list_exists_string( ignore_attribute_name_list,
					 attribute_name ) )
		{
			continue;
		}

		attribute =
			attribute_seek_attribute( 
				attribute_name,
				append_isa_attribute_list );

		if ( !attribute )
		{
			char msg[ 1024 ];

			sprintf(msg,
		"ERROR in %s/%s(): cannot find attribute = (%s) in list\n",
				__FILE__,
				__FUNCTION__,
				attribute_name );

			appaserver_output_error_message(
				application_name, msg, (char *)0 );

			exit( 1 );
		}

		if ( list_exists_string( attribute->exclude_permission_list,
					 "lookup" ) )
		{
			continue;
		}

		is_primary_attribute =
			list_exists_string(	
				primary_attribute_name_list,
				attribute_name );

		if ( is_primary_attribute && make_primary_keys_non_edit )
			prompt_data_element_only = 1;
		else
		if ( update_yn != 'y' )
			prompt_data_element_only = 1;
		else
		if ( list_exists_string( attribute->exclude_permission_list,
					 "update" ) )
			prompt_data_element_only = 1;
		else
			prompt_data_element_only = 0;

		if ( ( related_folder =
		       related_folder_attribute_consumes_related_folder(
			       &foreign_attribute_name_list,
			       ignore_attribute_name_list,
			       attribute_get_omit_update_attribute_name_list(
					attribute_list ),
			       mto1_append_isa_related_folder_list,
			       attribute_name,
			       (LIST *)0 /* include_attribute_name_list */ ) ) )
		{
			if ( list_exists_string(
					 non_edit_folder_name_list,
					 related_folder->
							folder->
							folder_name ) )
			{
				ignore_attribute_name_list =
					list_subtract_string_list(
						ignore_attribute_name_list,
						foreign_attribute_name_list );

				prompt_data_element_only = 1;
				goto skip_checking_drop_down;
			}

			if ( appaserver_exclude_permission(
				attribute->exclude_permission_list,
				"update" )
			||   attribute->omit_update )
			{
				prompt_data_element_only = 1;
			}

			if ( prompt_data_separate_folder
			&&   prompt_data_element_only )
			{
				ignore_attribute_name_list =
					list_subtract_string_list(
						ignore_attribute_name_list,
						foreign_attribute_name_list );

				goto skip_checking_drop_down;
			}

			if ( related_folder->ignore_output )
			{
				ignore_attribute_name_list =
					list_subtract_string_list(
						ignore_attribute_name_list,
						foreign_attribute_name_list );

				goto skip_checking_drop_down;
			}

			if ( isa_folder_list
			&&   list_length( isa_folder_list )
			&&   appaserver_isa_folder_accounted_for(
				isa_folder_list,
				related_folder->folder->folder_name,
				related_folder->
					related_attribute_name ) )
			{
				ignore_attribute_name_list =
					list_subtract_string_list(
						ignore_attribute_name_list,
						foreign_attribute_name_list );

				goto skip_checking_drop_down;
			}

			list_append_list(
				return_list,
				related_folder_update_element_list(
					ajax_fill_drop_down_related_folder,
					application_name,
					session,
					login_name,
					related_folder,
					foreign_attribute_name_list,
					update_yn,
					row_dictionary_list_length,
					preprompt_dictionary,
					query_dictionary,
					state,
					prompt_data_element_only,
					folder_post_change_javascript,
					max_drop_down_size,
					row_level_non_owner_forbid,
					override_row_restrictions,
					is_primary_attribute,
					role_name,
					max_query_rows_for_drop_downs,
					0 /* drop_down_multi_select */,
					related_folder->
						folder->
						no_initial_capital,
					one2m_folder_name_for_processes,
					related_folder->
						omit_lookup_before_drop_down
				) );

			related_folder->ignore_output = 1;

			objects_outputted++;
		}

skip_checking_drop_down:

		if ( !list_exists_string(	ignore_attribute_name_list,
						attribute_name ) )
		{
			element_list =
			appaserver_library_get_update_attribute_element_list(
					&objects_outputted,
					attribute,
					update_yn,
					primary_attribute_name_list,
					is_primary_attribute,
					folder_post_change_javascript,
					prompt_data_element_only );

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

	/* For each join_1tom_related_folder_list */
	/* -------------------------------------- */
	if ( list_rewind( join_1tom_related_folder_list ) )
	{
		do {
			related_folder =
				list_get_pointer(
					join_1tom_related_folder_list );

			list_append_list(
				return_list,
				related_folder_get_non_edit_multi_element_list(
					related_folder->
						one2m_folder->
						folder_name ) );

		} while( list_next( join_1tom_related_folder_list ) );
	}

	if ( no_display_pressed_attribute_name_list 
	&&   list_rewind( no_display_pressed_attribute_name_list ) )
	{
		do {
			attribute_name = list_get_string(
				no_display_pressed_attribute_name_list );

			element =
				element_appaserver_new(
					hidden,
					attribute_name );

			list_append_pointer(
					return_list, 
					element );
		} while( list_next( no_display_pressed_attribute_name_list ) );
	}

	return return_list;
}

enum row_security_state row_security_get_row_security_state(
			FOLDER **attribute_not_null_folder,
			char **attribute_not_null_string,
			LIST *role_update_list,
			char *select_folder_name,
			boolean override_role_restrictions )
{
	ROW_SECURITY_ROLE_UPDATE *role_update;
	LIST *one2m_recursive_related_folder_list;
	RELATED_FOLDER *related_folder;
	enum row_security_state row_security_state;

	*attribute_not_null_folder = (FOLDER *)0;
	*attribute_not_null_string = (char *)0;
	row_security_state = regular_user;

	if ( !list_rewind( role_update_list ) ) return row_security_state;

	do {
		role_update = list_get( role_update_list );

		if ( role_update
		&&   role_update->folder
		&&   timlib_strcmp(	role_update->folder->folder_name,
					select_folder_name ) == 0 )
		{
			*attribute_not_null_folder = role_update->folder;

			*attribute_not_null_string =
				role_update->attribute_not_null_string;

			if ( override_role_restrictions )
			{
				row_security_state = security_supervisor;
				break;
			}
			else
			{
				row_security_state = security_user;
				break;
			}
		}

		one2m_recursive_related_folder_list = (LIST *)0;

		if ( role_update
		&&   role_update->folder )
		{
			one2m_recursive_related_folder_list =
				role_update->
					folder->
					one2m_recursive_related_folder_list;
		}

		if ( !list_rewind( one2m_recursive_related_folder_list ) )
			continue;

		do {
			related_folder =
				list_get(
				  one2m_recursive_related_folder_list );

			if ( strcmp(	related_folder->
						one2m_folder->
						folder_name,
					select_folder_name ) == 0 )
			{

				/* ------------------------------------ */
				/* one2one firewall is like		*/
				/* PURCHASE_ORDER.transaction_date_time	*/
				/* ------------------------------------ */
				if ( related_folder_is_one2one_firewall(
					related_folder->
						foreign_attribute_name_list,
					related_folder->
						one2m_folder->
						attribute_list ) )
				{
					continue;
				}

				*attribute_not_null_folder =
					role_update->folder;

				*attribute_not_null_string =
					role_update->
					      attribute_not_null_string;

				if ( override_role_restrictions )
				{
					row_security_state =
						security_supervisor;
					break;
				}
				else
				{
					row_security_state = security_user;
					break;
				}
			}

		} while( list_next( one2m_recursive_related_folder_list ) );

		if ( row_security_state != regular_user ) break;

	} while( list_next( role_update_list ) );

	return row_security_state;

} /* row_security_get_row_security_state() */

boolean row_security_supervisor_logged_in(
				enum row_security_state row_security_state )
{
	return ( row_security_state == security_supervisor ||
		 row_security_state == regular_supervisor );

} /* row_security_supervisor_logged_in() */

char *row_security_role_update_list_display(
			LIST *role_update_list )
{
	char buffer[ 65536 ];
	char *ptr = buffer;
	ROW_SECURITY_ROLE_UPDATE *role_update;

	*ptr = '\0';

	if ( list_rewind( role_update_list ) )
	{
		do {
			role_update = list_get_pointer( role_update_list );

			ptr += sprintf( ptr,
					"\n"
					"folder = %s;\n"
					"attribute_not_null_string = %s.\n",
					role_update->folder->folder_name,
					role_update->
						attribute_not_null_string );

		} while( list_next( role_update_list ) );
	}

	return strdup( buffer );

} /* row_security_role_update_list_display() */

ROW_SECURITY_ROLE_UPDATE *row_security_role_update_fetch(
				LIST *role_update_list,
				char *folder_name )
{
	ROW_SECURITY_ROLE_UPDATE *role_update;

	if ( list_rewind( role_update_list ) )
	{
		do {
			role_update = list_get_pointer( role_update_list );

			if ( role_update->folder
			&&   timlib_strcmp(
					role_update->
						folder->
						folder_name,
					folder_name ) == 0 )
			{

				return role_update;
			}

		} while( list_next( role_update_list ) );
	}

	return (ROW_SECURITY_ROLE_UPDATE *)0;
}

ROW_SECURITY_ELEMENT_LIST_STRUCTURE *
		row_security_edit_table_structure_new(
			char *application_name,
			enum row_security_state row_security_state,
			char *login_name,
			char *state,
			ROLE *login_role,
			DICTIONARY *preprompt_dictionary,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			LIST *no_display_pressed_attribute_name_list,
			FOLDER *select_folder,
			FOLDER *attribute_not_null_folder,
			FOLDER *foreign_login_name_folder,
			LIST *non_edit_folder_name_list,
			boolean make_primary_keys_non_edit,
			enum omit_delete_operation omit_delete_operation,
			boolean omit_operation_buttons,
			char update_yn,
			boolean ajax_fill_drop_down_omit,
			LIST *append_isa_attribute_list )
{
	ROW_SECURITY_ELEMENT_LIST_STRUCTURE *element_list_structure;
	int row_dictionary_list_length;
	char query_select_folder_name[ 512 ];
	char folder_name[ 128 ];
	boolean prompt_data_separate_folder;

	if ( !list_length( append_isa_attribute_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty append_isa_attribute_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !select_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: select_folder is null.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	element_list_structure = row_security_element_list_structure_calloc();

	if ( row_security_state == security_supervisor
	||   row_security_state == security_user )
	{
		prompt_data_separate_folder = 0;
	}
	else
	{
		prompt_data_separate_folder = 1;
	}

	strcpy(	query_select_folder_name, select_folder->folder_name );

	/* -------------------------------------------------- */
	/* Note: these many folders aren't being implemented. */
	/* -------------------------------------------------- */
	if ( attribute_not_null_folder
	&&   strcmp(	attribute_not_null_folder->folder_name,
			select_folder->folder_name ) != 0 )
	{
		sprintf( query_select_folder_name,
			 "%s,%s",
			 select_folder->folder_name,
			 attribute_not_null_folder->folder_name );
	}
	else
	{
		strcpy( query_select_folder_name,
			list_display_delimited(
				attribute_distinct_folder_name_list(
					append_isa_attribute_list ),
				',' ) );
	}

	if ( foreign_login_name_folder )
	{
		sprintf(	query_select_folder_name +
				strlen( query_select_folder_name ),
				",%s",
				foreign_login_name_folder->folder_name );
	}

	element_list_structure->row_dictionary_list =
		row_security_edit_table_dictionary_list(
			application_name,
			query_dictionary,
			sort_dictionary,
			login_role,
			login_name,
			/* -------------------------------------------------- */
			/* Note: these many folders aren't being implemented. */
			/* -------------------------------------------------- */
			piece(	folder_name,
				',',
				query_select_folder_name,
				0 ),
			select_folder->join_1tom_related_folder_list );

	row_dictionary_list_length =
		list_length( element_list_structure->
				row_dictionary_list );

	element_list_structure->regular_element_list =
		row_security_get_element_list(
			&element_list_structure->
				ajax_fill_drop_down_related_folder,
			application_name,
			select_folder,
			select_folder->mto1_append_isa_related_folder_list,
			login_role,
			no_display_pressed_attribute_name_list,
			preprompt_dictionary,
			query_dictionary,
			row_dictionary_list_length,
			state,
			non_edit_folder_name_list,
			login_name,
			update_yn,
			omit_delete_operation,
			omit_operation_buttons,
			select_folder->join_1tom_related_folder_list,
			make_primary_keys_non_edit,
			prompt_data_separate_folder );

	if ( ajax_fill_drop_down_omit )
	{
		element_list_structure->
			ajax_fill_drop_down_related_folder =
				(RELATED_FOLDER *)0;
	}

	if ( row_security_state == security_user && update_yn == 'y' )
	{
		element_list_structure->viewonly_element_list =
			row_security_get_element_list(
				(RELATED_FOLDER **)0
				     /* ajax_fill_drop_down_related_folder */,
				application_name,
				select_folder,
				select_folder->
					mto1_append_isa_related_folder_list,
				login_role,
				no_display_pressed_attribute_name_list,
				preprompt_dictionary,
				query_dictionary,
				row_dictionary_list_length,
				state,
				non_edit_folder_name_list,
				login_name,
				'n' /* update_yn */,
				omit_delete_with_placeholder,
				omit_operation_buttons,
				select_folder->join_1tom_related_folder_list,
				make_primary_keys_non_edit,
				prompt_data_separate_folder );
	}

	return element_list_structure;
}

LIST *row_security_edit_table_dictionary_list(
			char *application_name,
			DICTIONARY *query_dictionary,
			DICTIONARY *sort_dictionary,
			ROLE *login_role,
			char *login_name,
			char *select_folder_name,
			LIST *join_1tom_related_folder_list )
{
	QUERY *query;
	LIST *row_dictionary_list;

/*
{
char msg[ 65536 ];
sprintf( msg, "%s/%s()/%d: select_folder_name = [%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
select_folder_name );
m2( application_name, msg );
}

{
char msg[ 65536 ];
sprintf( msg, "%s/%s()/%d: edit_table query_dictionary = [%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
dictionary_display( query_dictionary ) );
m2( application_name, msg );
}
*/
	query =
		query_edit_table_new(
			query_dictionary,
			application_name,
			login_name,
			select_folder_name,
			login_role );

/*
{
char msg[ 65536 ];
sprintf( msg, "%s/%s()/%d: where_clause = [%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
query->query_output->where_clause );
m2( application_name, msg );
}
*/

	query->sort_dictionary = sort_dictionary;

	if ( dictionary_length( query->sort_dictionary ) )
	{
		query->query_output->order_clause =
			query_get_order_clause(
				query->sort_dictionary,
				select_folder_name,
				query->folder->append_isa_attribute_list );
	}

	query->query_output->from_clause =
		list_display_delimited(
			attribute_distinct_folder_name_list(
				query->folder->append_isa_attribute_list ),
			',' );

	row_dictionary_list =
		query_edit_table_dictionary_list(
			query->folder->application_name,
			query->query_output->select_clause,
			query->query_output->from_clause,
			query->query_output->where_clause,
			query->query_output->order_clause,
			query->max_rows,
			query->folder->append_isa_attribute_list,
			query->login_name  );

	if ( list_length( join_1tom_related_folder_list ) )
	{
		row_security_append_join_1tom_related_folder_list(
			row_dictionary_list,
			join_1tom_related_folder_list,
			application_name,
			attribute_get_primary_attribute_name_list(
				query->folder->append_isa_attribute_list ) );
	}

	return row_dictionary_list;
}
