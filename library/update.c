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
#include "role_folder.h"
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
		role_fetch(
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
		security_entity_new(
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

char *update_where_clause( LIST *update_where_attribute_list )
{
	UPDATE_WHERE_ATTRIBUTE *update_where_attribute;
	char where_clause[ 4096 ];
	char *ptr = where_clause;

	if ( !list_rewind( update_where_attribute_list ) ) return (char *)0;

	ptr += sprintf( ptr, "where " );

	do {
		update_where_attribute =
			list_get(
				update_where_attribute_list );

		if ( !list_at_first( update_where_attribute_list ) )
		{
			ptr += sprintf( ptr, " and " );
		}

		if ( attribute_is_number(
			update_where_attribute->
				datatype_name ) )
		{
			ptr +=
			     sprintf(
				ptr, 
				"%s = %s",
				update_where_attribute->primary_attribute_name,
				update_where_attribute->
					sql_injection_escape_file_data );
		}
		else
		{
			ptr +=
			     sprintf(
				ptr, 
				"%s = '%s'",
				update_where_attribute->primary_attribute_name,
				update_where_attribute->
					sql_injection_escape_file_data );
		}

	} while( list_next( update_where_attribute_list ) );

	return strdup( where_clause );
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
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		update_where_clause(
			update_root->
				update_where_attribute_list );

	if ( !update_root->update_where_clause )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: update_where_clause() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_ROOT *)0;
	}

	update_root->update_root_where_clause =
		update_root_where_clause(
			update_root->update_where_clause,
			security_entity );

	update_root->update_sql_statement =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		update_sql_statement(
			folder_table_name(
				environment_application_name(),
				folder_name ),
			update_root->update_attribute_changed_list,
			update_root->update_root_where_clause );

	if ( !update_root->update_sql_statement )
	{
		return (UPDATE_ROOT *)0;
	}

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
				post_dictionary,
				file_dictionary,
				login_name,
				relation_one2m_recursive_list,
				row );
	}

	if ( !update_row->update_root
	&&   !update_row->update_mto1_isa_list
	&&   !update_row->update_one2m_list )
	{
		free( update_row );
		return (UPDATE_ROW *)0;
	}

	update_row->cell_count =
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
	UPDATE_WHERE_ATTRIBUTE *update_where_attribute;
	LIST *where_attribute_list;
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

		if ( !update_attribute->folder_attribute->primary_key_index )
		{
			continue;
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

		update_where_attribute =
			update_where_attribute_new(
				update_attribute->
					folder_attribute->
					attribute_name
					   /* primary_attribute_name */,
				update_attribute->
					folder_attribute->
						attribute->
						datatype_name,
				security_sql_injection_escape(
					update_attribute->file_data ) );

		list_set(
			where_attribute_list,
			update_where_attribute );

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
			char *datatype_name,
			char *sql_injection_escape_file_data )
{
	UPDATE_WHERE_ATTRIBUTE *update_where_attribute =
		update_where_attribute_calloc();

	update_where_attribute->primary_attribute_name =
		primary_attribute_name;

	update_where_attribute->datatype_name = datatype_name;

	update_where_attribute->sql_injection_escape_file_data =
		sql_injection_escape_file_data;

	return update_where_attribute;
}

int update_row_list_cell_count( UPDATE_ROW_LIST *update_row_list )
{
	int cell_count = 0;
	UPDATE_ROW *update_row;

	if ( !list_rewind( update_row_list->list ) ) return 0;

	do {
		update_row = list_get( update_row_list->list );

		if ( !update_row->cell_count )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: update_row->cell_count is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		cell_count += update_row->cell_count;

	} while( list_next( update_row_list->list ) );

	return cell_count;
}

int update_row_cell_count(
			UPDATE_ROOT *update_root,
			LIST *update_mto1_isa_list,
			LIST *update_one2m_list )
{
	UPDATE_MTO1_ISA *update_mto1_isa;
	UPDATE_ONE2M *update_one2m;
	int cell_count = 0;

	if ( update_root )
	{
		cell_count =
			list_length(
				update_root->
					update_attribute_changed_list );
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

	update_attribute_changed->sql_injection_escape_post_data =
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
			dictionary_row_get(
				folder_attribute->attribute_name,
				row,
				file_dictionary ) ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: dictionary_row_get(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			folder_attribute->attribute_name );
		return (UPDATE_ATTRIBUTE *)0;
	}

	update_attribute->sql_injection_escape_file_data =
		security_sql_injection_escape(
			update_attribute->file_data );

	if ( ! ( update_attribute->update_attribute_changed =
			update_attribute_changed_new(
				post_dictionary,
				folder_attribute->attribute_name,
				folder_attribute->attribute->datatype_name,
				folder_attribute->primary_key_index,
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
		update_attribute->sql_injection_escape_file_data );

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
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			RELATION *relation_one2m,
			int row )
{

	UPDATE_ONE2M *update_one2m;

	if ( !dictionary_length( post_dictionary ) )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: post_dictionary is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_ONE2M *)0;
	}

	if ( !dictionary_length( file_dictionary ) )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: file_dictionary is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_ONE2M *)0;
	}

	if ( !login_name )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: login_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_ONE2M *)0;
	}

	if ( !relation_one2m->many_folder )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: many_folder is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_ONE2M *)0;
	}

	if ( !list_length( relation_one2m->
				many_folder->
				folder_attribute_list ) )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: folder_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_ONE2M *)0;
	}

	if ( row <= 0 )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: row is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_ONE2M *)0;
	}

	update_one2m = update_one2m_calloc();

	update_one2m->update_attribute_list =
		update_attribute_list(
			/* ------------------------------------------ */
			/* Sets preupdate_$attribute_name for trigger */
			/* ------------------------------------------ */
			post_dictionary,
			file_dictionary,
			relation_one2m->many_folder->folder_attribute_list,
			row );

	update_one2m->update_attribute_changed_list =
		update_attribute_changed_list(
			update_one2m->update_attribute_list );

	if ( !list_length( update_one2m->update_attribute_changed_list ) )
	{
		list_free_container( update_one2m->update_attribute_list );
		free( update_one2m );
		return (UPDATE_ONE2M *)0;
	}

	update_one2m->update_where_attribute_list =
		update_where_attribute_list(
			update_one2m->update_attribute_list );

	update_one2m->update_where_clause =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		update_where_clause(
			update_one2m->
				update_where_attribute_list );

	if ( !update_one2m->update_where_clause )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: update_where_clause() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_ONE2M *)0;
	}

	update_one2m->update_sql_statement =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		update_sql_statement(
			folder_table_name(
				environment_application_name(),
				relation_one2m->many_folder->folder_name ),
			update_one2m->update_attribute_changed_list,
			update_one2m->update_where_clause );

	if ( !update_one2m->update_sql_statement )
	{
		return (UPDATE_ONE2M *)0;
	}

	if ( relation_one2m->many_folder->post_change_process )
	{
		update_one2m->update_command_line =
			update_command_line(
				relation_one2m->
					many_folder->
					post_change_process,
				login_name,
				update_one2m->update_attribute_list );
	}

	return update_one2m;
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

	if ( !relation_mto1_isa->one_folder )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: one_folder is empty.\n",
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
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		update_where_clause(
			update_mto1_isa->
				update_where_attribute_list );

	if ( !update_mto1_isa->update_where_clause )
	{
		fprintf(stderr,
	"Warning in %s/%s()/%d: update_where_clause() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE_MTO1_ISA *)0;
	}

	update_mto1_isa->update_sql_statement =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		update_sql_statement(
			folder_table_name(
				environment_application_name(),
				relation_mto1_isa->one_folder->folder_name ),
			update_mto1_isa->update_attribute_changed_list,
			update_mto1_isa->update_where_clause );

	if ( !update_mto1_isa->update_sql_statement )
	{
		return (UPDATE_MTO1_ISA *)0;
	}

	if ( relation_mto1_isa->one_folder->post_change_process )
	{
		update_mto1_isa->update_command_line =
			update_command_line(
				relation_mto1_isa->
					one_folder->
					post_change_process,
				login_name,
				update_mto1_isa->update_attribute_list );
	}

	return update_mto1_isa;
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
				folder_name,
				folder_attribute_list,
				relation_mto1_isa_list,
				relation_one2m_recursive_list,
				post_change_process,
				security_entity,
				row ) );
	}

	update_row_list->cell_count =
		update_row_list_cell_count(
			update_row_list );

	update_row_list->command_line_list =
		update_row_list_command_line_list(
			update_row_list );

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
	UPDATE_ATTRIBUTE_CHANGED *update_attribute_changed;

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

LIST *update_mto1_isa_list(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			LIST *relation_mto1_isa_list,
			int row )
{
	LIST *list = {0};
	RELATION *relation_mto1_isa;

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

LIST *update_one2m_list(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			LIST *relation_one2m_recursive_list,
			int row )
{
	LIST *list = {0};
	RELATION *relation_one2m;

	if ( !list_rewind( relation_one2m_recursive_list ) ) return (LIST *)0;

	list = list_new();

	do {
		relation_one2m =
			list_get(
				relation_one2m_recursive_list );

		list_set(
			list,
			update_mto1_isa_new(
				post_dictionary,
				file_dictionary,
				login_name,
				relation_one2m,
				row ) );

	} while ( list_next( relation_one2m_recursive_list ) );

	return list;
}

char *update_sql_statement(
			char *folder_table_name,
			LIST *update_attribute_changed_list,
			char *update_where_clause )
{
	char sql_statement[ STRING_INPUT_BUFFER ];
	char *ptr = sql_statement;
	UPDATE_ATTRIBUTE_CHANGED *update_attribute_changed;

	if ( !list_rewind( update_attribute_changed_list ) )
		return (char *)0;

	ptr += sprintf(
		ptr,
		"update %s set",
		folder_table_name );

	do {
		update_attribute_changed =
			list_get(
				update_attribute_changed_list );

		if ( !update_attribute_changed->
			sql_injection_escape_post_data
		||   !*update_attribute_changed->
			sql_injection_escape_post_data )
		{
			fprintf(stderr,
	"Warning in %s/%s()/%d: sql_injection_escape_post_data is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			return (char *)0;
		}

		if ( !list_at_first( update_attribute_changed_list ) )
		{
			ptr += sprintf( ptr, ", " );
		}

		if ( strcmp(
			update_attribute_changed->
				sql_injection_escape_post_data,
			"/" ) == 0 )
		{
			ptr += sprintf(
				ptr,
				"%s is null",
				update_attribute_changed->
					attribute_name );
		}
		else
		if ( attribute_is_number(
			update_attribute_changed->
				datatype_name ) )
		{
			ptr += sprintf(
				ptr,
				"%s = %s",
				update_attribute_changed->
					attribute_name,
				update_attribute_changed->
					sql_injection_escape_post_data );
		}
		else
		{
			ptr += sprintf(
				ptr,
				"%s = '%s'",
				update_attribute_changed->
					attribute_name,
				update_attribute_changed->
					sql_injection_escape_post_data );
		}

	} while ( list_next( update_attribute_changed_list ) );

	ptr += sprintf( ptr, " %s", update_where_clause );

	return strdup( sql_statement );
}

char *update_root_where_clause(
			char *update_where_clause,
			SECURITY_ENTITY *security_entity )
{
	char *where;

	if ( !security_entity )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: security_entity is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ( where = security_entity_where( security_entity ) ) )
	{
		char where_clause[ 4096 ];

		sprintf(where_clause,
			"%s and %s",
			update_where_clause,
			where );

		free( update_where_clause );
		return strdup( where_clause );
	}
	else
	{
		return update_where_clause;
	}
}

char *update_execute( char *sql_statement )
{
	char system_string[ 1024 ];

	if ( !sql_statement || !*sql_statement )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: sql_statement is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(system_string,
		"echo \"%s\" | sql 2>&1",
		sql_statement );

	return string_pipe_fetch( system_string );
}

void update_sql_statement_list_execute(
			FILE *update_sql_pipe,
			LIST *sql_statement_list )
{
	char *sql_statement;

	if ( !list_rewind( sql_statement_list ) ) return;

	do {
		sql_statement =
			list_get(
				sql_statement_list );

		fprintf(update_sql_pipe,
			"%s\n",
			sql_statement );

	} while ( list_next( sql_statement_list ) );
}

LIST *update_row_sql_statement_list(
			UPDATE_ROOT *update_root,
			LIST *update_mto1_isa_list,
			LIST *update_one2m_list )
{
	LIST *sql_statement_list = list_new();

	if ( update_root )
	{
		if ( !update_root->update_sql_statement )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: update_sql_statement is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			sql_statement_list,
			update_root->update_sql_statement );
	}

	if ( list_length( update_mto1_isa_list ) )
	{
		list_append_list(
			sql_statement_list,
			update_mto1_isa_sql_statement_list(
				update_mto1_isa_list ) );
	}

	if ( list_length( update_one2m_list ) )
	{
		list_append_list(
			sql_statement_list,
			update_one2m_sql_statement_list(
				update_one2m_list ) );
	}

	return sql_statement_list;
}

LIST *update_row_command_line_list(
			UPDATE_ROOT *update_root,
			LIST *update_mto1_isa_list,
			LIST *update_one2m_list )
{
	LIST *command_line_list = list_new();

	if ( update_root )
	{
		list_set(
			command_line_list,
			update_root->update_command_line );
	}

	if ( list_length( update_mto1_isa_list ) )
	{
		list_append_list(
			command_line_list,
			update_mto1_isa_command_line_list(
				update_mto1_isa_list ) );
	}

	if ( list_length( update_one2m_list ) )
	{
		list_append_list(
			command_line_list,
			update_one2m_command_line_list(
				update_one2m_list ) );
	}

	return command_line_list;
}

LIST *update_mto1_isa_sql_statement_list(
			LIST *update_mto1_isa_list )
{
	UPDATE_MTO1_ISA *update_mto1_isa;
	LIST *sql_statement_list;

	if ( !list_rewind( update_mto1_isa_list ) ) return (LIST *)0;

	sql_statement_list = list_new();

	do {
		update_mto1_isa =
			list_get(
				update_mto1_isa_list );

		if ( !update_mto1_isa->update_sql_statement )
		{
			fprintf(stderr,
		"Warning in %s/%s()/%d: update_sql_statement is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			continue;
		}

		list_set(
			sql_statement_list,
			update_mto1_isa->update_sql_statement );

	} while ( list_next( update_mto1_isa_list ) );

	return sql_statement_list;
}

LIST *update_one2m_sql_statement_list(
			LIST *update_one2m_list )
{
	UPDATE_ONE2M *update_one2m;
	LIST *sql_statement_list;

	if ( !list_rewind( update_one2m_list ) ) return (LIST *)0;

	sql_statement_list = list_new();

	do {
		update_one2m =
			list_get(
				update_one2m_list );

		if ( !update_one2m->update_sql_statement )
		{
			fprintf(stderr,
		"Warning in %s/%s()/%d: update_sql_statement is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			continue;
		}

		list_set(
			sql_statement_list,
			update_one2m->update_sql_statement );

	} while ( list_next( update_one2m_list ) );

	return sql_statement_list;
}

LIST *update_one2m_command_line_list(
			LIST *update_one2m_list )
{
	UPDATE_ONE2M *update_one2m;
	LIST *command_line_list = {0};

	if ( !list_rewind( update_one2m_list ) ) return (LIST *)0;

	do {
		update_one2m =
			list_get(
				update_one2m_list );

		if ( update_one2m->update_command_line )
		{
			if ( !command_line_list )
				command_line_list =
					list_new();

			list_set(
				command_line_list,
				update_one2m->update_command_line );
		}

	} while ( list_next( update_one2m_list ) );

	return command_line_list;
}

LIST *update_mto1_isa_command_line_list(
			LIST *update_mto1_isa_list )
{
	UPDATE_MTO1_ISA *update_mto1_isa;
	LIST *command_line_list = {0};

	if ( !list_rewind( update_mto1_isa_list ) ) return (LIST *)0;

	do {
		update_mto1_isa =
			list_get(
				update_mto1_isa_list );

		if ( update_mto1_isa->update_command_line )
		{
			if ( !command_line_list )
				command_line_list =
					list_new();

			list_set(
				command_line_list,
				update_mto1_isa->update_command_line );
		}

	} while ( list_next( update_mto1_isa_list ) );

	return command_line_list;
}

LIST *update_row_list_sql_statement_list(
			UPDATE_ROW_LIST *update_row_list )
{
	UPDATE_ROW *update_row;
	LIST *sql_statement_list;

	if ( !update_row_list )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: update_row_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( update_row_list->list ) ) return (LIST *)0;

	sql_statement_list = list_new();

	do {
		update_row =
			list_get(
				update_row_list->list );

		list_set_list(
			sql_statement_list,
			update_row->sql_statement_list );

	} while ( list_next( update_row_list->list ) );

	return sql_statement_list;
}

LIST *update_row_list_command_line_list(
			UPDATE_ROW_LIST *update_row_list )
{
	LIST *command_line_list;
	UPDATE_ROW *update_row;

	if ( !update_row_list )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: update_row_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( update_row_list->list ) ) return (LIST *)0;

	command_line_list = list_new();

	do {
		update_row =
			list_get(
				update_row_list->list );

		list_set_list(
			command_line_list,
			update_row->command_line_list );

	} while ( list_next( update_row_list->list ) );

	return command_line_list;
}

void update_row_list_command_line_execute(
			LIST *command_line_list )
{
	char *command_line;

	if ( !list_rewind( command_line_list ) ) return;

	do {
		command_line =
			list_get(
				command_line_list );

		if ( system( command_line ) ){};

	} while ( list_next( command_line_list ) );
}

char *update_row_list_execute(
			UPDATE_ROW_LIST *update_row_list )
{
	char *message_list_string = {0};
	UPDATE_ROW *update_row;
	char *message_string;
	FILE *sql_pipe;

	if ( !update_row_list )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: update_row_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( update_row_list->list ) ) return (char *)0;

	sql_pipe = update_sql_pipe( environment_application_name() );

	do {
		update_row = list_get( update_row_list->list );

		message_string = (char *)0;

		if ( update_row->update_root )
		{
			if ( ( message_string =
				/* ------------------------------------- */
				/* Returns message_string as heap memory */
				/* ------------------------------------- */
				update_execute(
					update_row->
						update_root->
						update_sql_statement ) ) )
			{
				message_list_string =
					string_append(
						message_list_string,
						message_string,
						"<br>" );
			}
		}

		if ( message_string ) continue;

		if ( list_length( update_row->update_mto1_isa_list ) )
		{
			update_sql_statement_list_execute(
				sql_pipe,
				update_mto1_isa_sql_statement_list(
					update_row->update_mto1_isa_list ) );
		}

		if ( update_row->update_root->changed_primary_key
		&&   list_length( update_row->update_one2m_list ) )
		{
			update_sql_statement_list_execute(
				sql_pipe,
				update_one2m_sql_statement_list(
					update_row->update_one2m_list ) );
		}

		if ( list_length( update_row->command_line_list ) )
		{
			if ( ( message_string =
				update_command_line_list_execute(
					update_row->command_line_list ) ) )
			{
				message_list_string =
					string_append(
						message_list_string,
						message_string,
						"<br>" );
			}
		}

	} while ( list_next( update_row_list->list ) );

	pclose( sql_pipe );
	return message_list_string;
}

FILE *update_sql_pipe( char *application_name )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"sql 2>>%s",
		appaserver_error_filename(
			application_name ) );

	return popen( system_string, "w" );
}

char *update_command_line(
			PROCESS *post_change_process,
			char *login_name,
			LIST *update_attribute_list )
{
	char command_line[ STRING_INPUT_BUFFER ];
	char buffer[ 1024 ];
	UPDATE_ATTRIBUTE *update_attribute;

	if ( !post_change_process ) return (char *)0;

	if ( !post_change_process->command_line )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: command_line is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !login_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: login_name empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( update_attribute_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: update_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	string_strcpy(
		command_line,
		post_change_process->command_line,
		STRING_INPUT_BUFFER );

	string_search_replace(
		command_line,
		"$login_name",
		double_quotes_around(
			buffer,
			login_name ) );

	string_search_replace(
		command_line,
		"$state",
		double_quotes_around(
			buffer,
			"update" ) );

	do {
		update_attribute =
			list_get(
				update_attribute_list );

		if ( !update_attribute->sql_injection_escape_file_data )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: sql_injection_escape_file_data is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		string_search_replace(
			command_line,
			update_attribute->folder_attribute->attribute_name,
			double_quotes_around(
				buffer,
				update_attribute->
					sql_injection_escape_file_data ) );

	} while ( list_next( update_attribute_list ) );

	return strdup( command_line );
}

char *update_command_line_list_execute(
			LIST *command_line_list )
{
	char *message_string;
	char *message_list_string = {0};

	if ( !list_rewind( command_line_list ) ) return (char *)0;

	do {
		if ( ( message_string =
			string_pipe_fetch(
				list_get(
					command_line_list ) ) ) )
		{
			message_list_string =
				string_append(
					message_list_string,
					message_string,
					"<br>" );
		}
	} while ( list_next( command_line_list ) );
					
	return message_list_string;
}

