/* $APPASERVER_HOME/library/delete.c			*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

	delete;
}

DELETE *delete_new(	char *application_name,
			char *login_name,
			char *role_name,
			LIST *primary_data_list,
			boolean dont_delete_mto1_isa,
			char *folder_name )
{
	DELETE *delete = delete_calloc();

	delete->application_name = application_name;
	delete->login_name = login_name;
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


	return delete;

}

LIST *delete_database_get_delete_folder_list(
				char *application_name,
				char *folder_name,
				PROCESS *post_change_process,
				LIST *primary_key_list,
				boolean row_level_non_owner_forbid,
				boolean row_level_non_owner_view_only,
				LIST *mto1_isa_related_folder_list,
				LIST *one2m_recursive_related_folder_list,
				boolean dont_delete_mto1_isa,
				LIST *primary_attribute_data_list,
				char *login_name )
{
	LIST *delete_folder_list = list_new_list();
	DELETE_FOLDER *delete_folder;
	DELETE_ROW *delete_row;

	delete_folder =
		delete_database_delete_folder_new(
			folder_name,
			0 /* not null_1tom_upon_delete */,
			post_change_process,
			primary_key_list );

	/* Make sure user didn't alter the web page source. */
	/* ------------------------------------------------ */
	if ( row_level_non_owner_forbid
	||   row_level_non_owner_view_only )
	{
		LIST *delete_row_list;


fprintf( stderr, "%s/%s()/%d\n",
__FILE__,
__FUNCTION__,
__LINE__ );

		delete_row_list =
			delete_database_get_delete_row_list(
				application_name,
				folder_name,
				row_level_non_owner_forbid,
				row_level_non_owner_view_only,
				primary_key_list
					/* select_attribute_name_list */,
				primary_key_list
					/* where_attribute_name_list */,
				primary_attribute_data_list
					/* where_attribute_data_list */,
				login_name );

		/* If list_length() > 1 then user altered web page source. */
		/* ------------------------------------------------------- */
		if ( list_length( delete_row_list ) != 1 )
		{
			return (LIST *)0;
		}

		delete_row = list_get_first_pointer( delete_row_list );
	}
	else
	{
		delete_row =
			delete_database_delete_row_new(
				primary_attribute_data_list );
	}

	delete_folder->delete_row_list = list_new();
	list_append_pointer( delete_folder->delete_row_list, delete_row );
	list_append_pointer( delete_folder_list, delete_folder );

	if ( !dont_delete_mto1_isa
	&&   list_length( mto1_isa_related_folder_list ) )
	{
		delete_database_append_mto1_isa_delete_folder_list(
			delete_folder_list,
			application_name,
			mto1_isa_related_folder_list,
			delete_row->primary_attribute_data_list );
	}

	if ( list_length( one2m_recursive_related_folder_list ) )
	{
		delete_database_append_one2m_related_folder_list(
			delete_folder_list,
			one2m_recursive_related_folder_list,
			application_name,
			delete_row->primary_attribute_data_list,
			login_name );
	}

	return delete_folder_list;

}

void delete_database_append_one2m_delete_folder_list(
				LIST *delete_folder_list,
				char *application_name,
				char *one2m_related_folder_name,
				LIST *where_attribute_name_list,
				LIST *where_attribute_data_list,
				char *login_name,
				LIST *one2m_related_folder_list,
				LIST *primary_key_list,
				PROCESS *post_change_process )
{
	LIST *delete_row_list;
	DELETE_ROW *delete_row;
	DELETE_FOLDER *delete_folder;
	RELATED_FOLDER *related_folder;

	delete_row_list =
		delete_database_get_delete_row_list(
			application_name,
			one2m_related_folder_name,
			0 /* row_level_non_owner_forbid */,
			0 /* row_level_non_owner_view_only */,
			primary_key_list
				/* select_attribute_name_list */,
			where_attribute_name_list,
			where_attribute_data_list,
			login_name );

	if ( !list_length( delete_row_list ) ) return;

	delete_folder =
		delete_database_get_or_set_delete_folder(
			delete_folder_list,
			one2m_related_folder_name,
			post_change_process,
			primary_key_list );

	if ( !delete_folder->delete_row_list )
		delete_folder->delete_row_list = list_new();

	list_append_list(
		delete_folder->delete_row_list,
		delete_row_list );

	if ( !list_rewind( one2m_related_folder_list ) ) return;

	do {
		related_folder =
			list_get(
				one2m_related_folder_list );

		related_folder->one2m_folder =
			folder_with_load_new(
				application_name,
				BOGUS_SESSION, 
				related_folder->
					one2m_folder->
					folder_name,
				(ROLE *)0 );

		list_rewind( delete_row_list );

		do {
			delete_row = list_get( delete_row_list );

			delete_database_append_one2m_delete_folder_list(
				delete_folder_list,
				application_name,
				related_folder->
					one2m_folder->
					folder_name,
				related_folder->
					foreign_attribute_name_list
					/* where_attribute_name_list */,
				delete_row->primary_attribute_data_list
					/* where_attribute_data_list */,
				login_name,
				related_folder->
					one2m_folder->
					one2m_related_folder_list,
				related_folder->
					one2m_folder->
					primary_key_list,
				related_folder->
					one2m_folder->
					post_change_process );

		} while( list_next( delete_row_list ) );

	} while( list_next( one2m_related_folder_list ) );

}

void delete_database_append_mto1_isa_delete_folder_list(
				LIST *delete_folder_list,
				char *application_name,
				LIST *mto1_isa_related_folder_list,
				LIST *primary_attribute_data_list )
{
	DELETE_FOLDER *delete_folder;
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( mto1_isa_related_folder_list ) ) return;

	do {
		related_folder = list_get( mto1_isa_related_folder_list );

		delete_folder =
			delete_database_get_or_set_delete_folder(
				delete_folder_list,
				related_folder->folder->folder_name,
				related_folder->folder->post_change_process,
				related_folder->
					folder->
					primary_key_list );

fprintf( stderr, "%s/%s()/%d\n",
__FILE__,
__FUNCTION__,
__LINE__ );

		delete_folder->delete_row_list =
			delete_database_get_delete_row_list(
				application_name,
				related_folder->folder->folder_name,
				related_folder->
					folder->
					row_level_non_owner_forbid,
				related_folder->
					folder->
					row_level_non_owner_view_only,
				related_folder->
					folder->
					primary_key_list
					/* select_attribute_name_list */,
				related_folder->
					folder->
					primary_key_list
					/* where_attribute_name_list */,
				primary_attribute_data_list
					/* where_attribute_data_list */,
				(char *)0 /* login_name */ );

		if ( list_length(
			related_folder->
				folder->
				one2m_related_folder_list ) )
		{
			delete_database_append_one2m_related_folder_list(
				delete_folder_list,
				related_folder->
					folder->
					one2m_related_folder_list,
				application_name,
				primary_attribute_data_list,
				(char *)0 /* login_name */ );
		}

	} while( list_next( mto1_isa_related_folder_list ) );

}

DELETE_ROW *delete_database_delete_row_new(
			LIST *primary_attribute_data_list )
{
	DELETE_ROW *delete_row;

	if ( ! ( delete_row =
			(DELETE_ROW *)
				calloc( 1, sizeof( DELETE_ROW ) ) ) )
	{
		fprintf(stderr,
			"ERROR: memory allocation failed in %s/%s()/%d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	delete_row->primary_attribute_data_list = primary_attribute_data_list;

	return delete_row;

}

DELETE_FOLDER *delete_database_get_or_set_delete_folder(
			LIST *delete_folder_list,
			char *folder_name,
			PROCESS *post_change_process,
			LIST *primary_key_list )
{
	DELETE_FOLDER *delete_folder;

	if ( !delete_folder_list )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: delete_folder_list not initialized.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( list_rewind( delete_folder_list ) )
	{
		do {
			delete_folder = list_get( delete_folder_list );

			/* ------------------------------------ */
			/* Can duplicate folders		*/
			/* if setting an attribute to null.     */
			/* ------------------------------------ */
			if ( delete_folder->null_1tom_upon_delete )
				continue;

			if ( strcmp(	delete_folder->folder_name,
					folder_name ) == 0 )
			{
				return delete_folder;
			}
		} while( list_next( delete_folder_list ) );

	}

	delete_folder =
		delete_database_delete_folder_new(
			folder_name,
			0 /* not null_1tom_upon_delete */,
			post_change_process,
			primary_key_list );

	list_append_pointer( delete_folder_list, delete_folder );

	return delete_folder;

}

DELETE_FOLDER *delete_database_delete_folder_new(
			char *folder_name,
			boolean null_1tom_upon_delete,
			PROCESS *post_change_process,
			LIST *primary_key_list )
{
	DELETE_FOLDER *delete_folder;

	if ( ! ( delete_folder =
			(DELETE_FOLDER *)
				calloc( 1, sizeof( DELETE_FOLDER ) ) ) )
	{
		fprintf(stderr,
			"ERROR: memory allocation failed in %s/%s()/%d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	delete_folder->folder_name = folder_name;
	delete_folder->null_1tom_upon_delete = null_1tom_upon_delete;
	delete_folder->post_change_process = post_change_process;
	delete_folder->primary_key_list =
		primary_key_list;

	return delete_folder;

}

LIST *delete_database_get_delete_row_list(
			char *application_name,
			char *folder_name,
			boolean row_level_non_owner_forbid,
			boolean row_level_non_owner_view_only,
			LIST *select_attribute_name_list,
			LIST *where_attribute_name_list,
			LIST *where_attribute_data_list,
			char *login_name )
{
	char sys_string[ 2048 ];
	char *where_clause;
	char input_buffer[ 1024 ];
	LIST *delete_row_list;
	FILE *input_pipe;
	DELETE_ROW *delete_row;
	LIST *primary_attribute_data_list;

	if ( !list_length( select_attribute_name_list )
	||   !list_length( where_attribute_name_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: length select_attribute_name_list = %d; length where_attribute_name_list = %d.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 list_length( select_attribute_name_list ),
			 list_length( where_attribute_name_list ) );
		exit( 1 );
	}

	if ( !row_level_non_owner_forbid
	&&   !row_level_non_owner_view_only )
	{
		login_name = (char *)0;
	}

	where_clause =
		query_login_name_where_clause(
			folder_new(
				application_name,
				BOGUS_SESSION,
				folder_name ),
			where_attribute_name_list,
			where_attribute_data_list,
			login_name );

	delete_row_list = list_new();

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=%s		"
		 "			where=\"%s\"		",
		 application_name,
		 list_display( select_attribute_name_list ),
		 folder_name,
		 where_clause );


/*
fprintf( stderr, "%s/%s()/%d: got sys_string = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
sys_string );
*/

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		primary_attribute_data_list =
			list_string2list(
				input_buffer,
				FOLDER_DATA_DELIMITER );

		delete_row =
			delete_database_delete_row_new(
				primary_attribute_data_list );

		list_append_pointer( delete_row_list, delete_row );

	}

	pclose( input_pipe );
	return delete_row_list;

}

void delete_database_execute_delete_folder_list(
			char *application_name,
			LIST *delete_folder_list,
			char *sql_executable )
{
	DELETE_FOLDER *delete_folder;

	if ( !list_go_tail( delete_folder_list ) ) return;

	do {
		delete_folder = list_get_pointer( delete_folder_list );

		if ( delete_folder->null_1tom_upon_delete )
		{
			delete_database_execute_update_to_null_row(
				application_name,
				delete_folder->folder_name,
				delete_folder->delete_row_list,
				delete_folder->
					primary_key_list,
				delete_folder->
					update_null_attribute_name_list,
				sql_executable );
		}
		else
		{
			delete_database_execute_delete_row_list(
				application_name,
				delete_folder->folder_name,
				delete_folder->delete_row_list,
				delete_folder->
					primary_key_list,
				sql_executable,
				delete_folder->post_change_process );
		}

	} while( list_previous( delete_folder_list ) );

}

void delete_database_set_parameter_dictionary(
			DICTIONARY *parameter_dictionary,
			LIST *primary_key_list,
			LIST *primary_attribute_data_list )
{
	char *primary_attribute;
	char *primary_data;

	if ( !list_rewind( primary_key_list ) )
		return;

	if ( !list_rewind( primary_attribute_data_list ) )
		return;

	do {
		primary_attribute =
			list_get_pointer(
				primary_key_list );

		primary_data =
			list_get_pointer(
				primary_attribute_data_list );

		dictionary_set_pointer(
			parameter_dictionary,
			primary_attribute,
			primary_data );

		list_next( primary_attribute_data_list );

	} while( list_next( primary_key_list ) );

}

void delete_database_execute_delete_row_list(
			char *application_name,
			char *folder_name,
			LIST *delete_row_list,
			LIST *primary_key_list,
			char *sql_executable,
			PROCESS *post_change_process )
{
	char sys_string[ 4096 ];
	char *table_name;
	DICTIONARY *parameter_dictionary = dictionary_small();
	FILE *output_pipe;
	FILE *stderr_pipe;

	if ( timlib_strncmp( sql_executable, "html_paragraph_wrapper" ) == 0 )
	{
		post_change_process = (PROCESS *)0;
	}

	if ( !list_length( delete_row_list )
	||   !list_length( primary_key_list ) )
	{
		return;
	}

	table_name = get_table_name( application_name, folder_name );

	sprintf( sys_string,
		 "delete_statement.e table=%s field=%s del='^' |"
		 "%s 					      ",
		 table_name,
		 list_display_delimited(
			primary_key_list,
			',' ),
		 sql_executable );

	output_pipe = popen( sys_string, "w" );

	sprintf( sys_string,
		 "delete_statement.e table=%s field=%s del='^' 1>&2",
		 table_name,
		 list_display_delimited(
			primary_key_list,
			',' ) );

	stderr_pipe = popen( sys_string, "w" );

	delete_database_execute_delete_row_list_pipe(
			output_pipe,
			stderr_pipe,
			parameter_dictionary,
			application_name,
			folder_name,
			delete_row_list,
			primary_key_list,
			post_change_process );

	pclose( output_pipe );
	pclose( stderr_pipe );
	dictionary_free( parameter_dictionary );

}

void delete_database_execute_delete_row_list_pipe(
			FILE *output_pipe,
			FILE *stderr_pipe,
			DICTIONARY *parameter_dictionary,
			char *application_name,
			char *folder_name,
			LIST *delete_row_list,
			LIST *primary_key_list,
			PROCESS *post_change_process )
{
	DELETE_ROW *delete_row;

	if ( !list_rewind( delete_row_list ) )
	{
		return;
	}

	do {
		delete_row = list_get_pointer( delete_row_list );

		if ( post_change_process )
		{
			delete_database_set_parameter_dictionary(
				parameter_dictionary,
				primary_key_list,
				delete_row->primary_attribute_data_list );

			/* Reload the command_line each row */
			/* -------------------------------- */
			free( post_change_process->executable );

			process_load_executable(
				&post_change_process->executable,
				post_change_process->process_name,
				application_name );

			process_convert_parameters(
				&post_change_process->executable,
				application_name,
				(char *)0 /* session */,
				"predelete" /* state */,
				(char *)0 /* login_name */,
				folder_name,
				(char *)0 /* role_name */,
				(char *)0 /* target_frame */,
				parameter_dictionary,
				parameter_dictionary
					/* where_clause_dictionary */,
				(LIST *)0 /* attribute_list */,
				(LIST *)0 /* prompt_list */,
				primary_key_list,
				(LIST *)0 /* primary_data_list */,
				0 /* row */,
				(char *)0 /* process_name */,
				(PROCESS_SET *)0,
				(char *)0 /* one2m_folder_name_for_processes */,
				(char *)0 /* operation_row_count_string */,
				(char *)0 /* prompt */ );

			if ( system( post_change_process->executable ) ){};
		}

		fprintf( output_pipe,
			 "%s\n", 
			 list_display_delimited(
				delete_row->primary_attribute_data_list,
				FOLDER_DATA_DELIMITER ) );
		fflush( output_pipe );

		fprintf( stderr_pipe,
			 "%s\n", 
			 list_display_delimited(
				delete_row->primary_attribute_data_list,
				FOLDER_DATA_DELIMITER ) );
		fflush( stderr_pipe );

		if ( post_change_process )
		{
			/* Reload the command_line each row */
			/* -------------------------------- */
			free( post_change_process->executable );

			process_load_executable(
				&post_change_process->executable,
				post_change_process->process_name,
				application_name );

			process_convert_parameters(
				&post_change_process->executable,
				application_name,
				(char *)0 /* session */,
				"delete" /* state */,
				(char *)0 /* login_name */,
				folder_name,
				(char *)0 /* role_name */,
				(char *)0 /* target_frame */,
				parameter_dictionary,
				parameter_dictionary
					/* where_clause_dictionary */,
				(LIST *)0 /* attribute_list */,
				(LIST *)0 /* prompt_list */,
				primary_key_list,
				(LIST *)0 /* primary_data_list */,
				0 /* row */,
				(char *)0 /* process_name */,
				(PROCESS_SET *)0,
				(char *)0 /* one2m_folder_name_for_processes */,
				(char *)0 /* operation_row_count_string */,
				(char *)0 /* prompt */ );

			if ( system( post_change_process->executable ) ){};
		}

	} while( list_next( delete_row_list ) );

}

void delete_database_execute_update_to_null_row(
			char *application_name,
			char *folder_name,
			LIST *delete_row_list,
			LIST *primary_key_list,
			LIST *update_null_attribute_name_list,
			char *sql_executable )
{
	char sys_string[ 4096 ];
	char *table_name;
	FILE *output_pipe;
	FILE *stderr_pipe;

	if ( !list_rewind( delete_row_list ) )
	{
		return;
	}

	table_name = get_table_name( application_name, folder_name );

	sprintf(sys_string,
 		"update_statement.e table=%s key=%s carrot=y |"
 		"%s 					      ",
 		table_name,
		list_display_delimited(
			primary_key_list,
			',' ),
		sql_executable );

	output_pipe = popen( sys_string, "w" );

	sprintf(sys_string,
 		"update_statement.e table=%s key=%s carrot=y 1>&2",
 		table_name,
		list_display_delimited(
			primary_key_list,
			',' ) );

	stderr_pipe = popen( sys_string, "w" );

	delete_database_execute_update_to_null_row_pipe(
			output_pipe,
			stderr_pipe,
			delete_row_list,
			update_null_attribute_name_list );

	pclose( output_pipe );
	pclose( stderr_pipe );

}

void delete_database_execute_update_to_null_row_pipe(
			FILE *output_pipe,
			FILE *stderr_pipe,
			LIST *delete_row_list,
			LIST *update_null_attribute_name_list )
{
	DELETE_ROW *delete_row;
	char *null_attribute_name;

	if ( !list_rewind( delete_row_list ) )
	{
		return;
	}

	do {
		delete_row = list_get_pointer( delete_row_list );

		if ( !list_rewind( update_null_attribute_name_list ) )
		{
			fprintf( stderr,
		"ERROR in %s/%s()/%d: empty update_null_attribute_name_list.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}

		do {
			null_attribute_name =
				list_get_pointer(
					update_null_attribute_name_list );

			fprintf(output_pipe,
				"%s^%s^\n",
				list_display_delimited(
					delete_row->primary_attribute_data_list,
					FOLDER_DATA_DELIMITER ),
				null_attribute_name );

			fprintf(stderr_pipe,
				"%s^%s^\n",
				list_display_delimited(
					delete_row->primary_attribute_data_list,
					FOLDER_DATA_DELIMITER ),
				null_attribute_name );

		} while( list_next( update_null_attribute_name_list ) );

	} while( list_next( delete_row_list ) );

}

char *delete_database_display(
			DELETE_DATABASE *delete_database )
{
	DELETE_FOLDER *delete_folder;
	DELETE_ROW *delete_row;
	char return_string[ 65536 ];
	char *ptr = return_string;

	ptr += sprintf( ptr,
"folder_name = %s; primary_attribute_data_list = %s; dont_delete_mto1_isa = %d\n",
			delete_database->folder->folder_name,
			list_display(
				delete_database->
					primary_attribute_data_list ),
			delete_database->dont_delete_mto1_isa );

	if ( !list_rewind( delete_database->delete_folder_list ) ) return "";

	do {
		delete_folder =
			list_get(
				delete_database->
					delete_folder_list );

		ptr += sprintf(	ptr,
				"\nfolder = %s\n"
				"null_1tom_upon_delete? = %d\n"
				"primary_key_list = %s\n",
				delete_folder->folder_name,
				delete_folder->null_1tom_upon_delete,
				list_display(
					delete_folder->
						primary_key_list ) );

		if ( delete_folder->null_1tom_upon_delete )
		{
			ptr += sprintf( ptr,
					"null_1tom_upon_delete = %s\n",
					list_display(
					delete_folder->
					update_null_attribute_name_list ) );
		}

		if ( !list_rewind( delete_folder->delete_row_list ) ) continue;

		do {
			delete_row =
				list_get_pointer(
					delete_folder->delete_row_list );

			ptr += sprintf(	ptr,
					"\t%s\n",
					list_display(
						delete_row->
						primary_attribute_data_list ) );

		} while( list_next( delete_folder->delete_row_list ) );

	} while( list_next( delete_database->delete_folder_list ) );

	*ptr = '\0';

	return strdup( return_string );

}

void delete_database_append_null_1tom_delete_folder_list(
			LIST *delete_folder_list,
			char *application_name,
			char *one2m_related_folder_name,
			PROCESS *one2m_related_post_change_process,
			LIST *one2m_related_primary_key_list,
			LIST *foreign_attribute_name_list,
			LIST *primary_attribute_data_list,
			char *login_name )
{
	DELETE_FOLDER *delete_folder;

	delete_folder =
		delete_database_delete_folder_new(
			one2m_related_folder_name,
			1 /* null_1tom_upon_delete */,
			one2m_related_post_change_process,
			one2m_related_primary_key_list );

	delete_folder->update_null_attribute_name_list =
		delete_database_get_update_null_attribute_name_list(
			foreign_attribute_name_list,
			one2m_related_primary_key_list );

/*
fprintf( stderr, "%s/%s()/%d: for folder_name = (%s), got one2m_related_primary_key = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
one2m_related_folder_name,
list_display( one2m_related_primary_key_list ) );
*/

	delete_folder->delete_row_list =
		delete_database_get_delete_row_list(
			application_name,
			one2m_related_folder_name,
			0 /* row_level_non_owner_forbid */,
			0 /* row_level_non_owner_view_only */,
			one2m_related_primary_key_list
				/* select_attribute_name_list*/,
			foreign_attribute_name_list
				/* where_attribute_name_list */,
			primary_attribute_data_list
				/* where_attribute_data_list */,
			login_name );

	list_append_pointer( delete_folder_list, delete_folder );

}

LIST *delete_database_get_update_null_attribute_name_list(
			LIST *foreign_attribute_name_list,
			LIST *primary_key_list )
{
	LIST *update_null_attribute_name_list = list_new();
	char *foreign_attribute_name;

	if ( !list_rewind( foreign_attribute_name_list ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty foreign_attribute_name_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	do {
		foreign_attribute_name =
			list_get_pointer(
				foreign_attribute_name_list );

		if ( !list_exists_string(
			foreign_attribute_name,
			primary_key_list ) )
		{
			list_append_pointer(
				update_null_attribute_name_list,
				foreign_attribute_name );
		}

	} while( list_next( foreign_attribute_name_list ) );

	return update_null_attribute_name_list;

}

void delete_database_append_one2m_related_folder_list(
			LIST *delete_folder_list,
			LIST *one2m_related_folder_list,
			char *application_name,
			LIST *primary_attribute_data_list,
			char *login_name )
{
	RELATED_FOLDER *related_folder;

	if ( !list_rewind( one2m_related_folder_list ) ) return;

	do {
		related_folder = list_get( one2m_related_folder_list );

		/* ------------------------ */
		/* If null_1tom_upon_delete */
		/* ------------------------ */
		if ( !related_folder->is_primary_key_subset
		&&   list_length(
			related_folder->
			folder_foreign_attribute_name_list ) )
		{
			delete_database_append_null_1tom_delete_folder_list(
				delete_folder_list,
				application_name,
				related_folder->
					one2m_folder->
					folder_name,
				related_folder->
					one2m_folder->
					post_change_process,
				related_folder->
					one2m_folder->
					primary_key_list,
				related_folder->
					folder_foreign_attribute_name_list,
				primary_attribute_data_list,
				login_name );

			continue;
		}
		else
		if ( !related_folder->is_primary_key_subset )
		{
			delete_database_append_null_1tom_delete_folder_list(
				delete_folder_list,
				application_name,
				related_folder->
					one2m_folder->
					folder_name,
				related_folder->
					one2m_folder->
					post_change_process,
				related_folder->
					one2m_folder->
					primary_key_list,
				related_folder->
					foreign_attribute_name_list,
				primary_attribute_data_list,
				login_name );

			continue;
		}

		delete_database_append_one2m_delete_folder_list(
			delete_folder_list,
			application_name,
			related_folder->
				one2m_folder->
				folder_name,
			related_folder->
				foreign_attribute_name_list
				/* where_attribute_name_list */,
			primary_attribute_data_list
				/* where_attribute_data_list */,
			login_name,
			related_folder->
				one2m_folder->
				one2m_related_folder_list,
			related_folder->
				one2m_folder->
				primary_key_list,
			related_folder->
				one2m_folder->
				post_change_process );

	} while( list_next( one2m_related_folder_list ) );

}

void delete_database_refresh_row_count(
			char *application_name,
			LIST *delete_folder_list,
			char *session,
			char *role_name )
{
	DELETE_FOLDER *delete_folder;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;

	if ( !list_rewind( delete_folder_list ) ) return;

	appaserver_parameter_file = appaserver_parameter_file_new();

	do {
		delete_folder = list_get( delete_folder_list );

		folder_menu_refresh_row_count(
			application_name,
			delete_folder->folder_name,
			session,
			appaserver_parameter_file->
				appaserver_data_directory,
			role_name );

	} while( list_next( delete_folder_list ) );
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
}

DELETE *delete_calloc( void )
{
}

LIST *delete_sql_statement_list(
			DELETE_ROOT *delete_root,
			LIST *delete_one2m_list,
			LIST *delete_mto1_isa_list )
{
}

LIST *delete_command_line_list(
			DELETE_ROOT *delete_root,
			LIST *delete_one2m_list,
			LIST *delete_mto1_isa_list )
{
}

char *delete_sql_statement(
			char *folder_table_name,
			LIST *primary_key_list,
			LIST *primary_data_list,
			char *security_entity_where )
{
}

DELETE_ROOT *delete_root_new(
			char *application_name,
			char *folder_name,
			LIST *primary_key_list,
			LIST *primary_data_list,
			PROCESS *post_change_process,
			char *security_entity_where )
{
	DELETE_ROOT *delete_root = delete_root_calloc();

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
		return (LIST *)0;
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
		delete_one2m->sql_statement_list(
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
		delete_one2m->command_line_list =
			delete_one2m_command_line_list(
				relation_one2m->
					many_folder->
					post_change_process,
				login_name,
				relation_one2m->foreign_key_list,
				delete_one2m->primary_delimited_list );
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
			LIST *relation_mto1_isa_list,
			LIST *primary_data_list )
{
	return (LIST *)0;
}

DELETE_MTO1_ISA *delete_mto1_isa_new(
			char *application_name,
			RELATION *relation_mto1_isa,
			LIST *primary_data_list )
{
	DELETE_MTO1_ISA *delete_mto1_isa = delete_mto1_isa_calloc();

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
	char set_statement[ 1024 ];
	char *ptr = set_statement;

	if ( !list_rewind( foreign_key_list ) ) return (char *)0;

	do {
		if ( !list_at_first( foreign_key_list ) )
			ptr += sprintf( ptr, ", " );

		ptr += sprintf(
			ptr,
			"%s = null",
			(char *)list_get( foreign_key_list ) );

	} while ( list_next( foreign_key_list ) );

	return strdup( set_statement );
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
	char sql_statement[ 1024 ];

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
	char sql_statement[ 1024 ];

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

char *delete_command_line(
			char *command_line,
			char *login_name,
			LIST *primary_key_list,
			LIST *primary_data_list )
{
	char line[ 1024 ];
	char buffer[ 128 ];

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
			"delete" ) );

	return strdup( line );
}

LIST *delete_one2m_command_line_list(
			PROCESS *post_change_process,
			char *login_name,
			LIST *foreign_key_list,
			LIST *primary_delimited_list )
{
	LIST *command_line_list;

	if ( !post_change_process ) return (LIST *)0;

	if ( !post_change_process->command_line )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: command_line is empty.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	if ( !list_rewind( primary_delimited_list ) ) return (LIST *)0;

	command_line_list = list_new();

	do {
		list_set(
			command_line_list,
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			delete_command_line(
				post_change_process->command_line,
				login_name,
				foreign_key_list
					/* primary_key_list */,
				list_string_extract(
					(char *)
					list_get(
						primary_delimited_list ),
					SQL_DELIMITER ) ) );

	} while ( list_next( primary_delimited_list ) );

	return command_line_list;
}

LIST *delete_mto1_isa_one2m_list(
			char *application_name,
			char *folder_name,
			LIST *primary_data_list )
{
}

LIST *delete_mto1_isa_one2m_sql_statement_list(
			LIST *one2m_list )
{
}

LIST *delete_mto1_isa_one2m_command_line_list(
			LIST *one2m_list )
{
}

