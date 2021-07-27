/* $APPASERVER_LIBRARY/library/query.c					*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "appaserver_library.h"
#include "timlib.h"
#include "String.h"
#include "security.h"
#include "date_convert.h"
#include "form.h"
#include "environ.h"
#include "attribute.h"
#include "prompt_recursive.h"
#include "query.h"

QUERY *query_calloc( void )
{
	QUERY *query;

	if ( ! ( query = calloc( 1, sizeof( QUERY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
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
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

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
		query_attribute_list(
			folder->append_isa_attribute_list,
			query->dictionary,
			exclude_attribute_name_list );

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

		} while( list_next( folder-> mto1_isa_related_folder_list ) );
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
		query_attribute_list(
			attribute_list,
			dictionary,
			(LIST *)0
				/* exclude_attribute_name_list */ );

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

char *query_system_string( 	char *select_clause,
				char *from_clause,
				char *where_clause,
				char *order_clause,
				int maxrows )
{
	char system_string[ QUERY_WHERE_BUFFER ];

	if ( !select_clause || !*select_clause )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: select_clause is empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !from_clause || !*from_clause )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: from_clause is empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !where_clause || !*where_clause )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: where_clause is empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !order_clause || !*order_clause )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: order_clause is empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf(sys_string,
		"get_folder_data	application=%s			 "
		"			select=\"%s\"			 "
		"			folder=%s			 "
		"			where_clause=\"%s\"		 "
		"			order_clause=\"%s\"		 "
		"			maxrows=\"%d\"			|"
		"cat							 ",
		environment_application_name(),
		select_clause,
		from_clause,
		where_clause,
		order_clause,
		maxrows );

	return strdup( system_string );
}

LIST *query_output_dictionary_list(
			char *query_select_display,
			LIST *query_output_select_name_list,
			char *query_output_from,
			char *query_output_where,
			char *query_output_order,
			int max_rows,
			QUERY_DATE_CONVERT *query_date_convert )
{
	char *system_string;

	system_string = 
		query_output_system_string(
			query_select_display,
			query_output_from,
			query_output_where,
			query_output_order,
			max_rows );

	return
		query_system_dictionary_list(
			system_string,
			query_output_select_name_list,
			query_date_convert );
}

LIST *query_output_record_list(
			char *query_select_display,
			LIST *query_output_select_name_list,
			char *query_output_from,
			char *query_output_where,
			char *query_output_order,
			int max_rows,
			QUERY_DATE_CONVERT *query_date_convert )
{
	char *system_string;

	system_string = 
		query_output_system_string(
			query_select_display,
			query_output_from,
			query_output_where,
			query_output_order,
			max_rows );

	return pipe2list( system_string );
}

char *query_attribute_between_date_time_where(
			char *date_attribute_name,
			char *time_attribute_name,
			QUERY_DATA *date_from_data,
			QUERY_DATA *time_from_data,
			QUERY_DATA *date_to_data,
			QUERY_DATA *time_to_data,
			char *folder_name )
{
	char where_clause[ 4066 ];
	char *where_ptr;
	char *start_end_time;
	char *finish_begin_time;
	char *begin_date;
	char *end_date;
	char *begin_time;
	char *end_time;
	char *application_name = environment_application_name();

	begin_date = date_from_data->escaped_replaced_data;
	end_date = date_to_data->escaped_replaced_data;
	begin_time = date_from_data->escaped_replaced_data;
	end_time = date_to_data->escaped_replaced_data;

	if ( string_strcmp( begin_date, end_date ) != 0 )
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
			appaserver_library_full_attribute_name(
				application_name,
				folder_name,
				date_column_name ),
		 	begin_date,
			appaserver_library_full_attribute_name(
				application_name,
				folder_name,
				time_column_name ),
		 	begin_time,
			appaserver_library_full_attribute_name(
				application_name,
				folder_name,
				time_column_name ),
			start_end_time );

	where_ptr +=
		sprintf(
		   where_ptr,
		   " or ( %s = '%s' and %s = 'null' )",
			appaserver_library_full_attribute_name(
				application_name,
				folder_name,
				date_column_name ),
		 	begin_date,
			appaserver_library_full_attribute_name(
				application_name,
				folder_name,
				time_column_name ) );

	where_ptr +=
		sprintf(
		   where_ptr,
		   " or ( %s = '%s' and %s = 'null' )",
			appaserver_library_full_attribute_name(
				application_name,
				folder_name,
				date_column_name ),
		 	end_date,
			appaserver_library_full_attribute_name(
				application_name,
				folder_name,
				time_column_name ) );

	where_ptr +=
		sprintf(
		   where_ptr,
		   " or ( %s > '%s' and %s < '%s' )",
			appaserver_library_full_attribute_name(
				application_name,
				folder_name,
				date_column_name ),
		 	begin_date,
			appaserver_library_full_attribute_name(
				application_name,
				folder_name,
				date_column_name ),
			end_date );

	where_ptr +=
		sprintf(
		   where_ptr,
		   " or ( %s = '%s' and %s >= '%s' and %s <= '%s' ) )",
			appaserver_library_full_attribute_name(
				application_name,
				folder_name,
				date_column_name ),
		 	end_date,
			appaserver_library_full_attribute_name(
				application_name,
				folder_name,
				time_column_name ),
			finish_begin_time,
			appaserver_library_full_attribute_name(
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
		query_sys_string(
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

LIST *query_process_drop_down_list(
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
			query_process_drop_down(
					exclude_attribute_name_list,
					folder_name,
					related_folder->
						foreign_attribute_name_list,
					related_folder->folder->attribute_list,
					preprompt_dictionary ) ) )
			{
				if ( !drop_down_list )
					drop_down_list = list_new();

				list_set( drop_down_list, query_drop_down );
			}

	} while( list_next( mto1_recursive_related_folder_list ) );

	return drop_down_list;
}

LIST *query_drop_down_list(
			LIST *exclude_attribute_name_list,
			FOLDER *mto1_folder,
			DICTIONARY *query_dictionary )
{
	LIST *drop_down_list = {0};
	RELATED_FOLDER *related_folder;
	RELATED_FOLDER *sub_related_folder;
	QUERY_DROP_DOWN *drop_down;
	LIST *mto1_isa_related_folder_list;

	if ( !list_rewind( mto1_folder->mto1_related_folder_list ) )
		goto check_mto1_isa_related_folder_list;

	do {
		related_folder =
			list_get(
				mto1_folder->
					mto1_related_folder_list );

		if ( ( drop_down =
			query_drop_down(
				exclude_attribute_name_list,
				mto1_folder->folder_name
					/* mto1_folder_name */,
				related_folder->
					folder->
					folder_name
					/* one2m_folder_name */,
				related_folder->
					folder->
					primary_attribute_name_list
					/* one2m_primary_attribute_name_list */,
				related_folder->
					foreign_attribute_name_list
					/* mto1_foreign_attribute_name_list */,
				related_folder->folder->attribute_list
					/* one2m_attribute_list */,
				query_dictionary ) ) )
		{
			if ( !drop_down_list )
				drop_down_list =
					list_new();

			list_set(
				drop_down_list,
				query_drop_down );
		}

	} while( list_next( mto1_folder->mto1_related_folder_list ) );

check_mto1_isa_related_folder_list:

	mto1_isa_related_folder_list =
		mto1_folder->mto1_isa_related_folder_list;

	if ( !list_rewind( mto1_isa_related_folder_list ) )
		return drop_down_list;

	do {
		related_folder =
			list_get(
				mto1_isa_related_folder_list );

		if ( !related_folder->relation_type_isa )
		{
			fprintf(stderr,
		"Warning in %s/%s()/%d: relation_type_isa is not set for %s.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				related_folder->folder->folder_name );

			continue;
		}

		mto1_folder = related_folder->folder;

		if ( !list_rewind( mto1_folder->mto1_related_folder_list ) )
		{
			continue;
		}

		do {
			sub_related_folder =
				list_get(
					mto1_folder->
						mto1_related_folder_list );

			if ( ( drop_down =
				query_drop_down(
					exclude_attribute_name_list,
					mto1_folder->folder_name
						/* mto1_folder_name */,
					sub_related_folder->
						folder->
						folder_name
						/* one2m_folder_name */,
					sub_related_folder->
						folder->
						primary_attribute_name_list,
					sub_related_folder->
						foreign_attribute_name_list,
					sub_related_folder->
						folder->
						attribute_list
						/* one2m_attribute_list */,
					query_dictionary ) ) )
			{
				if ( !drop_down_list )
					drop_down_list =
						list_new();

				list_set( drop_down_list, query_drop_down );
			}

		} while( list_next( 
				mto1_folder->
					mto1_related_folder_list ) );

	} while( list_next( mto1_isa_related_folder_list ) );

	return drop_down_list;
}

QUERY_DROP_DOWN *query_drop_down(
			LIST *exclude_attribute_name_list,
			char *mto1_folder_name,
			char *one2m_folder_name,
			LIST *one2m_primary_attribute_name_list,
			LIST *mto1_foreign_attribute_name_list,
			LIST *one2m_attribute_list,
			DICTIONARY *query_dictionary )
{
	QUERY_DROP_DOWN *drop_down = {0};
	int highest_index;
	LIST *drop_down_row_list;

	highest_index =
		dictionary_attribute_name_list_highest_index(
			query_dictionary,
			one2m_primary_attribute_name_list );

	if ( highest_index == -1 ) return (QUERY_DROP_DOWN *)0;

	drop_down_row_list =
		query_drop_down_row_list(
			exclude_attribute_name_list,
			mto1_folder_name,
			one2m_folder_name,
			one2m_primary_attribute_name_list,
			mto1_foreign_attribute_name_list,
			one2m_attribute_list,
			query_dictionary )
			highest_index );

	if ( list_length( drop_down_row_list ) )
	{
		drop_down =
			query_drop_down_new(
				folder_name,
				mto1_foreign_folder_name );

		drop_down->drop_down_row_list = drop_down_row_list;
	}

	return drop_down;
}

LIST *query_drop_down_row_list(
			LIST *exclude_attribute_name_list,
			char *mto1_folder_name,
			char *one2m_folder_name,
			LIST *one2m_primary_attribute_name_list,
			LIST *mto1_foreign_attribute_name_list,
			LIST *one2m_attribute_list,
			DICTIONARY *query_dictionary,
			int highest_index )
{
	int index;
	QUERY_DROP_DOWN_ROW *drop_down_row;
	LIST *drop_down_row_list = {0};

	for( index = 0; index <= highest_index; index++ )
	{
		drop_down_row =
			query_drop_down_row(
				exclude_attribute_name_list,
				mto1_folder_name,
				one2m_folder_name,
				one2m_primary_attribute_name_list,
				mto1_foreign_attribute_name_list,
				one2m_attribute_list,
				query_dictionary,
				index );

		if ( drop_down_row )
		{
			if ( !drop_down_row_list )
				drop_down_row_list =
					list_new();

			list_set( drop_down_row_list, drop_down_row );
		}
	}
	return drop_down_row_list;
}

char *query_key(	LIST *foreign_attribute_name_list,
			int index )
{
	static char key[ 512 ];
	char *tmp;

	*key = '\0';

	if ( !list_length( foreign_attribute_name_list ) )
		return key;

	sprintf(key,
		"%s_%d",
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		( tmp =
			list_display_delimited(
				foreign_attribute_name_list,
				MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER ) ),
		index );

	free( tmp );
	return key;
}

char *query_data_list_string(
			DICTIONARY *query_dictionary,
			char *query_key )
{
	return dictionary_fetch( query_dictionary, query_key );
}

QUERY_DROP_DOWN_ROW *query_drop_down_row(
			LIST *exclude_attribute_name_list,
			char *mto1_folder_name,
			char *one2m_folder_name,
			LIST *one2m_primary_attribute_name_list,
			LIST *mto1_foreign_attribute_name_list,
			LIST *one2m_attribute_list,
			DICTIONARY *query_dictionary,
			int index )
{
	QUERY_DROP_DOWN_ROW *drop_down_row = {0};
	char *key;
	char *data_list_string;
	char *operator_name;
	enum relational_operator relational_operator;
	char *one2m_attribute_name;
	char *mto1_attribute_name;

	if ( !list_length( mto1_foreign_attribute_name_list ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: empty mto1_foreign_attribute_name_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (QUERY_DROP_DOWN_ROW *)0;
	}

	key = query_key( mto1_foreign_attribute_name_list, index );

	operator_name =
		query_dictionary_operator_name(
			key,
			query_dictionary,
			QUERY_RELATION_OPERATOR_STARTING_LABEL );

	relational_operator =
		query_relational_operator(
			dictionary_operator_name,
			(char *)0 /* attribute_datatype */ );

	if ( index == 0 && relational_operator != equals )
	{
		return (QUERY_DROP_DOWN_ROW *)0;
	}

	/* -------------------------------------------------------- */
	/* If multi-drop-down, then no relational_operator is okay. */
	/* -------------------------------------------------------- */

	if ( ! ( data_list_string =
			query_data_list_string(
				query_dictionary,
				key ) ) )
	{
		return (QUERY_DROP_DOWN_ROW *)0;
	}

	query_drop_down_row =
		query_drop_down_row_new(
			mto1_folder_name,
			one2m_folder_name,
			one2m_primary_attribute_name_list,
			mto1_foreign_attribute_name_list,
			one2m_attribute_list,
			index );

	query_drop_down_row->key = key;
	query_drop_down_row->operator_name = operator_name;
	query_drop_down_row->relational_operator = relational_operator;
	query_drop_down_row->data_list_string = data_list_string;

	if ( strcmp( data_list_string, NULL_OPERATOR ) == 0 )
	{
		int length = list_length( mto1_foreign_attribute_name_list );

		query_drop_down_row->query_data_list_string =
			string_delimiter_repeat(
			   NULL_OPERATOR,
			   MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			   length /* number_times */ );
	}
	else
	if ( strcmp( data_list_string, NOT_NULL_OPERATOR ) == 0 )
	{
		int length = list_length( mto1_foreign_attribute_name_list );

		query_drop_down_row->query_data_list_string =
			string_delimiter_repeat(
			   NOT_NULL_OPERATOR,
			   MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			   length /* number_times */ );
	}
	else
	{
		query_drop_down_row->query_data_list_string =
			data_list_string;
	}

	query_drop_down_row->data_string_list =
		query_data_string_list(
			query_drop_down_row->
				query_data_list_string );

	query_drop_down_row->query_data_list =
		query_drop_down_query_data_list(
			query_drop_down_row->one2m_primary_attribute_name_list,
			query_drop_down_row->mto1_foreign_attribute_name_list,
			query_drop_down_row->one2m_attribute_list,
			query_drop_down_row->data_string_list );

	if ( exclude_attribute_name_list )
	{
		list_set_list(
			exclude_attribute_name_list,
			query_drop_down_row->mto1_foreign_attribute_name_list );
	}

	return query_drop_down_row;
}

QUERY_DROP_DOWN_ROW *query_drop_down_row_calloc( void )
{
	QUERY_DROP_DOWN_ROW *query_drop_down_row;

	if ( ! ( query_drop_down_row =
			calloc( 1, sizeof( QUERY_DROP_DOWN_ROW ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );

		exit( 1 );
	}
	return query_drop_down_row;
}

QUERY_DROP_DOWN_ROW *query_drop_down_row_calloc( void )
{
	QUERY_DROP_DOWN_ROW *query_drop_down_row;

	if ( ! ( query_drop_down_row =
			calloc( 1, sizeof( QUERY_DROP_DOWN_ROW ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return query_drop_down_row;
}

QUERY_DROP_DOWN_ROW *query_drop_down_row_new(
			char *mto1_folder_name,
			char *one2m_folder_name,
			LIST *one2m_primary_attribute_name_list,
			LIST *mto1_foreign_attribute_name_list,
			LIST *one2m_attribute_list,
			int index )
{
	QUERY_DROP_DOWN_ROW *query_drop_down_row =
		query_drop_down_row_calloc();

	query_drop_down_row->mto1_folder_name = mto1_folder_name;
	query_drop_down_row->one2m_folder_name = one2m_folder_name;

	query_drop_down_row->one2m_primary_attribut_name_list =
		one2m_primary_attribut_name_list;

	query_drop_down_row->mto1_foreign_attribute_name_list =
		mto1_foreign_attribute_name_list;

	query_drop_down_row->one2m_attribute_list =
		one2m_attribute_list;

	query_drop_down_row->index = index;

	return query_drop_down_row;
}

QUERY_DROP_DOWN *query_drop_down_new(
			char *root_folder_name,
			char *mto1_foreign_folder_name )
{
	QUERY_DROP_DOWN *query_drop_down;

	if ( ! ( query_drop_down = calloc( 1, sizeof( QUERY_DROP_DOWN ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	query_drop_down->root_folder_name = root_folder_name;
	query_drop_down->mto1_foreign_folder_name = mto1_foreign_folder_name;

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
#endif

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

		if ( ! ( operator_name =
			   query_dictionary_operator_name(
				attribute_name /* key */,
				dictionary,
				QUERY_RELATION_OPERATOR_STARTING_LABEL ) ) )
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

	*data = security_sql_injection_escape( *data );

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

char *query_dictionary_operator_name(
			char *key,
			DICTIONARY *dictionary,
			char *starting_label )
{
	char search_string[ 1024 ];
	char *operator_name;

	sprintf( search_string,
		 "%s%s",
		 starting_label,
		 key );

	if ( ( operator_name =
			dictionary_fetch(
				dictionary,
				search_string ) ) )
	{
		return operator_name;
	}

	/* ----------------------------------------------------------- */
	/* Returns: 	1 if the data comes from the index	       */
	/*		0 if the data comes from index = 0 or no index */
	/*	       -1 if the data is not found	   	       */
	/* ----------------------------------------------------------- */
	if ( dictionary_index_data(
			&operator_name,
			dictionary, 
			search_string,
			0 /* dictionary_offset */ ) < 0 )
	{
		return (char *)0;
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
		date_convert_date_time_evaluate(
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

QUERY_DATA *query_data_escaped(
			DICTIONARY *dictionary,
			char *attribute_name,
			char *attribute_datatype,
			int dictionary_offset,
			char *starting_label,
			char *dictionary_prepend_folder_name )
{
	char *data_string = {0};
	boolean results;

	results =
		query_get_dictionary_data(
			&data_string,
			dictionary, 
			attribute_name,
			dictionary_offset,
			starting_label,
			dictionary_prepend_folder_name );

	if ( !results || !data_string || !*data_string )
		return (QUERY_DATA *)0;

	return
		query_data_new(
			attribute_name,
			attribute_datatype,
			security_sql_injection_escape(
				security_replace_special_characters(
					string_trim_number_characters(
						data_string,
						attribute_datatype ) ) ) );
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
			QUERY_DATA *from_data,
			QUERY_DATA *to_data,
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

	query_attribute->attribute_name = attribute_name;
	query_attribute->folder_name = folder_name;
	query_attribute->datatype = datatype;
	query_attribute->relational_operator = relational_operator;
	query_attribute->primary_key_index = primary_key_index;
	query_attribute->from_data = from_data;
	query_attribute->to_data = to_data;

	return query_attribute;
}

LIST *query_attribute_list(
			LIST *append_isa_attribute_list,
			DICTIONARY *query_dictionary,
			LIST *exclude_attribute_name_list )
{
	LIST *query_attribute_list = {0};
	QUERY_ATTRIBUTE *query_attribute;
	ATTRIBUTE *attribute;
	QUERY_DATA *from_data;
	QUERY_DATA *to_data;
	char *operator_name;
	enum relational_operator relational_operator;

	if ( !list_rewind( append_isa_attribute_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: append_isa_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		attribute =
			list_get(
				append_isa_attribute_list );

		if ( list_exists_string(
			attribute->attribute_name,
			exclude_attribute_name_list ) )
		{
			continue;
		}

		if ( query_attribute_exists(
			query_attribute_list,
			attribute->attribute_name ) )
		{
			continue;
		}

		if ( ! ( operator_name =
				query_dictionary_operator_name(
					attribute->attribute_name,
					query_dictionary,
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
			query_relational_operator(
				operator_name,
				attribute->datatype );

		from_data = (QUERY_DATA *)0;
		to_data = (QUERY_DATA *)0;

		if ( relational_operator != is_null
		&&   relational_operator != not_null
		&&   ! ( from_data =
				query_data_escaped(
					dictionary,
					attribute->attribute_name,
					attribute->datatype,
					0 /* dictionary_offset */,
					QUERY_FROM_STARTING_LABEL,
					dictionary_prepend_folder_name ) ) )
		{
			continue;
		}

		to_data =
			query_data_escaped(
				dictionary, 
				attribute->attribute_name,
				attribute->datatype,
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
		{
			query_attribute_list = list_new();
		}

		list_set(
			query_attribute_list,
			query_attribute );

	} while( list_next( append_isa_attribute_list ) );

	return query_attribute_list;
}

enum relational_operator query_relational_operator(
			char *operator_name,
			char *attribute_datatype )
{
	enum relational_operator relational_operator;

	if ( !operator_name || !*operator_name )
		relational_operator = relational_operator_empty;
	else
	if ( strcmp( operator_name, EQUAL_OPERATOR ) == 0 )
		relational_operator = equals;
	else
	if ( strcmp( operator_name, NOT_EQUAL_OPERATOR ) == 0 )
		relational_operator = not_equal;
	else
	if ( strcmp( operator_name, NOT_EQUAL_OR_NULL_OPERATOR ) == 0 )
		relational_operator = not_equal_or_null;
	else
	if ( strcmp( operator_name, LESS_THAN_OPERATOR ) == 0 )
		relational_operator = less_than;
	else
	if ( strcmp( operator_name, LESS_THAN_EQUAL_TO_OPERATOR ) == 0 )
		relational_operator = less_than_equal_to;
	else
	if ( strcmp( operator_name, GREATER_THAN_OPERATOR ) == 0 )
		relational_operator = greater_than;
	else
	if ( strcmp( operator_name, GREATER_THAN_EQUAL_TO_OPERATOR ) == 0 )
		relational_operator = greater_than_equal_to;
	else
	if ( strcmp( operator_name, BETWEEN_OPERATOR ) == 0 )
		relational_operator = between;
	else
	if ( strcmp( operator_name, BEGINS_OPERATOR ) == 0 )
		relational_operator = begins;
	else
	if ( strcmp( operator_name, CONTAINS_OPERATOR ) == 0 )
		relational_operator = contains;
	else
	if ( strcmp( operator_name, NOT_CONTAINS_OPERATOR ) == 0 )
		relational_operator = not_contains;
	else
	if ( strcmp( operator_name, OR_OPERATOR ) == 0 )
		relational_operator = query_or;
	else
	if ( strcmp( operator_name, NULL_OPERATOR ) == 0 )
		relational_operator = is_null;
	else
	if ( strcmp( operator_name, NOT_NULL_OPERATOR ) == 0 )
		relational_operator = not_null;
	else
		relational_operator = relational_operator_unknown;

	if ( relational_operator == equals
	&& ( string_strcmp(	attribute_datatype,
				"date_time" ) == 0
	||     string_strcmp(	attribute_datatype,
				"current_date_time" ) == 0 ) )
	{
		relational_operator = begins;
	}

	return relational_operator;
}

char *query_display_where(
			char *query_output_where,
			char *folder_name )
{
	char remove_folder_name[ 256 ];

	if ( !query_output_where ) return "";

	sprintf(remove_folder_name,
		"%s.",
		folder_name );

	search_replace_string(
		query_output_where,
		remove_folder_name,
		"" );

	search_replace_string(
		query_output_where,
		"1 = 1 and ",
		"" );

	search_replace_string(
		query_output_where,
		"'",
		"" );

	format_initial_capital( query_output_where, query_output_where );

	if ( !*query_output_where )
	{
		strcpy( query_output_where, "Entire folder" );
	}

	return query_output_where;
}

char *query_output_related_join(
			char *folder_name,
			LIST *primary_attribute_name_list,
			char *isa_folder_name,
			LIST *isa_foreign_attribute_name_list )
{
	static char where[ 2048 ];
	char *ptr = where;
	char *primary_attribute_name;
	char *foreign_attribute_name;

	*ptr = '\0';

	if ( !list_length( primary_attribute_name_list ) )
		return where_clause;

	if (	list_length( primary_attribute_name_list ) !=
		list_length( isa_foreign_attribute_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: length = %d != length = %d.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_length( primary_attribute_name_list ),
			list_length( isa_foreign_attribute_name_list ) );
		exit( 1 );
	}

	list_rewind( primary_attribute_name_list );
	list_rewind( isa_related_attribute_name_list );

	do {
		primary_attribute_name =
			list_get(
				primary_attribute_name_list );

		foreign_attribute_name =
			list_get(
				isa_foreign_attribute_name_list );

		if ( !*ptr ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
				ptr,
				"%s.%s = %s.%s",
				folder_name,
				primary_attribute_name,
				isa_folder_name,
				foreign_attribute_name );

		list_next( isa_foreign_attribute_name_list );

	} while( list_next( primary_attribute_name_list ) );

	return where;
}

char *query_login_name_where_clause(
			FOLDER *folder,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			char *login_name )
{
	char where_clause[ QUERY_WHERE_BUFFER ];
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
	char where_clause[ QUERY_WHERE_BUFFER ];
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

char *query_attribute_list_display(
			char *subquery_folder_name,
			LIST *query_attribute_list )
{
	QUERY_ATTRIBUTE *query_attribute;
	char buffer[ QUERY_WHERE_BUFFER ];
	char *ptr = buffer;

	*ptr = '\0';

	ptr += sprintf(	ptr,
			"%s(): subquery_folder_name = %s:",
			__FUNCTION__,
			(subquery_folder_name) 		?
				subquery_folder_name	:
				"" );

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
"folder = %s; datatype = %s, relational_operator = %s, from_data = %s, to_data = %s\n",
				(query_attribute->folder_name)		?
					query_attribute->folder_name	:
					"",
				query_attribute->datatype,
				query_relational_operator_display(
					query_attribute->relational_operator ),
				(query_attribute->
					from_data->
					escaped_replaced_data )
					? query_attribute->
						from_data->
						escaped_replaced_data
					: "null",
				(query_attribute->
					to_data->
					escaped_replaced_data)
					? query_attribute->
						to_data->
						escaped_replaced_data
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
	char buffer[ QUERY_WHERE_BUFFER ];
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
	char buffer[ QUERY_WHERE_BUFFER ];
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
		char where_clause[ QUERY_WHERE_BUFFER ];
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
		char where_clause[ QUERY_WHERE_BUFFER ];

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

boolean query_attribute_date_time_between(
			QUERY_ATTRIBUTE **date_between_attribute,
			QUERY_ATTRIBUTE **time_between_attribute,
			LIST *query_attribute_list )
{
	QUERY_ATTRIBUTE *query_attribute;

	if ( !list_rewind( query_attribute_list ) ) return 0;

	*date_between_attribute = (QUERY_ATTRIBUTE *)0;
	*time_between_attribute = (QUERY_ATTRIBUTE *)0;

	do {
		query_attribute = list_get( query_attribute_list );

		if (  	query_attribute->primary_key_index
		&&    	query_attribute->relational_operator == between
		&&	attribute_is_time(
				query_attribute->datatype ) )
		{
			*time_between_attribute = query_attribute;
			continue;
		}

		if (   	query_attribute->primary_key_index
		&&     	query_attribute->relational_operator == between
		&&	attribute_is_date(
				query_attribute->datatype ) )
		{
			*date_between_attribute = query_attribute;
			continue;
		}

	} while( list_next( query_attribute_list ) );

	return ( *date_between_attribute && *time_between_attribute );
}

char *query_output_drop_down_data_where(
			char *mto1_folder_name,
			char *attribute_name,
			char *data )
{
	static char where[ 1024 ];
	char *application_name = environment_application_name();

	*where = '\0';

	if ( timlib_strcmp( data, NULL_OPERATOR ) == 0 )
	{
		sprintf(where,
			QUERY_NULL_EXPRESSION,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
		 	appaserver_library_full_attribute_name(
		 		application_name,
		 		folder_name,
		 		attribute_name ),
		 	appaserver_library_full_attribute_name(
		 		application_name,
		 		folder_name,
		 		attribute_name ),
		 	appaserver_library_full_attribute_name(
		 		application_name,
		 		folder_name,
		 		attribute_name ) );
	}
	else
	if ( timlib_strcmp( data, NOT_NULL_OPERATOR ) == 0 )
	{
		sprintf(where,
			QUERY_NOT_NULL_EXPRESSION,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
		 	appaserver_library_full_attribute_name(
		 		application_name,
		 		folder_name,
		 		attribute_name ),
		 	appaserver_library_full_attribute_name(
		 		application_name,
		 		folder_name,
		 		attribute_name ),
		 	appaserver_library_full_attribute_name(
		 		application_name,
		 		folder_name,
		 		attribute_name ) );
	}
	else
	{
		sprintf(where,
			" %s = '%s'",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
		 	appaserver_library_full_attribute_name(
		 		application_name,
		 		folder_name,
		 		attribute_name ),
		 	data );
	}

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

char *query_output_display(
			QUERY_OUTPUT *query_output )
{
	static char display_string[ QUERY_WHERE_BUFFER ];
	char *ptr = display_string;
	QUERY_SUBQUERY *subquery;

	ptr += sprintf( ptr, "%c", '"' );

	ptr += sprintf( ptr,
			"%s",
			query_drop_down_list_display(
				query_output->query_drop_down_list) );

	ptr += sprintf( ptr,
			"%s",
			query_attribute_list_display(
				(char *)0 /* subquery_folder_name */,
				query_output->query_attribute_list ) );

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
				query_attribute_list_display(
					subquery->folder_name,
					subquery->query_attribute_list ) );

		} while( list_next( query_output->query_subquery_list ) );
	}

	ptr += sprintf( ptr, "%c", '"' );

	return display_string;
}

char *query_get_drop_down_list_display(
			LIST *query_drop_down_list )
{
	static char display_string[ QUERY_WHERE_BUFFER ];
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

LIST *query_prompt_recursive_drop_down_list(
			LIST *exclude_attribute_name_list,
			char *query_folder_name,
			LIST *prompt_recursive_folder_list,
			DICTIONARY *query_dictionary )
{
	PROMPT_RECURSIVE_FOLDER *prompt_recursive_folder;
	QUERY_DROP_DOWN *drop_down;
	PROMPT_RECURSIVE_MTO1_FOLDER *prompt_recursive_mto1_folder;
	FOLDER *mto1_folder;
	LIST *return_list;
	LIST *drop_down_list;

	if ( !list_rewind( prompt_recursive_folder_list ) )
	{
		return (LIST *)0;
	}

	return_list = list_new();

	do {
		prompt_recursive_folder =
			list_get(
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

			mto1_folder = prompt_recursive_mto1_folder->folder;

			if ( list_is_subset_of(
				mto1_folder->primary_attribute_name_list,
				exclude_attribute_name_list ) )
			{
				continue;
			}

			drop_down_list =
				query_drop_down_list(
					exclude_attribute_name_list,
					mto1_folder,
					query_dictionary );

			if ( list_length( drop_down_list ) )
			{
				list_set_list(
					return_list,
					drop_down_list );
			}

		} while( list_next( prompt_recursive_folder->
					prompt_recursive_mto1_folder_list ) );

	} while( list_next( prompt_recursive_folder_list ) );

	return return_list;
}

char *query_get_dictionary_where_clause(
			DICTIONARY *dictionary,
			LIST *primary_attribute_name_list,
			char *dictionary_indexed_prefix )
{
	char where_clause[ QUERY_WHERE_BUFFER ];
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

int query_or_sequence_set_data_list_string(
			LIST *data_list_list,
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

			list_append_pointer(
				data_list_list,
				data_list );
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

int query_or_sequence_set_data_list(
			LIST *data_list_list,
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
	char where_clause[ QUERY_WHERE_BUFFER ];
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

boolean query_attribute_exists(
			LIST *query_attribute_list,
			char *attribute_name )
{
	QUERY_ATTRIBUTE *query_attribute;

	if ( !list_rewind( query_attribute_list ) ) return 0;

	do {
		query_attribute = list_get_pointer( query_attribute_list );

		if ( strcmp(
			query_attribute->attribute_name,
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
			security_sql_injection_escape(
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

/* Sets login_name or entity to query_dictionary */
/* --------------------------------------------- */
void query_dictionary_row_level_non_owner_forbid(
			DICTIONARY *query_dictionary,
			char *login_name_only,
			char *full_name_only,
			char *street_address_only )
{
	char key[ 128 ];

	if ( !query_dictionary )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: query_dictionary is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( full_name_only && *full_name_only )
	{
		sprintf(key,
			"%sfull_name_0",
			QUERY_FROM_STARTING_LABEL );

		dictionary_set_pointer(
			query_dictionary,
			strdup( key ),
			full_name_only );

		sprintf(key,
	 		"%sfull_name_0",
	 		QUERY_RELATION_OPERATOR_STARTING_LABEL );

		dictionary_set_pointer(
			query_dictionary,
			strdup( key ),
			EQUAL_OPERATOR );

		sprintf(key,
			"%sstreet_address_0",
			QUERY_FROM_STARTING_LABEL );

		dictionary_set_pointer(
			query_dictionary,
			strdup( key ),
			street_address_only );

		sprintf(key,
	 		"%sstreet_address_0",
	 		QUERY_RELATION_OPERATOR_STARTING_LABEL );

		dictionary_set_pointer(
			query_dictionary,
			strdup( key ),
			EQUAL_OPERATOR );
	}
	else
	if ( login_name_only && *login_name_only )
	{
		sprintf(key,
			"%slogin_name_0",
			QUERY_FROM_STARTING_LABEL );

		dictionary_set_pointer(
			query_dictionary,
			strdup( key ),
			login_name_only );

		sprintf(key,
	 		"%slogin_name_0",
	 		QUERY_RELATION_OPERATOR_STARTING_LABEL );

		dictionary_set_pointer(
			query_dictionary,
			strdup( key ),
			EQUAL_OPERATOR );
	}
	else
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: both login name and entity are empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
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

#ifdef NOT_DEFINED
QUERY_DROP_DOWN *query_related_data_drop_down(
				LIST *exclude_attribute_name_list,
				char *root_folder_name,
				LIST *foreign_attribute_name_list,
				LIST *attribute_list,
				DICTIONARY *dictionary )
{
	QUERY_DROP_DOWN *drop_down = {0};
	int highest_index;
	int index;

	highest_index =
		dictionary_attribute_name_list_highest_index(
			dictionary,
			foreign_attribute_name_list );

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
#endif

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

QUERY *query_edit_table_new(
			DICTIONARY *query_dictionary,
			char *login_name_only,
			char *full_name_only,
			char *street_address_only,
			FOLDER *mto1_folder,
			LIST *ignore_attribute_name_list,
			char *attribute_not_null_join,
			char *attribute_not_null_folder_name,
			DICTIONARY *sort_dictionary )
{
	QUERY *query;

	if ( mto1_folder->row_level_non_owner_forbid )
	{
		/* Sets login_name or entity to query_dictionary */
		/* --------------------------------------------- */
		query_dictionary_row_level_non_owner_forbid(
			query_dictionary,
			login_name_only,
			full_name_only,
			street_address_only );
	}

	query = query_calloc();

	query->dictionary = query_dictionary;
	query->login_name = login_name;
	query->mto1_folder = mto1_folder;
	query->sort_dictionary = sort_dictionary;
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
			query->dictionary,
			query->mto1_folder,
			query->ignore_attribute_name_list,
			query->prompt_recursive,
			attribute_not_null_join,
			attribute_not_null_folder_name,
			query->sort_dictionary );

	return query;
}

LIST *query_drop_down_list(
			LIST *exclude_attribute_name_list,
			FOLDER *mto1_folder,
			DICTIONARY *query_dictionary )
{
	LIST *drop_down_list = {0};
	RELATED_FOLDER *related_folder;
	RELATED_FOLDER *sub_related_folder;
	QUERY_DROP_DOWN *drop_down;
	LIST *mto1_related_folder_list;
	LIST *mto1_isa_related_folder_list;

	mto1_related_folder_list =
		mto1_folder->
			mto1_related_folder_list;

	mto1_isa_related_folder_list =
		mto1_folder->
			mto1_isa_related_folder_list;

	if ( !list_rewind( mto1_related_folder_list ) )
		goto check_mto1_isa_related_folder_list;

	do {
		related_folder =
			list_get(
				mto1_related_folder_list );

		drop_down =
			query_drop_down(
				exclude_attribute_name_list,
				mto1_folder->folder_name,
				related_folder->folder_name
					/* one2m_folder_name */,
				related_folder->primary_attribute_name_list
					/* one2m_primary_attribute_name_list */,
				related_folder->foreign_attribute_name_list
					/* mto1_foreign_attribute_name_list */,
				related_folder->folder->attribute_list
					/* one2m_attribute_list */,
				query_dictionary ) );

		if ( drop_down )
		{
			if ( !drop_down_list ) drop_down_list = list_new();

			list_set(
				drop_down_list,
				drop_down );
		}

	} while( list_next( mto1_related_folder_list ) );

check_mto1_isa_related_folder_list:

	if ( !list_rewind( mto1_isa_related_folder_list ) )
		return drop_down_list;

	do {
		related_folder =
			list_get(
				mto1_isa_related_folder_list );

		if ( !related_folder->relation_type_isa )
		{
			fprintf(stderr,
			"Warning in %s/%s()/%d: expecting relation_type_isa.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );

			continue;
		}

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

			drop_down =
			     query_drop_down(
				exclude_attribute_name_list,
				mto1_folder->folder_name,
				sub_related_folder->folder_name
					/* one2m_folder_name */,
				sub_related_folder->primary_attribute_name_list
					/* one2m_primary_attribute_name_list */,
				sub_related_folder->foreign_attribute_name_list
					/* mto1_foreign_attribute_name_list */,
				sub_related_folder->folder->attribute_list
					/* one2m_attribute_list */,
				query_dictionary ) );

			if ( drop_down )
			{
				if ( !drop_down_list )
					drop_down_list =
						list_new();

				list_set(
					drop_down_list,
					drop_down );
			}

		} while( list_next( 
				related_folder->
					folder->
					mto1_related_folder_list ) );

	} while( list_next( mto1_isa_related_folder_list ) );

	return drop_down_list;
}

QUERY_DROP_DOWN *query_edit_table_drop_down(
			LIST *exclude_attribute_name_list,
			char *root_folder_name,
			char *mto1_foreign_folder_name,
			LIST *foreign_attribute_name_list,
			LIST *foreign_attribute_list,
			DICTIONARY *query_dictionary )
{
	QUERY_DROP_DOWN *query_drop_down;
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	LIST *query_drop_down_row_list;
	int highest_index;
	int index;
	char *operator_name;
	enum relational_operator relational_operator;

	/* Check for relational operator of equals (maybe) */
	/* ----------------------------------------------- */
	operator_name =
		query_dictionary_operator_name(
			mto1_foreign_folder_name,
			query_dictionary,
			QUERY_RELATION_OPERATOR_STARTING_LABEL );

	relational_operator =
		query_relational_operator(
			operator_name,
			(char *)0 /* attribute_datatype */ );

	/* If multi-drop-down, then no relational_operator is okay. */
	/* -------------------------------------------------------- */
	if ( index == 0 && relational_operator != equals )
	{
		return (QUERY_DROP_DOWN *)0;
	}

	highest_index =
		dictionary_attribute_name_list_highest_index(
			dictionary,
			foreign_attribute_name_list );

	if ( highest_index == -1 ) return (QUERY_DROP_DOWN *)0;

	query_drop_down_row_list =
		query_edit_table_drop_down_row_list(
			exclude_attribute_name_list,
			root_folder_name,
			mto1_foreign_folder_name,
			foreign_attribute_name_list,
			foreign_attribute_list,
			query_dictionary,
			highest_index );

	if ( list_length( query_drop_down_row_list ) )
	{
		query_drop_down =
			query_drop_down_new(
				strdup( root_folder_name ),
				mto1_foreign_folder_name );

		query_drop_down->query_drop_down_row_list =
			query_drop_down_row_list;

		return query_drop_down;
	}
	else
	{
		return (QUERY_DROP_DOWN *)0;
	}
}

LIST *query_edit_table_drop_down_row_list(
			LIST *exclude_attribute_name_list,
			char *root_folder_name,
			char *mto1_foreign_folder_name,
			LIST *foreign_attribute_name_list,
			LIST *foreign_attribute_list,
			DICTIONARY *query_dictionary,
			int highest_index )
{
	LIST *query_drop_down_row_list = {0};
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	int index;

	for( index = 0; index <= highest_index; index++ )
	{
		query_drop_down_row =
			query_edit_table_drop_down_row(
				exclude_attribute_name_list,
				foreign_attribute_name_list,
				foreign_attribute_list,
				query_dictionary,
				index,
				query_drop_down_row_list );

		if ( !query_drop_down_row_list )
		{
			query_drop_down_row_list = list_new();
		}

		list_set(
			query_drop_down_row_list,
			query_drop_down_row );
	}

	return query_drop_down_row_list;
}

#ifdef NOT_DEFINED
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
		query_dictionary_operator_name(
			key,
			dictionary,
			QUERY_RELATION_OPERATOR_STARTING_LABEL );

	relational_operator =
		query_relational_operator(
			operator_name,
			(char *)0 /* attribute_datatype */ );

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
		list_set_list(
			exclude_attribute_name_list,
			foreign_attribute_name_list );
	}

	list_set(
		query_drop_down->
			query_drop_down_row_list,
		drop_down_row );

	return query_drop_down;
}
#endif

QUERY_DROP_DOWN_ROW *query_drop_down_edit_table_new(
			LIST *attribute_name_list,
			LIST *attribute_list,
			LIST *query_data_list )
{
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	LIST *local_attribute_name_list = {0};
	LIST *local_data_list = {0};
	char *attribute_name;
	QUERY_DATA *data;
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
				security_sql_injection_escape( data ) );
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
	char where_clause[ QUERY_WHERE_BUFFER ];
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
		char where_clause[ QUERY_WHERE_BUFFER ];

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

#ifdef NOT_DEFINED
QUERY_OUTPUT *query_detail_output_new(
			FOLDER *folder,
			LIST *where_attribute_data_list,
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
		char where[ QUERY_WHERE_BUFFER ];

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
		query_output_select_clause(
			folder->append_isa_attribute_list,
			ignore_attribute_name_list,
			folder->lookup_attribute_exclude_name_list );

	return query_output;
}

QUERY *query_detail_new(
			char *application_name,
			char *login_name,
			FOLDER *folder,
			LIST *where_attribute_data_list,
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
#endif

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
			security_sql_injection_escape(
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

QUERY *query_simple_new(
			DICTIONARY *query_dictionary,
			char *login_name,
			char *folder_name,
			char *role_name,
			LIST *ignore_select_attribute_name_list )
{
	QUERY *query;
	LIST *exclude_attribute_name_list = list_new();

	query = query_calloc();

	if ( ! ( query->role = role_fetch( role_name ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: role_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( query->query_folder = query_folder_new( folder_name ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: query_folder_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( query->query_entity =
			query_entity_new(
				login_name,
				query_folder->non_owner_forbid,
				role->override_row_restrictions ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: query_entity_new() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( query->role_folder =
			role_folder_fetch(
				role_name,
				folder_name ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: role_folder_fetch() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( mto1_folder->row_level_non_owner_forbid )
	{
		/* Sets login_name or entity to query_dictionary */
		/* --------------------------------------------- */
		query_dictionary_row_level_non_owner_forbid(
			query_dictionary,
			query->query_entity->login_name_only,
			query->query_entity->full_name_only,
			query->query_entity->street_address_only );
	}

	query->dictionary = query_dictionary;
	query->login_name = login_name;
	query->folder_name = folder_name;
	query->role_name = role_name;
	query->ignore_attribute_name_list = ignore_attribute_name_list;
	query->max_rows = QUERY_MAX_ROWS;

	query->prompt_recursive =
		prompt_recursive_new(
			query->query_folder->folder_name,
			query->
				query_folder->
				mto1_related_folder_list );


	/* Drop-down list */
	/* -------------- */
	query->query_drop_down_list =
		query_drop_down_list(
			exclude_attribute_name_list,
			query->query_folder,
			query->dictionary );

	if ( prompt_recursive
	&&   list_length(
		prompt_recursive->
			prompt_recursive_folder_list ) )
	{
		query->prompt_recursive_drop_down_list =
			query_prompt_recursive_drop_down_list(
				exclude_attribute_name_list,
				query->query_folder->folder_name
					/* query_folder_name */,
				prompt_recursive->
					prompt_recursive_folder_list,
				query->dictionary );

		if ( list_length( query->prompt_recursive_drop_down_list ) )
		{
			if ( !query->query_drop_down_list )
			{
				query->query_drop_down_list = list_new();
			}

			list_set_list(
				query->query_drop_down_list,
				query->prompt_recursive_drop_down_list );
		}
	}

	/* Attribute list */
	/* -------------- */
	query->query_attribute_list =
		query_attribute_list(
			query->query_folder->append_isa_attribute_list,
			query->dictionary,
			exclude_attribute_name_list );

	/* Select */
	/* ------ */
	query->query_select_name_list =
		query_select_name_list(
			query->query_folder->append_isa_attribute_list,
			query->ignore_select_attribute_name_list,
			query->
				query_folder->
				lookup_attribute_exclude_name_list,
			query->common_attribute_name_list );

	query->query_select_display =
		query_select_display(
			query->query_folder->folder_name,
			query->query_select_name_list,
			list_length(
				query->
					query_folder->
					mto1_isa_related_folder_list ),
			query->common_attribute_name_list );

	/* From */
	/* ---- */
	query->query_from =
		query_from(
			query->query_folder->folder_name,
			query->query_folder->mto1_isa_related_folder_list,
			query->attribute_not_null_folder_name );

	/* Where */
	/* ----- */
	query->query_drop_down_where =
		query_drop_down_where(
			query->query_drop_down_list,
			query->query_folder->folder_name,
			list_length(
				query->
					query_folder->
					mto1_isa_related_folder_list_length ) );

	query->query_attribute_where =
		query_attribute_where(
			query->query_attribute_list );

	query->query_join_where =
		query_join_where(
			query->query_folder->folder_name,
			query->query_folder->primary_attribute_name_list,
			query->query_folder->mto1_isa_related_folder_list,
			query->attribute_not_null_join );
			
	query->query_where =
		query_where(
			query->query_drop_down_where,
			query->query_attribute_where,
			query->query_join_where );

	/* Order */
	/* ----- */
	query->query_order =
		query_order(
			query->query_folder->folder_name,
			query->query_folder->primary_attribute_name_list,
			query->query_folder->append_isa_attribute_list,
			query->sort_dictionary );

	/* Date convert */
	/* ------------ */
	query->query_date_convert =
		query_date_convert(
			query->query_entity->login_name,
			query->query_folder->append_isa_attribute_list );


	return query;
}

QUERY_DATE_CONVERT *query_date_convert_calloc( void )
{
	QUERY_DATE_CONVERT *query_date_convert;

	if ( ! ( query_date_convert =
			calloc( 1, sizeof( QUERY_DATE_CONVERT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		exit( 1 );
	}
	return query_date_convert;
}

QUERY_DATE_CONVERT *query_date_convert(
			char *login_name,
			LIST *append_isa_attribute_list )
{
	QUERY_DATE_CONVERT *date_convert;
	LIST *date_attribute_name_list;
	DATE_CONVERT *convert;

	if ( !login_name || !*login_name )
		return (QUERY_DATE_CONVERT *)0;

	date_attribute_name_list =
		attribute_date_attribute_name_list(
			append_isa_attribute_list );

	if ( !list_length( date_attribute_name_list ) )
		return (QUERY_DATE_CONVERT *)0;

	date_convert = query_date_convert_calloc();

	date_convert->date_attribute_name_list =
		date_attribute_name_list;

	date_convert->database_date_format =
		date_convert_format_evaluate(
			application_database_date_format() );

	if ( ! ( convert =
			date_convert_login_name_fetch(
				login_name ) ) )
	{
		date_convert->user_date_format =
			date_convert->database_date_format;
	}
	else
	{
		date_convert->user_date_format =
			convert->user_date_format;
	}

	return date_convert;
}

QUERY_DATA *query_data_new(
			char *attribute_name,
			char *attribute_datatype,
			char *escaped_replaced_data )
{
	QUERY_DATA *query_data;

	if ( !escaped_replaced_data || !*escaped_replaced_data )
		return (QUERY_DATA *)0;

	if ( ! ( query_data = calloc( 1, sizeof( QUERY_DATA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		exit( 1 );
	}

	query_data->attribute_datatype = attribute_datatype;

	query_data->escaped_replaced_data =
		query_data_convert_date_international(
			query_data->attribute_datatype,
			escaped_replaced_data );

	return query_data;
}

char *query_data_convert_date_international(
			char *attribute_datatype,
			char *escaped_replaced_data )
{
	char date_international_string[ 128 ];
	char date_string[ 64 ];
	char time_string[ 64 ];
	char return_date_time[ 128 ];
	enum date_convert_format date_convert_format;
	DATE_CONVERT *date;

	if ( !escaped_replaced_data || !*escaped_replaced_data )
		return escaped_replaced_data;

	if ( strcmp(	attribute_datatype,
			"current_date" ) != 0
	||   strcmp(	attribute_datatype,
			"current_date_time" ) != 0
	||   strcmp(	attribute_datatype,
			"current_time" ) != 0
	||   strcmp(	attribute_datatype,
			"date" ) != 0
	||   strcmp(	attribute_datatype,
			"date_time" ) != 0 )
	{
		return escaped_replaced_data;
	}

	date_convert_format =
		date_convert_date_time_evaluate(
			escaped_replaced_data;

	if ( date_convert_format == date_convert_unknown )
		return escaped_replaced_data;

	*time_string = '\0';

	column( date_string, 0, escaped_replaced_data );
	column( time_string, 1, escaped_replaced_data );

	if ( date_convert_format == american )
	{
		/* Source American */
		/* --------------- */
		date =
			date_convert_new_date_convert( 
				international,
				date_string );
	}
	else
	if ( date_convert_format == military )
	{
		/* Source Military */
		/* --------------- */
		date =
			date_convert_new_date_convert( 
				international,
				date_string );
	}

	if ( *time_string )
	{
		sprintf(return_date_time,
			"%s %s",
			date->return_date,
			time_string );
	}
	else
	{
		strcpy( return_date_time, date->return_date );
	}

	date_convert_free( date );
	return strdup( return_date_time );
}

LIST *query_data_string_list(
			char *data_list_string )
{
	return list_string_list(
			data_list_string,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );
}

LIST *query_drop_down_query_data_list(
			LIST *one2m_primary_attribute_name_list,
			LIST *mto1_foreign_attribute_name_list,
			LIST *one2m_attribute_list,
			LIST *data_string_list )
{
	char *one2m_primary_attribute_name;
	char *mto1_foreign_attribute_name;
	ATTRIBUTE *attribute;
	QUERY_DATA *query_data;
	LIST *query_data_list = {0};
	char *data;

	if ( !list_length( data_string_list ) ) return (LIST *)0;

	if (	list_length( data_string_list ) !=
		list_length( one2m_primary_attribute_name_list ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: data_string_list length = %d != one2m_primary_attribute_name_list length = %d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_length( data_string_list ),
			list_length( one2m_primary_attribute_name_list ) );
		exit( 1 );

	}

	if (	list_length( data_string_list ) !=
		list_length( mto1_foreign_attribute_name_list ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: data_string_list length = %d != mto1_foreign_attribute_name_list length = %d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_length( data_string_list ),
			list_length( mto1_foreign_attribute_name_list ) );
		exit( 1 );

	}

	query_data_list = list_new();

	list_rewind( data_string_list );
	list_rewind( one2m_primary_attribute_name_list );
	list_rewind( mto1_foreign_attribute_name_list );

	do {
		data = list_get( data_string_list );

		one2m_primary_attribute_name =
			list_get(
				one2m_primary_attribute_name_list );

		if ( ! ( attribute =
				attribute_seek_attribute( 
					one2m_primary_attribute_name,
					one2m_attribute_list ) ) )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: cannot seek one2m_primary_attribute_name = (%s).\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
			 	one2m_primary_attribute_name );
			exit( 1 );
		}

		mto1_foreign_attribute_name =
			list_get(
				mto1_foreign_attribute_name_list );

		list_set(
			query_data_list,
			query_data_new(
				mto1_foreign_attribute_name,
				attribute->datatype,
				security_sql_injection_escape(
					security_replace_special_characters(
					     string_trim_number_characters(
						  data,
						  attribute->datatype ) ) ) ) );

		list_next( one2m_primary_attribute_name_list );
		list_next( mto1_foreign_attribute_name_list );

	} while ( list_next( data_string_list ) );

	return query_data_list;
}

QUERY_OUTPUT *query_simple_output_new(
			DICTIONARY *query_dictionary,
			char *login_name,
			FOLDER *mto1_folder,
			LIST *ignore_attribute_name_list,
			PROMPT_RECURSIVE *prompt_recursive )
{
	return query_output;
}

char *query_output_drop_down_where(
			LIST *query_drop_down_list,
			char *mto1_folder_name,
			int mto1_isa_related_folder_list_length )
{
	QUERY_DROP_DOWN *query_drop_down;
	QUERY_DROP_DOWN_ROW *query_drop_down_row;
	QUERY_DATA *query_data;
	LIST *query_drop_down_row_list;
	boolean outter_first_time;
	boolean inner_first_time;
	boolean inner_inner_first_time;
	char where[ QUERY_WHERE_BUFFER ];
	char *ptr = where;

	if ( !list_rewind( query_drop_down_list ) ) return (char *)0;

	*ptr = '\0';

	outter_first_time = 1;

	/* Don't need to prefix the folder_name if no isa relations */
	/* -------------------------------------------------------- */
	if ( !mto1_isa_related_folder_list_length )
		mto1_folder_name = (char *)0;

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

		if ( !list_rewind( query_drop_down_row_list ) ) continue;

		inner_first_time = 1;

		ptr += sprintf( ptr, " (" );

		do {
			query_drop_down_row =
				list_get(
					query_drop_down_row_list );

			if ( inner_first_time )
			{
				inner_first_time = 0;
			}
			else
			{
				ptr += sprintf( ptr, " or (" );
			}

			list_rewind( query_drop_down_row->query_data_list );

			inner_inner_first_time = 1;

			do {
				query_data =
					list_get(
						query_drop_down_row->
							query_data_list );

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
					/* --------------------- */
					/* Returns static memory */
					/* --------------------- */
					query_output_drop_down_data_where(
						mto1_folder_name,
						attribute_name,
						data ) );

			} while( list_next(
					query_drop_down_row->
						query_data_list ) );

			if ( !inner_inner_first_time )
				ptr += sprintf( ptr, " )" );

		} while( list_next( query_drop_down_row_list ) );

		ptr += sprintf( ptr, " )" );

	} while( list_next( query_drop_down_list ) );

	return strdup( where );
}

char *query_output_attribute_where(
			LIST *query_attribute_list )
{
	char where_clause[ QUERY_WHERE_BUFFER ];
	char *ptr = where_clause;
	QUERY_ATTRIBUTE *date_between_attribute = {0};
	QUERY_ATTRIBUTE *time_between_attribute = {0};
	QUERY_ATTRIBUTE *query_attribute;
	LIST *exclude_attribute_name_list = list_new();
	int str_len;
	boolean boolean_attribute;
	boolean first_time = 1;
	char *operator_character_string;
	char *return_where;
	char *application_name = environment_application_name();

	*ptr = '\0';

	if ( !list_length( query_attribute_list ) ) return (char *)0;

	if ( query_attribute_date_time_between(
			&date_between_attribute,
			&time_between_attribute,
			query_attribute_list ) )
	{
		return_where =
			query_attribute_between_date_time_where(
				date_between_attribute->attribute_name,
				time_between_attribute->attribute_name,
				date_between_attribute->from_data,
				time_between_attribute->from_data,
				date_between_attribute->to_data,
				time_between_attribute->to_data,
				date_between_attribute->folder_name );

		if ( return_where && *return_where )
		{
			ptr += sprintf(
				ptr,
				"%s",
				return_where );

			list_set(
				exclude_attribute_name_list,
				date_between_attribute->attribute_name );

			list_set(
				exclude_attribute_name_list,
				time_between_attribute->attribute_name );

			first_time = 0;
		}
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
			( strncmp(
				query_attribute->attribute_name +
				str_len - 3,
				"_yn",
				3 ) == 0 );

		if ( first_time )
			first_time = 0;
		else
			ptr += sprintf( ptr, " and" );

		operator_character_string = 
			appaserver_library_operator_character(
				query_relational_operator_display(
					query_attribute->
						relational_operator ) );

		if ( attribute_is_number( query_attribute->datatype )
		&&   query_attribute->relational_operator == between )
		{
			ptr += sprintf(
				ptr,
				" %s >= %s and %s <= %s",
				appaserver_library_full_attribute_name(
					application_name,
					query_attribute->folder_name,
					query_attribute->attribute_name ),
				query_attribute->
					from_data->
					escaped_replaced_data,
				appaserver_library_full_attribute_name(
					application_name,
					query_attribute->folder_name,
					query_attribute->attribute_name ),
				query_attribute->
					to_data->
					escaped_replaced_data );
		}
		else
		if ( query_attribute->relational_operator == between )
		{
			ptr += sprintf(
				ptr,
				" %s %s '%s' and '%s'",
				appaserver_library_full_attribute_name(
					application_name,
					query_attribute->folder_name,
					query_attribute->attribute_name ),
				operator_character_string,
				query_attribute->
					from_data->
					escaped_replaced_data,
				query_attribute->
					to_data->
					escaped_replaced_data );
		}
		else
		if ( query_attribute->relational_operator == is_null )
		{
			ptr += sprintf(
			   	ptr,
				QUERY_NULL_EXPRESSION,
				appaserver_library_full_attribute_name(
					application_name,
			   		query_attribute->folder_name,
			   		query_attribute->attribute_name ),
				appaserver_library_full_attribute_name(
					application_name,
			   		query_attribute->folder_name,
			   		query_attribute->attribute_name ),
				appaserver_library_full_attribute_name(
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
				appaserver_library_full_attribute_name(
					application_name,
			   		query_attribute->folder_name,
			   		query_attribute->attribute_name ),
				appaserver_library_full_attribute_name(
					application_name,
			   		query_attribute->folder_name,
			   		query_attribute->attribute_name ),
				appaserver_library_full_attribute_name(
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
					query_attribute->
						from_data->
						escaped_replaced_data,
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
					appaserver_library_full_attribute_name(
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
					appaserver_library_full_attribute_name(
						application_name,
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ),
					escape_character(
						buffer,
						query_attribute->
							from_data->
							escaped_replaced_data,
						'_' ),
					'%' );
		}
		else
		if ( query_attribute->relational_operator == contains )
		{
			char buffer[ 4096 ];

			ptr += sprintf( ptr,
					" %s like '%c%s%c'",
					appaserver_library_full_attribute_name(
						application_name,
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ),
					'%',
					escape_character(
						buffer,
						query_attribute->
							from_data->
							escaped_replaced_data,
						'_' ),
					'%' );
		}
		else
		if ( query_attribute->relational_operator == not_contains )
		{
			char buffer[ 4096 ];

			ptr += sprintf( ptr,
					" %s not like '%c%s%c'",
					appaserver_library_full_attribute_name(
						application_name,
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ),
					'%',
					escape_character(
						buffer,
						query_attribute->
							from_data->
							escaped_replaced_data,
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
				appaserver_library_full_attribute_name(
					application_name,
					query_attribute->folder_name,
			   		query_attribute->attribute_name ),
				query_attribute->from_data,
				appaserver_library_full_attribute_name(
					application_name,
					query_attribute->folder_name,
					query_attribute->attribute_name ) );
		}
		else
		if ( attribute_is_number(
			query_attribute->datatype ) )
		{
			ptr +=
				sprintf(
		   		ptr,
			   	" %s %s %s",
				appaserver_library_full_attribute_name(
					application_name,
			   		query_attribute->folder_name,
			   		query_attribute->attribute_name ),
				operator_character_string,
				query_attribute->
					from_data->
					escaped_replaced_data );
		}
		else
		if ( boolean_attribute )
		{
			if ( string_strcmp(
				query_attribute->
					from_data->
					escaped_replaced_data,
				NOT_NULL_OPERATOR ) == 0 )
			{
				ptr +=
					sprintf(
			   		ptr,
			   		" %s is not null",
					appaserver_library_full_attribute_name(
						application_name,
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ) );
			}
			else
			if ( *query_attribute->
				from_data->
				escaped_replaced_data == 'n' )
			{
				char *full_attribute_name;

				full_attribute_name =
					appaserver_library_full_attribute_name(
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
			if ( *query_attribute->
				from_data->
				escaped_replaced_data == 'y' )
			{
				ptr +=
					sprintf(
			   		ptr,
			   		" %s = 'y'",
					appaserver_library_full_attribute_name(
						application_name,
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ) );
			}
			else
			if ( string_strcmp(
					query_attribute->from_data,
					NULL_OPERATOR ) == 0 )
			{
				ptr +=
					sprintf(
			   		ptr,
			   		" %s is null",
					appaserver_library_full_attribute_name(
						application_name,
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ) );
			}
		}
		else
		{
			if ( attribute_is_number(
				query_attribute->datatype ) )
			{
				ptr +=
					sprintf( ptr,
					" %s %s %s",
					appaserver_library_full_attribute_name(
						application_name,
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ),
					operator_character_string,
					query_attribute->
						from_data->
						escaped_replaced_data );
			}
			else
			{
				ptr +=
					sprintf( ptr,
					" %s %s '%s'",
					appaserver_library_full_attribute_name(
						application_name,
			   			query_attribute->folder_name,
			   			query_attribute->
							attribute_name ),
					operator_character_string,
					query_attribute->
						from_data->
						esacaped_replaced_data );
			}
		}

		list_set(
			exclude_attribute_name_list,
			query_attribute->attribute_name );

	} while( list_next( query_attribute_list ) );

	return strdup( where_clause );
}

char *query_output_join_where(
			char *folder_name,
			LIST *primary_attribute_name_list,
			LIST *mto1_isa_related_folder_list )
{
	RELATED_FOLDER *isa_related_folder;
	char where[ QUERY_WHERE_BUFFER ];
	char *ptr = where;

	*ptr = '\0';

	if ( !list_rewind( mto1_isa_related_folder_list ) ) return (char *)0;

	do {
		isa_related_folder = list_get( mto1_isa_related_folder_list );

		if ( !list_length(
			isa_related_folder->
				foreign_attribute_name_list ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: foreign_attribute_name_list is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !*ptr ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			query_output_related_join(
				folder_name,
				primary_attribute_name_list,
				isa_related_folder->
					folder->
					folder_name,
				isa_related_folder->
					foreign_attribute_name_list ) );

	} while( list_next( mto1_isa_related_folder_list ) );

	return strdup( where );
}

char *query_output_where(
			char *query_output_drop_down_where,
			char *query_output_attribute_where,
			char *query_output_join_where,
			char *attribute_not_null_join )
{
	char where[ QUERY_WHERE_BUFFER ];
	char *ptr = where;

	*ptr = '\0';

	if ( string_strlen( query_output_drop_down_where ) )
	{
		ptr += sprintf(
			ptr,
			"%s",
			query_output_drop_down_where );
	}

	if ( string_strlen( query_output_attribute_where ) )
	{
		if ( !*ptr ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			query_output_attribute_where );
	}

	if ( string_strlen( query_output_join_where ) )
	{
		if ( !*ptr ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			query_output_join_where );
	}

	if ( string_strlen( attribute_not_null_join ) )
	{
		if ( !*ptr ) ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			"%s",
			attribute_not_null_join );
	}

	return strdup( where );
}

LIST *query_output_select_name_list(
			LIST *append_isa_attribute_list,
			LIST *ignore_attribute_name_list,
			LIST *lookup_attribute_exclude_name_list )
{
	char *attribute_name;
	ATTRIBUTE *attribute;
	LIST *select_name_list;
	char *application_name = environment_application_name();

	if ( !list_rewind( append_isa_attribute_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: append_isa_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	select_name_list = list_new();

	do {
		attribute = list_get( append_isa_attribute_list );

		if ( !*ptr ) ptr += sprintf( ptr, "," );

		if ( !attribute->folder_name )
		{
			fprintf(stderr,
				"ERROR in %s/%s()/%d: folder_name is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( list_exists_string(
			attribute->attribute_name,
			ignore_attribute_name_list )
		||   list_exists_string(
			attribute->attribute_name,
			lookup_attribute_exclude_name_list ) )
		{
			continue;
		}

		sprintf(attribute_display,
			"%s.%s",
			get_table_name(
				application_name,
				attribute->folder_name ),
			attribute->attribute_name );

		if ( attribute_is_date_time( attribute->datatype )
		&&   attribute->width == 16 )
		{
			ptr += sprintf( ptr,
					"substr(%s,1,16)",
					attribute_display,
		}
		else
		{
			ptr += sprintf( ptr,
					"%s",
					attribute_display );
		}

	} while( list_next( append_isa_attribute_list ) );

	return strdup( select );
}

char *query_output_from(
			char *mto1_folder_name,
			LIST *mto1_isa_related_folder_list,
			char *attribute_not_null_folder_name )
{
	LIST *from_list;

	from_list = list_new();

	list_set( from_list, mto1_folder_name );

	list_set_list(
		from_list,
		related_folder_mto1_folder_name_list(
			mto1_isa_related_folder_list ) );

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

char *query_attribute_name_list_order(
			char *folder_name,
			LIST *attribute_name_list,
			LIST *append_isa_attribute_list,
			char *descending_clause )
{
	char order[ 1024 ];
	char *ptr = order;
	char *attribute_name;
	char *table_name =
		get_table_name(
			environment_application_name(),
			folder_name );

	if ( !folder_name || !*folder_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( attribute_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}
	
	*ptr = '\0';

	do {
		attribute_name = list_get( attribute_name_list );

		if ( !attribute_exists(
			attribute_name,
			append_isa_attribute_list ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: attribute_exists(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				attribute_name );

			exit( 1 );
		}

		if ( !*ptr ) ptr += sprintf( ptr, "," );

		ptr += sprintf(	ptr,
				"%s.%s%s",
				table_name,
				attribute_name,
				descending_label );

	} while ( list_next( attribute_name_list ) );

	return stdup( order );
}

char *query_sort_prefix_direction_attribute_index(
			DICTIONARY *sort_dictionary )
{
	LIST *key_list;

	key_list = dictionary_key_list( sort_dictionary );

	if ( !list_rewind( key_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: dictionary_key_list() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	/* List of one */
	/* ----------- */
	return list_get( key_list );
}

char *query_output_order(
			char *mto1_folder_name,
			LIST *primary_attribute_name_list,
			LIST *append_isa_attribute_list,
			DICTIONARY *sort_dictionary )
{
	char *prefix_direction_attribute_index;
	char prefix_direction_attribute[ 512 ];
	char direction_attribute[ 512 ];
	char attribute_list_string[ 512 ];
	char *descending_label = "";

	if ( !dictionary_length( sort_dictionary ) )
	{
		return query_attribute_name_list_order(
			mto1_folder_name,
			primary_attribute_name_list,
			append_isa_attribute_list,
			descending_label );
	}

	prefix_direction_attribute_index =
		/* ------------------------------------------------------- */
		/* Sample: ssort_button_descend_full_name^street_address_0 */
		/* ------------------------------------------------------- */
		query_sort_prefix_direction_attribute_index(
			sort_dictionary );

	trim_index(
		prefix_direction_attribute,
		prefix_direction_attribute_index );

	string_strcpy(
		direction_attribute,
		prefix_direction_attribute + strlen( SORT_BUTTON_PREFIX ) );

	if ( string_strncmp(
		direction_attribute,
		FORM_DESCENDING_LABEL ) == 0 )
	{
		string_strcpy(
			attribute_list_string,
			direction_attribute +
			strlen( FORM_DESCENDING_LABEL ) );

		descending_label = " desc";
	}
	else
	{
		string_strcpy(
			attribute_list_string,
			direction_attribute +
			strlen( FORM_SORT_LABEL ) );
	}

	return query_attribute_name_list_order(
			mto1_folder_name,
			list_string_list(
				attribute_list_string,
				FOLDER_DATA_DELIMITER )
					/* attribute_name_list */,
			append_isa_attribute_list,
			descending_label );
}

char *query_output_select_display(
			char *mto1_folder_name,
			LIST *query_output_select_name_list,
			int mto1_isa_related_folder_list_length )
{
	char *select_name;
	char display[ QUERY_WHERE_BUFFER ];
	char *ptr = display;

	if ( !list_rewind( query_output_select_name_list ) )
		return (char *)0;

	*ptr = '\0';

	do {
		select_name =
			list_get(
				query_output_select_name_list );

		if ( mto1_isa_related_folder_list_length )
		{
			ptr += sprintf( ptr, "%s.", mto1_folder_name );
		}

		if ( *ptr ) ptr += sprintf( ptr, "," );

		ptr += sprintf( ptr, "%s", select_name );

	} while ( list_next( query_output_select_name_list ) );

	return display;
}

LIST *query_system_dictionary_list(
			char *system_string,
			LIST *select_name_list,
			QUERY_DATE_CONVERT *query_date_convert )
{
	char buffer[ QUERY_WHERE_BUFFER ];
	char data[ 65536 ];
	LIST *list = list_new();
	DICTIONARY *dictionary;
	char *attribute_name;
	int i;
	FILE *pipe;
	DATE_CONVERT *date_convert = {0};

	if ( !list_length( select_name_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d select_name_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( query_date_convert )
	{
		date_convert = date_convert_calloc();

		date_convert->source_format =
			query_date_convert->
				database_date_format;

		date_convert->destination_format =
			query_date_convert->
				user_date_format;
	}

	pipe = popen( sys_string, "r" );

	while( string_input_pipe( buffer, pipe, QUERY_WHERE_BUFFER ) )
	{
		dictionary = dictionary_small_new();
		list_set( list, dictionary );
		i = 0;

		list_rewind( select_name_list );

		do {
			if ( !piece( data, SQL_DELIMITER, buffer, i++ ) )
			{
				fprintf(stderr,
			"ERROR in %s/%s()/%d: piece(%d) returned empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__,
					i - 1 );

				pclose( pipe );
				exit( 1 );
			}

			attribute_name = list_get( select_name_list );

			if ( date_convert
			&&   list_exists_string(
				attribute_name,
				query_date_convert->date_attribute_name_list ) )
			{
				date_convert_evaluate( date_convert, data );

				if ( *date_convert->return_date )
				{
					strcpy( data,
						date_convert->return_date );
				}
				else
				{
					*data = '\0';
				}
			}

			dictionary_set_pointer(
				dictionary, 
				attribute_name, 
				strdup( data ) );

		} while ( list_next( select_name_list ) );
	}

	pclose( pipe );
	return list;
}

QUERY_ENTITY *query_entity_calloc( void )
{
	QUERY_ENTITY *query_entity;

	if ( ! ( query_entity =
			calloc( 1, sizeof( QUERY_ENTITY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty..\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return query_entity;
}

QUERY_ENTITY *query_entity_new(
			char *login_name,
			boolean non_owner_forbid,
			boolean override_row_restrictions )
{
	QUERY_ENTITY *query_entity = query_entity_calloc();

	query_entity->login_name = login_name;
	query_entity->non_owner_forbid = non_owner_forbid;
	query_entity->override_row_restrictions = login_name;

	if ( non_owner_forbid && !override_row_restrictions )
	{
		query_entity->login_name_only = login_name;

		query_entity->full_name =
			appaserver_entity_fetch(
				&query_entity->street_address,
				query_entity->login_name );
	}

	return query_entity;
}

QUERY_FOLDER *query_folder_calloc( void )
{
	QUERY_FOLDER *query_folder;

	if ( ! ( query_entity_only =
			calloc( 1, sizeof( QUERY_FOLDER ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty..\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return query_folder;
}

QUERY_FOLDER *query_folder_new( char *folder_name )
{
	QUERY_FOLDER *entity_folder = query_folder_calloc();

	query_folder->folder_name = folder_name;

	query_folder->non_owner_forbid =
		folder_row_level_restrictions_non_owner_forbid(
			query_folder->folder_name );

	returen query_folder;
}

#ifdef NOT_DEFINED
QUERY_OUTPUT *query_edit_table_output_new(
			DICTIONARY *query_dictionary,
			FOLDER *mto1_folder,
			LIST *ignore_attribute_name_list,
			PROMPT_RECURSIVE *prompt_recursive,
			char *attribute_not_null_join,
			char *attribute_not_null_folder_name,
			DICTIONARY *sort_dictionary )
{
	QUERY_OUTPUT *query_output;
	LIST *exclude_attribute_name_list;

	if ( !mto1_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: mto1_folder is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	exclude_attribute_name_list = list_new();
	query_output = query_output_calloc();

	query_output->
		query_drop_down_list =
			query_drop_down_list(
				exclude_attribute_name_list,
				mto1_folder,
				query_dictionary );

	if ( prompt_recursive
	&&   list_length(
		prompt_recursive->
			prompt_recursive_folder_list ) )
	{
		LIST *prompt_recursive_drop_down_list;

		prompt_recursive_drop_down_list =
			query_prompt_recursive_drop_down_list(
				exclude_attribute_name_list,
				folder->folder_name
					/* root_folder_name */,
				prompt_recursive->
					prompt_recursive_folder_list,
				query_dictionary );

		if ( list_length( prompt_recursive_drop_down_list ) )
		{
			if ( !query_output->query_drop_down_list )
			{
				query_output->query_drop_down_list =
					list_new();
			}

			list_set_list(
				query_output->query_drop_down_list,
				prompt_recursive_drop_down_list );
		}
	}

	query_output->query_attribute_list =
		query_attribute_list(
			folder->append_isa_attribute_list,
			query_dictionary,
			exclude_attribute_name_list );

	query_output->non_joined_where_clause =
	query_output->where_clause =
	query_edit_table_where(
		&query_output->drop_down_where_clause,
		&query_output->attribute_where_clause,
		query_output->query_drop_down_list,
		query_output->query_attribute_list,
		folder->application_name,
		root_folder_name,
		0 /* not combine_date_time */ );

	if ( !query_output->where_clause
	||   !*query_output->where_clause )
	{
		query_output->non_joined_where_clause =
		query_output->where_clause = "1 = 1";
	}

	query_output->select_clause =
		query_output_select_clause(
			folder->append_isa_attribute_list,
			folder->ignore_attribute_name_list,
			folder->lookup_attribute_exclude_name_list );

	query_output->from_clause =
		query_output_from_clause(
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

	if ( attribute_not_null_join
	&&   *attribute_not_null_join
	&&   attribute_not_null_folder_name
	&&   *attribute_not_null_folder_name
	&&   strcmp( attribute_not_null_folder_name, first_folder_name ) != 0 )
	{
		char where[ QUERY_WHERE_BUFFER ];

		sprintf(where,
			"%s and %s",
			query_output->where_clause,
			attribute_not_null_join );

		query_output->where_clause = strdup( where );
	}

	return query_output;
}
#endif

