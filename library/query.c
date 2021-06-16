/* $APPASERVER_LIBRARY/library/query.c					*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "timlib.h"
#include "list_usage.h"
#include "form.h"
#include "query.h"

QUERY *query_calloc( void )
{
	QUERY *query;

	if ( ! ( query = calloc( 1, sizeof( QUERY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: memory allocation error.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return query;

}

QUERY_OUTPUT *query_output_calloc( void )
{
	QUERY_OUTPUT *query_output;

	if ( ! ( query_output = calloc( 1, sizeof( QUERY_OUTPUT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: memory allocation error.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return query_output;

}

QUERY *query_process_parameter_new(
				char *application_name,
				LIST *attribute_list,
				DICTIONARY *parameter_dictionary )
{
	QUERY *query;

	query = query_calloc();

	query->dictionary = parameter_dictionary;

	query->query_output =
		query_process_parameter_output_new(
			query->dictionary,
			application_name,
			attribute_list );

	return query;

}

QUERY *query_process_new(	char *application_name,
				char *login_name,
				char *folder_name,
				DICTIONARY *preprompt_dictionary )
{
	QUERY *query;

	query = query_calloc();

	query->login_name = login_name;
	query->dictionary = preprompt_dictionary;

	if ( ! ( query->folder =
			folder_with_load_new(
				application_name,
				BOGUS_SESSION,
				folder_name,
				(ROLE *)0 /* role */ ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query->query_output =
		query_process_output_new(
			query->folder,
			query->dictionary );

	return query;

}

QUERY *query_insert_new(	char *application_name,
				char *login_name,
				char *folder_name,
				DICTIONARY *query_dictionary )
{
	QUERY *query;

	query = query_calloc();

	query->login_name = login_name;
	query->dictionary = query_dictionary;

	if ( ! ( query->folder =
			folder_with_load_new(
				application_name,
				BOGUS_SESSION,
				folder_name,
				(ROLE *)0 /* role */ ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query->query_output =
		query_insert_output_new(
			query->folder,
			query->dictionary );

	return query;

}

QUERY *query_primary_data_new(	char *application_name,
				char *login_name,
				char *folder_name,
				DICTIONARY *dictionary,
				ROLE *role,
				int max_rows,
				boolean include_root_folder )
{
	QUERY *query;

	query = query_calloc();

	if ( !folder_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty folder_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( query->folder =
			folder_with_load_new(
				application_name,
				BOGUS_SESSION,
				folder_name,
				role ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query->login_name = login_name;
	query->dictionary = dictionary;
	query->max_rows = max_rows;

	query->query_output =
		query_primary_data_output_new(
			query,
			query->folder,
			include_root_folder );

	return query;
}

QUERY *query_process_drop_down_new(
				char *application_name,
				char *folder_name,
				DICTIONARY *dictionary )
{
	QUERY *query;

	query = query_calloc();

	if ( ! ( query->folder =
			folder_with_load_new(
				application_name,
				BOGUS_SESSION,
				folder_name,
				(ROLE *)0 /* role */ ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query->dictionary = dictionary;
	query->max_rows = 0;

	query->query_output =
		query_process_drop_down_output_new(
			query->folder,
			query->folder->folder_name,
			query->dictionary );

	return query;
}

QUERY *query_new(		char *application_name,
				char *login_name,
				char *folder_name,
				LIST *append_isa_attribute_list,
				DICTIONARY *dictionary,
				DICTIONARY *sort_dictionary,
				ROLE *role,
				LIST *where_attribute_name_list,
				LIST *where_attribute_data_list,
				int max_rows,
				boolean include_root_folder,
				LIST *one2m_subquery_folder_name_list,
				RELATED_FOLDER *root_related_folder )
{
	QUERY *query;

	query = query_calloc();

	if ( folder_name )
	{
		char first_folder_name[ 128 ];

		piece( first_folder_name, ',', folder_name, 0 );

		if ( ! ( query->folder =
				folder_with_load_new(
					application_name,
					BOGUS_SESSION,
					strdup( first_folder_name ),
					role ) ) )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: cannot load folder.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		query->prompt_recursive =
			prompt_recursive_new(
				application_name,
				query->folder->folder_name
					/* query_folder_name */,
				query->
					folder->
					mto1_related_folder_list );

		if ( list_length( one2m_subquery_folder_name_list ) )
		{
			query->one2m_subquery_related_folder_list =
				query_get_one2m_subquery_related_folder_list(
					application_name,
					one2m_subquery_folder_name_list,
					query->
					  folder->
					  one2m_recursive_related_folder_list,
					root_related_folder );
		}
	}
	else
	{
		query->folder =
			folder_new(
				application_name,
				BOGUS_SESSION,
				(char *)0 /* folder_name */ );
	}

	if ( list_length( append_isa_attribute_list ) )
	{
		query->folder->append_isa_attribute_list =
			append_isa_attribute_list;
	}

	query->login_name = login_name;
	query->dictionary = dictionary;
	query->sort_dictionary = sort_dictionary;
	query->max_rows = max_rows;
	query->where_attribute_name_list = where_attribute_name_list;
	query->where_attribute_data_list = where_attribute_data_list;

	query->query_output =
		query_output_new(
			query,
			include_root_folder,
			query->where_attribute_name_list,
			query->where_attribute_data_list,
			query->folder,
			query->one2m_subquery_related_folder_list,
			query->prompt_recursive,
			append_isa_attribute_list );

	return query;
}

QUERY_OUTPUT *query_process_output_new(
				FOLDER *folder,
				DICTIONARY *preprompt_dictionary )
{
	QUERY_OUTPUT *query_output;
	LIST *exclude_attribute_name_list = list_new();

	query_output = query_output_calloc();

	query_output->query_drop_down_list =
		query_get_process_drop_down_list(
			exclude_attribute_name_list,
			folder->folder_name,
			folder->mto1_recursive_related_folder_list,
			preprompt_dictionary );

	query_output->query_attribute_list =
		query_get_attribute_list(
			folder->append_isa_attribute_list,
			preprompt_dictionary,
			exclude_attribute_name_list,
			folder->folder_name
				/* dictionary_prepend_folder_name */ );

	query_output->non_joined_where_clause =
	query_output->where_clause =
		query_get_process_where_clause(
			&query_output->drop_down_where_clause,
			&query_output->attribute_where_clause,
			query_output->query_drop_down_list,
			query_output->query_attribute_list,
			folder->application_name,
			folder->folder_name );

	if ( !query_output->where_clause
	||   !*query_output->where_clause )
	{
		query_output->non_joined_where_clause =
		query_output->where_clause = "1 = 1";
	}

	query_output->select_clause =
		query_get_select_clause(
			folder->application_name,
			folder->append_isa_attribute_list );

	query_output->from_clause =
		query_from_clause(
			folder->folder_name,
			(LIST *)0 /* isa_attribute_list */,
			(char *)0 /* attribute_not_null_folder_name */ );

	query_output->order_clause = query_output->select_clause;

	if ( list_length( folder->mto1_isa_related_folder_list ) )
	{
		RELATED_FOLDER *isa_related_folder;
		LIST *foreign_attribute_name_list;

		list_rewind( folder->mto1_isa_related_folder_list );
		do {
			isa_related_folder =
				list_get_pointer(
					folder->
					mto1_isa_related_folder_list );

			foreign_attribute_name_list =
			    related_folder_foreign_attribute_name_list(
				folder_primary_attribute_name_list(
					isa_related_folder->
						one2m_folder->
						attribute_list ),
				isa_related_folder->related_attribute_name,
				isa_related_folder->
					folder_foreign_attribute_name_list );

			query_output->where_clause =
				query_append_where_clause_related_join(
					folder->application_name,
					query_output->where_clause,
					folder_get_primary_attribute_name_list(
						folder->attribute_list ),
					foreign_attribute_name_list,
					folder->folder_name,
					isa_related_folder->
						folder->
						folder_name );

		} while( list_next(
				folder->
				mto1_isa_related_folder_list ) );
	}

	return query_output;
}

QUERY_OUTPUT *query_insert_output_new(
				FOLDER *folder,
				DICTIONARY *query_dictionary )
{
	QUERY_OUTPUT *query_output;

	query_output = query_output_calloc();

	query_output->query_drop_down_list =
		query_get_insert_drop_down_list(
			folder->folder_name,
			folder->mto1_related_folder_list,
			query_dictionary );

	query_output->non_joined_where_clause =
	query_output->where_clause =
		query_combined_where_clause(
			&query_output->drop_down_where_clause,
			&query_output->attribute_where_clause,
			query_output->query_drop_down_list,
			query_output->query_attribute_list,
			folder->application_name,
			folder->folder_name,
			0 /* not combine_date_time */ );

	if ( list_length( folder->mto1_isa_related_folder_list ) )
	{
		query_output->where_clause =
			query_output_mto1_isa_where(
				folder->application_name,
				folder->folder_name,
				folder->mto1_isa_related_folder_list,
				folder->primary_attribute_name_list,
				query_output->where_clause,
				0 /* not one_only */ );
	}

	if ( !query_output->where_clause
	||   !*query_output->where_clause )
	{
		query_output->non_joined_where_clause =
		query_output->where_clause = "1 = 1";
	}

	query_output->select_clause =
		query_get_select_clause(
			folder->application_name,
			folder->append_isa_attribute_list );

	query_output->from_clause =
		query_from_clause(
			folder->folder_name,
			folder->append_isa_attribute_list,
			(char *)0 /* attribute_not_null_folder_name */ );

	query_output->order_clause = query_output->select_clause;

	return query_output;
}

QUERY_OUTPUT *query_primary_data_output_new(
			QUERY *query,
			FOLDER *folder,
			boolean include_root_folder )
{
	QUERY_OUTPUT *query_output;
	LIST *exclude_attribute_name_list = list_new();

	query_output = query_output_calloc();

	if ( !folder )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query_output->from_clause =
		query_from_clause(
			folder->folder_name,
			(LIST *)0 /* isa_attribute_list */,
			(char *)0 /* attribute_not_null_folder_name */ );

	query_output->
		query_drop_down_list =
			query_primary_data_drop_down_list(
				exclude_attribute_name_list,
				folder /* root_folder */,
				folder->
					mto1_related_folder_list,
				folder->
				    mto1_append_isa_related_folder_list,
				query->dictionary,
				include_root_folder );

	query_output->query_attribute_list =
		query_get_attribute_list(
			folder->append_isa_attribute_list,
			query->dictionary,
			exclude_attribute_name_list,
			folder->folder_name
				/* dictionary_prepend_folder_name */ );

	query_output->where_clause =
		query_combined_where_clause(
			&query_output->drop_down_where_clause,
			&query_output->attribute_where_clause,
			query_output->query_drop_down_list,
			query_output->query_attribute_list,
			folder->application_name,
			folder->folder_name,
			0 /* not combine_date_time */ );

	if ( folder->row_level_non_owner_forbid
	&&   list_length( folder->mto1_isa_related_folder_list ) )
	{
		RELATED_FOLDER *isa_related_folder;
		LIST *foreign_attribute_name_list;

		list_rewind( folder->mto1_isa_related_folder_list );

		do {
			isa_related_folder =
				list_get_pointer(
					folder->
					mto1_isa_related_folder_list );

			foreign_attribute_name_list =
				   folder_get_primary_attribute_name_list(
					isa_related_folder->folder->
						attribute_list );

			query_output->where_clause =
				query_append_where_clause_related_join(
					folder->application_name,
					query_output->where_clause,
					folder_get_primary_attribute_name_list(
						folder->attribute_list ),
					foreign_attribute_name_list,
					folder->folder_name,
					isa_related_folder->
						folder->
						folder_name );

		} while( list_next(
				folder->
				mto1_isa_related_folder_list ) );
	}

	return query_output;
}

QUERY_OUTPUT *query_process_parameter_output_new(
				DICTIONARY *dictionary,
				char *application_name,
				LIST *attribute_list )
{
	QUERY_OUTPUT *query_output;

	query_output = query_output_calloc();

	query_output->query_attribute_list =
		query_get_attribute_list(
			attribute_list,
			dictionary,
			(LIST *)0
				/* exclude_attribute_name_list */,
			(char *)0
				/* dictionary_prepend_folder_name */ );

	query_output->where_clause =
		query_combined_where_clause(
			&query_output->drop_down_where_clause,
			&query_output->attribute_where_clause,
			query_output->query_drop_down_list,
			query_output->query_attribute_list,
			application_name,
			(char *)0 /* folder_name */,
			1 /* combine_date_time */ );

	return query_output;
}

QUERY_OUTPUT *query_output_new(	QUERY *query,
				boolean include_root_folder,
				LIST *where_attribute_name_list,
				LIST *where_attribute_data_list,
				FOLDER *folder,
				LIST *one2m_subquery_related_folder_list,
				PROMPT_RECURSIVE *prompt_recursive,
				LIST *append_isa_attribute_list )
{
	QUERY_OUTPUT *query_output;
	LIST *exclude_attribute_name_list = list_new();

	query_output = query_output_calloc();

	if ( list_length( where_attribute_name_list )
	&&   list_length( where_attribute_data_list ) )
	{
		if ( !folder )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: empty folder.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		query_output->where_clause =
			query_simple_where(
				folder->folder_name,
				where_attribute_name_list,
				where_attribute_data_list,
				append_isa_attribute_list );

		goto generate_select_clause;
	}

	if ( folder
	&&   folder->folder_name )
	{
		query_output->
			query_drop_down_list =
				query_drop_down_list(
					exclude_attribute_name_list,
					folder /* root_folder */,
					folder->
						mto1_related_folder_list,
					folder->
					    mto1_append_isa_related_folder_list,
					query->dictionary,
					include_root_folder );
	}

	if ( prompt_recursive
	&&   list_length(
		prompt_recursive->
			prompt_recursive_folder_list ) )
	{
		query_output->query_drop_down_list =
			query_append_prompt_recursive_folder_list(
				query_output->query_drop_down_list,
				exclude_attribute_name_list,
				folder->folder_name
					/* root_folder_name */,
				prompt_recursive->
					prompt_recursive_folder_list,
				query->dictionary );
	}

	if ( folder->row_level_non_owner_forbid )
	{
		query_set_row_level_non_owner_forbid_dictionary(
			query,
			folder );
	}

	if ( folder )
	{
		query_output->query_attribute_list =
			query_get_attribute_list(
				folder->append_isa_attribute_list,
				query->dictionary,
				exclude_attribute_name_list,
				folder->folder_name
				/* dictionary_prepend_folder_name */ );
	}

	if ( folder
	&&   !query_output->where_clause )
	{
		query_output->non_joined_where_clause =
		query_output->where_clause =
		query_combined_where_clause(
			&query_output->drop_down_where_clause,
			&query_output->attribute_where_clause,
			query_output->query_drop_down_list,
			query_output->query_attribute_list,
			folder->application_name,
			folder->folder_name,
			0 /* not combine_date_time */ );
	}

	if ( !query_output->where_clause
	||   !*query_output->where_clause )
	{
		query_output->non_joined_where_clause =
		query_output->where_clause = "1 = 1";
	}

generate_select_clause:

	if ( folder )
	{
		query_output->select_clause =
			query_get_select_clause(
				folder->application_name,
				folder->append_isa_attribute_list );

		query_output->from_clause =
			query_from_clause(
				folder->folder_name,
				folder->append_isa_attribute_list,
				(char *)0
					/* attribute_not_null_folder_name */ );
	}

	if ( dictionary_length( query->sort_dictionary ) )
	{
		LIST *attribute_list = {0};

		if ( folder )
			attribute_list =
				folder->attribute_list;

		query_output->order_clause =
			query_get_order_clause(
				query->sort_dictionary,
				(folder) ? folder->folder_name
					 : (char *)0,
				attribute_list );
	}

	if ( !query_output->order_clause )
	{
		query_output->order_clause = query_output->select_clause;
	}

	if ( folder
	&&   list_rewind( folder->mto1_isa_related_folder_list ) )
	{
		RELATED_FOLDER *isa_related_folder;
		LIST *foreign_attribute_name_list;

		do {
			isa_related_folder =
				list_get_pointer(
					folder->
					mto1_isa_related_folder_list );

			foreign_attribute_name_list =
				   folder_get_primary_attribute_name_list(
					isa_related_folder->folder->
						attribute_list );

			query_output->where_clause =
				query_append_where_clause_related_join(
					folder->application_name,
					query_output->where_clause,
					folder_get_primary_attribute_name_list(
						folder->attribute_list ),
					foreign_attribute_name_list,
					folder->folder_name,
					isa_related_folder->
						folder->
						folder_name );

		} while( list_next(
				folder->
				mto1_isa_related_folder_list ) );
	}

	if ( list_length( one2m_subquery_related_folder_list ) )
	{
		query_output->query_subquery_list =
			query_get_subquery_list(
				query->dictionary,
				one2m_subquery_related_folder_list,
				list_length(	folder->
						primary_attribute_name_list ) );

		if ( list_length( query_output->query_subquery_list ) )
		{
			query_output->subquery_where_clause =
				query_get_subquery_where_clause(
					folder->application_name,
					folder->folder_name,
					query_output->query_subquery_list,
					folder->primary_attribute_name_list );
/*
{
char msg[ 65536 ];
sprintf( msg, "%s/%s()/%d: got subquery_list = %s\n",
__FILE__,
__FUNCTION__,
__LINE__,
query_subquery_list_display( query_output->query_subquery_list ) );
m2( folder->application_name, msg );
}
*/
		}
	}

	/* ------------------------------------------------------------ */
	/* If row_level_non_owner_forbid where a m:1 isa ENTITY.	*/
	/* See EMPLOYEE_WORK_DAY --> EMPLOYEE isa ENTITY.		*/
	/* ------------------------------------------------------------ */
	if ( folder->row_level_non_owner_forbid
	&&   list_length( folder->mto1_related_folder_list ) )
	{
		query_output_set_row_level_non_owner_forbid_join(
			query_output,
			folder );
	}

	return query_output;

}

char *query_get_sys_string( 	char *application_name,
				char *select_clause,
				char *from_clause,
				char *where_clause,
				char *subquery_where_clause,
				char *order_by_clause,
				int maxrows )
{
	char sys_string[ 65536 ];

	if ( !from_clause || !*from_clause )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty from_clause.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf(sys_string,
		"get_folder_data	application=%s			 "
		"			select=\"%s\"			 "
		"			folder=%s			 "
		"			where_clause=\"%s and %s\"	 "
		"			order_clause=\"%s\"		 "
		"			maxrows=\"%d\"			|"
		"cat							 ",
		application_name,
		select_clause,
		from_clause,
		where_clause,
		(subquery_where_clause) ? subquery_where_clause : "1 = 1",
		order_by_clause,
		maxrows );

	return strdup( sys_string );

}

LIST *query_row_dictionary_list(
				char *application_name,
				char *select_clause,
				char *from_clause,
				char *where_clause,
				char *order_clause,
				int max_rows,
				LIST *append_isa_attribute_list,
				char *login_name )
{
	LIST *row_dictionary_list = {0};
	char *sys_string;

	sys_string = 
		query_get_sys_string(
			application_name,
			select_clause,
			from_clause,
			where_clause,
			(char *)0 /* subquery_where_clause */,
			order_clause,
			max_rows );

	row_dictionary_list =
		list_usage_pipe2dictionary_list(
			sys_string, 
	 		attribute_lookup_allowed_attribute_name_list(
				append_isa_attribute_list ),
	 		attribute_date_attribute_name_list(
				append_isa_attribute_list ),
			FOLDER_DATA_DELIMITER,
			application_name,
			login_name );

	return row_dictionary_list;
}

char *query_get_between_date_time_where(
			char *date_column_name,
			char *time_column_name,
			char *begin_date,
			char *begin_time,
			char *end_date,
			char *end_time,
			char *application_name,
			char *folder_name )
{
	char where_clause[ 4066 ];
	char *where_ptr;
	char *start_end_time, *finish_begin_time;

	if ( strcmp( begin_date, end_date ) != 0 )
	{
		start_end_time = "2359";
		finish_begin_time = "0000";
	}
	else
	{
		start_end_time = end_time;
		finish_begin_time = begin_time;
	}

	where_ptr = where_clause;

	where_ptr +=
		sprintf(
		   where_ptr,
		  " ( ( %s = '%s' and %s >= '%s' and %s <= '%s' )",
			get_full_attribute_name(
				application_name,
				folder_name,
				date_column_name ),
		 	begin_date,
			get_full_attribute_name(
				application_name,
				folder_name,
				time_column_name ),
		 	begin_time,
			get_full_attribute_name(
				application_name,
				folder_name,
				time_column_name ),
			start_end_time );

	where_ptr +=
		sprintf(
		   where_ptr,
		   " or ( %s = '%s' and %s = 'null' )",
			get_full_attribute_name(
				application_name,
				folder_name,
				date_column_name ),
		 	begin_date,
			get_full_attribute_name(
				application_name,
				folder_name,
				time_column_name ) );

	where_ptr +=
		sprintf(
		   where_ptr,
		   " or ( %s = '%s' and %s = 'null' )",
			get_full_attribute_name(
				application_name,
				folder_name,
				date_column_name ),
		 	end_date,
			get_full_attribute_name(
				application_name,
				folder_name,
				time_column_name ) );

	where_ptr +=
		sprintf(
		   where_ptr,
		   " or ( %s > '%s' and %s < '%s' )",
			get_full_attribute_name(
				application_name,
				folder_name,
				date_column_name ),
		 	begin_date,
			get_full_attribute_name(
				application_name,
				folder_name,
				date_column_name ),
			end_date );

	where_ptr +=
		sprintf(
		   where_ptr,
		   " or ( %s = '%s' and %s >= '%s' and %s <= '%s' ) )",
			get_full_attribute_name(
				application_name,
				folder_name,
				date_column_name ),
		 	end_date,
			get_full_attribute_name(
				application_name,
				folder_name,
				time_column_name ),
			finish_begin_time,
			get_full_attribute_name(
				application_name,
				folder_name,
				time_column_name ),
		 	end_time );

	return strdup( where_clause );
}

LIST *query_get_record_list(	char *application_name,
				QUERY_OUTPUT *query_output,
				char *select_clause,
				char *order_clause )
{
	LIST *query_record_list;
	char *sys_string;

	sys_string =
		query_get_sys_string(
			application_name,
			select_clause,
			query_output->from_clause,
			query_output->where_clause,
			query_output->subquery_where_clause,
			order_clause,
			0 /* max_rows */ );

	query_record_list = pipe2list( sys_string );
	return query_record_list;

}

char *query_get_select_clause(
			char *application_name,
			LIST *append_isa_attribute_list )
{
	ATTRIBUTE *attribute;
	char select_clause[ 65536 ];
	char *ptr = select_clause;
	register boolean first_time = 1;
	LIST *lookup_allowed_attribute_name_list;

	if ( !list_length( append_isa_attribute_list ) )
		return (char *)0; 

	lookup_allowed_attribute_name_list =
	 	attribute_lookup_allowed_attribute_name_list(
			append_isa_attribute_list );

	list_rewind( append_isa_attribute_list );

	do {
		attribute = list_get( append_isa_attribute_list );

		if ( !attribute->datatype ) continue;

		if ( !list_exists_string(
				attribute->attribute_name,
				lookup_allowed_attribute_name_list ) )
		{
			continue;
		}

		if ( first_time )
		{
			first_time = 0;
		}
		else
		{
			ptr += sprintf( ptr, "," );
		}

		if ( attribute->folder_name )
		{
			if ( ( strcmp(	attribute->datatype,
					"current_date_time" ) == 0
			||     strcmp(	attribute->datatype,
					"date_time" ) == 0 )
			&&     attribute->width == 16 )
			{
				ptr += sprintf( ptr,
						"substr(%s.%s,1,16)",
						get_table_name(
							application_name,
							attribute->
								folder_name ),
						attribute->attribute_name );
			}
			else
			{
				ptr += sprintf( ptr,
						"%s.%s",
						get_table_name(
							application_name,
							attribute->
								folder_name ),
						attribute->attribute_name );
			}
		}
		else
		{
			ptr += sprintf( ptr,
					"%s",
					attribute->attribute_name );
		}

	} while( list_next( append_isa_attribute_list ) );

	return strdup( select_clause );
}

LIST *query_get_subquery_drop_down_list(
				LIST *exclude_attribute_name_list,
				FOLDER *root_folder,
				LIST *mto1_related_folder_list,
				LIST *mto1_append_isa_related_folder_list,
				DICTIONARY *dictionary )
{
	LIST *drop_down_list = {0};
	RELATED_FOLDER *related_folder;
	RELATED_FOLDER *sub_related_folder;
	QUERY_DROP_DOWN *drop_down;

	if ( list_rewind( mto1_related_folder_list ) )
	{
		do {
			related_folder =
				list_get(
					mto1_related_folder_list );

			if ( ( drop_down =
				query_get_subquery_drop_down(
					exclude_attribute_name_list,
					root_folder->
						folder_name,
					related_folder->
						foreign_attribute_name_list,
					related_folder->folder->attribute_list,
					dictionary,
					related_folder->
						folder->
						folder_name ) ) )
			{
				if ( !drop_down_list )
					drop_down_list = list_new();

				list_append_pointer(
					drop_down_list,
					query_drop_down );
			}

		} while( list_next( mto1_related_folder_list ) );

	} /* if ( list_rewind( mto1_related_folder_list ) ) */

	if ( !list_rewind( mto1_append_isa_related_folder_list ) )
		return drop_down_list;

	do {
		related_folder =
			list_get(
				mto1_append_isa_related_folder_list );

		if ( !related_folder->relation_type_isa ) continue;

		if ( ( drop_down =
			query_drop_down(
				exclude_attribute_name_list,
				related_folder->folder->folder_name
				/* root_folder_name */,
				related_folder->folder->folder_name
				/* dictionary_prepend_folder_name */,
				related_folder->
					folder->
					primary_attribute_name_list
					/* foreign_attribute_name_list */,
				related_folder->folder->attribute_list,
				dictionary ) ) )
		{
			if ( !drop_down_list ) drop_down_list = list_new();

			list_append_pointer( drop_down_list, query_drop_down );
		}

		if ( list_rewind( related_folder->
					folder->
					mto1_related_folder_list ) )
		{
			do {
				sub_related_folder =
					list_get(
					related_folder->
						folder->
						mto1_related_folder_list );

				if ( ( drop_down =
					query_drop_down(
						exclude_attribute_name_list,
						related_folder->
							folder->
							folder_name
						/* root_folder_name */,
						related_folder->
							folder->
							folder_name
						/* dictionary_prepend_... */,
						sub_related_folder->
						    foreign_attribute_name_list,
						sub_related_folder->
							folder->
							attribute_list,
						dictionary ) ) )
				{
					if ( !drop_down_list )
					{
						drop_down_list = list_new();
					}

					list_append_pointer(
						drop_down_list,
						query_drop_down );
				}

			} while( list_next( 
					related_folder->
						folder->
						mto1_related_folder_list ) );
		}

	} while( list_next( mto1_append_isa_related_folder_list ) );

	return drop_down_list;
}

LIST *query_get_process_drop_down_list(
				LIST *exclude_attribute_name_list,
				char *folder_name,
				LIST *mto1_recursive_related_folder_list,
				DICTIONARY *preprompt_dictionary )
{
	LIST *drop_down_list = {0};
	RELATED_FOLDER *related_folder;
	QUERY_DROP_DOWN *query_drop_down;

	if ( !list_rewind( mto1_recursive_related_folder_list ) )
		return (LIST *)0;

	do {
		related_folder =
			list_get(
				mto1_recursive_related_folder_list );

		if ( ( query_drop_down =
			query_get_process_drop_down(
					exclude_attribute_name_list,
					folder_name,
					related_folder->
						foreign_attribute_name_list,
					related_folder->folder->attribute_list,
					preprompt_dictionary ) ) )
			{
				if ( !drop_down_list )
					drop_down_list = list_new();

				list_append_pointer(
					drop_down_list,
					query_drop_down );
			}

	} while( list_next( mto1_recursive_related_folder_list ) );

	return drop_down_list;

}

LIST *query_get_insert_drop_down_list(
				char *folder_name,
				LIST *mto1_related_folder_list,
				DICTIONARY *query_dictionary )
{
	LIST *drop_down_list = {0};
	RELATED_FOLDER *related_folder;
	QUERY_DROP_DOWN *query_drop_down;

	if ( !list_rewind( mto1_related_folder_list ) ) return (LIST *)0;

	do {
		related_folder =
			list_get(
				mto1_related_folder_list );

		if ( ( query_drop_down =
			query_get_insert_drop_down(
					folder_name,
					related_folder->
						foreign_attribute_name_list,
					related_folder->folder->attribute_list,
					query_dictionary ) ) )
			{
				if ( !drop_down_list )
					drop_down_list = list_new();

				list_append_pointer(
					drop_down_list,
					query_drop_down );
			}

	} while( list_next( mto1_related_folder_list ) );

	return drop_down_list;
}

LIST *query_primary_data_drop_down_list(
			LIST *exclude_attribute_name_list,
			FOLDER *root_folder,
			LIST *mto1_related_folder_list,
			LIST *mto1_append_isa_related_folder_list,
			DICTIONARY *dictionary,
			boolean include_root_folder )
{
	LIST *drop_down_list = list_new();
	RELATED_FOLDER *related_folder;
	RELATED_FOLDER *sub_related_folder;
	QUERY_DROP_DOWN *drop_down;

	if ( include_root_folder )
	{
		if ( ( drop_down =
			query_drop_down(
				exclude_attribute_name_list,
				root_folder->folder_name,
				root_folder->folder_name
					/* dictionary_prepend_folder_name */,
				root_folder->primary_attribute_name_list
					/* foreign_attribute_name_list */,
				root_folder->attribute_list,
				dictionary ) ) )
		{
			list_set( drop_down_list, drop_down );
		}
	}

	if ( !list_rewind( mto1_related_folder_list ) )
		goto check_mto1_append_isa_related_folder_list;

	do {
		related_folder =
			list_get(
				mto1_related_folder_list );

		if ( ( drop_down =
			query_primary_data_drop_down(
				exclude_attribute_name_list,
				root_folder->
					folder_name,
				related_folder->
					folder->
					folder_name
				   /* dictionary_prepend_folder_name */,
				related_folder->
					foreign_attribute_name_list,
				related_folder->folder->attribute_list,
				dictionary ) ) )
		{
			list_set(
				drop_down_list,
				drop_down );
		}

	} while( list_next( mto1_related_folder_list ) );

check_mto1_append_isa_related_folder_list:

	if ( !list_rewind( mto1_append_isa_related_folder_list ) )
	{
		return drop_down_list;
	}

	do {
		related_folder =
			list_get(
				mto1_append_isa_related_folder_list );

		if ( !related_folder->relation_type_isa ) continue;

		if ( !list_rewind( related_folder->
					folder->
					mto1_related_folder_list ) )
		{
			continue;
		}

		do {
			sub_related_folder =
				list_get(
				related_folder->
					folder->
					mto1_related_folder_list );

			if ( ( drop_down =
				query_primary_data_drop_down(
					exclude_attribute_name_list,
					related_folder->
						folder->
						folder_name
							/* root_folder_name */,
					related_folder->
						folder->
						folder_name
					   /* dictionary_prepend_folder_name */,
					sub_related_folder->
					    foreign_attribute_name_list,
					sub_related_folder->
						folder->
						attribute_list,
					dictionary ) ) )
			{
				list_set(
					drop_down_list,
					drop_down );
			}

		} while( list_next( 
				related_folder->
					folder->
					mto1_related_folder_list ) );

	} while( list_next( mto1_append_isa_related_folder_list ) );

	return drop_down_list;
}

LIST *query_process_generic_drop_down_list(
				char *value_folder_name,
				LIST *mto1_related_folder_list,
				DICTIONARY *dictionary )
{
	LIST *drop_down_list = {0};
	RELATED_FOLDER *related_folder;
	QUERY_DROP_DOWN *query_drop_down;

	if ( !list_rewind( mto1_related_folder_list ) ) return (LIST *)0;

	do {
		related_folder =
			list_get(
				mto1_related_folder_list );

		if ( ! ( query_drop_down =
			query_process_drop_down(
				related_folder->foreign_attribute_name_list,
				related_folder->folder->attribute_list,
				dictionary ) ) )
		{
			query_drop_down =
				query_process_multi_drop_down(
					value_folder_name,
					related_folder->
						foreign_attribute_name_list,
					related_folder->
						folder->
						attribute_list,
					dictionary );
		}

		if ( query_drop_down )
		{
			if ( !drop_down_list )
				drop_down_list = list_new();

			list_append_pointer(
				drop_down_list,
				query_drop_down );
		}

	} while( list_next( mto1_related_folder_list ) );

	return drop_down_list;
}

LIST *query_process_drop_down_get_drop_down_list(
				LIST *mto1_related_folder_list,
				DICTIONARY *dictionary )
{
	LIST *drop_down_list = {0};
	RELATED_FOLDER *related_folder;
	QUERY_DROP_DOWN *query_drop_down;

	if ( !list_rewind( mto1_related_folder_list ) ) return (LIST *)0;

	do {
		related_folder =
			list_get(
				mto1_related_folder_list );

		if ( ( query_drop_down =
			query_process_drop_down(
				related_folder->foreign_attribute_name_list,
				related_folder->folder->attribute_list,
				dictionary ) ) )
		{
			if ( !drop_down_list )
				drop_down_list = list_new();

			list_append_pointer(
				drop_down_list,
				query_drop_down );
		}

	} while( list_next( mto1_related_folder_list ) );

	return drop_down_list;
}

LIST *query_drop_down_list(	LIST *exclude_attribute_name_list,
				FOLDER *root_folder,
				LIST *mto1_related_folder_list,
				LIST *mto1_append_isa_related_folder_list,
				DICTIONARY *dictionary,
				boolean include_root_folder )
{
	LIST *drop_down_list = {0};
	RELATED_FOLDER *related_folder;
	RELATED_FOLDER *sub_related_folder;
	QUERY_DROP_DOWN *drop_down;
	LIST *foreign_attribute_name_list;

	if ( include_root_folder )
	{
		if ( ( drop_down =
			query_drop_down(
				exclude_attribute_name_list,
				root_folder->folder_name,
				root_folder->folder_name
					/* dictionary_prepend_folder_name */,
				root_folder->primary_attribute_name_list
					/* foreign_attribute_name_list */,
				root_folder->attribute_list,
				dictionary ) ) )
		{
			drop_down_list = list_new();
			list_append_pointer( drop_down_list, query_drop_down );
		}
	}

	if ( !list_rewind( mto1_related_folder_list ) )
		goto check_mto1_append_isa_related_folder_list;

	do {
		related_folder =
			list_get(
				mto1_related_folder_list );


		if ( related_folder->folder_foreign_attribute_name_list )
		{
			foreign_attribute_name_list =
				related_folder->
					folder_foreign_attribute_name_list;
		}
		else
		{
			foreign_attribute_name_list =
				related_folder->foreign_attribute_name_list;
		}

		if ( ( drop_down =
			query_drop_down(
				exclude_attribute_name_list,
				root_folder->
					folder_name,
				related_folder->
					folder->
					folder_name
				   /* dictionary_prepend_folder_name */,
				foreign_attribute_name_list,
				related_folder->folder->attribute_list,
				dictionary ) ) )
		{
			if ( !drop_down_list )
				drop_down_list = list_new();

			list_append_pointer(
				drop_down_list,
				query_drop_down );
		}

	} while( list_next( mto1_related_folder_list ) );

check_mto1_append_isa_related_folder_list:

	if ( !list_rewind( mto1_append_isa_related_folder_list ) )
		return drop_down_list;

	do {
		related_folder =
			list_get(
				mto1_append_isa_related_folder_list );

		if ( !related_folder->relation_type_isa ) continue;

		if ( !list_rewind( related_folder->
					folder->
					mto1_related_folder_list ) )
		{
			continue;
		}

		do {
			sub_related_folder =
				list_get(
				related_folder->
					folder->
					mto1_related_folder_list );

			if ( ( drop_down =
				query_drop_down(
					exclude_attribute_name_list,
					related_folder->
						folder->
						folder_name
							/* root_folder_name */,
					related_folder->
						folder->
						folder_name
					   /* dictionary_prepend_folder_name */,
					sub_related_folder->
					    foreign_attribute_name_list,
					sub_related_folder->
						folder->
						attribute_list,
					dictionary ) ) )
			{
				if ( !drop_down_list )
				{
					drop_down_list = list_new();
				}

				list_append_pointer(
					drop_down_list,
					query_drop_down );
			}

		} while( list_next( 
				related_folder->
					folder->
					mto1_related_folder_list ) );

	} while( list_next( mto1_append_isa_related_folder_list ) );

	return drop_down_list;
}

QUERY_DROP_DOWN *query_get_subquery_drop_down(
				LIST *exclude_attribute_name_list,
				char *folder_name,
				LIST *foreign_attribute_name_list,
				LIST *attribute_list,
				DICTIONARY *dictionary,
				char *dictionary_prepend_folder_name )
{
	QUERY_DROP_DOWN *query_drop_down = {0};
	int highest_index;
	int index;

	highest_index =
		dictionary_attribute_name_list_get_highest_index(
			dictionary,
			foreign_attribute_name_list,
			dictionary_prepend_folder_name );

	if ( highest_index == -1 )
		return (QUERY_DROP_DOWN *)0;

	for( index = 0; index <= highest_index; index++ )
	{
		query_drop_down =
			query_row_drop_down(
				exclude_attribute_name_list,
				query_drop_down,
				folder_name,
				foreign_attribute_name_list,
				attribute_list,
				dictionary,
				index,
				dictionary_prepend_folder_name );
	}

	return query_drop_down;
}

QUERY_DROP_DOWN *query_get_process_drop_down(
				LIST *exclude_attribute_name_list,
				char *folder_name,
				LIST *foreign_attribute_name_list,
				LIST *attribute_list,
				DICTIONARY *preprompt_dictionary )
{
	QUERY_DROP_DOWN *query_drop_down;
	QUERY_DROP_DOWN_ROW *drop_down_row;
	char *attribute_name;
	LIST *data_list = {0};
	char key[ 1024 ];
	char *data;
	int highest_index;
	int index;

	if ( !list_length( foreign_attribute_name_list ) )
		return (QUERY_DROP_DOWN *)0;

	highest_index =
		dictionary_attribute_name_list_get_highest_index(
			preprompt_dictionary,
			foreign_attribute_name_list,
			(char *)0 /* dictionary_prepend_folder_name */ );

	if ( highest_index == -1 ) return (QUERY_DROP_DOWN *)0;

	query_drop_down = query_drop_down_new( strdup( folder_name ) );

	list_rewind( foreign_attribute_name_list );

	do {
		attribute_name =
			list_get( foreign_attribute_name_list );

		data_list = (LIST *)0;

		for( index = 0; index <= highest_index; index++ )
		{
			sprintf( key, "%s_%d", attribute_name, index );

			if (  ( data = dictionary_fetch(
					preprompt_dictionary,
					key ) ) )
			{
				if ( !data_list ) data_list = list_new();

				list_append_pointer( data_list, data );
			}
		}

		if ( list_length( data_list ) )
		{
			if ( ! ( drop_down_row =
				query_process_drop_down_row_new(
					attribute_name,
					attribute_list,
					data_list ) ) )
			{
				fprintf( stderr,
"ERROR in %s/%s()/%d: cannot get query_process_drop_down_row_new()\n",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__ );
				exit( 1 );
			}

			list_append_pointer(
				query_drop_down->
					query_drop_down_row_list,
				drop_down_row );

			if ( exclude_attribute_name_list )
			{
				list_append_pointer(
					exclude_attribute_name_list,
					attribute_name );
			}
		}

	} while( list_next( foreign_attribute_name_list ) );

	return query_drop_down;

}

QUERY_DROP_DOWN *query_get_insert_drop_down(
				char *folder_name,
				LIST *foreign_attribute_name_list,
				LIST *attribute_list,
				DICTIONARY *query_dictionary )
{
	QUERY_DROP_DOWN *query_drop_down;
	QUERY_DROP_DOWN_ROW *drop_down_row;
	char *attribute_name;
	LIST *data_list = {0};
	char key[ 1024 ];
	char *data;

	if ( !list_rewind( foreign_attribute_name_list ) )
		return (QUERY_DROP_DOWN *)0;

	do {
		attribute_name = list_get( foreign_attribute_name_list );

		sprintf( key, "%s_1", attribute_name );

		if ( ! ( data = dictionary_fetch( query_dictionary, key ) ) )
		{
			sprintf( key, "%s_0", attribute_name );

			if ( ! ( data = dictionary_fetch(
						query_dictionary,
						key ) ) )
			{
				continue;
			}
		}

		if ( !data_list ) data_list = list_new();

		list_append_pointer( data_list, data );

	} while( list_next( foreign_attribute_name_list ) );

	if ( !list_length( data_list ) )
	{
		return (QUERY_DROP_DOWN *)0;
	}

	if ( ! ( drop_down_row =
			query_drop_down_row_new(
				foreign_attribute_name_list,
				attribute_list,
				data_list ) ) )
	{
		return (QUERY_DROP_DOWN *)0;
	}

	query_drop_down = query_drop_down_new( strdup( folder_name ) );

	list_append_pointer(	query_drop_down->query_drop_down_row_list,
				drop_down_row );

	return query_drop_down;
}

QUERY_DROP_DOWN *query_primary_data_drop_down(
				LIST *exclude_attribute_name_list,
				char *root_folder_name,
				char *dictionary_prepend_folder_name,
				LIST *foreign_attribute_name_list,
				LIST *attribute_list,
				DICTIONARY *dictionary )
{
	QUERY_DROP_DOWN *query_drop_down = {0};
	int highest_index;
	int index;

	highest_index =
		dictionary_attribute_name_list_get_highest_index(
			dictionary,
			foreign_attribute_name_list,
			dictionary_prepend_folder_name );

	if ( highest_index == -1 )
		return (QUERY_DROP_DOWN *)0;

	for( index = 0; index <= highest_index; index++ )
	{
		query_drop_down =
			query_row_drop_down(
				exclude_attribute_name_list,
				query_drop_down,
				root_folder_name,
				foreign_attribute_name_list,
				attribute_list,
				dictionary,
				index,
				dictionary_prepend_folder_name );
	}

	return query_drop_down;
}

QUERY_DROP_DOWN *query_process_drop_down(
				LIST *foreign_attribute_name_list,
				LIST *attribute_list,
				DICTIONARY *dictionary )
{
	QUERY_DROP_DOWN *query_drop_down;
	LIST *data_list = {0};
	QUERY_DROP_DOWN_ROW *drop_down_row;
	char key[ 1024 ];
	char *string_delimited;
	char *data_list_string;

	if ( !list_length( foreign_attribute_name_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: empty foreign_attribute_name_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	string_delimited =
		list2string_delimited(
			foreign_attribute_name_list,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );

	sprintf( key, "%s_0", string_delimited );

	if ( ( data_list_string = dictionary_fetch( dictionary, key ) ) )
	{
		data_list =
			list_string2list(
				data_list_string,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );

		if ( ! ( drop_down_row =
				query_drop_down_row_new(
					foreign_attribute_name_list,
					attribute_list,
					data_list ) ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: can't get query_drop_down_row_new().\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__ );
			exit( 1 );
		}

		query_drop_down =
			query_drop_down_new(
				(char *)0 /* folder_name */ );

		list_append_pointer(
			query_drop_down->query_drop_down_row_list,
			drop_down_row );

		return query_drop_down;
	}

	return (QUERY_DROP_DOWN *)0;
}

QUERY_DROP_DOWN *query_process_multi_drop_down(
				char *value_folder_name,
				LIST *foreign_attribute_name_list,
				LIST *attribute_list,
				DICTIONARY *dictionary )
{
	QUERY_DROP_DOWN *query_drop_down = {0};
	int highest_index;
	int index;

	if ( !list_length( foreign_attribute_name_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: empty foreign_attribute_name_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	highest_index =
		dictionary_attribute_name_list_get_highest_index(
			dictionary,
			foreign_attribute_name_list,
			(char *)0 /* dictionary_prepend_folder_name */ );

	if ( highest_index <= 0 )
		return (QUERY_DROP_DOWN *)0;

	for ( index = 1; index <= highest_index; index++ )
	{
		query_drop_down =
			query_row_drop_down(
				(LIST *)0 /* exclude_attribute_name_list */,
				query_drop_down,
				value_folder_name /* root_folder_name */,
				foreign_attribute_name_list,
				attribute_list,
				dictionary,
				index,
				(char *)0
					/* dictionary_prepend_folder_name  */ );
	}

	return query_drop_down;
}

QUERY_DROP_DOWN *query_drop_down(
				LIST *exclude_attribute_name_list,
				char *root_folder_name,
				char *dictionary_prepend_folder_name,
				LIST *foreign_attribute_name_list,
				LIST *attribute_list,
				DICTIONARY *dictionary )
{
	QUERY_DROP_DOWN *drop_down = {0};
	int highest_index;
	int index;

	highest_index =
		dictionary_attribute_name_list_get_highest_index(
			dictionary,
			foreign_attribute_name_list,
			dictionary_prepend_folder_name );

	if ( highest_index == -1 )
		return (QUERY_DROP_DOWN *)0;

	for( index = 0; index <= highest_index; index++ )
	{
		drop_down =
			query_row_drop_down(
				exclude_attribute_name_list,
				drop_down,
				root_folder_name,
				foreign_attribute_name_list,
				attribute_list,
				dictionary,
				index,
				dictionary_prepend_folder_name );
	}

	return drop_down;
}

QUERY_DROP_DOWN_ROW *query_process_drop_down_row_new(
					char *attribute_name,
					LIST *attribute_list,
					LIST *data_list )
{
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	char *datatype;
	LIST *attribute_name_list;
	LIST *local_data_list;
	char *data;
	ATTRIBUTE *attribute;

	if ( !list_rewind( data_list ) )
		return (QUERY_DROP_DOWN_ROW *)0;

	if ( !( attribute =
			attribute_seek_attribute( 
				attribute_name,
				attribute_list ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot seek attribute_name = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 attribute_name );
		exit( 1 );
	}

	datatype = attribute->datatype;

	local_data_list = list_new();

	attribute_name_list = list_new();
	list_append_pointer( attribute_name_list, attribute_name );

	do {
		data = list_get( data_list );

		if ( strcmp( datatype,
				"current_date" ) == 0
		||   strcmp( datatype,
				"current_date_time" ) == 0
		||   strcmp( datatype,
				"current_time" ) == 0
		||   strcmp( datatype,
				"date" ) == 0
		||   strcmp( datatype,
				"date_time" ) == 0 )
		{
			query_convert_date_international( &data );
		}

		list_set(
			local_data_list,
			timlib_sql_injection_escape( data ) );

	} while( list_next( data_list ) );

	query_drop_down_row = query_drop_down_row_calloc();
	query_drop_down_row->attribute_name_list = attribute_name_list;
	query_drop_down_row->data_list = local_data_list;

	return query_drop_down_row;

}

QUERY_DROP_DOWN_ROW *query_drop_down_row_calloc( void )
{
	QUERY_DROP_DOWN_ROW *query_drop_down_row;

	if ( ! ( query_drop_down_row =
			calloc( 1, sizeof( QUERY_DROP_DOWN_ROW ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: memory allocation error.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (QUERY_DROP_DOWN_ROW *)0;
	}
	return query_drop_down_row;
}

QUERY_DROP_DOWN_ROW *query_drop_down_row_new(
					LIST *attribute_name_list,
					LIST *attribute_list,
					LIST *data_list )
{
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	LIST *local_attribute_name_list = {0};
	LIST *local_data_list = {0};
	char *attribute_name;
	char *data;
	ATTRIBUTE *attribute;

	if ( !list_length( attribute_name_list )
	|| ( list_length( attribute_name_list ) != list_length( data_list ) ) )
	{
		return (QUERY_DROP_DOWN_ROW *)0;
	}

	list_rewind( attribute_name_list );
	list_rewind( data_list );

	do {
		data = list_get( data_list );

		if ( *data )
		{
			attribute_name = list_get( attribute_name_list );

			if ( ( attribute =
				attribute_seek_attribute( 
					attribute_name,
					attribute_list ) ) )
			{
				char *datatype;

				datatype = attribute->datatype;

				if ( strcmp( datatype,
						"current_date" ) == 0
				||   strcmp( datatype,
						"current_date_time" ) == 0
				||   strcmp( datatype,
						"current_time" ) == 0
				||   strcmp( datatype,
						"date" ) == 0
				||   strcmp( datatype,
						"date_time" ) == 0 )
				{
					query_convert_date_international(
						&data );
				}
			}

			if ( !local_attribute_name_list )
			{
				local_attribute_name_list = list_new();
				local_data_list = list_new();
			}

			list_set_pointer(
				local_attribute_name_list,
				attribute_name );

			list_set_pointer(
				local_data_list,
				timlib_sql_injection_escape( data ) );
		}
		else
		{
			return (QUERY_DROP_DOWN_ROW *)0;
		}

		list_next( data_list );

	} while( list_next( attribute_name_list ) );

	if ( ! ( query_drop_down_row =
			calloc( 1, sizeof( QUERY_DROP_DOWN_ROW ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: memory allocation error.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (QUERY_DROP_DOWN_ROW *)0;
	}

	query_drop_down_row->attribute_name_list = local_attribute_name_list;
	query_drop_down_row->data_list = local_data_list;

	return query_drop_down_row;
}

QUERY_DROP_DOWN *query_drop_down_new( char *folder_name )
{
	QUERY_DROP_DOWN *query_drop_down;

	if ( ! ( query_drop_down = calloc( 1, sizeof( QUERY_DROP_DOWN ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: memory allocation error.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query_drop_down->folder_name = folder_name;
	query_drop_down->query_drop_down_row_list = list_new();

	return query_drop_down;
}

QUERY_DROP_DOWN *query_row_drop_down(
				LIST *exclude_attribute_name_list,
				QUERY_DROP_DOWN *query_drop_down,
				char *root_folder_name,
				LIST *foreign_attribute_name_list,
				LIST *attribute_list,
				DICTIONARY *dictionary,
				int index,
				char *dictionary_prepend_folder_name )
{
	LIST *data_list = {0};
	QUERY_DROP_DOWN_ROW *drop_down_row;
	char key[ 1024 ];
	char *string_delimited;
	char *data_list_string;
	char *operator_name;
	enum relational_operator relational_operator;

	if ( !list_length( foreign_attribute_name_list ) )
		return query_drop_down;

	string_delimited =
		list2string_delimited(
			foreign_attribute_name_list,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );

	sprintf( key, "%s_%d", string_delimited, index );

	if ( ! ( data_list_string = dictionary_fetch( dictionary, key ) ) )
	{
		if ( dictionary_prepend_folder_name
		&&   *dictionary_prepend_folder_name )
		{
			sprintf(key,
				"%s.%s_%d",
				dictionary_prepend_folder_name,
				string_delimited,
				index );
		}

		if ( ! ( data_list_string =
				dictionary_fetch( dictionary, key ) ) )
		{
			return query_drop_down;
		}
	}

	if ( strcmp( data_list_string, NULL_OPERATOR ) == 0 )
	{
		int length = list_length( foreign_attribute_name_list );
		int i;

		data_list = list_new();

		for(	i = 0;
			i < length;
			i++ )
		{
			list_append_pointer( data_list, NULL_OPERATOR );
		}
	}
	else
	if ( strcmp( data_list_string, NOT_NULL_OPERATOR ) == 0 )
	{
		int length = list_length( foreign_attribute_name_list );
		int i;

		data_list = list_new();

		for(	i = 0;
			i < length;
			i++ )
		{
			list_append_pointer( data_list, NOT_NULL_OPERATOR );
		}
	}
	else
	{
		data_list =
			list_string2list(
				data_list_string,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );
	}

	if ( !list_length( data_list ) ) return query_drop_down;

	operator_name =
		query_get_operator_name(
			key /* attribute_name */,
			dictionary,
			0 /* dictionary_offset */,
			QUERY_RELATION_OPERATOR_STARTING_LABEL,
			(char *)0
			/* dictionary_prepend_folder_name */ );

	relational_operator = query_get_relational_operator( operator_name );

	/* If multi-drop-down, then no relational_operator is okay. */
	/* -------------------------------------------------------- */
	if ( index && relational_operator == relational_operator_empty )
	{
		goto query_get_drop_down_continue;
	}

	if ( relational_operator != equals )
	{
		return query_drop_down;
	}

query_get_drop_down_continue:

	if ( ! ( drop_down_row =
			query_drop_down_row_new(
				foreign_attribute_name_list,
				attribute_list,
				data_list ) ) )
	{
		return query_drop_down;
	}

	if ( !query_drop_down )
	{
		query_drop_down =
			query_drop_down_new(
				strdup( root_folder_name ) );
	}
	else
	if ( query_data_list_accounted_for(
		query_drop_down->query_drop_down_row_list,
		data_list ) )
	{
		return query_drop_down;
	}

	if ( exclude_attribute_name_list )
	{
		list_append_list(	exclude_attribute_name_list,
					foreign_attribute_name_list );
	}

	list_append_pointer(	query_drop_down->query_drop_down_row_list,
				drop_down_row );

	return query_drop_down;
}

LIST *query_get_drop_down_data_list(
			LIST *attribute_name_list,
			DICTIONARY *dictionary,
			int index )
{
	LIST *drop_down_data_list = list_new();
	char *operator_name;
	char *attribute_name;
	char *data;

	if ( !list_rewind( attribute_name_list ) ) return (LIST *)0;

	do {
		attribute_name =
			list_get_pointer(
				attribute_name_list );

		if ( !( operator_name =
			query_get_operator_name(
				attribute_name,
				dictionary,
				0 /* dictionary_offset */,
				QUERY_RELATION_OPERATOR_STARTING_LABEL,
				(char *)0
				/* dictionary_prepend_folder_name */ ) ) )
		{
			return (LIST *)0;
		}

		if ( strcmp( operator_name, EQUAL_OPERATOR ) != 0 )
		{
			return (LIST *)0;
		}

		if ( query_get_drop_down_dictionary_data(
				&data,
				dictionary, 
				attribute_name,
				index ) )
		{
			list_append_pointer( drop_down_data_list, data );
		}
		else
		{
			return (LIST *)0;
		}

	} while( list_next( attribute_name_list ) );

	return drop_down_data_list;
}

boolean query_get_drop_down_dictionary_data(
			char **data,
			DICTIONARY *dictionary, 
			char *primary_attribute_name,
			int dictionary_offset )
{
	int return_value;

	return_value =
		dictionary_get_index_data( 
			data,
			dictionary, 
			primary_attribute_name,
			dictionary_offset );

	*data = timlib_sql_injection_escape( *data );

	if ( return_value == -1 )
	{
		return 0;
	}
	else
	if ( ( return_value == 0 ) && ( dictionary_offset == 0 ) )
	{
		return 1;
	}
	else
	if ( ( return_value == -1 ) && ( dictionary_offset == 0 ) )
	{
		return 0;
	}
	if ( ( return_value >= 0  ) && ( dictionary_offset ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

char *query_get_operator_name(	char *attribute_name,
				DICTIONARY *dictionary,
				int dictionary_offset,
				char *starting_label,
				char *dictionary_prepend_folder_name )
{
	char search_string[ 1024 ];
	char *operator_name;

	sprintf( search_string,
		 "%s%s",
		 starting_label,
		 attribute_name );

	if ( ( operator_name =
		dictionary_fetch(
			dictionary,
			search_string ) ) )
	{
		return operator_name;
	}

	if ( dictionary_get_index_data(
			&operator_name,
			dictionary, 
			search_string,
			dictionary_offset ) < 0 )
	{
		if ( dictionary_get_index_data(
				&operator_name,
				dictionary, 
				search_string,
				0 ) < 0 )
		{
			if ( dictionary_prepend_folder_name
			&&   *dictionary_prepend_folder_name )
			{
				sprintf(search_string,
					"%s.%s%s",
					dictionary_prepend_folder_name,
					starting_label,
					attribute_name );

				if ( dictionary_get_index_data(
						&operator_name,
						dictionary, 
						search_string,
						0 ) < 0 )
				{
					return (char *)0;
				}
			}
			else
			{
				return (char *)0;
			}
		}
	}

	return operator_name;

}

void query_convert_date_international( char **data )
{
	static char date_international_string[ 16 ];
	enum date_convert_format date_convert_format;
	DATE_CONVERT *date;

	if ( !data || !*data ) return;

	date_convert_format =
		date_convert_date_get_format(
			*data );

	if ( date_convert_format == american )
	{
		/* Source American */
		/* --------------- */
		date = date_convert_new_date_convert( 
				international,
				*data );

		strcpy( date_international_string, date->return_date );
		*data = date_international_string;
		date_convert_free( date );
	}
	else
	if ( date_convert_format == military )
	{
		/* Source Military */
		/* --------------- */
		date = date_convert_new_date_convert( 
					international,
					*data );

		strcpy( date_international_string, date->return_date );
		*data = date_international_string;
		date_convert_free( date );
	}
}

boolean query_get_dictionary_data(
			char **data,
			DICTIONARY *dictionary, 
			char *attribute_name,
			int dictionary_offset,
			char *starting_label,
			char *dictionary_prepend_folder_name )
{
	char search_string[ 1024 ];

	if ( starting_label && *starting_label )
	{
		sprintf(search_string,
	 		"%s%s",
	 		starting_label,
	 		attribute_name );
	}
	else
	{
		strcpy( search_string, attribute_name );
	}

	if ( dictionary_get_index_data( 
			data,
			dictionary, 
			search_string,
			dictionary_offset ) == -1  )
	{
		if ( dictionary_get_index_data( 
				data,
				dictionary, 
				search_string,
				0 ) == -1  )
		{
			if ( dictionary_get_index_data( 
					data,
					dictionary, 
					attribute_name,
					dictionary_offset ) == -1  )
			{
				if (	starting_label
				&&	*starting_label
				&&	dictionary_prepend_folder_name
				&&	*dictionary_prepend_folder_name )
				{
					sprintf(search_string,
	 					"%s.%s%s",
						dictionary_prepend_folder_name,
	 					starting_label,
	 					attribute_name );

					if ( dictionary_get_index_data( 
							data,
							dictionary, 
							search_string,
							0 ) > -1  )
					{
					       query_convert_date_international(
							data );
						return 1;
					}
				}

				if ( ( *data = dictionary_get_data( 
							dictionary, 
							attribute_name ) ) )
				{
					query_convert_date_international(
							data );
					return 1;
				}
				else
				{
					return 0;
				}
			}
			else
			{
				query_convert_date_international( data );
				return 1;
			}
		}
		else
		{
			query_convert_date_international( data );
			return 1;
		}
	}
	else
	{
		query_convert_date_international( data );
		return 1;
	}
}

char *query_get_data_escaped(
			DICTIONARY *dictionary,
			char *attribute_name,
			int dictionary_offset,
			char *starting_label,
			char *dictionary_prepend_folder_name )
{
	char *from_data = (char *)0;
	char *from_data_escaped;
	boolean results;

	results = query_get_dictionary_data(
			&from_data,
			dictionary, 
			attribute_name,
			dictionary_offset,
			starting_label,
			dictionary_prepend_folder_name );

	if ( !results || !from_data || !*from_data ) return (char *)0;

	from_data_escaped =
		timlib_sql_injection_escape(
			from_data );

	return strdup( from_data_escaped );
}

QUERY_SUBQUERY *query_subquery_new(
				char *folder_name,
				LIST *foreign_attribute_name_list )
{
	QUERY_SUBQUERY *query_subquery;

	if ( ! ( query_subquery = calloc( 1, sizeof( QUERY_SUBQUERY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: memory allocation error.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query_subquery->folder_name = folder_name;

	query_subquery->foreign_attribute_name_list =
		foreign_attribute_name_list;

	return query_subquery;
}

QUERY_ATTRIBUTE *query_attribute_new(
			char *attribute_name,
			char *folder_name,
			char *datatype,
			enum relational_operator relational_operator,
			char *from_data,
			char *to_data,
			int primary_key_index )
{
	QUERY_ATTRIBUTE *query_attribute;

	if ( ! ( query_attribute = calloc( 1, sizeof( QUERY_ATTRIBUTE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: memory allocation error.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( datatype
	&&   ( strcmp( datatype, "float" ) == 0
	||     strcmp( datatype, "integer" ) == 0
	||     strcmp( datatype, "reference_number" ) == 0 ) )
	{
		timlib_remove_character( from_data, ',' );
		timlib_remove_character( from_data, '$' );
		timlib_remove_character( to_data, ',' );
		timlib_remove_character( to_data, '$' );
	}

	query_attribute->attribute_name = attribute_name;
	query_attribute->folder_name = folder_name;
	query_attribute->datatype = datatype;
	query_attribute->relational_operator = relational_operator;
	query_attribute->from_data = from_data;
	query_attribute->to_data = to_data;
	query_attribute->primary_key_index = primary_key_index;

	return query_attribute;

}

LIST *query_get_attribute_list(
			LIST *append_isa_attribute_list,
			DICTIONARY *dictionary,
			LIST *exclude_attribute_name_list,
			char *dictionary_prepend_folder_name )
{
	LIST *query_attribute_list = {0};
	QUERY_ATTRIBUTE *query_attribute;
	ATTRIBUTE *attribute;
	char *from_data;
	char *to_data;
	char *operator_name;
	enum relational_operator relational_operator;

	if ( !list_rewind( append_isa_attribute_list ) )
		return (LIST *)0;

	do {
		attribute =
			list_get_pointer(
				append_isa_attribute_list );

		if ( list_exists_string(
				attribute->attribute_name,
				exclude_attribute_name_list ) )
		{
			continue;
		}

		if ( query_attribute_list_exists(
				query_attribute_list,
				attribute->attribute_name ) )
		{
			continue;
		}

		if ( !( operator_name =
			query_get_operator_name(
				attribute->attribute_name,
				dictionary,
				0 /* dictionary_offset */,
				QUERY_RELATION_OPERATOR_STARTING_LABEL,
				dictionary_prepend_folder_name ) ) )
		{
			int str_len;

			str_len = strlen( attribute->attribute_name );

			if ( strncmp(	attribute->attribute_name +
					str_len - 3,
					"_yn",
					3 ) == 0 )
			{
				operator_name = EQUAL_OPERATOR;
			}
			else
			{
				continue;
			}
		}

		relational_operator =
			query_get_relational_operator(
				operator_name );

		if ( ( timlib_strcmp(	attribute->datatype,
					"date_time" ) == 0
		||     timlib_strcmp(	attribute->datatype,
					"current_date_time" ) == 0 )
		&& relational_operator == equals )
		{
			relational_operator = begins;
		}

		from_data = (char *)0;
		to_data = (char *)0;

		if ( relational_operator != is_null
		&&   relational_operator != not_null
		&&   ! ( from_data =
				query_get_data_escaped(
					dictionary,
					attribute->attribute_name,
					0 /* dictionary_offset */,
					QUERY_FROM_STARTING_LABEL,
					dictionary_prepend_folder_name ) ) )
		{
			continue;
		}

		to_data =
			query_get_data_escaped(
				dictionary, 
				attribute->attribute_name,
				0 /* dictionary_offset */,
				QUERY_TO_STARTING_LABEL,
				dictionary_prepend_folder_name );

		query_attribute =
			query_attribute_new(
				attribute->attribute_name,
				attribute->folder_name,
				attribute->datatype,
				relational_operator,
				from_data,
				to_data,
				attribute->primary_key_index );

		if ( !query_attribute_list )
			query_attribute_list = list_new();

		list_append_pointer(
			query_attribute_list,
			query_attribute );

	} while( list_next( append_isa_attribute_list ) );

	return query_attribute_list;
}

enum relational_operator query_get_relational_operator(
				char *operator_name )
{
	if ( !operator_name || !*operator_name )
		return relational_operator_empty;
	else
	if ( strcmp( operator_name, EQUAL_OPERATOR ) == 0 )
		return equals;
	else
	if ( strcmp( operator_name, NOT_EQUAL_OPERATOR ) == 0 )
		return not_equal;
	else
	if ( strcmp( operator_name, NOT_EQUAL_OR_NULL_OPERATOR ) == 0 )
		return not_equal_or_null;
	else
	if ( strcmp( operator_name, LESS_THAN_OPERATOR ) == 0 )
		return less_than;
	else
	if ( strcmp( operator_name, LESS_THAN_EQUAL_TO_OPERATOR ) == 0 )
		return less_than_equal_to;
	else
	if ( strcmp( operator_name, GREATER_THAN_OPERATOR ) == 0 )
		return greater_than;
	else
	if ( strcmp( operator_name, GREATER_THAN_EQUAL_TO_OPERATOR ) == 0 )
		return greater_than_equal_to;
	else
	if ( strcmp( operator_name, BETWEEN_OPERATOR ) == 0 )
		return between;
	else
	if ( strcmp( operator_name, BEGINS_OPERATOR ) == 0 )
		return begins;
	else
	if ( strcmp( operator_name, CONTAINS_OPERATOR ) == 0 )
		return contains;
	else
	if ( strcmp( operator_name, NOT_CONTAINS_OPERATOR ) == 0 )
		return not_contains;
	else
	if ( strcmp( operator_name, OR_OPERATOR ) == 0 )
		return query_or;
	else
	if ( strcmp( operator_name, NULL_OPERATOR ) == 0 )
		return is_null;
	else
	if ( strcmp( operator_name, NOT_NULL_OPERATOR ) == 0 )
		return not_null;
	else
		return relational_operator_unknown;

}

char *query_get_display_where_clause(
				char *where_clause,
				char *application_name,
				char *folder_name,
				boolean is_primary_application )
{
	char remove_folder_name[ 256 ];

	if ( !where_clause ) return "";

	if ( folder_name && *folder_name )
	{
		if ( is_primary_application )
		{
			sprintf(remove_folder_name,
				"%s.",
				folder_name );
			search_replace_string(
				where_clause,
				remove_folder_name,
				"" );
		}
		else
		{
			sprintf(remove_folder_name,
				"%s_%s.",
				application_name,
				folder_name );
			search_replace_string(
				where_clause,
				remove_folder_name,
				"" );
		}
	}

	search_replace_string(	where_clause,
				"1 = 1 and ",
				"" );
	if ( !*where_clause )
		return "Entire folder";
	else
		return where_clause;
}

char *query_append_where_clause(	char *source_where_clause,
					char *append_where_clause )
{
	char where_clause[ 65536 ];

	*where_clause = '\0';
	timlib_strcpy( where_clause, source_where_clause, 65536 );
	timlib_strcat( where_clause, " and " );
	timlib_strcat( where_clause, append_where_clause );

	/* free( source_where_clause ); */
	return strdup( where_clause );
}

char *query_append_folder_name(	char *folder_name,
				char *append_folder_name )
{
	char appended_folder_name[ 1024 ];

	sprintf( appended_folder_name,
		 "%s,%s",
		 folder_name,
		 append_folder_name );

	return strdup( appended_folder_name );
}

char *query_append_where_clause_related_join(
			char *application_name,
			char *source_where_clause,
			LIST *primary_attribute_name_list,
			LIST *related_attribute_name_list,
			char *folder_name,
			char *related_folder_name )
{
	char where_clause[ 65536 ];
	char *where_ptr = where_clause;
	char *primary_attribute_name;
	char *related_attribute_name;
	char last_folder_name[ 128 ];
	boolean prepend_and_clause = 0;

	piece_last( last_folder_name, ',', folder_name );

	if ( source_where_clause )
	{
		where_ptr += sprintf( where_ptr, "%s", source_where_clause );
	}
	else
	{
		*where_ptr = '\0';
		/* where_ptr += sprintf( where_ptr, "1 = 1" ); */
	}

	if ( !list_rewind( primary_attribute_name_list ) )
		return strdup( where_clause );

	if ( !list_rewind( related_attribute_name_list ) )
	{
		return strdup( where_clause );
	}

	if ( source_where_clause
	&&   *source_where_clause )
	{
		prepend_and_clause = 1;
	}

	do {
		primary_attribute_name =
			list_get_pointer(
				primary_attribute_name_list );

		related_attribute_name =
			list_get_pointer(
				related_attribute_name_list );

		if ( prepend_and_clause )
			where_ptr += sprintf( where_ptr, " and" );

		where_ptr += sprintf(
				where_ptr,
				" %s.%s = %s.%s",
				get_table_name(
					application_name,
					last_folder_name ),
				primary_attribute_name,
				get_table_name(
					application_name,
					related_folder_name ),
				related_attribute_name );

		prepend_and_clause = 1;

		if ( !list_next( related_attribute_name_list ) ) break;

	} while( list_next( primary_attribute_name_list ) );

	return strdup( where_clause );
}

char *query_login_name_where_clause(
			FOLDER *folder,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			char *login_name )
{
	char where_clause[ 65536 ];
	char *ptr = where_clause;
	char *attribute_name;
	char escaped_data[ 1024 ];
	char *table_name = {0};

	if ( folder && folder->folder_name && folder->application_name )
	{
		table_name =
			get_table_name(
				folder->application_name,
				folder->folder_name );
	}

	list_rewind( where_attribute_name_list );
	list_rewind( where_attribute_data_list );
	*ptr = '\0';

	do {
		attribute_name =
			list_get_pointer(
				where_attribute_name_list );

		if ( login_name
		&&   strcmp(
			attribute_name,
			QUERY_LOGIN_NAME_ATTRIBUTE_NAME ) == 0 )
		{
			timlib_strcpy(
				escaped_data,
				login_name,
				1024 );
		}
		else
		{
			timlib_strcpy(
				escaped_data,
				list_get_pointer( where_attribute_data_list ),
				1024 );

		}

		escape_single_quotes( escaped_data );

		if ( !list_at_head( where_attribute_name_list ) )
		{
			ptr += sprintf( ptr, " and " );
		}

		if ( table_name )
		{
			ptr += sprintf(
					ptr,
					 "%s.%s = '%s'",
					 table_name,
					 attribute_name,
					 escaped_data );
		}
		else
		{
			ptr += sprintf(
					ptr,
					 "%s = '%s'",
					 attribute_name,
					 escaped_data );
		}

		if ( !list_next( where_attribute_data_list ) ) break;

	} while( list_next( where_attribute_name_list ) );

	return strdup( where_clause );
}

char *query_folder_where_clause(
			FOLDER *folder,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			LIST *append_isa_attribute_list )
{
	char *folder_name = {0};

	if ( folder )
	{
		folder_name = folder->folder_name;
	}

	return query_simple_where(
			folder_name,
			where_attribute_name_list,
			where_attribute_data_list,
			append_isa_attribute_list );
}

char *query_simple_where(
			char *folder_name,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			LIST *append_isa_attribute_list )
{
	char where_clause[ 65536 ];
	char *ptr = where_clause;
	char *attribute_name;
	char escaped_data[ 1024 ];
	char *table_name = {0};
	ATTRIBUTE *attribute = {0};

	if ( folder_name )
	{
		table_name = folder_name;
	}

	list_rewind( where_attribute_name_list );
	list_rewind( where_attribute_data_list );
	*ptr = '\0';

	do {
		attribute_name =
			list_get_pointer(
				where_attribute_name_list );

		if ( list_length( append_isa_attribute_list ) )
		{
			attribute =
				attribute_seek(
					attribute_name,
					append_isa_attribute_list );
		}

		timlib_strcpy(
			escaped_data,
			list_get( where_attribute_data_list ),
			1024 );

		escape_single_quotes( escaped_data );

		if ( !list_at_head( where_attribute_name_list ) )
		{
			ptr += sprintf( ptr, " and " );
		}

		if ( table_name )
		{
			if ( attribute
			&&   ( strcmp(	attribute->datatype,
					"current_date_time" ) == 0
			||     strcmp(	attribute->datatype,
					"date_time" ) == 0 )
			&&     attribute->width == 16 )
			{
				ptr += sprintf( ptr,
						"substr(%s.%s,1,16) = '%s'",
						table_name,
						attribute_name,
						escaped_data );
			}
			else
			{
				ptr += sprintf( ptr,
				 		"%s.%s = '%s'",
				 		table_name,
				 		attribute_name,
				 		escaped_data );
			}
		}
		else
		{
			ptr += sprintf( ptr,
				 	"%s = '%s'",
				 	attribute_name,
				 	escaped_data );
		}

		if ( !list_next( where_attribute_data_list ) ) break;

	} while( list_next( where_attribute_name_list ) );

	return strdup( where_clause );
}

char *query_attribute_list_display(	char *folder_name,
					LIST *query_attribute_list )
{
	QUERY_ATTRIBUTE *query_attribute;
	char buffer[ 65536 ];
	char *ptr = buffer;

	*ptr = '\0';

	ptr += sprintf(	ptr,
			"%s(): folder_name = %s:",
			__FUNCTION__,
			folder_name );

	if ( !list_rewind( query_attribute_list ) )
	{
		ptr += sprintf( ptr,
				 "\nWarning: empty query_attribute_list." );
		return strdup( buffer );
	}

	do {
		query_attribute = list_get( query_attribute_list );

		ptr += sprintf(	ptr,
				"\nAttribute name = %s:",
				query_attribute->attribute_name );

		ptr += sprintf( ptr,
" datatype = %s, relational_operator = %s, from_data = %s, to_data = %s\n",
				query_attribute->datatype,
				query_relational_operator_display(
					query_attribute->relational_operator ),
				(query_attribute->from_data)
					? query_attribute->from_data
					: "null",
				(query_attribute->to_data)
					? query_attribute->to_data
					: "null" );

	} while( list_next( query_attribute_list ) );

	return strdup( buffer );
}

char *query_drop_down_list_display(	char *folder_name,
					LIST *query_drop_down_list )
{
	QUERY_DROP_DOWN *query_drop_down;
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	LIST *row_list;
	LIST *attribute_name_list;
	LIST *data_list;
	char buffer[ 65536 ];
	char *ptr = buffer;
	char *attribute_name;
	char *data;
	int row_number;

	*ptr = '\0';

	ptr += sprintf(	ptr,
			"%s(): folder_name = %s:",
			__FUNCTION__,
			folder_name );

	if ( !list_rewind( query_drop_down_list ) )
	{
		return strdup( buffer );
	}

	do {
		query_drop_down = list_get( query_drop_down_list );

		if ( !query_drop_down->folder_name )
		{
			fprintf(stderr,
				"ERROR %s/%s()/%d: empty folder_name\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		ptr += sprintf(	ptr,
				"\nquery_drop_down->folder_name = %s:",
				query_drop_down->folder_name );

		row_list = query_drop_down->query_drop_down_row_list;

		if ( !list_rewind( row_list ) )
		{
			ptr += sprintf(	ptr,
				" Warning: empty query_drop_down_row_list." );
			continue;
		}

		row_number = 1;

		do {
			query_drop_down_row = list_get( row_list );

			attribute_name_list =
				query_drop_down_row->
					attribute_name_list;

			data_list = query_drop_down_row->data_list;

			if ( !list_length( attribute_name_list ) )
			{
				ptr += sprintf(	ptr,
				" Warning: empty name list." );
				continue;
			}

			if ( list_length( attribute_name_list ) == 1
			&&   list_length( data_list ) )
			{
				ptr += sprintf(
					ptr,
					"%s",
					query_drop_down_list_in_clause_display(
						list_get_first_pointer(
							attribute_name_list ),
						data_list ) );
				continue;
			}

			if ( list_length( attribute_name_list ) !=
			     list_length( data_list ) )
			{
				ptr += sprintf(	ptr,
				" Warning: uneven name and data lists." );
				continue;
			}

			list_rewind( attribute_name_list );
			list_rewind( data_list );

			do {
				attribute_name =
					list_get( attribute_name_list );

				data = list_get( data_list );

				if ( list_at_head( data_list ) )
					ptr += sprintf( ptr,
							"row: %d",
							row_number );
				else
					ptr += sprintf( ptr, "," );

				ptr += sprintf( ptr,
						" %s = '%s'",
						attribute_name,
						(*data) ? data : "skipped" );

				list_next( attribute_name_list );

			} while( list_next( data_list ) );

			ptr += sprintf( ptr, ";" );
			row_number++;

		} while( list_next( row_list ) );

	} while( list_next( query_drop_down_list ) );

	return strdup( buffer );
}

char *query_drop_down_list_in_clause_display(
			char *attribute_name,
			LIST *data_list )
{
	char buffer[ 65536 ];
	char *ptr = buffer;

	if ( !list_rewind( data_list ) ) return "";

	ptr += sprintf(	ptr,
			"\nIn clause: %s in (%s)",
			attribute_name,
			list_display( data_list ) );

	return strdup( buffer );
}

char *query_relational_operator_display(
				enum relational_operator relational_operator )
{
	if ( relational_operator == equals )
		return EQUAL_OPERATOR;
	else
	if ( relational_operator == not_equal )
		return NOT_EQUAL_OPERATOR;
	else
	if ( relational_operator == not_equal_or_null )
		return NOT_EQUAL_OR_NULL_OPERATOR;
	else
	if ( relational_operator == less_than )
		return LESS_THAN_OPERATOR;
	else
	if ( relational_operator == less_than_equal_to )
		return LESS_THAN_EQUAL_TO_OPERATOR;
	else
	if ( relational_operator == greater_than )
		return GREATER_THAN_OPERATOR;
	else
	if ( relational_operator == greater_than_equal_to )
		return GREATER_THAN_EQUAL_TO_OPERATOR;
	else
	if ( relational_operator == between )
		return BETWEEN_OPERATOR;
	else
	if ( relational_operator == begins )
		return BEGINS_OPERATOR;
	else
	if ( relational_operator == contains )
		return CONTAINS_OPERATOR;
	else
	if ( relational_operator == not_contains )
		return NOT_CONTAINS_OPERATOR;
	else
	if ( relational_operator == query_or )
		return OR_OPERATOR;
	else
	if ( relational_operator == is_null )
		return NULL_OPERATOR;
	else
	if ( relational_operator == not_null )
		return NOT_NULL_OPERATOR;
	else
		return "Unknown";
}

char *query_get_process_where_clause(
			char **drop_down_where_clause,
			char **attribute_where_clause,
			LIST *query_drop_down_list,
			LIST *query_attribute_list,
			char *application_name,
			char *folder_name )
{
	if ( !list_length( query_drop_down_list )
	&&   !list_length( query_attribute_list ) )
	{
		return "1 = 1";
	}
	else
	if ( list_length( query_drop_down_list )
	&&   !list_length( query_attribute_list ) )
	{
		*drop_down_where_clause =
			query_get_process_drop_down_where_clause(
				query_drop_down_list,
				folder_name );

		return *drop_down_where_clause;
	}
	if ( !list_length( query_drop_down_list )
	&&   list_length( query_attribute_list ) )
	{
		boolean combine_date_time = 0;

		/* Fix this! */
		/* --------- */
		if ( !folder_name ) combine_date_time = 1;

		*attribute_where_clause =
			query_get_attribute_where_clause(
				query_attribute_list,
				application_name,
				combine_date_time );

		return *attribute_where_clause;
	}
	else
	{
		char where_clause[ 65536 ];
		boolean combine_date_time = 0;

		/* Fix this! */
		/* --------- */
		if ( !folder_name ) combine_date_time = 1;

		*drop_down_where_clause =
			query_get_process_drop_down_where_clause(
				query_drop_down_list,
				folder_name );

		*attribute_where_clause =
			query_get_attribute_where_clause(
				query_attribute_list,
				application_name,
				combine_date_time );

		sprintf( where_clause,
			 "%s and %s",
			 *drop_down_where_clause,
			 *attribute_where_clause );

		return strdup( where_clause );
	}
}

char *query_combined_where_clause(
			char **drop_down_where_clause,
			char **attribute_where_clause,
			LIST *query_drop_down_list,
			LIST *query_attribute_list,
			char *application_name,
			char *folder_name,
			boolean combine_date_time )
{
	if ( !list_length( query_drop_down_list )
	&&   !list_length( query_attribute_list ) )
	{
		return "1 = 1";
	}
	else
	if ( list_length( query_drop_down_list )
	&&   !list_length( query_attribute_list ) )
	{
		*drop_down_where_clause =
			query_folder_drop_down_where(
				query_drop_down_list,
				application_name,
				folder_name );

		return *drop_down_where_clause;
	}

	if ( !list_length( query_drop_down_list )
	&&   list_length( query_attribute_list ) )
	{
		*attribute_where_clause =
			query_get_attribute_where_clause(
				query_attribute_list,
				application_name,
				combine_date_time );

		return *attribute_where_clause;
	}
	else
	{
		char where_clause[ 65536 ];

		*drop_down_where_clause =
			query_get_drop_down_where_clause(
				query_drop_down_list,
				application_name,
				folder_name );

		*attribute_where_clause =
			query_get_attribute_where_clause(
				query_attribute_list,
				application_name,
				combine_date_time );

		sprintf( where_clause,
			 "%s and %s",
			 *drop_down_where_clause,
			 *attribute_where_clause );

		return strdup( where_clause );
	}
}

boolean query_get_date_time_between_attributes(
			QUERY_ATTRIBUTE **date_between_attribute,
			QUERY_ATTRIBUTE **time_between_attribute,
			LIST *query_attribute_list,
			boolean combine_date_time )
{
	QUERY_ATTRIBUTE *query_attribute;

	if ( !list_rewind( query_attribute_list ) ) return 0;

	do {
		query_attribute = list_get( query_attribute_list );

		if ( ( query_attribute->primary_key_index
		||     combine_date_time )
		&&   query_attribute->relational_operator == between
		&&  (timlib_strcmp( query_attribute->datatype, "time" ) == 0
		||   timlib_strcmp( query_attribute->
				datatype, "current_time" ) == 0 ) )
		{
			*time_between_attribute = query_attribute;
			continue;
		}

		if ( ( query_attribute->primary_key_index
		||     combine_date_time )
		&&   query_attribute->relational_operator == between
		&&  (timlib_strcmp( query_attribute->datatype, "date" ) == 0
		||   timlib_strcmp( query_attribute->
				datatype, "current_date" ) == 0 ) )
		{
			*date_between_attribute = query_attribute;
			continue;
		}

	} while( list_next( query_attribute_list ) );

	return ( *date_between_attribute && *time_between_attribute );

}

char *query_get_attribute_where_clause(
			LIST *query_attribute_list,
			char *application_name,
			boolean combine_date_time )
{
	char where_clause[ 65536 ];
	char *ptr = where_clause;
	QUERY_ATTRIBUTE *date_between_attribute = {0};
	QUERY_ATTRIBUTE *time_between_attribute = {0};
	QUERY_ATTRIBUTE *query_attribute;
	LIST *exclude_attribute_name_list = list_new();
	int str_len;
	boolean boolean_attribute;
	boolean first_time = 1;
	char *operator_character_string;

	*ptr = '\0';

	if ( !list_length( query_attribute_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty query_attribute_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( query_get_date_time_between_attributes(
			&date_between_attribute,
			&time_between_attribute,
			query_attribute_list,
			combine_date_time ) )
	{
		ptr += sprintf( ptr,
				"%s",
				query_get_between_date_time_where(
					date_between_attribute->attribute_name,
					time_between_attribute->attribute_name,
					date_between_attribute->from_data,
					time_between_attribute->from_data,
					date_between_attribute->to_data,
					time_between_attribute->to_data,
					application_name,
					date_between_attribute->folder_name ) );

		list_append_pointer(	exclude_attribute_name_list,
					date_between_attribute->
						attribute_name );

		list_append_pointer(	exclude_attribute_name_list,
					time_between_attribute->
						attribute_name );
		first_time = 0;
	}

	list_rewind( query_attribute_list );

	do {
		query_attribute = list_get( query_attribute_list );

		if ( list_exists_string(
			query_attribute->attribute_name,
			exclude_attribute_name_list ) )
		{
			continue;
		}

		str_len = strlen( query_attribute->attribute_name );

		boolean_attribute =
			( strncmp(	query_attribute->attribute_name +
					str_len - 3,
					"_yn",
					3 ) == 0 );

		if ( first_time )
			first_time = 0;
		else
			ptr += sprintf( ptr, " and" );

		operator_character_string = 
			get_operator_character(
				query_relational_operator_display(
					query_attribute->
						relational_operator ) );

		if ( ( timlib_strcmp(
				query_attribute->datatype,
				"float" ) == 0
		||     timlib_strcmp(
				query_attribute->datatype,
				"integer" ) == 0 )
		&&   query_attribute->relational_operator == between )
		{
			ptr += sprintf(
				ptr,
				" %s >= %s and %s <= %s",
				get_full_attribute_name(
					application_name,
					query_attribute->folder_name,
					query_attribute->attribute_name ),
				query_attribute->from_data,
				get_full_attribute_name(
					application_name,
					query_attribute->folder_name,
					query_attribute->attribute_name ),
				query_attribute->to_data );
		}
		else
		if ( query_attribute->relational_operator == between )
		{
			ptr += sprintf(
				ptr,
				" %s %s '%s' and '%s'",
				get_full_attribute_name(
					application_name,
					query_attribute->folder_name,
					query_attribute->attribute_name ),
				operator_character_string,
				query_attribute->from_data,
				query_attribute->to_data );
		}
		else
		if ( query_attribute->relational_operator == is_null )
		{
			ptr += sprintf(
			   	ptr,
				QUERY_NULL_EXPRESSION,
				get_full_attribute_name(
					application_name,
			   		query_attribute->folder_name,
			   		query_attribute->attribute_name ),
				get_full_attribute_name(
					application_name,
			   		query_attribute->folder_name,
			   		query_attribute->attribute_name ),
				get_full_attribute_name(
					application_name,
			   		query_attribute->folder_name,
					query_attribute->attribute_name ) );
		}
		else
		if ( query_attribute->relational_operator == not_null )
		{
			ptr += sprintf(
			   	ptr,
				QUERY_NOT_NULL_EXPRESSION,
				get_full_attribute_name(
					application_name,
			   		query_attribute->folder_name,
			   		query_attribute->attribute_name ),
				get_full_attribute_name(
					application_name,
			   		query_attribute->folder_name,
			   		query_attribute->attribute_name ),
				get_full_attribute_name(
					application_name,
			   		query_attribute->folder_name,
					query_attribute->attribute_name ) );
		}
		else
		if ( query_attribute->relational_operator == query_or )
		{
			char piece_buffer[ 512 ];
			int i;
			int local_first_time = 1;

			for(	i = 0;
				piece(	piece_buffer,
					',',
					query_attribute->from_data,
					i );
				i++ )
			{
				if ( local_first_time )
				{
					local_first_time = 0;
					ptr += sprintf( ptr, " (" );
				}
				else
				{
					ptr += sprintf( ptr, " or" );
				}
				ptr += sprintf(
			   		ptr,
			   		" %s = '%s'",
					get_full_attribute_name(
						application_name,
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ),
			   		strdup( piece_buffer ) );
			}
			ptr += sprintf( ptr, " )" );
		}
		else
		if ( query_attribute->relational_operator == begins )
		{
			char buffer[ 4096 ];

			ptr += sprintf( ptr,
					" %s like '%s%c'",
					get_full_attribute_name(
						application_name,
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ),
					escape_character( buffer,
							  query_attribute->
								from_data,
							  '_' ),
					'%' );
		}
		else
		if ( query_attribute->relational_operator == contains )
		{
			char buffer[ 4096 ];

			ptr += sprintf( ptr,
					" %s like '%c%s%c'",
					get_full_attribute_name(
						application_name,
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ),
					'%',
					escape_character( buffer,
							  query_attribute->
								from_data,
							  '_' ),
					'%' );
		}
		else
		if ( query_attribute->relational_operator == not_contains )
		{
			char buffer[ 4096 ];

			ptr += sprintf( ptr,
					" %s not like '%c%s%c'",
					get_full_attribute_name(
						application_name,
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ),
					'%',
					escape_character( buffer,
							  query_attribute->
								from_data,
							  '_' ),
					'%' );
		}
		else
		if ( query_attribute->relational_operator == not_equal_or_null )
		{
			ptr +=
				sprintf(
			   	ptr,
			   	" (%s <> '%s' or %s is null)",
				get_full_attribute_name(
					application_name,
					query_attribute->folder_name,
			   		query_attribute->attribute_name ),
				query_attribute->from_data,
				get_full_attribute_name(
					application_name,
					query_attribute->folder_name,
					query_attribute->attribute_name ) );
		}
		else
		if ( timlib_strcmp( query_attribute->datatype, "float" ) == 0
		||   timlib_strcmp(	query_attribute->datatype,
					"integer" ) == 0 )
		{
			ptr +=
				sprintf(
		   		ptr,
			   	" %s %s %s",
				get_full_attribute_name(
					application_name,
			   		query_attribute->folder_name,
			   		query_attribute->attribute_name ),
				operator_character_string,
				query_attribute->from_data );
		}
		else
		if ( boolean_attribute )
		{
			if ( timlib_strcmp(
					query_attribute->from_data,
					NOT_NULL_OPERATOR ) == 0 )
			{
				ptr +=
					sprintf(
			   		ptr,
			   		" %s is not null",
					get_full_attribute_name(
						application_name,
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ) );
			}
			else
			if ( *query_attribute->from_data == 'n' )
			{
				char *full_attribute_name;

				full_attribute_name =
					get_full_attribute_name(
						application_name,
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name );

				ptr +=
					sprintf(
			   		ptr,
			   		" (%s = 'n' or %s is null)",
					full_attribute_name,
					full_attribute_name );
			}
			else
			if ( *query_attribute->from_data == 'y' )
			{
				ptr +=
					sprintf(
			   		ptr,
			   		" %s = 'y'",
					get_full_attribute_name(
						application_name,
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ) );
			}
			else
			if ( timlib_strcmp(
					query_attribute->from_data,
					NULL_OPERATOR ) == 0 )
			{
				ptr +=
					sprintf(
			   		ptr,
			   		" %s is null",
					get_full_attribute_name(
						application_name,
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ) );
			}
		}
		else
		{
			ptr +=
				sprintf( ptr,
				" %s %s '%s'",
				get_full_attribute_name(
					application_name,
			   		query_attribute->folder_name,
			   		query_attribute->
						attribute_name ),
				operator_character_string,
				query_attribute->from_data );
		}

		list_append_pointer(	exclude_attribute_name_list,
					query_attribute->attribute_name );

	} while( list_next( query_attribute_list ) );

	return strdup( where_clause );
}

char *query_get_process_drop_down_where_clause(
					LIST *query_drop_down_list,
					char *folder_name )
{
	QUERY_DROP_DOWN *query_drop_down;
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	LIST *query_drop_down_row_list;
	char where_clause[ 65536 ];
	char *ptr = where_clause;
	char *attribute_name;
	char *data;
	boolean outter_first_time;
	boolean inner_first_time;
	boolean inner_inner_first_time;

	*ptr = '\0';

	if ( !list_rewind( query_drop_down_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty query_drop_down_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	outter_first_time = 1;

	do {
		query_drop_down = list_get( query_drop_down_list );

		if ( outter_first_time )
		{
			outter_first_time = 0;
		}
		else
		{
			ptr += sprintf( ptr, " and " );
		}

		query_drop_down_row_list =
			query_drop_down->
				query_drop_down_row_list;

		if ( !list_rewind( query_drop_down_row_list ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: empty query_drop_down_row_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		inner_first_time = 1;

		do {
			query_drop_down_row =
				list_get( query_drop_down_row_list );

			if ( inner_first_time )
			{
				inner_first_time = 0;
			}
			else
			{
				ptr += sprintf( ptr, " and " );
			}

			attribute_name =
				list_get_first_pointer(
					query_drop_down_row->
						attribute_name_list );

			if ( folder_name )
			{
				ptr += sprintf( ptr,
						" %s.%s in (",
						folder_name,
						attribute_name );
			}
			else
			{
				ptr += sprintf( ptr,
						" %s in (",
						attribute_name );
			}

			inner_inner_first_time = 1;

			list_rewind( query_drop_down_row->data_list );

			do {
				data =
					list_get(
						query_drop_down_row->
							data_list );

				if ( !*data )
				{
					fprintf( stderr,
		"ERROR in %s/%s()/%d: empty data for attribute_name = %s.\n",
						 __FILE__,
						 __FUNCTION__,
						 __LINE__,
						 attribute_name );
					exit( 1 );
				}

				if ( inner_inner_first_time )
				{
					inner_inner_first_time = 0;
				}
				else
				{
					ptr += sprintf( ptr, "," );
				}

				ptr += sprintf(
					ptr,
					"'%s'",
					data );

			} while( list_next( query_drop_down_row->data_list ) );

			ptr += sprintf( ptr, " )" );

		} while( list_next( query_drop_down_row_list ) );

	} while( list_next( query_drop_down_list ) );

	return strdup( where_clause );

}

char *query_folder_drop_down_where(
			LIST *query_drop_down_list,
			char *application_name,
			char *folder_name )
{
	QUERY_DROP_DOWN *query_drop_down;
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	LIST *query_drop_down_row_list;
	char where_clause[ 65536 ];
	char *ptr = where_clause;
	char *attribute_name;
	char *data;
	boolean outter_first_time;
	boolean inner_first_time;
	boolean inner_inner_first_time;

	*ptr = '\0';

	if ( !list_rewind( query_drop_down_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty query_drop_down_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	outter_first_time = 1;

	ptr += sprintf( ptr, " (" );

	do {
		query_drop_down = list_get( query_drop_down_list );

		if ( outter_first_time )
		{
			outter_first_time = 0;
		}
		else
		{
			ptr += sprintf( ptr, " and (" );
		}

		query_drop_down_row_list =
			query_drop_down->
				query_drop_down_row_list;

		if ( !list_rewind( query_drop_down_row_list ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: empty query_drop_down_row_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		inner_first_time = 1;

		ptr += sprintf( ptr, " (" );

		do {
			query_drop_down_row =
				list_get( query_drop_down_row_list );

			if ( inner_first_time )
			{
				inner_first_time = 0;
			}
			else
			{
				ptr += sprintf( ptr, " or (" );
			}

			list_rewind( query_drop_down_row->
					attribute_name_list );

			list_rewind( query_drop_down_row->
					data_list );

			inner_inner_first_time = 1;

			do {
				attribute_name =
					list_get(
						query_drop_down_row->
						attribute_name_list );

				data =
					list_get(
						query_drop_down_row->
							data_list );

				if ( !*data )
				{
					fprintf( stderr,
		"ERROR in %s/%s()/%d: empty data for attribute_name = %s.\n",
						 __FILE__,
						 __FUNCTION__,
						 __LINE__,
						 attribute_name );
					exit( 1 );
				}

				if ( inner_inner_first_time )
				{
					inner_inner_first_time = 0;
				}
				else
				{
					ptr += sprintf( ptr, " and" );
				}

				folder_name =
					query_drop_down->
						folder_name;

				ptr += sprintf(
					ptr,
					"%s",
					query_get_drop_down_data_where(
						application_name,
						folder_name,
						attribute_name,
						data ) );

				list_next( query_drop_down_row->
						attribute_name_list );

			} while( list_next( query_drop_down_row->data_list ) );

			if ( !inner_inner_first_time )
				ptr += sprintf( ptr, " )" );

		} while( list_next( query_drop_down_row_list ) );

		ptr += sprintf( ptr, " )" );

	} while( list_next( query_drop_down_list ) );

	return strdup( where_clause );
}

char *query_get_drop_down_where_clause(
			LIST *query_drop_down_list,
			char *application_name,
			char *folder_name )
{
	return query_drop_down_where(
			query_drop_down_list,
			application_name,
			folder_name );
}

char *query_drop_down_where(
			LIST *query_drop_down_list,
			char *application_name,
			char *folder_name )
{
	QUERY_DROP_DOWN *query_drop_down;
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	LIST *query_drop_down_row_list;
	char where_clause[ 65536 ];
	char *ptr;
	char *attribute_name;
	char *data;
	boolean outter_first_time;
	boolean inner_first_time;
	boolean inner_inner_first_time;

	ptr = where_clause;
	*ptr = '\0';

	if ( !list_rewind( query_drop_down_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty query_drop_down_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	outter_first_time = 1;

	ptr += sprintf( ptr, " (" );

	do {
		query_drop_down = list_get( query_drop_down_list );

		if ( outter_first_time )
		{
			outter_first_time = 0;
		}
		else
		{
			ptr += sprintf( ptr, " and (" );
		}

		query_drop_down_row_list =
			query_drop_down->
				query_drop_down_row_list;

		if ( !list_rewind( query_drop_down_row_list ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: empty query_drop_down_row_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		inner_first_time = 1;

		ptr += sprintf( ptr, " (" );

		do {
			query_drop_down_row =
				list_get( query_drop_down_row_list );

			if ( inner_first_time )
			{
				inner_first_time = 0;
			}
			else
			{
				ptr += sprintf( ptr, " or (" );
			}

			list_rewind( query_drop_down_row->
					attribute_name_list );

			list_rewind( query_drop_down_row->
					data_list );

			inner_inner_first_time = 1;

			do {
				attribute_name =
					list_get(
						query_drop_down_row->
						attribute_name_list );

				data =
					list_get(
						query_drop_down_row->
							data_list );

				if ( !*data )
				{
					fprintf( stderr,
		"ERROR in %s/%s()/%d: empty data for attribute_name = %s.\n",
						 __FILE__,
						 __FUNCTION__,
						 __LINE__,
						 attribute_name );
					exit( 1 );
				}

				if ( inner_inner_first_time )
				{
					inner_inner_first_time = 0;
				}
				else
				{
					ptr += sprintf( ptr, " and" );
				}

/*
				folder_name =
					query_drop_down->
						folder_name;
*/

				ptr += sprintf(
					ptr,
					"%s",
					query_get_drop_down_data_where(
						application_name,
						folder_name,
						attribute_name,
						data ) );

				list_next( query_drop_down_row->
						attribute_name_list );

			} while( list_next( query_drop_down_row->data_list ) );

			if ( !inner_inner_first_time )
				ptr += sprintf( ptr, " )" );

		} while( list_next( query_drop_down_row_list ) );

		ptr += sprintf( ptr, " )" );

	} while( list_next( query_drop_down_list ) );

	return strdup( where_clause );
}

char *query_get_subquery_where_clause(
			char *application_name,
			char *folder_name,
			LIST *query_subquery_list,
			LIST *primary_attribute_name_list )
{
	QUERY_SUBQUERY *query_subquery;
	char where_clause[ 65536 ];
	char *where_ptr = where_clause;

	*where_ptr = '\0';

	if ( !list_rewind( query_subquery_list ) ) return (char *)0;

	do {
		query_subquery = list_get( query_subquery_list );

		if ( !list_at_head( query_subquery_list ) )
		{
			where_ptr += sprintf( where_ptr, " and " );
		}

		where_ptr += sprintf(
				where_ptr,
				"exists ( select 1 from %s where %s",
				query_subquery->folder_name,
				query_append_where_clause_related_join(
					application_name,
					where_ptr /* source_where_clause */,
					primary_attribute_name_list,
					query_subquery->
						foreign_attribute_name_list,
					folder_name,
					query_subquery->folder_name
						/* related_folder_name */ ) );

		if ( list_length( query_subquery->query_drop_down_list ) )
		{
			where_ptr += sprintf(
					where_ptr,
					" and %s", 
					query_get_drop_down_where_clause(
						query_subquery->
							query_drop_down_list,
						application_name,
						(char *)0 /* folder_name */ ) );
		}

		if ( list_length( query_subquery->query_attribute_list ) )
		{
			boolean combine_date_time = 0;

			/* Fix this! */
			/* --------- */
			if ( !query_subquery->folder_name )
			{
				combine_date_time = 1;
			}

			where_ptr += sprintf(
					where_ptr,
					" and %s", 
					query_get_attribute_where_clause(
						query_subquery->
							query_attribute_list,
						application_name,
						combine_date_time ) );
		}

		where_ptr += sprintf( where_ptr, " )" );

	} while( list_next( query_subquery_list ) );

	return strdup( where_clause );

}

char *query_get_drop_down_data_where(	char *application_name,
					char *folder_name,
					char *attribute_name,
					char *data )
{
	static char where[ 1024 ];

	*where = '\0';

	if ( folder_name  )
	{
		if ( timlib_strcmp( data, NULL_OPERATOR ) == 0 )
		{
			sprintf(where,
				QUERY_NULL_EXPRESSION,
			 	get_full_attribute_name(
			 		application_name,
			 		folder_name,
			 		attribute_name ),
			 	get_full_attribute_name(
			 		application_name,
			 		folder_name,
			 		attribute_name ),
			 	get_full_attribute_name(
			 		application_name,
			 		folder_name,
			 		attribute_name ) );
		}
		else
		if ( timlib_strcmp( data, NOT_NULL_OPERATOR ) == 0 )
		{
			sprintf(where,
				QUERY_NOT_NULL_EXPRESSION,
			 	get_full_attribute_name(
			 		application_name,
			 		folder_name,
			 		attribute_name ),
			 	get_full_attribute_name(
			 		application_name,
			 		folder_name,
			 		attribute_name ),
			 	get_full_attribute_name(
			 		application_name,
			 		folder_name,
			 		attribute_name ) );
		}
		else
		{
			sprintf(where,
				" %s = '%s'",
			 	get_full_attribute_name(
			 		application_name,
			 		folder_name,
			 		attribute_name ),
			 	data );
		}
	}
	else
	{
		if ( timlib_strcmp( data, NULL_OPERATOR ) == 0 )
		{
			sprintf(where,
				QUERY_NULL_EXPRESSION,
			 	attribute_name,
			 	attribute_name,
			 	attribute_name );
		}
		else
		if ( timlib_strcmp( data, NOT_NULL_OPERATOR ) == 0 )
		{
			sprintf(where,
				QUERY_NOT_NULL_EXPRESSION,
			 	attribute_name,
			 	attribute_name,
			 	attribute_name );
		}
		else
		{
			sprintf(where,
					" %s = '%s'",
			 	attribute_name,
			 	data );
		}
	} /* if !folder_name */

	return where;
}

boolean query_data_list_accounted_for(
			LIST *query_drop_down_row_list,
			LIST *data_list )
{
	QUERY_DROP_DOWN_ROW *query_drop_down_row;

	if ( !list_rewind( query_drop_down_row_list ) ) return 0;

	do {
		query_drop_down_row = list_get( query_drop_down_row_list );

		if ( list_equals_string_list(
				query_drop_down_row->data_list,
				data_list ) )
		{
			return 1;
		}

	} while( list_next( query_drop_down_row_list ) );

	return 0;

}

char *query_get_order_clause(	DICTIONARY *sort_dictionary,
				char *folder_name,
				LIST *attribute_list )
{
	char *sort_multi_attribute_with_prefix_and_index;

	if ( ( sort_multi_attribute_with_prefix_and_index =
		query_get_multi_key_sort_attribute_with_prefix_and_index(
				sort_dictionary ) ) )
	{
		return query_with_sort_multi_attribute_get_order_clause(
				sort_multi_attribute_with_prefix_and_index,
				folder_name,
				attribute_list );
	}
	else
	if ( ( sort_multi_attribute_with_prefix_and_index =
		query_get_non_multi_key_sort_attribute_with_prefix_and_index(
				sort_dictionary ) ) )
	{
		return query_with_sort_multi_attribute_get_order_clause(
				sort_multi_attribute_with_prefix_and_index,
				folder_name,
				attribute_list );
	}
	else
	{
		return (char *)0;
	}

}

char *query_get_non_multi_key_sort_attribute_with_prefix_and_index(
				DICTIONARY *sort_dictionary )
{
	LIST *sort_attribute_name_list;
	char *sort_attribute_with_prefix_and_index;
	DICTIONARY *local_sort_dictionary;

	local_sort_dictionary = dictionary_copy( sort_dictionary );

	sort_attribute_name_list =
		dictionary_get_key_list(
			local_sort_dictionary );

	list_rewind( sort_attribute_name_list );

	do {
		sort_attribute_with_prefix_and_index =
			list_get_pointer(
				sort_attribute_name_list );

		if ( !character_exists(
				sort_attribute_with_prefix_and_index,
				'|' ) )
		{
			dictionary_free( local_sort_dictionary );
			return sort_attribute_with_prefix_and_index;
		}
	} while( list_next( sort_attribute_name_list ) );
	dictionary_free( local_sort_dictionary );
	return (char *)0;
}

char *query_get_multi_key_sort_attribute_with_prefix_and_index(
				DICTIONARY *sort_dictionary )
{
	LIST *sort_attribute_name_list;
	char *sort_attribute_with_prefix_and_index;
	DICTIONARY *local_sort_dictionary;

	local_sort_dictionary = dictionary_copy( sort_dictionary );

	sort_attribute_name_list =
		dictionary_get_key_list(
			local_sort_dictionary );

	if ( list_rewind( sort_attribute_name_list ) )
	{
		do {
			sort_attribute_with_prefix_and_index =
				list_get_pointer(
					sort_attribute_name_list );
	
			if ( character_exists(
					sort_attribute_with_prefix_and_index,
					MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ) )
			{
				return sort_attribute_with_prefix_and_index;
			}
	
		} while( list_next( sort_attribute_name_list ) );
	}

	dictionary_free( local_sort_dictionary );
	return (char *)0;

}

char *query_with_sort_multi_attribute_get_order_clause(
			char *sort_multi_attribute_with_prefix_and_index,
			char *folder_name,
			LIST *attribute_list )
{
	char sort_multi_attribute[ 512 ];
	boolean descending = 0;

	trim_index( 	sort_multi_attribute,
			sort_multi_attribute_with_prefix_and_index );

	if ( timlib_strncmp(
		sort_multi_attribute,
		FORM_DESCENDING_LABEL ) == 0 )
	{
		strcpy(	sort_multi_attribute,
			sort_multi_attribute +
			strlen(FORM_DESCENDING_LABEL) );

		descending = 1;
	}
	else
	/* ------------------------------------ */
	/* Allow attribute = sort_order,	*/
	/* but trim prefix = sort_$attribute 	*/
	/* ------------------------------------ */
	if ( timlib_strcmp(	sort_multi_attribute,
				"sort_order" ) != 0
	&&   timlib_strncmp(
			sort_multi_attribute,
			FORM_SORT_LABEL ) == 0 )
	{
		strcpy(	sort_multi_attribute,
			sort_multi_attribute +
			strlen( FORM_SORT_LABEL ) );
	}

	return query_parse_multi_attribute_get_order_clause(
			sort_multi_attribute,
			folder_name,
			descending,
			attribute_list );

}

char *query_parse_multi_attribute_get_order_clause(
			char *sort_multi_attribute,
			char *folder_name,
			boolean descending,
			LIST *attribute_list )
{
	char order_clause[ 1024 ] = {0};
	char *ptr = order_clause;
	char sort_attribute[ 128 ];
	char *descending_label;
	char first_folder_name[ 128 ];
	int i = 0;

	piece( first_folder_name, ',', folder_name, 0 );
	folder_name = first_folder_name;

	if ( descending )
		descending_label = " desc";
	else
		descending_label = "";
		
	while( piece(	sort_attribute,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			sort_multi_attribute,
			i ) )
	{
		if ( i > 0 ) ptr += sprintf( ptr, "," );

		if ( !attribute_seek_attribute(
				sort_attribute,
				attribute_list ) )
		{
			folder_name = (char *)0;
		}

		if ( folder_name && *folder_name )
		{
			ptr += sprintf(	ptr,
					"%s.%s%s",
					folder_name,
					sort_attribute,
					descending_label );
		}
		else
		{
			ptr += sprintf(	ptr,
					"%s%s",
					sort_attribute,
					descending_label );
		}
		i++;
	}

	return strdup( order_clause );
}

LIST *query_get_one2m_subquery_related_folder_list(
				char *application_name,
				LIST *one2m_subquery_folder_name_list,
				LIST *one2m_recursive_related_folder_list,
				RELATED_FOLDER *root_related_folder )
{
	RELATED_FOLDER *related_folder;
	LIST *one2m_subquery_related_folder_list;
	char *subquery_folder_name;

	if ( !list_rewind( one2m_subquery_folder_name_list ) )
		return (LIST *)0;

	one2m_subquery_related_folder_list = list_new();

	do {
		subquery_folder_name =
			list_get( one2m_subquery_folder_name_list );

		if ( ! ( related_folder =
				related_folder_one2m_seek(
					one2m_recursive_related_folder_list,
					subquery_folder_name ) ) )
		{
			if ( !root_related_folder ) continue;

			/* If subqueried a distant folder */
			/* ------------------------------ */
			related_folder = related_folder_calloc();

			related_folder->one2m_folder =
				folder_with_load_new(
					application_name,
					BOGUS_SESSION,
					subquery_folder_name,
					(ROLE *)0 /* role */ );

			/* ------------------------------ */
			/* root_related_folder is mto1.   */
			/* Only want to join root's keys. */
			/* ------------------------------ */
			related_folder->foreign_attribute_name_list =
				root_related_folder->
					folder->
					primary_attribute_name_list;
		}
		else
		{
			related_folder->
			one2m_folder =
				folder_with_load_new(
					application_name,
					BOGUS_SESSION,
					related_folder->
						one2m_folder->
						folder_name,
					(ROLE *)0 /* role */ );
		}

		list_append_pointer(
			one2m_subquery_related_folder_list,
			related_folder );

	} while( list_next( one2m_subquery_folder_name_list ) );

	return one2m_subquery_related_folder_list;

}

LIST *query_get_subquery_list(	DICTIONARY *dictionary,
				LIST *one2m_subquery_related_folder_list,
				int length_primary_attribute_name_list )
{
	LIST *query_subquery_list = {0};
	RELATED_FOLDER *related_folder;
	LIST *query_drop_down_list;
	LIST *query_attribute_list;
	QUERY_SUBQUERY *query_subquery;
	LIST *exclude_attribute_name_list = list_new();

	if ( !list_rewind( one2m_subquery_related_folder_list ) )
		return (LIST *)0;

	query_subquery_list = list_new();

	do {
		related_folder =
			list_get( 
				one2m_subquery_related_folder_list );

		query_subquery = (QUERY_SUBQUERY *)0;

		query_drop_down_list =
			query_get_subquery_drop_down_list(
				exclude_attribute_name_list,
				related_folder->one2m_folder
					/* root_folder */,
				related_folder->
					one2m_folder->
					mto1_related_folder_list,
				related_folder->
					one2m_folder->
					mto1_append_isa_related_folder_list,
				dictionary );

		if ( list_length( query_drop_down_list ) )
		{
			query_subquery =
			query_subquery_new(
				related_folder->
					one2m_folder->
					folder_name,
				list_copy_count(
					related_folder->
					foreign_attribute_name_list,
					length_primary_attribute_name_list ) );

			list_append_pointer(
				query_subquery_list,
				query_subquery );

			query_subquery->
				query_drop_down_list =
					query_drop_down_list;

		} /* if list_length( query_drop_down_list ) */

		query_attribute_list =
			query_get_attribute_list(
				related_folder->
					one2m_folder->
					append_isa_attribute_list,
				dictionary,
				exclude_attribute_name_list,
				related_folder->
					one2m_folder->
					folder_name
					/* dictionary_prepend_folder_name */ );

		if ( list_length( query_attribute_list ) )
		{
			if ( !query_subquery )
			{
				query_subquery =
				query_subquery_new(
					related_folder->
						one2m_folder->
						folder_name,
					list_copy_count(
					related_folder->
						foreign_attribute_name_list,
					length_primary_attribute_name_list ) );

				list_append_pointer(
					query_subquery_list,
					query_subquery );
			}

			query_subquery->
				query_attribute_list =
					query_attribute_list;
		}

	} while( list_next( one2m_subquery_related_folder_list ) );

	return query_subquery_list;
}

char *query_subquery_list_display( LIST *subquery_list )
{
	QUERY_SUBQUERY *subquery;
	char buffer[ 65536 ];
	char *ptr = buffer;

	*ptr = '\0';

	if ( !list_rewind( subquery_list ) )
	{
		ptr += sprintf( ptr, "\nWarning: empty subquery_list." );
		return strdup( buffer );
	}

	do {
		subquery = list_get( subquery_list );

		ptr += sprintf( ptr,
"folder_name = %s; query_drop_down_list = (%s); query_attribute_list = (%s), where_clause = (%s), foreign_attribute_name_list = (%s).\n",
				subquery->folder_name,
				query_drop_down_list_display(
					subquery->folder_name,
					subquery->query_drop_down_list ),
				query_attribute_list_display(
					subquery->folder_name,
					subquery->query_attribute_list ),
				subquery->where_clause,
				list_display( subquery->
						foreign_attribute_name_list ) );

	} while( list_next( subquery_list ) );

	*ptr = '\0';
	return strdup( buffer );
}

char *query_get_display( QUERY_OUTPUT *query_output )
{
	static char display_string[ 65536 ];
	char *ptr = display_string;
	QUERY_SUBQUERY *subquery;

	ptr += sprintf( ptr, "%c", '"' );

	ptr += sprintf( ptr,
			"%s",
			query_get_drop_down_list_display(
				query_output->query_drop_down_list) );

	ptr += sprintf( ptr,
			"%s",
			query_get_query_attribute_list_display(
				query_output->query_attribute_list) );

	if ( list_rewind( query_output->query_subquery_list ) )
	{
		do {
			subquery =
				list_get( query_output->query_subquery_list );

			ptr += sprintf( ptr,
				"%s",
				query_get_drop_down_list_display(
					subquery->query_drop_down_list ) );

			ptr += sprintf( ptr,
				"%s",
				query_get_query_attribute_list_display(
					subquery->query_attribute_list ) );

		} while( list_next( query_output->query_subquery_list ) );
	}

	ptr += sprintf( ptr, "%c", '"' );

	return display_string;
}

char *query_get_drop_down_list_display(
				LIST *query_drop_down_list )
{
	static char display_string[ 65536 ];
	char *ptr = display_string;
	QUERY_DROP_DOWN *query_drop_down;
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	LIST *row_list;
	LIST *attribute_name_list;
	LIST *data_list;
	char *attribute_name;
	char *data;

	if ( !list_rewind( query_drop_down_list ) ) return "";

	do {
		query_drop_down = list_get( query_drop_down_list );

		ptr += sprintf(	ptr,
				"%s: ",
				query_drop_down->folder_name );

		row_list = query_drop_down->query_drop_down_row_list;

		if ( list_rewind( row_list ) )
		{
			do {
				query_drop_down_row = list_get( row_list );

				attribute_name_list =
					query_drop_down_row->
						attribute_name_list;

				data_list = query_drop_down_row->data_list;

				if ( !list_rewind( attribute_name_list ) )
					continue;

				list_rewind( data_list );

				do {
					attribute_name =
						list_get( attribute_name_list );

					data = list_get( data_list );

					if ( !list_at_head( data_list ) )
					{
						ptr += sprintf( ptr, "," );
					}

					ptr +=
					sprintf(ptr,
						" %s = '%s'",
						attribute_name,
						(*data) ? data : "skipped" );

					list_next( attribute_name_list );

				} while( list_next( data_list ) );

			} while( list_next( row_list ) );

			ptr += sprintf( ptr, ";" );

		} /* if */

	} while( list_next( query_drop_down_list ) );

	return display_string;

}

char *query_get_query_attribute_list_display(
				LIST *query_attribute_list )
{
	static char display_string[ 65536 ];
	char *ptr = display_string;
	QUERY_ATTRIBUTE *query_attribute;

	if ( !list_rewind( query_attribute_list ) ) return "";

	do {
		query_attribute =
			list_get(
				query_attribute_list );

		ptr += sprintf( ptr,
				"%s %s %s %s;",
				query_attribute->attribute_name,
				query_relational_operator_display(
					query_attribute->relational_operator ),
				(query_attribute->from_data)
					? query_attribute->from_data
					: "null",
				(query_attribute->to_data)
					? query_attribute->to_data
					: "" );

	} while( list_next( query_attribute_list ) );

	return display_string;

}

LIST *query_append_prompt_recursive_folder_list(
				LIST *query_drop_down_list,
				LIST *exclude_attribute_name_list,
				char *root_folder_name,
				LIST *prompt_recursive_folder_list,
				DICTIONARY *query_dictionary )
{
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;
	QUERY_DROP_DOWN *drop_down;
	PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder;
	FOLDER *folder;

	if ( !list_rewind( prompt_recursive_folder_list ) )
	{
		return query_drop_down_list;
	}

	do {
		prompt_recursive_folder =
			list_get_pointer(
				prompt_recursive_folder_list );

		if ( !list_rewind( prompt_recursive_folder->
					prompt_recursive_mto1_folder_list ) )
		{
			continue;
		}

		do {
			prompt_recursive_mto1_folder =
				list_get(
					prompt_recursive_folder->
					  prompt_recursive_mto1_folder_list );

			folder = prompt_recursive_mto1_folder->folder;

			if ( list_is_subset_of(
				folder->primary_attribute_name_list,
				exclude_attribute_name_list ) )
			{
				continue;
			}

			if ( ( drop_down =
				query_drop_down(
					exclude_attribute_name_list,
					root_folder_name,
					folder->folder_name
					/* dictionary_prepend_folder_name */,
					folder->
					      primary_attribute_name_list
					      /* foreign_attribute_name_list */,
					folder->attribute_list,
					query_dictionary ) ) )
			{
				if ( !query_drop_down_list )
					query_drop_down_list = list_new();

				list_append_pointer(
					query_drop_down_list,
					drop_down );
			}

		} while( list_next( prompt_recursive_folder->
					prompt_recursive_mto1_folder_list ) );

	} while( list_next( prompt_recursive_folder_list ) );

	return query_drop_down_list;

}

char *query_get_dictionary_where_clause(
			DICTIONARY *dictionary,
			LIST *primary_attribute_name_list,
			char *dictionary_indexed_prefix )
{
	char where_clause[ 65536 ];
	char piece_buffer[ 512 ];
	char key[ 128 ];
	char *data;
	char *primary_attribute_name;
	int first_time = 1;
	char *ptr = where_clause;
	int index;
	int p;

	if ( !list_length( primary_attribute_name_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty primary_attribute_name_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	*ptr = '\0';

	for( index = 1; ; index++ )
	{
		sprintf( key, "%s_%d", dictionary_indexed_prefix, index );

		if ( ! ( data = dictionary_fetch( dictionary, key ) ) )
		{
			break;
		}

		if ( first_time )
			first_time = 0;
		else
			ptr += sprintf( ptr, " or " );

		ptr += sprintf( ptr, "(" );

		p = 0;
		list_rewind( primary_attribute_name_list );

		do {
			primary_attribute_name =
				list_get_pointer(
					primary_attribute_name_list );

			if ( !piece(	piece_buffer,
					MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
					data,
					p ) )
			{
				fprintf( stderr,
			"ERROR in %s/%s()/%d: cannot piece(%d) in (%s).\n",
			 		__FILE__,
			 		__FUNCTION__,
			 		__LINE__,
					p,
					data );
				exit( 1 );
			}

			if ( p ) ptr += sprintf( ptr, " and " );

			ptr += sprintf(
			   		ptr,
			   		"%s = '%s'",
					primary_attribute_name,
			   		timlib_escape_field( piece_buffer ) );

			p++;

		} while( list_next( primary_attribute_name_list ) );

		ptr += sprintf( ptr, ")" );
	}

	if ( first_time ) strcpy( where_clause, "1 = 1" );

	return strdup( where_clause );

}

QUERY_OR_SEQUENCE *query_or_sequence_new( LIST *attribute_name_list )
{
	QUERY_OR_SEQUENCE *query_or_sequence;

	query_or_sequence =
		(QUERY_OR_SEQUENCE *)
			calloc( 1, sizeof( QUERY_OR_SEQUENCE ) );
	query_or_sequence->attribute_name_list = attribute_name_list;
	query_or_sequence->data_list_list = list_new();
	return query_or_sequence;

}

int query_or_sequence_set_data_list_string(	LIST *data_list_list,
					char *data_list_string )
{
	LIST *temp_list;
	LIST *data_list;
	int length;
	char *data;
	int i;

	temp_list = list_string2list( data_list_string, ',' );
	length = list_length( temp_list );

	if ( !length ) return 0;

	if ( !list_length( data_list_list ) )
	{
		for ( i = 0; i < length; i++ )
		{
			data_list = list_new();
			list_append_pointer( data_list_list, data_list );
		}
	}

	list_rewind( temp_list );
	list_rewind( data_list_list );

	do {
		data = list_get_pointer( temp_list );
		data_list = list_get_pointer( data_list_list );
		list_append_pointer( data_list, data );
		list_next( data_list_list );
	} while( list_next( temp_list ) );
	list_free_container( temp_list );
	return length;
}

int query_or_sequence_set_data_list(	LIST *data_list_list,
					LIST *data_list )
{
	char *data_list_string;

	data_list_string = list_display_delimited( data_list, ',' );

	return query_or_sequence_set_data_list_string(
			data_list_list,
			data_list_string );

}

char *query_or_sequence_where_clause(	LIST *attribute_name_list,
					LIST *data_list_list,
					boolean with_and_prefix )
{
	return query_or_sequence_get_where_clause(
					attribute_name_list,
					data_list_list,
					with_and_prefix );
}

char *query_or_sequence_get_where_clause(
					LIST *attribute_name_list,
					LIST *data_list_list,
					boolean with_and_prefix )
{
	char *attribute_name;
	LIST *data_list;
	char *data;
	char where_clause[ 65536 ];
	char *where_ptr = where_clause;
	boolean first_time_and;
	boolean first_time_or;

	if ( !list_rewind( data_list_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty data_list_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	*where_ptr = '\0';

	if ( with_and_prefix )
	{
		where_ptr += sprintf( where_ptr, "and ( " );
	}

	first_time_or = 1;
	do {
		if ( first_time_or )
		{
			first_time_or = 0;
		}
		else
		{
			where_ptr += sprintf( where_ptr, " or " );
		}

		data_list = list_get_pointer( data_list_list );

		list_rewind( attribute_name_list );
		list_rewind( data_list );

		where_ptr += sprintf( where_ptr, "(" );
		first_time_and = 1;
		do {
			attribute_name =
				list_get_pointer( attribute_name_list );
			data = list_get_pointer( data_list );

			if ( first_time_and )
			{
				first_time_and = 0;
			}
			else
			{
				where_ptr +=
					sprintf( where_ptr,
						 " and " );
			}

			where_ptr +=
				sprintf( where_ptr,
					 "%s = '%s'",
					 attribute_name,
					 data );

			list_next( data_list );

		} while( list_next( attribute_name_list ) );

		where_ptr += sprintf( where_ptr, ")" );

	} while( list_next( data_list_list ) );

	if ( with_and_prefix )
	{
		where_ptr += sprintf( where_ptr, " )" );
	}

	return strdup( where_clause );

}

boolean query_attribute_list_exists(
				LIST *query_attribute_list,
				char *attribute_name )
{
	QUERY_ATTRIBUTE *query_attribute;

	if ( !list_rewind( query_attribute_list ) ) return 0;

	do {
		query_attribute = list_get_pointer( query_attribute_list );

		if ( strcmp(	query_attribute->attribute_name,
				attribute_name ) == 0 )
		{
			return 1;
		}
	} while( list_next( query_attribute_list ) );

	return 0;

}

char *query_key_list_where_clause(
			LIST *primary_key_list,
			char *input_buffer,
			char delimiter )
{
	char *primary_key;
	char piece_buffer[ 256 ];
	char where[ 2048 ];
	char *ptr = where;
	char *escaped_data;
	int i;

	if ( !list_rewind( primary_key_list ) ) return (char *)0;


	for(	i = 0;
		piece(	piece_buffer,
			delimiter,
			input_buffer,
			i );
		i++ )
	{
		primary_key = list_get_pointer( primary_key_list );

		if ( i ) ptr += sprintf( ptr, " and" );

		escaped_data =
			timlib_sql_injection_escape(
				piece_buffer );

		ptr += sprintf(
		   		ptr,
		   		" %s = '%s'",
				primary_key,
		   		escaped_data );

		free( escaped_data );

		if ( !list_next( primary_key_list ) ) break;
	}

	return strdup( where );

}

LIST *query_with_folder_name_get_mto1_join_folder_list(
			char *application_name,
			char *folder_name,
			ROLE *role )
{
	LIST *mto1_join_folder_list;
	char join_folder_name[ 128 ];
	int p;
	FOLDER *join_folder;

	if ( !character_exists( folder_name, ',' ) ) return (LIST *)0;

	mto1_join_folder_list = list_new();

	for(	p = 1;
		piece( join_folder_name, ',', folder_name, p );
		p++ )
	{
		if ( ! ( join_folder =
			folder_with_load_new(
				application_name,
				BOGUS_SESSION,
				strdup( join_folder_name ),
				role ) ) )
		{
			fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load folder = (%s).\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
				join_folder_name );
			exit( 1 );
		}

		list_append_pointer( mto1_join_folder_list, join_folder );
	}

	return mto1_join_folder_list;

}

QUERY_OUTPUT *query_folder_output_new(
			QUERY *query,
			FOLDER *folder,
			PROMPT_RECURSIVE *prompt_recursive,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			LIST *mto1_join_folder_list )
{
	QUERY_OUTPUT *query_output;

	/* -------------------------------------------- */
	/* This function may be called two times if	*/
	/* ROW_SECURITY_ROLE_UPDATE is populated.	*/
	/* -------------------------------------------- */
	static LIST *exclude_attribute_name_list = {0};

	if ( !exclude_attribute_name_list )
		exclude_attribute_name_list = list_new();

	if ( !folder )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query_output = query_output_calloc();

	if ( list_length( where_attribute_name_list )
	&&   list_length( where_attribute_data_list ) )
	{
		query_output->where_clause =
			query_login_name_where_clause(
				folder,
				where_attribute_name_list,
				where_attribute_data_list,
				(char *)0 /* login_name */ );

		if ( list_rewind( mto1_join_folder_list ) )
		{
			LIST *foreign_attribute_name_list;
			FOLDER *join_folder;
			QUERY *mto1_query;

			do {
				join_folder = list_get( mto1_join_folder_list );

				foreign_attribute_name_list =
			   	folder_get_primary_attribute_name_list(
					join_folder->attribute_list );

				query_output->where_clause =
				query_append_where_clause_related_join(
					folder->application_name,
					query_output->where_clause,
					list_duplicate(
						foreign_attribute_name_list ),
					foreign_attribute_name_list,
					folder->folder_name,
					join_folder->folder_name );

				mto1_query =
					query_folder_new(
						folder->application_name,
						query->login_name,
						join_folder->folder_name,
						query->dictionary,
						(ROLE *)0,
						(LIST *)0
					   	/* where_at..._name_list */,
						(LIST *)0
					   	/* where_at..._data_list */ );

				query_output->where_clause =
					query_append_where_clause(
						query_output->where_clause,
						mto1_query->
							query_output->
							where_clause);

			} while( list_next( mto1_join_folder_list ) );
		}

		query_output->order_clause =
		query_output->select_clause =
			query_get_select_clause(
				folder->application_name,
				folder->append_isa_attribute_list );

		query_output->from_clause =
			query_from_clause(
				folder->folder_name,
				folder->append_isa_attribute_list,
				(char *)0
					/* attribute_not_null_folder_name */ );

		return query_output;

	} /* if simple where clause */

	query_output->
		query_drop_down_list =
			query_get_drop_down_list(
				exclude_attribute_name_list,
				folder /* root_folder */,
				folder->
					mto1_related_folder_list,
				folder->
				    mto1_append_isa_related_folder_list,
				query->dictionary,
				0 /* not include_root_folder */ );

	if ( prompt_recursive
	&&   list_length(
		prompt_recursive->
			prompt_recursive_folder_list ) )
	{
		query_output->query_drop_down_list =
			query_append_prompt_recursive_folder_list(
				query_output->query_drop_down_list,
				exclude_attribute_name_list,
				folder->folder_name
					/* root_folder_name */,
				prompt_recursive->
					prompt_recursive_folder_list,
				query->dictionary );
	}

	if ( folder->row_level_non_owner_forbid )
	{
		query_set_row_level_non_owner_forbid_dictionary(
			query,
			folder );
	}

	query_output->query_attribute_list =
		query_get_attribute_list(
			folder->append_isa_attribute_list,
			query->dictionary,
			exclude_attribute_name_list,
			folder->folder_name
				/* dictionary_prepend_folder_name */ );

	if ( !query_output->where_clause )
	{
		query_output->non_joined_where_clause =
		query_output->where_clause =
		query_combined_where_clause(
			&query_output->drop_down_where_clause,
			&query_output->attribute_where_clause,
			query_output->query_drop_down_list,
			query_output->query_attribute_list,
			folder->application_name,
			folder->folder_name,
			0 /* not combine_date_time */ );
	}

	if ( !query_output->where_clause
	||   !*query_output->where_clause )
	{
		query_output->non_joined_where_clause =
		query_output->where_clause = "1 = 1";
	}

	query_output->select_clause =
		query_get_select_clause(
			folder->application_name,
			folder->append_isa_attribute_list );

	query_output->from_clause =
		query_from_clause(
			folder->folder_name,
			folder->append_isa_attribute_list,
			(char *)0 /* attribute_not_null_folder_name */ );

	if ( dictionary_length( query->sort_dictionary ) )
	{
		LIST *attribute_list = {0};

		if ( folder )
			attribute_list =
				folder->attribute_list;

		query_output->order_clause =
			query_get_order_clause(
				query->sort_dictionary,
				(folder) ? folder->folder_name
					 : (char *)0,
				attribute_list );
	}

	if ( !query_output->order_clause )
	{
		query_output->order_clause = query_output->select_clause;
	}

	if ( list_length( folder->mto1_isa_related_folder_list ) )
	{
		RELATED_FOLDER *isa_related_folder;
		LIST *foreign_attribute_name_list;

		list_rewind( folder->mto1_isa_related_folder_list );
		do {
			isa_related_folder =
				list_get_pointer(
					folder->
					mto1_isa_related_folder_list );

			foreign_attribute_name_list =
				   folder_get_primary_attribute_name_list(
					isa_related_folder->folder->
						attribute_list );

			query_output->where_clause =
				query_append_where_clause_related_join(
					folder->application_name,
					query_output->where_clause
						/* source_where_clause */,
					folder_get_primary_attribute_name_list(
						folder->attribute_list ),
					foreign_attribute_name_list,
					folder->folder_name,
					isa_related_folder->
						folder->
						folder_name );

		} while( list_next(
				folder->
				mto1_isa_related_folder_list ) );
	}

	if ( list_rewind( mto1_join_folder_list ) )
	{
		LIST *foreign_attribute_name_list;
		FOLDER *join_folder;
		QUERY *mto1_query;

		do {
			join_folder = list_get( mto1_join_folder_list );

			foreign_attribute_name_list =
			   folder_get_primary_attribute_name_list(
				join_folder->attribute_list );

			query_output->where_clause =
			query_append_where_clause_related_join(
				folder->application_name,
				query_output->where_clause,
				list_duplicate( foreign_attribute_name_list ),
				foreign_attribute_name_list,
				folder->folder_name,
				join_folder->folder_name );

			mto1_query =
				query_folder_new(
					folder->application_name,
					query->login_name,
					join_folder->folder_name,
					query->dictionary,
					(ROLE *)0,
					(LIST *)0
					   /* where_attribute_name_list */,
					(LIST *)0
					   /* where_attribute_data_list */ );

			query_output->where_clause =
				query_append_where_clause(
					query_output->where_clause,
					mto1_query->query_output->where_clause);

		} while( list_next( mto1_join_folder_list ) );
	}

	/* ------------------------------------------------------------ */
	/* If row_level_non_owner_forbid where a m:1 isa ENTITY.	*/
	/* See EMPLOYEE_WORK_DAY --> EMPLOYEE isa ENTITY.		*/
	/* ------------------------------------------------------------ */
	if ( folder->row_level_non_owner_forbid
	&&   list_length( folder->mto1_related_folder_list ) )
	{
		query_output_set_row_level_non_owner_forbid_join(
			query_output,
			folder );
	}

	return query_output;
}

char *query_get_folder_drop_down_where_clause(
			LIST *query_drop_down_list,
			char *application_name,
			char *folder_name )
{
	QUERY_DROP_DOWN *query_drop_down;
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	LIST *query_drop_down_row_list;
	char where_clause[ 65536 ];
	char *ptr = where_clause;
	char *attribute_name;
	char *data;
	boolean outter_first_time;
	boolean inner_first_time;
	boolean inner_inner_first_time;

	*ptr = '\0';

	if ( !list_rewind( query_drop_down_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty query_drop_down_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	outter_first_time = 1;

	ptr += sprintf( ptr, " (" );

	do {
		query_drop_down = list_get( query_drop_down_list );

		if ( outter_first_time )
		{
			outter_first_time = 0;
		}
		else
		{
			ptr += sprintf( ptr, " and (" );
		}

		query_drop_down_row_list =
			query_drop_down->
				query_drop_down_row_list;

		if ( !list_rewind( query_drop_down_row_list ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: empty query_drop_down_row_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		inner_first_time = 1;

		ptr += sprintf( ptr, " (" );

		do {
			query_drop_down_row =
				list_get( query_drop_down_row_list );

			if ( inner_first_time )
			{
				inner_first_time = 0;
			}
			else
			{
				ptr += sprintf( ptr, " or (" );
			}

			list_rewind( query_drop_down_row->
					attribute_name_list );

			list_rewind( query_drop_down_row->
					data_list );

			inner_inner_first_time = 1;

			do {
				attribute_name =
					list_get(
						query_drop_down_row->
						attribute_name_list );

				data =
					list_get(
						query_drop_down_row->
							data_list );

				if ( !*data )
				{
					fprintf( stderr,
		"ERROR in %s/%s()/%d: empty data for attribute_name = %s.\n",
						 __FILE__,
						 __FUNCTION__,
						 __LINE__,
						 attribute_name );
					exit( 1 );
				}

				if ( inner_inner_first_time )
				{
					inner_inner_first_time = 0;
				}
				else
				{
					ptr += sprintf( ptr, " and" );
				}

				folder_name =
					query_drop_down->
						folder_name;

				ptr += sprintf(
					ptr,
					"%s",
					query_get_drop_down_data_where(
						application_name,
						folder_name,
						attribute_name,
						data ) );

				list_next( query_drop_down_row->
						attribute_name_list );

			} while( list_next( query_drop_down_row->data_list ) );

			if ( !inner_inner_first_time )
				ptr += sprintf( ptr, " )" );

		} while( list_next( query_drop_down_row_list ) );

		ptr += sprintf( ptr, " )" );

	} while( list_next( query_drop_down_list ) );

	return strdup( where_clause );

}

QUERY *query_folder_new(	char *application_name,
				char *login_name,
				char *folder_name,
				DICTIONARY *dictionary,
				ROLE *role,
				LIST *where_attribute_name_list,
				LIST *where_attribute_data_list )
{
	QUERY *query;
	char first_folder_name[ 128 ];

	query = query_calloc();

	piece( first_folder_name, ',', folder_name, 0 );

	if ( ! ( query->folder =
			folder_with_load_new(
				application_name,
				BOGUS_SESSION,
				strdup( first_folder_name ),
				role ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( character_exists( folder_name, ',' ) )
	{
		query->mto1_join_folder_list =
		       query_with_folder_name_get_mto1_join_folder_list(
				application_name,
				folder_name,
				role );
	}

	query->login_name = login_name;
	query->dictionary = dictionary;
	query->max_rows = QUERY_MAX_ROWS;

	query->prompt_recursive =
		prompt_recursive_new(
			application_name,
			query->folder->folder_name
				/* query_folder_name */,
			query->
				folder->
				mto1_related_folder_list );

	query->query_output =
		query_folder_output_new(
			query,
			query->folder,
			query->prompt_recursive,
			where_attribute_name_list,
			where_attribute_data_list,
			query->mto1_join_folder_list );

	return query;
}

QUERY_DROP_DOWN *query_get_drop_down(
			LIST *exclude_attribute_name_list,
			char *root_folder_name,
			char *dictionary_prepend_folder_name,
			LIST *foreign_attribute_name_list,
			LIST *attribute_list,
			DICTIONARY *dictionary )
{
	QUERY_DROP_DOWN *query_drop_down = {0};
	int highest_index;
	int index;

	highest_index =
		dictionary_attribute_name_list_get_highest_index(
			dictionary,
			foreign_attribute_name_list,
			dictionary_prepend_folder_name );

	if ( highest_index == -1 )
		return (QUERY_DROP_DOWN *)0;

	for( index = 0; index <= highest_index; index++ )
	{
		query_drop_down =
			query_get_row_drop_down(
				exclude_attribute_name_list,
				query_drop_down,
				root_folder_name,
				foreign_attribute_name_list,
				attribute_list,
				dictionary,
				index,
				dictionary_prepend_folder_name );
	}

	return query_drop_down;
}

QUERY_DROP_DOWN *query_get_row_drop_down(
				LIST *exclude_attribute_name_list,
				QUERY_DROP_DOWN *query_drop_down,
				char *root_folder_name,
				LIST *foreign_attribute_name_list,
				LIST *attribute_list,
				DICTIONARY *dictionary,
				int index,
				char *dictionary_prepend_folder_name )
{
	LIST *data_list = {0};
	QUERY_DROP_DOWN_ROW *drop_down_row;
	char key[ 1024 ];
	char *string_delimited;
	char *data_list_string;
	char *operator_name;
	enum relational_operator relational_operator;

	if ( !list_length( foreign_attribute_name_list ) )
		return query_drop_down;

	string_delimited =
		list2string_delimited(
			foreign_attribute_name_list,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );

	sprintf( key, "%s_%d", string_delimited, index );

	if ( ! ( data_list_string = dictionary_fetch( dictionary, key ) ) )
	{
		if ( dictionary_prepend_folder_name
		&&   *dictionary_prepend_folder_name )
		{
			sprintf(key,
				"%s.%s_%d",
				dictionary_prepend_folder_name,
				string_delimited,
				index );
		}

		if ( ! ( data_list_string =
				dictionary_fetch( dictionary, key ) ) )
		{
			return query_drop_down;
		}
	}

	if ( strcmp( data_list_string, NULL_OPERATOR ) == 0 )
	{
		int length = list_length( foreign_attribute_name_list );
		int i;

		data_list = list_new();

		for(	i = 0;
			i < length;
			i++ )
		{
			list_append_pointer( data_list, NULL_OPERATOR );
		}
	}
	else
	if ( strcmp( data_list_string, NOT_NULL_OPERATOR ) == 0 )
	{
		int length = list_length( foreign_attribute_name_list );
		int i;

		data_list = list_new();

		for(	i = 0;
			i < length;
			i++ )
		{
			list_append_pointer( data_list, NOT_NULL_OPERATOR );
		}
	}
	else
	{
		data_list =
			list_string2list(
				data_list_string,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );
	}

	if ( !list_length( data_list ) ) return query_drop_down;

	operator_name =
		query_get_operator_name(
			key /* attribute_name */,
			dictionary,
			0 /* dictionary_offset */,
			QUERY_RELATION_OPERATOR_STARTING_LABEL,
			(char *)0
			/* dictionary_prepend_folder_name */ );

	relational_operator = query_get_relational_operator( operator_name );

	/* If multi-drop-down, then no relational_operator is okay. */
	/* -------------------------------------------------------- */
	if ( index && relational_operator == relational_operator_empty )
	{
		goto query_get_drop_down_continue;
	}

	if ( relational_operator != equals )
	{
		return query_drop_down;
	}

query_get_drop_down_continue:

	if ( ! ( drop_down_row =
			query_drop_down_row_new(
				foreign_attribute_name_list,
				attribute_list,
				data_list ) ) )
	{
		return query_drop_down;
	}

	if ( !query_drop_down )
	{
		query_drop_down = query_drop_down_new( root_folder_name );
	}
	else
	if ( query_data_list_accounted_for(
		query_drop_down->query_drop_down_row_list,
		data_list ) )
	{
		return query_drop_down;
	}

	if ( exclude_attribute_name_list )
	{
		list_append_list(	exclude_attribute_name_list,
					foreign_attribute_name_list );
	}

	list_append_pointer(	query_drop_down->query_drop_down_row_list,
				drop_down_row );

	return query_drop_down;
}

LIST *query_get_drop_down_list(	LIST *exclude_attribute_name_list,
				FOLDER *root_folder,
				LIST *mto1_related_folder_list,
				LIST *mto1_append_isa_related_folder_list,
				DICTIONARY *dictionary,
				boolean include_root_folder )
{
	LIST *drop_down_list = {0};
	RELATED_FOLDER *related_folder;
	RELATED_FOLDER *sub_related_folder;
	QUERY_DROP_DOWN *query_drop_down;
	LIST *foreign_attribute_name_list;

	if ( include_root_folder )
	{
		if ( ( query_drop_down =
			query_get_drop_down(
				exclude_attribute_name_list,
				root_folder->folder_name,
				root_folder->folder_name
					/* dictionary_prepend_folder_name */,
				root_folder->primary_attribute_name_list
					/* foreign_attribute_name_list */,
				root_folder->attribute_list,
				dictionary ) ) )
		{
			drop_down_list = list_new();
			list_append_pointer( drop_down_list, query_drop_down );
		}
	}

	if ( !list_rewind( mto1_related_folder_list ) )
		goto check_mto1_append_isa_related_folder_list;

	do {
		related_folder =
			list_get(
				mto1_related_folder_list );


		if ( related_folder->folder_foreign_attribute_name_list )
		{
			foreign_attribute_name_list =
				related_folder->
					folder_foreign_attribute_name_list;
		}
		else
		{
			foreign_attribute_name_list =
				related_folder->foreign_attribute_name_list;
		}

		if ( ( query_drop_down =
			query_get_drop_down(
				exclude_attribute_name_list,
				root_folder->
					folder_name,
				related_folder->
					folder->
					folder_name
				   /* dictionary_prepend_folder_name */,
				foreign_attribute_name_list,
				related_folder->folder->attribute_list,
				dictionary ) ) )
		{
			if ( !drop_down_list )
				drop_down_list = list_new();

			list_append_pointer(
				drop_down_list,
				query_drop_down );
		}

	} while( list_next( mto1_related_folder_list ) );

check_mto1_append_isa_related_folder_list:

	if ( !list_rewind( mto1_append_isa_related_folder_list ) )
		return drop_down_list;

	do {
		related_folder =
			list_get(
				mto1_append_isa_related_folder_list );

		if ( !related_folder->relation_type_isa ) continue;

		if ( !list_rewind( related_folder->
					folder->
					mto1_related_folder_list ) )
		{
			continue;
		}

		do {
			sub_related_folder =
				list_get(
				related_folder->
					folder->
					mto1_related_folder_list );

			if ( ( query_drop_down =
				query_get_drop_down(
					exclude_attribute_name_list,
					related_folder->
						folder->
						folder_name
							/* root_folder_name */,
					related_folder->
						folder->
						folder_name
					   /* dictionary_prepend_folder_name */,
					sub_related_folder->
					    foreign_attribute_name_list,
					sub_related_folder->
						folder->
						attribute_list,
					dictionary ) ) )
			{
				if ( !drop_down_list )
				{
					drop_down_list = list_new();
				}

				list_append_pointer(
					drop_down_list,
					query_drop_down );
			}

		} while( list_next( 
				related_folder->
					folder->
					mto1_related_folder_list ) );

	} while( list_next( mto1_append_isa_related_folder_list ) );

	return drop_down_list;
}


/* ------------------------------------------------------------ */
/* If row_level_non_owner_forbid where a m:1 isa ENTITY.	*/
/* See EMPLOYEE_WORK_DAY --> EMPLOYEE isa ENTITY.		*/
/* ------------------------------------------------------------ */
void query_output_set_row_level_non_owner_forbid_join(
			QUERY_OUTPUT *query_output,
			FOLDER *folder )
{
	if ( list_rewind( folder->mto1_related_folder_list ) )
	{
		RELATED_FOLDER *related_folder;
		RELATED_FOLDER *isa_related_folder;
		LIST *foreign_attribute_name_list;

		do {
			related_folder =
				list_get_pointer(
					folder->
					mto1_related_folder_list );

			if ( !related_folder->
				folder->
				row_level_non_owner_forbid )
			{
				continue;
			}


			/* This is the old way of setting. */
			/* ------------------------------- */ 
			related_folder->
				folder->
				mto1_isa_related_folder_list =
				   related_folder_get_isa_related_folder_list(
					folder->application_name,
					BOGUS_SESSION,
					related_folder->folder->folder_name,
					(char *)0 /* role_name */,
					0 /* role_override_row_restrictions */,
					related_folder_recursive_all );

			if ( !list_length(
				related_folder->
					folder->
					mto1_isa_related_folder_list ) )
			{
				continue;
			}

			list_rewind( related_folder->
					folder->
					mto1_isa_related_folder_list );

			do {
				isa_related_folder =
					list_get_pointer(
					  related_folder->
					  folder->
					  mto1_isa_related_folder_list );

				foreign_attribute_name_list =
				   folder_get_primary_attribute_name_list(
					isa_related_folder->folder->
						attribute_list );

				query_output->where_clause =
				   query_append_where_clause_related_join(
					folder->application_name,
					query_output->where_clause,
					folder_get_primary_attribute_name_list(
						folder->attribute_list ),
					foreign_attribute_name_list,
					folder->folder_name,
					isa_related_folder->
						folder->
						folder_name );

			} while( list_next(
					related_folder->
					folder->
					mto1_isa_related_folder_list ) );

		} while( list_next(
					folder->
					mto1_related_folder_list ) );
	}

}

void query_set_row_level_non_owner_forbid_dictionary(
				QUERY *query,
				FOLDER *folder )
{
	char key[ 128 ];

	if ( !query->login_name || !*query->login_name )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: has row_level_non_owner_forbid but empty login_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !query->dictionary )
	{
		query->dictionary =
			dictionary_small_new();
	}

	sprintf(key,
		"%s%s_0",
		QUERY_FROM_STARTING_LABEL,
		QUERY_LOGIN_NAME_ATTRIBUTE_NAME );

	dictionary_set_pointer(	query->dictionary,
				strdup( key ),
				query->login_name );

	sprintf(key,
	 	"%s%s_0",
	 	QUERY_RELATION_OPERATOR_STARTING_LABEL,
	 	QUERY_LOGIN_NAME_ATTRIBUTE_NAME );

	dictionary_set_pointer(	query->dictionary,
				strdup( key ),
				EQUAL_OPERATOR );

	if ( !attribute_list_exists(
		folder->append_isa_attribute_list,
		QUERY_LOGIN_NAME_ATTRIBUTE_NAME ) )
	{
		ATTRIBUTE *attribute;

		attribute = attribute_new( QUERY_LOGIN_NAME_ATTRIBUTE_NAME );

		list_append_pointer(
			folder->append_isa_attribute_list,
			attribute );
	}
}

QUERY_OUTPUT *query_process_drop_down_output_new(
				FOLDER *folder,
				char *folder_name,
				DICTIONARY *query_dictionary )
{
	QUERY_OUTPUT *query_output;

	query_output = query_output_calloc();

	if ( !folder )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query_output->
		query_drop_down_list =
			query_process_drop_down_get_drop_down_list(
				folder->mto1_related_folder_list,
				query_dictionary );

	query_output->where_clause =
		query_combined_where_clause(
			&query_output->drop_down_where_clause,
			&query_output->attribute_where_clause,
			query_output->query_drop_down_list,
			query_output->query_attribute_list,
			folder->application_name,
			folder_name,
			1 /* combine_date_time */ );

	return query_output;
}

QUERY *query_folder_data_new(	char *application_name,
				char *login_name,
				char *folder_name,
				DICTIONARY *dictionary,
				ROLE *role,
				int max_rows,
				boolean include_root_folder )
{
	QUERY *query;

	query = query_calloc();

	if ( !folder_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty folder_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( query->folder =
			folder_with_load_new(
				application_name,
				BOGUS_SESSION,
				folder_name,
				role ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query->login_name = login_name;
	query->dictionary = dictionary;
	query->max_rows = max_rows;

	query->query_output =
		query_primary_data_output_new(
			query,
			query->folder,
			include_root_folder );

	return query;
}

QUERY *query_related_folder_data_new(
			char *application_name,
			char *login_name,
			char *folder_name,
			DICTIONARY *dictionary,
			ROLE *role,
			int max_rows )
{
	QUERY *query;

	query = query_calloc();

	if ( !folder_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty folder_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( query->folder =
			folder_with_load_new(
				application_name,
				BOGUS_SESSION,
				folder_name,
				role ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query->login_name = login_name;
	query->dictionary = dictionary;
	query->max_rows = max_rows;

	query->query_output =
		query_related_data_output_new(
			query,
			query->folder );

	return query;
}

QUERY_DROP_DOWN *query_related_data_drop_down(
				LIST *exclude_attribute_name_list,
				char *root_folder_name,
				char *dictionary_prepend_folder_name,
				LIST *foreign_attribute_name_list,
				LIST *attribute_list,
				DICTIONARY *dictionary )
{
	QUERY_DROP_DOWN *drop_down = {0};
	int highest_index;
	int index;

	highest_index =
		dictionary_attribute_name_list_get_highest_index(
			dictionary,
			foreign_attribute_name_list,
			dictionary_prepend_folder_name );

	if ( highest_index == -1 ) return (QUERY_DROP_DOWN *)0;

	for( index = 0; index <= highest_index; index++ )
	{
		drop_down =
			query_row_drop_down(
				exclude_attribute_name_list,
				drop_down,
				root_folder_name,
				foreign_attribute_name_list,
				attribute_list,
				dictionary,
				index,
				dictionary_prepend_folder_name );
	}

	return drop_down;
}

QUERY_OUTPUT *query_related_data_output_new(
			QUERY *query,
			FOLDER *folder )
{
	QUERY_OUTPUT *query_output;
	LIST *exclude_attribute_name_list = list_new();

	query_output = query_output_calloc();

	if ( !folder )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query_output->from_clause =
		query_from_clause(
			folder->folder_name,
			(LIST *)0 /* isa_attribute_list */,
			(char *)0 /* attribute_not_null_folder_name */ );

	query_output->
		query_drop_down_list =
			query_related_data_drop_down_list(
				exclude_attribute_name_list,
				folder /* root_folder */,
				folder->
					mto1_related_folder_list,
				folder->
				    mto1_append_isa_related_folder_list,
				query->dictionary );

	query_output->where_clause =
		query_combined_where_clause(
			&query_output->drop_down_where_clause,
			&query_output->attribute_where_clause,
			query_output->query_drop_down_list,
			query_output->query_attribute_list,
			folder->application_name,
			folder->folder_name,
			0 /* not combine_date_time */ );

	if ( folder->row_level_non_owner_forbid
	&&   list_length( folder->mto1_isa_related_folder_list ) )
	{
		RELATED_FOLDER *isa_related_folder;
		LIST *foreign_attribute_name_list;

		list_rewind( folder->mto1_isa_related_folder_list );

		do {
			isa_related_folder =
				list_get_pointer(
					folder->
					mto1_isa_related_folder_list );

			foreign_attribute_name_list =
				   folder_get_primary_attribute_name_list(
					isa_related_folder->folder->
						attribute_list );

			query_output->where_clause =
				query_append_where_clause_related_join(
					folder->application_name,
					query_output->where_clause,
					folder_get_primary_attribute_name_list(
						folder->attribute_list ),
					foreign_attribute_name_list,
					folder->folder_name,
					isa_related_folder->
						folder->
						folder_name );

		} while( list_next(
				folder->
				mto1_isa_related_folder_list ) );
	}

	return query_output;
}

LIST *query_related_data_drop_down_list(
			LIST *exclude_attribute_name_list,
			FOLDER *root_folder,
			LIST *mto1_related_folder_list,
			LIST *mto1_append_isa_related_folder_list,
			DICTIONARY *dictionary )
{
	LIST *drop_down_list = {0};
	RELATED_FOLDER *related_folder;
	RELATED_FOLDER *sub_related_folder;
	QUERY_DROP_DOWN *drop_down;

	if ( !list_rewind( mto1_related_folder_list ) )
		goto check_mto1_append_isa_related_folder_list;

	do {
		related_folder =
			list_get(
				mto1_related_folder_list );

		if ( ( drop_down =
			query_related_data_drop_down(
				exclude_attribute_name_list,
				root_folder->
					folder_name,
				related_folder->
					folder->
					folder_name
				   /* dictionary_prepend_folder_name */,
				related_folder->
					foreign_attribute_name_list,
				related_folder->folder->attribute_list,
				dictionary ) ) )
		{
			if ( !drop_down_list )
				drop_down_list = list_new();

			list_set(
				drop_down_list,
				drop_down );
		}

	} while( list_next( mto1_related_folder_list ) );

check_mto1_append_isa_related_folder_list:

	if ( !list_rewind( mto1_append_isa_related_folder_list ) )
		return drop_down_list;

	do {
		related_folder =
			list_get(
				mto1_append_isa_related_folder_list );

		if ( !related_folder->relation_type_isa ) continue;

		if ( !list_rewind( related_folder->
					folder->
					mto1_related_folder_list ) )
		{
			continue;
		}

		do {
			sub_related_folder =
				list_get(
				related_folder->
					folder->
					mto1_related_folder_list );

			if ( ( drop_down =
				query_related_data_drop_down(
					exclude_attribute_name_list,
					related_folder->
						folder->
						folder_name
							/* root_folder_name */,
					related_folder->
						folder->
						folder_name
					   /* dictionary_prepend_folder_name */,
					sub_related_folder->
					    foreign_attribute_name_list,
					sub_related_folder->
						folder->
						attribute_list,
					dictionary ) ) )
			{
				if ( !drop_down_list )
				{
					drop_down_list = list_new();
				}

				list_set(
					drop_down_list,
					drop_down );
			}

		} while( list_next( 
				related_folder->
					folder->
					mto1_related_folder_list ) );

	} while( list_next( mto1_append_isa_related_folder_list ) );

	return drop_down_list;
}

LIST *query_edit_table_dictionary_list(
			char *application_name,
			char *select_clause,
			char *from_clause,
			char *where_clause,
			char *order_clause,
			int max_rows,
			LIST *append_isa_attribute_list,
			char *login_name )
{
	LIST *row_dictionary_list = {0};
	char *sys_string;
	LIST *attribute_name_list;

	sys_string = 
		query_get_sys_string(
			application_name,
			select_clause,
			from_clause,
			where_clause,
			(char *)0 /* subquery_where_clause */,
			order_clause,
			max_rows );

	attribute_name_list =
	 	attribute_lookup_allowed_attribute_name_list(
			append_isa_attribute_list );

	row_dictionary_list =
		list_usage_pipe2dictionary_list(
			sys_string, 
			attribute_name_list,
	 		attribute_date_attribute_name_list(
				append_isa_attribute_list ),
			FOLDER_DATA_DELIMITER,
			application_name,
			login_name );

	return row_dictionary_list;
}

QUERY *query_edit_table_new(
			DICTIONARY *dictionary,
			char *application_name,
			char *login_name,
			char *query_select_folder_name,
			ROLE *role,
			LIST *append_isa_attribute_list,
			char *attribute_not_null_join,
			char *attribute_not_null_folder_name )
{
	QUERY *query;
	char first_folder_name[ 128 ];

	dictionary_appaserver_parse_multi_attribute_keys(
		dictionary,
		QUERY_RELATION_OPERATOR_STARTING_LABEL );

	query = query_calloc();

	piece( first_folder_name, ',', query_select_folder_name, 0 );

	if ( ! ( query->folder =
			folder_with_load_new(
				application_name,
				BOGUS_SESSION,
				strdup( first_folder_name ),
				role ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query->login_name = login_name;
	query->dictionary = dictionary;
	query->max_rows = QUERY_MAX_ROWS;

	query->prompt_recursive =
		prompt_recursive_new(
			application_name,
			query->folder->folder_name
				/* query_folder_name */,
			query->
				folder->
				mto1_related_folder_list );

	query->query_output =
		query_edit_table_output_new(
			query,
			first_folder_name,
			query->folder,
			append_isa_attribute_list,
			query->prompt_recursive,
			query_select_folder_name,
			attribute_not_null_join,
			attribute_not_null_folder_name );

	return query;
}

QUERY_OUTPUT *query_edit_table_output_new(
			QUERY *query,
			char *first_folder_name,
			FOLDER *folder,
			LIST *append_isa_attribute_list,
			PROMPT_RECURSIVE *prompt_recursive,
			char *query_select_folder_name,
			char *attribute_not_null_join,
			char *attribute_not_null_folder_name )
{
	QUERY_OUTPUT *query_output;

	/* -------------------------------------------- */
	/* This function may be called two times if	*/
	/* ROW_SECURITY_ROLE_UPDATE is populated.	*/
	/* -------------------------------------------- */
	static LIST *exclude_attribute_name_list = {0};

	if ( !query )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty query.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty folder.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !exclude_attribute_name_list )
		exclude_attribute_name_list = list_new();

	query_output = query_output_calloc();

	query_output->
		query_drop_down_list =
			query_edit_table_drop_down_list(
				exclude_attribute_name_list,
				first_folder_name,
				folder->
					mto1_related_folder_list,
				folder->
				    mto1_append_isa_related_folder_list,
				query->dictionary );


	if ( prompt_recursive
	&&   list_length(
		prompt_recursive->
			prompt_recursive_folder_list ) )
	{
		query_output->query_drop_down_list =
			query_append_prompt_recursive_folder_list(
				query_output->query_drop_down_list,
				exclude_attribute_name_list,
				folder->folder_name
					/* root_folder_name */,
				prompt_recursive->
					prompt_recursive_folder_list,
				query->dictionary );
	}

	if ( folder->row_level_non_owner_forbid )
	{
		query_set_row_level_non_owner_forbid_dictionary(
			query,
			folder );
	}

	query_output->query_attribute_list =
		query_get_attribute_list(
			append_isa_attribute_list,
			query->dictionary,
			exclude_attribute_name_list,
			folder->folder_name
				/* dictionary_prepend_folder_name */ );

	query_output->non_joined_where_clause =
	query_output->where_clause =
	query_edit_table_where(
		&query_output->drop_down_where_clause,
		&query_output->attribute_where_clause,
		query_output->query_drop_down_list,
		query_output->query_attribute_list,
		folder->application_name,
		first_folder_name,
		0 /* not combine_date_time */ );

	if ( !query_output->where_clause
	||   !*query_output->where_clause )
	{
		query_output->non_joined_where_clause =
		query_output->where_clause = "1 = 1";
	}

	query_output->select_clause =
		query_get_select_clause(
			folder->application_name,
			append_isa_attribute_list );

	query_output->from_clause =
		query_from_clause(
			query_select_folder_name,
			append_isa_attribute_list,
			attribute_not_null_folder_name );

	if ( dictionary_length( query->sort_dictionary ) )
	{
		query_output->order_clause =
			query_get_order_clause(
				query->sort_dictionary,
				(folder) ? folder->folder_name
					 : (char *)0,
				append_isa_attribute_list );
	}

	if ( !query_output->order_clause )
	{
		query_output->order_clause = query_output->select_clause;
	}

	if ( list_length( folder->mto1_isa_related_folder_list ) )
	{
		RELATED_FOLDER *isa_related_folder;
		LIST *foreign_attribute_name_list;

		list_rewind( folder->mto1_isa_related_folder_list );
		do {
			isa_related_folder =
				list_get_pointer(
					folder->
					mto1_isa_related_folder_list );

			foreign_attribute_name_list =
				   folder_get_primary_attribute_name_list(
					isa_related_folder->folder->
						attribute_list );

			query_output->where_clause =
				query_append_where_clause_related_join(
					folder->application_name,
					query_output->where_clause
						/* source_where_clause */,
					folder_get_primary_attribute_name_list(
						folder->attribute_list ),
					foreign_attribute_name_list,
					folder->folder_name,
					isa_related_folder->
						folder->
						folder_name );

		} while( list_next(
				folder->
				mto1_isa_related_folder_list ) );
	}

	/* ------------------------------------------------------------ */
	/* If row_level_non_owner_forbid where a m:1 isa ENTITY.	*/
	/* See EMPLOYEE_WORK_DAY --> EMPLOYEE isa ENTITY.		*/
	/* ------------------------------------------------------------ */
	if ( folder->row_level_non_owner_forbid
	&&   list_length( folder->mto1_related_folder_list ) )
	{
		query_output_set_row_level_non_owner_forbid_join(
			query_output,
			folder );
	}

	if ( attribute_not_null_join && *attribute_not_null_join )
	{
		char where[ 65536 ];

		sprintf(where,
			"%s and %s",
			query_output->where_clause,
			attribute_not_null_join );

		query_output->where_clause = strdup( where );
	}

	return query_output;
}

LIST *query_edit_table_drop_down_list(
			LIST *exclude_attribute_name_list,
			char *first_folder_name,
			LIST *mto1_related_folder_list,
			LIST *mto1_append_isa_related_folder_list,
			DICTIONARY *dictionary )
{
	LIST *drop_down_list = list_new();
	RELATED_FOLDER *related_folder;
	RELATED_FOLDER *sub_related_folder;
	LIST *foreign_attribute_name_list;

	if ( !list_rewind( mto1_related_folder_list ) )
		goto check_mto1_append_isa_related_folder_list;

	do {
		related_folder =
			list_get(
				mto1_related_folder_list );

		if ( related_folder->folder_foreign_attribute_name_list )
		{
			foreign_attribute_name_list =
				related_folder->
					folder_foreign_attribute_name_list;
		}
		else
		{
			foreign_attribute_name_list =
				related_folder->
					foreign_attribute_name_list;
		}

#ifdef NOT_DEFINED
		if ( ( drop_down =
			query_edit_table_drop_down(
				exclude_attribute_name_list,
				related_folder->
					folder->
					folder_name
					/* root_folder_name */,
				related_folder->
					folder->
					folder_name
				   	/* dictionary_prepend_folder_name */,
				foreign_attribute_name_list,
				related_folder->folder->attribute_list,
				dictionary ) ) )
#endif
	list_set_list(
		drop_down_list,
		query_edit_table_drop_down_list_evaluate(
			exclude_attribute_name_list,
			first_folder_name
				/* root_folder_name */,
			related_folder->
				folder->
				folder_name
			   	/* dictionary_prepend_folder_name */,
			foreign_attribute_name_list,
			related_folder->folder->attribute_list,
			dictionary ) );

	} while( list_next( mto1_related_folder_list ) );

check_mto1_append_isa_related_folder_list:

	if ( !list_rewind( mto1_append_isa_related_folder_list ) )
		return drop_down_list;

	do {
		related_folder =
			list_get(
				mto1_append_isa_related_folder_list );

		if ( !related_folder->relation_type_isa ) continue;

		if ( !list_rewind(
			related_folder->
				folder->
				mto1_related_folder_list ) )
		{
			continue;
		}

		do {
			sub_related_folder =
				list_get(
				related_folder->
					folder->
					mto1_related_folder_list );

			list_set_list(
				drop_down_list,
				query_edit_table_drop_down_list_evaluate(
					exclude_attribute_name_list,
					related_folder->
						folder->
						folder_name
							/* root_folder_name */,
					related_folder->
						folder->
						folder_name
					   /* dictionary_prepend_folder_name */,
					sub_related_folder->
					    foreign_attribute_name_list,
					sub_related_folder->
						folder->
						attribute_list,
					dictionary ) );

		} while( list_next( 
				related_folder->
					folder->
					mto1_related_folder_list ) );

	} while( list_next( mto1_append_isa_related_folder_list ) );

	return drop_down_list;
}

LIST *query_edit_table_drop_down_list_evaluate(
			LIST *exclude_attribute_name_list,
			char *root_folder_name,
			char *dictionary_prepend_folder_name,
			LIST *foreign_attribute_name_list,
			LIST *attribute_list,
			DICTIONARY *dictionary )
{
	LIST *drop_down_list = {0};
	QUERY_DROP_DOWN *drop_down = {0};
	int highest_index;
	int index;

	highest_index =
		dictionary_attribute_name_list_get_highest_index(
			dictionary,
			foreign_attribute_name_list,
			dictionary_prepend_folder_name );

	if ( highest_index == -1 ) return (LIST *)0;

	for( index = 0; index <= highest_index; index++ )
	{
		drop_down =
			query_edit_table_row_drop_down(
				exclude_attribute_name_list,
				drop_down,
				root_folder_name,
				foreign_attribute_name_list,
				attribute_list,
				dictionary,
				index,
				dictionary_prepend_folder_name );

		if ( drop_down )
		{
			if ( !drop_down_list ) drop_down_list = list_new();
		}

		list_set( drop_down_list, drop_down );
	}

	return drop_down_list;
}

QUERY_DROP_DOWN *query_edit_table_row_drop_down(
			LIST *exclude_attribute_name_list,
			QUERY_DROP_DOWN *query_drop_down,
			char *root_folder_name,
			LIST *foreign_attribute_name_list,
			LIST *attribute_list,
			DICTIONARY *dictionary,
			int index,
			char *dictionary_prepend_folder_name )
{
	LIST *data_list = {0};
	QUERY_DROP_DOWN_ROW *drop_down_row;
	char key[ 1024 ];
	char *string_delimited;
	char *data_list_string;
	char *operator_name;
	enum relational_operator relational_operator;

	if ( !list_length( foreign_attribute_name_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: empty foreign_attribute_name_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	string_delimited =
		list2string_delimited(
			foreign_attribute_name_list,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );

	sprintf( key, "%s_%d", string_delimited, index );

	if ( ! ( data_list_string = dictionary_fetch( dictionary, key ) ) )
	{
		if ( dictionary_prepend_folder_name
		&&   *dictionary_prepend_folder_name )
		{
			sprintf(key,
				"%s.%s_%d",
				dictionary_prepend_folder_name,
				string_delimited,
				index );
		}

		if ( ! ( data_list_string =
				dictionary_fetch( dictionary, key ) ) )
		{
			/* Return disappointed */
			/* ------------------- */
			return query_drop_down;
		}
	}

	if ( strcmp( data_list_string, NULL_OPERATOR ) == 0 )
	{
		int length = list_length( foreign_attribute_name_list );
		int i;

		data_list = list_new();

		for(	i = 0;
			i < length;
			i++ )
		{
			list_set( data_list, NULL_OPERATOR );
		}
	}
	else
	if ( strcmp( data_list_string, NOT_NULL_OPERATOR ) == 0 )
	{
		int length = list_length( foreign_attribute_name_list );
		int i;

		data_list = list_new();

		for(	i = 0;
			i < length;
			i++ )
		{
			list_set( data_list, NOT_NULL_OPERATOR );
		}
	}
	else
	{
		data_list =
			list_string2list(
				data_list_string,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );
	}

	if ( !list_length( data_list ) )
	{
		/* Return disappointed */
		/* ------------------- */
		return query_drop_down;
	}

	operator_name =
		query_get_operator_name(
			key /* attribute_name */,
			dictionary,
			0 /* dictionary_offset */,
			QUERY_RELATION_OPERATOR_STARTING_LABEL,
			(char *)0
			/* dictionary_prepend_folder_name */ );

	relational_operator = query_get_relational_operator( operator_name );

	/* If multi-drop-down, then no relational_operator is okay. */
	/* -------------------------------------------------------- */
	if ( index && relational_operator == relational_operator_empty )
	{
		goto query_get_drop_down_continue;
	}

	if ( relational_operator != equals )
	{
		return query_drop_down;
	}

query_get_drop_down_continue:

	if ( ! ( drop_down_row =
			query_drop_down_edit_table_new(
				foreign_attribute_name_list,
				attribute_list,
				data_list ) ) )
	{
		return query_drop_down;
	}

	if ( !query_drop_down )
	{
		query_drop_down =
			query_drop_down_new(
				strdup( root_folder_name ) );
	}
	else
	if ( query_data_list_accounted_for(
		query_drop_down->query_drop_down_row_list,
		data_list ) )
	{
		return query_drop_down;
	}

	if ( exclude_attribute_name_list )
	{
		list_set_list(	exclude_attribute_name_list,
				foreign_attribute_name_list );
	}

	list_set(
		query_drop_down->
			query_drop_down_row_list,
		drop_down_row );

	return query_drop_down;
}

QUERY_DROP_DOWN_ROW *query_drop_down_edit_table_new(
			LIST *attribute_name_list,
			LIST *attribute_list,
			LIST *data_list )
{
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	LIST *local_attribute_name_list = {0};
	LIST *local_data_list = {0};
	char *attribute_name;
	char *data;
	ATTRIBUTE *attribute;

	if ( !list_length( attribute_name_list )
	|| ( list_length( attribute_name_list ) != list_length( data_list ) ) )
	{
		return (QUERY_DROP_DOWN_ROW *)0;
	}

	list_rewind( attribute_name_list );
	list_rewind( data_list );

	do {
		attribute_name = list_get( attribute_name_list );
		data = list_get( data_list );

		if ( *data )
		{
			if ( ( attribute =
				attribute_seek_attribute( 
					attribute_name,
					attribute_list ) ) )
			{
				char *datatype;

				datatype = attribute->datatype;

				if ( strcmp( datatype,
						"current_date" ) == 0
				||   strcmp( datatype,
						"current_date_time" ) == 0
				||   strcmp( datatype,
						"current_time" ) == 0
				||   strcmp( datatype,
						"date" ) == 0
				||   strcmp( datatype,
						"date_time" ) == 0 )
				{
					query_convert_date_international(
						&data );
				}
			}

			if ( !local_attribute_name_list )
			{
				local_attribute_name_list = list_new();
				local_data_list = list_new();
			}

			list_set(
				local_attribute_name_list,
				attribute_name );

			list_set(
				local_data_list,
				timlib_sql_injection_escape( data ) );
		}
		else
		{
			return (QUERY_DROP_DOWN_ROW *)0;
		}

		list_next( data_list );

	} while( list_next( attribute_name_list ) );

	if ( ! ( query_drop_down_row =
			calloc( 1, sizeof( QUERY_DROP_DOWN_ROW ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: memory allocation error.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return (QUERY_DROP_DOWN_ROW *)0;
	}

	query_drop_down_row->attribute_name_list = local_attribute_name_list;
	query_drop_down_row->data_list = local_data_list;

	return query_drop_down_row;
}

char *query_edit_table_drop_down_where(
			LIST *query_drop_down_list,
			char *application_name,
			char *folder_name )
{
	QUERY_DROP_DOWN *query_drop_down;
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	LIST *query_drop_down_row_list;
	char where_clause[ 65536 ];
	char *ptr = where_clause;
	char *attribute_name;
	char *data;
	boolean outter_first_time;
	boolean inner_first_time;
	boolean inner_inner_first_time;

	*ptr = '\0';

	if ( !list_rewind( query_drop_down_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty query_drop_down_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	outter_first_time = 1;

	ptr += sprintf( ptr, " (" );

	do {
		query_drop_down = list_get( query_drop_down_list );

		if ( outter_first_time )
		{
			outter_first_time = 0;
		}
		else
		{
			ptr += sprintf( ptr, " and (" );
		}

		query_drop_down_row_list =
			query_drop_down->
				query_drop_down_row_list;

		if ( !list_rewind( query_drop_down_row_list ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: empty query_drop_down_row_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		inner_first_time = 1;

		ptr += sprintf( ptr, " (" );

		do {
			query_drop_down_row =
				list_get( query_drop_down_row_list );

			if ( inner_first_time )
			{
				inner_first_time = 0;
			}
			else
			{
				ptr += sprintf( ptr, " or (" );
			}

			list_rewind( query_drop_down_row->
					attribute_name_list );

			list_rewind( query_drop_down_row->
					data_list );

			inner_inner_first_time = 1;

			do {
				attribute_name =
					list_get(
						query_drop_down_row->
						attribute_name_list );

				data =
					list_get(
						query_drop_down_row->
							data_list );

				if ( !*data )
				{
					fprintf( stderr,
		"ERROR in %s/%s()/%d: empty data for attribute_name = %s.\n",
						 __FILE__,
						 __FUNCTION__,
						 __LINE__,
						 attribute_name );
					exit( 1 );
				}

				if ( inner_inner_first_time )
				{
					inner_inner_first_time = 0;
				}
				else
				{
					ptr += sprintf( ptr, " and" );
				}

				ptr += sprintf(
					ptr,
					"%s",
					query_get_drop_down_data_where(
						application_name,
						folder_name,
						attribute_name,
						data ) );

				list_next( query_drop_down_row->
						attribute_name_list );

			} while( list_next( query_drop_down_row->data_list ) );

			if ( !inner_inner_first_time )
				ptr += sprintf( ptr, " )" );

		} while( list_next( query_drop_down_row_list ) );

		ptr += sprintf( ptr, " )" );

	} while( list_next( query_drop_down_list ) );

	return strdup( where_clause );
}

char *query_edit_table_where(
			char **drop_down_where_clause,
			char **attribute_where_clause,
			LIST *query_drop_down_list,
			LIST *query_attribute_list,
			char *application_name,
			char *folder_name,
			boolean combine_date_time )
{
	if ( !list_length( query_drop_down_list )
	&&   !list_length( query_attribute_list ) )
	{
		return "1 = 1";
	}
	else
	if ( list_length( query_drop_down_list )
	&&   !list_length( query_attribute_list ) )
	{
		*drop_down_where_clause =
			query_edit_table_drop_down_where(
				query_drop_down_list,
				application_name,
				folder_name );

		return *drop_down_where_clause;
	}

	if ( !list_length( query_drop_down_list )
	&&   list_length( query_attribute_list ) )
	{
		*attribute_where_clause =
			query_get_attribute_where_clause(
				query_attribute_list,
				application_name,
				combine_date_time );

		return *attribute_where_clause;
	}
	else
	{
		char where_clause[ 65536 ];

		*drop_down_where_clause =
			query_edit_table_drop_down_where(
				query_drop_down_list,
				application_name,
				folder_name );

		*attribute_where_clause =
			query_get_attribute_where_clause(
				query_attribute_list,
				application_name,
				combine_date_time );

		sprintf( where_clause,
			 "%s and %s",
			 *drop_down_where_clause,
			 *attribute_where_clause );

		return strdup( where_clause );
	}
}

QUERY_OUTPUT *query_detail_output_new(
			FOLDER *folder,
			LIST *mto1_isa_related_folder_list,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			LIST *append_isa_attribute_list,
			char *attribute_not_null_join,
			char *attribute_not_null_folder_name )
{
	QUERY_OUTPUT *query_output;
	int len_name_list;
	int len_data_list;

	len_name_list = list_length( where_attribute_name_list );
	len_data_list = list_length( where_attribute_data_list );

	if ( ( !len_name_list )
	||   (  len_name_list != len_data_list ) )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: lengths not the same: [%s] vs [%s].\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_display( where_attribute_name_list ),
			list_display( where_attribute_data_list ) );
		return (QUERY_OUTPUT*)0;
	}

	query_output = query_output_calloc();

	query_output->from_clause =
		query_from_clause(
			folder->folder_name,
			append_isa_attribute_list,
			attribute_not_null_folder_name );

	query_output->where_clause =
		query_simple_where(
			folder->folder_name,
			where_attribute_name_list,
			where_attribute_data_list,
			append_isa_attribute_list );

	if ( attribute_not_null_join && *attribute_not_null_join )
	{
		char where[ 65536 ];

		sprintf(where,
			"%s and %s",
			query_output->where_clause,
			attribute_not_null_join );

		free( query_output->where_clause );
		query_output->where_clause = strdup( where );
	}

	if ( list_length( mto1_isa_related_folder_list ) )
	{
		query_output->where_clause =
			query_output_mto1_isa_where(
				folder->application_name,
				folder->folder_name,
				mto1_isa_related_folder_list,
				folder->primary_attribute_name_list,
				query_output->where_clause,
				1 /* one_only */ );
	}

	query_output->order_clause =
	query_output->select_clause =
		query_get_select_clause(
			folder->application_name,
			append_isa_attribute_list );

	return query_output;
}

QUERY *query_detail_new(
			char *application_name,
			char *folder_name,
			ROLE *role,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			LIST *append_isa_attribute_list,
			char *attribute_not_null_join,
			char *attribute_not_null_folder_name )
{
	QUERY *query;

	query = query_calloc();

	if ( ! ( query->folder =
			folder_with_load_new(
				application_name,
				BOGUS_SESSION,
				folder_name,
				role ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query->max_rows = QUERY_MAX_ROWS;

	query->query_output =
		query_detail_output_new(
			query->folder,
			query->folder->mto1_isa_related_folder_list,
			where_attribute_name_list,
			where_attribute_data_list,
			append_isa_attribute_list,
			attribute_not_null_join,
			attribute_not_null_folder_name );

	return query;
}

char *query_output_one2m_isa_where(
			char **from_clause,
			char *application_name,
			char *folder_name,
			LIST *one2m_isa_related_folder_list,
			LIST *primary_attribute_name_list,
			char *where_clause,
			boolean one_only )
{
	LIST *foreign_attribute_name_list;
	RELATED_FOLDER *isa_related_folder;
	char from_buffer[ 1024 ];

	if ( !list_rewind( one2m_isa_related_folder_list ) )
		return where_clause;

	if ( !from_clause ) return where_clause;

	strcpy( from_buffer, *from_clause );

	do {
		isa_related_folder =
			list_get(
				one2m_isa_related_folder_list );

		foreign_attribute_name_list =
			    related_folder_foreign_attribute_name_list(
				primary_attribute_name_list,
				isa_related_folder->related_attribute_name,
				isa_related_folder->
					folder_foreign_attribute_name_list );

		where_clause =
			query_append_where_clause_related_join(
				application_name,
				where_clause,
				primary_attribute_name_list,
				foreign_attribute_name_list,
				isa_related_folder->
					one2m_folder->
					folder_name,
				folder_name );

		sprintf( from_buffer + strlen( from_buffer ),
			 ",%s",
			 get_table_name(
				application_name,
				isa_related_folder->
					one2m_folder->
					folder_name ) );

		if ( one_only ) break;

	} while( list_next( one2m_isa_related_folder_list ) );

	*from_clause = strdup( from_buffer );
	return where_clause;
}

char *query_output_mto1_isa_where(
			char *application_name,
			char *folder_name,
			LIST *mto1_isa_related_folder_list,
			LIST *primary_attribute_name_list,
			char *where_clause,
			boolean one_only )
{
	LIST *foreign_attribute_name_list;
	RELATED_FOLDER *isa_related_folder;

	if ( !list_rewind( mto1_isa_related_folder_list ) )
		return where_clause;

	do {
		isa_related_folder =
			list_get(
				mto1_isa_related_folder_list );

		foreign_attribute_name_list =
			isa_related_folder->
				folder->
				primary_attribute_name_list;

		where_clause =
			query_related_join_where(
				application_name,
				where_clause /* source_where */,
				primary_attribute_name_list,
				foreign_attribute_name_list,
				isa_related_folder->
					folder->
					folder_name /* folder_name */,
				folder_name /* related_folder_name */ );

		if ( one_only ) break;

	} while( list_next( mto1_isa_related_folder_list ) );

	return where_clause;
}

char *query_related_join_where(
			char *application_name,
			char *source_where_clause,
			LIST *primary_attribute_name_list,
			LIST *foreign_attribute_name_list,
			char *folder_name,
			char *related_folder_name )
{
	char where_clause[ 65536 ];
	char *where_ptr = where_clause;
	char *primary_attribute_name;
	char *foreign_attribute_name;
	boolean prepend_and_clause = 0;
	char *table_name;
	char *related_table_name;

	table_name =
		get_table_name(
			application_name,
			folder_name );

	related_table_name =
		get_table_name(
			application_name,
			related_folder_name );

	if ( source_where_clause )
	{
		where_ptr += sprintf( where_ptr, "%s", source_where_clause );
	}
	else
	{
		*where_ptr = '\0';
	}

	if ( !list_rewind( primary_attribute_name_list ) )
		return strdup( where_clause );

	if ( !list_rewind( foreign_attribute_name_list ) )
	{
		return strdup( where_clause );
	}

	if ( source_where_clause
	&&   *source_where_clause )
	{
		prepend_and_clause = 1;
	}

	do {
		primary_attribute_name =
			list_get(
				primary_attribute_name_list );

		foreign_attribute_name =
			list_get(
				foreign_attribute_name_list );

		if ( prepend_and_clause )
			where_ptr += sprintf( where_ptr, " and" );

		where_ptr += sprintf(
				where_ptr,
				" %s.%s = %s.%s",
				related_table_name,
				primary_attribute_name,
				table_name,
				foreign_attribute_name );

		prepend_and_clause = 1;

		if ( !list_next( foreign_attribute_name_list ) ) break;

	} while( list_next( primary_attribute_name_list ) );

	return strdup( where_clause );
}

QUERY *query_prompt_folder_data_new(
			char *application_name,
			char *login_name,
			char *folder_name,
			DICTIONARY *dictionary,
			ROLE *role,
			int max_rows )
{
	QUERY *query;

	query = query_calloc();

	if ( !folder_name )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty folder_name.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( ! ( query->folder =
			folder_with_load_new(
				application_name,
				BOGUS_SESSION,
				folder_name,
				role ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query->login_name = login_name;
	query->dictionary = dictionary;
	query->max_rows = max_rows;

	query->query_output =
		query_prompt_data_output_new(
			query,
			query->folder );

	return query;
}

QUERY_OUTPUT *query_prompt_data_output_new(
			QUERY *query,
			FOLDER *folder )
{
	QUERY_OUTPUT *query_output;
	LIST *exclude_attribute_name_list = list_new();

	query_output = query_output_calloc();

	if ( !folder )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query_output->from_clause =
		query_from_clause(
			folder->folder_name,
			(LIST *)0 /* isa_attribute_list */,
			(char *)0 /* attribute_not_null_folder_name */ );

	query_output->
		query_drop_down_list =
			query_related_data_drop_down_list(
				exclude_attribute_name_list,
				folder /* root_folder */,
				folder->
					mto1_related_folder_list,
				folder->
				    mto1_append_isa_related_folder_list,
				query->dictionary );

	query_output->
		query_attribute_list =
			query_get_attribute_list(
				folder->append_isa_attribute_list,
				query->dictionary,
				exclude_attribute_name_list,
				folder->folder_name
					/* dictionary_prepend_folder_name */ );

	query_output->where_clause =
		query_combined_where_clause(
			&query_output->drop_down_where_clause,
			&query_output->attribute_where_clause,
			query_output->query_drop_down_list,
			query_output->query_attribute_list,
			folder->application_name,
			folder->folder_name,
			0 /* not combine_date_time */ );

	if ( folder->row_level_non_owner_forbid
	&&   list_length( folder->mto1_isa_related_folder_list ) )
	{
		RELATED_FOLDER *isa_related_folder;
		LIST *foreign_attribute_name_list;

		list_rewind( folder->mto1_isa_related_folder_list );

		do {
			isa_related_folder =
				list_get_pointer(
					folder->
					mto1_isa_related_folder_list );

			foreign_attribute_name_list =
				   folder_get_primary_attribute_name_list(
					isa_related_folder->folder->
						attribute_list );

			query_output->where_clause =
				query_append_where_clause_related_join(
					folder->application_name,
					query_output->where_clause,
					folder_get_primary_attribute_name_list(
						folder->attribute_list ),
					foreign_attribute_name_list,
					folder->folder_name,
					isa_related_folder->
						folder->
						folder_name );

		} while( list_next(
				folder->
				mto1_isa_related_folder_list ) );
	}

	return query_output;
}

char *query_join_where_clause(
			LIST *primary_attribute_name_list,
			LIST *related_attribute_name_list,
			char *folder_name,
			char *related_folder_name )
{
	char where_clause[ 65536 ];
	char *where_ptr = where_clause;
	char *primary_attribute_name;
	char *related_attribute_name;
	boolean prepend_and_clause = 0;

	*where_ptr = '\0';

	if ( !list_length( primary_attribute_name_list ) )
		return strdup( where_clause );

	list_rewind( primary_attribute_name_list );
	list_rewind( related_attribute_name_list );

	do {
		primary_attribute_name =
			list_get(
				primary_attribute_name_list );

		if ( list_length( related_attribute_name_list ) )
		{
			related_attribute_name =
				list_get(
					related_attribute_name_list );
		}
		else
		{
			related_attribute_name = primary_attribute_name;
		}

		if ( !prepend_and_clause )
			prepend_and_clause = 1;
		else
			where_ptr += sprintf( where_ptr, " and" );

		where_ptr += sprintf(
				where_ptr,
				" %s.%s = %s.%s",
				folder_name,
				primary_attribute_name,
				related_folder_name,
				related_attribute_name );

		if ( list_length( related_attribute_name_list ) )
			list_next( related_attribute_name_list );
	} while( list_next( primary_attribute_name_list ) );

	return strdup( where_clause );
}

char *query_where_clause(
			LIST *primary_key_list,
			char *input_buffer,
			char delimiter )
{
	char *primary_key;
	char piece_buffer[ 256 ];
	char where[ 2048 ];
	char *ptr = where;
	char *escaped_data;
	int i;

	if ( !list_rewind( primary_key_list ) ) return (char *)0;


	for(	i = 0;
		piece(	piece_buffer,
			delimiter,
			input_buffer,
			i );
		i++ )
	{
		primary_key = list_get_pointer( primary_key_list );

		if ( i ) ptr += sprintf( ptr, " and" );

		escaped_data =
			timlib_sql_injection_escape(
				piece_buffer );

		ptr += sprintf(
		   		ptr,
		   		" %s = '%s'",
				primary_key,
		   		escaped_data );

		free( escaped_data );

		if ( !list_next( primary_key_list ) ) break;
	}

	return strdup( where );
}

char *query_from_clause(
			char *folder_name,
			LIST *isa_attribute_list,
			char *attribute_not_null_folder_name )
{
	LIST *from_list;

	from_list = list_string_list( folder_name, ',' );

	from_list =
		list_append_unique_string_list(
			from_list,
			attribute_distinct_folder_name_list(
				isa_attribute_list ) );

	if ( attribute_not_null_folder_name
	&&   *attribute_not_null_folder_name
	&&   !list_exists_string(
			attribute_not_null_folder_name,
			from_list ) )
	{
		list_set( from_list, attribute_not_null_folder_name );
	}

	return list_display_delimited( from_list, ',' );
}


LIST *query_detail_dictionary_list(
			char *application_name,
			char *select_clause,
			char *from_clause,
			char *where_clause,
			char *order_clause,
			int max_rows,
			LIST *append_isa_attribute_list,
			char *login_name )
{
	LIST *row_dictionary_list = {0};
	char *sys_string;
	LIST *attribute_name_list;

	sys_string = 
		query_get_sys_string(
			application_name,
			select_clause,
			from_clause,
			where_clause,
			(char *)0 /* subquery_where_clause */,
			order_clause,
			max_rows );

	attribute_name_list =
	 	attribute_lookup_allowed_attribute_name_list(
			append_isa_attribute_list );

	row_dictionary_list =
		list_usage_pipe2dictionary_list(
			sys_string, 
			attribute_name_list,
	 		attribute_date_attribute_name_list(
				append_isa_attribute_list ),
			FOLDER_DATA_DELIMITER,
			application_name,
			login_name );

	return row_dictionary_list;
}

QUERY *query_simple_new(
			DICTIONARY *dictionary,
			char *application_name,
			char *login_name,
			char *folder_name )
{
	QUERY *query;

	dictionary_appaserver_parse_multi_attribute_keys(
		dictionary,
		QUERY_RELATION_OPERATOR_STARTING_LABEL );

	query = query_calloc();

	if ( ! ( query->folder =
			folder_with_load_new(
				application_name,
				BOGUS_SESSION,
				folder_name,
				(ROLE *)0 /* role */ ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot load folder.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query->login_name = login_name;
	query->dictionary = dictionary;
	query->max_rows = QUERY_MAX_ROWS;

	query->prompt_recursive =
		prompt_recursive_new(
			application_name,
			query->folder->folder_name
				/* query_folder_name */,
			query->
				folder->
				mto1_related_folder_list );

	query->query_output =
		query_edit_table_output_new(
			query,
			query->folder->folder_name
				/* first_folder_name */,
			query->folder,
			query->folder->append_isa_attribute_list,
			query->prompt_recursive,
			query->folder->folder_name
				/* select_folder_name */,
			(char *)0 /* attribute_not_null_join */,
			(char *)0 /* attribute_not_null_folder_name */ );

	return query;
}

QUERY *query_sort_order_new(
			DICTIONARY *dictionary,
			FOLDER *folder )
{
	QUERY *query;

	dictionary_appaserver_parse_multi_attribute_keys(
		dictionary,
		QUERY_RELATION_OPERATOR_STARTING_LABEL );

	query = query_calloc();
	query->folder = folder;

	query->dictionary = dictionary;
	query->max_rows = QUERY_MAX_ROWS;

	query->query_output =
		query_sort_order_output_new(
			query );

	return query;
}

QUERY_OUTPUT *query_sort_order_output_new(
			QUERY *query )
{
	QUERY_OUTPUT *query_output;
	LIST *exclude_attribute_name_list;

	if ( !query )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty query.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !query->folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty folder.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	query_output = query_output_calloc();
	exclude_attribute_name_list = list_new();

	query_output->
		query_drop_down_list =
			query_edit_table_drop_down_list(
				exclude_attribute_name_list,
				query->folder->folder_name,
				query->folder->
					mto1_related_folder_list,
				query->folder->
				    mto1_append_isa_related_folder_list,
				query->dictionary );

	if ( query->folder->row_level_non_owner_forbid )
	{
		query_set_row_level_non_owner_forbid_dictionary(
			query,
			query->folder );
	}

	query_output->query_attribute_list =
		query_get_attribute_list(
			query->folder->attribute_list,
			query->dictionary,
			exclude_attribute_name_list,
			query->folder->folder_name
				/* dictionary_prepend_folder_name */ );

	query_output->non_joined_where_clause =
	query_output->where_clause =
	query_edit_table_where(
		&query_output->drop_down_where_clause,
		&query_output->attribute_where_clause,
		query_output->query_drop_down_list,
		query_output->query_attribute_list,
		query->folder->application_name,
		query->folder->folder_name,
		0 /* not combine_date_time */ );

	if ( !query_output->where_clause
	||   !*query_output->where_clause )
	{
		query_output->non_joined_where_clause =
		query_output->where_clause = "1 = 1";
	}

	query_output->select_clause =
		query_get_select_clause(
			query->folder->application_name,
			query->folder->attribute_list );

	query_output->from_clause =
		query_from_clause(
			query->folder->folder_name,
			query->folder->attribute_list,
			(char *)0 /* attribute_not_null_folder_name */ );

	if ( dictionary_length( query->sort_dictionary ) )
	{
		query_output->order_clause =
			query_get_order_clause(
				query->sort_dictionary,
				query->folder->folder_name,
				query->folder->attribute_list );
	}

	if ( !query_output->order_clause )
	{
		query_output->order_clause = query_output->select_clause;
	}

	return query_output;
}

