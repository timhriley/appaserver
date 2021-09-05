/* $APPASERVER_HOME/library/update.c					*/
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
#include "folder_attribute.h"
#include "folder.h"
#include "relation.h"
#include "appaserver_library.h"
#include "appaserver_user.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "update.h"

UPDATE *update_calloc( void )
{
	UPDATE *update;

	if ( ! ( update = calloc( 1, sizeof( UPDATE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return update;
}

UPDATE_PRIMARY *update_primary_calloc( void )
{
	UPDATE_PRIMARY *update_primary;

	if ( ! ( update_primary =
			calloc(
				1,
				sizeof( UPDATE_PRIMARY ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return update_primary;
}

UPDATE *update_new(	DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			char *role_name,
			char *folder_name )
{
	UPDATE *update = update_calloc();

	update->login_name = login_name;
	update->role_name = role_name;
	update->folder_name = folder_name;

	update->role =
		fole_fetch(
			role_name,
			1 /* fetch_attribute_exclude_list */ );

	update->post_dictionary = dictionary_copy( post_dictionary );
	update->file_dictionary = file_dictionary;

	update->folder =
		folder_fetch(
			update->folder_name,
			update->role_name,
			role->exclude_attribute_name_list,
			/* -------------------------- */
			/* Also sets primary_key_list */
			/* -------------------------- */
			1 /* fetch_folder_attribute_list */,
			0 /* not fetch_relation_mto1_non_isa_list */,
			/* ------------------------------------------ */
			/* Also sets folder_attribute_append_isa_list */
			/* ------------------------------------------ */
			1 /* fetch_relation_mto1_isa_list */,
			0 /* not fetch_relation_one2m_list */,
			1 /* fetch_relation_one2m_recursive_list */,
			1 /* fetch_process */,
			1 /* fetch_role_folder_list */,
			1 /* fetch_row_level_restriction */ );

	if ( !update->folder )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			update->folder_name );
		exit( 1 );
	}

	if ( !role_folder_update(
		folder->role_folder_list ) )
	{
		return (UPDATE *)0;
	}

	update->security_entity =
		security_entity(
			login_name,
			folder->non_owner_forbid,
			role->override_row_restrictions );

	update->update_row_list =
		update_row_list(
			post_dictionary,
			file_dictionary,
			update->folder,
			update->folder->relation_mto1_isa_list,
			update->folder->relation_one2m_recursive_list,
			update->security_entity );

	return update;
}

UPDATE_ROW *update_row_calloc( void )
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

char *update_execute(	char *application_name,
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

		update_execute_row(
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

void update_execute_row(
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
			update_execute_folder(
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
				(LIST *)0 /* primary_key_list */,
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

char *update_execute_folder(
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

#ifdef UPDATE_DEBUG_MODE
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

char *update_primary_set_clause(
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

		if ( strcmp( data, UPDATE_NULL_TOKEN ) == 0 )
			*data = '\0';

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
			destination += sprintf( destination, "," );
			first_time = 0;
		}

		destination +=
			sprintf(
				destination,
				" %s=%s",
				changed_attribute->attribute_name,
				data );

	} while( list_next( changed_attribute_list ) );

	return strdup( destination_buffer );
}

boolean update_dictionary_index_data(
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

	data = dictionary_get( dictionary_key, dictionary );

	if ( !data ) return 0;

	*destination  = data;

	return 1;
}

char *update_row_list_display(
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
				update_folder_list_display(
					update_row->update_folder_list ) );

	} while( list_next( update_row_list ) );

	buffer_pointer += sprintf( buffer_pointer, "\n" );

	return strdup( output_buffer );

}

char *update_folder_list_display(
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
					update_folder_display(
						update_folder ) );

	} while( list_next( update_folder_list ) );

	return buffer;
}

char *update_folder_display(
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
		update_changed_display(
			buffer_pointer,
			update_folder->changed_attribute_list );

	buffer_pointer +=
		update_where_display(
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

int update_changed_display(	char *buffer_pointer,
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

int update_where_display(
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

void update_set_login_name_each_row(
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

UPDATE_PRIMARY *update_primary_new(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER *folder,
			SECURITY_ENTITY *security_entity,
			int row )
{
	UPDATE_PRIMARY *primary;
	LIST *changed_attribute_list;

	if ( !dictionary_length( post_dictionary ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: post_dictionary is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !dictionary_length( file_dictionary ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: file_dictionary is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: folder is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( folder->folder_attribute_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( folder->primary_key_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !row )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: row is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	changed_attribute_list =
		update_primary_changed_attribute_list(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			post_dictionary,
			file_dictionary,
			folder->folder_attribute_list,
			row );

	if ( !list_length( changed_attribute_list ) )
	{
		return (UPDATE_PRIMARY *)0;
	}

	primary = update_primary_calloc();

	primary->changed_attribute_list = changed_attribute_list;

	primary->where_attribute_list =
		update_primary_where_attribute_list(
			file_dictionary,
			folder->primary_key_list,
			row );

	primary->primary_where_clause =
		security_entity_where_clause(
			update_where_clause(
				primary->where_attribute_list ),
		security_entity );

	primary->folder_delimited =
		folder_delimited(
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			folder_table_name(
				environment_application_name(),
				folder->folder_name ),
			folder->primary_key_list,
			primary->primary_where_clause );

	return primary;
}

LIST *update_folder_primary_data_list(
			DICTIONARY *file_dictionary,
			LIST *primary_key_list,
			int row )
{
	LIST *data_list;
	char *attribute_name;
	char *data;

	if ( !list_rewind( primary_key_list ) )
		return (LIST *)0;

	data_list = list_new();

	do {
		attribute_name =
			list_get(
				primary_key_list );

		if ( !update_dictionary_index_data(
			&data,
			file_dictionary,
			attribute_name,
			row ) )
		{
			char msg[ 1024 ];
			sprintf(msg,
"ERROR in %s/%s()/%d: update_dictionary_index_data(%s/%d) returned empty.\n",
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

	} while ( list_next( primary_key_list ) );
	return data_list;
}

UPDATE_ROW *update_row_new(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER *folder,
			LIST *relation_mto1_isa_list,
			LIST *relation_one2m_recursive_list,
			SECURITY_ENTITY *security_entity,
			int row )
{
	UPDATE_ROW *update_row;
	UPDATE_PRIMARY *update_primary;

	if ( ! ( update_primary =
			update_primary_new(
				post_dictionary,
				file_dictionary,
				folder,
				security_entity,
				row ) ) )
	{
		return (UPDATE_ROW *)0;
	}

	if ( !list_length( primary->changed_attribute_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: changed_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( primary->where_attribute_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: where_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !string_length( primary->folder_delimited ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_delimited is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	u = update_row_calloc();

	u->row = row;
	u->update_primary = primary;

	primary->key_changed_attribute_list =
		update_primary_key_changed_attribute_list(
			primary->changed_attribute_list );

	if ( list_length( primary->key_changed_attribute_list ) )
	{
		if ( list_length( relation_one2m_recursive_list ) )
		{
			u->one2m_list =
				update_row_one2m_list(
				   u->update_primary->
					key_changed_attribute_list,
				   u->update_primary->
					where_attribute_list,
				   relation_one2m_recursive_list );
		}

		if ( list_length( folder->relation_mto1_isa_list ) )
		{
			u->folder_mto1_list =
				update_row_mto1_list(
				   u->update_primary->
					key_changed_attribute_list,
				   u->update_primary->
					where_attribute_list,
				   relation_mto1_isa_list );
		}
	}

	u->update_row_cell_count =
		update_row_cell_count(
			list_length(
				u->update_primary->changed_attribute_list ),
			u->folder_one2m_list,
			u->folder_mto_list );

	return u;
}

LIST *update_row_list(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER *folder,
			LIST *relation_mto1_isa_list,
			LIST *relation_one2m_recursive_list,
			SECURITY_ENTITY *security_entity )
{
	LIST *update_row_list = {0};
	UPDATE_ROW *u;
	int row;
	int highest_index;

	if ( !dictionary_length( post_dictionary ) ) return (LIST *)0;
	if ( !dictionary_length( file_dictionary ) ) return (LIST *)0;

	highest_index = dictionary_key_highest_index( post_dictionary );

	for( row = 1; row <= highest_index; row++ )
	{
		if ( ( u =
			update_row(
				/* ------------------------------ */
				/* Sets preupdate_$attribute_name */
				/* ------------------------------ */
				post_dictionary,
				file_dictionary,
				folder,
				relation_mto1_isa_list,
				relation_one2m_recursive_list,
				security_entity,
				row ) ) )
		{
			if ( !update_row_list ) update_row_list = list_new();

			list_set( update_row_list, u );
		}
	}
	return update_row_list;
}

LIST *update_primary_where_attribute_list(
			DICTIONARY *file_dictionary,
			LIST *primary_key_list,
			int row )
{
	UPDATE_WHERE_ATTRIBUTE *where_attribute;
	LIST *where_attribut_list;
	char *primary_key;

	if ( !list_rewind( primary_key_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (LIST *)0;
	}

	where_attribute_list = list_new();

	do {
		primary_key =
			list_get(
				primary_key_list );

		if ( ! ( where_attribute =
				update_where_attribute(
					file_dictionary,
					primary_key,
					row ) ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: update_where_attribute(%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				primary_key );
			return (LIST *)0;
		}

		list_set( where_attribute_list, where_attribute );

	} while( list_next( primary_key_list ) );

	return where_attribute_list;
}

UPDATE_WHERE_ATTRIBUTE *update_where_attribute_calloc( void )
{
	UPDATE_WHERE_ATTRIBUTE *where_attribute;

	if ( ! ( where_attribute =
			calloc( 1, sizeof( UPDATE_WHERE_ATTRIBUTE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return where_attribute;
}


UPDATE_WHERE_ATTRIBUTE *update_where_attribute_new(
			DICTIONARY *file_dictionary,
			char *primary_key,
			int row )
{
	char *key;
	char *data;
	UPDATE_WHERE_ATTRIBUTE *where_attribute;

	key =
		/* Returns static memory */
		/* --------------------- */
		update_dictionary_key(
			primary_key,
			row );

	if ( ! data = dictionary_fetch( key, file_dictionary ) )
	{
		return (UPDATE_WHERE_ATTRIBUTE *)0;
	}


	where_attribute = update_where_attribute_calloc();

	where_attribute->attribute_name = attribute_name;
	where_attribute->data = data;

	return where_attribute;
}

int update_cell_count( LIST *update_row_list )
{
	int cells_updated = 0;
	UPDATE_ROW *update_row;

	if ( !list_rewind( update_row_list ) ) return 0;

	do {
		update_row = list_get( update_row_list );

		if ( !update_row->update_row_cell_count )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: update_row_cell_count is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		cells_updated += update_row->update_row_cell_count;

	} while( list_next( update_row_list ) );

	return cells_updated;
}

int update_row_cell_count(
			int primary_cell_count,
			LIST *update_one2m_list,
			LIST *update_mto1_list )
{
	UPDATE_ONE2M *update_one2m;
	UPDATE_MTO1 *update_mto1;
	int cell_count = primary_cell_count;

	if ( list_rewind( update_one2m_list ) )
	{
		do {
			update_one2m =
				list_get(
					update_one2m_list );

			cell_count +=
			( list_length(
				update_one2m->
					one2m_changed_attribute_list ) *
			  list_length(
				update_one2m->
					folder_delimited_list ) );

		} while( list_next( update_one2m_list ) );
	}

	if ( list_rewind( update_mto1_list ) )
	{
		do {
			update_mto1 =
				list_get(
					update_mto1_list );

			cell_count +=
			( list_length(
				update_mto1->
					mto1_changed_attribute_list ) *
			  list_length(
				update_mto1->
					folder_delimited_list ) );

		} while( list_next( update_mto1_list ) );
	}

	return cell_count;
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

UPDATE_CHANGED_ATTRIBUTE *update_changed_attribute_new(
			char *folder_name,
			char *attribute_name )
{
	UPDATE_CHANGED_ATTRIBUTE_DATA *u =
		update_changed_attribute_data_calloc();

	u->folder_attribute =
		folder_attribute_new(
			folder_name,
			attribute_name );
}

UPDATE_CHANGED_ATTRIBUTE_DATA *update_changed_attribute_data_new(
			char *key,
			char *old_data,
			char *escaped_replaced_new_data )
{
	UPDATE_CHANGED_ATTRIBUTE_DATA *changed_attribute_data;

	changed_attribute_data = changed_attribute_data_calloc();

	changed_attribute_data->key = key;
	changed_attribute_data->old_data = old_data;

	changed_attribute_data->escaped_replaced_new_data =
		escaped_replaced_new_data;

	return changed_attribute_data;
}

UPDATE_CHANGED_ATTRIBUTE_DATA *update_changed_attribute_data_fetch(
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

	if ( strcmp( new_data, UPDATE_NULL_TOKEN ) == 0 )
		*new_data = '\0';

	if ( strcmp( old_data, "select" ) == 0 && !*new_data )
	{
		return (CHANGED_ATTRIBUTE_DATA *)0;
	}

	if ( ! ( changed_attribute = ( strcmp( old_data, new_data ) != 0 ) ) )
	{
		return (CHANGED_ATTRIBUTE_DATA *)0;
	}

	changed_attribute_data = update_changed_attribute_data_calloc();

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

UPDATE_CHANGED_ATTRIBUTE *update_changed_attribute_fetch(
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

	changed_attribute->data = update_changed_attribute_data;

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
		UPDATE_PREUPDATE_PREFIX,
		attribute_name,
		row );

	return strdup( label );
}

LIST *update_primary_changed_attribute_list(
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
				update_changed_attribute_fetch(
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

UPDATE_MTO1 *update_mto1_calloc( void )
{
	UPDATE_MTO1 *u;

	if ( ! ( u = calloc( 1, sizeof( UPDATE_MTO1 ) ) ) )
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

UPDATE_ONE2M *update_one2m_calloc( void )
{
	UPDATE_ONE2M *u;

	if ( ! ( u = calloc( 1, sizeof( UPDATE_ONE2M ) ) ) )
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

UPDATE_ONE2M *update_one2m_new(
			LIST *primary_key_changed_attribute_list,
			LIST *primary_where_attribute_list,
			RELATION *relation_one2m )
{
	UPDATE_ONE2M *one2m;

	if ( !list_length( primary_key_changed_attribute_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: primary_key_changed_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( primary_where_attribute_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: primary_where_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !relation_one2m->many_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: many_folder is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( relation_one2m->foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	one2m = update_one2m_calloc();

	one2m->primary_key_changed_attribute_list =
		primary_key_changed_attribute_list;

	one2m->foreign_changed_attribute_list =
		update_foreign_changed_attribute_list(
			primary_key_changed_attribute_list,
			relation_one2m->many_folder->folder_name,
			relation_one2m->foreign_key_list );

	one2m->foreign_where_attribute_list =
		update_foreign_where_attribute_list(
			primary_key_changed_attribute_list,
			relation_one2m->foreign_key_list );

	one2m->folder_delimited_list =
		folder_delimited_list(
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			folder_table_name(
				environment_application_name(),
				one2m->many_folder->folder_name ),
					/* table_name */,
			one2m->many_folder->primary_key_list
				/* attribute_name_list */,
			update_where_clause(
				one2m->foreign_where_attribute_list ),
				/* where_clause */ );

	return one2m;
}

UPDATE_MTO1 *update_mto1(
			LIST *primary_key_changed_attribute_list,
			LIST *primary_where_attribute_list,
			RELATION *relation_one2m )
{
	UPDATE_MTO1 *mto1;

	if ( !list_length( primary_key_changed_attribute_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: primary_key_changed_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( primary_where_attribute_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: primary_where_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !relation_mto1->one_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: one_folder is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( relation_mto1->foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	mto1 = update_mto1_calloc();

	mto1->primary_key_changed_attribute_list =
		primary_key_changed_attribute_list;

	mto1->foreign_changed_attribute_list =
		update_foreign_changed_attribute_list(
			primary_key_changed_attribute_list,
			relation_mto1->one_folder->folder_name,
			relation_mto1->foreign_key_list );

	mto1->foreign_where_attribute_list =
		update_foreign_where_attribute_list(
			primary_key_changed_attribute_list,
			relation_mto1->foreign_key_list );

	mto1->folder_delimited_list =
		folder_delimited_list(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			folder_table_name(
				environment_application_name(),
				mto1->one_folder->folder_name ),
					/* table_name */,
			mto1->many_folder->primary_key_list
				/* attribute_name_list */,
			update_where_clause(
				mto1->foreign_where_attribute_list ),
				/* where_clause */ );

	return mto1;
}

LIST *update_related_changed_attribute_list(
			LIST *primary_key_changed_attribute_list,
			LIST *foreign_key_list )
{
}

LIST *update_related_delimited_list(
			char *folder_name,
			LIST *related_primary_key_list,
			char *where_clause )
{
}

LIST *update_primary_key_changed_attribute_list(
			LIST *primary_changed_attribute_list )
{
	UPDATE_CHANGED_ATTRIBUTE *update_changed_attribute;
	LIST *primary_key_changed_attribute_list = {0};

	if ( !list_rewind( primary_changed_attribute_list ) )
		return (LIST *)0;

	do {
		update_changed_attribute =
			list_get(
				primary_changed_attribute_list );

		if ( !update_changed_attribute->folder_attribute )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_attribute is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( update_changed_attribute->
			folder_attribute->
			primary_key_index )
		{
			if ( !primary_key_changed_attribute_list )
				primary_key_changed_attribute_list =
					list_new();

			list_set(
				primary_key_changed_attribute_list,
				update_changed_attribute );
		}

	} while ( list_next( primary_changed_attribute_list ) );

	return primary_key_changed_attribute_list;
}

LIST *update_foreign_where_attribute_list(
			LIST *primary_key_changed_attribute_list,
			LIST *foreign_key_list )
{
	UPDATE_WHERE_ATTRIBUTE *where_attribute;
	UPDATE_CHANGED_ATTRIBUTE *changed_attribute;
	LIST *where_attribute_list;
	char *foreign_key;

	if ( !list_rewind( primary_key_changed_attribute_list ) )
		return (LIST *)0;

	where_attribute_list = list_new();

	do {
		changed_attribute =
			list_get(
				primary_key_changed_attribute_list );

		foreign_key =
			/* index is one based */
			/* ------------------ */
			list_seek_index(
				foreign_key_list,
				changed_attribute->
					folder_attribute->
					primary_key_index );

		if ( !foreign_key )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: for folder_name = %s, list_seek_index(%s,%d) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				changed_attribute->
					folder_attribute->
					folder->
					folder_name,
				list_display_delimited(
					foreign_key_list,
					'^' ),
				changed_attribute->
					folder_attribute->
					primary_key_index );
			exit( 1 );
		}

		where_attribute = update_where_attribute_calloc();

		where_attribute->attribute_name = foreign_key;

		where_attribute->data =
			changed_attribute->
				changed_attribute_data->
				old_data;

		list_set( where_attribute_list, where_attribute );

	} while ( list_next( primary_key_changed_attribute_list ) );

	return where_attribute_list;
}

LIST *update_foreign_changed_attribute_list(
			LIST *primary_key_changed_attribute_list,
			char *foreign_folder_name,
			LIST *foreign_key_list )
{
	UPDATE_CHANGED_ATTRIBUTE *changed_attribute;
	UPDATE_CHANGED_ATTRIBUTE *foreign_changed_attribute;
	LIST *changed_attribute_list;
	char *foreign_key;

	if ( !list_rewind( primary_key_changed_attribute_list ) )
		return (LIST *)0;

	changed_attribute_list = list_new();

	do {
		changed_attribute =
			list_get(
				primary_key_changed_attribute_list );

		foreign_key =
			/* index is one based */
			/* ------------------ */
			list_seek_index(
				foreign_key_list,
				changed_attribute->
					folder_attribute->
					primary_key_index );

		if ( !foreign_key )
		{
			fprintf(stderr,
"ERROR in %s/%s()/%d: for folder_name = %s, list_seek_index(%s,%d) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				changed_attribute->
					folder_attribute->
					folder->
					folder_name,
				list_display_delimited(
					foreign_key_list,
					'^' ),
				changed_attribute->
					folder_attribute->
					primary_key_index );
			exit( 1 );
		}

		foreign_changed_attribute =
			update_changed_attribute_new(
				foreign_folder_name /* folder_name */,
				foreign_key /* attribute_name */ );

		changed_attribute->data =
			update_changed_attribute_data_new(
				changed_attribute->
					changed_attribute_data->
					key;
				changed_attribute->
					changed_attribute_data->
					old_data;
				changed_attribute->
					changed_attribute_data->
					escaped_replaced_new_data );

		list_set( changed_attribute_list, changed_attribute );

	} while ( list_next( primary_key_changed_attribute_list ) );

	return changed_attribute_list;
}

char *update_row_list_sql(
			char *login_name,
			SECURITY_ENTITY *security_entity,
			LIST *update_row_list )
{
	UPDATE_ROW *update_row;
	char mysql_message_list_string[ STRING_SYSTEM_BUFFER ];
	char *ptr = mysql_message_list_string;
	char *return_string = {0};

	*ptr = '\0';

	if ( list_rewind( update_row_list ) )
	{
		do {
			update_row = list_get( update_row_list );

			return_string =
				/* ------------------- */
				/* Returns heap memory */
				/* ------------------- */
				update_row_sql(
					login_name,
					security_entity,
					update_row );

			if ( return_string )
			{
				ptr += sprintf( ptr, "%s;", return_string );
				free( return_string );
			}

		} while ( list_next( update_row_list ) );
	}
	return strdup( mysql_message_list_string );
}

char *update_row_sql(	char *login_name,
			SECURITY_ENTITY *security_entity,
			UPDATE_ROW *update_row )
{
	char *set_clause;
	char *security_where_clause;
	char mysql_message[ STRING_SYSTEM_BUFFER ];
	char *ptr = mysql_message;
	char *return_message;

	if ( !update_row )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: update_row is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	/* Execute the update primary folder */
	/* --------------------------------- */
	if ( !update_row->update_primary )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: update_primary is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( set_clause =
			update_set_clause(
				update_row->
					update_primary->
					changed_attribute_list ) ) )
	{
		fprintf(stderr,
			"%s/%s()/%d: update_set_clause() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( where_clause =
			security_entity_where_clause(
				update_where_clause(
					update_row->
						update_primary->
						where_attribute_list ),
				security_entity ) ) )
	{
		fprintf(stderr,
		"%s/%s()/%d: security_entity_where_clause() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return_message =
		/* ------------------------------------------- */
		/* Safely returns mysql_message as heap memory */
		/* ------------------------------------------- */
		update_folder_sql(
			login_name,
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			folder_table_name(
				environment_application_name(),
				update_row->
					update_primary->
					folder->
					folder_name ),
			set_clause,
			where_clause );

	/* If the primary_folder generates an error, then return now. */
	/* ---------------------------------------------------------- */
	if ( return_message && *return_message )
	{
		return return_message;
	}

	/* Execute the post_change_process */
	/* ------------------------------- */
}

char *update_post_change_process( char *command_line )
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
				(LIST *)0 /* primary_key_list */,
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

char *update_set_clause( LIST *changed_attribute_list )
{
}

char *update_where_clause(
			LIST *where_attribute_list )
{
}

char *update_folder_sql(
			char *login_name,
			char *table_name,
			char *update_set_clause,
			char *security_entity_where_clause )
{
	char system_string[ STRING_SYSTEM_BUFFER ];
	char *error_message_string = {0};
	char sql_executable[ 1024 ];

#ifdef UPDATE_DEBUG_MODE
	sprintf( sql_executable,
		 "cat >> %s",
		 appaserver_error_filename(
			application_name ) );
#else
	sprintf( sql_executable,
		 "sql.e 2>&1 >> %s",
		 appaserver_error_filename(
			application_name ) );
#endif

	sprintf( sys_string,
		 "echo \"update %s %s %s;\" | %s",
		 table_name,
		 set_clause,
		 where_clause,
		 sql_executable );

	if ( strcmp( table_name, "appaserver_user" ) == 0 )
	{
		appaserver_output_error_message(
			application_name,
			"Hidden appaserver_user update statement executed",
			login_name );
	}
	else
	{
		appaserver_output_error_message(
			application_name,
			system_string,
			login_name );
	}

	/* Here is the update execution. */
	/* ----------------------------- */
	error_message_string = string_pipe_fetch( sys_string );

	if ( error_message_string && *error_message )
	{
		appaserver_output_error_message(
			application_name,
			error_message_string,
			login_name );

		return error_message_string;
	}
	else
	{
		return strdup( "" );
	}
}

LIST *update_data_list( LIST *changed_attribute_list )
{
	LIST *data_list;
	UPDATE_CHANGED_ATTRIBUTE *changed_attribute;

	if ( !list_rewind( changed_attribute_list ) ) return (LIST *)0;

	data_list = list_new();

	do {
		changed_attibute = list_get( changed_attribute_list );

		if ( !changed_attribute->changed_attribute_data )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: changed_attribute_data is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			data_list,
			changed_attribute->
				changed_attribute_data->
				escaped_replaced_new_data );

	} while ( list_next( changed_attribute_list ) );

	return data_list;
}
