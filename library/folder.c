/* $APPASERVER_LIBRARY/library/folder.c					*/
/* -------------------------------------------------------------------- */
/* This is the appaserver folder ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "folder.h"
#include "query.h"
#include "related_folder.h"
#include "dictionary.h"
#include "appaserver_error.h"
#include "list.h"
#include "list_usage.h"
#include "piece.h"
#include "attribute.h"
#include "timlib.h"
#include "insert_database.h"
#include "process.h"
#include "appaserver_library.h"
#include "appaserver.h"
#include "appaserver_parameter_file.h"
#include "lookup_before_drop_down.h"
#include "attribute.h"

FOLDER *folder_new(	char *application_name,
			char *session,
			char *folder_name )
{
	return folder_new_folder(	application_name,
					session,
					folder_name );
}

FOLDER *folder_folder_new(	char *application_name,
				char *session,
				char *folder_name )
{
	return folder_new_folder(	application_name,
					session,
					folder_name );
}

FOLDER *folder_calloc( void )
{
	FOLDER *f;

	if ( ! ( f = (FOLDER *)calloc( 1, sizeof( FOLDER ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	return f;
}

FOLDER *folder_new_folder(	char *application_name,
				char *session,
				char *folder_name )
{
	FOLDER *f;

	if ( folder_name && strcmp( folder_name, "null" ) == 0 )
	{
		fprintf( stderr,
			 "Warning in %s/%s()/%d: empty folder_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (FOLDER *)0;
	}


	f = folder_calloc();

	f->application_name = application_name;
	f->session = session;
	f->folder_name = folder_name;

	return f;

} /* folder_new_folder() */

FOLDER *folder_with_load_new(	char *application_name,
				char *session,
				char *folder_name,
				ROLE *role )
{
	FOLDER *folder;
	char *role_name = {0};
	char override_row_restrictions_yn = {0};

	folder = folder_new(	application_name,
				session,
				folder_name );

	if ( !folder ) return (FOLDER *)0;

	if ( role )
	{
		role_name = role->role_name;

		override_row_restrictions_yn =
			role->override_row_restrictions_yn;
	}

	folder->mto1_recursive_related_folder_list =
	     related_folder_get_mto1_related_folder_list(
		list_new(),
		application_name,
		BOGUS_SESSION,
		folder_name,
		role_name,
		0 /* isa_flag */,
		related_folder_recursive_all,
		0 /* dont override_row_restrictions */,
		(LIST *)0 /* root_primary_attribute_name_list */,
		0 /* recursive_level */ );

	folder->mto1_related_folder_list =
	     related_folder_get_mto1_related_folder_list(
		list_new(),
		application_name,
		BOGUS_SESSION,
		folder_name,
		role_name,
		0 /* isa_flag */,
		related_folder_no_recursive,
		0 /* dont override_row_restrictions */,
		(LIST *)0 /* root_primary_attribute_name_list */,
		0 /* recursive_level */ );

	folder_append_mto1_related_folder_list(
		folder->mto1_related_folder_list,
		application_name );

	if ( !folder_load(
			&folder->insert_rows_number,
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
			folder->application_name,
			folder->session,
			folder->folder_name,
			role_get_override_row_restrictions(
				override_row_restrictions_yn ),
			role_name,
			folder->mto1_related_folder_list ) )
	{
		fprintf( stderr,
			 "Warning in %s/%s()/%d: cannot load folder=%s\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 folder->folder_name );

		return (FOLDER *)0;
	}

	/* This is the new way of setting. */
	/* ------------------------------- */
	folder->mto1_isa_related_folder_list =
		folder_mto1_isa_related_folder_list(
			list_new() /* existing_related_folder_list */,
			application_name,
			folder_name,
			role_name,
			0 /* recursive_level */ );

	folder->append_isa_attribute_list =
		attribute_append_isa_attribute_list(
			folder->application_name,
			folder->folder_name,
			folder->mto1_isa_related_folder_list,
			role_name );

	folder_append_one2m_related_folder_list(
		folder->mto1_isa_related_folder_list,
		application_name );

	folder_append_one2m_related_folder_list(
		folder->mto1_recursive_related_folder_list,
		application_name );

	if ( ! ( folder->mto1_append_isa_related_folder_list = 
		folder_append_isa_mto1_related_folder_list(
			application_name,
			BOGUS_SESSION,
			role_name,
			role_get_override_row_restrictions(
				override_row_restrictions_yn ),
			folder->mto1_isa_related_folder_list ) ) )
	{
		folder->mto1_append_isa_related_folder_list = list_new();
	}

	related_folder_mto1_append_unique(
			folder->mto1_append_isa_related_folder_list,
			folder->mto1_related_folder_list );

	folder->attribute_list =
		attribute_get_attribute_list(
			folder->application_name,
			folder->folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			role_name );

	folder->primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			folder->attribute_list );

	folder->pair_one2m_related_folder_list =
		related_folder_get_pair_one2m_related_folder_list(
				folder->application_name,
				folder->folder_name,
				role_name );

	folder->one2m_related_folder_list =
		related_folder_1tom_related_folder_list(
			application_name,
			BOGUS_SESSION,
			folder->folder_name,
			role_name,
			update,
			(LIST *)0 /* primary_data_list */,
			list_new() /* related_folder_list */,
			0 /* dont omit_isa_relations */,
			related_folder_no_recursive,
			(LIST *)0 /* parent_primary_attribute_name_list */,
			(LIST *)0 /* original_primary_attribute_name_list */,
			(char *)0 /* prior_related_attribute_name */ );

	folder->one2m_recursive_related_folder_list =
		related_folder_1tom_related_folder_list(
			application_name,
			BOGUS_SESSION,
			folder->folder_name,
			role_name,
			update,
			(LIST *)0 /* primary_data_list */,
			list_new() /* related_folder_list */,
			0 /* dont omit_isa_relations */,
			related_folder_recursive_all,
			folder->primary_attribute_name_list
				/* parent_primary_attribute_name_list */,
			folder->primary_attribute_name_list
				/* original_primary_attribute_name_list */,
			(char *)0 /* prior_related_attribute_name */ );

	folder->join_1tom_related_folder_list =
		related_folder_get_join_1tom_related_folder_list(
			folder->one2m_related_folder_list );

	return folder;

} /* folder_with_load_new() */

LIST *folder_get_attribute_list(
			char *application_name,
			char *folder_name )
{
	return attribute_get_attribute_list(
			application_name,
			folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			(char *)0 /* role_name */ );

} /* folder_get_attribute_list() */

LIST *folder_get_non_primary_attribute_name_list( LIST *attribute_list )
{
	LIST *non_primary_attribute_name_list;
	ATTRIBUTE *attribute;

	non_primary_attribute_name_list = list_new();

	if ( list_rewind( attribute_list ) )
		do {
			attribute = (ATTRIBUTE *)
			      list_get_pointer( attribute_list );

			if ( !attribute->primary_key_index )
			{
				list_append_pointer(
					non_primary_attribute_name_list,
					attribute->attribute_name );
			}
		} while( list_next( attribute_list ) );
	return non_primary_attribute_name_list;
} /* folder_get_non_primary_attribute_name_list() */

LIST *folder_get_attribute_name_list( LIST *attribute_list )
{
	return attribute_get_attribute_name_list( attribute_list );
}

LIST *folder_get_primary_attribute_name_list(
				LIST *attribute_list )
{
	ATTRIBUTE *attribute;
	LIST *primary_attribute_name_list;

	if ( !attribute_list || !list_rewind( attribute_list ) )
		return (LIST *)0;

	primary_attribute_name_list = list_new_list();

	do {
		attribute = (ATTRIBUTE *)
		      list_get_pointer( attribute_list );

		if ( attribute->primary_key_index )
		{
			list_append_string(
					primary_attribute_name_list,
					attribute->attribute_name );
		}
	} while( list_next( attribute_list ) );

	return primary_attribute_name_list;
}

LIST *folder_get_primary_attribute_list( LIST *attribute_list )
{
	LIST *primary_attribute_name_list;
	LIST *primary_attribute_list;
	ATTRIBUTE *attribute;

	primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			attribute_list );

	primary_attribute_list = list_new();

	if ( !list_length( primary_attribute_name_list ) )
		return primary_attribute_list;

	list_rewind( attribute_list );
	do {
		attribute = (ATTRIBUTE *)list_get_pointer( attribute_list );

		if ( list_exists_string( primary_attribute_name_list,
					 attribute->attribute_name ) )
		{
			list_append_pointer(	primary_attribute_list,
						attribute );
		}
	} while( list_next( attribute_list ) );

	return primary_attribute_list;

}

LIST *folder_get_process_primary_data_list(
			char *application_name,
			char *folder_name,
			char *login_name,
			DICTIONARY *preprompt_dictionary,
			char delimiter,
			PROCESS *populate_drop_down_process,
			LIST *attribute_list,
			LIST *common_non_primary_attribute_name_list,
			boolean row_level_non_owner_forbid,
			LIST *exclude_attribute_name_list,
			char *role_name,
			char *state,
			char *one2m_folder_name_for_processes,
			char *process_name,
			char *prompt )
{
	char *drop_down_where_clause;
	char *attribute_where_clause;
	LIST *primary_attribute_name_list;
	LIST *date_attribute_name_list;
	LIST *date_position_list;
	LIST *data_list;
	char where_clause[ 65536 ];
	QUERY *query;

	if ( populate_drop_down_process )
	{
		LIST *drop_down_process_list;
		char *local_role_name;

		if ( role_name )
			local_role_name = role_name;
		else
			local_role_name = 
				populate_drop_down_process->role_name;

		drop_down_process_list =
		     folder_get_drop_down_process_list(
			application_name,
			BOGUS_SESSION,
			folder_name,
			login_name,
			populate_drop_down_process,
			local_role_name,
			preprompt_dictionary /* parameter_dictionary */,
			preprompt_dictionary /* where_clause_dictionary */,
			state,
			one2m_folder_name_for_processes,
			attribute_list,
			0
			/* piece_multi_attribute_data_label_delimiter */,
			process_name,
			prompt );

		return drop_down_process_list;
	}

	if ( row_level_non_owner_forbid && login_name )
	{
		dictionary_set_pointer(	preprompt_dictionary,
					"login_name",
					login_name );
	}

	primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			attribute_list );

	date_attribute_name_list =
		attribute_get_date_attribute_name_list(
			attribute_list );

	if ( list_length( exclude_attribute_name_list ) )
	{
		primary_attribute_name_list =
			list_subtract_string_list(
				primary_attribute_name_list,
				exclude_attribute_name_list );
	}

	if ( !list_rewind( primary_attribute_name_list ) )
		return list_new();

	query = query_process_new(
			application_name,
			login_name,
			folder_name,
			preprompt_dictionary
				/* query_dictionary */ );

	drop_down_where_clause =
		query->query_output->drop_down_where_clause;

	attribute_where_clause =
		query->query_output->attribute_where_clause;

	date_position_list =
		list_get_position_list(
			primary_attribute_name_list,
			date_attribute_name_list );

	if ( !drop_down_where_clause && !attribute_where_clause )
	{
		strcpy( where_clause, "1 = 1" );
	}
	else
	if ( !drop_down_where_clause && attribute_where_clause )
	{
		strcpy( where_clause, attribute_where_clause );
	}
	else
	if ( drop_down_where_clause && !attribute_where_clause )
	{
		strcpy( where_clause, drop_down_where_clause );
	}
	else
	{
		sprintf(	where_clause,
				"%s and %s",
				drop_down_where_clause,
				attribute_where_clause );
	}

	data_list =
		folder_data_list(
			application_name,
			folder_name,
			primary_attribute_name_list,
			where_clause,
			delimiter,
			common_non_primary_attribute_name_list,
			login_name,
			date_position_list,
			0 /* maxrows */ );

	return data_list;
}

LIST *folder_primary_data_process_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			DICTIONARY *parameter_dictionary,
			DICTIONARY *where_clause_dictionary,
			PROCESS *populate_drop_down_process,
			LIST *attribute_list,
			char *role_name,
			char *state,
			char *one2m_folder_name_for_processes )
{
	LIST *drop_down_process_list;
	char *local_role_name;

	if ( role_name )
		local_role_name = role_name;
	else
		local_role_name = 
			populate_drop_down_process->role_name;

	drop_down_process_list =
	     folder_get_drop_down_process_list(
		application_name,
		session,
		folder_name,
		login_name,
		populate_drop_down_process,
		local_role_name,
		parameter_dictionary,
		where_clause_dictionary,
		state,
		one2m_folder_name_for_processes,
		attribute_list,
		0
		/* piece_multi_attribute_data_label_delimiter */,
		(char *)0 /* process_name */,
		(char *)0 /* prompt */ );

	return drop_down_process_list;
}

LIST *folder_related_primary_data_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			DICTIONARY *parameter_dictionary,
			DICTIONARY *where_clause_dictionary,
			char delimiter,
			PROCESS *populate_drop_down_process,
			LIST *attribute_list,
			LIST *common_non_primary_attribute_name_list,
			boolean filter_only_login_name,
			char *role_name,
			char *state,
			char *one2m_folder_name_for_processes,
			char *appaserver_user_foreign_login_name )
{
	/* if row level non-owner forbid */
	/* ----------------------------- */
	if ( filter_only_login_name && login_name )
	{
		char operator_entry[ 128 ];

		if ( !parameter_dictionary )
			parameter_dictionary =
				dictionary_small_new();

		if ( !where_clause_dictionary )
			where_clause_dictionary =
				dictionary_small_new();

		dictionary_set_pointer(	parameter_dictionary,
					"login_name",
					login_name );

		dictionary_set_pointer(	where_clause_dictionary,
					"login_name",
					login_name );

		sprintf(operator_entry,
		 	"%s%s",
		 	QUERY_RELATION_OPERATOR_STARTING_LABEL,
		 	"login_name" );

		dictionary_set_pointer(	where_clause_dictionary,
					strdup( operator_entry ),
					EQUAL_OPERATOR );

		if ( appaserver_user_foreign_login_name )
		{
			dictionary_set_pointer(
				parameter_dictionary,
				appaserver_user_foreign_login_name,
				login_name );

			dictionary_set_pointer(
				where_clause_dictionary,
				appaserver_user_foreign_login_name,
				login_name );

			sprintf(operator_entry,
		 		"%s%s",
		 		QUERY_RELATION_OPERATOR_STARTING_LABEL,
		 		appaserver_user_foreign_login_name );

			dictionary_set_pointer(	where_clause_dictionary,
						strdup( operator_entry ),
						EQUAL_OPERATOR );

		}

	} /* if row level non-owner forbid */

	if ( populate_drop_down_process )
	{
		return folder_primary_data_process_list(
			application_name,
			session,
			folder_name,
			login_name,
			parameter_dictionary,
			where_clause_dictionary,
			populate_drop_down_process,
			attribute_list,
			role_name,
			state,
			one2m_folder_name_for_processes );
	}
	else
	{
		return folder_related_primary_data_table_list(
			application_name,
			folder_name,
			login_name,
			where_clause_dictionary,
			delimiter,
			attribute_list,
			common_non_primary_attribute_name_list,
			list_new() /* exclude_attribute_name_list */,
			role_name );
	}
}

LIST *folder_primary_data_list(
				char *application_name,
				char *session,
				char *folder_name,
				char *login_name,
				DICTIONARY *parameter_dictionary,
				DICTIONARY *where_clause_dictionary,
				char delimiter,
				PROCESS *populate_drop_down_process,
				LIST *attribute_list,
				LIST *common_non_primary_attribute_name_list,
				boolean filter_only_login_name,
				LIST *exclude_attribute_name_list,
				char *role_name,
				char *state,
				char *one2m_folder_name_for_processes,
				char *appaserver_user_foreign_login_name,
				boolean include_root_folder )
{

	/* if row level non-owner forbid */
	/* ----------------------------- */
	if ( filter_only_login_name && login_name )
	{
		char operator_entry[ 128 ];

		if ( !parameter_dictionary )
			parameter_dictionary =
				dictionary_small_new();

		if ( !where_clause_dictionary )
			where_clause_dictionary =
				dictionary_small_new();

		dictionary_set_pointer(	parameter_dictionary,
					"login_name",
					login_name );

		dictionary_set_pointer(	where_clause_dictionary,
					"login_name",
					login_name );

		sprintf(operator_entry,
		 	"%s%s",
		 	QUERY_RELATION_OPERATOR_STARTING_LABEL,
		 	"login_name" );

		dictionary_set_pointer(	where_clause_dictionary,
					strdup( operator_entry ),
					EQUAL_OPERATOR );

		if ( appaserver_user_foreign_login_name )
		{
			dictionary_set_pointer(
				parameter_dictionary,
				appaserver_user_foreign_login_name,
				login_name );

			dictionary_set_pointer(
				where_clause_dictionary,
				appaserver_user_foreign_login_name,
				login_name );

			sprintf(operator_entry,
		 		"%s%s",
		 		QUERY_RELATION_OPERATOR_STARTING_LABEL,
		 		appaserver_user_foreign_login_name );

			dictionary_set_pointer(	where_clause_dictionary,
						strdup( operator_entry ),
						EQUAL_OPERATOR );

		}

	} /* if row level non-owner forbid */

	if ( populate_drop_down_process )
	{
		return folder_primary_data_process_list(
			application_name,
			session,
			folder_name,
			login_name,
			parameter_dictionary,
			where_clause_dictionary,
			populate_drop_down_process,
			attribute_list,
			role_name,
			state,
			one2m_folder_name_for_processes );
	}
	else
	{
		return folder_primary_data_table_list(
			application_name,
			folder_name,
			login_name,
			where_clause_dictionary,
			delimiter,
			attribute_list,
			common_non_primary_attribute_name_list,
			exclude_attribute_name_list,
			role_name,
			include_root_folder );
	}
}

LIST *folder_get_process_dictionary_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			PROCESS *populate_drop_down_process,
			char *role_name,
			DICTIONARY *parameter_dictionary,
			char *state,
			char *one2m_folder_name_for_processes,
			LIST *attribute_list,
			char piece_multi_attribute_data_label_delimiter,
			char *process_name,
			char *prompt,
			LIST *primary_attribute_name_list )
{
	LIST *primary_data_list;

	primary_data_list =
		folder_get_drop_down_process_list(
			application_name,
			session,
			folder_name,
			login_name,
			populate_drop_down_process,
			role_name,
			parameter_dictionary,
			parameter_dictionary /* where_clause_dictionary */,
			state,
			one2m_folder_name_for_processes,
			attribute_list,
			piece_multi_attribute_data_label_delimiter,
			process_name,
			prompt );

	return dictionary_data_list_attribute_name_list_merge_dictionary_list(
			primary_data_list,
			primary_attribute_name_list,
			FOLDER_DATA_DELIMITER );

} /* folder_get_process_dictionary_list() */

LIST *folder_get_drop_down_process_list(
			char *application_name,
			char *session,
			char *folder_name,
			char *login_name,
			PROCESS *populate_drop_down_process,
			char *role_name,
			DICTIONARY *parameter_dictionary,
			DICTIONARY *where_clause_dictionary,
			char *state,
			char *one2m_folder_name_for_processes,
			LIST *attribute_list,
			char piece_multi_attribute_data_label_delimiter,
			char *process_name,
			char *prompt )
{
	LIST *return_list;
	char *parameter_process_name;

	if ( process_name )
	{
		parameter_process_name = process_name;
	}
	else
	{
		parameter_process_name =
			populate_drop_down_process->process_name;
	}

	if ( !populate_drop_down_process )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: null pointer for populate_drop_down_process\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	process_convert_parameters(
			&populate_drop_down_process->executable,
			application_name,
			session,
			state,
			login_name,
			folder_name,
			role_name,
			(char *)0 /* target_frame */,
			parameter_dictionary,
			where_clause_dictionary,
			attribute_list,
			(LIST *)0 /* prompt_list */,
			(LIST *)0 /* primary_attribute_name_list */,
			(LIST *)0 /* primary_data_list */,
			0 /* row */,
			parameter_process_name,
			(PROCESS_SET *)0 /* process_set */,
			one2m_folder_name_for_processes,
			(char *)0 /* operation_row_count_string */,
			prompt );

/*
fprintf( stderr, "%s/%s()/%d: executable = [%s]\n",
__FILE__,
__FUNCTION__,
__LINE__,
populate_drop_down_process->executable );
*/

	return_list = process2list( populate_drop_down_process->executable );

	if ( piece_multi_attribute_data_label_delimiter )
	{
		return_list =
			list_usage_piece_list(
				return_list,
				piece_multi_attribute_data_label_delimiter,
				0 /* offset */ );
	}

	return return_list;

} /* folder_get_drop_down_process_list() */

boolean folder_get_pair_one2m_related_folder_boolean(
					char *folder_name,
					LIST *one2m_related_folder_list )
{
	RELATED_FOLDER *related_folder;

	if ( list_rewind( one2m_related_folder_list ) )
	{
		do {
			related_folder = (RELATED_FOLDER *)
				list_get_pointer( one2m_related_folder_list );

			if ( related_folder->pair_1tom_order
			&&   strcmp( related_folder->
					folder->
					folder_name,
				     folder_name ) == 0 )
			{
				return 1;
			}

		} while( list_next( one2m_related_folder_list ) );
	}
	return 0;
}

LIST *folder_data_list(
			char *application_name,
			char *folder_name,
			LIST *attribute_name_list,
			char *where_clause,
			char delimiter,
			LIST *common_non_primary_attribute_name_list,
			char *login_name,
			LIST *date_position_list,
			int maxrows )
{
	char sys_string[ 65536 ];
	char select[ 4096 ];
	char common_non_primary_process[ 512 ];
	char date_convert_process[ 512 ];
	char escape_delimiter_process[ 512 ];
	char first_folder_name[ 128 ];

	sprintf( escape_delimiter_process,
		 "sed 's/%c/\\\\%c/g'",
		 MULTI_ATTRIBUTE_DATA_LABEL_DELIMITER,
		 MULTI_ATTRIBUTE_DATA_LABEL_DELIMITER );

	if ( list_length( date_position_list ) && login_name )
	{
		sprintf(	date_convert_process,
				"date_convert.e '%c' %s %s",
				FOLDER_DATA_DELIMITER,
				list_display( date_position_list ),
				date_convert_get_date_format_string(
					date_convert_get_user_date_format(
						application_name,
						login_name ) ) );
	}
	else
	{
		strcpy( date_convert_process, "cat" );
	}

	strcpy( common_non_primary_process, "cat" );

	strcpy(	select,
		attribute_get_select_clause(
			application_name,
			piece( first_folder_name, ',', folder_name, 0 ),
			attribute_name_list ) );

	if ( common_non_primary_attribute_name_list
	&&   list_length( common_non_primary_attribute_name_list ) )
	{
		sprintf( select + strlen( select ),
			 ",%s",
			 list_display(
				common_non_primary_attribute_name_list ) );

		sprintf( common_non_primary_process,
			 "sed 's/\\%c/ [/%d' | sed 's/$/]/'",
			 delimiter,
			 list_length( attribute_name_list ) );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s			 "
		 "			folder=%s			 "
		 "			select=\"%s\"			 "
		 "			where_clause=\"%s\"		 "
		 "			maxrows=%d 2>>%s		|"
		 "%s							|"
		 "tr '%c' '%c'						|"
		 "%s							|"
		 "%s							 ",
		 	application_name,
		 	folder_name,
			select,
		 	where_clause,
			maxrows,
		 	appaserver_error_get_filename( application_name ),
			date_convert_process,
		 	FOLDER_DATA_DELIMITER,
		 	delimiter,
			common_non_primary_process,
			escape_delimiter_process );

	return pipe2list( sys_string );
}

boolean folder_load(	int *insert_rows_number,
			boolean *lookup_email_output,
			boolean *row_level_non_owner_forbid,
			boolean *row_level_non_owner_view_only,
			PROCESS **populate_drop_down_process,
			PROCESS **post_change_process,
			char **folder_form,
			char **notepad,
			char **html_help_file_anchor,
			char **post_change_javascript,
			boolean *lookup_before_drop_down,
			char **data_directory,
			char **index_directory,
			boolean *no_initial_capital,
			char **subschema_name,
			char **create_view_statement,
			char *application_name,
			char *session,
			char *folder_name,
			boolean override_row_restrictions,
			char *role_name,
			LIST *mto1_related_folder_list )
{
	char buffer[ 65536 ];
	char *record;

	if ( !folder_name || !*folder_name )
	{
		char msg[ 1024 ];
		sprintf( msg,
			 "ERROR in %s/%s(): empty folder_name",
			 __FILE__,
			 __FUNCTION__ );
		appaserver_output_error_message(
			application_name, msg, (char *)0 );
		exit( 1 );
	}

	if ( !( record = folder_get_folder_record(
				application_name,
				folder_name ) ) )
	{
		return 0;
	}

	if ( !record
	||   count_character( '^', record ) !=
			FOLDER_CREATE_VIEW_PIECE )
	{
		char msg[ 1024 ];
		sprintf(msg,
"ERROR in %s/%s()/%d with record = (%s), invalid record count of %d for (%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			record,
			count_character( '^', record ),
			folder_name );

		appaserver_output_error_message(
			application_name, msg, (char *)0 );

		exit( 1 );
	}

	piece( buffer, '^', record, FOLDER_INSERT_ROWS_NUMBER_PIECE );
	*insert_rows_number = atoi( buffer );

	piece( buffer, '^', record, FOLDER_LOOKUP_EMAIL_OUTPUT_PIECE );
	*lookup_email_output = (*buffer == 'y');

	piece( buffer, '^', record, FOLDER_POPULATE_DROP_DOWN_PROCESS_PIECE );
	if ( *buffer )
	{
		*populate_drop_down_process =
			process_new_process(
				application_name,
				session,
				strdup( buffer ),
				(DICTIONARY *)0,
				role_name,
				0 /* not with_check_executable_ok */ );
	}

	piece( buffer, '^', record, FOLDER_POST_CHANGE_PROCESS_PIECE );
	if ( *buffer )
	{
		*post_change_process =
			process_new_process(
				application_name,
				session,
				strdup( buffer),
				(DICTIONARY *)0,
				role_name,
				0 /* not with_check_executable_ok */ );
	}

	piece( buffer, '^', record, FOLDER_FORM_PIECE );
	*folder_form = strdup( buffer );

	piece( buffer, '^', record, FOLDER_NOTEPAD_PIECE );
	*notepad = strdup( buffer );

	piece( buffer, '^', record, FOLDER_HTML_HELP_FILE_ANCHOR_PIECE );
	*html_help_file_anchor = strdup( buffer );

	piece( buffer, '^', record, FOLDER_POST_CHANGE_JAVASCRIPT_PIECE );
	*post_change_javascript = strdup( buffer );

	piece( buffer, '^', record, FOLDER_LOOKUP_BEFORE_DROP_DOWN_PIECE );
	*lookup_before_drop_down = (*buffer == 'y');

	piece( buffer, '^', record, FOLDER_DATA_DIRECTORY_PIECE );
	*data_directory = strdup( buffer );

	piece( buffer, '^', record, FOLDER_INDEX_DIRECTORY_PIECE );
	*index_directory = strdup( buffer );

	piece( buffer, '^', record, FOLDER_NO_INITIAL_CAPITAL_PIECE );
	*no_initial_capital = (*buffer == 'y');

	piece( buffer, '^', record, FOLDER_SUBSCHEMA_PIECE );
	*subschema_name = strdup( buffer );

	piece( buffer, '^', record, FOLDER_CREATE_VIEW_PIECE );
	*create_view_statement = strdup( buffer );

	folder_load_row_level_restrictions(
			row_level_non_owner_forbid,
			row_level_non_owner_view_only,
			application_name,
			folder_name,
			mto1_related_folder_list );

	*row_level_non_owner_forbid =
		*row_level_non_owner_forbid &&
		!override_row_restrictions;

	*row_level_non_owner_view_only =
		*row_level_non_owner_view_only &&
		!override_row_restrictions;

	return 1;

} /* folder_load() */

void folder_load_row_level_restrictions(
			boolean *row_level_non_owner_forbid,
			boolean *row_level_non_owner_view_only,
			char *application_name,
			char *folder_name,
			LIST *mto1_related_folder_list )
{
	RELATED_FOLDER *related_folder;
	char *record;
	char row_level_restriction[ 128 ];

	if ( list_rewind( mto1_related_folder_list ) )
	{
		do {
			related_folder = list_get( mto1_related_folder_list );

			if ( !related_folder->is_primary_key_subset )
				continue;

			if ( related_folder->
				folder->
				row_level_non_owner_view_only )
			{
				*row_level_non_owner_view_only = 1;
				return;
			}

		} while( list_next( mto1_related_folder_list ) );
	}

	if ( ! ( record = folder_get_folder_row_level_restrictions_record(
				application_name,
				folder_name ) ) )
	{
		return;
	}

	if ( !piece( row_level_restriction, '^', record, 1 ) ) return;

	*row_level_non_owner_forbid =
		( strcmp(
			row_level_restriction,
			"row_level_non_owner_forbid" ) == 0 );

	*row_level_non_owner_view_only =
		( strcmp(
			row_level_restriction,
			"row_level_non_owner_view_only" ) == 0 );

} /* folder_load_row_level_restrictions() */

DICTIONARY *folder_get_primary_data_dictionary(
					char *application_name,
					char *folder_name,
					LIST *primary_attribute_name_list )
{
	char sys_string[ 1024 ];
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	DICTIONARY *primary_data_dictionary;

	sprintf(sys_string,
		"get_folder_data	application=%s		"
		"			folder=%s		"
		"			select=%s		",
		application_name,
		folder_name,
		list_display_delimited( primary_attribute_name_list, ',' ) );

	input_pipe = popen( sys_string, "r" );

	primary_data_dictionary = dictionary_huge_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		dictionary_set_pointer(	primary_data_dictionary,
					strdup( input_buffer ),
					"" );
	}
	pclose( input_pipe );

	return primary_data_dictionary;
} /* folder_get_primary_data_dictionary() */

FOLDER *folder_seek_folder( LIST *folder_list, char *folder_name )
{
	FOLDER *folder;

	if ( list_rewind( folder_list ) )
	{
		do {
			folder = (FOLDER *)list_get_pointer( folder_list );
			if ( strcmp( folder->folder_name, folder_name ) == 0 )
				return folder;
		} while( list_next( folder_list ) );
	}
	return (FOLDER *)0;
} /* folder_seek_folder() */

LIST *folder_get_non_primary_attribute_list( LIST *attribute_list )
{
	LIST *non_primary_attribute_list;
	ATTRIBUTE *attribute;

	non_primary_attribute_list = list_new();

	if ( list_rewind( attribute_list ) )
		do {
			attribute = (ATTRIBUTE *)
			      list_get_pointer( attribute_list );

			if ( !attribute->primary_key_index )
			{
				list_append_pointer(
					non_primary_attribute_list,
					attribute );
			}
		} while( list_next( attribute_list ) );
	return non_primary_attribute_list;
} /* folder_get_non_primary_attribute_list() */

char *folder_get_folder_record(
				char *application_name,
				char *folder_name )
{
	char *record;
	char compare_folder_name[ 128 ];
	char local_folder_name[ 128 ];
	char sys_string[ 1024 ];
	static LIST *folder_record_list = {0};

	piece( local_folder_name, ',', folder_name, 0 );

	if ( !folder_record_list )
	{
		sprintf( sys_string,
		 	"folder_load.sh %s 2>>%s",
		 	application_name,
		 	appaserver_error_get_filename( application_name ) );

		folder_record_list = pipe2list( sys_string );
	}

	if ( !list_rewind( folder_record_list ) ) return (char *)0;

	do {
		record = list_get_pointer( folder_record_list );

		piece( 	compare_folder_name,
			'^',
			record,
			FOLDER_FOLDER_NAME_PIECE );

		if ( strcmp( compare_folder_name, local_folder_name ) == 0 )
		{
			return record;
		}
	} while( list_next( folder_record_list ) );

	return (char *)0;

} /* folder_get_folder_record() */

char *folder_get_folder_row_level_restrictions_record(
				char *application_name,
				char *folder_name )
{
	char *record;
	char local_folder_name[ 128 ];
	char compare_folder_name[ 128 ];
	static LIST *folder_row_level_restrictions_record_list = {0};

	piece( local_folder_name, ',', folder_name, 0 );

	if ( !folder_row_level_restrictions_record_list )
	{
		char *table_name;
		char sys_string[ 1024 ];

		table_name =
			get_table_name(
				application_name,
				"folder_row_level_restrictions" );

		sprintf( sys_string,
		 "echo \"select folder,row_level_restriction from %s;\" | sql",
		 	 table_name );

		folder_row_level_restrictions_record_list =
			pipe2list( sys_string );
	}

	if ( !list_rewind( folder_row_level_restrictions_record_list ) )
		return (char *)0;

	do {
		record =
			list_get_pointer(
				folder_row_level_restrictions_record_list );

		piece( 	compare_folder_name, '^', record, 0 );

		if ( strcmp( compare_folder_name, local_folder_name ) == 0 )
		{
			return record;
		}
	} while( list_next( folder_row_level_restrictions_record_list ) );

	return (char *)0;

} /* folder_get_folder_row_level_restrictions_record() */

LIST *folder_get_folder_name_list( char *application_name )
{
	char sys_string[ 1024 ];
	char *where;

	where = "create_view_statement is null and folder <> 'null'";

	sprintf(sys_string,
		"get_folder_data	application=%s			"
		"			select=folder			"
		"			folder=folder			"
		"			where=\"%s\"			",
		application_name,
		where );

	return pipe2list( sys_string );

} /* folder_get_folder_name_list() */

LIST *folder_get_folder_list(
			char *application_name,
			boolean override_row_restrictions,
			char *role_name )
{
	LIST *folder_name_list;
	LIST *folder_list;
	char *folder_name;
	FOLDER *folder;

	folder_name_list = folder_get_folder_name_list( application_name );

	if ( !list_rewind( folder_name_list ) )
		return( LIST *)0;

	folder_list = list_new_list();
	do {
		folder_name = list_get_pointer( folder_name_list );

		if ( strcmp( folder_name, "null" ) == 0 ) continue;

		folder = folder_new_folder(
				application_name,
				BOGUS_SESSION,
				folder_name );

		if ( folder_load(
				&folder->insert_rows_number,
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
				BOGUS_SESSION,
				folder->folder_name,
				override_row_restrictions,
				role_name,
				(LIST *)0 /* mto1_related_folder_list */ ) )
		{
			list_append_pointer( folder_list, folder );
		}

	} while( list_next( folder_name_list ) );
	return folder_list;
} /* folder_get_folder_list() */

LIST *folder_get_role_folder_name_list(
				char *application_name,
				char *role_name )
{
	char *role_folder_table_name;
	char sys_string[ 2048 ];

	role_folder_table_name =
		get_table_name(
			application_name,
			"role_folder" );

	sprintf( sys_string,
		 "echo \"	select distinct folder		 	 "
		 "		from %s					 "
		 "		where permission = 'update'		 "
		 "		  and folder <> 'null'			 "
		 "		  and role = '%s';\"			|"
		 "sql.e							 ",
		 role_folder_table_name,
		 role_name );

	return pipe2list( sys_string );

} /* folder_get_role_folder_name_list() */

LIST *folder_get_single_primary_key_folder_name_list(
				char *application_name,
				char *login_name,
				char *role_name )
{
	char *role_appaserver_user_table_name;
	char *role_folder_table_name;
	char *folder_attribute_table_name;
	char sys_string[ 2048 ];
	char subquery[ 1024 ];

	role_appaserver_user_table_name =
		get_table_name(
			application_name,
			"role_appaserver_user" );

	role_folder_table_name =
		get_table_name(
			application_name,
			"role_folder" );

	folder_attribute_table_name =
		get_table_name(
			application_name,
			"folder_attribute" );

	sprintf( subquery,
		 "not exists (	select *			 "
		 "		from %s				 "
		 "		where %s.folder = %s.folder	 "
		 "		  and primary_key_index = 2 )	 ",
		 folder_attribute_table_name,
		 role_folder_table_name,
		 folder_attribute_table_name );
		 
	sprintf( sys_string,
		 "echo \"	select distinct %s.folder		 "
		 "		from %s,%s				 "
		 "		where %s.role = %s.role			 "
		 "		  and %s.login_name = '%s'		 "
		 "		  and permission = 'insert'		 "
		 "		  and %s.role = '%s'			 "
		 "		  and %s;\"				|"
		 "sql.e							 ",
		 role_folder_table_name,
		 role_folder_table_name,
		 role_appaserver_user_table_name,
		 role_folder_table_name,
		 role_appaserver_user_table_name,
		 role_appaserver_user_table_name,
		 login_name,
		 role_appaserver_user_table_name,
		 role_name,
		 subquery );

	return pipe2list( sys_string );

} /* folder_get_single_primary_key_folder_name_list() */

LIST *folder_get_insert_folder_name_list(
				char *application_name,
				char *login_name,
				char *role_name )
{
	char *role_appaserver_user_table_name;
	char *role_folder_table_name;
	char sys_string[ 1024 ];

	role_appaserver_user_table_name =
		get_table_name(
			application_name,
			"role_appaserver_user" );

	role_folder_table_name =
		get_table_name(
			application_name,
			"role_folder" );

	sprintf( sys_string,
		 "echo \"	select distinct %s.folder		 "
		 "		from %s,%s				 "
		 "		where %s.role = %s.role			 "
		 "		  and %s.login_name = '%s'		 "
		 "		  and permission = 'insert'		 "
		 "		  and %s.role = '%s';\"			|"
		 "sql.e							 ",
		 role_folder_table_name,
		 role_folder_table_name,
		 role_appaserver_user_table_name,
		 role_folder_table_name,
		 role_appaserver_user_table_name,
		 role_appaserver_user_table_name,
		 login_name,
		 role_appaserver_user_table_name,
		 role_name );

	return pipe2list( sys_string );

} /* folder_get_insert_folder_name_list() */

LIST *folder_get_zealot_folder_name_list(
				char *application_name )
{
	char sys_string[ 1024 ];
	char *table_name;

	table_name = get_table_name( application_name, "folder" );

	sprintf( sys_string,
		 "echo \"	select folder				 "
		 "		from %s					 "
		 "		where ifnull( appaserver_yn, 'n' ) = 'n' "
		 "		order by folder;\"			|"
		 "sql.e							 ",
		 table_name );

	return pipe2list( sys_string );

} /* folder_get_zealot_folder_name_list() */

LIST *folder_get_select_folder_name_list(
				char *application_name,
				char *login_name,
				char *role_name )
{
	char *role_appaserver_user_table_name;
	char *role_folder_table_name;
	char sys_string[ 1024 ];

	role_appaserver_user_table_name =
		get_table_name(
			application_name,
			"role_appaserver_user" );

	role_folder_table_name =
		get_table_name(
			application_name,
			"role_folder" );

	sprintf( sys_string,
		 "echo \"	select distinct %s.folder		 "
		 "		from %s,%s				 "
		 "		where %s.role = %s.role			 "
		 "		  and %s.login_name = '%s'		 "
		 "		  and ( permission = 'lookup'		 "
		 "		   or   permission = 'update' )		 "
		 "		  and %s.role = '%s';\"			|"
		 "sql.e							 ",
		 role_folder_table_name,
		 role_folder_table_name,
		 role_appaserver_user_table_name,
		 role_folder_table_name,
		 role_appaserver_user_table_name,
		 role_appaserver_user_table_name,
		 login_name,
		 role_appaserver_user_table_name,
		 role_name );

	return pipe2list( sys_string );

} /* folder_get_select_folder_name_list() */

LIST *folder_get_primary_text_element_list(
				char *application_name,
				char *session,
				char *role_name,
				FOLDER *folder )
{
	ATTRIBUTE *attribute;
	LIST *return_element_list = list_new_list();

	if ( !folder->mto1_related_folder_list )
	{
		folder->mto1_related_folder_list =
			related_folder_get_mto1_related_folder_list(
			   list_new_list(),
			   application_name,
			   session,
			   folder->folder_name,
			   role_name,
			   0 /* isa_flag */,
			   related_folder_no_recursive,
			   0 /* dont override_row_restrictions */,
			   (LIST *)0 /* root_primary_attribute_name_list */,
			   0 /* recursive_level */ );
	}

	if ( !folder->attribute_list )
	{
		folder->attribute_list =
			attribute_get_attribute_list(
			application_name,
			folder->folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			role_name );
	}

	if ( !list_rewind( folder->attribute_list ) )
		return return_element_list;

	do {
		attribute = list_get_pointer( folder->attribute_list );

		if ( !attribute->primary_key_index ) continue;

		if ( related_folder_attribute_consumes_related_folder(
				(LIST **)0 /* foreign_attribute_name_list */,
				(LIST *)0 /* done_attribute_name_list */,
			        (LIST *)0 /* omit_update_attribute_name_list */,
				folder->mto1_related_folder_list,
				attribute->attribute_name,
				(LIST *)0 /* include_attribute_name_list */ ) )
		{
			continue;
		}

		list_append_list(
			return_element_list,
			attribute_get_attribute_element_list(
				attribute->attribute_name,
				(char *)0 /* prepend_folder_name */,
				attribute->datatype,
				attribute->post_change_javascript,
				attribute->width,
				attribute->hint_message,
				1 /* is_primary_attribute */,
				0 /* not omit_push_buttons */ ) );

	} while( list_next( folder->attribute_list ) );

	return return_element_list;
} /* folder_get_primary_text_element_list() */

FOLDER *folder_get_unfulfilled_lookup_before_drop_down_folder(
		LIST *mto1_related_folder_list,
		DICTIONARY *post_dictionary )
{
	RELATED_FOLDER *unfulfilled_related_folder;

	if ( !post_dictionary )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: post_dictionary is null.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( mto1_related_folder_list ) )
		return (FOLDER *)0;

	do {
		unfulfilled_related_folder =
			list_get_pointer(
				mto1_related_folder_list );

		if ( unfulfilled_related_folder->
				folder->
				lookup_before_drop_down )
		{
			char *unfulfilled_dictionary_key;

			unfulfilled_dictionary_key =
				folder_get_unfulfilled_dictionary_key(
					unfulfilled_related_folder->
					folder->
					folder_name );

			if ( !dictionary_key_exists(
					post_dictionary,
					unfulfilled_dictionary_key ) )
			{
				return	unfulfilled_related_folder->
					folder;
			}
		}
	} while( list_next( mto1_related_folder_list ) );
	return (FOLDER *)0;
} /* folder_get_unfulfilled_lookup_before_drop_down_folder() */

char *folder_get_unfulfilled_dictionary_key(
			char *folder_name )
{
	char dictionary_key[ 128 ];

	sprintf(dictionary_key,
		"%s_%s",
		LOOKUP_BEFORE_DROP_DOWN_UNFULFILLED_FOLDER_PREFIX,
		folder_name );
	return strdup( dictionary_key );
} /* folder_get_unfulfilled_dictionary_key() */

#ifdef NOT_DEFINE
char *folder_primary_data_dictionary_display(
				DICTIONARY *primary_data_dictionary )
{
	char buffer[ 65536 ];
	char *buf_ptr = buffer;
	DICTIONARY *primary_data_dictionary;
	int first_time = 1;

	if ( !list_rewind( primary_data_dictionary_list ) ) return "";

	*buf_ptr = '\0';

	do {
		primary_data_dictionary =
			list_get_pointer(
				primary_data_dictionary_list );

		if ( first_time )
		{
			first_time = 0;
			buf_ptr += sprintf( buf_ptr,
					    "%s",
					    dictionary_display(
						    primary_data_dictionary ) );
		}
		else
		{
			buf_ptr += sprintf( buf_ptr,
					    ":%s",
					    dictionary_display(
						    primary_data_dictionary ) );
		}
	} while( next_item( primary_data_dictionary_list ) );
	return strdup( buffer );
} /* folder_primary_data_dictionary_display() */
#endif

void folder_convert_date_attributes_to_database_format(
			DICTIONARY *dictionary,
			char *application_name,
			LIST *attribute_list )
{
	char date_key[ 128 ];
	char *date_string;
	ATTRIBUTE *attribute;

	if ( list_rewind( attribute_list ) )
	{
		do {
			attribute = list_get_pointer( attribute_list );

			if ( strcmp(	attribute->datatype,
					"date" ) == 0
			||   strcmp(	attribute->datatype,
					"current_date" ) == 0 )
			{
				sprintf( date_key,
					 "from_%s_0",
					 attribute->attribute_name );

				date_string =
					dictionary_fetch(
						dictionary, date_key );

				appaserver_library_dictionary_convert_date( 
					dictionary,
					application_name,
					date_string,
					date_key );

				sprintf( date_key,
					 "to_%s_0",
					 attribute->attribute_name );

				date_string =
					dictionary_fetch(
						dictionary, date_key );

				appaserver_library_dictionary_convert_date( 
					dictionary,
					application_name,
					date_string,
					date_key );

				sprintf( date_key,
					 "%s_0",
					 attribute->attribute_name );

				date_string =
					dictionary_fetch(
						dictionary, date_key );

				appaserver_library_dictionary_convert_date( 
					dictionary,
					application_name,
					date_string,
					date_key );

			}
		} while( list_next( attribute_list ) );
	} /* if list_rewind() */
} /* folder_convert_date_attributes_to_database_format() */

void folder_append_one2m_related_folder_list(
		LIST *mto1_related_folder_list,
		char *application_name )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( mto1_related_folder_list ) ) return;

	do {
		related_folder =
			list_get_pointer(
				mto1_related_folder_list );

		related_folder->folder->one2m_recursive_related_folder_list =
		related_folder_1tom_related_folder_list(
			application_name,
			BOGUS_SESSION,
			related_folder->folder->folder_name,
			(char *)0 /* role_name */,
			other,
			(LIST *)0 /* primary_data_list */,
			list_new() /* related_folder_list */,
			1 /* omit_isa_relations */,
			related_folder_recursive_all,
			related_folder->folder->primary_attribute_name_list
				/* parent_primary_attribute_name_list */,
			related_folder->folder->primary_attribute_name_list
				/* original_primary_attribute_name_list */,
			(char *)0 /* prior_related_attribute_name */ );

		related_folder->folder->one2m_related_folder_list =
		related_folder_1tom_related_folder_list(
			application_name,
			BOGUS_SESSION,
			related_folder->folder->folder_name,
			(char *)0 /* role_name */,
			other,
			(LIST *)0 /* primary_data_list */,
			list_new() /* related_folder_list */,
			1 /* omit_isa_relations */,
			related_folder_no_recursive,
			related_folder->folder->primary_attribute_name_list
				/* parent_primary_attribute_name_list */,
			related_folder->folder->primary_attribute_name_list
				/* original_primary_attribute_name_list */,
			(char *)0 /* prior_related_attribute_name */ );

	} while( list_next( mto1_related_folder_list ) );

} /* folder_append_one2m_related_folder_list() */

void folder_append_mto1_related_folder_list(
			LIST *mto1_related_folder_list,
			char *application_name )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( mto1_related_folder_list ) ) return;

	do {
		related_folder =
			list_get_pointer(
				mto1_related_folder_list );

		related_folder->folder->mto1_related_folder_list =
			related_folder_get_mto1_related_folder_list(
				list_new(),
				application_name,
				BOGUS_SESSION,
				related_folder->folder->folder_name,
				(char *)0 /* role_name */,
				0 /* isa_flag */,
				related_folder_no_recursive,
				1 /* override_row_restrictions */,
				(LIST *)0
				/* root_primary_attribute_name_list */,
				0 /* recursive_level */ );

	} while( list_next( mto1_related_folder_list ) );

} /* folder_append_mto1_related_folder_list() */

LIST *folder_append_isa_mto1_related_folder_list(
			char *application_name,
			char *session,
			char *role_name,
			boolean override_row_restrictions,
			LIST *mto1_isa_related_folder_list )
{
	RELATED_FOLDER *related_folder;
	LIST *return_mto1_isa_related_folder_list;

	if ( !list_length( mto1_isa_related_folder_list ) )
		return (LIST *)0;

	return_mto1_isa_related_folder_list =
		list_copy( mto1_isa_related_folder_list );

	list_rewind( mto1_isa_related_folder_list );

	do {
		related_folder =
			list_get_pointer(
				mto1_isa_related_folder_list );

		related_folder->folder->mto1_related_folder_list =
			related_folder_get_mto1_related_folder_list(
				list_new_list(),
				application_name,
				session,
				related_folder->folder->folder_name,
				role_name,
				0 /* isa_flag */,
				related_folder_no_recursive,
				override_row_restrictions,
				(LIST *)0
				/* root_primary_attribute_name_list */,
				0 /* recursive_level */ );

		related_folder_mto1_append_unique(
			return_mto1_isa_related_folder_list,
			related_folder->folder->mto1_related_folder_list );

	} while( list_next( mto1_isa_related_folder_list ) );

	return return_mto1_isa_related_folder_list;

} /* folder_append_isa_mto1_related_folder_list() */

char *folder_get_foreign_join_where_clause(
		LIST *primary_attribute_name_list,
		char *one_folder_name,
		char *many_folder_name )
{
	char where_clause[ 65536 ];
	char *ptr = where_clause;
	boolean first_time = 1;
	char *attribute_name;

	if ( !list_rewind( primary_attribute_name_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: empty primary_attribute_name_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		attribute_name =
			list_get_pointer(
				primary_attribute_name_list );

		if ( first_time )
			first_time = 0;
		else
			ptr += sprintf( ptr, " and " );

		ptr += sprintf( ptr,
				"%s.%s = %s.%s",
				one_folder_name,
				attribute_name,
				many_folder_name,
				attribute_name );

	} while( list_next( primary_attribute_name_list ) );

	*ptr = '\0';
	return strdup( where_clause );

} /* folder_get_foreign_join_where_clause() */

boolean folder_exists_folder(		char *application_name,
					char *folder_name )
{
	LIST *folder_list;

	folder_list = folder_get_folder_list(
				application_name,
				0 /* don't override_row_restrictions */,
				(char *)0 /* role_name */ );

	return (boolean)( folder_seek_folder(	folder_list,
						folder_name ) != (void *)0 );

} /* folder_exists_folder() */

boolean folder_exists_attribute(	char *application_name,
					char *folder_name,
					char *attribute_name )
{
	LIST *attribute_list;

	attribute_list =
		folder_get_attribute_list(
			application_name,
			folder_name );

	return (boolean) ( attribute_seek_attribute(
				attribute_list,
				attribute_name ) != (void *)0 );

} /* folder_exists_folder() */

char *folder_display( FOLDER *folder )
{
	char buffer[ 65536 ];
	char *buffer_pointer = buffer;

	buffer_pointer +=
		sprintf(	buffer_pointer,
				"folder = %s;",
				folder->folder_name );

	buffer_pointer +=
		sprintf(	buffer_pointer,
				" mto1_related_folder_list = %s;",
				related_folder_list_display(
					folder->mto1_related_folder_list,
					mto1,
					'\n' ) );

	buffer_pointer += sprintf( buffer_pointer, "\n" );

	return strdup( buffer );

} /* folder_display() */

LIST *folder_get_table_name_list( char *application_name )
{
	LIST *folder_name_list;
	LIST *table_name_list;
	char *folder_name;
	char *table_name;

	folder_name_list = folder_get_folder_name_list( application_name );

	if ( !list_rewind( folder_name_list ) ) return (LIST *)0;

	table_name_list = list_new();

	do {
		folder_name = list_get_pointer( folder_name_list );

		table_name =
			/* ------------------------------ */
			/* Returns heap memory [strdup()] */
			/* ------------------------------ */
			appaserver_library_get_table_name(
				application_name,
				folder_name );

		if ( folder_table_exists( table_name ) )
		{
			list_append_pointer( table_name_list, table_name );
		}

	} while( list_next( folder_name_list ) );

	return table_name_list;

} /* folder_get_table_name_list() */

LIST *folder_fetch_table_name_list( void )
{
	return pipe2list( "echo \"show tables;\" | sql.e" );
}

boolean folder_table_exists( char *table_name )
{
	static LIST *table_name_list = {0};

	if ( !table_name_list )
	{
		table_name_list = folder_fetch_table_name_list();
	}

	return list_exists_string( table_name_list, table_name );

} /* folder_table_exists() */

LIST *folder_append_isa_attribute_list(
				char *application_name,
				char *folder_name,
				LIST *mto1_isa_related_folder_list,
				char *role_name )
{
	return attribute_append_isa_attribute_list(
			application_name,
			folder_name,
			mto1_isa_related_folder_list,
			role_name );
}

LIST *folder_mto1_isa_related_folder_list(
			LIST *existing_related_folder_list,
			char *application_name,
			char *folder_name,
			char *role_name,
			int recursive_level )
{
	return related_folder_mto1_isa_related_folder_list(
			existing_related_folder_list,
			application_name,
			folder_name,
			role_name,
			recursive_level );
}

LIST *folder_attribute_list(
			char *application_name,
			char *folder_name,
			char *role_name )
{
	return attribute_get_attribute_list(
			application_name,
			folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			role_name );
}

LIST *folder_append_isa_attribute_name_list(
			LIST *append_isa_attribute_list )
{
	return folder_attribute_name_list(
			append_isa_attribute_list );
}

LIST *folder_attribute_name_list(
			LIST *attribute_list )
{
	return attribute_folder_name_list( attribute_list, (char *)0 );
}

LIST *folder_primary_data_table_list(
			char *application_name,
			char *folder_name,
			char *login_name,
			DICTIONARY *where_clause_dictionary,
			char delimiter,
			LIST *attribute_list,
			LIST *common_non_primary_attribute_name_list,
			LIST *exclude_attribute_name_list,
			char *role_name,
			boolean include_root_folder )
{
	char *where_clause;
	LIST *primary_attribute_name_list;
	LIST *date_attribute_name_list;
	LIST *date_position_list;
	LIST *data_list;
	QUERY *query;

	primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			attribute_list );

	date_attribute_name_list =
		attribute_get_date_attribute_name_list(
			attribute_list );

	if ( exclude_attribute_name_list
	&&   list_length( exclude_attribute_name_list ) )
	{
		primary_attribute_name_list =
			list_subtract_string_list(
				primary_attribute_name_list,
				exclude_attribute_name_list );
	}

	if ( !list_length( primary_attribute_name_list ) )
		return list_new();

	query =	query_folder_data_new(
			application_name,
			login_name,
			folder_name,
			where_clause_dictionary,
			role_new( application_name, role_name ),
			0 /* infinite max_rows */,
			include_root_folder );

	where_clause = query->query_output->where_clause;

	date_position_list =
		list_get_position_list(
			primary_attribute_name_list,
			date_attribute_name_list );

	data_list =
		folder_data_list(
			application_name,
			query->query_output->from_clause
				/* folder_name */,
			primary_attribute_name_list,
			where_clause,
			delimiter,
			common_non_primary_attribute_name_list,
			login_name,
			date_position_list,
			FOLDER_MAXROWS );

	return data_list;
}

LIST *folder_related_primary_data_table_list(
			char *application_name,
			char *folder_name,
			char *login_name,
			DICTIONARY *where_clause_dictionary,
			char delimiter,
			LIST *attribute_list,
			LIST *common_non_primary_attribute_name_list,
			LIST *exclude_attribute_name_list,
			char *role_name )
{
	char *where_clause;
	LIST *primary_attribute_name_list;
	LIST *date_attribute_name_list;
	LIST *date_position_list;
	LIST *data_list;
	QUERY *query;

	primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			attribute_list );

	date_attribute_name_list =
		attribute_get_date_attribute_name_list(
			attribute_list );

	if ( exclude_attribute_name_list
	&&   list_length( exclude_attribute_name_list ) )
	{
		primary_attribute_name_list =
			list_subtract_string_list(
				primary_attribute_name_list,
				exclude_attribute_name_list );
	}

	if ( !list_length( primary_attribute_name_list ) )
		return list_new();

	query =	query_related_folder_data_new(
			application_name,
			login_name,
			folder_name,
			where_clause_dictionary,
			role_new( application_name, role_name ),
			0 /* infinite max_rows */ );

	where_clause = query->query_output->where_clause;

	date_position_list =
		list_get_position_list(
			query->folder->primary_attribute_name_list,
			date_attribute_name_list );

	data_list =
		folder_data_list(
			application_name,
			query->query_output->from_clause
				/* folder_name */,
			query->folder->primary_attribute_name_list,
			where_clause,
			delimiter,
			common_non_primary_attribute_name_list,
			login_name,
			date_position_list,
			FOLDER_MAXROWS );

	return data_list;
}

