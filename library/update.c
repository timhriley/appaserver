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
#include "appaserver_error.h"
#include "folder_attribute.h"
#include "attribute.h"
#include "folder.h"
#include "role_folder.h"
#include "relation.h"
#include "appaserver_library.h"
#include "appaserver_user.h"
#include "sql.h"
#include "appaserver_parameter.h"
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

UPDATE *update_new(	
			char *application_name,
			char *login_name,
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			ROLE *role,
			FOLDER *folder )
{
	UPDATE *update;

	if ( !application_name
	||   !login_name
	||   !role
	||   !folder )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (UPDATE *)0;
	}

	if ( !post_dictionary ) return (UPDATE *)0;

	if ( !file_dictionary ) return (UPDATE *)0;

	if ( !role_folder_update(
		folder->role_folder_list ) )
	{
		return (UPDATE *)0;
	}

 	update = update_calloc();

	update->security_entity =
		security_entity_new(
			login_name,
			folder->non_owner_forbid,
			role->override_row_restrictions );

	update->update_row_list =
		/* -------------------------- */
		/* Returns null if no updates */
		/* -------------------------- */
		update_row_list_new(
			application_name,
			login_name,
			post_dictionary,
			file_dictionary,
			folder->folder_name,
			folder->primary_key_list,
			folder->folder_attribute_list,
			folder->relation_one2m_recursive_list,
			folder->relation_mto1_isa_list,
			folder->post_change_process,
			security_entity );

	if ( update->update_row_list )
	{
		update->sql_statement_list =
			update_sql_statement_list_new(
				update->update_row_list );
	}

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
			char *application_name,
			char *login_name,
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *folder_name,
			LIST *primary_key_list,
			LIST *folder_attribute_list,
			PROCESS *post_change_process,
			SECURITY_ENTITY *security_entity,
			int row )
{
	UPDATE_ROOT *update_root;
	char *tmp;

	if ( !application_name
	||   !login_name
	||   !folder_name
	||   !list_length( folder_attribute_list )
	||   row <= 0 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !dictionary_length( post_dictionary ) ) return (UPDATE_ROOT *)0;
	if ( !dictionary_length( file_dictionary ) ) return (UPDATE_ROOT *)0;

	update_root = update_root_calloc();

	update_root->update_changed_list =
		update_changed_list(
			post_dictionary,
			file_dictionary,
			folder_attribute_list,
			row );

	if ( !list_length( update_root->update_changed_list ) )
	{
		free( update_root );
		return (UPDATE_ROOT *)0;
	}

	update_root->update_where_list =
		update_where_list(
			file_dictionary,
			primary_key_list,
			folder_attribute_list,
			row );

	if ( !list_length( update_root->update_where_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: update_where_list() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	update_root->where_clause =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		update_root_where_clause(
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			(  tmp =
				update_where_list_clause(
					update_root->update_where_list ) ),
			security_entity,
			folder_attribute_list );

	free( tmp );

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
				post_change_process->command_line,
				login_name,
				update_root->update_attribute_list );
	}

	update_root->changed_primary_key =
		update_attribute_changed_primary_key(
			update_root->update_attribute_changed_list );

	return update_root;
}

UPDATE_ROW *update_row_new(
			char *application_name,
			char *login_name,
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *folder_name,
			LIST *primary_key_list,
			LIST *folder_attribute_list,
			LIST *relation_one2m_recursive_list,
			LIST *relation_mto1_isa_list,
			PROCESS *post_change_process,
			SECURITY_ENTITY *security_entity,
			int row )
{
	UPDATE_ROW *update_row = update_row_calloc();

	update_row->row = row;

	update_row->update_root =
		update_root_new(
			application_name,
			login_name,
			post_dictionary,
			file_dictionary,
			folder_name,
			primary_key_list,
			folder_attribute_list,
			post_change_process,
			security_entity,
			row );

	if ( update_row->update_root
	&&   update_row->update_root->changed_primary_key
	&&   list_length( relation_one2m_recursive_list ) )
	{
		update_row->update_one2m_list =
			update_one2m_list(
				application_name,
				login_name,
				post_dictionary,
				file_dictionary,
				relation_one2m_recursive_list,
				row );
	}

	if ( list_length( relation_mto1_isa_list ) )
	{
		update_row->update_mto1_isa_list =
			update_mto1_isa_list(
				application_name,
				login_name,
				post_dictionary,
				file_dictionary,
				relation_mto1_isa_list,
				row,
				(update_row->update_root)
					? update_row->
						update_root->
						changed_primary_key
					: 0 );
	}

	if ( !update_row->update_root
	&&   !update_row->update_one2m_list
	&&   !update_row->update_mto1_isa_list )
	{
		free( update_row );
		return (UPDATE_ROW *)0;
	}

	update_row->cell_count =
		update_row_cell_count(
			update_row->update_root,
			update_row->update_one2m_list,
			update_row->update_mto1_isa_list );

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
				/* --------------------------- */
				/* Returns heap memory or null */
				/* --------------------------- */
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

int update_row_list_cell_count( LIST *update_row_list )
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
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
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

	update_attribute = update_attribute_calloc();

	update_attribute->folder_attribute = folder_attribute;
	update_attribute->row = row;

	if ( ! ( update_attribute->file_data =
			dictionary_row_get(
				folder_attribute->attribute_name,
				row,
				file_dictionary ) ) )
	{
		free( update_attribute );
		return (UPDATE_ATTRIBUTE *)0;
	}

	update_attribute->sql_injection_escape_file_data =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
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

		if ( ( update_attribute =
				update_attribute_new(
					post_dictionary,
					file_dictionary,
					folder_attribute,
					row ) ) )
		{
			list_set( attribute_list, update_attribute );
		}

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

	if ( !login_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: login_name is empty.\n",
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

	if ( !list_length( relation_one2m->
				many_folder->
				folder_attribute_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: folder_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( row <= 0 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: row is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !dictionary_length( post_dictionary ) )
	{
		return (UPDATE_ONE2M *)0;
	}

	if ( !dictionary_length( file_dictionary ) )
	{
		return (UPDATE_ONE2M *)0;
	}

	update_one2m = update_one2m_calloc();

	update_one2m->update_attribute_list =
		update_attribute_list(
			post_dictionary,
			file_dictionary,
			relation_one2m->many_folder->folder_attribute_list,
			row );

	if ( !list_length( update_one2m->update_attribute_list ) )
	{
		free( update_one2m );
		return (UPDATE_ONE2M *)0;
	}

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

	if ( !list_length( update_one2m->update_where_attribute_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: update_where_attribute_list() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	update_one2m->primary_delimited_list =
		update_one2m_primary_delimited_list(
			folder_table_name(
				environment_application_name(),
				relation_one2m->many_folder->folder_name ),
			relation_one2m->many_folder->primary_key_list,
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			update_where_clause(
				update_one2m->
					update_where_attribute_list ) );

	if ( !list_length( update_one2m->primary_delimited_list ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: update_one2m_primary_delimited_list() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	update_one2m->sql_statement_list =
		update_one2m_sql_statement_list(
			folder_table_name(
				environment_application_name(),
				relation_one2m->many_folder->folder_name ),
			update_one2m->update_attribute_changed_list,
			relation_one2m->many_folder->primary_key_list,
			update_one2m->primary_delimited_list );

	if ( !list_length( update_one2m->sql_statement_list ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: update_one2m_sql_statement_list() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( relation_one2m->many_folder->post_change_process )
	{
		update_one2m->command_line_list =
			update_one2m_command_line_list(
				relation_one2m->
					many_folder->
					post_change_process->
					command_line,
				login_name,
				relation_one2m->many_folder->primary_key_list,
				update_one2m->primary_delimited_list,
				update_one2m->update_attribute_changed_list );
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
					post_change_process->
					command_line,
				login_name,
				update_mto1_isa->update_attribute_list );
	}

	update_mto1_isa->one2m_list =
		update_mto1_isa_one2m_list(
			post_dictionary,
			file_dictionary,
			login_name,
			relation_mto1_isa->
				one_folder->
				folder_name,
			row );

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
			char *application_name,
			char *login_name,
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *folder_name,
			LIST *primary_key_list,
			LIST *folder_attribute_list,
			LIST *relation_one2m_recursive_list,
			LIST *relation_mto1_isa_list,
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
				application_name,
				login_name,
				post_dictionary,
				file_dictionary,
				folder_name,
				primary_key_list,
				folder_attribute_list,
				relation_one2m_recursive_list,
				relation_mto1_isa_list,
				post_change_process,
				security_entity,
				row ) );
	}

	if ( list_length( update_row_list->list ) )
	{
		update_row_list->cell_count =
			update_row_list_cell_count(
				update_row_list->list );
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
			update_one2m_new(
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
			char *set_clause,
			char *where_clause )
{
	char sql_statement[ STRING_64K ];

	if ( !folder_table_name
	||   !set_clause
	||   !where_clause )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	sprintf(sql_statement,
		"update %s set %s where %s",
		folder_table_name,
		set_clause,
		where_clause );

	return strdup( sql_statement );
}

char *update_root_where_clause(
			char *update_where_clause,
			SECURITY_ENTITY *security_entity,
			LIST *folder_attribute_list )
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

	if ( ( where =
			security_entity_where(
				security_entity,
				folder_attribute_list ) ) )
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
			update_one2m_list_sql_statement_list(
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
			update_one2m_list_command_line_list(
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

		if ( list_length( update_mto1_isa->one2m_list ) )
		{
			list_set_list(
				sql_statement_list,
				update_mto1_isa_one2m_sql_statement_list(
					update_mto1_isa->one2m_list ) );
		}

	} while ( list_next( update_mto1_isa_list ) );

	return sql_statement_list;
}

LIST *update_one2m_list_sql_statement_list(
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

		list_set_list(
			sql_statement_list,
			update_one2m->sql_statement_list );

	} while ( list_next( update_one2m_list ) );

	return sql_statement_list;
}

LIST *update_one2m_list_command_line_list(
			LIST *update_one2m_list )
{
	UPDATE_ONE2M *update_one2m;
	LIST *command_line_list = {0};

	if ( !list_rewind( update_one2m_list ) ) return (LIST *)0;

	do {
		update_one2m =
			list_get(
				update_one2m_list );


		if ( list_length( update_one2m->command_line_list ) )
		{
			list_set_list(
				command_line_list,
				update_one2m->command_line_list );
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

		if ( list_length( update_mto1_isa->one2m_list ) )
		{
			if ( !command_line_list )
				command_line_list =
					list_new();

			list_set_list(
				command_line_list,
				update_mto1_isa_one2m_command_line_list(
					update_mto1_isa->one2m_list ) );
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
				update_one2m_list_sql_statement_list(
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

FILE *update_non_root_sql_pipe( char *appaserver_error_filename )
{
	char system_string[ 1024 ];

	sprintf(system_string,
		"sql 2>>%s",
		appaserver_error_filename );

	return popen( system_string, "w" );
}

char *update_command_line(
			char *command_line,
			char *login_name,
			LIST *update_attribute_list )
{
	char line[ STRING_INPUT_BUFFER ];
	char buffer[ 1024 ];
	UPDATE_ATTRIBUTE *update_attribute;

	if ( !command_line )
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

	string_strcpy( line, command_line, STRING_INPUT_BUFFER );

	string_search_replace(
		line,
		"$login_name",
		double_quotes_around(
			buffer,
			login_name ) );

	string_search_replace(
		line,
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
			line,
			update_attribute->folder_attribute->attribute_name,
			double_quotes_around(
				buffer,
				update_attribute->
					sql_injection_escape_file_data ) );

	} while ( list_next( update_attribute_list ) );

	return strdup( line );
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

LIST *update_mto1_isa_one2m_list(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			char *login_name,
			char *folder_name,
			int row )
{
	FOLDER *folder;

	if ( ! ( folder =
			folder_fetch(
				folder_name,
				(char *)0 /* role_name */,
				(LIST *)0 /* role_exclude_attribute_name_list*/,
				/* Also sets primary_key_list */
				1 /* fetch_folder_attribute_list */,
				0 /* not fetch_relation_mto1_non_isa_list */,
				/* Also sets folder_attribute_append_isa_list */
				0 /* not fetch_relation_mto1_isa_list */,
				0 /* not fetch_relation_one2m_list */,
				1 /* fetch_relation_one2m_recursive_list */,
				1 /* fetch_process */,
				0 /* not fetch_role_folder_list */,
				0 /* not fetch_row_level_restriction */,
				0 /* not fetch_role_operation_list */ ) ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			folder_name );
		exit( 1 );
	}


	return
	update_one2m_list(
		post_dictionary,
		file_dictionary,
		login_name,
		folder->relation_one2m_recursive_list,
		row );
}

LIST *update_mto1_isa_one2m_sql_statement_list(
			LIST *update_one2m_list )
{
	if ( !list_length( update_one2m_list ) ) return (LIST *)0;

	return
	update_one2m_list_sql_statement_list(
		update_one2m_list );
}

LIST *update_mto1_isa_one2m_command_line_list(
			LIST *update_one2m_list )
{
	if ( !list_length( update_one2m_list ) ) return (LIST *)0;

	return
	update_one2m_list_command_line_list(
		update_one2m_list );
}

LIST *update_one2m_primary_delimited_list(
			char *folder_table_name,
			LIST *primary_key_list,
			char *update_where_clause )
{
	char system_string[ STRING_INPUT_BUFFER ];

	sprintf(system_string,
		"select.sh \"%s\" %s \"%s\"",
		list_display_delimited(
			primary_key_list, ',' ),
		folder_table_name,
		update_where_clause );

	return list_pipe_fetch( system_string );
}

char *update_one2m_sql_statement(
			char *folder_table_name,
			LIST *update_attribute_changed_list,
			LIST *primary_key_list,
			LIST *primary_data_list )
{
	UPDATE_ATTRIBUTE_CHANGED *update_attribute_changed;
	char sql_statement[ 1024 ];
	char *ptr = sql_statement;

	if ( !list_rewind( update_attribute_changed_list ) )
		return (char *)0;

	if ( !list_length( primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_length( primary_key_list ) !=
	     list_length( primary_data_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: length:%d != length: %d.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_length( primary_key_list ),
			list_length( primary_data_list ) );
		exit( 1 );
	}

	ptr += sprintf(
		ptr,
		"update %s set ",
		folder_table_name );

	do {
		update_attribute_changed =
			list_get(
				update_attribute_changed_list );

		if ( !list_at_first( update_attribute_changed_list ) )
			ptr += sprintf( ptr, "," );

		if ( attribute_is_number(
			update_attribute_changed->
				datatype_name ) )
		{
			ptr += sprintf(
				ptr,
				"%s = %s",
				update_attribute_changed->attribute_name,
				update_attribute_changed->
					sql_injection_escape_post_data );
		}
		else
		{
			ptr += sprintf(
				ptr,
				"%s = '%s'",
				update_attribute_changed->attribute_name,
				update_attribute_changed->
					sql_injection_escape_post_data );
		}

	} while ( list_next( update_attribute_changed_list ) );

	ptr += sprintf(
		ptr,
		" where " );

	list_rewind( primary_key_list );
	list_rewind( primary_data_list );

	do {
		if ( !list_at_first( primary_key_list  ) )
			ptr += sprintf( ptr, " and " );

		ptr += sprintf(
			ptr,
			" %s = '%s'",
			(char *)list_get( primary_key_list ),
			(char *)list_get( primary_data_list ) );

		list_next( primary_key_list );

	} while ( list_next( primary_key_list ) );

	ptr += sprintf( ptr, ";" );

	return strdup( sql_statement );
}

char *update_one2m_command_line(
			char *command_line,
			char *login_name,
			LIST *primary_key_list,
			LIST *primary_data_list,
			LIST *update_attribute_changed_list )
{
	char line[ 1024 ];
	char buffer[ 128 ];
	char preupdate_attribute_name[ 128 ];
	char *primary_key;
	UPDATE_ATTRIBUTE_CHANGED *update_attribute_changed;

	if ( !list_rewind( primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( list_length( primary_key_list ) !=
	     list_length( primary_data_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: length:%d != length:%d.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			list_length( primary_key_list ),
			list_length( primary_data_list ) );
		exit( 1 );
	}

	string_strcpy( line, command_line, 1024 );
	list_rewind( primary_data_list );

	do {
		primary_key = list_get( primary_key_list );

		if ( ( update_attribute_changed =
			update_attribute_changed_seek(
				primary_key,
				update_attribute_changed_list ) ) )
		{
			string_search_replace(
				line,
				primary_key,
				double_quotes_around(
					buffer,
					update_attribute_changed->
					     sql_injection_escape_post_data ) );
		}
		else
		{
			string_search_replace(
				line,
				primary_key,
				double_quotes_around(
					buffer,
					(char *)
					   list_get( primary_data_list ) ) );
		}

		list_next( primary_key_list );

	} while ( list_next( primary_data_list ) );

	string_search_replace(
		line,
		"$login_name",
		double_quotes_around(
			buffer,
			login_name ) );

	string_search_replace(
		line,
		"$state",
		double_quotes_around(
			buffer,
			"update" ) );

	list_rewind( update_attribute_changed_list );

	do {
		update_attribute_changed =
			list_get(
				update_attribute_changed_list );

		sprintf(preupdate_attribute_name,
			"%s%s",
			UPDATE_PREUPDATE_PREFIX,
			update_attribute_changed->attribute_name );

		string_search_replace(
			line,
			preupdate_attribute_name,
			double_quotes_around(
				buffer,
				update_attribute_changed->file_data ) );

	} while ( list_next( update_attribute_changed_list ) );

	return strdup( line );
}

LIST *update_one2m_sql_statement_list(
			char *folder_table_name,
			LIST *update_attribute_changed_list,
			LIST *primary_key_list,
			LIST *primary_delimited_list )
{
	LIST *sql_statement_list;

	if ( !list_rewind( primary_delimited_list ) ) return (LIST *)0;

	sql_statement_list = list_new();

	do {
		list_set(
			sql_statement_list,
			/* -------------------------- */
			/* Safely returns heap memory */
			/* -------------------------- */
			update_one2m_sql_statement(
				folder_table_name,
				update_attribute_changed_list,
				primary_key_list,
				list_string_extract(
					(char *)list_get(
						primary_delimited_list ),
					SQL_DELIMITER )
					/* primary_data_list */ ) );
	
	} while ( list_next( primary_delimited_list ) );

	return sql_statement_list;
}

UPDATE_ATTRIBUTE_CHANGED *update_attribute_changed_seek(
			char *attribute_name,
			LIST *update_attribute_changed_list )
{
	UPDATE_ATTRIBUTE_CHANGED *update_attribute_changed;

	if ( !list_rewind( update_attribute_changed_list ) )
		return (UPDATE_ATTRIBUTE_CHANGED *)0;

	do {
		update_attribute_changed =
			list_get(
				update_attribute_changed_list );

		if ( strcmp(
			update_attribute_changed->attribute_name,
			attribute_name ) == 0 )
		{
			return update_attribute_changed;
		}

	} while ( list_next( update_attribute_changed_list ) );

	return (UPDATE_ATTRIBUTE_CHANGED *)0;
}

LIST *update_one2m_command_line_list(
			char *command_line,
			char *login_name,
			LIST *primary_key_list,
			LIST *primary_delimited_list,
			LIST *update_attribute_changed_list )
{
	LIST *command_line_list;

	if ( !command_line ) return (LIST *)0;

	if ( !list_rewind( primary_delimited_list ) ) return (LIST *)0;

	command_line_list = list_new();

	do {
		list_set(
			command_line_list,
			update_one2m_command_line(
				command_line,
				login_name,
				primary_key_list,
				list_string_extract(
					list_get( primary_delimited_list ),
					SQL_DELIMITER ),
				update_attribute_changed_list ) );

	} while ( list_next( primary_delimited_list ) );

	return command_line_list;
}

LIST *update_where_list(
			DICTIONARY *file_dictionary,
			LIST *key_list,
			LIST *folder_attribute_list,
			int row )
{
	LIST *where_list;

	if ( !list_rewind( key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	where_list = list_new();

	do {
		list_set(
			where_list,
			update_where_new(
				file_dictionary,
				(char *)list_get( key_list ),
				folder_attribute_list,
				row ) );

	} while ( list_next( key_list ) );

	return where_list;
}

UPDATE_WHERE *update_where_new(
			DICTIONARY *file_dictionary,
			char *key,
			LIST *folder_attribute_list,
			int row )
{
	UPDATE_WHERE *update_where = update_where_calloc();

	update_where->file_data =
		dictionary_row(
			key,
			row,
			file_dictionary );

	if ( !update_where->file_data )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: dictionary_row(%s/%d) returned empty for file_dictionary.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			key,
			row,
			dictionary_display_delimiter(
				file_dictionary,
				SQL_DELIMITER ) );
		exit( 1 );
	}

	update_where->sql_injection_escape_file_data =
		security_sql_injection_escape(
			update_where->file_data );

	update_where->folder_attribute =
		folder_attribute_seek(
			key,
			folder_attribute_list );

	if ( !update_where->folder_attribute )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: folder_attribute_seek(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			key );
		exit( 1 );
	}

	if ( !update_where->folder_attribute->datatype )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: datatype is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			key );
		exit( 1 );
	}

	update_where->clause =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		update_where_clause(
			key,
			update_where->sql_injection_escape_file_data,
			update_where->
				folder_attribute->
				datatype->
				datatype_name );

	return update_where;
}

UPDATE_WHERE *update_where_calloc( void )
{
	UPDATE_WHERE *update_where;

	if ( ! ( update_where = calloc( 1, sizeof( UPDATE_WHERE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return update_where;
}

char *update_where_clause(
			char *key,
			char *sql_injection_escape_file_data,
			char *datatype_name )
{
	char clause[ 1024 ];

	if ( !key
	||   !sql_injection_escape_file_data
	||   !datatype_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( attribute_is_number( datatype_name ) )
	{
		sprintf(clause,
			"%s = %s",
			key,
			sql_injection_escape_file_data );
	}
	else
	{
		sprintf(clause,
			"%s = '%s'",
			key,
			sql_injection_escape_file_data );
	}

	return strdup( clause );
}

char *update_where_list_clause( LIST *update_where_list )
{
	UPDATE_WHERE *update_where;
	char clause[ STRING_16K ];
	char *ptr = clause;

	if ( !list_rewind( update_where_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: update_where_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		update_where = list_get( update_where_list );

		if ( !update_where->clause )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: clause is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( ptr != clause ) ptr += sprintf( ptr, "," );

		ptr += sprintf( ptr, "%s", update_where->clause );

	} while ( list_next( update_where_list ) );

	return strdup( clause );
}

LIST *update_changed_list(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			LIST *folder_attribute_list,
			int row )
{
	UPDATE_CHANGED *update_changed;
	FOLDER_ATTRIBUTE *folder_attribute;
	LIST *list;

	if ( !list_rewind( folder_attribute_list ) )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_attribute_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	list = list_new();

	do {
		folder_attribute = list_get( folder_attribute_list );

		update_changed =
			update_changed_new(
				post_dictionary,
				file_dictionary,
				folder_attribute,
				row );

		if ( update_changed )
		{
			list_set( list, update_changed );
		}

	} while ( list_next( folder_attribute_list ) );

	return list;
}

UPDATE_CHANGED *update_changed_new(
			DICTIONARY *post_dictionary,
			DICTIONARY *file_dictionary,
			FOLDER_ATTRIBUTE *folder_attribute,
			int row )
{
	UPDATE_CHANGED *update_changed = update_changed_calloc();

	if ( !folder_attribute
	||   !folder_attribute->attribute_name
	||   !folder_attribute->datatype
	||   !folder_attribute->datatype->datatype_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_attribute is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	update_changed->post_data =
		dictionary_row(
			folder_attribute->attribute_name,
			row,
			post_dictionary );

	if ( !update_changed->post_data )
	{
		free( update_changed );
		return (UPDATE_CHANGED *)0;
	}

	update_changed->sql_injection_escape_post_data =
		security_sql_injection_escape(
			security_replace_special_characters(
				string_trim_number_characters(
					update_changed->post_data,
					datatype_name ) ) );

	update_changed->file_data =
		dictionary_row(
			folder_attribute->attribute_name,
			row,
			file_dictionary );

	if ( !update_changed->file_data )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: dictionary_row(%s/%d) returned empty for dictionary = \n[%s].\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			folder_attribute->attribute_name,
			row,
			dictionary_display_delimiter(
				file_dictionary,
				SQL_DELIMITER ) );
		exit( 1 );
	}

	update_changed->sql_injection_escape_file_data =
		security_sql_injection_escape(
			update_changed->file_data );

	update_changed->update_changed_boolean =
		update_changed_boolesn(
			update_changed->sql_injection_escape_file_data,
			update_changed->sql_injection_escape_post_data );

	if ( !update_changed->update_changed_boolean )
	{
		free( update_changed );
		return (UPDATE_CHANGED *)0;
	}

	update_changed->set_clause =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		update_changed_set_clause(
			folder_attribute->attribute_name,
			folder_attribute->datatype->datatype_name,
			update_changed->sql_injection_escape_post_data );

	update_changed->attribute_name = folder_attribute->attribute_name;
	update_changed->datatype = folder_attribute->datatype->datatype_name;
	update_changed->primary_key_index = folder_attribute->primary_key_index;

	return update_changed;
}

UPDATE_CHANGED *update_changed_calloc( void )
{
	UPDATE_CHANGED *update_changed;

	if ( ! ( update_changed = calloc( 1, sizeof( UPDATE_CHANGED ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return update_changed;
}

boolean update_changed_boolean(
			char *sql_injection_escape_file_data,
			char *sql_injection_escape_post_data )
{
	return
	( string_strcmp(
		sql_injection_escape_file_data,
		sql_injection_escape_post_data ) != 0 );
}

char *update_changed_set_clause(
			char *attribute_name,
			char *datatype_name,
			char *post_data )
{
	char set_clause[ STRING_16K ];

	if ( !attribute_name
	||   !datatype_name
	||   !post_data )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: parameter is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( strcmp( post_data, "/" ) == 0 )
	{
		sprintf(set_clause,
			"%s = null",
			attribute_name );
	}
	else
	if ( attribute_is_number( datatype_name ) )
	{
		sprintf(set_clause,
			"%s = %s",
			attribute_name,
			post_data );
	}
	else
	{
		sprintf(set_clause,
			"%s = '%s'",
			attribute_name,
			post_data );
	}

	return strdup( set_clause );
}

char *update_changed_list_set_clause(
			LIST *update_changed_list )
{
	char set_clause[ STRING_64K ];
	char *ptr = set_clause;
	UPDATE_CHANGED *update_changed;

	if ( !list_rewind( update_changed_list ) ) return (char *)0;

	do {
		update_changed = list_get( update_changed_list );

		if ( !update_changed->set_clause )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: set_clause is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( ptr != set_clause ) ptr += sprintf( ptr, "," );

		ptr += sprintf(
			ptr,
			"%s",
			update_changed->set_clause );

		free( update_changed->set_clause );

	} while ( list_next( update_changed_list ) );

	return strdup( set_clause );
}

boolean update_changed_primary_key(
			LIST *update_changed_list )
{
	UPDATE_CHANGED *update_changed;

	if ( !list_rewind( update_changed_list ) ) return 0;

	do {
		update_changed = list_get( update_changed_list );

		if ( update_changed->primary_key_index ) return 1;

	} while ( list_next( update_changed_list ) );

	return 0;
}

char *update_where_primary_data_delimiter(
			LIST *update_where_list,
			char sql_delimiter )
{
	UPDATE_WHERE *update_where;
	char data_delimiter[ 1024 ];
	char *ptr = data_delimiter;

	if ( !list_rewind( update_where_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: update_where_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	do {
		update_where = list_get( update_where_list );

		if ( !update_where->sql_injection_escape_file_data )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: sql_injection_escape_file_data is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		if ( ptr != data_delimiter )
		{
			ptr += sprintf(
				ptr,
				"%c",
				sql_delimiter );
		}

		ptr += sprintf(
			ptr,
			"%s",
			update_where->sql_injection_escape_file_data );

	} while ( list_next( update_where_list ) );

	return strdup( data_delimiter );
}
