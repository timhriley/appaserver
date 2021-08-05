/* $APPASERVER_HOME/library/update_database.c				*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "String.h"
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

	if ( ! ( update_database = calloc( 1, sizeof( UPDATE_DATABASE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return update_database;
}

UPDATE_FOLDER_PRIMARY *update_folder_primary_calloc( void )
{
	UPDATE_FOLDER_PRIMARY *update_folder_primary;

	if ( ! ( update_folder_primary =
			calloc(
				1,
				sizeof( UPDATE_FOLDER_PRIMARY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return update_folder_primary;
}

UPDATE_DATABASE *update_database(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			char *role_name,
			char *folder_name )
{
	UPDATE_DATABASE *update_database = update_database_calloc();

	update_database->login_name = login_name;
	update_database->role_name = role_name;
	update_database->folder_name = folder_name;

	update_database->post_dictionary = dictionary_copy( post_dictionary );
	update_database->file_dictionary = file_dictionary;

	update_database->folder =
		folder_fetch(
			update_database->folder_name,
			update_database->role_name,
			exclude_attribute_name_list,
			1 /* fetch_folder_attribute_list */,
			1 /* fetch_relation_mto1_isa_list */,
			0 /* not fetch_relation_one2m_list */,
			1 /* fetch_relation_one2m_recursive_list */,
			1 /* fetch_process */,
			0 /* not fetch_role_folder_list */,
			1 /* fetch_row_level_restriction */ );

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

WHERE_ATTRIBUTE *update_where_attribute(
			char *attribute_name,
			char *data )
{
	WHERE_ATTRIBUTE *where_attribute;

	if ( ! ( where_attribute =
			(WHERE_ATTRIBUTE *)
				calloc( 1, sizeof( WHERE_ATTRIBUTE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	where_attribute->attribute_name = attribute_name;
	where_attribute->data = data;

	return where_attribute;
}

UPDATE_ROW *update_database_update_row_calloc( void )
{
	UPDATE_ROW *update_row;

	update_row = calloc( 1, sizeof( UPDATE_ROW ) );

	if ( !update_row )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
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

	if ( strcmp( table_name, "appaserver_user" ) != 0 )
	{
		appaserver_output_error_message(
			application_name,
			sys_string,
			login_name );
	}

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
			     where_attribute->attribute_name,
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
	char data[ 65536 ];
	char buffer[ 65536 ];
	char destination_buffer[ 65536 ];
	char *destination = destination_buffer;

	destination += sprintf( destination, "set" );

	if ( !list_reset( changed_attribute_list ) ) return (char *)0;

	do {
		changed_attribute = list_get( changed_attribute_list );

		strcpy( data, changed_attribute->escaped_replaced_new_data );

		if ( strcmp( data, UPDATE_DATABASE_NULL_TOKEN ) == 0 )
			*data = '\0';

/*
		security_replace_special_characters( data );
		strcpy( data, security_sql_injection_escape( data ) );
*/

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
			destination +=
				sprintf(
					destination,
					" %s=%s",
					changed_attribute->attribute_name,
					data );

			first_time = 0;
		}
		else
		{
			destination +=
				sprintf(
					destination,
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
				 changed_attribute->escaped_replaced_new_data );

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

LIST *update_folder_changed_attribute_list(
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

		if ( !update_database_data_if_changed(
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
			continue;
		}

		changed_attribute =
			update_changed_attribute_new(
				attribute->attribute_name,
				attribute->datatype,
				old_data,
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				security_sql_injection_escape(
					/* ------------ */
					/* Returns data */
					/* ------------ */
					security_replace_special_characters(
						/* ------------ */
						/* Returns data */
						/* ------------ */
						string_trim_number_characters(
							new_data,
							attribute->
							     datatype ) ) ) );

		if ( !changed_attribute )
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

		list_set(
			changed_attribute_list,
			changed_attribute );

	} while( list_next( attribute_list ) );

	return changed_attribute_list;
}

CHANGED_ATTRIBUTE *update_changed_attribute_new(
			char *attribute_name,
			char *attribute_datatype,
			char *old_data,
			char *escaped_replaced_new_data )
{
	CHANGED_ATTRIBUTE *changed_attribute;

	changed_attribute =
		(CHANGED_ATTRIBUTE *)
			calloc( 1, sizeof( CHANGED_ATTRIBUTE ) );

	changed_attribute->attribute_name = attribute_name;
	changed_attribute->attribute_datatype = attribute_datatype;
	changed_attribute->old_data = old_data;

	if ( strcmp( escaped_replaced_new_data, FORBIDDEN_NULL ) == 0 )
		changed_attribute->escaped_replaced_new_data = NULL_STRING;
	else
		changed_attribute->escaped_replaced_new_data =
			escaped_replaced_new_data;

	return changed_attribute;
}

CHANGED_ATTRIBUTE *update_database_changed_attribute(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			ATTRIBUTE *attribute,
			int row )
{
	CHANGED_ATTRIBUTE *changed_attribute;
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

	/* ------------ */
	/* Got a change */
	/* ------------ */
	changed_attribute =
		update_changed_attribute_new(
			attribute->attribute_name,
			attribute->datatype,
			old_data,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			security_sql_injection_escape(
				/* ------------ */
				/* Returns data */
				/* ------------ */
				security_replace_special_characters(
					/* ------------ */
					/* Returns data */
					/* ------------ */
					string_trim_number_characters(
						new_data,
						attribute->
						     datatype ) ) ) );

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

	return changed_attribute;
}

UPDATE_FOLDER_PRIMARY *update_folder_primary(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER *folder,
			SECURITY_ENTITY *security_entity,
			int row )
{
	UPDATE_FOLDER_PRIMARY *update_folder_primary;
	LIST *changed_attribute_list;

	changed_attribute_list =
		changed_attribute_list(
			post_dictionary,
			file_dictionary,
			folder->folder_attribute_list,
			row );

	if ( !list_length( changed_attribute_list ) )
	{
		return (UPDATE_FOLDER_PRIMARY *)0;
	}

	update_folder_primary = update_folder_primary_calloc();

	update_folder_primary->changed_attribute_list =
		changed_attribute_list;


	update_folder_primary->post_dictionary = post_dictionary;
	update_folder_primary->file_dictionary = file_dictionary;
	update_folder_primary->folder = folder;
	update_folder_primary->security_entity = security_entity;
	update_folder_primary->row = row;

	update_folder_primary->where_attribute_list =
		update_where_attribute_list(
			file_dictionary,
			folder_attribute_primary_name_list(
				folder->folder_attribute_list ),
			row );

	update_folder_primary->where_clause =
		update_folder_primary_where_clause(
			update_folder_primary->where_attribute_list );

	update_folder_primary->security_entity_where_clause =
		security_entity_where_clause(
			update_folder_primary->where_clause,
			update_folder_primary->security_entity );

	update_folder_primary->set_clause =
		update_folder_primary_set_clause(
			update_folder_primary->
				changed_attribute_list );

	update_folder_primary->folder_delimited_list =
		folder_delimited_list(
			folder_table_name(
				folder->folder_name ),
			folder_attribute_primary_name_list(
				folder->folder_attribute_list ),
			update_folder_primary->
				security_entity_where_clause );

	return update_folder_primary;
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
			char msg[ 1024 ];
			sprintf(msg,
"ERROR in %s/%s()/%d: update_database_dictionary_index_data(%s/%d) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				attribute_name,
				row );

			m2( environment_application_name(), msg );

			sprintf(msg,
"Message in %s/%s()/%d: file_dictionary = [%s]\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				dictionary_display( file_dictionary ) );

			m2( environment_application_name(), msg );

			exit( 1 );
		}

		list_set( data_list, data );

	} while ( list_next( primary_attribute_name_list ) );
	return data_list;
}

UPDATE_ROW *update_row(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER *folder,
			SECURITY_ENTITY *security_entity,
			int row )
{
	UPDATE_ROW *r;
	UPDATE_FOLDER_PRIMARY *folder_primary;

	if ( ! ( folder_primary =
			update_folder_primary(
				post_dictionary,
				file_dictionary,
				folder,
				security_entity,
				row ) ) )
	{
		return (UPDATE_ROW *)0;
	}

	if ( !string_length( folder_primary->folder_delimited_fetch ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_delimited_fetch is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( folder_primary->update_where_attribute_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: update_where_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	r = update_row_calloc();

	r->row = row;
	r->update_folder_primary = folder_primary;

	folder_primary->primary_changed_attribute_list =
		update_primary_changed_attribute_list(
			folder_primary->update_changed_attribute_list );

	if ( list_length( folder_primary->primary_changed_attribute_list )
	&&   list_length( folder->relation_one2m_recursive_list ) )
	{
		folder_primary->folder_related_list =
			update_folder_related_list(
				folder_primary->
					primary_changed_attribute_list,
				folder_primary->
					update_where_attribute_list,
				folder->relation_one2m_recursive_list );
	}

	r->update_row_cell_count =
		update_row_cell_count(
			list_length(
				folder_primary->
					update_changed_attribute_list ),
			folder_primary->folder_related_list );

	return r;
}

LIST *update_row_list(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER *folder,
			SECURITY_ENTITY *security_entity )
{
	LIST *update_row_list = {0};
	UPDATE_ROW *update_row;
	int row;
	int highest_index;

	if ( !dictionary_length( post_dictionary ) ) return (LIST *)0;
	if ( !dictionary_length( file_dictionary ) ) return (LIST *)0;

	highest_index = dictionary_key_highest_index( post_dictionary );

	for( row = 1; row <= highest_index; row++ )
	{
		if ( ( update_row =
			update_database_update_row(
				/* ------------------------------ */
				/* Sets preupdate_$attribute_name */
				/* ------------------------------ */
				post_dictionary,
				file_dictionary,
				folder,
				security_entity,
				row ) ) )
		{
			if ( !update_row_list ) update_row_list = list_new();

			list_set( update_row_list, update_row );
		}
	}
	return update_row_list;
}

LIST *update_where_attribute_list(
			DICTIONARY *file_dictionary,
			LIST *primary_attribute_name_list,
			int row )
{
	WHERE_ATTRIBUTE *where_attribute;
	LIST *where_attribut_list;
	char *attibute_name;
	char *key;
	char *data;

	if ( !list_rewind( primary_attribute_name_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: primary_attribute_name_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	where_attribute_list = list_new();

	do {
		attribute_name =
			list_get(
				primary_attribute_name_list );

		key =
			/* Returns static memory */
			/* --------------------- */
			update_dictionary_key(
				attribute_name,
				row );

		if ( ! data = dictionary_fetch( key, file_dictionary ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: dictionary_fetch(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				key );
			exit( 1 );
		}

		list_set(
			where_attribute_list,
			update_where_attribute(
				attribute_name,
				data ) );

	} while( list_next( primary_attribute_name_list ) );

	return where_attribute_list;
}

int update_database_cell_count(
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

int update_row_cell_count(
			int primary_cell_count,
			LIST *update_folder_related_list )
{
	int related_cell_count;
	UPDATE_FOLDER_RELATED *update_folder_related;

	if ( !list_rewind( update_folder_related_list ) )
	{
		return primary_cell_count;
	}

	related_cell_count = 0;

	do {
		update_folder_related = list_get( update_folder_related_list );

here1

	} while( list_next( update_folder_related_list ) );

	return primary_cell_count + related_cell_count;
}

UPDATE_CHANGED_ATTRIBUTE_DATA *update_changed_attribute_data_calloc( void )
{
	UPDATE_CHANGED_ATTRIBUTE_DATA *u;

	if ( ! ( u = calloc( 1, sizeof( UPDATE_CHANGED_ATTRIBUTE_DATA ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return u;
}

UPDATE_CHANGED_ATTRIBUTE_DATA *update_changed_attribute_data(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *attribute_name,
			char *attribute_datatype,
			int primary_key_index,
			int row )
{
	UPDATE_CHANGED_ATTRIBUTE_DATA *changed_attribute_data;
	char *key;
	char *old_data;
	char *new_data;
	boolean changed_attribute;

	key =
		/* Returns static memory */
		/* --------------------- */
		update_dictionary_key(
			attribute_name,
			row );

	if ( ! ( old_data = dictionary_fetch( key, file_dictionary ) ) )
	{
		return (CHANGED_ATTRIBUTE_DATA *)0;
	}

	if ( ! ( new_data = dictionary_fetch( key, post_dictionary ) ) )
	{
		return (CHANGED_ATTRIBUTE_DATA *)0;
	}

	if ( strcmp( new_data, UPDATE_DATABASE_NULL_TOKEN ) == 0 )
		*new_data = '\0';

	if ( strcmp( old_data, "select" ) == 0 && !*new_data )
	{
		return (CHANGED_ATTRIBUTE_DATA *)0;
	}

	if ( ! ( changed_attribute = ( strcmp( old_data, new_data ) != 0 ) ) )
	{
		return (CHANGED_ATTRIBUTE_DATA *)0;
	}

	changed_attribute_data = changed_attribute_data_calloc();

	changed_attribute_data->post_dictionary = post_dictionary;
	changed_attribute_data->file_dictionary = file_dictionary;
	changed_attribute_data->attribute_name = attribute_name;
	changed_attribute_data->attribute_datatype = attribute_datatype;
	changed_attribute_data->primary_key_index = primary_key_index;
	changed_attribute_data->row = row;
	changed_attribute_data->changed_attribute = changed_attribute;

	changed_attribute_data->escaped_replaced_new_data =
		security_sql_injection_escape(
			security_replace_special_characters(
				string_trim_number_characters(
					new_data,
					attribute_datatype ) ) );

	return changed_attribute_data;
}

char *update_dictionary_key(
			char *attribute_name,
			int row )
{
	static char key[ 128 ];

	sprintf(key,
		"%s_%d",
		attribute_name,
		row );

	return key;
}

UPDATE_CHANGED_ATTRIBUTE *update_changed_attribute_calloc( void )
{
	UPDATE_CHANGED_ATTRIBUTE *u;

	if ( ! ( u = calloc( 1, sizeof( UPDATE_CHANGED_ATTRIBUTE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return u;
}

UPDATE_CHANGED_ATTRIBUTE *update_changed_attribute(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER_ATTRIBUTE *folder_attribute,
			int row )
{
	UPDATE_CHANGED_ATTRIBUTE *changed_attribute;
	UPDATE_CHANGED_ATTRIBUTE_DATA *changed_attribute_data;

	if ( !folder_attribute )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_attribute is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !folder_attribute->attribute )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( changed_attribute_data =
			update_changed_attribute_data(
				post_dictionary,
				file_dictionary,
				folder_attribute->attribute->attribute_name,
				folder_attribute->attribute->attribute_datatype,
				folder_attribute->primary_key_index,
				row ) ) )
	{
		return (UPDATE_CHANGED_ATTRIBUTE *)0;
	}

	changed_attribute = update_changed_attribute_calloc();
	changed_attribute->folder_attribute = folder_attribute;

	changed_attribute->changed_attribute_data =
		update_changed_attribute_data;

	changed_attribute->changed_attribute_preupdate_label =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		update_changed_attribute_preupdate_label(
			folder_attribute->attribute->attribute_name,
			row );

	dictionary_set(
		post_dictionary,
		changed_attribute->changed_attribute_preupdate_label,
		old_data );

	return changed_attribute;
}

char *update_changed_attribute_preupdate_label(
			char *attribute_name,
			int row )
{
	char label[ 128 ];

	if ( !attribute_name || !*attribute_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: attribute_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(label,
		"%s%s_%d",
		UPDATE_DATABASE_PREUPDATE_PREFIX,
		attribute_name,
		row );

	return strdup( label );
}

LIST *update_changed_attribute_list(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			LIST *folder_attribute_list,
			int row )
{
	UPDATE_CHANGED_ATTRIBUTE *changed_attribute;
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *changed_attribute_list = {0};

	if ( !list_rewind( folder_attribute_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: list_rewind() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		folder_attribute = list_get( folder_attribute_list );

		if ( ( changed_attribute =
				update_changed_attribute(
					post_dictionary,
					file_dictionary,
					folder_attribute,
					row ) ) )
		{
			if ( !changed_attribute_list )
				changed_attribute_list =
					list_new();

			list_set(
				changed_attribute_list,
				changed_attribute );
		}

	} while ( list_next( folder_attribute_list ) );

	return changed_attribute_list;
}

