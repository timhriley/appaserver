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

UPDATE_ROOT *update_root_calloc( void )
{
	UPDATE_ROOT *update_root;

	if ( ! ( update_root = calloc( 1, sizeof( UPDATE_ROOT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return update_root;
}

UPDATE *update_new(	DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			char *role_name,
			char *folder_name )
{
	UPDATE *update;

	if ( !post_dictionary )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: post_dictionary is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE *)0;
	}

	if ( !file_dictionary )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: file_dictionary is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE *)0;
	}

	if ( !login_name )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: login_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE *)0;
	}

	if ( !role_name )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: role_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE *)0;
	}

	if ( !folder_name )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE *)0;
	}

 	update = update_calloc();

	update->role =
		fole_fetch(
			role_name,
			1 /* fetch_attribute_exclude_list */ );

	update->folder =
		folder_fetch(
			folder_name,
			role_name,
			role_exclude_update_attribute_name_list(
				update->role->attribute_exclude_list ),
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
			1 /* fetch_row_level_restriction */,
			0 /* not fetch_role_operation_list */ );

	if ( !update->folder )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: folder_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			update->folder_name );
		return (UPDATE *)0;
	}

	if ( !role_folder_update(
		update->folder->role_folder_list ) )
	{
		return (UPDATE *)0;
	}

	update->security_entity =
		security_entity(
			login_name,
			update->folder->non_owner_forbid,
			update->role->override_row_restrictions );

	update->update_row_list =
		/* -------------------------- */
		/* Returns null if no updates */
		/* -------------------------- */
		update_row_list_new(
			post_dictionary,
			file_dictionary,
			login_name,
			update->folder->folder_name,
			update->folder->folder_attribute_list,
			update->folder->relation_mto1_isa_list,
			update->folder->relation_one2m_recursive_list,
			update->folder->post_change_process,
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

UPDATE_ROOT *update_root_new(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			char *folder_name,
			LIST *folder_attribute_list,
			PROCESS *post_change_process,
			SECURITY_ENTITY *security_entity,
			int row )
{
	UPDATE_ROOT *update_root;

	if ( !dictionary_length( post_dictionary ) )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: post_dictionary is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_ROOT *)0;
	}

	if ( !dictionary_length( file_dictionary ) )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: file_dictionary is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_ROOT *)0;
	}

	if ( !login_name )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: login_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_ROOT *)0;
	}

	if ( !folder_name )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_ROOT *)0;
	}

	if ( !list_length( folder_attribute_list ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: folder_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_ROOT *)0;
	}

	if ( row <= 0 )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: row is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_ROOT *)0;
	}

	update_root = update_root_calloc();

	update_root->update_attribute_list =
		update_attribute_list(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			post_dictionary,
			file_dictionary,
			folder_attribute_list,
			row );

	update_root->update_attribute_changed_list =
		update_attribute_changed_list(
			update_root->update_attribute_list );

	if ( !list_length( update_root->update_attribute_changed_list ) )
	{
		list_free_container( update_root->update_attribute_list );
		free( update_root );
		return (UPDATE_ROOT *)0;
	}

	update_root->update_where_attribute_list =
		update_where_attribute_list(
			update_root->update_attribute_list );

	update_root->update_where_clause =
		update_where_clause(
			update_root->update_where_attribute_list );

	update_root->update_root_where_clause =
		update_root_where_clause(
			update_root->update_where_clause,
			security_entity );

	update_root->update_sql_statement =
		update_sql_statement(
			folder_table_name(
				environment_application_name(),
				folder_name ),
			update_root->update_attribute_changed_list,
			update_root->update_root_where_clause );

	if ( post_change_process )
	{
		update_root->update_command_line =
			update_command_line(
				post_change_process,
				login_name,
				update_root->update_attribute_list );
	}

	update_root->changed_primary_key =
		update_attribute_changed_primary_key(
			update_root->update_attribute_changed_list );

	return update_root;
}

UPDATE_ROW *update_row_new(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			char *folder_name,
			LIST *folder_attribute_list,
			LIST *relation_mto1_isa_list,
			LIST *relation_one2m_recursive_list,
			PROCESS *post_change_process,
			SECURITY_ENTITY *security_entity,
			int row )
{
	UPDATE_ROW *update_row = update_row_calloc();

	update_row->row = row;

	update_row->update_root =
		update_root_new(
			post_dictionary,
			file_dictionary,
			login_name,
			folder_name,
			folder_attribute_list,
			post_change_process,
			security_entity,
			row );

	if ( list_length( relation_mto1_isa_list ) )
	{
		update_row->update_mto1_isa_list =
			update_mto1_isa_list(
				post_dictionary,
				file_dictionary,
				login_name,
				relation_mto1_isa_list,
				row );
	}

	if ( update_row->update_root
	&&   update_row->update_root->changed_primary_key
	&&   list_length( relation_one2m_recursive_list ) )
	{
		update_row->update_one2m_list =
			update_one2m_list(
				login_name,
				update_row->
					update_root->
					update_changed_attribute_list,
			   	update_row->
					update_root->
					update_where_attribute_list,
			   	relation_one2m_recursive_list );
	}

	if ( !update_row->update_root
	&&   !update_row->update_mto1_isa_list
	&&   !update_row->update_one2m_list )
	{
		free( update_row );
		return (UPDATE_ROW *)0;
	}

	update_row->update_row_cell_count =
		update_row_cell_count(
			update_row->update_root,
			update_row->update_mto1_isa_list,
			update_row->update_one2m_list );

	update_row->sql_statement_list =
		update_row_sql_statement_list(
			update_row->update_root,
			update_row->update_mto1_isa_list,
			update_row->update_one2m_list );

	update_row->command_line_list =
		update_row_command_line_list(
			update_row->update_root,
			update_row->update_mto1_isa_list,
			update_row->update_one2m_list );

	return update_row;
}

LIST *update_where_attribute_list(
			LIST *update_attribute_list )
{
	UPDATE_WHERE_ATTRIBUTE *where_attribute;
	LIST *where_attribut_list;
	UPDATE_ATTRIBUTE *update_attribute;

	if ( !list_rewind( update_attribute_list ) )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: update_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (LIST *)0;
	}

	where_attribute_list = list_new();

	do {
		update_attribute =
			list_get(
				update_attribute_list );

		if ( !update_attribute->folder_attribute )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_attribute is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( !update_attribute->file_data )
		{
			fprintf(stderr,
			"Warning in %s/%s()/%d: file_data is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			return (LIST *)0;
		}


		if ( update_attribute->folder_attribute->primary_key_index )
		{
			update_where_attribute =
				update_where_attribute_new(
					update_attribute->
						folder_attribute->
						attribute_name
						   /* primary_attribute_name */,
					update_attribute->file_data );

			list_set(
				where_attribute_list,
				update_where_attribute );
		}

	} while( list_next( update_attribute_list ) );

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
			char *primary_attribute_name,
			char *file_data );
{
	UPDATE_WHERE_ATTRIBUTE *update_where_attribute =
		update_where_attribute_calloc();

	update_where_attribute->primary_attribute_name =
		primary_attribute_name;

	update_where_attribute->file_data = file_data;

	return update_where_attribute;
}

int update_cell_count( UPDATE_ROW_LIST *update_row_list )
{
	int cells_updated = 0;
	UPDATE_ROW *update_row;

	if ( !list_rewind( update_row_list->list ) ) return 0;

	do {
		update_row = list_get( update_row_list->list );

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

	} while( list_next( update_row_list->list ) );

	return cells_updated;
}

int update_row_cell_count(
			int primary_cell_count,
			LIST *update_one2m_isa_list,
			LIST *update_mto1_list )
{
	UPDATE_ONE2M *update_one2m;
	UPDATE_MTO1_ISA *update_mto1_isa;
	int cell_count = primary_cell_count;

	if ( list_rewind( update_one2m_list ) )
	{
		do {
			update_one2m =
				list_get(
					update_one2m_list );

			cell_count +=
			list_length(
				update_one2m->
					update_attribute_changed_list );

		} while( list_next( update_one2m_list ) );
	}

	if ( list_rewind( update_mto1_isa_list ) )
	{
		do {
			update_mto1_isa =
				list_get(
					update_mto1_isa_list );

			cell_count +=
			list_length(
				update_mto1_isa->
					update_attribute_changed_list );

		} while( list_next( update_mto1_isa_list ) );
	}

	return cell_count;
}

UPDATE_ATTRIBUTE_CHANGED *update_attribute_changed_calloc( void )
{
	UPDATE_ATTRIBUTE_CHANGED *update_attribute_changed;

	if ( ! ( update_attribute_changed =
			calloc( 1, sizeof( UPDATE_ATTRIBUTE_CHANGED ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return update_attribute_changed;
}

UPDATE_ATTRIBUTE_CHANGED *update_attribute_changed_new(
			DICTIONARY *post_dictionary,
			char *attribute_name,
			char *datatype_name,
			int primary_key_index,
			char *file_data,
			int row )
{
	UPDATE_ATTRIBUTE_CHANGED *update_attribute_changed;
	char *post_data;
	boolean attribute_changed;

	if ( !file_data || !*file_data )
	{
		return (UPDATE_ATTRIBUTE_CHANGED *)0;
	}

	if ( ! ( post_data =
			dictionary_row_get(
				attribute_name,
				row,
				post_dictionary ) ) )
	{
		return (UPDATE_ATTRIBUTE_CHANGED *)0;
	}

	attribute_changed = ( strcasecmp( file_data, post_data ) != 0 );

	if ( !attribute_changed )
	{
		return (UPDATE_ATTRIBUTE_CHANGED *)0;
	}

	update_attribute_changed = update_attribute_changed_calloc();

	update_attribute_changed->attribute_name = attribute_name;
	update_attribute_changed->datatype_name = datatype_name;
	update_attribute_changed->primary_key_index = primary_key_index;
	update_attribute_changed->file_data = file_data;
	update_attribute_changed->post_data = post_data;
	update_attribute_changed->attribute_changed = attribute_changed;

	update_attribute_changed->sql_injection_escape_data =
		security_sql_injection_escape(
			security_replace_special_characters(
				string_trim_number_characters(
					post_data,
					datatype_name ) ) );

	return update_attribute_changed;
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

UPDATE_ATTRIBUTE *update_attribute_calloc( void )
{
	UPDATE_ATTRIBUTE *update_attribute;

	if ( ! ( update_attribute =
			calloc( 1, sizeof( UPDATE_ATTRIBUTE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return update_attribute;
}

UPDATE_ATTRIBUTE *update_attribute_new(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER_ATTRIBUTE *folder_attribute,
			int row )
{
	UPDATE_ATTRIBUTE *update_attribute;

	if ( !folder_attribute )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: folder_attribute is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_ATTRIBUTE *)0;
	}

	if ( !folder_attribute->attribute )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: attribute is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_ATTRIBUTE *)0;
	}

	update_attribute = update_attribute_calloc();

	update_attribute->folder_attribute = folder_attribute;
	update_attribute->row = row;

	if ( ! ( update_attribute->file_data =
			dictionary_row_data(
				folder_attribute->attribute_name,
				row,
				file_dictionary ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: dictionary_row_data(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			folder_attribute->attribute_name );
		return (UPDATE_ATTRIBUTE *)0;
	}

	if ( ! ( update_attribute->update_changed_attribute_data =
			update_changed_attribute_data_new(
				post_dictionary,
				folder_attribute->attribute_name,
				folder_attribute->attribute->datatype_name,
				update_attribute->file_data,
				row ) ) )
	{
		return update_attribute;
	}

	update_attribute->update_attribute_preupdate_label =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		update_attribute_preupdate_label(
			folder_attribute->attribute_name,
			row );

	dictionary_set(
		post_dictionary,
		update_attribute->update_attribute_preupdate_label,
		file_data );

	return update_attribute;
}

char *update_attribute_preupdate_label(
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

LIST *update_attribute_list(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			LIST *folder_attribute_list,
			int row )
{
	UPDATE_ATTRIBUTE *update_attribute;
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *attribute_list;

	if ( !list_rewind( folder_attribute_list ) )
	{
		fprintf(stderr,
		"Warning in %s/%s()/%d: folder_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (LIST *)0;
	}

	attribute_list = list_new();

	do {
		folder_attribute = list_get( folder_attribute_list );

		if ( ! ( update_attribute =
				update_attribute_new(
					post_dictionary,
					file_dictionary,
					folder_attribute,
					row ) ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: update_attribute_new(%s/%s) returned empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				folder_attribute->folder_name,
				folder_attribute->attribute_name );
			exit( 1 );
		}

		list_set( attribute_list, update_attribute );

	} while ( list_next( folder_attribute_list ) );

	return attribute_list;
}

UPDATE_MTO1_ISA *update_mto1_isa_calloc( void )
{
	UPDATE_MTO1_ISA *update_mto1_isa;

	if ( ! ( update_mto1_isa = calloc( 1, sizeof( UPDATE_MTO1_ISA ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return update_mto1_isa;
}

UPDATE_ONE2M *update_one2m_calloc( void )
{
	UPDATE_ONE2M *update_one2m;

	if ( ! ( update_one2m = calloc( 1, sizeof( UPDATE_ONE2M ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return update_one2m;
}

UPDATE_ONE2M *update_one2m_new(
			LIST *update_primary_changed_attribute_list,
			RELATION *relation_one2m,
			char *login_name )
{
	UPDATE_ONE2M *update_one2m;

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

	update_one2m = update_one2m_calloc();

	update_one2m->primary_key_changed_attribute_list =
		primary_key_changed_attribute_list;

	update_one2m->foreign_changed_attribute_list =
		update_foreign_changed_attribute_list(
			primary_key_changed_attribute_list,
			relation_one2m->many_folder->folder_name,
			relation_one2m->foreign_key_list );

	update_one2m->foreign_where_attribute_list =
		update_foreign_where_attribute_list(
			primary_key_changed_attribute_list,
			relation_one2m->foreign_key_list );

	update_one2m->sql_statement =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		update_one2m_sql_statement(
			/* ----------------------------- */
			/* Returns static memory or null */
			/* ----------------------------- */
			folder_table_name(
				environment_application_name(),
				update_one2m->many_folder->folder_name ),
			update_one2m->many_folder->primary_key_list
				/* attribute_name_list */,
			update_where_clause(
				update_one2m->foreign_where_attribute_list ) );

	return update_one2m;
}

LIST *update_mto1_isa_list(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			LIST *relation_mto1_isa_list,
			int row )
{
	RELATION *relation_mto1_isa;
	LIST *list;

	if ( !list_rewind( relation_mto1_isa_list ) ) return (LIST *)0;

	list = list_new();

	do {
		relation_mto1_isa =
			list_get(
				relation_mto1_isa_list );

		list_set(
			list,
			update_mto1_isa_new(
				post_dictionary,
				file_dictionary,
				login_name,
				relation_mto1_isa,
				row ) );

	} while ( list_next( relation_mto1_isa_list ) );

	return list;
}

UPDATE_MTO1_ISA *update_mto1_isa_new(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			RELATION *relation_mto1_isa,
			int row )
{
	UPDATE_MTO1_ISA *update_mto1_isa;

	if ( !dictionary_length( post_dictionary ) )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: post_dictionary is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_MTO1_ISA *)0;
	}

	if ( !dictionary_length( file_dictionary ) )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: file_dictionary is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_MTO1_ISA *)0;
	}

	if ( !login_name )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: login_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_MTO1_ISA *)0;
	}

	if ( !folder_name )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: folder_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_MTO1_ISA *)0;
	}

	if ( !list_length( relation_mto1_isa->
				one_folder->
				folder_attribute_list ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: folder_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_MTO1_ISA *)0;
	}

	if ( row <= 0 )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: row is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_MTO1_ISA *)0;
	}

	update_mto1_isa = update_mto1_isa_calloc();

	update_mto1_isa->update_attribute_list =
		update_attribute_list(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			post_dictionary,
			file_dictionary,
			relation_mto1_isa->one_folder->folder_attribute_list,
			row );

	update_mto1_isa->update_attribute_changed_list =
		update_attribute_changed_list(
			update_mto1_isa->update_attribute_list );

	if ( !list_length( update_mto1_isa->update_attribute_changed_list ) )
	{
		list_free_container( update_mto1_isa->update_attribute_list );
		free( update_mto1_isa );
		return (UPDATE_MTO1_ISA *)0;
	}

	update_mto1_isa->update_where_attribute_list =
		update_where_attribute_list(
			update_mto1_isa->update_attribute_list );

	update_mto1_isa->update_where_clause =
		update_where_clause(
			update_mto1_isa->update_where_attribute_list );

	update_mto1_isa->update_sql_statement =
		update_sql_statement(
			folder_table_name(
				environment_application_name(),
				relation_mto1_isa->one_folder->folder_name ),
			update_mto1_isa->update_attribute_changed_list,
			update_mto1_isa->update_where_clause );

	if ( update_mto1_isa->one_folder->post_change_process )
	{
		update_mto1_isa->update_command_line =
			update_command_line(
				update_mto1_isa->
					one_folder->
					post_change_process,
				login_name,
				update_mto1_isa->update_attribute_list );
	}

	return update_mto1_isa;
}

char *update_row_list_sql(
			char *login_name,
			SECURITY_ENTITY *security_entity,
			LIST *update_row_list )
{
	UPDATE_ROW *update_row;
	char return_message_list_string[ STRING_SYSTEM_BUFFER ];
	char *ptr = return_message_list_string;
	char *return_message;

	*ptr = '\0';

	if ( list_rewind( update_row_list ) )
	{
		do {
			update_row = list_get( update_row_list );

			return_message =
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
				update_row_sql(
					login_name,
					security_entity,
					update_row );

			if ( return_message )
			{
				ptr += sprintf( ptr, "%s;", return_message );
				free( return_message );
			}

		} while ( list_next( update_row_list ) );
	}

	if ( *return_message_list_string )
	{
		return strdup( return_message_list_string );
	}
	else
	{
		return (char *)0;
	}
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
		update_row_table_sql(
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

char *update_set_clause( LIST *update_attribute_changed_list )
{
}

char *update_where_clause(
			LIST *update_where_attribute_list )
{
}

UPDATE_ROW_LIST *update_row_list_calloc( void )
{
	UPDATE_ROW_LIST *update_row_list;

	if ( ! ( update_row_list =
			calloc( 1, sizeof( UPDATE_ROW_LIST ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return update_row_list;
}

UPDATE_ROW_LIST *update_row_list_new(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			char *folder_name,
			LIST *folder_attribute_list,
			LIST *relation_mto1_isa_list,
			LIST *relation_one2m_recursive_list,
			PROCESS *post_change_process,
			SECURITY_ENTITY *security_entity )
{
	UPDATE_ROW_LIST *update_row_list = update_row_list_calloc();
	int row;

	if ( ( update_row_list->dictionary_highest_row =
		dictionary_highest_row(
			post_dictionary ) ) == -1 )
	{
		free( update_row_list );
		return (UPDATE_ROW_LIST *)0;
	}

	update_row_list->list = list_new();

	for(	row = 1;
		row <= update_row_list->dictionary_highest_row;
		row++ )
	{
		list_set(
			update_row_list->list,
			update_row_new(
				/* ------------------------------ */
				/* Sets preupdate_$attribute_name */
				/* ------------------------------ */
				post_dictionary,
				file_dictionary,
				login_name,
				folder_attribute_list,
				relation_mto1_isa_list,
				relation_one2m_recursive_list,
				post_change_process,
				security_entity,
				row ) );
	}
	return update_row_list;
}

LIST *update_attribute_changed_list(
			LIST *update_attribute_list )
{
	LIST *attribute_changed_list = {0};
	UPDATE_ATTRIBUTE *update_attribute;

	if ( !list_rewind( update_attribute_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: update_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		update_attribute = list_get( update_attribute_list );

		if ( update_attribute->update_attribute_changed )
		{
			if ( !attribute_changed_list )
				attribute_changed_list =
					list_new();

			list_set(
				attribute_changed_list,
				update_attribute->update_attribute_changed );
		}

	} while ( list_next( update_attribute_list ) );

	return attribute_changed_list;
}

boolean update_attribute_changed_primary_key(
			LIST *update_attribute_changed_list )
{
	UPDATE_ATTRIBUTE_CHANGED *update_attribute_change;

	if ( !list_rewind( update_attribute_changed_list ) ) return 0;

	do {
		update_attribute_changed =
			list_get(
				update_attribute_changed_list );

		if ( update_attribute_changed->primary_key_index )
			return 1;

	} while ( list_next( update_attribute_changed_list ) );

	return 0;
}

