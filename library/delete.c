/* $APPASERVER_HOME/library/delete.c			*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "timlib.h"
#include "list.h"
#include "sql.h"
#include "dictionary.h"
#include "process.h"
#include "appaserver.h"
#include "appaserver_error.h"
#include "relation.h"
#include "folder.h"
#include "query.h"
#include "appaserver_library.h"
#include "folder_menu.h"
#include "appaserver_parameter_file.h"
#include "role_operation.h"
#include "element.h"
#include "delete.h"

DELETE *delete_calloc( void )
{
	DELETE *delete;

	if ( ! ( delete = calloc( 1, sizeof( DELETE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: calloc() returned empty.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return delete;
}

DELETE *delete_new(
			char *application_name,
			char *session_key,
			char *login_name,
			char *folder_name,
			char *role_name,
			LIST *primary_data_list,
			boolean dont_delete_mto1_isa )
{
	DELETE *delete = delete_calloc();

	delete->application_name = application_name;
	delete->session_key = session_key;
	delete->login_name = login_name;
	delete->folder_name = folder_name;
	delete->role_name = role_name;
	delete->primary_data_list = primary_data_list;
	delete->dont_delete_mto1_isa = dont_delete_mto1_isa;

	delete->folder =
		folder_fetch(
			folder_name,
			role_name,
			(LIST *)0 /* role_exclude_attribute_name_list */,
			/* Also sets primary_key_list */
			1 /* fetch_folder_attribute_list */,
			0 /* not fetch_relation_mto1_non_isa_list */,
			/* Also sets folder_attribute_append_isa_list */
			1 /* fetch_relation_mto1_isa_list */,
			0 /* not fetch_relation_one2m_list */,
			1 /* fetch_relation_one2m_recursive_list */,
			1 /* fetch_process */,
			0 /* not fetch_role_folder_list */,
			1 /* fetch_row_level_restriction */,
			1 /* fetch_role_operation_list */ );

	if ( !delete->folder )
	{
		fprintf(stderr,
		"ERROR in %s/%s()/%d: folder_fetch(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			folder_name );
		exit( 1 );
	}


	if ( !role_operation_delete(
			delete->folder->role_operation_list ) )
	{
		return (DELETE *)0;
	}

	delete->role =
		role_fetch(
			role_name,
			0 /* not fetch_attribute_exclude_list */ );

	delete->security_entity =
		security_entity_new(
			login_name,
			delete->folder->non_owner_forbid ||
			delete->folder->non_owner_view_only,
			delete->role->override_row_restrictions );

	delete->delete_root =
		delete_root_new(
			application_name,
			folder_name,
			login_name,
			delete->folder->primary_key_list,
			primary_data_list,
			delete->folder->post_change_process,
			security_entity_where(
				delete->security_entity,
				delete->folder->folder_attribute_list ) );

	if ( list_length( delete->folder->relation_one2m_recursive_list ) )
	{
		delete->delete_one2m_list =
			delete_one2m_list(
				application_name,
				login_name,
				delete->folder->relation_one2m_recursive_list,
				primary_data_list /* foreign_data_list */ );
	}

	if ( !dont_delete_mto1_isa
	&&   list_length( delete->folder->relation_mto1_isa_list ) )
	{
		delete->delete_mto1_isa_list =
			delete_mto1_isa_list(
				application_name,
				login_name,
				delete->folder->relation_mto1_isa_list,
				primary_data_list );
	}

	return delete;
}

LIST *delete_sql_statement_list(
			DELETE_ROOT *delete_root,
			LIST *delete_one2m_list,
			LIST *delete_mto1_isa_list )
{
	LIST *sql_statement_list = {0};

	if ( delete_root )
	{
		sql_statement_list = list_new();

		list_set(
			sql_statement_list,
			delete_root->delete_sql_statement );
	}

	if ( list_length( delete_one2m_list ) )
	{
		if ( !sql_statement_list )
			sql_statement_list =
				list_new();

		list_set_list(
			sql_statement_list,
			delete_one2m_list_sql_statement_list(
				delete_one2m_list ) );
	}

	if ( list_length( delete_mto1_isa_list ) )
	{
		if ( !sql_statement_list )
			sql_statement_list =
				list_new();

		list_set_list(
			sql_statement_list,
			delete_mto1_isa_sql_statement_list(
				delete_mto1_isa_list ) );
	}

	return sql_statement_list;
}

LIST *delete_distinct_folder_name_list(
			DELETE_ROOT *delete_root,
			LIST *delete_one2m_list,
			LIST *delete_mto1_isa_list )
{
	LIST *folder_name_list = {0};

	if ( delete_root )
	{
		folder_name_list = list_new();

		list_set(
			folder_name_list,
			delete_root->folder_name );
	}

	if ( list_length( delete_one2m_list ) )
	{
		if ( !folder_name_list )
			folder_name_list =
				list_new();

		list_unique_list(
			folder_name_list,
			delete_one2m_list_distinct_folder_name_list(
				delete_one2m_list ) );
	}

	if ( list_length( delete_mto1_isa_list ) )
	{
		if ( !folder_name_list )
			folder_name_list =
				list_new();

		list_unique_list(
			folder_name_list,
			delete_mto1_isa_distinct_folder_name_list(
				delete_mto1_isa_list ) );
	}

	return folder_name_list;
}

LIST *delete_pre_command_line_list(
			DELETE_ROOT *delete_root,
			LIST *delete_one2m_list,
			LIST *delete_mto1_isa_list )
{
	LIST *command_line_list = {0};

	if ( delete_root && delete_root->delete_command_line )
	{
		command_line_list = list_new();

		list_set(
			command_line_list,
			delete_root->delete_pre_command_line );
	}

	if ( list_length( delete_one2m_list ) )
	{
		if ( !command_line_list )
			command_line_list =
				list_new();

		list_set_list(
			command_line_list,
			delete_one2m_list_pre_command_line_list(
				delete_one2m_list ) );
	}

	if ( list_length( delete_mto1_isa_list ) )
	{
		if ( !command_line_list )
			command_line_list =
				list_new();

		list_set_list(
			command_line_list,
			delete_mto1_isa_pre_command_line_list(
				delete_mto1_isa_list ) );
	}

	return command_line_list;
}

LIST *delete_command_line_list(
			DELETE_ROOT *delete_root,
			LIST *delete_one2m_list,
			LIST *delete_mto1_isa_list )
{
	LIST *command_line_list = {0};

	if ( delete_root && delete_root->delete_command_line )
	{
		command_line_list = list_new();

		list_set( command_line_list, delete_root->delete_command_line );
	}

	if ( list_length( delete_one2m_list ) )
	{
		if ( !command_line_list )
			command_line_list =
				list_new();

		list_set_list(
			command_line_list,
			delete_one2m_list_command_line_list(
				delete_one2m_list ) );
	}

	if ( list_length( delete_mto1_isa_list ) )
	{
		if ( !command_line_list )
			command_line_list =
				list_new();

		list_set_list(
			command_line_list,
			delete_mto1_isa_command_line_list(
				delete_mto1_isa_list ) );
	}

	return command_line_list;
}

char *delete_sql_statement(
			char *folder_table_name,
			LIST *primary_key_list,
			LIST *primary_data_list,
			char *security_entity_where )
{
	char sql_statement[ 1024 ];
	char *data_where;
	char where[ 512 ];

	if ( !folder_table_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_table_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	data_where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_data_where(
			(char *)0 /* folder_name */,
			primary_key_list /* where_attribute_name_list */,
			primary_data_list /* where_attribute_data_list */,
			(LIST *)0 /* folder_attribute_list */ );

	if ( security_entity_where )
	{
		sprintf(where,
			"%s and %s",
			data_where,
			security_entity_where );
	}
	else
	{
		strcpy( where, data_where );
	}

	sprintf(sql_statement,
		"delete from %s where %s;",
		folder_table_name,
		where );

	free( data_where );
	return strdup( sql_statement );
}

char *delete_command_line(
			char *command_line,
			char *login_name,
			LIST *primary_key_list,
			LIST *primary_data_list,
			char *state )
{
	char line[ 1024 ];
	char buffer[ 128 ];

	if ( !command_line ) return (char *)0;

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
		string_search_replace(
			line,
			(char *)list_get( primary_key_list ),
			double_quotes_around(
				buffer,
				(char *)list_get( primary_data_list ) ) );

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
			state ) );

	return strdup( line );
}

DELETE_ROOT *delete_root_new(
			char *application_name,
			char *folder_name,
			char *login_name,
			LIST *primary_key_list,
			LIST *primary_data_list,
			PROCESS *post_change_process,
			char *security_entity_where )
{
	DELETE_ROOT *delete_root = delete_root_calloc();

	delete_root->application_name = application_name;
	delete_root->folder_name = folder_name;
	delete_root->login_name = login_name;
	delete_root->primary_key_list = primary_key_list;
	delete_root->primary_data_list = primary_data_list;
	delete_root->post_change_process = post_change_process;
	delete_root->security_entity_where = security_entity_where;

	delete_root->delete_sql_statement =
		delete_sql_statement(
			folder_table_name(
				application_name,
				folder_name ),
			primary_key_list,
			primary_data_list,
			security_entity_where );

	if ( post_change_process )
	{
		delete_root->delete_pre_command_line =
			delete_command_line(
				post_change_process->command_line,
				login_name,
				primary_key_list,
				primary_data_list,
				"predelete" );

		delete_root->delete_command_line =
			delete_command_line(
				post_change_process->command_line,
				login_name,
				primary_key_list,
				primary_data_list,
				"delete" );
	}

	return delete_root;
}

DELETE_ROOT *delete_root_calloc( void )
{
	DELETE_ROOT *delete_root;

	if ( ! ( delete_root = calloc( 1, sizeof( DELETE_ROOT ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return delete_root;
}

LIST *delete_one2m_list(
			char *application_name,
			char *login_name,
			LIST *relation_one2m_recursive_list,
			LIST *foreign_data_list )
{
	LIST *list;
	RELATION *relation_one2m;

	if ( !list_rewind( relation_one2m_recursive_list ) ) return (LIST *)0;

	list = list_new();

	do {
		relation_one2m =
			list_get(
				relation_one2m_recursive_list );

		list_set(
			list,
			delete_one2m_new(
				application_name,
				login_name,
				relation_one2m,
				foreign_data_list ) );

	} while ( list_next( relation_one2m_recursive_list ) );

	return list;
}

DELETE_ONE2M *delete_one2m_new(
			char *application_name,
			char *login_name,
			RELATION *relation_one2m,
			LIST *foreign_data_list )
{
	DELETE_ONE2M *delete_one2m = delete_one2m_calloc();

	if ( !relation_one2m )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: relation_one2m is empty.\n",
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

	if ( !list_length( relation_one2m->many_folder->primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
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

	delete_one2m->primary_delimited_list =
		delete_one2m_primary_delimited_list(
			folder_table_name(
				application_name,
				relation_one2m->many_folder->folder_name ),
			relation_one2m->many_folder->primary_key_list,
			relation_one2m->foreign_key_list,
			foreign_data_list );

	if ( !list_length( delete_one2m->primary_delimited_list ) )
	{
		free( delete_one2m );
		return (DELETE_ONE2M *)0;
	}

	if ( !relation_one2m->is_primary_key_subset )
	{
		delete_one2m->update_sql_statement_list =
			delete_one2m_update_sql_statement_list(
				folder_table_name(
					application_name,
					relation_one2m->
						many_folder->
						folder_name ),
				relation_one2m->foreign_key_list,
				relation_one2m->
					many_folder->
					primary_key_list,
				delete_one2m->primary_delimited_list );
	}
	else
	{
		delete_one2m->delete_sql_statement_list =
			delete_one2m_sql_statement_list(
				folder_table_name(
					application_name,
					relation_one2m->
						many_folder->
						folder_name ),
				relation_one2m->
					many_folder->
					primary_key_list,
				delete_one2m->primary_delimited_list );
	}

	if ( relation_one2m->many_folder->post_change_process )
	{
		delete_one2m->pre_command_line_list =
			delete_one2m_command_line_list(
				relation_one2m->
					many_folder->
					post_change_process->
					command_line,
				login_name,
				relation_one2m->foreign_key_list,
				delete_one2m->primary_delimited_list,
				"predelete" /* state */ );

		delete_one2m->command_line_list =
			delete_one2m_command_line_list(
				relation_one2m->
					many_folder->
					post_change_process->
					command_line,
				login_name,
				relation_one2m->foreign_key_list,
				delete_one2m->primary_delimited_list,
				"delete" /* state */ );
	}

	return delete_one2m;
}

DELETE_ONE2M *delete_one2m_calloc( void )
{
	DELETE_ONE2M *delete_one2m;

	if ( ! ( delete_one2m = calloc( 1, sizeof( DELETE_ONE2M ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return delete_one2m;
}

LIST *delete_mto1_isa_list(
			char *application_name,
			char *login_name,
			LIST *relation_mto1_isa_list,
			LIST *primary_data_list )
{
	LIST *mto1_isa_list;

	if ( !list_rewind( relation_mto1_isa_list ) ) return (LIST *)0;

	mto1_isa_list = list_new();

	do {
		list_set(
			mto1_isa_list,
			delete_mto1_isa_new(
				application_name,
				login_name,
				list_get( relation_mto1_isa_list ),
				primary_data_list ) );

	} while ( list_next( relation_mto1_isa_list ) );

	return mto1_isa_list;
}

DELETE_MTO1_ISA *delete_mto1_isa_new(
			char *application_name,
			char *login_name,
			RELATION *relation_mto1_isa,
			LIST *primary_data_list )
{
	DELETE_MTO1_ISA *delete_mto1_isa = delete_mto1_isa_calloc();

	if ( !relation_mto1_isa )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: relation_mto1_isa is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !relation_mto1_isa->one_folder )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: one_folder is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( relation_mto1_isa->foreign_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: foreign_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	delete_mto1_isa->delete_sql_statement =
		delete_sql_statement(
			folder_table_name(
				application_name,
				relation_mto1_isa->one_folder->folder_name ),
			relation_mto1_isa->foreign_key_list,
			primary_data_list,
			(char *)0 /* security_entity_where */ );

	if ( !delete_mto1_isa->delete_sql_statement )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: delete_sql_statement(%s) returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			relation_mto1_isa->one_folder->folder_name );
		exit( 1 );
	}

	if ( relation_mto1_isa->one_folder->post_change_process )
	{
		delete_mto1_isa->delete_pre_command_line =
			delete_command_line(
				relation_mto1_isa->
					one_folder->
					post_change_process->
					command_line,
				login_name,
				relation_mto1_isa->foreign_key_list,
				primary_data_list,
				"predelete" /* state */ );

		delete_mto1_isa->delete_command_line =
			delete_command_line(
				relation_mto1_isa->
					one_folder->
					post_change_process->
					command_line,
				login_name,
				relation_mto1_isa->foreign_key_list,
				primary_data_list,
				"delete" /* state */ );
	}

	delete_mto1_isa->one2m_list =
		delete_mto1_isa_one2m_list(
			application_name,
			login_name,
			relation_mto1_isa->
				one_folder->
				folder_name,
			primary_data_list );

	return delete_mto1_isa;
}

DELETE_MTO1_ISA *delete_mto1_isa_calloc( void )
{
	DELETE_MTO1_ISA *delete_mto1_isa;

	if ( ! ( delete_mto1_isa =
			calloc( 1, sizeof( DELETE_MTO1_ISA ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: calloc() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return delete_mto1_isa;
}

LIST *delete_one2m_primary_delimited_list(
			char *folder_table_name,
			LIST *primary_key_list,
			LIST *foreign_key_list,
			LIST *foreign_data_list )
{
	return
	query_primary_delimited_list(
			folder_table_name,
			primary_key_list,
			foreign_key_list,
			foreign_data_list );
}

char *delete_one2m_update_set_null_statement(
			LIST *foreign_key_list )
{
	char set_null_statement[ 1024 ];
	char *ptr = set_null_statement;

	if ( !list_rewind( foreign_key_list ) ) return (char *)0;

	do {
		if ( !list_at_first( foreign_key_list ) )
			ptr += sprintf( ptr, ", " );

		ptr += sprintf(
			ptr,
			"%s = null",
			(char *)list_get( foreign_key_list ) );

	} while ( list_next( foreign_key_list ) );

	return strdup( set_null_statement );
}

char *delete_one2m_update_sql_statement(
			char *folder_table_name,
			LIST *foreign_key_list,
			LIST *primary_key_list,
			LIST *primary_data_list )
{
	char sql_statement[ 1024 ];
	char *set_null_statement;
	char *where;

	if ( !folder_table_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_table_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( foreign_key_list ) )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: foreign_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return (char *)0;
	}

	set_null_statement =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		delete_one2m_update_set_null_statement(
			foreign_key_list );

	if ( !set_null_statement ) return (char *)0;

	where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_data_where(
			(char *)0 /* folder_name */,
			primary_key_list /* where_attribute_name_list */,
			primary_data_list /* where_attribute_data_list */,
			(LIST *)0 /* folder_attribute_list */ );

	sprintf(sql_statement,
		"update %s set where %s;",
		folder_table_name,
		where );

	free( where );
	free( set_null_statement );

	return strdup( sql_statement );
}

char *delete_one2m_sql_statement(
			char *folder_table_name,
			LIST *primary_key_list,
			LIST *primary_data_list )
{
	char sql_statement[ 1024 ];
	char *where;

	if ( !folder_table_name )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: folder_table_name is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_length( primary_key_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: primary_key_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	where =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_data_where(
			(char *)0 /* folder_name */,
			primary_key_list /* where_attribute_name_list */,
			primary_data_list /* where_attribute_data_list */,
			(LIST *)0 /* folder_attribute_list */ );

	sprintf(sql_statement,
		"delete from %s where %s;",
		folder_table_name,
		where );

	free( where );
	return strdup( sql_statement );
}

LIST *delete_one2m_sql_statement_list(
			char *folder_table_name,
			LIST *primary_key_list,
			LIST *primary_delimited_list )
{
	LIST *sql_statement_list;

	if ( !list_rewind( primary_delimited_list ) ) return (LIST *)0;

	sql_statement_list = list_new();

	do {
		list_set(
			sql_statement_list,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			delete_one2m_sql_statement(
				folder_table_name,
				primary_key_list,
				list_delimiter_extract(
					list_get(
						primary_delimited_list ),
					SQL_DELIMITER ) ) );

	} while ( list_next( primary_delimited_list ) );

	return sql_statement_list;
}

LIST *delete_one2m_update_sql_statement_list(
			char *folder_table_name,
			LIST *foreign_key_list,
			LIST *primary_key_list,
			LIST *primary_delimited_list )
{
	LIST *sql_statement_list;

	if ( !list_rewind( primary_delimited_list ) ) return (LIST *)0;

	sql_statement_list = list_new();

	do {
		list_set(
			sql_statement_list,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			delete_one2m_update_sql_statement(
				folder_table_name,
				foreign_key_list,
				primary_key_list,
				list_delimiter_extract(
					list_get(
						primary_delimited_list ),
					SQL_DELIMITER ) ) );

	} while ( list_next( primary_delimited_list ) );

	return sql_statement_list;
}

LIST *delete_one2m_command_line_list(
			char *command_line,
			char *login_name,
			LIST *foreign_key_list,
			LIST *primary_delimited_list,
			char *state )
{
	LIST *command_line_list;

	if ( !command_line ) return (LIST *)0;
	if ( !list_rewind( primary_delimited_list ) ) return (LIST *)0;

	command_line_list = list_new();

	do {
		list_set(
			command_line_list,
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			delete_command_line(
				command_line,
				login_name,
				foreign_key_list
					/* primary_key_list */,
				list_string_extract(
					(char *)
					list_get(
						primary_delimited_list ),
					SQL_DELIMITER ),
				state ) );

	} while ( list_next( primary_delimited_list ) );

	return command_line_list;
}

LIST *delete_mto1_isa_one2m_list(
			char *application_name,
			char *login_name,
			char *folder_name,
			LIST *primary_data_list )
{
	LIST *relation_list;

	relation_list =
		relation_one2m_recursive_list(
			(LIST *)0 /* relation_list */,
			folder_name,
			1 /* fetch_process */ );

	if ( !list_length( relation_list ) ) return (LIST *)0;

	return
	delete_one2m_list(
		application_name,
		login_name,
		relation_list,
		primary_data_list /* foreign_data_list */ );
}

LIST *delete_mto1_isa_one2m_sql_statement_list(
			LIST *one2m_list )
{
	return delete_one2m_list_sql_statement_list( one2m_list );
}

LIST *delete_mto1_isa_one2m_pre_command_line_list(
			LIST *one2m_list )
{
	return delete_one2m_list_pre_command_line_list( one2m_list );
}

LIST *delete_mto1_isa_one2m_command_line_list(
			LIST *one2m_list )
{
	return delete_one2m_list_command_line_list( one2m_list );
}

LIST *delete_one2m_list_sql_statement_list(
			LIST *delete_one2m_list )
{
	DELETE_ONE2M *delete_one2m;
	LIST *sql_statement_list;

	if ( !list_rewind( delete_one2m_list ) ) return (LIST *)0;

	sql_statement_list = list_new();

	do {
		delete_one2m = list_get( delete_one2m_list );

		if ( list_length( delete_one2m->update_sql_statement_list ) )
		{
			list_set_list(
				sql_statement_list,
				delete_one2m->update_sql_statement_list );
		}
		else
		if ( list_length( delete_one2m->delete_sql_statement_list ) )
		{
			list_set_list(
				sql_statement_list,
				delete_one2m->delete_sql_statement_list );
		}
		else
		{
			fprintf(stderr,
"Warning in %s/%s()/%d: both update_sql_statement_list and delete_sql_statement_list are empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
		}

	} while ( list_next( delete_one2m_list ) );

	return sql_statement_list;
}

LIST *delete_one2m_list_pre_command_line_list(
			LIST *delete_one2m_list )
{
	DELETE_ONE2M *delete_one2m;
	LIST *command_line_list;

	if ( !list_rewind( delete_one2m_list ) ) return (LIST *)0;

	command_line_list = list_new();

	do {
		delete_one2m = list_get( delete_one2m_list );

		if ( list_length( delete_one2m->command_line_list ) )
		{
			list_set_list(
				command_line_list,
				delete_one2m->pre_command_line_list );
		}

	} while ( list_next( delete_one2m_list ) );

	return command_line_list;
}

LIST *delete_one2m_list_command_line_list(
			LIST *delete_one2m_list )
{
	DELETE_ONE2M *delete_one2m;
	LIST *command_line_list;

	if ( !list_rewind( delete_one2m_list ) ) return (LIST *)0;

	command_line_list = list_new();

	do {
		delete_one2m = list_get( delete_one2m_list );

		if ( list_length( delete_one2m->command_line_list ) )
		{
			list_set_list(
				command_line_list,
				delete_one2m->command_line_list );
		}

	} while ( list_next( delete_one2m_list ) );

	return command_line_list;
}

LIST *delete_mto1_isa_sql_statement_list(
			LIST *delete_mto1_isa_list )
{
	DELETE_MTO1_ISA *delete_mto1_isa;
	LIST *sql_statement_list;

	if ( !list_rewind( delete_mto1_isa_list ) ) return (LIST *)0;

	sql_statement_list = list_new();

	do {
		delete_mto1_isa =
			list_get(
				delete_mto1_isa_list );

		if ( !delete_mto1_isa->delete_sql_statement )
		{
			fprintf(stderr,
			"ERROR in %s/%s()/%d: delete_sql_statement is empty.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		list_set(
			sql_statement_list,
			delete_mto1_isa->delete_sql_statement );

	} while ( list_next( delete_mto1_isa_list ) );

	return sql_statement_list;
}

LIST *delete_mto1_isa_pre_command_line_list(
			LIST *delete_mto1_isa_list )
{
	DELETE_MTO1_ISA *delete_mto1_isa;
	LIST *command_line_list;

	if ( !list_rewind( delete_mto1_isa_list ) ) return (LIST *)0;

	command_line_list = list_new();

	do {
		delete_mto1_isa =
			list_get(
				delete_mto1_isa_list );

		if ( delete_mto1_isa->delete_pre_command_line )
		{
			list_set(
				command_line_list,
				delete_mto1_isa->delete_pre_command_line );
		}

		if ( list_length( delete_mto1_isa->one2m_list ) )
		{
			list_set_list(
				command_line_list,
				delete_mto1_isa_one2m_pre_command_line_list(
					delete_mto1_isa->one2m_list ) );
		}

	} while ( list_next( delete_mto1_isa_list ) );

	return command_line_list;
}

LIST *delete_mto1_isa_command_line_list(
			LIST *delete_mto1_isa_list )
{
	DELETE_MTO1_ISA *delete_mto1_isa;
	LIST *command_line_list;

	if ( !list_rewind( delete_mto1_isa_list ) ) return (LIST *)0;

	command_line_list = list_new();

	do {
		delete_mto1_isa =
			list_get(
				delete_mto1_isa_list );

		if ( delete_mto1_isa->delete_command_line )
		{
			list_set(
				command_line_list,
				delete_mto1_isa->delete_command_line );
		}

		if ( list_length( delete_mto1_isa->one2m_list ) )
		{
			list_set_list(
				command_line_list,
				delete_mto1_isa_one2m_command_line_list(
					delete_mto1_isa->one2m_list ) );
		}

	} while ( list_next( delete_mto1_isa_list ) );

	return command_line_list;
}

LIST *delete_mto1_isa_distinct_folder_name_list(
			LIST *delete_mto1_isa_list )
{
	DELETE_MTO1_ISA *delete_mto1_isa;
	LIST *folder_name_list;

	if ( !list_rewind( delete_mto1_isa_list ) ) return (LIST *)0;

	folder_name_list = list_new();

	do {
		delete_mto1_isa =
			list_get(
				delete_mto1_isa_list );

		list_set(
			folder_name_list,
			delete_mto1_isa->
				relation_mto1_isa->
				one_folder->
				folder_name );

	} while ( list_next( delete_mto1_isa_list ) );

	return folder_name_list;
}

LIST *delete_mto1_isa_one2m_distinct_folder_name_list(
			LIST *delete_mto1_isa_list )
{
	DELETE_MTO1_ISA *delete_mto1_isa;
	LIST *folder_name_list;

	if ( !list_rewind( delete_mto1_isa_list ) ) return (LIST *)0;

	folder_name_list = list_new();

	do {
		delete_mto1_isa =
			list_get(
				delete_mto1_isa_list );

		if ( !list_length( delete_mto1_isa->one2m_list ) )
			continue;

		list_set_list(
			folder_name_list,
			delete_one2m_list_distinct_folder_name_list(
				delete_mto1_isa->one2m_list ) );

	} while ( list_next( delete_mto1_isa_list ) );

	return folder_name_list;
}

char *delete_message_html(
			LIST *primary_data_list,
			int delete_sql_statement_list_length,
			LIST *delete_distinct_folder_name_list )
{
	char html[ STRING_INPUT_BUFFER ];
	char *ptr = html;
	char *display;

	if ( !list_length( primary_data_list ) ) return (char *)0;
	if ( !delete_sql_statement_list_length ) return (char *)0;

	if ( !list_rewind( delete_distinct_folder_name_list ) )
	{
		fprintf(stderr,
	"ERROR in %s/%s()/%d: delete_distinct_folder_name_list is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( ! ( display =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			delete_primary_data_list_display(
				primary_data_list ) ) )
	{
		fprintf(stderr,
"ERROR in %s/%s()/%d: element_drop_down_data_list_display() returned empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	ptr += sprintf( ptr, "<table><tr>" );

	ptr += sprintf( ptr, "<td>%s", display );

	ptr += sprintf(
		ptr,
		"<td>Deleted %d %s in<td>",
		delete_sql_statement_list_length,
		(delete_sql_statement_list_length == 1)
			? "row"
			: "rows" );

	do {
		if ( !list_at_first( delete_distinct_folder_name_list ) )
			ptr += sprintf( ptr, ", " );

		ptr += sprintf(
			ptr,
			"%s",
			(char *)list_get(
				delete_distinct_folder_name_list ) );

	} while ( list_next( delete_distinct_folder_name_list ) );

	ptr += sprintf( ptr, "</table>" );

	free( display );
	return strdup( html );
}

LIST *delete_one2m_list_distinct_folder_name_list(
			LIST *delete_one2m_list )
{
	DELETE_ONE2M *delete_one2m;
	LIST *folder_name_list;

	if ( !list_rewind( delete_one2m_list ) ) return (LIST *)0;

	folder_name_list = list_new();

	do {
		delete_one2m =
			list_get(
				delete_one2m_list );

		list_set(
			folder_name_list,
			delete_one2m->
				relation_one2m->
				many_folder->
				folder_name );

	} while ( list_next( delete_one2m_list ) );

	return folder_name_list;
}

char *delete_primary_data_list_display(
			LIST *primary_data_list )
{
	char display[ 1024 ];
	char *ptr = display;

	if ( !list_rewind( primary_data_list ) ) return (char *)0;

	do {
		if ( !list_at_head( primary_data_list ) )
		{
			ptr += sprintf(
				ptr,
				"%s", 
				ELEMENT_LONG_DASH_DELIMITER );
		}

		ptr += sprintf(
			ptr,
			"%s",
			(char *)list_get(
				primary_data_list ) );

	} while ( list_next( primary_data_list ) );

	return strdup( display );
}

