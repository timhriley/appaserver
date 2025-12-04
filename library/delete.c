/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/delete.c		 			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org 	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "list.h"
#include "sql.h"
#include "dictionary.h"
#include "process.h"
#include "update.h"
#include "relation_mto1.h"
#include "relation_one2m.h"
#include "appaserver.h"
#include "query.h"
#include "folder_menu.h"
#include "appaserver_parameter.h"
#include "folder_operation.h"
#include "delete.h"

DELETE *delete_calloc( void )
{
	DELETE *delete;

	if ( ! ( delete = calloc( 1, sizeof ( DELETE ) ) ) )
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
		char *login_name,
		char *role_name,
		char *folder_name,
		LIST *primary_query_cell_list,
		boolean isa_boolean,
		boolean update_null_boolean )
{
	DELETE *delete;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name
	||   !list_length( primary_query_cell_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delete = delete_calloc();

	delete->delete_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		delete_input_new(
			application_name,
			login_name,
			role_name,
			folder_name );

	if ( !delete->delete_input->folder_operation_delete_boolean )
		return delete;

	delete->delete_root =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		delete_root_new(
			application_name,
			folder_name,
			primary_query_cell_list,
			delete->delete_input->folder->post_change_process,
			delete->delete_input->
				security_entity->
				where,
			delete->delete_input->appaserver_error_filename );

	if ( list_length(
		delete->
			delete_input->
			relation_one2m_list ) )
	{
		delete->delete_one2m_list =
			delete_one2m_list_new(
				application_name,
				role_name,
				update_null_boolean,
				delete->
					delete_input->
					appaserver_error_filename,
				delete->
					delete_input->
					appaserver_parameter_mount_point,
				primary_query_cell_list,
				delete->
					delete_input->
					relation_one2m_list );
	}

	if ( isa_boolean
	&&   list_length( delete->delete_input->relation_mto1_isa_list ) )
	{
		delete->delete_mto1_isa_list =
			delete_mto1_isa_list_new(
				application_name,
				role_name,
				update_null_boolean,
				delete->
					delete_input->
					appaserver_error_filename,
				delete->
					delete_input->
					appaserver_parameter_mount_point,
				primary_query_cell_list,
				delete->
					delete_input->
					relation_mto1_isa_list );
	}

	delete->delete_sql_list =
		delete_sql_list(
			delete->delete_root,
			delete->delete_one2m_list,
			delete->delete_mto1_isa_list );

	delete->row_count =
		delete_row_count(
			delete->delete_sql_list );

	delete->update_cell_count =
		delete_update_cell_count(
			delete->delete_sql_list );

	delete->message_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		delete_message_string(
			DELETE_MESSAGE,
			delete->row_count,
			delete->update_cell_count );

	delete->complete_message_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		delete_message_string(
			DELETE_COMPLETE_MESSAGE,
			delete->row_count,
			delete->update_cell_count );

	return delete;
}

char *delete_where(
		LIST *primary_query_cell_list,
		char *security_entity_where )
{
	char *cell_where_string;
	static char where_string[ 1024 ];

	if ( !list_length( primary_query_cell_list ) )
	{
		char message[ 128 ];

		sprintf(message, "primary_query_cell_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ! ( cell_where_string =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			query_cell_where_string(
				primary_query_cell_list ) ) )
	{
		char message[ 128 ];

		sprintf(message, "query_cell_where_string() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( security_entity_where )
	{
		snprintf(
			where_string,
			sizeof ( where_string ),
			"%s and %s",
			cell_where_string,
			security_entity_where );
	}
	else
	{
		string_strcpy(
			where_string,
			cell_where_string,
			sizeof ( where_string ) );
	}

	free( cell_where_string );

	return where_string;
}

char *delete_sql_statement(
		char *folder_table_name,
		char *delete_where )
{
	char sql_statement[ 2048 ];

	if ( !folder_table_name
	||   !delete_where )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		sql_statement,
		sizeof ( sql_statement ),
		"delete from %s where %s;",
		folder_table_name,
		delete_where );

	return strdup( sql_statement );
}

char *delete_command_line(
		char *delete_state,
		LIST *primary_query_cell_list,
		char *post_change_process_command_line,
		char *appaserver_error_filename )
{
	char command_line[ 1024 ];
	char destination[ 1024 ];

	if ( !delete_state
	||   !list_length( primary_query_cell_list )
	||   !post_change_process_command_line
	||   !appaserver_error_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	string_strcpy(
		command_line,
		post_change_process_command_line,
		1024 );

	query_cell_command_line_replace(
		command_line,
		primary_query_cell_list );

	string_with_space_search_replace(
		command_line,
		PROCESS_STATE_PLACEHOLDER,
		double_quotes_around(
			destination,
			delete_state ) );

	sprintf(
		command_line + strlen( command_line ),
		" 2>>%s",
		appaserver_error_filename );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	string_escape_dollar( command_line );
}

DELETE_ROOT *delete_root_new(
		char *application_name,
		char *folder_name,
		LIST *primary_query_cell_list,
		PROCESS *post_change_process,
		char *security_entity_where,
		char *appaserver_error_filename )
{
	DELETE_ROOT *delete_root;

	if ( !application_name
	||   !folder_name
	||   !list_length( primary_query_cell_list )
	||   !appaserver_error_filename )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delete_root = delete_root_calloc();

	delete_root->delete_where =
		strdup(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			delete_where(
				primary_query_cell_list,
				security_entity_where ) );

	delete_root->delete_sql_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		delete_sql_statement(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			folder_table_name(
				application_name,
				folder_name ),
			delete_root->delete_where );

	if ( post_change_process )
	{
		delete_root->delete_pre_command_line =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			delete_command_line(
				APPASERVER_PREDELETE_STATE,
				primary_query_cell_list,
				post_change_process->command_line,
				appaserver_error_filename );

		delete_root->delete_command_line =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			delete_command_line(
				APPASERVER_DELETE_STATE,
				primary_query_cell_list,
				post_change_process->command_line,
				appaserver_error_filename );

	}

	return delete_root;
}

DELETE_ROOT *delete_root_calloc( void )
{
	DELETE_ROOT *delete_root;

	if ( ! ( delete_root = calloc( 1, sizeof ( DELETE_ROOT ) ) ) )
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

DELETE_ONE2M *delete_one2m_new(
		char *application_name,
		char *role_name,
		boolean update_null_boolean,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		LIST *one_folder_query_cell_list,
		char *many_folder_name,
		LIST *many_folder_attribute_primary_list,
		LIST *many_folder_primary_key_list,
		LIST *relation_foreign_key_list,
		boolean foreign_key_none_primary,
		PROCESS *post_change_process )
{
	DELETE_ONE2M *delete_one2m;

	if ( !application_name
	||   !role_name
	||   !appaserver_error_filename
	||   !appaserver_parameter_mount_point
	||   !list_length( one_folder_query_cell_list )
	||   !many_folder_name
	||   !list_length( many_folder_attribute_primary_list )
	||   !list_length( many_folder_primary_key_list )
	||   !list_length( relation_foreign_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delete_one2m = delete_one2m_calloc();

	delete_one2m->post_change_process = post_change_process;
	delete_one2m->many_folder_name = many_folder_name;

	delete_one2m->table_name =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		delete_one2m_table_name(
			application_name,
			many_folder_name );

	if (	list_length( one_folder_query_cell_list ) !=
		list_length( relation_foreign_key_list ) )
	{
		char message[ 1024 ];

		sprintf(message,
			"cannot align primary=[%s] with foreign=[%s]",
			query_cell_list_display(
				one_folder_query_cell_list ),
			list_display_delimited(
				relation_foreign_key_list,
				',' ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delete_one2m->query_cell_list =
		delete_one2m_query_cell_list(
			one_folder_query_cell_list,
			relation_foreign_key_list );

	delete_one2m->select_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		delete_one2m_select_string(
			many_folder_primary_key_list,
			',' /* delimiter */ );

	delete_one2m->query_cell_where_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		query_cell_where_string(
			delete_one2m->
				query_cell_list );

	if ( !delete_one2m->query_cell_where_string )
	{
		char message[ 128 ];

		sprintf(message, "query_cell_where_string() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delete_one2m->query_system_string =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		query_system_string(
			application_name,
			delete_one2m->select_string,
			many_folder_name /* from_string */,
			delete_one2m->query_cell_where_string,
			(char *)0 /* group_by_string */,
			(char *)0 /* order_string */,
			0 /* max_rows */ );

	delete_one2m->query_fetch =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		query_fetch_new(
			many_folder_name,
			many_folder_attribute_primary_list
				/* folder_attribute_list */,
			0 /* destination_enum */,
			many_folder_primary_key_list
				/* select_name_list */,
			delete_one2m->query_system_string,
			1 /* input_save_boolean for MERGE_PURGE */ );

	if ( !list_length( delete_one2m->query_fetch->row_list ) )
	{
		free( delete_one2m );
		return NULL;
	}

	if ( foreign_key_none_primary )
	{
		if ( update_null_boolean )
		{
			delete_one2m->delete_one2m_update =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				delete_one2m_update_new(
					appaserver_error_filename,
					many_folder_name,
					delete_one2m->table_name,
					delete_one2m->
						query_cell_list
						/* set_null_query_cell_list */,
					post_change_process,
					delete_one2m->query_fetch->row_list );
		}

		return delete_one2m;
	}

	delete_one2m->relation_one2m_list =
		relation_one2m_list(
			many_folder_name
				/* one_folder_name */,
			many_folder_primary_key_list
				/* one_folder_primary_key_list */,
			1 /* include_isa_boolean */ );

	delete_one2m->delete_one2m_fetch =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		delete_one2m_fetch_new(
			application_name,
			role_name,
			update_null_boolean,
			appaserver_error_filename,
			appaserver_parameter_mount_point,
			delete_one2m->table_name
				/* many_folder_table_name */,
			post_change_process,
			delete_one2m->query_fetch->row_list,
			delete_one2m->relation_one2m_list );

	return delete_one2m;
}

DELETE_ONE2M *delete_one2m_calloc( void )
{
	DELETE_ONE2M *delete_one2m;

	if ( ! ( delete_one2m = calloc( 1, sizeof ( DELETE_ONE2M ) ) ) )
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

DELETE_MTO1_ISA *delete_mto1_isa_new(
		char *application_name,
		char *role_name,
		boolean update_null_boolean,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		LIST *primary_query_cell_list,
		char *one_folder_name,
		LIST *one_folder_attribute_primary_key_list,
		char *post_change_process_name )
{
	DELETE_MTO1_ISA *delete_mto1_isa;
	char *table_name;

	if ( !application_name
	||   !role_name
	||   !appaserver_error_filename
	||   !appaserver_parameter_mount_point
	||   !list_length( primary_query_cell_list )
	||   !one_folder_name
	||   !list_length( one_folder_attribute_primary_key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if (	list_length( primary_query_cell_list ) !=
		list_length( one_folder_attribute_primary_key_list ) )
	{
		char message[ 1024 ];

		sprintf(message,
		"cannot align query_cell_list=[%s] with primary_key_list=[%s]",
			query_cell_list_display(
				primary_query_cell_list ),
			list_display_delimited(
				one_folder_attribute_primary_key_list,
				',' ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delete_mto1_isa = delete_mto1_isa_calloc();

	table_name =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_table_name(
			application_name,
			one_folder_name );

	delete_mto1_isa->query_cell_list =
		delete_mto1_isa_query_cell_list(
			primary_query_cell_list,
			one_folder_attribute_primary_key_list );

	delete_mto1_isa->delete_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		delete_where(
			delete_mto1_isa->query_cell_list,
			(char *)0 /* security_entity_where */ );

	delete_mto1_isa->delete_sql_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		delete_sql_statement(
			table_name,
			delete_mto1_isa->delete_where );

	if ( post_change_process_name )
	{
		delete_mto1_isa->post_change_process =
			process_fetch(
				post_change_process_name,
				(char *)0 /* document_root */,
				(char *)0 /* relative_source */,
				1 /* check_executable_inside_filesystem */,
				appaserver_parameter_mount_point );

		delete_mto1_isa->pre_command_line =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			delete_command_line(
				APPASERVER_PREDELETE_STATE,
				delete_mto1_isa->
					query_cell_list,
				delete_mto1_isa->
					post_change_process->
					command_line,
				appaserver_error_filename );

		delete_mto1_isa->command_line =
			delete_command_line(
				APPASERVER_DELETE_STATE,
				delete_mto1_isa->
					query_cell_list,
				delete_mto1_isa->
					post_change_process->
					command_line,
				appaserver_error_filename );
	}

	delete_mto1_isa->relation_one2m_list =
		relation_one2m_list(
			one_folder_name,
			one_folder_attribute_primary_key_list,
			1 /* include_isa_boolean */ );

	if ( list_length( delete_mto1_isa->relation_one2m_list ) )
	{
		delete_mto1_isa->delete_one2m_list =
			delete_one2m_list_new(
				application_name,
				role_name,
				update_null_boolean,
				appaserver_error_filename,
				appaserver_parameter_mount_point,
				delete_mto1_isa->query_cell_list,
				delete_mto1_isa->relation_one2m_list );
	}

	return delete_mto1_isa;
}

DELETE_MTO1_ISA *delete_mto1_isa_calloc( void )
{
	DELETE_MTO1_ISA *delete_mto1_isa;

	if ( ! ( delete_mto1_isa = calloc( 1, sizeof ( DELETE_MTO1_ISA ) ) ) )
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

void delete_display( LIST *delete_sql_list )
{
	FILE *output_pipe;
	DELETE_SQL *delete_sql;

	fflush( stdout );

	output_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			"html_table.e '' sql_statement" );

	if ( list_rewind( delete_sql_list ) )
	do {
		delete_sql = list_get( delete_sql_list );

		fprintf(
			output_pipe,
			"%s\n",
			delete_sql->statement );

	} while ( list_next( delete_sql_list ) );

	pclose( output_pipe );
	fflush( stdout );
}

void delete_execute(
		const char *sql_executable,
		LIST *delete_sql_list )
{
	DELETE_SQL *delete_sql;
	FILE *output_pipe;

	if ( list_rewind( delete_sql_list ) )
	do {
		delete_sql = list_get( delete_sql_list );

		if ( delete_sql->pre_command_line )
		{
			security_system(
				SECURITY_FORK_CHARACTER,
				SECURITY_FORK_STRING,
				delete_sql->pre_command_line );
		}

	} while ( list_next( delete_sql_list ) );

	output_pipe =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		appaserver_output_pipe(
			/* --------------------- */
			/* Returns static memory */
			/* --------------------- */
			delete_execute_system_string(
				sql_executable ) );

	if ( list_rewind( delete_sql_list ) )
	do {
		delete_sql = list_get( delete_sql_list );

		fprintf(output_pipe,
			"%s\n",
			delete_sql->statement );

	} while ( list_next( delete_sql_list ) );

	pclose( output_pipe );

	if ( list_rewind( delete_sql_list ) )
	do {
		delete_sql = list_get( delete_sql_list );

		if ( delete_sql->command_line )
		{
			security_system(
				SECURITY_FORK_CHARACTER,
				SECURITY_FORK_STRING,
				delete_sql->command_line );
		}

	} while ( list_next( delete_sql_list ) );
}

char *delete_one2m_select_string(
		LIST *many_folder_primary_key_list,
		char delimiter )
{
	if ( !list_length( many_folder_primary_key_list )
	||   !delimiter )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	/* ------------------------- */
	/* Returns heap memory or "" */
	/* ------------------------- */
	list_delimited( many_folder_primary_key_list, delimiter );
}

char *delete_execute_system_string(
		const char *sql_executable )
{
	static char system_string[ 128 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"tee_appaserver.sh | %s 2>&1 | html_paragraph_wrapper.e",
		sql_executable );

	return system_string;
}

LIST *delete_query_cell_list(
		LIST *primary_query_cell_list,
		LIST *key_list )
{
	LIST *cell_list;
	QUERY_CELL *query_cell;

	if ( !list_length( primary_query_cell_list )
	||   !list_length( key_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty " );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if (	list_length( primary_query_cell_list ) !=
		list_length( key_list ) )
	{
		char message[ 1024 ];

		sprintf(message,
			"cannot align primary=[%s] with foreign=[%s]",
			query_cell_list_display(
				primary_query_cell_list ),
			list_display_delimited(
				key_list,
				',' ) );

		sprintf(message,
			"length=%d != length=%d",
			list_length( primary_query_cell_list ),
			list_length( key_list ) );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	cell_list = list_new();

	list_rewind( primary_query_cell_list );
	list_rewind( key_list );

	do {
		query_cell = list_get( primary_query_cell_list );

		list_set(
			cell_list,
			query_cell_parse(
				(LIST *)0 /* folder_attribute_list */,
				0 /* destination_enum */,
				list_get( key_list ) /* attribute_name */,
				query_cell->select_datum ) );

		list_next( key_list );

	} while ( list_next( primary_query_cell_list ) );

	return cell_list;
}

LIST *delete_mto1_isa_query_cell_list(
		LIST *primary_query_cell_list,
		LIST *one_folder_attribute_primary_key_list )
{
	return
	delete_query_cell_list(
		primary_query_cell_list,
		one_folder_attribute_primary_key_list );
}

LIST *delete_one2m_query_cell_list(
		LIST *primary_query_cell_list,
		LIST *relation_foreign_key_list )
{
	return
	delete_query_cell_list(
		primary_query_cell_list,
		relation_foreign_key_list );
}

DELETE_INPUT *delete_input_new(
		char *application_name,
		char *login_name,
		char *role_name,
		char *folder_name )
{
	DELETE_INPUT *delete_input;

	if ( !application_name
	||   !login_name
	||   !role_name
	||   !folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delete_input = delete_input_calloc();

	delete_input->folder_operation_list =
		folder_operation_list(
			folder_name,
			role_name,
			0 /* not fetch_operation */,
			0 /* not fetch_process */ );

	delete_input->folder_operation_delete_boolean =
		folder_operation_delete_boolean(
			delete_input->folder_operation_list );

	if ( !delete_input->folder_operation_delete_boolean )
		return delete_input;

	delete_input->appaserver_error_filename =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		appaserver_error_filename(
			application_name );

	delete_input->appaserver_parameter_mount_point =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_parameter_mount_point();

	delete_input->role =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		role_fetch(
			role_name,
		0 /* not fetch_attribute_exclude_list */ );

	delete_input->folder_row_level_restriction =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_row_level_restriction_fetch(
			folder_name );

	delete_input->security_entity =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		security_entity_new(
			login_name,
			delete_input->
				folder_row_level_restriction->
				non_owner_forbid ||
			delete_input->
				folder_row_level_restriction->
				non_owner_viewonly,
			delete_input->role->override_row_restrictions );

	delete_input->folder =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		folder_fetch(
			folder_name,
			(LIST *)0 /* exclude_attribute_name_list */,
			/* -------------------------------------------- */
			/* Sets folder_attribute_primary_list		*/
			/* Sets folder_attribute_primary_key_list	*/
			/* Sets folder_attribute_name_list		*/
			/* -------------------------------------------- */
			1 /* fetch_folder_attribute_list */,
			1 /* not fetch_attribute */,
			1 /* cache_boolean */ );

	delete_input->relation_one2m_list =
		relation_one2m_list(
			folder_name
				/* one_folder_name */,
			delete_input->folder->folder_attribute_primary_key_list
				/* one_folder_primary_key_list */,
			1 /* include_isa_boolean */ );

	delete_input->relation_mto1_isa_list =
		relation_mto1_isa_list(
			(LIST *)0 /* mto1_isa_list Pass in null */,
			folder_name
				/* many_folder_name */,
			delete_input->folder->folder_attribute_primary_key_list
				/* many_folder_primary_key_list */,
			0 /* not fetch_relation_one2m_list */,
			0 /* not fetch_relation_mto1_list */ );

	if ( delete_input->folder->post_change_process_name )
	{
		delete_input->folder->post_change_process =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			process_fetch(
				delete_input->folder->post_change_process_name,
				(char *)0 /* document_root */,
				(char *)0 /* relative_source */,
			 	1 /* check_executable_inside_filesystem */,
				delete_input->
					appaserver_parameter_mount_point );
	}

	return delete_input;
}

DELETE_INPUT *delete_input_calloc( void )
{
	DELETE_INPUT *delete_input;

	if ( ! ( delete_input = calloc( 1, sizeof ( DELETE_INPUT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return delete_input;
}

DELETE_ONE2M_UPDATE_CELL *delete_one2m_update_cell_new(
		char *appaserver_update_state,
		char *update_preupdate_prefix,
		char *appaserver_error_filename,
		char *folder_table_name,
		PROCESS *post_change_process,
		LIST *query_row_cell_list,
		char *query_cell_where_string,
		QUERY_CELL *set_null_query_cell )
{
	DELETE_ONE2M_UPDATE_CELL *delete_one2m_update_cell;

	if ( !appaserver_update_state
	||   !update_preupdate_prefix
	||   !appaserver_error_filename
	||   !folder_table_name
	||   !list_length( query_row_cell_list )
	||   !query_cell_where_string
	||   !set_null_query_cell )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delete_one2m_update_cell = delete_one2m_update_cell_calloc();

	delete_one2m_update_cell->sql_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		delete_one2m_update_cell_sql_statement(
			folder_table_name,
			set_null_query_cell->attribute_name
				/* set_null_attribute_name */,
			query_cell_where_string );

	if ( post_change_process )
	{
		delete_one2m_update_cell->command_line =
			delete_one2m_update_cell_command_line(
				appaserver_update_state,
				update_preupdate_prefix,
				appaserver_error_filename,
				set_null_query_cell->attribute_name,
				set_null_query_cell->select_datum,
				query_row_cell_list,
				post_change_process->command_line );
	}

	return delete_one2m_update_cell;
}

DELETE_ONE2M_UPDATE_CELL *delete_one2m_update_cell_calloc( void )
{
	DELETE_ONE2M_UPDATE_CELL *delete_one2m_update_cell;

	if ( ! ( delete_one2m_update_cell =
			calloc( 1,
				sizeof ( DELETE_ONE2M_UPDATE_CELL ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return delete_one2m_update_cell;
}

char *delete_one2m_update_cell_sql_statement(
		char *folder_table_name,
		char *set_null_attribute_name,
		char *query_cell_where_string )
{
	char update_sql_statement[ 512 ];

	if ( !folder_table_name
	||   !set_null_attribute_name
	||   !query_cell_where_string )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	sprintf(update_sql_statement,
		"update %s set %s = null where %s;",
		folder_table_name,
		set_null_attribute_name,
		query_cell_where_string );

	return strdup( update_sql_statement );
}

char *delete_one2m_update_cell_command_line(
		char *appaserver_update_state,
		char *update_preupdate_prefix,
		char *appaserver_error_filename,
		char *query_cell_attribute_name,
		char *query_cell_select_datum,
		LIST *query_row_cell_list,
		char *post_change_process_command_line )
{
	char command_line[ 1024 ];
	char destination[ 1024 ];
	char preupdate_attribute_name[ 128 ];

	if ( !appaserver_update_state
	||   !update_preupdate_prefix
	||   !appaserver_error_filename
	||   !query_cell_attribute_name
	||   !query_cell_select_datum
	||   !list_length( query_row_cell_list )
	||   !post_change_process_command_line )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	string_strcpy(
		command_line,
		post_change_process_command_line,
		1024 );

	query_cell_command_line_replace(
		command_line,
		query_row_cell_list );

	string_with_space_search_replace(
		command_line,
		PROCESS_STATE_PLACEHOLDER,
		double_quotes_around(
			destination,
			appaserver_update_state ) );

	string_with_space_search_replace(
		command_line,
		query_cell_attribute_name,
		double_quotes_around(
			destination, "" ) );

	sprintf(preupdate_attribute_name,
		"%s%s",
		update_preupdate_prefix,
		query_cell_attribute_name );

	string_with_space_search_replace(
		command_line,
		preupdate_attribute_name,
		double_quotes_around(
			destination,
			query_cell_select_datum ) );

	sprintf(command_line + strlen( command_line ),
		" 2>>%s",
		appaserver_error_filename );

	return
	/* ------------------- */
	/* Returns heap memory */
	/* ------------------- */
	string_escape_dollar( command_line );
}

DELETE_ONE2M_UPDATE_ROW *delete_one2m_update_row_new(
		char *appaserver_error_filename,
		char *folder_table_name,
		LIST *set_null_query_cell_list,
		PROCESS *post_change_process,
		LIST *query_row_cell_list )
{
	DELETE_ONE2M_UPDATE_ROW *delete_one2m_update_row;
	QUERY_CELL *set_null_query_cell;
	char *where_string;
	DELETE_ONE2M_UPDATE_CELL *delete_one2m_update_cell;

	if ( !appaserver_error_filename
	||   !folder_table_name
	||   !list_rewind( set_null_query_cell_list )
	||   !list_length( query_row_cell_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delete_one2m_update_row = delete_one2m_update_row_calloc();

	where_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		query_cell_where_string(
			query_row_cell_list );

	delete_one2m_update_row->delete_one2m_update_cell_list = list_new();

	do {
		set_null_query_cell =
			list_get(
				set_null_query_cell_list );

		delete_one2m_update_cell =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			delete_one2m_update_cell_new(
				APPASERVER_UPDATE_STATE,
				UPDATE_PREUPDATE_PREFIX,
				appaserver_error_filename,
				folder_table_name,
				post_change_process,
				query_row_cell_list,
				where_string,
				set_null_query_cell );

		list_set(
			delete_one2m_update_row->
				delete_one2m_update_cell_list,
			delete_one2m_update_cell );

	} while ( list_next( set_null_query_cell_list ) );

	free( where_string );

	return delete_one2m_update_row;
}

DELETE_ONE2M_UPDATE_ROW *delete_one2m_update_row_calloc( void )
{
	DELETE_ONE2M_UPDATE_ROW *delete_one2m_update_row;

	if ( ! ( delete_one2m_update_row =
			calloc( 1,
				sizeof ( DELETE_ONE2M_UPDATE_ROW ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return delete_one2m_update_row;
}

DELETE_ONE2M_UPDATE *delete_one2m_update_new(
		char *appaserver_error_filename,
		char *folder_name,
		char *folder_table_name,
		LIST *set_null_query_cell_list,
		PROCESS *post_change_process,
		LIST *query_fetch_row_list )
{
	DELETE_ONE2M_UPDATE *delete_one2m_update;
	QUERY_ROW *query_row;
	DELETE_ONE2M_UPDATE_ROW *delete_one2m_update_row;

	if ( !appaserver_error_filename
	||   !folder_name
	||   !folder_table_name
	||   !list_length( set_null_query_cell_list )
	||   !list_rewind( query_fetch_row_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delete_one2m_update = delete_one2m_update_calloc();

	delete_one2m_update->folder_name = folder_name;
	delete_one2m_update->delete_one2m_update_row_list = list_new();

	do {
		query_row = list_get( query_fetch_row_list );

		delete_one2m_update_row =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			delete_one2m_update_row_new(
				appaserver_error_filename,
				folder_table_name,
				set_null_query_cell_list,
				post_change_process,
				query_row->cell_list );

		list_set(
			delete_one2m_update->
				delete_one2m_update_row_list,
			delete_one2m_update_row );

	} while ( list_next( query_fetch_row_list ) );

	return delete_one2m_update;
}

DELETE_ONE2M_UPDATE *delete_one2m_update_calloc( void )
{
	DELETE_ONE2M_UPDATE *delete_one2m_update;

	if ( ! ( delete_one2m_update =
			calloc( 1,
				sizeof ( DELETE_ONE2M_UPDATE ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return delete_one2m_update;
}

DELETE_ONE2M_ROW *delete_one2m_row_new(
		char *application_name,
		char *role_name,
		boolean update_null_boolean,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		char *many_folder_table_name,
		PROCESS *post_change_process,
		LIST *query_row_cell_list,
		LIST *relation_one2m_list )
{
	DELETE_ONE2M_ROW *delete_one2m_row;

	if ( !application_name
	||   !role_name
	||   !appaserver_error_filename
	||   !appaserver_parameter_mount_point
	||   !many_folder_table_name
	||   !list_length( query_row_cell_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delete_one2m_row = delete_one2m_row_calloc();

	delete_one2m_row->delete_where =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		delete_where(
			query_row_cell_list /* primary_query_cell_list */,
			(char *)0 /* security_entity_where */ );

	delete_one2m_row->delete_sql_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		delete_sql_statement(
			many_folder_table_name,
			delete_one2m_row->delete_where );

	if ( post_change_process )
	{
		delete_one2m_row->pre_command_line =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			delete_command_line(
				APPASERVER_PREDELETE_STATE,
				query_row_cell_list
					/* primary_query_cell_list */,
				post_change_process->command_line,
				appaserver_error_filename );

		delete_one2m_row->command_line =
			delete_command_line(
				APPASERVER_DELETE_STATE,
				query_row_cell_list
					/* primary_query_cell_list */,
				post_change_process->command_line,
				appaserver_error_filename );
	}

	if ( list_length( relation_one2m_list ) )
	{
		delete_one2m_row->delete_one2m_list =
			delete_one2m_list_new(
				application_name,
				role_name,
				update_null_boolean,
				appaserver_error_filename,
				appaserver_parameter_mount_point,
				query_row_cell_list
					/* primary_query_cell_list */,
				relation_one2m_list );
	}

	return delete_one2m_row;
}

DELETE_ONE2M_ROW *delete_one2m_row_calloc( void )
{
	DELETE_ONE2M_ROW *delete_one2m_row;

	if ( ! ( delete_one2m_row = calloc( 1, sizeof ( DELETE_ONE2M_ROW ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return delete_one2m_row;
}

DELETE_ONE2M_FETCH *delete_one2m_fetch_new(
		char *application_name,
		char *role_name,
		boolean update_null_boolean,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		char *many_folder_table_name,
		PROCESS *post_change_process,
		LIST *query_fetch_row_list,
		LIST *relation_one2m_list )
{
	DELETE_ONE2M_FETCH *delete_one2m_fetch;
	QUERY_ROW *query_row;
	DELETE_ONE2M_ROW *delete_one2m_row;

	if ( !application_name
	||   !role_name
	||   !appaserver_error_filename
	||   !appaserver_parameter_mount_point
	||   !many_folder_table_name
	||   !list_rewind( query_fetch_row_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delete_one2m_fetch = delete_one2m_fetch_calloc();

	delete_one2m_fetch->delete_one2m_row_list = list_new();

	do {
		query_row = list_get( query_fetch_row_list );

		delete_one2m_row =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			delete_one2m_row_new(
				application_name,
				role_name,
				update_null_boolean,
				appaserver_error_filename,
				appaserver_parameter_mount_point,
				many_folder_table_name,
				post_change_process,
				query_row->cell_list,
				relation_one2m_list );

		list_set(
			delete_one2m_fetch->delete_one2m_row_list,
			delete_one2m_row );

	} while ( list_next( query_fetch_row_list ) );

	return delete_one2m_fetch;
}

DELETE_ONE2M_FETCH *delete_one2m_fetch_calloc( void )
{
	DELETE_ONE2M_FETCH *delete_one2m_fetch;

	if ( ! ( delete_one2m_fetch =
			calloc( 1,
				sizeof ( DELETE_ONE2M_FETCH ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return delete_one2m_fetch;
}

DELETE_ONE2M_LIST *delete_one2m_list_new(
		char *application_name,
		char *role_name,
		boolean update_null_boolean,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		LIST *primary_query_cell_list,
		LIST *relation_one2m_list )
{
	DELETE_ONE2M_LIST *delete_one2m_list;
	RELATION_ONE2M *relation_one2m;
	DELETE_ONE2M *delete_one2m;

	if ( !application_name
	||   !role_name
	||   !appaserver_error_filename
	||   !appaserver_parameter_mount_point
	||   !list_length( primary_query_cell_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !list_rewind( relation_one2m_list ) ) return NULL;

	delete_one2m_list = delete_one2m_list_calloc();

	do {
		relation_one2m = list_get( relation_one2m_list );

		if ( !relation_one2m->many_folder
		||   !list_length(
			relation_one2m->
				many_folder->
				folder_attribute_primary_key_list )
		||   !list_length(
			relation_one2m->
				relation_foreign_key_list ) )
		{
			char message[ 128 ];

			sprintf(message,
				"relation_one2m is incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		/* Cache PROCESS *post_change_process */
		/* ---------------------------------- */
		if (	relation_one2m->
				many_folder->
				post_change_process_name
		&&	!relation_one2m->
				many_folder->
				post_change_process )
		{
			relation_one2m->many_folder->post_change_process =
				/* -------------- */
				/* Safely returns */
				/* -------------- */
				process_fetch(
					relation_one2m->
						many_folder->
						post_change_process_name,
					(char *)0 /* document_root */,
					(char *)0 /* relative_source */,
					1 /* check_inside */,
					appaserver_parameter_mount_point );
		}

		delete_one2m =
			delete_one2m_new(
				application_name,
				role_name,
				update_null_boolean,
				appaserver_error_filename,
				appaserver_parameter_mount_point,
				primary_query_cell_list
					/* one_folder_query_cell_list */,
				relation_one2m->many_folder_name,
				relation_one2m->
				    many_folder->
				    folder_attribute_primary_list
				    /* many_folder_attribute_primary_list */,
				relation_one2m->
					many_folder->
					folder_attribute_primary_key_list
					/* many_folder_primary_key_list */,
				relation_one2m->relation_foreign_key_list,
				relation_one2m->foreign_key_none_primary,
				relation_one2m->
					many_folder->
					post_change_process );

		if ( delete_one2m )
		{
			if ( !delete_one2m_list->list )
			{
				delete_one2m_list->list = list_new();
			}

			list_set(
				delete_one2m_list->list,
				delete_one2m );
		}

	} while ( list_next( relation_one2m_list ) );

	if ( !list_length( delete_one2m_list->list ) )
	{
		free( delete_one2m_list );
		delete_one2m_list = NULL;
	}

	return delete_one2m_list;
}

DELETE_ONE2M_LIST *delete_one2m_list_calloc( void )
{
	DELETE_ONE2M_LIST *delete_one2m_list;

	if ( ! ( delete_one2m_list =
			calloc( 1,
			sizeof ( DELETE_ONE2M_LIST ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return delete_one2m_list;
}

DELETE_MTO1_ISA_LIST *delete_mto1_isa_list_new(
		char *application_name,
		char *role_name,
		boolean update_null_boolean,
		char *appaserver_error_filename,
		char *appaserver_parameter_mount_point,
		LIST *primary_query_cell_list,
		LIST *relation_mto1_isa_list )
{
	DELETE_MTO1_ISA_LIST *delete_mto1_isa_list;
	RELATION_MTO1 *relation_mto1_isa;
	DELETE_MTO1_ISA *delete_mto1_isa;

	if ( !application_name
	||   !role_name
	||   !appaserver_error_filename
	||   !appaserver_parameter_mount_point
	||   !list_length( primary_query_cell_list )
	||   !list_rewind(relation_mto1_isa_list ) )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delete_mto1_isa_list = delete_mto1_isa_list_calloc();
	delete_mto1_isa_list->list = list_new();

	do {
		relation_mto1_isa = list_get( relation_mto1_isa_list );

		if ( !relation_mto1_isa->one_folder_name
		||   !relation_mto1_isa->one_folder
		||   !list_length(
			relation_mto1_isa->
				relation_foreign_key_list ) )
		{
			char message[ 128 ];

			sprintf(message,
				"relation_mto1_isa is incomplete." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		delete_mto1_isa =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			delete_mto1_isa_new(
				application_name,
				role_name,
				update_null_boolean,
				appaserver_error_filename,
				appaserver_parameter_mount_point,
				primary_query_cell_list,
				relation_mto1_isa->one_folder_name,
				relation_mto1_isa->
					one_folder->
					folder_attribute_primary_key_list,
				relation_mto1_isa->
					one_folder->
					post_change_process_name );

		list_set(
			delete_mto1_isa_list->list,
			delete_mto1_isa );

	} while ( list_next( relation_mto1_isa_list ) );

	return delete_mto1_isa_list;
}

DELETE_MTO1_ISA_LIST *delete_mto1_isa_list_calloc( void )
{
	DELETE_MTO1_ISA_LIST *delete_mto1_isa_list;

	if ( ! ( delete_mto1_isa_list =
			calloc( 1,
				sizeof ( DELETE_MTO1_ISA_LIST ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return delete_mto1_isa_list;
}

char *delete_one2m_table_name(
		char *application_name,
		char *many_folder_name )
{
	if ( !application_name
	||   !many_folder_name )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return
	strdup(
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		folder_table_name(
			application_name,
			many_folder_name ) );
}

char *delete_message_string(
		const char *message,
		unsigned int delete_row_count,
		unsigned int delete_update_cell_count )
{
	char message_string[ 256 ];

	snprintf(
		message_string,
		sizeof ( message_string ),
		message,
		delete_row_count,
		delete_update_cell_count );

	return strdup( message_string );
}

LIST *delete_one2m_query_fetch_list( LIST *delete_one2m_list )
{
	LIST *list = list_new();
	DELETE_ONE2M *delete_one2m;

	if ( list_rewind( delete_one2m_list ) )
	do {
		delete_one2m = list_get( delete_one2m_list );

		if ( !delete_one2m->query_fetch )
		{
			char message[ 128 ];

			sprintf(message,
				"delete_one2m->query_fetch is empty." );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		list_set(
			list,
			delete_one2m->query_fetch );

	} while ( list_next( delete_one2m_list ) );

	if ( !list_length( list ) )
	{
		list_free( list );
		list = NULL;
	}

	return list;
}

char *delete_key_list_data_string_sql(
		const char sql_delimiter,
		char *table_name,
		LIST *key_list,
		char *data_string )
{
	char sql[ 2048 ];
	char *ptr = sql;
	int p = 0;
	char *key;
	char datum[ 1024 ];
	char *escape;

	if ( !table_name
	||   !list_rewind( key_list )
	||   !data_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	ptr += sprintf(
		ptr,
		"delete from %s where ",
		table_name );

	do {
		key = list_get( key_list );

		if ( p ) ptr += sprintf( ptr, " and " );

		/* Returns null if not enough delimiters */
		/* ------------------------------------- */
		if ( !piece(	datum,
				sql_delimiter,
				data_string,
				p ) )
		{
			char message[ 128 ];

			sprintf(message,
				"piece(%s,%d) returned empty.",
				data_string,
				p );

			appaserver_error_stderr_exit(
				__FILE__,
				__FUNCTION__,
				__LINE__,
				message );
		}

		escape =
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			security_sql_injection_escape(
				SECURITY_ESCAPE_CHARACTER_STRING,
				datum );

		ptr += sprintf(
			ptr,
			"%s = '%s'",
			key,
			escape );

		free( escape );
		p++;

	} while ( list_next( key_list ) );

	ptr += sprintf( ptr, ";" );

	return strdup( sql );
}

LIST *delete_sql_list(
		DELETE_ROOT *delete_root,
		DELETE_ONE2M_LIST *delete_one2m_list,
		DELETE_MTO1_ISA_LIST *delete_mto1_isa_list )
{
	LIST *sql_list = list_new();
	DELETE_SQL *delete_sql;

	if ( !delete_root )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"delete_root is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delete_sql =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		delete_sql_new(
			delete_root->delete_sql_statement,
			delete_root->delete_pre_command_line,
			delete_root->delete_command_line );

	(void)delete_sql_getset(
		sql_list /* in/out */,
		delete_sql /* candidate_delete_sql */ );

	if ( delete_one2m_list )
	{
		delete_sql_one2m(
			sql_list /* in/out */,
			delete_one2m_list->list
				/* delete_one2m_list */ );
	}

	if ( delete_mto1_isa_list )
	{
		delete_sql_mto1_isa(
			sql_list /* in/out */,
			delete_mto1_isa_list->list
				/* delete_mto1_isa_list */ );
	}

	return sql_list;
}

DELETE_SQL *delete_sql_new(
		char *statement,
		char *pre_command_line,
		char *command_line )
{
	DELETE_SQL *delete_sql;

	if ( !statement )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"statement is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	delete_sql = delete_sql_calloc();

	delete_sql->statement = statement;
	delete_sql->pre_command_line = pre_command_line;
	delete_sql->command_line = command_line;

	return delete_sql;
}

DELETE_SQL *delete_sql_calloc( void )
{
	DELETE_SQL *delete_sql;

	if ( ! ( delete_sql = calloc( 1, sizeof ( DELETE_SQL ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return delete_sql;
}

boolean delete_sql_getset(
		LIST *delete_sql_list /* in/out */,
		DELETE_SQL *candidate_delete_sql )
{
	DELETE_SQL *delete_sql;

	if ( !delete_sql_list
	||   !candidate_delete_sql )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( delete_sql_list ) )
	do {
		delete_sql = list_get( delete_sql_list );

		if ( delete_sql_duplicate_boolean(
			delete_sql,
			candidate_delete_sql ) )
		{
			return 0;
		}
	} while ( list_next( delete_sql_list ) );

	list_set( delete_sql_list, candidate_delete_sql );

	return 1;
}

boolean delete_sql_duplicate_boolean(
		DELETE_SQL *delete_sql,
		DELETE_SQL *candidate_delete_sql )
{
	boolean duplicate_boolean = 0;

	if ( !delete_sql
	||   !candidate_delete_sql )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( strcmp(
		delete_sql->statement,
		candidate_delete_sql->statement ) == 0 )
	{
		duplicate_boolean = 1;
	}

	return duplicate_boolean;
}

void delete_sql_free( DELETE_SQL *delete_sql )
{
	if ( !delete_sql )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"delete_sql is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	free( delete_sql );
}

void delete_sql_one2m(
		LIST *delete_sql_list /* in/out */,
		LIST *delete_one2m_list )
{
	DELETE_ONE2M *delete_one2m;

	if ( !delete_sql_list )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"delete_sql_list is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( delete_one2m_list ) )
	do {
		delete_one2m = list_get( delete_one2m_list );

		if ( !delete_one2m->delete_one2m_fetch
		&&   !delete_one2m->delete_one2m_update )
		{
			continue;
		}

		delete_sql_one2m_delete_update(
			delete_sql_list /* in/out */,
			delete_one2m->delete_one2m_fetch,
			delete_one2m->delete_one2m_update );

	} while ( list_next( delete_one2m_list ) );
}

void delete_sql_one2m_delete_update(
		LIST *delete_sql_list /* in/out */,
		DELETE_ONE2M_FETCH *delete_one2m_fetch,
		DELETE_ONE2M_UPDATE *delete_one2m_update )
{
	if ( delete_one2m_fetch )
	{
		delete_sql_one2m_delete(
			delete_sql_list /* in/out */,
			delete_one2m_fetch->
				delete_one2m_row_list );
	}
	else
	{
		delete_sql_one2m_update(
			delete_sql_list /* in/out */,
			delete_one2m_update->
				delete_one2m_update_row_list );
	}
}

void delete_sql_one2m_delete(
		LIST *delete_sql_list /* in/out */,
		LIST *delete_one2m_row_list )
{
	DELETE_ONE2M_ROW *delete_one2m_row;
	DELETE_SQL *delete_sql;

	if ( list_rewind( delete_one2m_row_list ) )
	do {
		delete_one2m_row = list_get( delete_one2m_row_list );

		delete_sql =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			delete_sql_new(
				delete_one2m_row->delete_sql_statement,
				delete_one2m_row->pre_command_line,
				delete_one2m_row->command_line );

		if ( !delete_sql_getset(
			delete_sql_list /* in/out */,
			delete_sql /* candidate_delete_sql */ ) )
		{
			delete_sql_free( delete_sql );
		}

		if ( delete_one2m_row->delete_one2m_list )
		{
			delete_sql_one2m(
				delete_sql_list /* in/out */,
				delete_one2m_row->
					delete_one2m_list->list
						/* delete_one2m_list */ );
		}

	} while ( list_next( delete_one2m_row_list ) );
}

void delete_sql_one2m_update(
		LIST *delete_sql_list /* in/out */,
		LIST *delete_one2m_update_row_list )
{
	DELETE_ONE2M_UPDATE_ROW *delete_one2m_update_row;

	if ( list_rewind( delete_one2m_update_row_list ) )
	do {
		delete_one2m_update_row =
			list_get(
				delete_one2m_update_row_list );

		delete_sql_update_cell(
			delete_sql_list /* in/out */,
			delete_one2m_update_row->
				delete_one2m_update_cell_list );

	} while ( list_next( delete_one2m_update_row_list ) );
}

void delete_sql_update_cell(
		LIST *delete_sql_list /* in/out */,
		LIST *delete_one2m_update_cell_list )
{
	DELETE_ONE2M_UPDATE_CELL *delete_one2m_update_cell;
	DELETE_SQL *delete_sql;

	if ( list_rewind( delete_one2m_update_cell_list ) )
	do {
		delete_one2m_update_cell =
			list_get(
				delete_one2m_update_cell_list );

		delete_sql =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			delete_sql_new(
				delete_one2m_update_cell->sql_statement,
				(char *)0 /* pre_command_line */,
				delete_one2m_update_cell->command_line );

		if ( !delete_sql_getset(
			delete_sql_list /* in/out */,
			delete_sql /* candidate_delete_sql */ ) )
		{
			delete_sql_free( delete_sql );
		}

	} while ( list_next( delete_one2m_update_cell_list ) );
}

void delete_sql_mto1_isa(
		LIST *delete_sql_list /* in/out */,
		LIST *delete_mto1_isa_list )
{
	DELETE_MTO1_ISA *delete_mto1_isa;
	DELETE_SQL *delete_sql;

	if ( list_rewind( delete_mto1_isa_list ) )
	do {
		delete_mto1_isa = list_get( delete_mto1_isa_list );

		delete_sql =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			delete_sql_new(
				delete_mto1_isa->delete_sql_statement,
				delete_mto1_isa->pre_command_line,
				delete_mto1_isa->command_line );

		if ( !delete_sql_getset(
			delete_sql_list /* in/out */,
			delete_sql /* candidate_delete_sql */ ) )
		{
			delete_sql_free( delete_sql );
		}

		if ( delete_mto1_isa->delete_one2m_list )
		{
			delete_sql_one2m(
				delete_sql_list /* in/out */,
				delete_mto1_isa->
					delete_one2m_list->list
						/* delete_one2m_list */ );
		}

	} while ( list_next( delete_mto1_isa_list ) );
}

unsigned int delete_row_count( LIST *delete_sql_list )
{
	unsigned int row_count = 0;
	DELETE_SQL *delete_sql;

	if ( list_rewind( delete_sql_list ) )
	do {
		delete_sql = list_get( delete_sql_list );

		if ( string_strncmp(
			delete_sql->statement,
			"delete " ) == 0 )
		{
			row_count++;
		}

	} while ( list_next( delete_sql_list ) );

	return row_count;
}

unsigned int delete_update_cell_count( LIST *delete_sql_list )
{
	unsigned int update_cell_count = 0;
	DELETE_SQL *delete_sql;

	if ( list_rewind( delete_sql_list ) )
	do {
		delete_sql = list_get( delete_sql_list );

		if ( string_strncmp(
			delete_sql->statement,
			"update " ) == 0 )
		{
			update_cell_count++;
		}

	} while ( list_next( delete_sql_list ) );

	return update_cell_count;
}

