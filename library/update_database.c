/* $APPASERVER_HOME/library/update_database.c				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver update_database ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

/* -------------------------------------------------------------------- */
/* Note: need to test copy_common_attributes to make sure they don't	*/
/* update when they're not supposed to.					*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "list.h"
#include "dictionary.h"
#include "process.h"
#include "update_database.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "related_folder.h"
#include "folder.h"
#include "appaserver_library.h"
#include "appaserver_user.h"
#include "document.h"
#include "appaserver_parameter_file.h"

UPDATE_DATABASE *update_database_new(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *folder_name,
			DICTIONARY *row_dictionary,
			DICTIONARY *file_dictionary )
{
	UPDATE_DATABASE *update_database;

	if ( ! ( update_database =
			(UPDATE_DATABASE *)
				calloc( 1, sizeof( UPDATE_DATABASE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	update_database->application_name = application_name;
	update_database->session = session;
	update_database->login_name = login_name;
	update_database->role_name = role_name;
	update_database->row_dictionary = dictionary_copy( row_dictionary );
	update_database->file_dictionary = file_dictionary;

	update_database->foreign_attribute_dictionary = dictionary_small_new();

	update_database->folder =
		folder_with_load_new( 	application_name,
					session, 
					folder_name,
					role_new_role(
						application_name,
						role_name ) );

	dictionary_set_indexed_date_time_to_current(
		update_database->row_dictionary,
		update_database->folder->attribute_list );

	if ( update_database->folder->row_level_non_owner_forbid )
	{
		update_database_set_login_name_each_row(
				row_dictionary,
				login_name );

		update_database_set_login_name_each_row(
				file_dictionary,
				login_name );
	}

	update_database->exclude_attribute_name_list =
		attribute_get_update_lookup_exclude_attribute_name_list(
			update_database->folder->attribute_list );

	appaserver_library_post_dictionary_database_convert_dates(
			file_dictionary,
			application_name,
			update_database->folder->attribute_list );

	dictionary_remove_symbols_in_numbers(
			file_dictionary,
			update_database->folder->attribute_list );

	if ( list_length( update_database->
				folder->
				one2m_recursive_related_folder_list ) )
	{
		related_folder_list_populate_one2m_foreign_attribute_dictionary(
				update_database->
					foreign_attribute_dictionary,
				list_get_last_pointer(
					update_database->
						folder->
						primary_attribute_name_list ),
				update_database->
					folder->
					one2m_recursive_related_folder_list );
	}

	if ( list_length( update_database->
				folder->
				mto1_isa_related_folder_list ) )
	{
	     related_folder_list_populate_mto1_isa_foreign_attribute_dictionary(
			update_database->
				foreign_attribute_dictionary,
			list_get_last_pointer(
				update_database->
					folder->
					primary_attribute_name_list ),
			update_database->
				folder->
				mto1_isa_related_folder_list,
			application_name );
	}

	return update_database;

} /* update_database_new() */

WHERE_ATTRIBUTE *update_database_where_attribute_new(
			char *where_attribute_name,
			char *data )
{
	WHERE_ATTRIBUTE *where_attribute;

	if ( ! ( where_attribute =
			(WHERE_ATTRIBUTE *)
				calloc( 1, sizeof( WHERE_ATTRIBUTE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	where_attribute->attribute_name = where_attribute_name;
	where_attribute->data = data;

	return where_attribute;

} /* update_database_where_attribute_new() */

CHANGED_ATTRIBUTE *update_database_changed_attribute_new(
			char *folder_name,
			char *attribute_name,
			char *attribute_datatype,
			char *old_data,
			char *new_data )
{
	CHANGED_ATTRIBUTE *changed_attribute;

	changed_attribute =
		(CHANGED_ATTRIBUTE *)
			calloc( 1, sizeof( CHANGED_ATTRIBUTE ) );

	changed_attribute->folder_name = folder_name;
	changed_attribute->attribute_name = attribute_name;
	changed_attribute->old_data = old_data;

	if ( timlib_strcmp( attribute_datatype, "float" ) == 0 )
	{
		strcpy(	new_data,
			timlib_trim_money_characters( new_data ) );
	}

	if ( strcmp( new_data, FORBIDDEN_NULL ) == 0 )
		changed_attribute->new_data = NULL_STRING;
	else
		changed_attribute->new_data = new_data;

	return changed_attribute;

} /* update_database_changed_attribute_new() */

boolean update_database_get_index_data_if_changed(
					char **old_data,
					char **new_data,
					DICTIONARY *row_dictionary,
					DICTIONARY *file_dictionary,
					char *attribute_name,
					int row )
{
	char preupdate_attribute_name[ 128 ];

	if ( !row_dictionary )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: no row_dictionary\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !file_dictionary )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: no file_dictionary\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	*old_data = (char *)0;
	*new_data = (char *)0;

	if ( !update_database_get_dictionary_index_data(
						new_data,
						row_dictionary,
						attribute_name,
						row ) )
	{
		return 0;
	}

	if ( !update_database_get_dictionary_index_data(
					old_data,
					file_dictionary,
					attribute_name,
					row ) )
	{
		return 0;
	}

	if ( strcmp( *old_data, *new_data ) == 0 )
	{
		return 0;
	}
	else
	if ( strcmp( *old_data, "select" ) == 0 && !**new_data )
	{
		return 0;
	}
	else
	{
		sprintf(	preupdate_attribute_name,
				"%s%s_%d",
				UPDATE_DATABASE_PREUPDATE_PREFIX,
				attribute_name,
				row );

		dictionary_set_pointer(
			row_dictionary,
			strdup( preupdate_attribute_name ),
			*old_data );

		return 1;
	}
} /* update_database_get_index_data_if_changed() */

UPDATE_ROW *update_database_update_row_new( int row )
{
	UPDATE_ROW *update_row;

	update_row =
		(UPDATE_ROW *)
			calloc( 1, sizeof( UPDATE_ROW ) );

	if ( !update_row )
	{
		fprintf( stderr,
			 "Memory allocation error in %s/%s()/%d\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	update_row->row = row;
	update_row->update_folder_list = list_new_list();

	return update_row;

} /* update_database_update_row_new() */

UPDATE_FOLDER *update_database_update_folder_new(
			char *folder_name )
{
	UPDATE_FOLDER *update_folder;

	if ( ! ( update_folder =
			(UPDATE_FOLDER *)
				calloc( 1, sizeof( UPDATE_FOLDER ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	update_folder->folder_name = folder_name;
	return update_folder;

} /* update_database_update_folder_new() */

LIST *update_database_update_row_list(
			DICTIONARY *row_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER *folder,
			LIST *exclude_attribute_name_list,
			DICTIONARY *foreign_attribute_dictionary )
{
	LIST *update_row_list = list_new_list();
	int row;
	int highest_index;
	LIST *mto1_isa_related_folder_list;
	RELATED_FOLDER *related_folder;
	UPDATE_ROW *update_row = {0};

	if ( !row_dictionary ) return update_row_list;

	mto1_isa_related_folder_list =
		folder->mto1_isa_related_folder_list;

	highest_index = dictionary_get_key_highest_index( row_dictionary );

	for( row = 1; row <= highest_index; row++ )
	{
		update_row =
			update_database_get_update_row(
				row,
				row_dictionary,
				file_dictionary,
				folder,
				exclude_attribute_name_list,
				foreign_attribute_dictionary );

		if ( update_row )
		{
			list_append_pointer( update_row_list, update_row );
		}

		if ( list_rewind( mto1_isa_related_folder_list ) )
		{
			do {
				related_folder =
					list_get(
						mto1_isa_related_folder_list );

				update_row =
					update_database_get_update_row(
					row,
					row_dictionary,
					file_dictionary,
					related_folder->folder,
					exclude_attribute_name_list,
					foreign_attribute_dictionary );

				if ( update_row )
				{
					list_append_pointer(
						update_row_list,
						update_row );
				}

			} while( list_next( mto1_isa_related_folder_list ) );
		}
	}

	return update_row_list;

} /* update_database_update_row_list() */

UPDATE_ROW *update_database_get_update_row(
			int row,
			DICTIONARY *row_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER *folder,
			LIST *exclude_attribute_name_list,
			DICTIONARY *foreign_attribute_dictionary )
{
	UPDATE_ROW *update_row;
	UPDATE_FOLDER *update_folder;
	boolean changed_key = 0;
	LIST *include_attribute_name_list;
	LIST *additional_unique_index_attribute_name_list;

	additional_unique_index_attribute_name_list =
		attribute_get_additional_unique_index_attribute_name_list(
			folder->attribute_list );

	include_attribute_name_list =
		attribute_get_attribute_name_list(
			folder->attribute_list );

	update_folder =
		update_database_get_update_folder(
			&changed_key,
			row,
			row_dictionary,
			file_dictionary,
			folder,
			(LIST *)0 /* foreign_attribute_name_list */,
			include_attribute_name_list,
			exclude_attribute_name_list,
			foreign_attribute_dictionary,
			additional_unique_index_attribute_name_list );

	if ( !update_folder ) return (UPDATE_ROW *)0;

	update_row = update_database_update_row_new( row );
	update_row->update_folder_list = list_new();
	list_append_pointer( update_row->update_folder_list, update_folder );
	update_row->changed_key = changed_key;

	if ( update_row->changed_key
	&&   list_length( folder->one2m_recursive_related_folder_list ) )
	{
		update_database_set_one2m_related_folder_list(
			update_row->update_folder_list,
			folder->one2m_recursive_related_folder_list,
			row,
			row_dictionary,
			file_dictionary,
			exclude_attribute_name_list,
			foreign_attribute_dictionary );
	}

	return update_row;

} /* update_database_get_update_row() */

UPDATE_FOLDER *update_database_get_folder_foreign_update_folder(
			int row,
			DICTIONARY *row_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER *folder,
			LIST *primary_attribute_name_list,
			LIST *folder_foreign_attribute_name_list )
{
	UPDATE_FOLDER *update_folder;
	LIST *changed_attribute_list;

	changed_attribute_list =
		update_database_get_folder_foreign_changed_attribute_list(
			row_dictionary,
			file_dictionary,
			row,
			folder->folder_name,
			primary_attribute_name_list,
			folder_foreign_attribute_name_list );

	if ( !changed_attribute_list ) return (UPDATE_FOLDER *)0;

	update_folder =
		update_database_update_folder_new(
			folder->folder_name );

	update_folder->changed_attribute_list =
		changed_attribute_list;

	update_folder->post_change_process = folder->post_change_process;

	update_folder->primary_attribute_name_list =
		folder->primary_attribute_name_list;

	update_folder->where_attribute_list =
		update_database_get_folder_foreign_where_attribute_list(
			file_dictionary,
			primary_attribute_name_list,
			row,
			folder->application_name,
			folder->folder_name,
			folder_foreign_attribute_name_list );

	return update_folder;

} /* update_database_get_folder_foreign_update_folder() */

UPDATE_FOLDER *update_database_get_update_folder(
			boolean *changed_key,
			int row,
			DICTIONARY *row_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER *folder,
			LIST *foreign_attribute_name_list,
			LIST *include_attribute_name_list,
			LIST *exclude_attribute_name_list,
			DICTIONARY *foreign_attribute_dictionary,
			LIST *additional_unique_index_attribute_name_list )
{
	UPDATE_FOLDER *update_folder;
	LIST *changed_attribute_list;
	LIST *where_attribute_name_list;

	changed_attribute_list =
		update_database_get_changed_attribute_list(
			changed_key,
			row_dictionary,
			file_dictionary,
			folder->folder_name,
			folder->attribute_list,
			row,
			foreign_attribute_dictionary,
			include_attribute_name_list,
			exclude_attribute_name_list,
			additional_unique_index_attribute_name_list );

	if ( !changed_attribute_list ) return (UPDATE_FOLDER *)0;

	update_folder =
		update_database_update_folder_new(
			folder->folder_name );

	update_folder->changed_attribute_list =
		changed_attribute_list;

	update_folder->post_change_process = folder->post_change_process;

	update_folder->primary_attribute_name_list =
		folder->primary_attribute_name_list;

	if ( list_length( foreign_attribute_name_list ) )
		where_attribute_name_list =
			foreign_attribute_name_list;
	else
		where_attribute_name_list =
			update_folder->primary_attribute_name_list;

	update_folder->where_attribute_list =
		update_database_where_attribute_list(
			file_dictionary,
			where_attribute_name_list,
			row,
			foreign_attribute_dictionary );

	return update_folder;

} /* update_database_get_update_folder() */

LIST *update_database_get_changed_attribute_name_list(
			LIST *changed_attribute_list )
{
	LIST *changed_attribute_name_list;
	CHANGED_ATTRIBUTE *changed_attribute;

	if ( !list_rewind( changed_attribute_list ) ) return (LIST *)0;

	changed_attribute_name_list = list_new();

	do {
		changed_attribute = list_get( changed_attribute_list );

		list_append_pointer(	changed_attribute_name_list,
					changed_attribute->attribute_name );

	} while( list_next( changed_attribute_list ) );

	return changed_attribute_name_list;

} /* update_database_get_changed_attribute_name_list() */

LIST *update_database_get_folder_foreign_where_attribute_list(
			DICTIONARY *file_dictionary,
			LIST *primary_attribute_name_list,
			int row,
			char *application_name,
			char *folder_name,
			LIST *folder_foreign_attribute_name_list )
{
	char *where_attribute_name;
	char *file_attribute_name;
	char *data;
	LIST *where_attribute_list;
	WHERE_ATTRIBUTE *where_attribute;

	if ( !file_dictionary )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: no file_dictionary\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( list_length( primary_attribute_name_list ) !=
	     list_length( folder_foreign_attribute_name_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: lists are not the same: %d vs. %d\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 list_length( primary_attribute_name_list ),
			 list_length( folder_foreign_attribute_name_list ) );

		exit( 1 );
	}

	if ( !list_rewind( primary_attribute_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: no primary_attribute_name_list\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list_rewind( folder_foreign_attribute_name_list );

	where_attribute_list = list_new_list();

	do {
		file_attribute_name =
			list_get_pointer(
				primary_attribute_name_list );

		where_attribute_name =
			list_get_pointer(
				folder_foreign_attribute_name_list );

		data = (char *)0;

		dictionary_get_index_data(
			&data,
			file_dictionary,
			file_attribute_name,
			row );

		if ( !data || !*data )
		{
			char message[ 1024 ];

			sprintf( message,
"ERROR in %s/%s()/%d: with folder_name = (%s), cannot find data for file_attribute_name = (%s) in file_dictionary = (%s).",
				 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__,
				folder_name,
				file_attribute_name,
				dictionary_display(
				       file_dictionary ) );

			appaserver_output_error_message(
				application_name,
				message,
				(char *)0 /* login_name */ );

			exit( 0 );
		}

		where_attribute =
			update_database_where_attribute_new(
				where_attribute_name,
				data );

		list_append_pointer(
			where_attribute_list,
			where_attribute );

		list_next( primary_attribute_name_list );

	} while( list_next( folder_foreign_attribute_name_list ) );

	return where_attribute_list;

} /* update_database_get_folder_foreign_where_attribute_list() */

LIST *update_database_where_attribute_list(
			DICTIONARY *file_dictionary,
			LIST *where_attribute_name_list,
			int row,
			DICTIONARY *foreign_attribute_dictionary )
{
	char *where_attribute_name;
	char *data;
	LIST *where_attribute_list;
	WHERE_ATTRIBUTE *where_attribute;
	char *alternative_attribute_name;

	if ( !file_dictionary )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: no file_dictionary\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( where_attribute_name_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: no where_attribute_name_list\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	where_attribute_list = list_new_list();

	do {
		where_attribute_name =
			list_get_pointer(
				where_attribute_name_list );

		if ( !*where_attribute_name )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: empty where_attribute_name.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		data = (char *)0;

		/* First, check for the last foreign_attribute_name */
		/* ------------------------------------------------ */
		if ( ( alternative_attribute_name =
			dictionary_get_data(
				foreign_attribute_dictionary,
				where_attribute_name ) )
		&&     *alternative_attribute_name
		&&     strcmp(	alternative_attribute_name,
				NULL_STRING ) != 0 )
		{
			dictionary_get_index_data(
				&data,
				file_dictionary,
				alternative_attribute_name,
				row );
		}

		if ( !data || !*data )
		{
			dictionary_get_index_data(
					&data,
					file_dictionary,
					where_attribute_name,
					row );
		}

		if ( !data || !*data ) continue;

		where_attribute =
			update_database_where_attribute_new(
				where_attribute_name,
				data );

		list_append_pointer(
			where_attribute_list,
			where_attribute );

	} while( list_next( where_attribute_name_list ) );

	return where_attribute_list;
}

LIST *update_database_get_folder_foreign_changed_attribute_list(
			DICTIONARY *row_dictionary,
			DICTIONARY *file_dictionary,
			int row,
			char *folder_name,
			LIST *primary_attribute_name_list,
			LIST *folder_foreign_attribute_name_list )
{
	LIST *changed_attribute_list = {0};
	CHANGED_ATTRIBUTE *changed_attribute;
	char *primary_attribute_name;
	char *folder_foreign_attribute_name;
	char *old_data;
	char *new_data;

	if ( !row_dictionary ) return (LIST *)0;

	if ( list_length( primary_attribute_name_list ) !=
	     list_length( folder_foreign_attribute_name_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: lists are not the same: %d vs. %d\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 list_length( primary_attribute_name_list ),
			 list_length( folder_foreign_attribute_name_list ) );

		exit( 1 );
	}

	if ( !list_rewind( primary_attribute_name_list ) ) return (LIST *)0;

	list_rewind( folder_foreign_attribute_name_list );

	do {
		primary_attribute_name =
			list_get_pointer(
				primary_attribute_name_list );

		folder_foreign_attribute_name =
			list_get_pointer(
				folder_foreign_attribute_name_list );

		old_data = (char *)0;
		new_data = (char *)0;

		if ( update_database_get_index_data_if_changed(
					&old_data,
					&new_data,
					row_dictionary,
					file_dictionary,
					primary_attribute_name,
					row ) )
		{
			if ( !changed_attribute_list )
			{
				changed_attribute_list = list_new_list();
			}

			changed_attribute =
				update_database_changed_attribute_new(
					folder_name,
					folder_foreign_attribute_name,
					(char *)0 /* datatype */,
					old_data,
					new_data );

			if ( changed_attribute )
			{
				list_append_pointer(
					changed_attribute_list,
					changed_attribute );
			}

		}

		list_next( folder_foreign_attribute_name_list );

	} while( list_next( primary_attribute_name_list ) );

	return changed_attribute_list;

} /* update_database_get_folder_foreign_changed_attribute_list() */

LIST *update_database_get_changed_attribute_list(
			boolean *changed_key,
			DICTIONARY *row_dictionary,
			DICTIONARY *file_dictionary,
			char *folder_name,
			LIST *attribute_list,
			int row,
			DICTIONARY *foreign_attribute_dictionary,
			LIST *include_attribute_name_list,
			LIST *exclude_attribute_name_list,
			LIST *additional_unique_index_attribute_name_list )
{
	ATTRIBUTE *attribute;
	LIST *changed_attribute_list = {0};
	CHANGED_ATTRIBUTE *changed_attribute;
	char *old_data;
	char *new_data;
	char *alternative_attribute_name;

	if ( !row_dictionary ) return (LIST *)0;

	if ( !list_rewind( attribute_list ) ) return (LIST *)0;

	do {
		attribute = list_get_pointer( attribute_list );

		if ( !list_exists_string(	include_attribute_name_list,
						attribute->attribute_name ) )
		{
			continue;
		}

		if ( list_exists_string(	exclude_attribute_name_list,
						attribute->attribute_name ) )
		{
			continue;
		}

		old_data = (char *)0;
		new_data = (char *)0;

		if ( update_database_get_index_data_if_changed(
					&old_data,
					&new_data,
					row_dictionary,
					file_dictionary,
					attribute->attribute_name,
					row ) )
		{
			if ( timlib_strcmp( new_data, NULL_OPERATOR ) == 0
			&&   list_exists_string(
				additional_unique_index_attribute_name_list,
				attribute->attribute_name ) )
			{
				return (LIST *)0;
			}

			if ( !changed_attribute_list )
			{
				changed_attribute_list = list_new_list();
			}

			changed_attribute =
				update_database_changed_attribute_new(
					folder_name,
					attribute->attribute_name,
					attribute->datatype,
					old_data,
					new_data );

			if ( changed_attribute )
			{
				list_append_pointer(
					changed_attribute_list,
					changed_attribute );
			}

			if ( attribute->primary_key_index ) *changed_key = 1;
		}
		else
		if ( foreign_attribute_dictionary )
		{
			if ( ( alternative_attribute_name =
				dictionary_get_data(
					foreign_attribute_dictionary,
					attribute->attribute_name ) ) )
			{
				old_data = (char *)0;
				new_data = (char *)0;

				if ( update_database_get_index_data_if_changed(
					&old_data,
					&new_data,
					row_dictionary,
					file_dictionary,
					alternative_attribute_name,
					row ) )
				{
					if ( !changed_attribute_list )
					{
						changed_attribute_list =
							list_new_list();
					}

					changed_attribute =
					update_database_changed_attribute_new(
						attribute->folder_name,
						attribute->attribute_name,
						attribute->datatype,
						old_data,
						new_data );

					list_append_pointer(
						changed_attribute_list,
						changed_attribute );

					if ( attribute->primary_key_index )
						*changed_key = 1;
				}
			}
		}
	} while( list_next( attribute_list ) );

	return changed_attribute_list;

} /* update_database_get_changed_attribute_list() */

char *update_database_execute(	char *application_name,
				char *session,
				LIST *update_row_list,
				DICTIONARY *row_dictionary,
				char *login_name,
				char *role_name,
				LIST *additional_update_attribute_name_list,
				LIST *additional_update_data_list,
				boolean abort_if_first_update_failed )
{
	UPDATE_ROW *update_row;
	static char error_messages[ 4096 ];

	if ( !list_rewind( update_row_list ) ) return "";

	*error_messages = '\0';

	do {
		update_row = list_get_pointer( update_row_list );

		update_database_execute_for_row(
				error_messages,
			   	application_name,
				session,
				row_dictionary,
				login_name,
			   	update_row->update_folder_list,
				update_row->row,
				role_name,
				additional_update_attribute_name_list,
				additional_update_data_list,
				abort_if_first_update_failed );

	} while( list_next( update_row_list ) );

	return error_messages;

} /* update_database_execute() */

void update_database_execute_for_row(
			char *error_messages,
			char *application_name,
			char *session,
			DICTIONARY *row_dictionary,
			char *login_name,
			LIST *update_folder_list,
			int row,
			char *role_name,
			LIST *additional_update_attribute_name_list,
			LIST *additional_update_data_list,
			boolean abort_if_first_update_failed )
{
	UPDATE_FOLDER *update_folder;
	char *error_message_string;
	int first_time = 1;

	if ( !list_rewind( update_folder_list ) ) return;

	do {
		update_folder = list_get_pointer( update_folder_list );

		if ( ( error_message_string =
			update_database_execute_for_folder(
			   	application_name,
				login_name,
			   	update_folder->
					folder_name,
				update_folder->
					where_attribute_list,
				update_folder->
					changed_attribute_list,
				update_folder->
					primary_attribute_name_list,
				additional_update_attribute_name_list,
				additional_update_data_list ) ) )
		{
			update_folder->update_failed = 1;

			sprintf( error_messages +
				 strlen( error_messages ),
				 "<p>%s:%s",
				 error_message_string,
				 update_folder->folder_name );

			if ( abort_if_first_update_failed
			&&   first_time )
			{
				return;
			}
		}

		if ( first_time ) first_time = 0;

	} while( list_next( update_folder_list ) );

	/* Execute the post change processes */
	/* --------------------------------- */
	list_rewind( update_folder_list );

	do {
		update_folder = list_get_pointer( update_folder_list );

		if ( update_folder->update_failed ) continue;

		if ( update_folder->post_change_process )
		{
			/* Reload the command_line each row */
			/* -------------------------------- */
			free( update_folder->post_change_process->executable );

			process_load_executable(
				&update_folder->post_change_process->
					executable,
				update_folder->post_change_process->
					process_name,
				application_name );

			process_convert_parameters(
				&update_folder->
					post_change_process->executable,
				application_name,
				session,
				"update" /* state */,
				login_name,
				update_folder->folder_name,
				role_name,
				(char *)0 /* target_frame */,
				row_dictionary /* parameter_dictionary */,
				row_dictionary /* where_clause_dictionary */,
				update_folder->
					post_change_process->attribute_list,
				(LIST *)0 /* prompt_list */,
				(LIST *)0 /* primary_attribute_name_list */,
				(LIST *)0 /* primary_data_list */,
				row,
				(char *)0 /* process_name */,
				(PROCESS_SET *)0,
				(char *)0
				/* one2m_folder_name_for_processes */,
				(char *)0 /* operation_row_count_string */,
				(char *)0 /* prompt */ );

			error_message_string =
				pipe2string( update_folder->
						post_change_process->
						executable );

			if ( error_message_string && *error_message_string )
			{
				sprintf(error_messages +
				 	strlen( error_messages ),
				 	"%s",
				 	error_message_string );
			}

		}
	} while( list_next( update_folder_list ) );

} /* update_database_execute_for_row() */

char *update_database_execute_for_folder(
			char *application_name,
			char *login_name,
			char *folder_name,
			LIST *where_attribute_list,
			LIST *changed_attribute_list,
			LIST *primary_attribute_name_list,
			LIST *additional_update_attribute_name_list,
			LIST *additional_update_data_list )
{
	char *table_name;
	char sys_string[ 8192 ];
	char update_clause[ 8192 ];
	char where_clause[ 8192 ];
	char *error_message_string = {0};
	char sql_executable[ 1024 ];

	table_name = get_table_name( application_name, folder_name );

	update_database_build_update_clause(
		update_clause,
		application_name,
		changed_attribute_list,
		primary_attribute_name_list );

	*where_clause = '\0';

	update_database_build_where_clause(
		where_clause,
		where_attribute_list );

#ifdef UPDATE_DATABASE_DEBUG_MODE
	sprintf( sql_executable,
		 "cat >> %s",
		 appaserver_error_get_filename(
			application_name ) );
#else
	strcpy( sql_executable, "sql.e 2>&1" );
#endif

	sprintf( sys_string,
		 "echo \"update %s %s %s;\" | %s",
		 table_name,
		 update_clause,
		 where_clause,
		 sql_executable );

	appaserver_output_error_message(
		application_name,
		sys_string,
		login_name );

	/* Here is the update execution. */
	/* ----------------------------- */
	error_message_string = pipe2string( sys_string );

	if ( error_message_string )
	{
		appaserver_output_error_message(
			application_name,
			error_message_string,
			login_name );
		return error_message_string;
	}
	else
	{
		if ( list_length( additional_update_attribute_name_list ) )
		{
			char *additional_update_attribute_name;
			char *additional_update_data;

			if (	list_length(
				additional_update_attribute_name_list ) !=
		     		list_length(
				additional_update_data_list ) )
			{
				fprintf(stderr,
"ERROR in %s/%s()/%d: length of additional_update_attribute_name_list does not equal length of additional_update_data_list.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				exit( 1 );
			}

			list_rewind( additional_update_attribute_name_list );
			list_rewind( additional_update_data_list );

			do {
				additional_update_attribute_name =
					list_get_pointer(
					additional_update_attribute_name_list );

				additional_update_data =
					list_get_pointer(
					additional_update_data_list );

				if ( strcmp(	additional_update_data,
						NULL_STRING ) == 0 )
				{
					sprintf(update_clause,
				 		"set %s=null",
				 	additional_update_attribute_name );
				}
				else
				{
					sprintf(update_clause,
				 		"set %s='%s'",
				 	additional_update_attribute_name,
				 	additional_update_data );
				}

				sprintf( sys_string,
		 			"echo \"update %s %s %s;\" | %s",
		 			table_name,
		 			update_clause,
		 			where_clause,
					sql_executable );

				appaserver_output_error_message(
					application_name,
					sys_string,
					login_name );

				if ( system( sys_string ) ){};

				list_next( additional_update_data_list );

			} while( list_next(
				additional_update_attribute_name_list ) );
		}
	}

	return (char *)0;

} /* update_database_execute_for_folder() */

void update_database_build_where_clause(
				char *destination,
				LIST *where_attribute_list )
{
	WHERE_ATTRIBUTE *where_attribute;
	char data_buffer[ 4096 ];

	destination += sprintf( destination, "where 1 = 1" );

	if ( !list_reset( where_attribute_list ) ) return;

	do {
		where_attribute = list_get_pointer( where_attribute_list );

		if ( !where_attribute->data || !*where_attribute->data )
		{
			destination +=
				sprintf(
				destination, 
				" and (%s = 'null' or %s is null)",
				where_attribute->attribute_name,
				where_attribute->attribute_name );
		}
		else
		{
			strcpy( data_buffer, where_attribute->data );

			search_replace_special_characters( data_buffer );
			escape_special_characters( data_buffer );

			destination += sprintf( destination, 
						" and %s = '%s'",
						where_attribute->
							attribute_name,
						data_buffer );
		}

	} while( list_next( where_attribute_list ) );

} /* update_database_build_where_clause() */

void update_database_build_update_clause(
				char *destination,
				char *application_name,
				LIST *changed_attribute_list,
				LIST *primary_attribute_name_list )
{
	CHANGED_ATTRIBUTE *changed_attribute;
	int first_time = 1;
	char data[ 4096 ];
	char buffer[ 4096 ];

	destination += sprintf( destination, "set" );

	if ( !list_reset( changed_attribute_list ) ) return;

	do {
		changed_attribute = list_get_pointer( changed_attribute_list );

		strcpy( data, changed_attribute->new_data );

		if ( strcmp( data, UPDATE_DATABASE_NULL_TOKEN ) == 0 )
			*data = '\0';

		search_replace_special_characters( data );
		escape_special_characters( data );

		/* Encode password changes. However, exclude password resets. */
		/* ---------------------------------------------------------- */
		if ( strcmp(	changed_attribute->attribute_name,
				"password" ) == 0
		&&   *data
		&&   strcmp( data, UPDATE_DATABASE_NULL_TOKEN ) != 0
		&&   strcmp( data, NULL_STRING ) != 0 )
		{
			char *results;

			results =
				appaserver_user_version_encrypted_password(
					application_name,
					data /* typed_in_password */,
					appaserver_user_mysql_version() );

			if ( results )
			{
				strcpy( data, results );
			}
		}

		if ( !*data
		||   strcmp( data, NULL_OPERATOR ) == 0 )
		{
			strcpy( data, NULL_STRING );

			if ( list_exists_string(
					primary_attribute_name_list, 
					changed_attribute->attribute_name ) )
			{
				single_quotes_around( buffer, data );
				strcpy( data, buffer );
			}
		}
		else
		{
			single_quotes_around( buffer, data );
			strcpy( data, buffer );
		}

		if ( first_time )
		{
			destination += sprintf( destination,
					" %s=%s",
					changed_attribute->attribute_name,
					data );
			first_time = 0;
		}
		else
		{
			destination += sprintf( destination,
					",%s=%s",
					changed_attribute->attribute_name,
					data );
		}

	} while( list_next( changed_attribute_list ) );

} /* update_database_build_update_clause() */

boolean update_database_get_dictionary_index_data(
				char **destination,
				DICTIONARY *dictionary,
				char *key,
				int index )
{
	char dictionary_key[ 1024 ];
	char *data;

	sprintf( dictionary_key, 
	 	"%s_%d",
	 	key, index );

	data = dictionary_get( dictionary, dictionary_key );

	if ( !data ) return 0;

	*destination  = data;

	return 1;

} /* update_database_get_dictionary_index_data() */

void update_database_set_one2m_related_folder_list(
			LIST *update_folder_list,
			LIST *one2m_recursive_related_folder_list,
			int row,
			DICTIONARY *row_dictionary,
			DICTIONARY *file_dictionary,
			LIST *exclude_attribute_name_list,
			DICTIONARY *foreign_attribute_dictionary )
{
	RELATED_FOLDER *related_folder;
	LIST *include_attribute_name_list;
	UPDATE_FOLDER *update_folder;
	/* ------- */
	/* Ignored */
	/* ------- */
	boolean changed_key = 0;

	if ( !list_rewind( one2m_recursive_related_folder_list ) ) return;

	do {
		related_folder =
			list_get_pointer(
				one2m_recursive_related_folder_list );

		if ( list_length( related_folder->
					folder_foreign_attribute_name_list ) )
		{
			update_folder =
			update_database_get_folder_foreign_update_folder(
				row,
				row_dictionary,
				file_dictionary,
				related_folder->one2m_folder,
				related_folder->
					folder->
					primary_attribute_name_list,
				related_folder->
					folder_foreign_attribute_name_list );
		}
		else
		{
			include_attribute_name_list =
				list_copy_string_list(
					related_folder->
						foreign_attribute_name_list );

			update_folder =
			update_database_get_update_folder(
				&changed_key,
				row,
				row_dictionary,
				file_dictionary,
				related_folder->one2m_folder,
				related_folder->foreign_attribute_name_list,
				include_attribute_name_list,
				exclude_attribute_name_list,
				foreign_attribute_dictionary,
				(LIST *)0
				/* additional_unique_attribute_name_list */ );
		}

		if ( update_folder )
		{
			list_append_pointer(
				update_folder_list,
				update_folder );
		}

	} while( list_next( one2m_recursive_related_folder_list ) );

} /* update_database_set_one2m_related_folder_list() */

void update_row_free( UPDATE_ROW *update_row )
{
	list_free( update_row->update_folder_list );
	free( update_row );
} /* update_row_free() */

boolean update_database_changed_attribute(
			char *attribute_name,
			LIST *update_row_list )
{
	UPDATE_ROW *update_row;
	UPDATE_FOLDER *update_folder;
	CHANGED_ATTRIBUTE *changed_attribute;

	if ( !list_rewind( update_row_list ) ) return 0;

	do {
		update_row = list_get_pointer( update_row_list );

		if ( !list_rewind( update_row->update_folder_list ) ) continue;

		do {
			update_folder =
				list_get_pointer(
					update_row->update_folder_list );

			if ( !list_rewind( update_folder->
						changed_attribute_list ) )
			{
				continue;
			}

			do {
				changed_attribute =
					list_get_pointer(
						update_folder->
						changed_attribute_list );

				if ( strcmp(	changed_attribute->
							attribute_name,
						attribute_name ) == 0 )
				{
					return 1;
				}
			} while( list_next( update_folder->
						changed_attribute_list ) );

		} while( list_next( update_row->update_folder_list ) );
	} while( list_next( update_row_list ) );

	return 0;

} /* update_database_changed_attribute() */

char *update_database_update_row_list_display(
			LIST *update_row_list )
{
	char output_buffer[ 65536 ];
	char *buffer_pointer = output_buffer;
	UPDATE_ROW *update_row;
	boolean update_row_first_time = 1;

	*buffer_pointer = '\0';

	if ( !list_rewind( update_row_list ) ) return "";

	do {
		update_row = list_get_pointer( update_row_list );

		if ( update_row_first_time )
			update_row_first_time = 0;
		else
			buffer_pointer += sprintf( buffer_pointer, "," );

		buffer_pointer +=
			sprintf(	buffer_pointer,
					"\n\nrow = %d, changed_key = %d",
					update_row->row,
					update_row->changed_key );

		buffer_pointer +=
			sprintf( buffer_pointer,
				 "%s\n",
				 update_database_folder_list_display(
					update_row->update_folder_list ) );

	} while( list_next( update_row_list ) );

	buffer_pointer += sprintf( buffer_pointer, "\n" );

	return strdup( output_buffer );

} /* update_database_update_row_list_display() */

char *update_database_folder_list_display(
				LIST *update_folder_list )
{
	static char buffer[ 2048 ];
	char *buffer_pointer = buffer;
	UPDATE_FOLDER *update_folder;

	*buffer_pointer = '\0';

	if ( !list_rewind( update_folder_list ) )
	{
		return "";
	}

	do {
		update_folder = list_get_pointer( update_folder_list );

		buffer_pointer +=
			sprintf(	buffer_pointer,
					"%s",
					update_database_folder_display(
						update_folder ) );

	} while( list_next( update_folder_list ) );

	return buffer;

} /* update_database_folder_list_display() */

char *update_database_folder_display(
				UPDATE_FOLDER *update_folder )
{
	static char buffer[ 2048 ];
	char *buffer_pointer = buffer;

	*buffer_pointer = '\0';

	buffer_pointer += sprintf( buffer_pointer, "\n%c", 9 );

	buffer_pointer +=
		sprintf(buffer_pointer,
			"folder = %s, primary = %s",
			update_folder->folder_name,
			list_display(
				update_folder->
					primary_attribute_name_list ) );

	buffer_pointer +=
		update_database_changed_display(
			buffer_pointer,
			update_folder->changed_attribute_list );

	buffer_pointer +=
		update_database_where_display(
			buffer_pointer,
			update_folder->where_attribute_list );

	if ( update_folder->post_change_process )
	{
		buffer_pointer +=
			sprintf(buffer_pointer,
				",post_change=%s",
				update_folder->
					post_change_process->
					process_name );
	}

	return buffer;

} /* update_database_folder_display() */

int update_database_changed_display(	char *buffer_pointer,
					LIST *changed_attribute_list )
{
	CHANGED_ATTRIBUTE *changed_attribute;
	int output_character_count = 0;

	if ( !list_rewind( changed_attribute_list ) ) return 0;

	do {
		changed_attribute = list_get_pointer( changed_attribute_list );
		output_character_count +=
			sprintf( buffer_pointer,
			";changed_attribute = %s.%s, old = %s, new = %s",
				 changed_attribute->folder_name,
				 changed_attribute->attribute_name,
				 changed_attribute->old_data,
				 changed_attribute->new_data );
	} while( list_next( changed_attribute_list ) );
	return output_character_count;
} /* update_database_changed_display() */

int update_database_where_display(	char *buffer_pointer,
					LIST *where_attribute_list )
{
	WHERE_ATTRIBUTE *where_attribute;
	int output_character_count = 0;

	if ( !list_rewind( where_attribute_list ) ) return 0;

	do {
		where_attribute = list_get_pointer( where_attribute_list );

		output_character_count +=
			sprintf( buffer_pointer + output_character_count,
				 ", where_attribute: %s = '%s'",
				 where_attribute->attribute_name,
				 where_attribute->data );

	} while( list_next( where_attribute_list ) );
	return output_character_count;
} /* update_database_where_display() */

void update_database_set_login_name_each_row(
				DICTIONARY *dictionary,
				char *login_name )
{
	int row;
	int highest_index;
	char key[ 128 ];

	highest_index = dictionary_get_key_highest_index( dictionary );

	for( row = 1; row <= highest_index; row++ )
	{
		sprintf( key, "login_name_%d", row );
		dictionary_set_pointer( dictionary,
					strdup( key ),
					login_name );
	}

} /* update_database_set_login_name_each_row() */

int update_database_get_columns_updated(
				char *application_name,
				LIST *update_row_list )
{
	int columns_updated = 0;
	UPDATE_ROW *update_row;
	UPDATE_FOLDER *update_folder;
#ifdef UPDATE_DATABASE_DEBUG_MODE
	CHANGED_ATTRIBUTE *changed_attribute;
	char message[ 1024 ];
#endif

	/* Prevent compiler warning. */
	/* ------------------------- */
	if ( application_name );

	if ( !list_rewind( update_row_list ) ) return 0;

	do {
		update_row = list_get_pointer( update_row_list );

		if ( !list_rewind( update_row->update_folder_list ) )
		{
			continue;
		}

		do {
			update_folder =
				list_get_pointer(
					update_row->update_folder_list );

			if ( !list_rewind( update_folder->
						changed_attribute_list ) )
			{
				continue;
			}

			do {
#ifdef UPDATE_DATABASE_DEBUG_MODE
				changed_attribute =
					list_get_pointer(
						update_folder->
						changed_attribute_list );

				sprintf( message,
"Message: %s/%s()/%d got changed_attribute: %s.%s='%s'; Was='%s'",
					 __FILE__,
					 __FUNCTION__,
					 __LINE__,
					changed_attribute->folder_name,
					changed_attribute->attribute_name,
					changed_attribute->new_data,
					changed_attribute->old_data );

				appaserver_output_error_message(
					application_name,
					message,
					(char *)0 /* login_name */ );

#endif
				columns_updated++;

			} while( list_next( update_folder->
						changed_attribute_list ) );

		} while( list_next( update_row->update_folder_list ) );

	} while( list_next( update_row_list ) );

	return columns_updated;

} /* update_database_get_columns_updated() */

LIST *update_database_get_changed_folder_name_list(
				LIST *update_row_list )
{
	UPDATE_ROW *update_row;
	UPDATE_FOLDER *update_folder;
	CHANGED_ATTRIBUTE *changed_attribute;
	LIST *changed_folder_name_list;

	if ( !list_rewind( update_row_list ) )
	{
		return (LIST *)0;
	}

	changed_folder_name_list = list_new();
	do {
		update_row = list_get_pointer( update_row_list );

		if ( !list_rewind( update_row->update_folder_list ) )
		{
			continue;
		}

		do {
			update_folder =
				list_get_pointer(
					update_row->update_folder_list );

			if ( !list_rewind( update_folder->
						changed_attribute_list ) )
			{
				continue;
			}

			do {
				changed_attribute =
					list_get_pointer(
						update_folder->
						changed_attribute_list );

				list_append_unique_string(
					changed_folder_name_list,
					changed_attribute->folder_name );

			} while( list_next( update_folder->
						changed_attribute_list ) );

		} while( list_next( update_row->update_folder_list ) );

	} while( list_next( update_row_list ) );

	return changed_folder_name_list;

} /* update_database_get_changed_folder_name_list() */

