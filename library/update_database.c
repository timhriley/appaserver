/* $APPASERVER_HOME/library/update_database.c				*/
/* -------------------------------------------------------------------- */
/* This is the appaserver update_database ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "list.h"
#include "dictionary.h"
#include "process.h"
#include "environ.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "attribute.h"
#include "folder.h"
#include "relation.h"
#include "appaserver_library.h"
#include "appaserver_user.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "update_database.h"

UPDATE_DATABASE *update_database_calloc( void )
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
	return update_database;
}

UPDATE_FOLDER *update_folder_calloc( void )
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
	return update_folder;
}

UPDATE_DATABASE *update_database_new(
			char *application_name,
			char *session,
			char *login_name,
			char *role_name,
			char *folder_name,
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary )
{
	UPDATE_DATABASE *update_database =
		update_database_calloc();

	update_database->application_name = application_name;
	update_database->session = session;
	update_database->login_name = login_name;
	update_database->role_name = role_name;
	update_database->folder_name = folder_name;
	update_database->post_dictionary = dictionary_copy( post_dictionary );
	update_database->file_dictionary = file_dictionary;

	update_database->folder =
		folder_fetch(
			update_database->folder_name,
			1 /* fetch_attribute_list */,
			0 /* not fetch_one2m_relation_list */,
			1 /* fetch_one2m_recursive_relation_list */,
			1 /* fetch_mto1_isa_recursive_relation_list */,
			0 /* not fetch_mto1_relation_list */ );

	if ( !update_database->folder )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			update_database->folder_name );
		exit( 1 );
	}

	dictionary_set_indexed_date_time_to_current(
		update_database->post_dictionary,
		update_database->folder->attribute_list );

	if ( update_database->folder->row_level_non_owner_forbid )
	{
		update_database_set_login_name_each_row(
				post_dictionary,
				login_name );

		update_database_set_login_name_each_row(
				file_dictionary,
				login_name );
	}

	appaserver_library_post_dictionary_database_convert_dates(
			file_dictionary,
			application_name,
			update_database->folder->attribute_list );

	dictionary_remove_symbols_in_numbers(
			file_dictionary,
			update_database->folder->attribute_list );

	return update_database;
}

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

}

boolean update_database_data_if_changed(
			char **old_data,
			char **new_data,
			/* ------------------------------------------------- */
			/* Set the preupdate_$attribute_name for the trigger */
			/* ------------------------------------------------- */
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *attribute_name,
			int row )
{
	char preupdate_attribute_name[ 128 ];

	if ( !post_dictionary )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: post_dictionary is empty./\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !file_dictionary )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: file_dictionary is empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	*old_data = (char *)0;
	*new_data = (char *)0;

	if ( !update_database_dictionary_index_data(
		new_data,
		post_dictionary,
		attribute_name,
		row ) )
	{
		return 0;
	}

	if ( !update_database_dictionary_index_data(
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
	/* --------------------------------------------------------------- */
	/* Got a change. Set the preupdate attribute name for the trigger. */
	/* --------------------------------------------------------------- */
	{
		sprintf(	preupdate_attribute_name,
				"%s%s_%d",
				UPDATE_DATABASE_PREUPDATE_PREFIX,
				attribute_name,
				row );

		dictionary_set_pointer(
			post_dictionary,
			strdup( preupdate_attribute_name ),
			*old_data );

		return 1;
	}
}

UPDATE_ROW *update_database_update_row_calloc( void )
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
	return update_row;
}

UPDATE_ROW *update_database_update_row_new( int row )
{
	UPDATE_ROW *update_row =
		update_database_update_row_calloc();

	update_row->row = row;
	update_row->update_folder_list = list_new_list();

	return update_row;

}

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

}

char *update_database_execute(	char *application_name,
				char *session,
				LIST *update_row_list,
				DICTIONARY *post_dictionary,
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

		update_database_execute_row(
				error_messages,
			   	application_name,
				session,
				post_dictionary,
				login_name,
			   	update_row->update_folder_list,
				update_row->row,
				role_name,
				additional_update_attribute_name_list,
				additional_update_data_list,
				abort_if_first_update_failed );

	} while( list_next( update_row_list ) );

	return error_messages;
}

void update_database_execute_row(
			char *error_messages,
			char *application_name,
			char *session,
			DICTIONARY *post_dictionary,
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
		update_folder = list_get( update_folder_list );

		if ( ( error_message_string =
			update_database_execute_folder(
			   	application_name,
				login_name,
			   	update_folder->
					folder_name,
				update_folder->
					set_clause,
				update_folder->
					where_clause,
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
		update_folder = list_get( update_folder_list );

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
				post_dictionary /* parameter_dictionary */,
				post_dictionary /* where_clause_dictionary */,
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
}

char *update_database_execute_folder(
			char *application_name,
			char *login_name,
			char *folder_name,
			char *set_clause,
			char *where_clause,
			LIST *additional_update_attribute_name_list,
			LIST *additional_update_data_list )
{
	char *table_name;
	char sys_string[ 8192 ];
	char *error_message_string = {0};
	char additional_set_clause[ 1024 ];
	char sql_executable[ 8192 ];

	table_name = get_table_name( application_name, folder_name );

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
		 set_clause,
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
				list_get(
				additional_update_attribute_name_list );

			additional_update_data =
				list_get(
				additional_update_data_list );

			if ( strcmp(	additional_update_data,
					NULL_STRING ) == 0 )
			{
				sprintf(additional_set_clause,
			 		"set %s=null",
				 	additional_update_attribute_name );
			}
			else
			{
				sprintf(additional_set_clause,
			 		"set %s='%s'",
			 	additional_update_attribute_name,
			 	additional_update_data );
			}

			sprintf( sys_string,
	 			"echo \"update %s %s %s;\" | %s",
	 			table_name,
	 			additional_set_clause,
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
	return (char *)0;
}

char *update_folder_where_clause(
			LIST *where_attribute_list )
{
	WHERE_ATTRIBUTE *where_attribute;
	char destination_buffer[ 4096 ];
	char *destination = destination_buffer;
	boolean first_time = 1;

	destination += sprintf( destination, "where " );

	if ( !list_reset( where_attribute_list ) ) return (char *)0;

	do {
		where_attribute = list_get( where_attribute_list );

		if ( first_time )
		{
			first_time = 0;
		}
		else
		{
			destination +=
				sprintf(destination,
					" and " );
		}

		if ( !where_attribute->data || !*where_attribute->data )
		{
			destination +=
				sprintf(
				destination, 
				"(%s = 'null' or %s is null)",
				where_attribute->attribute_name,
				where_attribute->attribute_name );
		}
		else
		{
			destination +=
				sprintf(
				destination, 
				"%s = '%s'",
				where_attribute->
					attribute_name,
				/* ------------------------------------------ */
				/* Assume search_replace_special_characters() */
				/* ------------------------------------------ */
				where_attribute->data );
		}

	} while( list_next( where_attribute_list ) );

	return strdup( destination_buffer );
}

char *update_folder_set_clause(
			LIST *changed_attribute_list )
{
	CHANGED_ATTRIBUTE *changed_attribute;
	int first_time = 1;
	char data[ 4096 ];
	char buffer[ 4096 ];
	char destination_buffer[ 65536 ];
	char *destination = destination_buffer;

	destination += sprintf( destination, "set" );

	if ( !list_reset( changed_attribute_list ) ) return (char *)0;

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
					environment_application_name(),
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
			if ( changed_attribute->changed_primary_key_index )
			{
				single_quotes_around( buffer, data );
				strcpy( data, buffer );
			}
			else
			{
				strcpy( data, NULL_STRING );
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

	return strdup( destination_buffer );
}

boolean update_database_dictionary_index_data(
			char **destination,
			DICTIONARY *dictionary,
			char *key,
			int index )
{
	char dictionary_key[ 1024 ];
	char *data;

	sprintf(dictionary_key, 
	 	"%s_%d",
	 	key, index );

	data = dictionary_get( dictionary, dictionary_key );

	if ( !data ) return 0;

	*destination  = data;

	return 1;
}

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
			sprintf(buffer_pointer,
				"\n\nrow = %d, changed_primary_key = %d",
				update_row->row,
				update_row->changed_primary_key );

		buffer_pointer +=
			sprintf(buffer_pointer,
				"%s\n",
				update_database_folder_list_display(
					update_row->update_folder_list ) );

	} while( list_next( update_row_list ) );

	buffer_pointer += sprintf( buffer_pointer, "\n" );

	return strdup( output_buffer );

}

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
}

char *update_database_folder_display(
				UPDATE_FOLDER *update_folder )
{
	static char buffer[ 2048 ];
	char *buffer_pointer = buffer;

	*buffer_pointer = '\0';

	buffer_pointer += sprintf( buffer_pointer, "\n%c", 9 );

	buffer_pointer +=
		sprintf(buffer_pointer,
			"folder = %s, where_attribute_name_list = %s",
			update_folder->folder_name,
			list_display(
				update_folder->
					where_attribute_name_list ) );

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

}

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
			";changed_attribute = %s, old = %s, new = %s",
				 changed_attribute->attribute_name,
				 changed_attribute->old_data,
				 changed_attribute->new_data );
	} while( list_next( changed_attribute_list ) );
	return output_character_count;
}

int update_database_where_display(
			char *buffer_pointer,
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
}

void update_database_set_login_name_each_row(
			DICTIONARY *dictionary,
			char *login_name )
{
	int row;
	int highest_index;
	char key[ 128 ];

	highest_index = dictionary_key_highest_index( dictionary );

	for( row = 1; row <= highest_index; row++ )
	{
		sprintf( key, "login_name_%d", row );
		dictionary_set_pointer( dictionary,
					strdup( key ),
					login_name );
	}

}

LIST *update_changed_attribute_list(
			boolean *changed_primary_key,
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			LIST *attribute_list,
			int row )
{
	ATTRIBUTE *attribute;
	LIST *changed_attribute_list = {0};
	CHANGED_ATTRIBUTE *changed_attribute;
	char *old_data;
	char *new_data;

	if ( !dictionary_length( post_dictionary )
	||   !dictionary_length( file_dictionary ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: row and file dictionary lengths don't match.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );

		return (LIST *)0;
	}

	if ( !list_rewind( attribute_list ) ) return (LIST *)0;

	do {
		attribute = list_get( attribute_list );

		old_data = (char *)0;
		new_data = (char *)0;

		if ( update_database_data_if_changed(
			&old_data,
			&new_data,
			/* ------------------------------------------------- */
			/* Set the preupdate_$attribute_name for the trigger */
			/* ------------------------------------------------- */
			post_dictionary,
			file_dictionary,
			attribute->attribute_name,
			row ) )
		{
			if ( ! ( changed_attribute =
				    update_changed_attribute_new(
					attribute->attribute_name,
					attribute->datatype,
					old_data,
					new_data ) ) )
			{
				fprintf(stderr,
"Warning in %s/%s()/%d: update_database_changed_attribute_new() returned empty.\n",
					__FILE__,
					__FUNCTION__,
					__LINE__ );
				continue;
			}

			if ( !changed_attribute_list )
			{
				changed_attribute_list = list_new_list();
			}

			changed_attribute->changed_primary_key_index =
				attribute->primary_key_index;

			if ( changed_attribute->changed_primary_key_index )
				*changed_primary_key = 1;

			list_append_pointer(
				changed_attribute_list,
				changed_attribute );
		}
	} while( list_next( attribute_list ) );

	return changed_attribute_list;
}

CHANGED_ATTRIBUTE *update_changed_attribute_new(
			char *attribute_name,
			char *attribute_datatype,
			char *old_data,
			char *new_data )
{
	CHANGED_ATTRIBUTE *changed_attribute;

	changed_attribute =
		(CHANGED_ATTRIBUTE *)
			calloc( 1, sizeof( CHANGED_ATTRIBUTE ) );

	changed_attribute->attribute_name = attribute_name;
	changed_attribute->attribute_datatype = attribute_datatype;
	changed_attribute->old_data = old_data;

	if ( timlib_strcmp( attribute_datatype, "float" ) == 0 )
	{
		strcpy(	new_data,
			timlib_trim_money_characters(
				new_data ) );
	}

	if ( strcmp( new_data, FORBIDDEN_NULL ) == 0 )
		changed_attribute->new_data = NULL_STRING;
	else
		changed_attribute->new_data = new_data;

	return changed_attribute;
}

CHANGED_ATTRIBUTE *update_database_changed_attribute(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			ATTRIBUTE *attribute,
			int row )
{
	CHANGED_ATTRIBUTE *changed_attribute = {0};
	char preupdate_attribute_name[ 128 ];
	char *old_data = {0};
	char *new_data = {0};

	if ( !post_dictionary )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: no post_dictionary\n",
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

	if ( !update_database_dictionary_index_data(
		&new_data,
		post_dictionary,
		attribute->attribute_name,
		row ) )
	{
		return (CHANGED_ATTRIBUTE *)0;
	}

	if ( !update_database_dictionary_index_data(
		&old_data,
		file_dictionary,
		attribute->attribute_name,
		row ) )
	{
		return (CHANGED_ATTRIBUTE *)0;
	}

	if ( strcmp( old_data, new_data ) == 0 )
	{
		return (CHANGED_ATTRIBUTE *)0;
	}
	else
	if ( strcmp( old_data, "select" ) == 0 && !*new_data )
	{
		return (CHANGED_ATTRIBUTE *)0;
	}
	else
	/* ------------ */
	/* Got a change */
	/* ------------ */
	{
		changed_attribute =
			update_changed_attribute_new(
				attribute->attribute_name,
				attribute->datatype,
				old_data,
				new_data );

		/* ------------------------------------------------- */
		/* Set the preupdate_$attribute_name for the trigger */
		/* ------------------------------------------------- */
		sprintf(	preupdate_attribute_name,
				"%s%s_%d",
				UPDATE_DATABASE_PREUPDATE_PREFIX,
				attribute->attribute_name,
				row );

		dictionary_set_pointer(
			post_dictionary,
			strdup( preupdate_attribute_name ),
			old_data );
	}
	return changed_attribute;
}

UPDATE_FOLDER *update_database_primary_update_folder(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *folder_name,
			LIST *attribute_list,
			PROCESS *post_change_process,
			int row )
{
	UPDATE_FOLDER *update_folder;
	LIST *changed_attribute_list;
	boolean changed_primary_key = 0;

	changed_attribute_list =
		update_changed_attribute_list(
			&changed_primary_key,
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			post_dictionary,
			file_dictionary,
			attribute_list,
			row );

	if ( !changed_attribute_list ) return (UPDATE_FOLDER *)0;

	update_folder = update_folder_calloc();

	/* Input */
	/* ----- */
	update_folder->folder_name = folder_name;
	update_folder->attribute_list = attribute_list;
	update_folder->row = row;
	update_folder->post_change_process = post_change_process;

	update_folder->primary_attribute_name_list =
		attribute_primary_name_list(
			attribute_list );

	/* Process */
	/* ------- */
	update_folder->changed_primary_key = changed_primary_key;
	update_folder->changed_attribute_list = changed_attribute_list;

	update_folder->primary_data_list =
		update_folder_primary_data_list(
			file_dictionary,
			attribute_primary_name_list(
				attribute_list ),
			row );

	return update_folder;
}

LIST *update_folder_primary_data_list(
			DICTIONARY *file_dictionary,
			LIST *primary_attribute_name_list,
			int row )
{
	LIST *data_list;
	char *attribute_name;
	char *data;

	if ( !list_rewind( primary_attribute_name_list ) )
		return (LIST *)0;

	data_list = list_new();

	do {
		attribute_name =
			list_get(
				primary_attribute_name_list );

		if ( !update_database_dictionary_index_data(
			&data,
			file_dictionary,
			attribute_name,
			row ) )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: update_database_dictionary_index_data(%s/%d) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				attribute_name,
				row );

			fprintf(stderr,
"Message in %s/%s()/%d: file_dictionary = [%s]\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				dictionary_display( file_dictionary ) );

			exit( 1 );
		}

		list_set( data_list, data );

	} while ( list_next( primary_attribute_name_list ) );
	return data_list;
}

UPDATE_ROW *update_database_update_row(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *folder_name,
			LIST *attribute_list,
			LIST *one2m_recursive_relation_list,
			PROCESS *post_change_process,
			int row )
{
	UPDATE_ROW *update_row;
	UPDATE_FOLDER *primary_update_folder;

	if ( ! ( primary_update_folder =
			update_folder_set_where_clause(
				update_database_primary_update_folder(
					post_dictionary,
					file_dictionary,
					folder_name,
					attribute_list,
					post_change_process,
					row ) ) ) )
	{
		return (UPDATE_ROW *)0;
	}

	primary_update_folder->count =
		update_folder_count(
			primary_update_folder->
				folder_name,
			primary_update_folder->
				where_clause );

	primary_update_folder->set_clause =
		update_folder_set_clause(
			primary_update_folder->
				changed_attribute_list );

	update_row = update_database_update_row_calloc();
	update_row->row = row;

	update_row->changed_primary_key =
		primary_update_folder->changed_primary_key;

	update_row->update_folder_list =
		update_database_update_folder_list(
			primary_update_folder->primary_data_list,
			primary_update_folder->changed_attribute_list,
			update_row->changed_primary_key,
			one2m_recursive_relation_list,
			primary_update_folder );

	return update_row;
}

LIST *update_database_update_folder_list(
			LIST *primary_data_list,
			LIST *primary_changed_attribute_list,
			boolean changed_primary_key,
			LIST *one2m_recursive_relation_list,
			UPDATE_FOLDER *primary_update_folder )
{
	LIST *update_folder_list = list_new();
	RELATION *relation;
	UPDATE_FOLDER *update_folder;

	list_set( update_folder_list, primary_update_folder );

	if ( !changed_primary_key
	||   !list_rewind( one2m_recursive_relation_list ) )
	{
		return update_folder_list;
	}

	do {
		relation =
			list_get(
				one2m_recursive_relation_list );

		update_folder =
			update_folder_set_where_clause(
				update_secondary_update_folder(
					relation->many_folder->folder_name,
					relation->foreign_attribute_name_list,
					primary_data_list,
					primary_changed_attribute_list ) );

		if ( !update_folder )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: update_folder_set_where_clause(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				relation->many_folder->folder_name );
			exit( 1 );
		}

		if ( ! ( update_folder->count =
				update_folder_count(
					update_folder->folder_name,
					update_folder->where_clause ) ) )
		{
			continue;
		}

		update_folder->set_clause =
			update_folder_set_clause(
				update_folder->
					changed_attribute_list );

		list_set( update_folder_list, update_folder );

	} while ( list_next( one2m_recursive_relation_list ) );

	return update_folder_list;
}

LIST *update_database_update_row_list(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *folder_name,
			LIST *attribute_list,
			LIST *one2m_recursive_relation_list,
			LIST *mto1_isa_recursive_relation_list,
			PROCESS *post_change_process )
{
	LIST *update_row_list;
	int row;
	int highest_index;
	RELATION *relation;

	if ( !post_dictionary ) return (LIST *)0;

	update_row_list = list_new();
	highest_index = dictionary_key_highest_index( post_dictionary );

	for( row = 1; row <= highest_index; row++ )
	{
		list_set(
			update_row_list,
			update_database_update_row(
				post_dictionary,
				file_dictionary,
				folder_name,
				attribute_list,
				one2m_recursive_relation_list,
				post_change_process,
				row ) );

		if ( !list_rewind( mto1_isa_recursive_relation_list ) )
			continue;

		do {
			relation =
				list_get(
					mto1_isa_recursive_relation_list );

			list_set(
				update_row_list,
				update_database_update_row(
					post_dictionary,
					file_dictionary,
					relation->
						one_folder->
						folder_name,
					relation->
						one_folder->
						attribute_list,
					relation->
						one_folder->
						one2m_recursive_relation_list,
					relation->
						one_folder->
						post_change_process,
					row ) );

		} while ( list_next( mto1_isa_recursive_relation_list ) );
	}
	return update_row_list;
}

UPDATE_FOLDER *update_secondary_update_folder(
			char *many_folder_name,
			LIST *relation_foreign_attribute_name_list,
			LIST *primary_data_list,
			LIST *primary_changed_attribute_list )
{
	UPDATE_FOLDER *update_folder;
	CHANGED_ATTRIBUTE *primary_changed_attribute;
	char *changed_attribute_name;

	if ( !list_rewind( primary_changed_attribute_list ) )
		return (UPDATE_FOLDER *)0;

	update_folder = update_folder_calloc();

	update_folder->folder_name = many_folder_name;
	update_folder->primary_data_list = primary_data_list;

	update_folder->relation_foreign_attribute_name_list =
		relation_foreign_attribute_name_list;

	update_folder->changed_attribute_list = list_new();

	do {
		primary_changed_attribute =
			list_get( primary_changed_attribute_list );

		changed_attribute_name =
			list_seek_index(
				update_folder->
					relation_foreign_attribute_name_list,
				primary_changed_attribute->
					changed_primary_key_index );

		if ( !changed_attribute_name )
		{
			fprintf(stderr,
"Warning in %s/%s()/%d: many_folder_name = %s cannot list_seek_index = %d\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				many_folder_name,
				primary_changed_attribute->
					changed_primary_key_index );
			continue;
		}

		list_set(
			update_folder->changed_attribute_list,
			update_changed_attribute_new(
				changed_attribute_name,
				primary_changed_attribute->attribute_datatype,
				primary_changed_attribute->old_data,
				primary_changed_attribute->new_data ) );

	} while ( list_next( primary_changed_attribute_list ) );

	return update_folder;
}

UPDATE_FOLDER *update_folder_set_where_clause(
			UPDATE_FOLDER *update_folder )
{
	if ( !update_folder ) return (UPDATE_FOLDER *)0;

	update_folder->where_attribute_name_list =
		    update_database_where_attribute_name_list(
			update_folder->
				primary_attribute_name_list,
			update_folder->
			     relation_foreign_attribute_name_list );

	update_folder->where_attribute_list =
		update_folder_where_attribute_list(
			update_folder->where_attribute_name_list,
			update_folder->primary_data_list );

	if ( !list_length( update_folder->where_attribute_list ) )
		return (UPDATE_FOLDER *)0;

	update_folder->where_clause =
		update_folder_where_clause(
			update_folder->where_attribute_list );

	return update_folder;
}

LIST *update_folder_where_attribute_list(
			LIST *attribute_name_list,
			LIST *primary_data_list )
{
	WHERE_ATTRIBUTE *where_attribute;
	char *attribute_name;
	char data_buffer[ 1024 ];
	LIST *where_attribute_list;

	if ( !list_rewind( attribute_name_list ) ) return (LIST *)0;

	list_rewind( primary_data_list );

	where_attribute_list = list_new();

	do {
		attribute_name = list_get( attribute_name_list );
		strcpy( data_buffer, list_get( primary_data_list ) );

		search_replace_special_characters( data_buffer );
		escape_special_characters( data_buffer );

		where_attribute =
			update_database_where_attribute_new(
				attribute_name,
				strdup( data_buffer ) );

		list_set( where_attribute_list, where_attribute );

		if ( !list_next( primary_data_list ) ) break;

	} while( list_next( attribute_name_list ) );

	return where_attribute_list;
}

int update_database_cells_updated(
			LIST *update_row_list )
{
	int cells_updated = 0;
	UPDATE_ROW *update_row;
	UPDATE_FOLDER *update_folder;

	if ( !list_rewind( update_row_list ) ) return 0;

	do {
		update_row = list_get_pointer( update_row_list );

		if ( !list_rewind( update_row->update_folder_list ) )
		{
			continue;
		}

		do {
			update_folder =
				list_get(
					update_row->update_folder_list );

			cells_updated +=
				( update_folder_columns_updated(
					update_folder->
						changed_attribute_list ) *
					update_folder->count ); 

		} while( list_next( update_row->update_folder_list ) );

	} while( list_next( update_row_list ) );

	return cells_updated;
}

int update_folder_columns_updated(
			LIST *changed_attribute_list )
{
	int columns_updated;

	if ( !list_rewind( changed_attribute_list ) )
	{
		return 0;
	}

	columns_updated = 0;

	do {
		columns_updated++;

	} while( list_next( changed_attribute_list ) );

	return columns_updated;
}

LIST *update_database_changed_folder_name_list(
			LIST *update_row_list )
{
	UPDATE_ROW *update_row;
	UPDATE_FOLDER *update_folder;
	LIST *changed_folder_name_list;

	if ( !list_rewind( update_row_list ) ) return (LIST *)0;

	changed_folder_name_list = list_new();

	do {
		update_row = list_get( update_row_list );

		if ( !list_rewind( update_row->update_folder_list ) )
		{
			continue;
		}

		do {
			update_folder =
				list_get(
					update_row->update_folder_list );

			list_set_unique(
				changed_folder_name_list,
				update_folder->folder_name );

		} while( list_next( update_row->update_folder_list ) );

	} while( list_next( update_row_list ) );

	return changed_folder_name_list;
}

LIST *update_database_where_attribute_name_list(
			LIST *primary_attribute_name_list,
			LIST *relation_foreign_attribute_name_list )
{
	if ( list_length( primary_attribute_name_list ) )
		return primary_attribute_name_list;
	else
		return relation_foreign_attribute_name_list;
}

int update_folder_count(
			char *folder_name,
			char *where_clause )
{
	char sys_string[ 1024 ];
	char *results;

	sprintf(sys_string,
		"select.sh 'count(1)' %s \"%s\"",
		folder_name,
		where_clause );

	if ( ( results = pipe2string( sys_string ) ) )
	{
		return atoi( results );
	}
	else
	{
		return 0;
	}
}

boolean update_database_attribute_exists(
			char *attribute_name,
			LIST *update_row_list )
{
	UPDATE_ROW *update_row;
	UPDATE_FOLDER *update_folder;
	CHANGED_ATTRIBUTE *changed_attribute;

	if ( !list_rewind( update_row_list ) ) return 0;

	do {
		update_row = list_get( update_row_list );

		if ( !list_rewind( update_row->update_folder_list ) )
			continue;

		do {
			update_folder =
				list_get(
					update_row->update_folder_list );

			if ( !list_rewind(
				update_folder->
					changed_attribute_list ) )
			{
				continue;
			}

			do {
				changed_attribute =
					list_get(
						update_folder->
						     changed_attribute_list );

				if ( strcmp(
					changed_attribute->attribute_name,
					attribute_name ) == 0 )
				{
					return 1;
				}
			} while ( list_next( 
					update_folder->
						changed_attribute_list ) );

		} while ( list_next( update_row->update_folder_list ) );

	} while ( list_next( update_row_list ) );

	return 0;
}

